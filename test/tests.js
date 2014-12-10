var assert = require('assert');
var path = require('path');
var temp = require('temp');
var async = require('async');
var _ = require('..');

var bufferEqual = function(b1, b2) {
	if (b1.length !== b2.length) {
		return false;
	}

	for (var i = 0; i < b1.length; i++) {
		if (b1[i] !== b2[i]) {
			return false;
		}

		return true;
	}
};

describe('archive', function() {
	it('should read an existing archive', function(cb) {
		var directories, files, symlinks;
		files = 0;
		directories = 0;
		symlinks = 0;
		return _.read(path.join(__dirname, 'fixtures', '1.zip'), function(entry) {
			switch (entry.type) {
				case 'file':
					return files++;
				case 'directory':
					return directories++;
				case 'symlink':
					return symlinks++;
			}
		}, function(err) {
			assert(!err);
			assert.equal(3, files);
			assert.equal(1, directories);
			assert.equal(1, symlinks);
			return cb();
		});
	});
	
	it('should provide the right properties', function(cb) {
		return _.read(path.join(__dirname, 'fixtures', '1.zip'), function(entry) {
			switch (entry.type) {
				case 'file':
					assert(entry.path);
					assert(entry.type);
					assert(entry.data);
					assert(!entry.symlink);
					assert(entry.stat);
					return assert(entry.stat.permissions);
				case 'directory':
					assert(entry.path);
					assert(entry.type);
					assert(!entry.data);
					assert(!entry.symlink);
					assert(entry.stat);
					return assert(entry.stat.permissions);
				case 'symlink':
					assert(entry.path);
					assert(entry.type);
					assert(!entry.data);
					assert(entry.symlink);
					assert(entry.stat);
					return assert(entry.stat.permissions);
			}
		}, cb);
	});

	it('should fail when opening a non existing archive', function(cb) {
		return _.read(path.join(__dirname, 'fixtures', '-1.zip'), function(entry) {
			return assert(false);
		}, function(err) {
			assert(err);
			return cb();
		});
	});

	it('should be able to write an archive', function(cb) {
		var w;
		w = new _.Writer(temp.openSync('node-libarchive-tests').path);
		return async.series([
			w.writeDirectory.bind(w, 'folder', {
				permissions: 493
			}), w.writeFile.bind(w, 'folder/hello.txt', new Buffer('hello there\n', 'utf8'), {
				permissions: 436
			}), w.writeSymlink.bind(w, 'link.txt', 'folder/hello.txt', {
				permissions: 436
			}), w.close.bind(w)
		], cb);
	});

	it('should be able to copy an archive', function(cb) {
		var archivePath, entries, w;
		archivePath = temp.openSync('node-libarchive-tests').path;
		w = new _.Writer(archivePath);
		entries = [];
		return _.read(path.join(__dirname, 'fixtures', '1.zip'), function(entry) {
			return entries.push(entry);
		}, function(err) {
			var ops;
			assert(!err);
			ops = [];
			entries.forEach(function(entry) {
				switch (entry.type) {
					case 'file':
						return ops.push(w.writeFile.bind(w, entry.path, entry.data, entry.stat));
					case 'directory':
						return ops.push(w.writeDirectory.bind(w, entry.path, entry.stat));
					case 'symlink':
						return ops.push(w.writeSymlink.bind(w, entry.path, entry.symlink, entry.stat));
				}
			});
			ops.push(w.close.bind(w));
			return async.series(ops, function(err) {
				var entriesCopy;
				assert(!err);
				entriesCopy = [];
				return _.read(archivePath, function(entry) {
					return entriesCopy.push(entry);
				}, function(err) {
					var d, dCopy;
					assert(!err);
					assert.equal(entries.length, entriesCopy.length);
					d = {};
					entries.forEach(function(e) {
						return d[e.path] = e;
					});
					dCopy = {};
					entriesCopy.forEach(function(e) {
						return dCopy[e.path] = e;
					});
					Object.keys(d).forEach(function(k) {
						assert.equal(d[k].path, dCopy[k].path);
						if (d[k].data || dCopy[k].data) {
							assert(bufferEqual(d[k].data, dCopy[k].data));
						}
						assert.equal(d[k].symlink, dCopy[k].symlink);
						return assert.deepEqual(d[k].stat, dCopy[k].stat);
					});
					return cb();
				});
			});
		});
	});
});
