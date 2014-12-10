assert = require 'assert'
path = require 'path'
temp = require 'temp'
async = require 'async'
_ = require '../build/Release/archive'

describe 'archive', ->
	it 'should read an existing archive', (cb) ->
		files = 0
		directories = 0
		symlinks = 0

		_.read path.join(__dirname, 'fixtures', '1.zip'),
			(entry) -> 
				switch entry.type
					when 'file' then files++
					when 'directory' then directories++
					when 'symlink' then symlinks++
			(err) ->
				assert !err
				assert.equal 3, files
				assert.equal 1, directories
				assert.equal 1, symlinks
				cb()

	it 'should provide the right properties', (cb) ->
		_.read path.join(__dirname, 'fixtures', '1.zip'),
			(entry) -> 
				switch entry.type
					when 'file'
						assert entry.path
						assert entry.type
						assert entry.data
						assert !entry.symlink
						assert entry.stat
						assert entry.stat.permissions
					when 'directory'
						assert entry.path
						assert entry.type
						assert !entry.data
						assert !entry.symlink
						assert entry.stat
						assert entry.stat.permissions
					when 'symlink'
						assert entry.path
						assert entry.type
						assert !entry.data
						assert entry.symlink
						assert entry.stat
						assert entry.stat.permissions
			cb

	it 'should fail when opening a non existing archive', (cb) ->
		_.read path.join(__dirname, 'fixtures', '-1.zip'),
			(entry) -> 
				assert false
			(err) ->
				assert err
				cb()

	it 'should be able to write an archive', (cb) ->
		w = new _.Writer temp.openSync('node-libarchive-tests').path
		async.series [
			w.writeDirectory.bind w, 'folder', { permissions: 493 }
			w.writeFile.bind w, 'folder/hello.txt', new Buffer('hello there\n', 'utf8'), { permissions: 436 }
			w.writeSymlink.bind w, 'link.txt', 'folder/hello.txt', { permissions: 436 }
			w.close.bind w
		], cb

	it 'should be able to copy an archive', (cb) ->
		p = temp.openSync('node-libarchive-tests').path
		w = new _.Writer p
		entries = []

		_.read path.join(__dirname, 'fixtures', '1.zip'),
			(entry) -> entries.push entry
			(err) ->
				assert !err
				ops = []

				entries.forEach (entry) ->
					switch entry.type
						when 'file'
							ops.push w.writeFile.bind w, entry.path, entry.data, entry.stat
						when 'directory'
							ops.push w.writeDirectory.bind w, entry.path, entry.stat
						when 'symlink'
							ops.push w.writeSymlink.bind w, entry.path, entry.symlink, entry.stat

				ops.push w.close.bind w

				async.series ops, (err) ->
					assert !err
					console.log p
					cb()
