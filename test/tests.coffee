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

	it 'should fail when opening a non existing archive', (cb) ->
		_.read path.join(__dirname, 'fixtures', '-1.zip'),
			(entry) -> 
				assert false
			(err) ->
				assert err
				cb()
