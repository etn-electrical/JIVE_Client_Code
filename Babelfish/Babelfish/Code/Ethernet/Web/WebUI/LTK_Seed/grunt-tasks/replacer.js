/**
 * Replacer replaces a regular expression found in a file with text.
 * This is done using String.replace(), so regular expressions declared with /'s are recommended, along with a /g flag to capture all instances.
 *
 * The options is expected to be a key-expression object of what to find.
 *
 * Here's an example:
 *
 * (grunt.initConfig)
 *     replacer: {
 *         src: "in.txt",
 *         dest: "out.txt",
 *         options: {
 *             "includes": /bower_components/g
 *             "remove": [/^.*unused.*$/g]
 *         }
 *     }
 *
 * (in.txt)
 *   <script src="../bower_components/px-blue/component.js" />
 *   <script src="../bower_components/px-blue/behavior.js" />
 *   <script src="../bower_components/unused/junk.js" />
 *
 * (out.txt)
 *   <script src="../includes/px-blue/component.js" />
 *   <script src="../includes/px-blue/behavior.js" />
 *
 * Written by Alan Hayashi
 */

'use strict';
module.exports = function(grunt) {
	//"replacer" task which replaces matched text with other text
	grunt.registerMultiTask('replacer', 'Replaces text that matches the pattern', function() {
		var options = this.options();
		var remove;
		if (typeof options.remove === 'object') {
			remove = options.remove;
			delete options.remove;
			grunt.verbose.writeln(remove, 'Remove Patterns');
		}

		this.files.forEach(function(file) {
			var oldLength;
			var newLength;
			var contents = file.src.filter(function(filepath) {
				// Remove nonexistent files
				if (!grunt.file.exists(filepath)) {
					grunt.log.warn('Source file "' + filepath + '" not found.');
					return false;
				} else {
					return true;
				}
			}).map(function(filepath) {
				// Read and return the file's source, replacing text as appropriate
				var ret = grunt.file.read(filepath);
				oldLength = ret.length;
				if (remove) {
					for (var i = 0; i < remove.length; i++) {
						ret = ret.replace(remove[i], function(match) {
							grunt.verbose.writeln("Removed "+match);
							return '';
						});
					}
				}
				for (var rep in options) {
					ret = ret.replace(options[rep], rep);
					//ret = ret.replace(patterns[rep].find, patterns[rep].replace);
				}
				newLength = ret.length;
				return ret;
			}).join('\n');

			// Write joined contents to destination filepath.
			grunt.file.write(file.dest, contents);
			// Print a success message.
			if (oldLength > newLength) {
				grunt.log.writeln('File ' + file.dest.cyan + ' created: ' + (oldLength - newLength + '').red + ' characters removed');
			} else {
				grunt.log.writeln('File ' + file.dest.cyan + ' created: ' + (newLength - oldLength + '').green + ' characters added');
			}
		});
	});
};
