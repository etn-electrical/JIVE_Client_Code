(function() {

	// The existance of window.debug enables all debug features.
	//	This structure should be removed in the final product
	window.debug = {
		urlBase: "10.9.51.222", // "166.99.89.74:8080",
		find: function(needle, haystack) {
			if (haystack === needle) {
				return haystack;
			} else if (typeof haystack === "object") {
				var ret;
				for (var n in haystack) {
					if (n === needle || haystack[n] === needle) {
						return haystack;
					} else {
						ret = window.debug.find(needle, haystack[n]);
						if (ret) {
							return ret;
						}
					}
				}
			}
			return null;
		}		
	};

	// Shim to get all values in an object
	if (Object.values === undefined) {
		Object.values = function(obj) {
			return Object.keys(obj).map(function(key) {
				return obj[key];
			});
		};
	}

	// ENUM object for all numeric values.
	window.ENUM = Object.freeze({
		NAME_ARRAY_DELIM: ";",

		PRIORITY: {
			HIGH: 30,
			NORMAL: 20,
			LOW: 10
		},

		// ENUM.STATUS used for navbar.getStatus and navbar.setStatus
		// Higher numbers override lower ones
		STATUS: {
			LOADING: 3,
			UNSAVED_CHANGES: 10,
			EXPORT: 10,
			SAVED: 11,
			IMPORT: 11
		},

		ROLE: {
			NONE: 0,
			VIEWER: 7,
			OPERATOR: 15,
			ENGINEER: 31,
			ADMIN: 99
		},

		EVENT: {
			// PB Events
			HTTP: "HTTP", // Occurs when an HTTP message is queued, or received
			LOGIN: "LOGIN", // Occurs when the user changes

			// navbar Events
			GO: "GO",

			// Param Events
			PARAM: {
				READ: "READ", // Occurs when a Param is Read from the LTK
				WRITE: "WRITE", // Occurs when a Param is Written to the LTK
				SET: "SET", // Occurs when a Param is Set by the user
				CHANGE: "CHANGE" // Occurs when a Param changes, either from a Read or a Set
			},
			PARAMS_LIST_CHANGED: "PARAMS_LIST_CHANGED", // Occurs when an element's params list changes

			// Tab Events
			TAB: {
				ACTIVE_CHANGED: "TAB_ACTIVE_CHANGED",
			}
		},

		DATATYPE_SIZE: {
			string8: 1,
			bool: 1,
			float: 4,
			dfloat: 8,
			sint8: 1,
			sint16: 2,
			sint32: 4,
			sint64: 8,
			uint8: 1,
			uint16: 2,
			uint32: 4,
			uint64: 8,
			byte: 1,
			word: 2,
			dword: 4
		}
	});

})();
