assert = require 'assert'
path = require 'path'
_ = require '..'

describe 'archive', ->
	it 'should open an existing archive', (cb) ->
		_.open path.join(__dirname, 'fixtures', 'src-1.zip'), (err, archive) ->
			assert !err
			assert archive
			cb()
			
	it 'should err with a missing archive', (cb) ->
		_.open path.join(__dirname, 'fixtures', '__src-1.zip'), (err, archive) ->
			assert err
			assert !archive
			cb()
