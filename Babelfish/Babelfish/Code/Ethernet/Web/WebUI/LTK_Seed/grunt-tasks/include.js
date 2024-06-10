/**
 * include is a custom-made task that adds all of the files in the "files" option to a particular source file.
 * The source file will be searched for <!-- include:KEY -->.*<!-- endinclude -->, where KEY is html, js, or css.
 * When found, the files in the "files" option are included as HTML links or scripts, as appropriate.
 *
 * Written by Alan Hayashi
 */

'use strict';
module.exports = function(grunt) {
	//Includes .js and .html files between the tags:
	//  <!-- include-scripts:[target] -->
	//  <!-- endinclude -->
	grunt.registerMultiTask('include', 'Includes files from a given directory', function() {
		var options = this.options();

		// The text that is added to the import section, depending on the file type
		var replace = {
			html: '<link rel="import" href="{{filePath}}" />',
			js: '<script src="{{filePath}}"></script>',
			css: '<link rel="stylesheet" href="{{filePath}}" />'
		};

		// Create a map of type array of include strings that should be added to the target file
		var includes = {};
		var size = 0;
		for (var n in options) {
			includes[n] = grunt.file.expand(this.data, options[n]).map(function(f) {
				f = f.replace(/^\.\//, '');
				for (var n in replace) {
					if (f.match("." + n + "$")) {
						return replace[n].replace('{{filePath}}', f);
					}
				}
			});
			size += includes[n].length;
		}

		// Identifies the block comment block where the files will be inserted
		// 	Also identifies 4 groups:
		// 		1. header, "		<!-- include:html -->"
		// 		2. space,  "		"
		// 		3. id,     "html"
		// 		4. footer, "<!-- endinclude -->"
		var block = /(([ \t]*)<!--\s*include:*(\S*)\s*-->)(?:\n|\r|.)*?(<!--\s*endinclude\s*-->)/gi;


		// Add array of strings to each of the target files
		this.files.forEach(function(file) {
			var cwd = file.cwd + '/';
			var f = file.src.filter(function(filepath) {
				// Remove nonexistent files
				if (!grunt.file.exists(cwd + filepath)) {
					grunt.log.warn('Source file "' + filepath + '" not found.');
					return false;
				} else {
					return true;
				}
			}).forEach(function(filepath) {
				// Read and return the file's source, replacing text as appropriate
				var ret = grunt.file.read(cwd + filepath);

				// Find the block and insert the includeStrings, with appropriate beginning spacings
				ret = ret.replace(block, function(match, header, space, id, footer) {
					grunt.log.writeln('Found '+id);
					space = '\n'+space;
					return header + space + (includes[id] || []).join(space) + space + footer;
				});

				// Write joined contents to destination filepath.
				var dest = file.dest || filepath;
				grunt.file.write(cwd + dest, ret);
				// Print a success message.
				grunt.log.writeln((''+size).green + ' scripts included into ' + dest.cyan);
			});

		});
	});
};
