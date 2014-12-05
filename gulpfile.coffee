gulp = require 'gulp'
cp = require 'child_process'

spawn = (name, argv, cb) ->
	child = cp.spawn name, argv
	child.stdout.setEncoding 'utf8'
	child.stderr.setEncoding 'utf8'
	child.stdout.on 'data', process.stdout.write.bind process.stdout
	child.stderr.on 'data', process.stderr.write.bind process.stderr
	child.on 'exit', (code) -> if code is 0 then cb(null) else cb(new Error("'#{name} #{argv.join(' ')}' returned #{code}")) 

gulp.task 'build', (cb) ->
	spawn 'node-gyp', ['build'], cb

gulp.task 'test', ['build'], (cb) ->
	spawn 'mocha', ['--compilers', 'coffee:coffee-script/register'], cb

gulp.task 'default', ['build']