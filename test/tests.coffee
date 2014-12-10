assert = require 'assert'
path = require 'path'
temp = require 'temp'
async = require 'async'
_ = require '../build/Release/archive'

bufferEqual = (b1, b2) -> b1.length is b2.length and [b1[i] is b2[i] for i in [0..b1.length]]

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
		archivePath = temp.openSync('node-libarchive-tests').path
		w = new _.Writer archivePath
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

					entriesCopy = []
					_.read archivePath,
						(entry) -> entriesCopy.push entry
						(err) ->
							assert !err

							# both archives must have the same number of entries
							assert.equal entries.length, entriesCopy.length

							# put them into dictionaries
							d = {}
							entries.forEach (e) -> d[e.path] = e

							dCopy = {}
							entriesCopy.forEach (e) -> dCopy[e.path] = e

							# compare them
							Object.keys(d).forEach (k) ->
								assert.equal d[k].path, dCopy[k].path

								if d[k].data or dCopy[k].data
									assert bufferEqual d[k].data, dCopy[k].data

								assert.equal d[k].symlink, dCopy[k].symlink
								assert.deepEqual d[k].stat, dCopy[k].stat

							cb()
