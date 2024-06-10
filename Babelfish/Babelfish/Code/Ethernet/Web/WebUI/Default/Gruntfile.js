'use strict';

module.exports = function(grunt) {

	// Load grunt tasks automatically
	require('load-grunt-tasks')(grunt);
	grunt.loadTasks('./grunt-tasks');


	// Configurable paths for the application
	var appConfig = {
		app: 'Default_UI',
		dist: 'dist',
		gzip: 'gzip'
	};

	// Define the configuration for all the tasks
	grunt.initConfig({
		yeoman: appConfig,

		// Empties folders to start fresh
		clean: {
			dist: {
				files: [{
					dot: true,
					src: [
						'<%= yeoman.dist %>/{,*/}*',
						'!<%= yeoman.dist %>/.git{,*/}*'
					]
				}]
			},
			gzip: {
				files: [{
					dot: true,
					src: [
						'<%= yeoman.gzip %>/{,*/}*',
						'!<%= yeoman.gzip %>/.git{,*/}*'
					]
				}]
			}
		},

		// Copies remaining files to places other tasks can use
		copy: {
			dist: {
				files: [{
					expand: true,
					dot: true,
					cwd: '<%= yeoman.app %>',
					dest: '<%= yeoman.dist %>',
					src: './**/*.*'
				}]
			},
			gzip: {
				files: [{
					expand: true,
					dot: true,
					cwd: '<%= yeoman.app %>',
					dest: '<%= yeoman.gzip %>',
					src: ['./**/*.*', '!./**/index*.html']
				}]
			}
		},

		minimize: {
			dist: {
				files: [{
					expand: true,
					cwd: '<%= yeoman.dist %>',
					src: './**/*.html',
					dest: '<%= yeoman.dist %>',
				}]
			}
		},

		compress: {
			gzip: {
				options: {
					mode: 'gzip',
					level: 9,
					pretty: 'true'
				},
				expand: true,
				cwd: '<%= yeoman.dist %>',
				src: './**/index*.html',
				dest: '<%= yeoman.gzip %>'
			}
		}
	});

	grunt.registerTask('build', [
		'clean',
		'copy',
		'minimize',
		'compress'
	]);

	grunt.registerTask('default', ['build']);

};
