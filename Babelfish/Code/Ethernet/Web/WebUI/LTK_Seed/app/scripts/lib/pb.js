(function() {
	"use strict";

	// /////////
	// MiniQ //
	// /////////
	var STATE_PENDING = -1;
	var STATE_RESOLVED = 0;
	var STATE_REJECTED = 1;
	// Tiny Promise implementation
	var MiniQ = function MiniQ() {
		this.q = [];
		this.state = STATE_PENDING;
	};

	// Use defineProperties to prevent creation of functions each time
	// 	(MiniQ is used a lot!)
	Object.defineProperties(MiniQ.prototype, {
		getMiniQ: {
			value: function() {
				if (this.value && this.value[0] instanceof MiniQ) {
					return this.value[0].getMiniQ();
				}
				return this;
			}
		},

		doQueue: {
			value: function() {
				var q = this.q;
				var value = this.value;
				while (q.length) {
					var func = q.shift()[this.state];
					var newValue;
					if (func) {
						newValue = func.apply(this, value);
						if (newValue !== undefined) {
							newValue = [newValue];
							for (var i = 1; i < value.length; i++) {
								newValue[i] = value[i];
							}
							this.value = value = newValue;
							if (value[0] instanceof MiniQ) {
								while (q.length) {
									value[0].q.push(q.shift());
								}
							}
						}
					}
				}

				if (value && value[0] instanceof MiniQ) {
					if (!value[0].isPending()) {
						value[0].doQueue();
					}
				}
				this.value = value;
			}
		},

		// When this process finishes, run the resolution or reject callback functions
		//  These functions will run with ALL of the arguments sent by the resolve/reject function.
		//  Providing a return value for the callback will replace the first argument in all future "then" callbacks
		then: {
			value: function then(resCallback, rejCallback) {
				var mQ = this.getMiniQ();
				if (mQ !== this) {
					mQ.then(resCallback, rejCallback);
					return mQ;
				} else {
					mQ.q.push([resCallback, rejCallback]);
					if (!mQ.isPending()) {
						mQ.doQueue();
					}
					return mQ;
				}
			}
		},

		// Resolve this process and send the data down its chain
		resolve: {
			value: function resolve() {
				this.state = STATE_RESOLVED;
				this.value = arguments;
				this.doQueue();
				return this;
			}
		},

		// Reject this process and send the data down its chain
		reject: {
			value: function reject() {
				this.state = STATE_REJECTED;
				this.value = arguments;
				this.doQueue();
				return this;
			}
		},

		// Convenience function for .then(callback, callback)
		always: {
			value: function always(callback) {
				return this.then(callback, callback);
			}
		},

		isPending: {
			value: function isPending() {
				return this.getMiniQ().state === STATE_PENDING;
			}
		},
		isResolved: {
			value: function isResolved() {
				return this.getMiniQ().state === STATE_RESOLVED;
			}
		},
		isRejected: {
			value: function isRejected() {
				return this.getMiniQ().state === STATE_REJECTED;
			}
		}

	});

	var createAuthHeader = function(method, path) {
		var header;
		
		if (PB.checkIfLocalStorageExists()) {
			var obj = PB.retrieveFromLocalStorage();
			var auth = {
				nonce: obj.nonce,
				opaque: obj.opaque,
				qop: obj.qop,
				realm: obj.realm,
				type: obj.type,
				algorithm: obj.algorithm
			}
			PB.cred.digest = auth;
			if (auth.type === "TK_Digest") {
				PB.cred.type = "DIGEST";
			} else if (auth.type === "TK_Basic") {
				PB.cred.type = "BASIC"
			}
		} else if (!PB.loginModalOpen) {
			window.location.href = "/";
		}
		
		if (PB.cred.username && PB.cred.password && PB.cred.type === "DIGEST" && PB.cred.digest) {
			header = createDigestHeader(method, path);
		}
		else if (PB.cred.type === "BASIC") {
			header = "TK_Basic " + btoa(PB.cred.username + ":" + PB.cred.password);
		}
		else {
			header = "Auth_TK";
		}
		return header;
	}

	// ///////////////
	// Digest Auth //
	// ///////////////
	// Everything needed to create a Digest Authentication header
	var createDigestHeader = (function() {
		var _chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		var _nc = 0;

		var generateNonce = function(length) {
			var nonce = "";
			var charsLength = _chars.length;

			for (var i = 0; i < length; ++i) {
				nonce += _chars[Math.floor(Math.random() * charsLength)];
			}

			return nonce;
		};

		var getNc = function() {
			_nc++;
			var zeros = 8 - _nc.toString().length;
			var nc = "";
			for (var i = 0; i < zeros; i++) {
				nc += "0";
			}
			return (nc + _nc);
		};

		var generateResponse = function(method, path, nc, cnonce) {
			var ha1 = md5(PB.cred.username + ":" + PB.cred.digest.realm + ":" + PB.cred.password);
			var ha2 = md5(method + ":" + path);
			return md5(ha1 + ":" + PB.cred.digest.nonce + ":" + nc + ":" + cnonce + ":" + PB.cred.digest.qop + ":" + ha2);
		};

		return function(method, path) {
			if (!PB.cred.digest || !PB.cred.username) {
				return;
			}
			var nc = getNc();
			var cnonce = generateNonce(16);
			return PB.cred.digest.type + " " +
				"username=\"" + PB.cred.username + "\", " +
				"realm=\"" + PB.cred.digest.realm + "\", " +
				"nonce=\"" + PB.cred.digest.nonce + "\", " +
				"uri=\"" + path + "\", " +
				"algorithm=MD5, " +
				"response=\"" + generateResponse(method, path, nc, cnonce) + "\", " +
				"opaque=\"" + PB.cred.digest.opaque + "\", " +
				"qop=" + PB.cred.digest.qop + ", " +
				"nc=" + nc + ", " +
				"cnonce=\"" + cnonce + "\"";
		};
	})();

	// //////////////////
	// The PB Library //
	// //////////////////
	// PB acts as a replacement for many features normally found in Angular.

	window.PB = (function() {

		// Set the URLbase, which is typically an empty string
		this.urlBase = (location.href.indexOf("localhost") < 0) ? "" : "http://" + window.debug.urlBase;
		this.firmwareUpgradeInProgress = false;
		this.cancelClicked = false;
		this.createSessionInProgress = false;
		this.loginCount = 0;
		this.http401Count = 0;
		this.loginModalOpen = false;
		this.oldNonce = null;
		this.continueFlag = false;
		this.continueResetFlag = false;
		this.connectionLossCounter = 0;

		// /////////////
		// Credentials//
		// /////////////
		// This will need to be altered later
		this.cred = {
			digest: undefined,
			type : undefined
		};

		// ////////////////
		// Expose MiniQ //
		// ////////////////

		// Creates a new Defer object, which is also a Promise
		this.defer = function() {
			return new MiniQ();
		};

		this.delay = function (t) {
			var dfd = jQuery.Deferred();
			setTimeout(dfd.resolve, t);
			return dfd.promise();
		}

		// Creates a promise that evaluates when all promise arguments finish
		//  Resolves with an array of each value, or rejects with the first rejection
		this.all = function() {
			var q = PB.defer();
			var result = [];
			var count = 0;
			var args = window.PB.isArray(arguments[0]) ? arguments[0] : arguments;

			if (!args.length) {
				return q.resolve(args);
			}

			var resFunc = function(index, v) {
				result[index] = v;
				count++;
				if (count === result.length) {
					q.resolve(result);
				}
			};
			var rejFunc = function(index, v) {
				q.reject(v);
			};

			for (var i = 0; i < args.length; i++) {
				result.push(undefined);
				args[i].then(resFunc.bind(this, i), rejFunc.bind(this, i));
			}

			return q;
		};

		// Creates a promise that resolves with the value specified
		this.resolve = function() {
			var ret = PB.defer();
			return ret.resolve.apply(ret, arguments);
		};

		// Creates a promise that rejects with the value specified
		this.reject = function() {
			var ret = PB.defer();
			return ret.reject.apply(ret, arguments);
		};

		this.appLogout = function() {
			PB.deleteLocalStorage();
			PB.JQhttp('DELETE', '/rs/users/session').then(function(response) {
				//Deleting digest info from localStorage
				PB.resetSessionInfo();
			}, function(errorResponse) {
				navbar.modals.confirm.go({
					message: errorResponse.status + ":" + errorResponse.statusText,
					title: 'Alert',
					options: ["Ok"]
				});
			})
			
		};

		this.resetSessionInfo = function() {
			PB.cred.digest = undefined;
			PB.cred.type = undefined;
			PB.loginCount = 0;
			if(!PB.loginModalOpen) {
				window.location.href ="/";
			}
		}

		this.showLoginHistory = function() {
			navbar.modals.loginHistory.go({
				title: 'Login History',
				subtitle: 'Since you last logged in...',
				options: ["Ok"]
			});
		}

		this.showlicenseInformation = function() {
			navbar.modals.licenseInformation.go({
				title: 'License Information',
				options: ["Ok"]
			});
		}

		// ////////
		// HTTP //
		// ////////

		// PB.http implements a queued sending to reduce load on the device.
		// The number of simultaneous requests can be adjusted to suit the hardware
		var maxSimultaneousRequests = 1;
		var firstTry = true;
		var pending = [];
		var sendQueue = [];

		// Queues an HTTP request, returning a promise that resolves when then send is complete
		//  Note that in the event of a 401 NOT AUTHORIZED error, PB.http.onAuthError will be called, and the request retried
		this.httpPriority = function(priority, method, path, data) {
			// Create a return deferred which will resolve depending on the result of the XHR request
			var ret = PB.defer();

			var qObject = {
				args: Array.prototype.slice.call(arguments, 1),
				defer: ret,
				priority: priority
			};

			for (var i = 0; i <= sendQueue.length; i++) {
				var sq = sendQueue[i];
				if (!sq || sq.priority < priority) {
					sendQueue.splice(i, 0, qObject);
					break;
				}
			}

			if (PB.firmwareUpgradeInProgress) {
				pending = [];
				sendQueue = [];
			}

			// Wait a second until the DOM is fully loaded
			if (firstTry) {
				setTimeout(function() {
					firstTry = false;
					nextQ();
				}, 1000);
			}

			nextQ();
			return ret;
		};

		// Sends the next command in sendQueue, retrying on 401 errors
		var nextQ = function() {
			PB.dispatchEvent(ENUM.EVENT.HTTP, {
				queueSize: (sendQueue.length + pending.length),
				retrySize: retries.length
			});
			if (pending.length >= maxSimultaneousRequests || sendQueue.length === 0 || firstTry) {
				return;
			}

			var qObject = sendQueue.shift();
			pending.push(qObject);
			var ret = qObject.defer;
			var args = qObject.args;

			return sendHttp.apply(window.PB, args).then(function(xhr) {
				// Success, return xhr
				ret.resolve(xhr);
			}, function(xhr) {
				// Failure
				// 401 retry case applies on login module so checking with URL too.
				// only status check cause issue of infinite calls when 401 returned for any end point.
				if (xhr.status === 401 && xhr.statusText != "Invalid User Credentials") {
					// Store authentication information
					PB.storeAuthInfo(xhr);
					// console.log('Received auth', auth);

					if (PB.http.retryOnAuthError) {
						// Add request to authentication retry
						//  This will put the return deferred on hold until PB.http.onAuthError resolves
						addRetry(ret, xhr, args);
						return;
					}			
				} else if (((xhr.status === 401 && xhr.statusText == "Invalid User Credentials") || (xhr.status === 423 && xhr.statusText == "User Locked Temporarily")) && !PB.loginModalOpen) {
					window.location.href ="/";		
				} else {
					PB.http401Count = 0;
				}
				ret.reject(xhr);
			}).always(function() {
				// Always remove the request from pending and start the nextQ
				pending.splice(pending.indexOf(qObject), 1);
				nextQ();
			});
		};

		// Returns a promise that is resolved on success or rejected on failure
		//  If authentication retries were to be done, it would have to be as a result of failure
		var sendHttp = function(method, path, data) {
			var q = new MiniQ();
			var xhr = new XMLHttpRequest();

			if (xhr.hasOwnProperty("mozBackgroundRequest")) {
				xhr.mozBackgroundRequest = true;
			}
			
			xhr.open(method, PB.urlBase + path, true);
			if (path != "/rs/certstore/cacert") {
				xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
				xhr.setRequestHeader("Accept", "application/json, text/plain, */*");
			} else {
				// To retrieve the ca certificate content in binary format.
				xhr.responseType = 'arraybuffer';
			}
			xhr.withCredentials = true;

			xhr.setRequestHeader("Authorization", createAuthHeader(method, path));
			xhr.onerror = function(xhr) {
				q.reject(xhr);
			};
			xhr.onabort = xhr.onerror;
			xhr.onload = function() {
				if (xhr.status >= 200 && xhr.status < 300) {
					q.resolve(xhr);
				} else {
					q.reject(xhr);
				}
			};
			
			try {
				//Do not Send any request while SFU in progress. ie. abort request.
				if((PB.firmwareUpgradeInProgress && path.indexOf('/rs/fw') < 0) || (PB.createSessionInProgress && path.indexOf('/rs/users/accounts/curruser') < 0 && path.indexOf('/rs/users/roles'))) {
					xhr.abort();
				} else {
					xhr.send(data);
				}	
				
			} catch (e) {
				q.reject(xhr);
			}
			return q;
		};

		// Get and Put shortcuts for http
		this.http = this.httpPriority.bind(this, ENUM.PRIORITY.NORMAL);

		this.http.get = this.httpPriority.bind(this, ENUM.PRIORITY.NORMAL, "GET");
		this.http.put = this.httpPriority.bind(this, ENUM.PRIORITY.NORMAL, "PUT");

		this.http.priorityGet = this.httpPriority.bind(this, ENUM.PRIORITY.HIGH, "GET");
		this.http.priorityPut = this.httpPriority.bind(this, ENUM.PRIORITY.HIGH, "PUT");

		// When true, 401 Authentication errors are not resolved immediately, but instead queued for retry
		//  When queued, PB.http.onAuthError is called
		this.http.retryOnAuthError = true;

		// AddRetry adds a request to the a retry queue. The queue is held until PB.http.onAuthError() is resolved
		//  This triggers a call to PB.http.onAuthError, which should be set to a UI event handler.
		//  PB.http.onAuthError should return a promise that resolves when the user has entered login information and the messages retried
		//  PB.http.onAuthError should reject if the messages should be rejected as well
		this.http.onAuthError = undefined;
		var retries = [];
		var authErrorQ;
		var addRetry = function addRetry(q, xhr, args) {
			retries.push([q, xhr, args]);
			if (!authErrorQ) {
				var retryCB = function(doRetry) {
					authErrorQ = undefined;

					// Retry all of the tries in the queue
					while (retries.length) {
						var retry = retries.shift();
						var q = retry[0];
						var xhr = retry[1];
						var args = retry[2];
						if (doRetry) {
							PB.http.apply(PB.http, args).then(q.resolve.bind(q), q.reject.bind(q));
						} else {
							q.reject(xhr);
						}
					}
				};

				authErrorQ = PB.defer();
				authErrorQ.then(PB.http.onAuthError).then(retryCB, retryCB.bind(this, false));
				authErrorQ.resolve(xhr);
			}
		};

		//adding this wrapper method for firmware upgrade.
		//as promise chain is not working properly with PB framework we are using promise from Jquery.
		//currently it is targeting only firmware upgrade use case.
		this.JQhttp = function (method, url, data, timeout, retryCount) {
			return $.ajax({
				method: method,
				url: PB.urlBase + url,
				data: data,
				contentType: "application/x-www-form-urlencoded",
				dataType: "text",
				timeout: timeout,
				beforeSend: function (request) {
					if (PB.cancelClicked && PB.firmwareUpgradeInProgress) {
						request.abort();
						retryCount = 0;
					} else {
						if ((!PB.createSessionInProgress && !PB.firmwareUpgradeInProgress) || (PB.firmwareUpgradeInProgress && url.indexOf('/rs/fw') >= 0)) {
							request.setRequestHeader("Authorization", createAuthHeader(method, url));
						}
						else {
							request.abort();
						}
					}
				},
				error: function (xhr) {
					if (xhr.status === 401 && xhr.statusText != "Invalid User Credentials") {
						PB.storeAuthInfo(xhr);
					} else if (((xhr.status === 401 && xhr.statusText == "Invalid User Credentials") || (xhr.status === 423 && xhr.statusText == "User Locked Temporarily")) && !PB.loginModalOpen) {
						window.location.href = "/";
					} else {
						PB.http401Count = 0;
					}
				}
			}).retry({
				times: retryCount,
				timeout: 10000,
				statusCodes: [0, 420] //arry of status code for retry 
				//retry only when status network time out ajax status code is zero and device sends busy request during firmware upgrade
			});
		};

		// Store authentication information
		this.storeAuthInfo = function (xhr) {
			PB.http401Count += 1;
			if (PB.http401Count > 0 && !PB.loginModalOpen && !PB.createSessionInProgress) {
				if (PB.checkIfLocalStorageExists()) {
					PB.oldOpaque = PB.cred.digest.opaque;
					PB.newOpaque = PB.generateAuth(xhr);
					if (!PB.continueResetFlag && PB.oldOpaque != PB.newOpaque.opaque) {
						PB.continueResetFlag = true
						PB.deviceRebootTimer = setTimeout(function () {
							window.location.href = "/";
						}, 15000);
						document.getElementById('fwContinueBtn').click();
						navbar.modals.confirm.go({
							message: "Device is rebooted, please Reload Web UI.",
							title: "Alert",
							options: ["Reload"]
						}).then(function (response) {
							if (response == 0) {
								PB.deleteLocalStorage();
								PB.resetSessionInfo();
							}
						});
					}
					else if (!PB.continueResetFlag && PB.http401Count > 0 && !PB.loginModalOpen && !PB.createSessionInProgress) {
						if (!PB.continueFlag && PB.checkIfLocalStorageExists()) {
							PB.continueFlag = true;
							PB.oldNonce = PB.cred.digest.nonce;
							PB.sessionLogoutTimer = setTimeout(function () {
								window.location.href = "/";
							}, 60000)
							navbar.modals.sessionExpired.go({
								message: ['Your session time has expired. You have been logged out.', 'Would you like to restart your session?'],
								title: 'Session Expired',
								options: ["Yes", "No"]
							}).then(function (response) {
								if (response == 0) {
									clearTimeout(PB.sessionLogoutTimer);
									PB.createSessionInProgress = true;
									PB.cred.digest = undefined;
									PB.cred.type = undefined;
									PB.loginCount = 0;
									PB.http401Count = -1;
									PB.login(PB.cred.username, PB.cred.password).then(function () {
										// Re-Login successful
										PB.continueFlag = false;
										PB.createSessionInProgress = false;
									}, function (error) {
										// Re-Login failed
										if ((error.statusText == "timeout") || (error.statusText == undefined)) {
											navbar.modals.confirm.go({
												message: "Communication has been lost with the device. Please verify network connection and refresh browser.",
												title: "Alert",
												options: ["Refresh"]
											}).then(function (response) {
												if (response == 0) {
													PB.deleteLocalStorage();
													PB.resetSessionInfo();
												}
											});
										} else {
											var errMsg = error.status + ":" + error.statusText;
											navbar.modals.confirm.go({
												message: errMsg,
												title: 'Alert',
												options: ["Logout"]
											}).then(function (response) {
												if (response == 0) {
													PB.deleteLocalStorage();
													PB.resetSessionInfo();
												}
											});
										}
									});
								} else if (response == 1) {
									PB.deleteLocalStorage();
									PB.resetSessionInfo();
								}
							});
						}
					}
				}
			} else {
				PB.cred.digest = PB.generateAuth(xhr);
				//Storing Digest info in localStorage	
				PB.storeInLocalStorage(PB.cred.digest);
			}
		};

		// Function to extract digest information from response header.
		this.generateAuth =  function(xhr){
			var header = xhr.getResponseHeader("www-authenticate");
			var type = header.split(" ")[0];
			var rows = xhr.getResponseHeader("www-authenticate").replace(type, "").split(",");
			var auth = {
				type: type
			};
			if (auth.type === "TK_Digest") {
				for (var i = 0; i < rows.length; i++) {
					var e = rows[i].split("=");
					auth[e[0].trim()] = e[1].trim().replace("\"", "").replace("\"", "");
				}
				PB.cred.type = "DIGEST";
			}
			else if (auth.type === "TK_Basic") {
				PB.cred.type = "BASIC"
			}
			return auth
		};

		// Sets the login credentials for XHR
		this.setLogin = function(username, password) {
			var old = {
				username: PB.cred.username,
				password: PB.cred.password
			};
			PB.cred.username = username;
			PB.cred.password = password;
			return old;
		};

		// Clears the login credentials for XHR
		this.clearLogin = function() {
			return this.setLogin(undefined, undefined);
		};

		// //////////////////////////////
		// Standard Library Functions //
		// //////////////////////////////
		this.noop = function() {};

		this.isObject = function isObject(o) {
			return (typeof o === "object" || o instanceof Object);
		};
		this.isNumber = function isNumber(n) {
			return !isNaN(parseFloat(n)) && isFinite(n);
		};

		this.isArray = Array.isArray;

		this.isString = function isString(s) {
			return (typeof s === "string" || s instanceof String);
		};

		this.isFunction = function isFunction(f) {
			return (typeof f === "function");
		};

		// Shallow equals function
		this.equals = function equals(o1, o2, deep) {
			if (o1 == o2) {
				return true;
			}
			if (typeof o1 !== typeof o2) {
				return false;
			}
			var keys = Object.keys(o2);
			for (var n in o1) {
				var i = keys.indexOf(n);
				if (i < 0) {
					return false;
				}
				keys.splice(i, 1);
				if ((deep && !equals(o1[n], o2[n], deep)) || (!deep && o1[n] !== o2[n])) {
					return false;
				}
			}
			if (keys.length > 0) {
				return false;
			}
			return true;
		};

		/**
		 * Deep Copy function
		 */
		this.copy = function copy(x) {
			var ret;
			if (PB.isFunction(x)) {
				// Won't copy functions
				return x;
			} else if (PB.isArray(x)) {
				ret = [];
			} else if (PB.isObject(x)) {
				ret = {};
			} else {
				return x;
			}
			for (var n in x) {
				ret[n] = PB.copy(x[n]);
			}
			return ret;
		};

		// Copies all values from one object into another
		//  var x = {a: 1, b: 2}
		//  PB.extend(x, {b:20, c:30, e:40}, {e:44})
		//  
		//  x -> {a:1, b:20, c:30, e:44}
		//  
		this.extend = function(dest) {
			for (var i = 1; i < arguments.length; i++) {
				for (var n in arguments[i]) {
					dest[n] = arguments[i][n];
				}
			}
			return dest;
		};

		// Returns this object as an array if it isn't already an array
		this.asArray = function(obj) {
			return PB.isArray(obj) ? obj : [obj];
		};

		this.values = function(obj) {
			var r = [];
			for (var n in obj) {
				r.push(obj[n]);
			}
			return r;
		};

		// Returns true if the value is found in the given array.
		this.inArray = function(needle, haystack) {
			haystack = PB.asArray(haystack);
			for (var i = haystack.length - 1; i >= 0; i--) {
				if (haystack[i] == needle) {
					return true;
				}
			}
		};

		// Returns true if the given element is a sidebar subtype.
		this.isSidebar = function(elem) {
			return (elem.is && elem.is.indexOf("sidebar") === 0);
		}

		// Returns true if the given element is a sidebar subtype.
		this.isAction = function(elem) {
			return (elem.is && elem.is.indexOf("action") === 0 || elem.className.match(/\baction\b/));
		}


		// Add zero in front of numbers < 10
		var formatDate_checkTime = function(i) {
			return i < 10 ? "0" + i : i;
		};

		// Handles formatting of symbols H m s Y M d h a
		var formatRegex = [
			[/H+/, function(date) {
				return date.getHours();
			}],
			[/m+/, function(date) {
				return formatDate_checkTime(date.getMinutes());
			}],
			[/s+/, function(date) {
				return formatDate_checkTime(date.getSeconds());
			}],
			[/Y+/, function(date) {
				return date.getFullYear();
			}],
			[/M+/, function(date) {
				return date.getMonth() + 1;
			}],
			[/d+/, function(date) {
				return date.getDate();
			}],
			[/h+/, function(date) {
				return ((date.getHours() + 11) % 12) + 1;
			}],
			[/a[m]?(?:\/p[m]?)?/, function(date) {
				return date.getHours() >= 12 ? "pm" : "am";
			}],
			[/A[M]?(?:\/P[M]?)?/, function(date) {
				return date.getHours() >= 12 ? "PM" : "AM";
			}]
		];

		//Date format type
		this.dateFormatType = function(type) {
			var formatType;
			switch(type) {
				case 0:
					formatType = "MM/dd/YYYY";
					break;
				case 1:
					formatType = "dd/MM/YYYY";
					break;
				case 2:
					formatType = "YYYY-MM-dd";
					break;
				case 3:
					formatType = "dd MM YYYY";
					break;
				default:
					formatType = "MM/dd/YYYY";
			}
			return formatType;
		}

		//Time format type
		this.timeFormatType = function(type) {
			var formatType;
			switch(type) {
				case 0:					
					formatType = "hh:mm:ss A"; //12Hrs - AM/PM
					break;
				case 1:
					formatType = "HH:mm:ss"; //24Hrs
					break;
				default: 
					formatType = "hh:mm:ss A"
					break;
			}
			return formatType;
		}

		// Formats the date as a string
		this.formatDate = function(date, format) {
			var ret = format;
			for (var i = 0; i < formatRegex.length; i++) {
				var n = formatRegex[i];
				ret = ret.replace(n[0], n[1](date));
			}
			return ret;
		};

		Date.prototype.addDays = function(days) {
			var dat = new Date(this.valueOf());
			dat.setDate(dat.getDate() + days);
			return dat;
		}

		this.dateDiffInDays = function(startDate, EndDate) {
			var _MS_PER_DAY = 1000 * 60 * 60 * 24;
			var utc1 = Date.UTC(startDate.getFullYear(), startDate.getMonth(), startDate.getDate());
  			var utc2 = Date.UTC(EndDate.getFullYear(), EndDate.getMonth(), EndDate.getDate());
			return Math.floor((utc2 - utc1) / _MS_PER_DAY);
		};

		// Formats a number as a string 
		this.formatDecimal = function(number, precision) {
			if (!PB.isNumber(number) || isNaN(number)) {
				return "--";
			}

			var p = Math.pow(10, precision);
			return parseFloat(Math.round(number * p) / p).toFixed(precision);
		};

		// Global method of getting the time. Especially useful if using the device time instead of the local time
		this.now = function() {
			return new Date();
		};

		//validate IP Address format.
		//Also allow to valid IP address for multipe entries with comma. In that case, all IP address should be valid.
		this.validateIPaddress = function(ipaddress) {
			if(ipaddress && ipaddress.indexOf(',') > -1) {
				var segments = ipaddress.split(',');
				var allValid = true;
				segments.every(function(element, index) {
					if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(element)) {  
						return true; // to continue checking.
					}
					else {
						allValid = false;
						return false; // to break loop.
					}
				});
				return allValid;
			}
			else if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ipaddress)) {  
				return true ;
			}
			return false;
		};

		//Validate Bitwise format
		this.validateBitfields = function(hexValue, param) {
			var minMax = {
				"byte": [0, 255],
				"word": [0, 65535],
				"dword": [0, 4294967295]
			}[param.datatype];
			var decimalValue = Number(hexValue);

			if (decimalValue != NaN) {
				if (decimalValue <= minMax[1] && decimalValue >= minMax[0]) {
					return true;
				} 
			}	
			return false;
		};

		this.getDeviceTime = function () {
			//when checking device time, we also check connection status.
			//if this api fails 3 times then we indicate as connection loss.
			PB.JQhttp('GET', '/rs/assy/13/values', '', 0, 0).then(function (data) {
				//This will ensure to clear existing timeout calls.
				//If we don't clear our clock will be flicker when any param get changed.
				for (var i = 0; i < window.TimerTimeOut.length; i++) {
					clearTimeout(window.TimerTimeOut[i]);
				}
				window.TimerTimeOut = [];
				PB.connectionLossCounter = 0;
				var json = PB.parseXML(data);
				deviceTimeCallback(json);
				PB.refreshNavbarTime();
			}, function (xhr) {
				PB.refreshNavbarTime();
				if (xhr.statusText == "timeout" || (xhr.status == 0 && xhr.statusText != "canceled")) {
					PB.connectionLossCounter += 1;
					if (PB.connectionLossCounter == 3 && !PB.continueFlag) {
						navbar.modals.connLost.go({
							message: "Communication has been lost with the device. Please verify network connection and refresh browser.",
							title: "Alert",
							options: ["Refresh"]
						}).then(function (response) {
							if (response == 0) {
								PB.deleteLocalStorage();
								PB.JQhttp('DELETE', '/rs/users/session').then(function() {
									PB.resetSessionInfo();
								}, function(error) {
									PB.resetSessionInfo();
								});
							}
						});
					}
				}
			});			
		}

		//Refreshing time in the navbar
		this.refreshNavbarTime = function() {
			setTimeout(function () {
				PB.getDeviceTime();
			}, 15000);
		}
		
		//User inactivity timeout session
		this.idleLogout = function() {
			var t, inactivityTime;
			var param = PB.DCI.get("DCI_USER_INACTIVITY_TIMEOUT");
			if (param) {
				PB.JQhttp('GET', "/rs/param/" + param.pid + "/value").then(function(data){
					var json = PB.parseXML(data);
					inactivityTime = json.Value.value * 1000;
					window.onload = resetTimer;
					window.onmousemove = resetTimer;
					window.onmousedown = resetTimer;
					window.onclick = resetTimer;  
					window.onscroll = resetTimer;
					window.onkeypress = resetTimer;
					resetTimer();
				});
			}

			function logout() {
				PB.appLogout();
			}
		
			function resetTimer() {
				clearTimeout(t);
				t = setTimeout(logout, inactivityTime);  // time is in milliseconds
			}
		}

		//Function to store in localstorage
		this.storeInLocalStorage = function(data) {
			localStorage.setItem(md5(PB.cred.username), JSON.stringify(data));
		}

		//Function to retrieve from localstorage
		this.retrieveFromLocalStorage = function() {
			var data = JSON.parse(localStorage.getItem(md5(PB.cred.username)));
			return data;
		}

		//Function to delete localstorage
		this.deleteLocalStorage = function() {
			localStorage.removeItem(md5(PB.cred.username));
		}

		//Function to check if localstorage exists
		this.checkIfLocalStorageExists = function() {
			if (PB.retrieveFromLocalStorage() != undefined && PB.retrieveFromLocalStorage() != null ) {
				return true;
			} else {
				return false;
			}
		}

		// Saves an object as a file
		// Takes option filename and object type, which default to "file.json" and "application/json" respectively.
		this.saveFile = function(obj, name, type) {
			if (!name) {
				name = "file.json";
			}
			if (!type) {
				type = "application/json"; 
			}
			var blob = new window.Blob([JSON.stringify(obj, null, 4)], {
				name: name,
				type: type
			});
			//This hack creates a link to the blob, and then clicks on it.
			//  Only works in modern browsers (IE10+)
			var a = document.createElement("a");
			a.style.display = "none";
			document.body.appendChild(a);
			a.href = window.URL.createObjectURL(blob);
			a.download = name;
			a.click();
			window.URL.revokeObjectURL(a.href);
			a.parentNode.removeChild(a);
		};

		// Loads a file as an object and returns a promise that evaluates on success.
		// Handles the file chooser and automatically translates the file if it is a JSON or a String if not.
		this.loadFile = function(extention) {
			var promise = PB.defer();
			var input = document.createElement("input");
			input.style.display = "none";
			input.type = "file";
			input.accept = extention;
			input.onchange = function() {
				//Confirm file is valid before continuing
				var myFile = input.files[0];
				if (myFile.type != extention) {
					navbar.modals.confirm.go({
						message: 'Incorrect File Selected',
						title: 'Alert',
						options: ["Ok"]
					});
				} else {
					var reader = new FileReader();
					reader.onload = function (e) {
						var result;
						try {
							result = JSON.parse(e.target.result)
						} catch (x) {
							result = e.target.result;
						}
						promise.resolve(result);
					};
					reader.readAsText(myFile);
				}
			};
			document.body.appendChild(input);
			input.click();
			input.parentNode.removeChild(input);
			return promise;
		};

		this.isIE = function(){
			return !!navigator.userAgent.match(/Trident/g) || !!navigator.userAgent.match(/MSIE/g);
		};
		// Converts XML into a Javascript Object
		//  - Creates a new object or appends data to an existing one
		//  - Attributes are assigned as keys to the object
		//  - If xml is only a value (not XML) it is assigned to parent.value
		//    - If parent doesn't exist, xml is returned
		this.parseXML = function(xml, parent) {
			// RegEx divides proper XML in to three parts:
			// $1 - The node name
			// $2 - Space-separated attributes, or a blank string
			// $3 - The contents of the node, or undefined if self-closing
			var regex = /<(\w+)\s*(.*?)(?:\/>|>([\S\s]*?)<\/\1>)/g
				// Create the basic XML structure
			var ex = regex.exec(xml);
			if (ex === null) {
				// This occurs when "xml" is not XML-formatted.
				//  In this case, apply it to its parent and move on
				if (!parent) {
					parent = xml;
				} else if (xml) {
					parent.value = xml;
				}
				return parent;
			}
			var ret = parent || {};
			while (ex !== null) {
				// Assign each attribute as a property of the value
				var nodeName = ex[1];
				var attributes = ex[2];
				var contents = ex[3];
				var attr = undefined;
				if (attributes) {
					attr = {};
					var attributeList= attributes.match(/(".*?"|[^"\s]+)+(?=\s*|\s*$)/g);
					attributeList.forEach(function(n) {
						var i = n.indexOf("=");
						var k = n.substr(0, i);
						var v = n.substr(i + 1).replace(/^"(.*)"$/, "$1");
						attr[k] = v;
					});
				}
				// Assign the value to the key
				var value = PB.parseXML(contents, attr);
				var old = ret[nodeName];
				if (old) {
					if (!PB.isArray(old)) {
						old = ret[nodeName] = [old];
					}
					old.push(value)
				} else {
					ret[nodeName] = value;
				}
				ex = regex.exec(xml);
			}
			return ret;
		};

		this.EventTarget = function EventTarget() {};
		Object.defineProperties(this.EventTarget.prototype, {
			// Listeners should have the following signature:
			//  function(type, event)
			//  
			//  Where event = {
			//      type: (The type of event)
			//      data: (Any other data included with the event)
			//      param: (The calling Param) (Param Events only)
			//  }
			addEventListener: {
				value: function(type, listener) { // Adds an event listener to the given type
					if (!type) {
						console.warn('Add Event type ' + type);
					}
					if (!this.listeners) {
						this.listeners = {};
					}
					if (!this.listeners[type]) {
						this.listeners[type] = [];
					}
					this.listeners[type].push(listener);
				}
			},
			removeEventListener: {
				value: function(type, listener) { // Removes an event listener from the given type
					if (!type) {
						console.warn('Remove Event type ' + type);
					}
					if (this.listeners && this.listeners[type]) {
						var index = this.listeners[type].indexOf(listener);
						if (index > -1) {
							this.listeners[type].splice(index, 1);
						}
					}
				}
			},
			dispatchEvent: {
				value: function(type, e) { // Dispatches an event
					if (!type) {
						console.warn('Dispatch Event type ' + type);
					}
					if (this.listeners && this.listeners[type]) {
						var listeners = this.listeners[type];
						for (var i = listeners.length - 1; i >= 0; i--) {
							listeners[i](type, e);
						}
					}
				}
			}
		});
		Object.setPrototypeOf(this, this.EventTarget.prototype);

		return this;
	}).apply({});

})();
