(function() {
	"use strict";

	// ////////////////////
	// Param Definition //
	// ////////////////////

	// Param Object, with special functions to read and parse values
	//  This creates Param objects from the DCI object in dci.js elements.
	//
	// dci.js obj contains these keys: define, pid, r, w, format, datatype, count, name, desc, units, min, max, enum, enum_desc, modbus
	// [modbus] array contains object with keys: reg, length, start
	//
	var Param = PB.Param = function Param(obj) {
		var i;
		for (var n in obj) {
			if (isNaN(obj[n]) || (obj[n].trim && obj[n].trim() === "")) {
				this[n] = obj[n];
			} else {
				this[n] = Number(obj[n]);
			}
		}

		// enum becomes an array:
		// [{
		// 		value: (the value selected)
		// 		desc:  (the string to display with the value; may be a copy of value)
		// }]
		if (!("enum" in this)) {
			this.enum = false;
		} else {
			var en = [];
			for (i = 0; i < this.enum.length; i++) {
				en[i] = {
					value: this.enum[i],
					desc: (this.enum_desc[i] === undefined ? this.enum[i] : this.enum_desc[i])
				};
			}
			this.enum = en;
		}

		// bitfield becomes an array:
		// [{
		// 		value: (the value selected)
		// 		desc:  (the string to display with the value)
		// }]
		if (!("bitfield" in this)) {
			this.bitfield = false;
		} else {
			var en = [];
			for (i = 0; i < this.bitfield.length; i++) {
				en[i] = {
					value: this.bitfield[i],
					desc: (this.bitfield_desc[i] === undefined ? this.bitfield[i] : this.bitfield_desc[i])
				};
			}
			this.bitfield = en;
		}

		//export and import values are converted to Boolean
		if (this.export === 1) {
			this.export = true;
		} else if (this.export == undefined) {
			console.log("Export functionality removed!!")
		} 
		else {
			this.export = false;
		}

		if (this.import === 1) {
			this.import = true;
		} else if (this.import == undefined) {
			console.log("Import functionality removed!!")
		} else {
			this.import = false;
		}

		// Special handling for arrays
		// Original array param gains numeric "properties" of SubParams
		// Original param is expected to have name, desc, and units split by ";"
		if (this.isArray()) {
			var keys = ["name", "desc", "units"];
			var splits = {};
			var key, j;
			var subParams = [];
			for (i = keys.length - 1; i >= 0; i--) {
				key = keys[i];
				splits[key] = this[key].split(ENUM.NAME_ARRAY_DELIM);
				this[key] = splits[key][0];
			}
			for (i = 0; i < this.count; i++) {
				var ext = {
					count: 1
				};
				for (j = keys.length - 1; j >= 0; j--) {
					key = keys[j];
					ext[key] = splits[key][i + 1];
					if (ext[key] === undefined && key === "name") {
						ext.name = this[key] + "[" + i + "]";
					}
				}
				subParams[i] = new SubParam(this, i, ext);
			}
			PB.extend(this, subParams);

		}

		this.asAttribute = this.define;
		this.debug = this.asAttribute;
		this.saveFailed = false;
		this.clear();
	};

	Param.prototype = Object.create(PB.EventTarget.prototype);
	Param.prototype.constructor = Param;

	Object.defineProperties(Param.prototype, {
		isArray: {
			value: function() {
				return this.datatype !== "string8" && this.count > 1;
			}
		},
		canEdit: { // Returns true if this param can be written by the given user level
			value: function(userW) {
				return this.w <= userW;
			}
		},
		canRead: { // Returns true if this param can be read by the given user level
			value: function(userR) {
				return this.r <= userR;
			}
		},
		input_type: { // Returns the input_type of this param, used for <INPUT> and <PX-INPUT> tags
			get: function() {
				if (this.format) {
					return "text";
				}
				switch (this.datatype) {
					case "string8":
						return "text";
					case "bool":
						return "checkbox";
					default:
						return "number";
				}
			}
		},

		/**
		 * Param.value uses both get and set.
		 *
		 * Get queues a read and returns the value, which is either the cached value returned from the device, or the write value from the UI.
		 * Get prefers the write value over the cached value, but both are available via .cacheValue and .writeValue.
		 *
		 * Set writes the writeValue and sets the isDirty flag. The write value can later be reverted using Param.revert
		 */
		value: {
			get: function() {
				this.read();
				return this._value;
			},
			set: function(newValue) {
				var data = {
					oldValue: this._value,
					newValue: newValue
				};

				//When this two values are different then we should set isDirty to True.
				if(this._value !== newValue) {
					this.isDirty = true;
					this.saveFailed = false;
				}

				// toLTKValue may be equal even if the value is not
				//   (Occurs with arrays, and undefined/boolean)
				if (toLTKValue(this, data.oldValue) === toLTKValue(this, data.newValue)) {
					// In this case, writing won't change anything. Set the appropriate value based on the dirty flag.
					if (this.isDirty) {
						this.writeValue = newValue;
					} else {
						this.cacheValue = newValue;
					}
				} else {
					// Set the write value as normal
					this.writeValue = newValue;
				}

				// Dispatch the event, even if the value hasn't changed
				this.dispatchEvent(ENUM.EVENT.PARAM.SET, data);
				this.dispatchEvent(ENUM.EVENT.PARAM.CHANGE, data);
			}
		},
		_value: {
			get: function() {
				return this.isDirty ? this.writeValue : this.cacheValue;
			}
		},
		clear: { // Clears Param.value by clearning both the .cacheValue and the .writeValue.
			value: function() {
				var data = {
					oldValue: this._value
				};
				this.isDirty = false;
				this.saveFailed = false;
				this.saveFailureMsg = '';
				if(PB.isArray(this._value)) {
					for(var i=0; i<this._value.length; i++) {
						this[i].isDirty = false;
						this[i].saveFailed = false;
						this[i].saveFailureMsg = '';
					}
				}
				delete this.writeValue;
				delete this.cacheValue;
				delete this.cachePromise;
				if (data.oldValue !== data.newValue) {
					this.dispatchEvent(ENUM.EVENT.PARAM.SET, data);
					this.dispatchEvent(ENUM.EVENT.PARAM.CHANGE, data);
				}
			}
		},
		read: { // Updates Param.value (.cacheValue). Won't read if already read, but can be forced. Returns a promise.
			value: function paramRead(forceRead) {
				var promise = PB.defer();
				if (forceRead || this.cachePromise === undefined) {
					var param = this;
					param.cachePromise = PB.http.get("/rs/param/" + param.pid + "/value").then(function(xhr) {
						// Set the event data object with the old data
						var data = {
							oldValue: param._value
						};

						// Dumb xml parse: use the value between > and <
						var value = xhr.responseXML.firstChild.firstChild.data;
						if (param.format) {
							switch (param.format) {
								case "CRED()":
									param.cacheValue = toJSValue(param, value);
									break;
								case "IPV4_BIG_ENDIAN()":
								case "IPV4_BIG_ENDIAN_U8()":
									// Reads as a String, writes as a dot-seperated array; no need to change anything here
									param.cacheValue = value;
									break;
								case "MAC_ADDRESS()":
								case "WAIVER()":
									console.log("Warning: " + param.debug + " format " + param.format + " not handled for read");
									param.cacheValue = value;
									break;
								default:
									console.log("Warning: " + param.debug + " format " + param.format + " not handled for read");
									param.cacheValue = value;
									break;
							}
						} else {
							//console.log(" To Js Value :"+ toJSValue(param, value));
							param.cacheValue = toJSValue(param, value);
							//param.cacheValue =  value;
							// This check is to make sure that DCI_COUNTERS parameters appear without decimal points.
							if(param.define == "DCI_COUNTERS"){
								for(var arrCount = 0; arrCount < param.count; arrCount++){
             					  param.cacheValue[arrCount] = parseInt(param.cacheValue[arrCount]);
            					}
							}
						}
						console.log("Read " + param.debug + ": " + param.cacheValue);

						// Set the event data object with new data
						data.newValue = param.cacheValue;

						// Dispatch the READ and CHANGE events
						param.dispatchEvent(ENUM.EVENT.PARAM.READ, data);
						if (data.oldValue !== data.newValue) {
							param.dispatchEvent(ENUM.EVENT.PARAM.CHANGE, data);
						}

						// Return the read value
						promise.resolve(param.cacheValue);
					}, function(xhr) {						
						console.error("Failed to read " + param.debug + ": " + xhr.statusText, xhr);
						delete param.cachePromise;
						promise.reject(xhr);
					});
				}
				return promise;
			}
		},
		write: { // Writes Param.value (.writeValue) to the device if isDirty. Can be called with a parameter to set the writeValue first. Returns a promise.
			value: function paramWrite(writeValue) {
				var param = this;

				// Don't do anything if the Param isn't dirty
				if (!param.isDirty && writeValue === undefined) {
					if (window.debug) {
						console.log("No value to write for " + param.debug);
					}
					var d = PB.defer();					
					return d.resolve();
				}

				if (writeValue === undefined) {
					writeValue = this.writeValue;
				}

				// What to write depends on the type and format
				var value;
				if (param.format) {
					// All bets are off when setting a custom-format parameter
					switch (param.format) {
						case "CRED()":
							// UserPass concatination handled by splitUserPass
							value = toLTKValue(param, writeValue);
							break;
						case "IPV4_BIG_ENDIAN()":
						case "IPV4_BIG_ENDIAN_U8()":					
							//value = toLTKValue(param, writeValue.split("."));
							//commenting above code as it returns value like 192,168,1,2 and PUT comannd fails with bad request.
							//API is expecting value like 192.168.1.2
							value = writeValue;
							break;
						case "MAC_ADDRESS()":
						case "WAIVER()":
							value = writeValue;
							console.warn("Warning: " + param.debug + " format " + param.format + " not handled for write");
							break;
						default:
							value = writeValue;
							console.warn("Warning: " + param.debug + " format " + param.format + " not handled for write");
							break;
					}
				} else {
					// Set .value if a value was passed in
					if (writeValue !== undefined) {
						param.value = toJSValue(param, writeValue);
					}

					// Convert writeValue to an LTK-formatted value, based on this parameter.
					value = toLTKValue(param, writeValue);
				}

				// Format the request and send it to the device
				var data = "<Value pid=\"" + param.pid + "\">" + value + "</Value>";
				return PB.http.put("/rs/param/" + param.pid + "/value", data).then(function() {
					if (window.debug) {
						console.log("Wrote " + param.debug + " " + value);
					}
					var data = {
						oldValue: writeValue,
						LTKValue: value
					};
					param.clear();
					param.dispatchEvent(ENUM.EVENT.PARAM.WRITE, data);
				}, function(reason) {
					console.error("Failed to write " + param.debug + " = " + data, reason);
					param.saveFailed = true;
					if(reason.status == 400) {
						param.saveFailureMsg = "Invalid value. Please enter correct value."
					} else if(reason.status == 401) {
						param.saveFailureMsg = "Save not permitted. You may not have permission. Check your login."
					} else {
						param.saveFailureMsg = "A save error occurred. Please try again.";
					}
					var data = {
						oldValue: this._value,
						newValue: this.cacheValue
					};
					param.dispatchEvent(ENUM.EVENT.PARAM.CHANGE, data);
				});
			}
		},
		revert: { // Sets Param.value (.writeValue) to the .cacheValue, and clears the isDirty flag.
			value: function revert() {
				var data = {
					oldValue: this._value,
					newValue: this.cacheValue
				};
				this.isDirty = false; // Set isDirty to false again, since setting it made it true
				this.saveFailed = false;
				this.saveFailureMsg = '';
				delete this.writeValue;
				if (data.oldValue !== data.newValue) {
					this.dispatchEvent(ENUM.EVENT.PARAM.CHANGE, data);
				}
			}
		},
		dispatchEvent: {
			value: function(type, data) { // Dispatches an event
				var e = PB.extend({
					param: this,
					type: type
						// newValue: [value]
						// oldValue: [value]
				}, data);
				var ret = PB.EventTarget.prototype.dispatchEvent.call(this, type, e);

				// Forward the event to all of its children
				if (this.isArray()) {
					for (var i = 0; i < this.count; i++) {
						this[i].dispatchEvent(type, {
							oldValue: data.oldValue && data.oldValue[i],
							newValue: data.newValue && data.newValue[i]
						});
					}
				}

				return ret;
			}
		},

		isValid: {
			get: function() {
				var v = this._value;
				if (v === undefined && !this.isDirty) {
					return true;
				}
				else if(v && this.format && this.format == "$IP" && !PB.validateIPaddress(v)) {
					return false;
				}
				else if(this.format) {
					return true;
				}
				else if(this.bitfield && !PB.validateBitfields(v, this)) {
					return false;
				}
				// Compare ltk Values because they are always Strings
				return PB.equals(v, toJSValue(this, toLTKValue(this, v)));
			}
		}
	});


	// SubParam is a type of Param, but it represents a single element of an Array Param
	var SubParam = function SubParam(param, index, ext) {
		PB.extend(this, param, ext);
		this.asAttribute = this.define + "[" + index + "]";
		this.debug = this.asAttribute;
		this.index = index;
		this.parent = param;
	};

	SubParam.prototype = Object.create(Param.prototype, {
		cacheValue: {
			get: function() {
				return this.parent.cacheValue && this.parent.cacheValue[this.index];
			},
			set: function(n) {
				if (this.parent.cacheValue) {
					this.parent.cacheValue[this.index] = n;
				}
			}
		},

		writeValue: {
			get: function() {
				return this.parent.writeValue && this.parent.writeValue[this.index];
			},
			set: function(n) {
				if (!this.parent.writeValue) {
					this.parent.writeValue = PB.copy(this.parent.cacheValue);
				}
				if (this.parent.writeValue) {
					n = toLTKValue(this.parent, n);
					this.parent.writeValue[this.index] = n;
				}
				this.parent.isDirty = true;
			}
		},
		read: {
			value: function() {
				var index = this.index;
				var parent = this.parent;
				return parent.read.apply(parent, arguments).then(function(arr) {
					return arr[index];
				});
			}
		},
		write: {
			value: function(value) {
				if (value) {
					this.writeValue = value;
				}
				return this.parent.write();
			}
		}
	});
	SubParam.prototype.constructor = Param;

	// //////////////////////////
	// Param Helper Functions //
	// //////////////////////////

	// Formats a value into a form used in the JS interface
	var toJSValue = function(param, value) {
		var typeFunction;
		switch (param.datatype) {
			case "string8":
				typeFunction = function(value) {
					// Strings will have \" surrounding them
					if (value.charAt(0) === "\"" && value.charAt(value.length - 1) === "\"") {
						return value.substr(1, value.length - 2);
					} else {
						return value;
					}
				};
				break;
			case "bool":
				typeFunction = function(value) {
					if (typeof(value) === "boolean") {
						return value;
					} else {
						return parseInt(value) ? true : false;
					}
				};
				break;
			case "float":
				typeFunction = function(value) {
					return customParseFloat(value);
				};
				break;
			case "dfloat":
				typeFunction = function(value) {
					return parseFloat(value);
				};
				break;
			case "byte":
			case "word":
			case "dword":
				typeFunction = function(value) {
					return value;
				};
				break;
			default:
				typeFunction = function(value) {
					return parseInt(value);
				};
				break;
		}

		// Handle arrays
		if (param.datatype === "string8" || !param.count || param.count === 1) {
			return typeFunction(value);
		} else {
			if (PB.isArray(value)) {
				console.log("Value inside handling arrays : "+value);
				value = value.join(",");
			}
			var ret = value.split(",").map(typeFunction);
			if (ret.length !== param.count) {
				if (!param.unexpectedCount) {
					console.warn("Unexpected count for %s - expected %d received %d (value=%s)", param.debug, param.count, ret.length, value);
					param.unexpectedCount = true;
				}
			}
			return ret;
		}
	};

	// Custom function to take Hexadecimal value as String and return float with 2 decimal points
	function customParseFloat(str) {	
		var float = 0, sign, order, mantiss,exp,
		int = 0, multi = 1;
		if("0x00000000" == str){
		return (0).toFixed(2).toString();
		}else{
		if (/^0x/.exec(str)) {
			int = parseInt(str,16);
		}else{
			for (var i = str.length -1; i >=0; i -= 1) {
				if (str.charCodeAt(i)>255) {
					console.log('Wrong string parametr'); 
					return false;
				}
				int += str.charCodeAt(i) * multi;
				multi *= 256;
			}
		}
		sign = (int>>>31)?-1:1;
		exp = (int >>> 23 & 0xff) - 127;
		mantiss = ((int & 0x7fffff) + 0x800000).toString(2);
		for (i=0; i<mantiss.length; i+=1){
			float += parseInt(mantiss[i])? Math.pow(2,exp):0;
			exp--;
		}
		return (float*sign).toFixed(2).toString();	
		}
	}
	
	// Formats a value into a form used in LTK backend
	//  Also handles an array of values
	var toLTKValue = function(param, value) {
		var typeFunction;

		// Set the type function
		switch (param.datatype) {
			case "string8":
				typeFunction = function(value) {
					if (value && param.count < value.length) {
						var oldValue = value;
						value = value.substr(0, param.count);
						console.warn(param.debug + " String value was truncated from " + oldValue + " to " + value);
					}
					return "\"" + value + "\"";
				};
				break;
			case "bool":
				typeFunction = function(value) {
					return value ? "1" : "0";
				};
				break;
			case "float":
				typeFunction = function(value) {
					var buffer = new ArrayBuffer(4);
					var intView = new Int32Array(buffer);
					var floatView = new Float32Array(buffer);
					floatView[0] = value;
					return "0x" + intView[0].toString(16);
				};
				break;
			case "dfloat":
				typeFunction = function(value) {
					var buffer = new ArrayBuffer(8);
					var intView = new Int32Array(buffer);
					var floatView = new Float64Array(buffer);
					floatView[0] = value;
					return "0x" + ((intView[0] * 0x100000000) + (intView[1])).toString(16);
				};
				break;
			case "byte":
			case "word":
			case "dword":
				typeFunction = function (value) {
					return value;
				};
				break;
			default:
				typeFunction = function(value) {
					var minMax = {
						"sint8": [-128, 127],
						"sint16": [-32768, 32767],
						"sint32": [-2147483648, 2147483647],
						"sint64": [-9223372036854775808, 9223372036854775807],
						"uint8": [0, 255],
						"uint16": [0, 65535],
						"uint32": [0, 4294967295],
						"uint64": [0, 18446744073709551615],
						"byte": [-128, 255],
						"word": [-32768, 65535],
						"dword": [-2147483648, 4294967295]
					}[param.datatype];
					var oldValue = parseInt(value);
					// Update from datatype limits
					value = Math.max(Math.min(oldValue, minMax[1]), minMax[0]);

					// Update from custom param limits
					if (param.min && value < param.min) {
						value = param.min;
					} else if (param.max && value > param.max) {
						value = param.max;
					}
					if (oldValue !== value) {
						console.warn("Out Of Bounds Warning: " + param.debug + " changed from " + oldValue + " to " + value);
					}
					return value;
				};
				break;
		}

		// Apply the type function the array, if appropriate
		if (PB.isArray(value)) {
			return value.map(typeFunction).join(",");
		} else {
			return typeFunction(value);
		}

	};


	// //////////////////
	// DCI Definition //
	// //////////////////
	PB.DCI = {};
	DCI.forEach(function(dciParam) {
		PB.DCI[dciParam.pid] = new Param(dciParam);
	});

	PB.DCI.get = function(name) {
		if (name instanceof Param) {
			return name;
		} else if (this[name]) {
			return this[name];
		}
		// Pieces = ["pid[index]", pid, index]
		var pieces = (/(\w+)(?:\[(\w+)\])?/).exec(name);
		name = pieces[1];
		var index = pieces[2];
		if (this[name]) {
			return this[name][index] || this[name];
		}

		for (var n in this) {
			if (this[n].define === name) {
				return this[n][index] || this[n];
			}
		}
	};

	PB.DCI.writeAll = function() {
		for (var n in PB.DCI) {
			if (PB.DCI[n] instanceof Param) {
				PB.DCI[n].write();
			}
		}
	};


	/**
	 * Add params array from any pid array
	 */
	var pidToParam = function(obj) {
		if (PB.isObject(obj)) {
			for (var n in obj) {
				if (obj[n] === undefined) {
					delete obj[n];
				} else {
					pidToParam(obj[n]);
				}
			}
		}
		if (obj.pid) {
			obj.params = [];
			for (var i = 0; i < obj.pid.length; i++) {
				obj.params[i] = PB.DCI[obj.pid[i]];
			}
		}
		return obj;
	};

	// Creates a new ASM list as part of PB
	// PB.WEBUI = pidToParam(WEBUI);
	// PB.WEBUI.forEach(function(nav) {
	//     for (var n in nav.tabs) {
	//         var t = nav.tabs[n];
	//         t.params = [];
	//         for (var m in t.groups) {
	//             t.params = t.params.concat(t.groups[m].params);
	//         }
	//     }
	// });
	// console.log(PB.WEBUI);

	// Creates a new ASM list as part of PB
	PB.ASM = pidToParam(ASM);

}());
