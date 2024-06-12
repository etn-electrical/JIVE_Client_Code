/**
 * sizetest is a custom-made task that outputs the size of a folder. Helpful for determining compressed sizes
 *
 * Written by Alan Hayashi
 */

'use strict';
module.exports = function(grunt) {
	var fs = require('fs');
	var prettyBytes = require('pretty-bytes');

	grunt.registerMultiTask('sizetest', 'Outputs the total size of a given file or directory', function() {

		var options = this.options({
			filter: /.*/
		});
		var filter = options.filter;

		var results = [];
		// Add array of strings to each of the target files
		this.filesSrc.forEach(function(filepath) {
			var size = 0;
			var callback = function (abspath, rootdir, subdir, filename) {
				if (grunt.file.isDir(abspath)) {
					grunt.verbose.write('Source "' + abspath + '" is a folder.');
					grunt.file.recurse(abspath, callback);
				} else if (grunt.file.isFile(abspath)) {
					grunt.verbose.write('Source "' + abspath + '" is a file.');
					if (filepath.match(filter)) {
						var stat = fs.statSync(abspath);
						size += stat.size;
					}
				} else {
					grunt.log.warn('Source "' + abspath + '" not found.');
				}
			};

			callback("./" + filepath);
			results.push({filepath: filepath, size: size});
		});

		results.sort(function(a, b) {
			return b.size - a.size;
		});

		for (var i = 0; i < results.length; i++) {
			var filepath = results[i].filepath;
			var size = results[i].size;
			grunt.log.writeln(filepath + ' = ' + prettyBytes(size) + " ("+ size +" bytes)");
		}
	});
};
