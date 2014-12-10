var gulp = require('gulp');
var cp = require('child_process');

var spawn = function(name, argv, cb) {
  var child;
  child = cp.spawn(name, argv);
  child.stdout.setEncoding('utf8');
  child.stderr.setEncoding('utf8');
  child.stdout.on('data', process.stdout.write.bind(process.stdout));
  child.stderr.on('data', process.stderr.write.bind(process.stderr));
  
  return child.on('exit', function(code) {
    if (code === 0) {
      return cb(null);
    } else {
      return cb(new Error("'" + name + " " + (argv.join(' ')) + "' returned " + code));
    }
  });
};

gulp.task('build', function(cb) {
  return spawn('node-gyp', ['build'], cb);
});

gulp.task('test', ['build'], function(cb) {
  return spawn('mocha', [], cb);
});

gulp.task('default', ['build']);
