(function() {
	"use strict";
	// UserParams, are similar to Params and contains these keys: define, datatype, count, mask, name, desc, min, max, enum_desc:enum
	var userParams = [
		["FullName", "string8", "FullNameSizeLimit", 3, "Full name", "The full name of the user"],
		["UserName", "string8", "UserNameSizeLimit", 1, "Username", "The username used to login"],
		["PwdComplexity", "uint16", 1, 2, "Password Complexity", "The level of complexity for the user", 0, 3, {
			"1. It should be at least 6 characters long\n2. It should not match with user name or full name\n3. It should not match with existing password": 0,
			"1. It should be at least 8 characters long\n2. It should not match with user name or full name\n3. It should not match with existing password\n4. It should contain at least 1 alphabetic and 1 numeric character": 1,
			"1. It should be at least 12 characters long\n2. It should not match with user name or full name\n3. It should not match with existing password\n4. It should contain at least 1 alphabetic and 1 numeric character\n5. It should contain at least 1 special character\n6. It should contain at least 1 upper case alphabetic character\n": 2,
			"1. It should be at least 16 characters long\n2. It should not match with user name or full name\n3. It should not match with existing password\n4. It should contain at least 2 alphabetic character and 1 numeric character\n5. It should contain at least 2 special character\n6. It should contain at least 1 upper case alphabetic character\n7. It should contain at least 1 lower case alphabetic character\n": 3
		}],
		["PwdTimeoutDays", "uint16", 1, 3, "Password Timeout", "The number of days until the password expires (0 to disable)"],
		["Role", "uint16", 1, 3, "Role", "Role"],
		["Pwd", "string8", 20, 0, "Password", "The user's password"], // Write-only parameter
		["PwdSetDate", "string8", 1, 0, "Password Set Date", "The date the password was set"]

	];

	var UserParam = function UserParam(user, info) {
		this.user = user;
		this.key = info[0];
		this.datatype = info[1];
		this.count = info[2];
		this.group = info[3]
		this.name = info[4];
		this.desc = info[5];
		this.min = info[6];
		this.max = info[7];
		this.debug = this.key;

		if (this.key === "Pwd" || this.key === "PwdConfirm") {
			// Special case for password
			Object.defineProperty(this, "input_type", {
				value: "password"
			});
		} else if (this.key === "PwdSetDate") {
			// Special case for Date
			this.getWriteString = function() {
				return PB.formatDate(PB.now(), "DD:MM:YYYY");
			};
		} else if (this.datatype === "string8") {
			// Special case for string8; use Base64 encoding
			this.getWriteString = function() {
				return atob(this.value);
			};
		} else {
			this.getWriteString = function() {
				return Number(this.value);
			};
		}
	};

	UserParam.prototype = Object.create(PB.Param.prototype, {
		canEdit: {
			value: function(role) {
				return this.user.canEdit(role);
			}
		},

		canView: {
			value: function(role) {
				return this.user.canView(role);
			}
		},

		_value: {
			get: function() {
				return this.value;
			}
		},

		value: {
			get: function() {
				return this.user.writeValue[this.key];
			},
			set: function(value) {
				var data = {
					oldValue: this.user.writeValue[this.key],
					newValue: value
				};
				this.user.writeValue[this.key] = value;
				this.dispatchEvent(ENUM.EVENT.PARAM.CHANGE, data);
				this.user.dispatchChange();
			}
		},

		clear: {
			value: function() {
				this.value = "";
			}
		},

		isDirty: {
			get: function() {
				if(this.key == 'Role') {
					return this.user.writeValue[this.key] !== this.user.cacheValue[this.key].value;
				}
				return this.user.writeValue[this.key] !== this.user.cacheValue[this.key];
			},
			set: PB.noop
		},
		read: { // Updates User.value (.cacheValue). Won't read if already read, but can be forced. Returns a promise.
			value: function userRead(forceRead) {
				// Not supported
				return PB.resolve(this);
			}
		},
		write: { // Writes User.value (.writeValue) to the device
			value: function userWrite() {
				return this.user.write();
			}
		},
		revert: { // Sets Param.value (.writeValue) to the .cacheValue, and clears the isDirty flag.
			value: function revert() {
				this.value = this.user.cacheValue[this.key];
			}
		}
	});
	UserParam.prototype.constructor = PB.Param;

	var User = function User(url) {
		this.url = url;
		this.cacheValue = {}; // cacheValue should only be read by the User (i.e. user.read)
		this.writeValue = {}; // writeValue should only be accessed by the individual params
		this.define = "USER/" + url.lastNumber;
		this.debug = url;

		// this.lastLogin = undefined; // Read-only date set at first read
		//this.PwdConfirm = new UserParam(this, ["PwdConfirm", "string8", 20, 0, "Confirm Password", "Re-enter password"]); // Psuedo parameter; not sent or received
		//PB.DCI[url + "/PwdConfirm"] = this.PwdConfirm;

		// These are the parameters that are defined for the user
		this.params = {};
		for (var n in userParams) {
			var p = new UserParam(this, userParams[n]);
			this.params[p.key] = this[p.key] = p;
		}
	};

	User.prototype = Object.create(PB.Param.prototype, {
		role: {
			get: function() {
				var Role = this.Role;
				return (Role && Role.value) || ENUM.ROLE.NONE;
			}
		},

		dispatchChange: {
			value: function(data) {
				this.dispatchEvent(ENUM.EVENT.PARAM.CHANGE, {
					oldValue: this.cacheValue,
					newValue: this.writeValue
				})
			}
		},

		canEdit: {
			value: function(role) {
				return (this === PB.currentUser || PB.currentUser.Role.value === ENUM.ROLE.ADMIN);
			}
		},

		canView: {
			value: function(role) {
				return (this === PB.currentUser || PB.currentUser.Role.value === ENUM.ROLE.ADMIN);
			}
		},

		read: { // Updates User.value (.cacheValue). Won't read if already read, but can be forced. Returns a promise.
			value: function userRead(forceRead) {
				if (!forceRead) {
					return PB.resolve(this);
				}
				var user = this;
				// priorityGet is used so the whole login sequence is high priority
				return PB.http.priorityGet(this.url).then(function(xhr) {
					user.cacheValue = PB.parseXML(xhr.responseText).UserAccount;
					user.revert();
					return user;
				}, function(xhr) {
					console.error("Failed to read " + user.debug + ": " + xhr.statusText, xhr);
					delete user.cachePromise;
				});
			}
		},
		write: { // Writes User.value (.writeValue) to the device
			value: function userWrite() {
				console.warn("NOT SAVING USER " + this.UserName.value);
				return PB.resolve();


				// Determine what case to write
				if (!this.isValid()) {
					// Can't change anything without a password
					console.warn("Invalid user cannot be saved");
					return PB.reject();
				}

				// Find the OR Bitmask for the mask
				var mask = 0;
				for (var n in this.params) {
					var p = this.params[n];
					if (p.isDirty) {
						mask = mask | p.mask;
					}
				}

				// Create the send string based on the mask
				var send = "<UserAccount>";
				for (var n in this.params) {
					var p = this.params[n];
					if (p.mask === 0 || (p.mask & mask !== 0)) {
						send += "<" + n + ">" + p.getWriteString() + "</" + n + ">";
					}
				}
				send += "</UserAccount>";

				// Format the request and send it to the device
				var user = this;
				return PB.http.put(this.url, send).then(function() {
					if (window.debug) {
						console.log("Wrote " + user.debug, send);
					}
					user.read(true);
				}, function(reason) {
					console.error("Failed to write " + user.debug, send, reason);
				});
			}
		},
		revert: { // Sets Param.value (.writeValue) to the .cacheValue, and clears the isDirty flag.
			value: function revert() {
				var user = this;
				user.writeValue = PB.extend({}, user.cacheValue);
				for (var n in user.writeValue) {
					if (PB.isObject(user.writeValue[n])) {
						user.writeValue[n] = user.writeValue[n].value;
					}

					// Set the incoming values to correspond to the ENUM values of the respective parameter
					if (user[n] && user[n].enum) {
						var param = user[n];
						var e = param.enum;
						for (var i = 0; i < e.length; i++) {
							if (param.value === e.value || param.value === e.desc) {
								param.value = e.value;
							}
						}
					}
				}
				user.Pwd.clear();
				//user.PwdConfirm.clear();
				this.isDirty = false;
				if (user.cacheValue.LastLoginTime == 0) {
					user.lastLogin = "Never";
				} else {
					user.lastLogin = new Date(user.cacheValue.LastLoginTime * 1000);
				}
				user.lastPasswordSet = new Date(user.cacheValue.PwdSetEpochTime * 1000);
				user.failedAttemptCount = user.cacheValue.FailedAttemptCount;
				this.dispatchEvent(ENUM.EVENT.PARAM.CHANGE);
			}
		},
		isValid: {
			get: function() {
				for (var n in this.params) {
					if (!this.params[n].isValid) {
						console.log("User Param invalid: ", this.params[n]);
						return false;
					}
				}
				return true; // (this.Pwd.value || this.Pwd.value !== this.PwdConfirm.value);
			}
		}
	});
	User.prototype.constructor = PB.Param;

	// ////////////
	// Users API //
	// ////////////
	PB.createUser = function(url) {
		for (var i = 0; i < PB.userList.length; i++) {
			if (PB.userList[i].url === url) {
				PB.userList[i].clear();
				return PB.userList[i];
			}
		}
		var newUser = new User(url);
		PB.userList.push(newUser);
		return newUser;
	};

	PB.newUser = function(url) {
		var newUser = new User(url);
		return newUser;
	};

	PB.getAllUsers = function () {
		var dfd = jQuery.Deferred();
		PB.http.priorityGet("/rs/users/accounts").then(function(response) {
			var json = PB.parseXML(response.responseText);
			var accountList = json.UserAccountList;
			accountList.User = PB.asArray(accountList.User);
			PB.userList = [];
			for (var i = 0; i < accountList.User.length; i++) {
				var userObject = accountList.User[i];
				var user = PB.createUser(userObject["xlink:href"]);
			}
			return PB.userList;
		}).then(function (userList) {
			PB.all(userList.map(function(user) {
				return user.read(true);
			})).then(function(users) {
				users.forEach(function(user) {
				user["lastLoginDetail"] = user.lastLogin.toLocaleString();
				var datePasswordChangeDate = user.lastPasswordSet;
				var numberPwdTimeoutDays = parseInt(user.PwdTimeoutDays.value);	

				if(numberPwdTimeoutDays == 0) {
					user["pwdNextDate"] = "Never";
				}
				else {
					var nextDate = datePasswordChangeDate.addDays(numberPwdTimeoutDays);
					var currentTime = new Date();
					if(currentTime > nextDate) {
						user["pwdNextDate"] = "Expired"
					}
					else {
						var pwdChangeDue = PB.dateDiffInDays(new Date(), nextDate)
						if(pwdChangeDue > 1) {
							user["pwdNextDate"] = pwdChangeDue + " Days";
						} else {
							user["pwdNextDate"] = pwdChangeDue + " Day";
						}
						
					}
				}
			});
			dfd.resolve(users);
		});
	});
	return dfd.promise();
}

	PB.getAllRoles = function() {
		var dfd = jQuery.Deferred();
		PB.http.priorityGet("/rs/users/roles").then(function(response) {
			var json = PB.parseXML(response.responseText);
			var roleList = PB.asArray(json.UserRoles.Role);
			dfd.resolve(roleList);
		})
		return dfd.promise();
	}

	PB.currentUser = Object.create(User.prototype);
	
	PB.currentUserRoleLevel = null;

	PB.login = function(username, password) {
		
		PB.logout();
		PB.setLogin(username, password);
		
		// It's possible the user wants to login before any digest error has occurred
		//  Turn off retry and send a message that is guaranteed to fail, then send the actual check
		var doLogin = function(xhr) {		
			var json = PB.parseXML(xhr.responseText); 	
			var user = PB.createUser(json.UserAccount["xlink:href"]);
			if (json.UserAccount.ChangePwd == "2") {
				 PB.changePswd = true
			} else if (json.UserAccount.ChangePwd == "1")  {
				 PB.changePswd = false
			}
			user.cacheValue = json.UserAccount;
			user.revert();
			PB.currentUser = user;

			if (PB.currentUser) {
				var roleInfoUrl = user.cacheValue.Role['xlink:href'];
				return PB.http.priorityGet(roleInfoUrl).then(function (xhr) {
					PB.currentUserRoleLevel = parseInt(PB.parseXML(xhr.responseText).UserRole.RoleLevel);
					var dciProductCode = PB.DCI.get("DCI_PRODUCT_CODE").pid;
					return PB.http.priorityGet("/rs/param/" + dciProductCode + "/value").then(function (dciResp) {
						PB.currentProductCode = PB.parseXML(dciResp.responseText).Value.value;
						return user;
					})
				});
			} else {
				// We couldn't find our name! Logout and reject.
				return PB.logout().then(function() {
					return PB.reject();
				});
			}
		};

		var oldRetry = PB.http.retryOnAuthError;
		PB.http.retryOnAuthError = false;
		
		return PB.http.priorityGet("/rs/users/accounts/curruser").then(undefined, function(xhr) {
			if (PB.cred.digest && PB.oldNonce != PB.cred.digest.nonce) {
				PB.loginCount = 1;
			}
			if (PB.loginCount <= 1) {
				if (xhr.status === 401 && xhr.statusText != "Invalid User Credentials") {
					// We might be facing a Digest issue, try again
					return PB.http.priorityGet("/rs/users/accounts/curruser");
				} else {
					return PB.reject(xhr);
				}
			} else {
				return PB.reject(xhr);
			}
		}).then(doLogin).always(function() {
			PB.http.retryOnAuthError = oldRetry;
		});
		
	};

	PB.logout = function() {
		PB.clearLogin();
		PB.userList = [];
		PB.currentUser = undefined;
		return PB.resolve();
	};
}());