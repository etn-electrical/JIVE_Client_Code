/*
 * grunt-vulcanize
 * https://github.com/Polymer/grunt-vulcanize
 *
 * Copyright (c) 2013 The Polymer Authors
 * Licensed under the BSD license.
 *
 * ----
 *
 * Customizations have been made to be compatible with modern versions of Vulcanize.
 * 
 * Altered by Alan Hayashi
 * 
 */

'use strict';

module.exports = function(grunt) {

	var Vulcanize = require('vulcanize');
	var prettyBytes = require('pretty-bytes');
	// Please see the Grunt documentation for more information regarding task
	// creation: http://gruntjs.com/creating-tasks

	grunt.registerMultiTask('vulcanize', 'Inline HTML Imports', function() {
		var done = this.async();
		// Merge task-specific and/or target-specific options with these defaults.
		var options = this.options({
			abspath: null, // A folder to treat as "webroot".
			// When specified, use an absolute path to target.

			// excludes: [],       // An array of RegExp objects to exclude paths from being inlined.

			// stripExcludes: [],  // Similar to excludes, but strips the imports from the output entirely.
			// If stripExcludes is empty, it will be set the value of excludes by default.

			inlineScripts: false, // Inline external scripts.
			inlineCss: false, // Inline external stylesheets.
			// addedImports: [],     // Additional HTML imports to inline, added to the end of the target file
			// redirects: [],        // An array of strings with the format URI|PATH where url is a URI composed of a protocol, hostname, and path and PATH is a local filesystem path to replace the matched URI part with. Multiple redirects may be specified; the earliest ones have the highest priority.
			stripComments: false, // Remove non-license HTML comments.

			//inputUrl: '',       // A URL string that will override the target argument to vulcanize.process(). By design, gulp and grunt plugins expect to work on the given file path. vulcanize has its own file loader, and expects to be given URLs. In instances where the filename cannot be used as a URL inputUrl will override the filename.

			//loader: null, A hydrolysis loader. This loader is generated with the target argument to vulcan.process and the exclude paths. A custom loader can be given if more advanced setups are necesssary.
		});

		var filesCount = this.files ? this.files.length : 0;

		if (filesCount <= 0) {
			done();
			return;
		}

		// Iterate over all specified file groups.
		this.files.forEach(function(f) {
			// Concat specified files.
			var src = f.src.filter(function(filepath) {
				// Warn on and remove invalid source files (if nonull was set).
				if (!grunt.file.exists(filepath)) {
					grunt.log.warn('Source file "' + filepath + '" not found.');
					return false;
				} else {
					return true;
				}
			});

			var vulcan = new Vulcanize(options);
			vulcan.process(src[0], function(err, inlineHtml) {
				if (err) {
					return grunt.fatal(err);
				}

				// Write joined contents to destination filepath.
				grunt.file.write(f.dest, inlineHtml);
				// Print a success message.
				grunt.log.writeln('Wrote ' + prettyBytes(inlineHtml.length).green + ' bytes to ' + f.dest.cyan);

				filesCount--;

				if (filesCount <= 0) {
					done();
				}
			});

		});
	});

};
