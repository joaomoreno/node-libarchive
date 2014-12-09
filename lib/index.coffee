fs = require 'fs'
_ = require '../build/Release/archive'

exports.open = (name, cb) ->
	fs.realpath name, (err, path) ->
		return cb err if err
		_.open path, cb
