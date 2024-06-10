/**
 * PX-Icon-Remover finds and deletes unused px-icons by searching files for "px-icons:"
 * and then removing extraneous <g> icons from the destination file
 *
 * Written by Alan Hayashi
 */

'use strict';
module.exports = function(grunt) {
	var prettyBytes = require('pretty-bytes');

	//"replacer" task which replaces matched text with other text
	grunt.registerMultiTask('px-icon-remover', 'Removes unused icons from px-icons', function() {
		var options = this.options();

		var usedIcons = [];
		// Read each of the source files and find all icons that start with "px-icons:"
		this.filesSrc.forEach(function(filepath) {
			if (!grunt.file.exists(filepath)) {
				return;
			}
			// Use regex to find all matching icons
			var ret = grunt.file.read(filepath);
			var matches = ret.match(/px-icons:[\w-]*/g);
			if (matches) {
				for (var n in matches) {
					var icon = matches[n];
					icon = icon.substr(icon.indexOf(":")+1); // Only text to the right of ":"
					if (usedIcons.indexOf(icon) === -1) {
						usedIcons.push(icon);
						grunt.verbose.writeln("Found icon " + icon);
					}
				}
			}
		});

		// Generate a regex that describes sections of <g></g> that don't have any of the listed icons
		var regstr = "(<g";
		for (var n in usedIcons) {
			var icon = usedIcons[n];
			regstr += "(?!.*?id=[\"']" + icon + "[\"'])";
		}
		regstr += "[\\w\\W]*?<\\/g>)";
		grunt.verbose.writeln(regstr);

		this.files.forEach(function(file) {
			var regex = new RegExp(regstr, "g");

			// Read the contents of the destination file
			var contents = grunt.file.read(file.dest);
			var oldLength = contents.length;
			var unusedIcons = 0;

			// Replace the contents of the file
			contents = contents.replace(regex, function(match) {
				unusedIcons++;
				var ids = match.match(/id=["'](.*?)["']/g);
				for (var n in ids) {
					grunt.verbose.writeln("Removing icon " + ids[n]);
				}
				return "";
			});
			var newLength = contents.length;

			// Write the new file
			grunt.file.write(file.dest, contents);

			// Print a success message.
			grunt.log.writeln('Removed ' + ((unusedIcons) + '').cyan + ' icons (' + prettyBytes(oldLength - newLength).green + ') from ' + file.dest);
		});
	});
};
