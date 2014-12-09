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
						assert entry.permissions
						assert entry.type
						assert entry.data
						assert !entry.symlink
					when 'directory'
						assert entry.path
						assert entry.permissions
						assert entry.type
						assert !entry.data
						assert !entry.symlink
					when 'symlink'
						assert entry.path
						assert entry.permissions
						assert entry.type
						assert !entry.data
						assert entry.symlink
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
			w.writeDirectory.bind w, 'folder', 493
			w.writeFile.bind w, 'folder/hello.txt', 436, new Buffer('hello there\n', 'utf8')
			w.writeSymlink.bind w, 'link.txt', 436, 'folder/hello.txt'
			w.close.bind w
		], cb
