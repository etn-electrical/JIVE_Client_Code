// Generated on 2015-01-30 using generator-angular 0.10.0
'use strict';

// # Globbing
// for performance reasons we're only matching one level down:
// 'test/spec/{,*/}*.js'
// use this if you want to recursively match all subfolders:
// 'test/spec/**/*.js'

module.exports = function(grunt) {

	// Load grunt tasks automatically
	require('load-grunt-tasks')(grunt);

	// Time how long tasks take. Can help when optimizing build times
	require('time-grunt')(grunt);

	// Configurable paths for the application
	var appConfig = {
		app: require('./bower.json').appPath || 'app',
		dist: 'dist',
		gzip: 'prod-gzip'
	};

	grunt.loadTasks('./grunt-tasks');

	// Define the configuration for all the tasks
	grunt.initConfig({

		// Project settings
		yeoman: appConfig,

		// Watches files for changes and runs tasks based on the changed files
		watch: {
			bower: {
				files: ['bower.json'],
				tasks: ['wiredep']
			},
			//For some reason this causes a Recursive process that then crashes grunt serve...
			sass: {
				files: ['<%= yeoman.app %>/styles/**'],
				tasks: ['sass']
			},
			pxblue: {
				files: ['pxblue/{,*/*}*.*'],
				tasks: ['copy:pxblue', 'wiredep']
			},
			js: {
				files: ['<%= yeoman.app %>/scripts/{,*/}*.js', '<%= yeoman.app %>/test/scripts/{,*/}*.js'],
				options: {
					livereload: '<%= connect.options.livereload %>'
				}
			},
			jsTest: {
				files: ['test/spec/{,*/}*.js']
			},
			gruntfile: {
				files: ['Gruntfile.js']
			},
			// livereload: {
			// 	options: {
			// 		livereload: '<%= connect.options.livereload %>'
			// 	},
			// 	files: [
			// 		'<%= yeoman.app %>/elements/{,*/}*.html',
			// 		'<%= yeoman.app %>/test/{,*/}*.html',
			// 		'<%= yeoman.app %>/{,*/}*.html',
			// 		'<%= yeoman.app %>/styles/{,*/}*.css',
			// 		'pxblue/{,*/}*',
			// 		'<%= yeoman.app %>/images/{,*/}*.{png,jpg,jpeg,gif,webp,svg}'
			// 	]
			// },
			include: {
				files: ['<%= yeoman.app %>/elements/{,*/}*.html'],
				tasks: ['include:index']
			}
		},

		// The actual grunt server settings
		connect: {
			options: {
				port: 9001,
				open: false,
				// Change this to '0.0.0.0' to access the server from outside.
				//hostname is typically 'localhost' but if it isn't working, try your IP
				hostname: 'localhost',
				//hostname: '0.0.0.0',
				livereload: 35728
			},
			livereload: {
				options: {
					open: true,
					middleware: function(connect, options, middlewares) {
						var serveStatic = require('serve-static');
						return [
							serveStatic('.tmp'),
							connect().use(
								'/bower_components',
								serveStatic('./bower_components')
							),
							serveStatic(appConfig.app)
						];
					}
				}
			},
			test: {
				options: {
					port: 9001,
					middleware: function(connect) {
						var serveStatic = require('serve-static');
						return [
							serveStatic('.tmp'),
							serveStatic('test'),
							connect().use(
								'/bower_components',
								serveStatic('./bower_components')
							),
							serveStatic(appConfig.app)
						];
					}
				}
			},
			dist: {
				options: {
					open: false,
					base: '<%= yeoman.dist %>'
				}
			}
		},

		// Empties folders to start fresh
		clean: {
			dist: {
				files: [{
					dot: true,
					src: [
						'.tmp',
						'<%= yeoman.gzip %>/{,*/}*',
						'<%= yeoman.dist %>/{,*/}*',
						'!<%= yeoman.dist %>/.git{,*/}*'
					]
				}]
			},
			server: '.tmp',
			elements: '<%= yeoman.dist %>/elements.html'
		},

		// Add vendor prefixed styles
		autoprefixer: {
			options: {
				browsers: ['last 1 version']
			},
			dist: {
				files: [{
					expand: true,
					cwd: '.tmp/styles/',
					src: '{,*/}*.css',
					dest: '.tmp/styles/'
				}]
			}
		},

		// Automatically inject Bower components into the app
		wiredep: {
			app: {
				src: ['<%= yeoman.app %>/elements/elements.html', '<%= yeoman.app %>/index.html', '<%= yeoman.app %>/test/index.html'],
				exclude: ['bower_components/font-roboto/roboto.html'],
				ignorePath: /\.\.\//,
				fileTypes: {
					// defaults:
					html: {
						block: /(([ \t]*)<!--\s*bower:*(\S*)\s*-->)(\n|\r|.)*?(<!--\s*endbower\s*-->)/gi,
						detect: {
							html: /<link.*rel="import".*href=['"]([^'"]+)/gi,
							js: /<script.*src=['"]([^'"]+)/gi,
							css: /<link.*href=['"]([^'"]+)/gi
						},
						replace: {
							html: '<link rel="import" href="{{filePath}}" />',
							js: '<script src="{{filePath}}"></script>',
							css: '<link rel="stylesheet" href="{{filePath}}" />'
						}
					}
				}
			},
			sass: {
				src: ['<%= yeoman.app %>/styles/{,*/}*.{scss,sass}'],
				ignorePath: /(\.\.\/){1,2}bower_components\//
			}
		},

		// Renames files for browser caching purposes
		filerev: {
			dist: {
				src: [
					'<%= yeoman.dist %>/scripts/{,*/}*.js',
					'<%= yeoman.dist %>/styles/{,*/}*.css',
					'<%= yeoman.dist %>/images/{,*/}*.{png,jpg,jpeg,gif,webp,svg}'
				]
			}
		},

		//Compiles sass with grunt-contrib-sass
		sass: { // Task
			compile: {
				files: [{
					expand: true,
					cwd: '<%= yeoman.app %>/styles',
					src: ['*.scss'],
					dest: '<%= yeoman.app %>/styles',
					ext: '.css'
				}]
			}
		},

		// Reads HTML for usemin blocks to enable smart builds that automatically
		// concat, minify and revision files. Creates configurations in memory so
		// additional tasks can operate on them
		useminPrepare: {
			html: '<%= yeoman.dist %>/index.html',
			options: {
				dest: '<%= yeoman.dist %>',
				flow: {
					html: {
						steps: {
							js: ['concat', 'uglifyjs'],
							css: ['concat', 'cssmin']
						},
						post: {}
					}
				}
			}
		},

		// Performs rewrites based on filerev and the useminPrepare configuration
		usemin: {
			html: ['<%= yeoman.dist %>/{,*/}*.html'],
			css: ['<%= yeoman.dist %>/styles/{,*/}*.css', '<%= yeoman.dist %>/fonts/{,*/}*.css'],
			options: {
				assetsDirs: ['<%= yeoman.dist %>', '<%= yeoman.dist %>/images'],
			}
		},

		// The following *-min tasks will produce minified files in the dist folder
		// By default, your `index.html`'s <!-- Usemin block --> will take care of
		// minification. These next options are pre-configured if you do not wish
		// to use the Usemin blocks.
		// cssmin: {
		//   dist: {
		//     files: {
		//       '<%= yeoman.dist %>/styles/main.css': [
		//         '.tmp/styles/{,*/}*.css'
		//       ]
		//     }
		//   }
		// },
		uglify: {
			dcijs: {
				files: {
					'<%= yeoman.dist %>/dci.js': ['<%= yeoman.app %>/scripts/lib/dci.js']
				}
			}
		},
		// concat: {
		//   dist: {}
		// },

		imagemin: {
			dist: {
				files: [{
					expand: true,
					cwd: '<%= yeoman.app %>/images',
					src: '{,*/}*.{png,jpg,jpeg,gif}',
					dest: '<%= yeoman.dist %>/images'
				}]
			}
		},

		svgmin: {
			dist: {
				files: [{
					expand: true,
					cwd: '<%= yeoman.app %>/images',
					src: '{,*/}*.svg',
					dest: '<%= yeoman.dist %>/images'
				}]
			}
		},

		htmlmin: {
			dist: {
				options: {
					collapseWhitespace: true,
					conservativeCollapse: true,
					collapseBooleanAttributes: true,
					removeCommentsFromCDATA: true,
					keepClosingSlash: true,
					caseSensitive: true,
					minifyJS: true,
					minifyCSS: true
				},
				files: [{
					expand: true,
					cwd: '<%= yeoman.dist %>',
					src: ['*.html'],
					dest: '<%= yeoman.dist %>'
				}]
			}
		},

		// Replace Google CDN references
		cdnify: {
			dist: {
				html: ['<%= yeoman.dist %>/*.html']
			}
		},

		// Copies remaining files to places other tasks can use
		copy: {
			test: {
				files: [{
					expand: true,
					dot: true,
					cwd: 'app/scripts',
					dest: '.tmp/scripts',
					src: ['{,*/}*.*']
				}]
			},
			dist: {
				files: [{
					expand: true,
					dot: true,
					cwd: '<%= yeoman.app %>',
					dest: '<%= yeoman.dist %>',
					src: [
						'*.{ico,png,txt}',
						'.htaccess',
						'*.html',
						'images/{,*/}*.{webp}',
						'!index.html'
					]
				}, {
					expand: true,
					cwd: '.tmp/images',
					dest: '<%= yeoman.dist %>/images',
					src: ['generated/*']
				}]
			},

			fonts: {
				files: [{
					expand: true,
					cwd: 'bower_components/font-opensans/',
					dest: '<%= yeoman.dist %>/styles/',
					src: '*.woff'
				}]
			},

			styles: {
				files: [{
					expand: true,
					cwd: '<%= yeoman.app %>/styles',
					dest: '.tmp/styles/',
					src: '{,*/}*.css'
				}]
			},
			elements: {
				expand: true,
				cwd: '<%= yeoman.app %>/elements',
				dest: '<%= yeoman.dist %>/elements',
				src: 'elements.html'
			}
		},

		// Test settings
		karma: {
			unit: {
				configFile: 'test/karma.conf.js',
				singleRun: true
			}
		},

		vulcanize: {
			index: {
				options: {
					inlineScripts: true, // Inline external scripts.
					inlineCss: true, // Inline external stylesheets.
					//redirects: ['bower_components|../bower_components'],        // An array of strings with the format URI|PATH where url is a URI composed of a protocol, hostname, and path and PATH is a local filesystem path to replace the matched URI part with. Multiple redirects may be specified; the earliest ones have the highest priority.
					stripComments: false, // Remove non-license HTML comments.
					excludes: ['/dci.js']
				},
				src: '<%= yeoman.dist %>/index.html',
				dest: '<%= yeoman.dist %>/index.html'
			},
			elements: {
				options: {
					inlineScripts: true, // Inline external scripts.
					inlineCss: true, // Inline external stylesheets.
					//redirects: ['bower_components|../bower_components'],        // An array of strings with the format URI|PATH where url is a URI composed of a protocol, hostname, and path and PATH is a local filesystem path to replace the matched URI part with. Multiple redirects may be specified; the earliest ones have the highest priority.
					stripComments: false, // Remove non-license HTML comments.
				},
				src: '<%= yeoman.dist %>/elements/elements.html',
				dest: '<%= yeoman.dist %>/elements/elements.html'
			}
		},

		replacer: {
			// Adds an additional ../ to all the bower_components imports in index
			bower_components: {
				src: '<%= yeoman.app %>/index.html',
				dest: '<%= yeoman.dist %>/index.html',
				options: {
					'../bower_components': /bower_components/g,
					'"dci.js': /\".*scripts\/lib\/dci.js/,
					'"../app/styles/': /\"styles\//g,
					'"../app/elements/': /\"elements\//g,
					'"../app/scripts/': /\"scripts\//g
				}
			},

			// Cleans up the output of elements.html so that images and such point to the right places (this is a bit of a hack!)
			post_vulcanize: {
				src: '<%= yeoman.dist %>/index.html',
				dest: '<%= yeoman.dist %>/index.html',
				options: {
					//'': / assetpath=\".*\"/g,
					'url("../images/': /url\(".*\/images\//g,
					//'<path fill="currentColor" ': /\<path /g,				// Only needed for ShadowDom
					//'<polygon fill="currentColor" ': /\<polygon /g,	// Only needed for ShadowDom
					'url(\'styles/opensans': /url\('.*opensans/g,
					'url(\"styles/opensans': /url\(".*opensans/g
				}
			}
		},


		minimize: {
			elements: {
				files: [{
					expand: true,
					cwd: '<%= yeoman.dist %>/elements',
					src: ['./**/*.html', '!./elements.html'],
					dest: '<%= yeoman.dist %>/elements',
				}]
			},
			index: {
				files: [{
					expand: true,
					cwd: '<%= yeoman.dist %>',
					src: ['*.html'],
					dest: '<%= yeoman.dist %>'
				}]
			}

		},

		compress: {
			gzip: {
				options: {
					mode: 'gzip'
				},
				expand: true,
				cwd: '<%= yeoman.dist %>',
				src: ['**/*'],
				dest: '<%= yeoman.gzip %>'
			}
		},

		//Custom function that includes the components files
		include: {
			index: {
				src: 'index.html',
				dest: 'index.html',
				cwd: '<%= yeoman.app %>',
				options: {
					elements: ['./elements/**/*.html']
				}
			}
		},

		sizetest: {
			dist: {
				src: ["<%= yeoman.dist %>", "<%= yeoman.gzip %>"]
			}
		},

		"px-icon-remover": {
			dist: {
				src: ["<%= yeoman.dist %>/**/*.*"],
				dest: "<%= yeoman.dist %>/index.html",
			}
		}

	});

	grunt.registerTask('serve', 'Compile then start a connect web server', function(target) {
		if (target === 'dist' || target === 'debug') {
			return grunt.task.run(['build:' + target, 'connect:dist:keepalive']);
		}

		grunt.task.run([
			'clean:server',
			'sass',
			'wiredep',
			'include:index', //Injects elements into index.html
			'autoprefixer',
			'connect:livereload',
			'watch'
		]);
	});

	grunt.registerTask('server', 'DEPRECATED TASK. Use the "serve" task instead', function(target) {
		grunt.log.warn('The `server` task has been deprecated. Use `grunt serve` to start a server.');
		grunt.task.run(['serve:' + target]);
	});

	grunt.registerTask('test', [
		'clean:server',
		'autoprefixer',
		'connect:test',
		'karma'
	]);

	//Task to test grunt
	// grunt.registerTask('v', [
	//   'clean:dist',
	//   'copy:pxblue',
	//   'sass',
	//   'wiredep'
	// ]);

	grunt.registerTask('build', function(target) {
		var tasks = [
			'clean:dist',
			'sass',
			'wiredep',
			'include:index', //Injects elements into index.html
			'autoprefixer',
			'copy:dist',
			'copy:fonts',
			'uglify:dcijs',
			'imagemin',
			'svgmin',
			'cdnify',
			'replacer:bower_components',
			'vulcanize:index', //Combines all files into one big index.html
			'px-icon-remover',
			'replacer:post_vulcanize',
			'minimize:index',
			'compress:gzip',
			'sizetest:dist'
		];

		if (target === 'debug') {
			var notask = ['minimize'];
			tasks = tasks.filter(function(x) {
				for (var n in notask) {
					if (x.indexOf(notask[n]) === 0) {
						return false;
					}
				}
				return true;
			});
			grunt.task.run(tasks);
		} else {
			grunt.task.run(tasks);
		}
	});



	grunt.registerTask('default', [
		'test',
		'build'
	]);

	//grunt.loadNpmTasks('grunt-dgeni');
	//grunt.registerTask('docs', ['dgeni']);
};
