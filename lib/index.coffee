fs = require 'fs'
_ = require '../build/Release/hello'

exports.open = (name, cb) ->
	fs.realpath name, (err, path) ->
		return cb err if err
		_.open path, cb
