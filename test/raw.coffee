assert = require 'assert'
path = require 'path'
_ = require '../build/Release/archive'

describe 'archive', ->
	it 'should open an existing archive', (cb) ->
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
			
	# it 'should err with a missing archive', (cb) ->
	# 	_.open path.join(__dirname, 'fixtures', '__src-1.zip'), (err, archive) ->
	# 		assert err
	# 		assert !archive
	# 		cb()
			
	# it 'should err with a missing archive', (cb) ->
	# 	_.open path.join(__dirname, 'fixtures', 'src-1.zip'), (err, archive) ->
	# 		count = 0
			
	# 		archive.read (-> count++), (err) ->
	# 			assert !err
	# 			assert.equal 3, count
