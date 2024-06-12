/**
 * minimize is a custom-made task that reduces the size of HTML files
 * This is done by running NPM's minimize and identifying <script> and <style> tags and running uglify-js and clean-css as appropriate
 *
 * minimize is intended to be used on the output of vulcanize.
 *
 * Written by Alan Hayashi
 */

'use strict';
module.exports = function(grunt) {
	var equals = function(o1, o2) {
		var keys1 = Object.keys(o1);
		var keys2 = Object.keys(o2);
		if (keys1.length !== keys2.length) {
			return false;
		}

		for (var i = keys1.length - 1; i >= 0; i--) {
			if (o1[keys1[i]] !== o2[keys1[i]]) {
				return false;
			}
		}

		return true;
	}

	//Customize Elements.html for build
	grunt.registerMultiTask('minimize', 'Runs minimize on an HTML file', function() {

		var Minimize = require('minimize');
		var UglifyJS = require('uglify-js');
		var CleanCSS = require('clean-css');
		var prettyBytes = require('pretty-bytes');
		var done = this.async();
		var options = this.options({
			empty: true, // KEEP empty attributes
			cdata: false, // DROP CDATA from scripts
			comments: false, // DROP comments
			ssi: false, // DROP Server Side Includes
			conditionals: true, // KEEP conditional internet explorer comments
			spare: true, // KEEP redundant attributes
			quotes: true, // KEEP arbitrary quotes
			loose: true, // KEEP one whitespace
			plugins: [{
				id: 'uglifyjs',
				element: function(node, next) {
					if (helpers.isJS(node) && node.children.length === 1) {
						var content = node.children[0].data;
						var result = UglifyJS.minify(content, {
							fromString: true,
							warnings: true
						});
						node.children[0].data = result.code;
						grunt.verbose.writeln('Uglified ' + prettyBytes(content.length) + ' → ' + prettyBytes(result.code.length));
					}
					next();
				}
			}, {
				id: 'cleancss',
				element: function(node, doNext) {
					//Translate any style tags
					if (helpers.isStyle(node) && node.children.length === 1) {
						var content;
						try {
							content = node.children[0].data;

							//Special: Combine adjacent STYLE tags that have matching attributes
							var next = node.next;
							//  Adjacent tags are separated by text tags, which should be made blank
							while (next !== undefined && next !== null && next.type === 'text') {
								if (next.data !== undefined && '' === next.data.replace(/\s+/g, '')) {
									next.data = '';
								}
								next = next.next;
							}

							if (next && helpers.isStyle(next) && next.children.length === 1 && equals(node.attribs, next.attribs)) {
								//Adjacent styles are combined by putting this node's data into the next one's
								next.children[0].data = content + "\n\n" + next.children[0].data

								//This node is transmuted into a blank text node to delete it
								node.children[0].data = '';
								node.type = 'text';
								node.data = '';
							} else {
								//If the CSS can't be moved, just minify it where it is.
								//
								//Method 1: Use CleanCSS
								//  This doesn't work because it crunches Polymer somehow...
								var minified = new CleanCSS().minify(content);
								grunt.verbose.writeln('Cl '+minified.length);
								node.children[0].data = minified.styles;
								grunt.verbose.writeln('CleanCSS '+prettyBytes(content.length)+' → ' + prettyBytes(minified.styles.length));

								//Method 2: Use regex and replace whitespaces only
								node.children[0].data = content
									.replace(/\s{2,}/g, ' ') //Any two-white spaces is replaced with a single white space
									.replace(/\s*\/\*[\s\S]*?\*\/\s*/g, '') //Remove any comments and any surrounding white space
									.replace(/\s*\,\s*/g, ',') //All whitespace around a comma
									.replace(/\s*\{\s*/g, '{') //All whitespace around an open bracket
									.replace(/\s*\}\s*/g, '}') //All whitespace around a closed bracket
									.replace(/\s*\;\s*/g, ';'); //All whitespace around a semicolon
							}
							if (!this.used) {
								this.used = true;
								//grunt.log.writeln('',node);
								//grunt.log.writeln(minified.styles);
							}
						} catch (e) {
							//Occurs when something failed to parse
							//  Try CleanCSS chunk by chunk to provide a better guess at what went wrong
							try {
								var regex = /(.+\{[^\}]+\})/g
								var match = regex.exec(content);
								while (match != null) {
									var text = match[1];
									grunt.verbose.writeln(text);
									new CleanCSS().minify(text);
									match = regex.exec(content);
								}
							} catch (ex) {
								grunt.log.error('Failed to parse CSS:\r\n', text);
								throw e;
							}

							grunt.log.error('Something wrong with ', node);
							throw e;
						}
					}
					doNext();
				}
			}]

		});

		//Identified issue in Minimize:
		//  https://github.com/Swaagie/minimize/issues/70
		//
		// Work-around by manually writing blank attributes so <div layout> remains <div layout>

		var Helpers = require('minimize/lib/helpers');
		var helpers = new Helpers(options);
		helpers.attributes = function attributes(element) {
			var attr = element.attribs,
				self = this;

			if (!attr || typeof attr !== 'object') return '';
			return Object.keys(attr).reduce(function(result, key) {
				var value = attr[key];
				if (!value.length) {
					return result + ' ' + key;
				} else {
					return result + ' ' + key + '=' + self.quote(!self.config.whitespace ? value.replace(/\s+/g, ' ').trim() : value);
				}
			}, '');
		};


		var tries = 1;
		var dones = 0;

		this.files.forEach(function(file) {
			var minimize = new Minimize(options);
			minimize.helpers = helpers;
			var contents = file.src.filter(function(filepath) {
					// Remove nonexistent files
					if (!grunt.file.exists(filepath)) {
						grunt.log.warn('Source file "' + filepath + '" not found.');
						return false;
					} else {
						return true;
					}
				})
				.map(grunt.file.read)
				.join('\n');

			tries++;

			grunt.verbose.writeln('Read ' + prettyBytes(contents.length));
			minimize.parse(contents, function(error, data) {
				if (error) {
					grunt.log.writeln(error);
				}
				// Write joined contents to destination filepath.
				grunt.file.write(file.dest, data);
				// Print a success message.
				grunt.log.writeln('Minified ' + file.dest.cyan + ' ' + prettyBytes(contents.length) + ' → ' + prettyBytes(data.length));
				if (++dones === tries) {
					done();
				}
			});

		});

		if (++dones === tries) {
			done();
		}
	});
};
