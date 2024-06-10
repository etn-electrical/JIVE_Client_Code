"use strict";
(function() {

	/**
	 * Helper function that identifies an event as being from a Param
	 */
	var isParamEvent = function(type) {
		for (var n in ENUM.EVENT.PARAM) {
			if (type === ENUM.EVENT.PARAM[n]) {
				return true;
			}
		}
		return false;
	};

	PB.behaviors = {
		/**
		 * Describes this element as a Tab
		 * Must also be a navElem
		 */
		tabElem: {
			properties: {
				tab: Object,
				isTab: {
					type: Boolean,
					value: true
				}
			},

			// Returns true if this tab is "active"
			// An "active" tab is one that is currently selected, regardless of modals
			isActive: function() {
				var tab = this.tab;
				return tab && tab === (tab.isModal() ? navbar.activeModal : navbar.activeTab);
			},

			created: function() {
				var self = this;

				// Add a listener that fires whenever the ACTIVE state changes
				var lastActive = false;
				this._goListener = function() {
					var isActive = self.isActive();
					if (lastActive !== isActive) {
						self.dispatchEvent(ENUM.EVENT.TAB.ACTIVE_CHANGED);
						lastActive = isActive;
					}
				}
			},

			attached: function() {
				this.tab = this.parentElement.tab;
				navbar.addEventListener(ENUM.EVENT.GO, this._goListener);

				// Since the tab was created because of the navigation, the active state is already been changed
				// Send the update manually
				this._goListener();
			},

			detached: function() {
				navbar.removeEventListener(ENUM.EVENT.GO, this._goListener);
				clearTimeout(this._updateParamTimeout);
			}


		},

		/**
		 * NavElem
		 * Basic behavior provided to all navElem-containing elements
		 * - param = navElem.param     // If navElem contains a param, it is promoted to "param" and a special paramListener is added
		 * - sidebar = navElem.sidebar // If navElem becomes a child of a navElem with a sidebar, that sidebar becomes available
		 *
		 * Also adds default listeners that can be overridden:
		 *   childrenAttached - Fired exactly once, the first time children are attached and their params are available
		 *   onChildren - Fired each time any of the children change
		 */
		navElem: {
			properties: {
				navParent: Object,

				isNavElem: {
					type: Boolean,
					value: true
				},
				param: {
					type: Object,
					reflectToAttribute: true
				},
				paramList: {
					type: Array
				},
				eventTarget: Object

				// childrenAttached: Function,
				// onChildren: Function
			},

			/**
			 * Returns an array of all params found in this element and its children
			 */
			getParams: function() {
				// Returns an array of params used in this NavElem and all its children
				//  Returned value is cached and will retain all changes.
				if (!this.params) {
					var params = [];
					if (this.param) {
						// This NavElem is a Pelem (and thus only has one param)
						params.push(PB.DCI.get(this.param));
					}
					if (this.paramList) {
						params = params.concat(this.paramList);
					}

					var children = this.getEffectiveChildren();
					if (children) {
						while (children.length) {
							var child = children.shift();
							if (child.getParams) {
								params = params.concat(child.getParams());
							} else if (child.children) {
								children = children.concat(child.children);
							}
						}
					}
					this.params = params;
				}
				return this.params;
			},

			/**
			 * Convenience Function to get a specific effectiveChild property in a template
			 * If an object is found in the path, it is ignored. Useful for forcing getChildProperty to update
			 */
			getChildProperty: function() {
				var ret = this.getEffectiveChildren();
				for (var i = 0; i < arguments.length; i++) {
					var arg = arguments[i];
					if (PB.isObject(arg)) {
						continue;
					}
					if (ret === undefined) {
						break;
					}
					ret = ret[arguments[i]];
				}
				return ret;
			},

			getSidebar: function() {
				return (this.navSidebar || this.navParent && this.navParent.getSidebar());
			},

			getTab: function() {
				return (this.isTab && this) || (this.navParent && this.navParent.getTab());
			},

			addEventListener: function(type, listener) {
				if (isParamEvent(type)) {
					var params = this.getParams();
					for (var i = params.length - 1; i >= 0; i--) {
						if (params[i].addEventListener) {
							params[i].addEventListener(type, listener);
						}
					}
				}
				return this.eventTarget.addEventListener(type, listener);
			},

			removeEventListener: function(type, listener) {
				if (isParamEvent(type)) {
					var params = this.getParams();
					for (var i = params.length - 1; i >= 0; i--) {
						if (params[i].removeEventListener) {
							params[i].removeEventListener(type, listener);
						}
					}
				}
				return this.eventTarget.removeEventListener(type, listener);
			},

			dispatchEvent: function(type, e) {
				// return this.debounce(type, this.eventTarget.dispatchEvent.bind(this,type, e));
				return this.eventTarget.dispatchEvent(type, e);
			},

			// addParam adds one or more parameters to the element, allowing them to react to navigation event listeners,
			// such as isValid, etc.
			addParam: function() {
				var params = arguments;
				if (!params.length) {
					return;
				}
				if (!this.paramList) {
					this.paramList = [];
				}
				for (var i = 0; i < params.length; i++) {
					this.paramList.push(params[i]);
					for (var type in this.eventTarget.listeners) {
						if (isParamEvent(type)) {
							var listeners = this.eventTarget.listeners[type];
							for (var j = listeners.length - 1; j >= 0; j--) {
								params[i].addEventListener(type, listeners[j]);
							}
						}
					}
				}
				this.dispatchEvent(ENUM.EVENT.PARAMS_LIST_CHANGED, this.getParams());
			},

			removeParam: function() {
				var params = arguments;
				if (!params.length) {
					return;
				}
				for (var i = 0; i < params.length; i++) {
					var index = this.paramList.indexOf(params[i]);
					if (index < 0) {
						continue;
					}
					this.paramList.splice(index, 1);
					for (var type in this.eventTarget.listeners) {
						if (isParamEvent(type)) {
							var listeners = this.eventTarget.listeners[type];
							for (var j = listeners.length - 1; j >= 0; j--) {
								params[i].removeEventListener(type, listeners[j]);
							}
						}
					}
				}
				this.dispatchEvent(ENUM.EVENT.PARAMS_LIST_CHANGED, this.getParams());
			},

			created: function() {
				this.eventTarget = new PB.EventTarget();

				// Fire special event listeners "childrenAttached" and "onChildren";
				var firstOnly;
				var fireOnChildren = function() {
					if (this.getEffectiveChildren().length) {
						if (!firstOnly) {
							firstOnly = true;
							if (this.childrenAttached) {
								this.childrenAttached.apply(this);
							}
						}
					}
					if (firstOnly && this.onChildren) {
						this.onChildren.apply(this);
					}
				}.bind(this);

				Polymer.dom(this).observeNodes(function(info) {
					info.target.dispatchEvent(ENUM.EVENT.PARAMS_LIST_CHANGED, info);

					var self = info.target;
					setTimeout(fireOnChildren);

				});

				var self = this;
				this.addEventListener(ENUM.EVENT.PARAMS_LIST_CHANGED, function(type, e) {
					// Clear the list of params so it will be recalculated
					delete self.params;

					// Re-apply listeners intended for all child params
					for (var n in ENUM.EVENT.PARAM) {
						var t = ENUM.EVENT.PARAM[n];
						var oldList = PB.copy(self.eventTarget.listeners[n]);
						for (var m in oldList) {
							var list = oldList[m];
							self.removeEventListener(t, list);
							self.addEventListener(t, list);
						}
					}

					// Update getChildProperty
					self.notifyPath("getChildProperty");

					// Echo the event to our parent
					if (self.navParent) {
						self.navParent.dispatchEvent(type, e);
					}

				});
			},

			attached: function() {
				// Define our navParent
				// 	 That is the nearest parent that is also a navElem
				var parent = Polymer.dom(this).parentNode;
				while (parent && !parent.isNavElem) {
					parent = Polymer.dom(parent).parentNode;
				}
				if (parent && parent.isNavElem) {
					this.navParent = parent;
				}


				// Attach ourselves to our parent if needed
				if (this.navParent && this.getParams().length) {
					this.navParent.dispatchEvent(ENUM.EVENT.PARAMS_LIST_CHANGED, {
						target: this
					});
				}

				this._loginListener = function() {
					return this.dispatchEvent.apply(this, arguments);
				}.bind(this);
				PB.currentUser.addEventListener(ENUM.EVENT.LOGIN, this._loginListener);

				if (this.param) {
					var param = PB.DCI.get(this.param);
					if (param) {
						this.set("param", param);
						this.set("label", param.name);
					}
				}
			},

			detached: function() {
				for (var type in this.eventTarget.listeners) {
					var listeners = this.eventTarget.listeners[type];
					for (var i = listeners.length - 1; i >= 0; i--) {
						this.removeEventListener(listeners[i]);
					}
				}
				PB.currentUser.removeEventListener(ENUM.EVENT.LOGIN, this._loginListener);
			},
		},

		param: {
			properties: {
				param: {
					type: Object,
					reflectToAttribute: true,
					observer: '_myParamListener'
				}
			},

			_myParamListener: function(newParam) {
				// Since the Param has changed, event listeners attached to the old param are no longer valid
				//  and need to be attached to the new param
				//  Fortunately, navElem does this automatically with this function
				var param = PB.DCI.get(newParam);
				if (param !== this.param) {
					this.set("param", param);
				} else {
					this.dispatchEvent(ENUM.EVENT.PARAMS_LIST_CHANGED, {
						target: this
					});
				}
			},

			created: function() {
				var self = this;
				this.paramListener = function(type, e) {
					self.notifyPath("param.value", self.param.value);
					self.notifyPath("param.isValid", self.param.isValid);
					self.notifyPath("param.saveFailed", self.param.saveFailed);
					self.notifyPath("param.saveFailureMsg", self.param.saveFailureMsg);
				};
			},

			ready: function() {
				// addEventListener forwards the listener to this.param
				this.addEventListener(ENUM.EVENT.PARAM.CHANGE, this.paramListener); // works in Chrome
			},

			attached: function() {
				// addEventListener forwards the listener to this.param
				this.addEventListener(ENUM.EVENT.PARAM.CHANGE, this.paramListener); // works in IE and Firefox
			},

			detached: function() {
				this.removeEventListener(ENUM.EVENT.PARAM.CHANGE, this.paramListener);
			},

			/**
			 * serialize is overridden to prevent the entire Param object from appearing in the HTML
			 */
			serialize: function(value) {
				if (value instanceof PB.Param) {
					return value.asAttribute;
				}
				return Polymer.Base.serialize(value);
			}

		},

		/**
		 * Disabled
		 * Provides the object with a "disabled" attribute which is passed down to its effective children.
		 */
		disabled: {
			properties: {
				disableEdit: {
					type: Boolean,
					value: false,
					notify: true,
					reflectToAttribute: true,
					observer: '_disabled-changed'
				}
			},

			"_disabled-changed": function(newValue, oldValue) {
				// Set disabled on all effective children
				var children = this.getEffectiveChildren();

				// Set disabled on sidebar (special case; only used in tabs with sidebars)
				if (this.navSidebar) {
					//children = children.concat(this.navSidebar.getEffectiveChildren());
					if (this.navSidebar.querySelector('px-sidebar')){
						children = children.concat(this.navSidebar.querySelector('px-sidebar').getEffectiveChildren());
					}

					if (newValue) {
						this.navSidebar.close();
					}
					
				}

				for (var n in children) {
					var c = children[n];
					if (c && c.set) {
						c.set("disableEdit", newValue);
					}
				}
			},

			ready: function() {
				Polymer.dom(this).observeNodes(function() {
					this["_disabled-changed"].call(this, this.disableEdit);
				}.bind(this));
			},

			attached: function() {
				if (this.navParent) {
					this.set("disableEdit", this.navParent.disableEdit);
				}
			}
		},

		/**
		 * Distribute
		 * Creates a property "distChildren" which is an array of indexes of children, i.e. [0, 1, 2, 3]
		 * This array can be repeated on to dynamically redistribute children:
		 *
		 * <template is="dom-repeat" items="[[distChildren]]">
		 *     <content select='.item[[item]]'></content>
		 * </template>
		 * 
		 * This is done by adding 'itemN' to the Light DOM children, and so is not nestable.
		 * Note distChildren does NOT include any sidebar children, which makes for easier selections
		 *
		 * Child elements can be gotten normally using self.getEffectiveChildren()
		 */
		distribute: {
			properties: {
				distChildren: Array,
			},
			created: function() {
				Polymer.dom(this).observeNodes(function(info) {
					var self = info.target;
					var children = self.getEffectiveChildren();
					var distChildren = [];
					for (var i = 0; i < children.length; i++) {
						var child = children[i];
						if (!PB.isSidebar(child) && !PB.isAction(child)) {
							distChildren.push(i);
						}
						for (var j = 0; j < children.length; j++) {
							Polymer.Base.toggleClass("item" + j, i === j, child);
						}
					}
					self.distChildren = distChildren;
				});
			}
		},

		/**
		 * UpdateParam
		 * Updates all the params on this navElem at a regular interval
		 */
		updateParam: {
			properties: {
				updateInterval: {
					type: Number,
					value: undefined,
					observer: "_updateInterval",
					reflectToAttribute: true
				}
			},

			created: function() {
				// Start a timer that will automatically read all the values in this page
				var updateMap = {};
				var self = this;
				var doUpdate = false;
				var update = function() {
					// Clear pending updates
					// 	This is important to update immediately when updateInterval is changed
					clearTimeout(self._updateParamTimeout);

					if (!self.updateInterval || !doUpdate || !self.disableEdit) {
						// Don't update if updateInterval isn't defined, or is 0.
						return;
					}
					else {
						// This tab should continue to update, even if it isn't the active tab
						//  This occurs, for example, if the user opens a modal
						//  The timer is stopped in the detached function
						var params = self.getParams();
						var paramCounter = 0;

						var repeatLoop = function () {
							if (paramCounter == params.length) {
								self._updateParamTimeout = setTimeout(update, self.updateInterval);
							} else {
								var param = params[paramCounter];
								// Pointless to update if the user has changed it
								if (param.isDirty) {
									paramCounter += 1;
									repeatLoop();
								} else {
									param.read(true).then(function () {
										paramCounter += 1;
										repeatLoop();
									}, function (error) {
										paramCounter += 1;
										repeatLoop();
									});
								}	
							}
						}
						repeatLoop();				
					}
				};
				this._updateInterval = update;

				this.addEventListener(ENUM.EVENT.TAB.ACTIVE_CHANGED, function() {
					doUpdate = self.getTab().isActive();
					if (doUpdate) {
						update();
					}
				});
			},
		},

		/**
		 * EditSave
		 * Provides the capacity to edit and save on a particular page.
		 * - Provides warning when editing is disabled before save
		 * - Provides warning when navigating away
		 * - Locks navbar when editing
		 * - Updates status bar when any param is dirty
		 */
		editSave: {
			properties: {
				canEdit: {
					type: Boolean,
					value: false
				},
				canSave: {
					type: Boolean,
					value: true,
				},
				editing: {
					type: Boolean,
					value: false,
					notify: true,
					observer: '_editing-changed'
				},

				lockIcon: {
					type: String
				},

				editClass: {
					type: String
				}
			},

			"_editing-changed": function(editing) {
				this._editSaveParamListener();
				if (editing) {
					navbar.lock(this.edit.bind(this));
				} else {
					navbar.unlock();
				}
				this.set("disableEdit", !editing);
				this.set("lockIcon", editing ? "px-icons:unlock" : "px-icons:lock");
				this.set("editClass", editing ? "edit" : "");
			},
			// Toggles the editing state of the tab
			edit: function() {
				if (!this.canEdit) {
					return;
				}
				if (this.editing) {
					// Determine if any params on this page are dirty
					var isDirty = false;
					var params = this.getParams();
					for (var i = params.length - 1; i >= 0; i--) {
						if (params[i].isDirty) {
							isDirty = true;
							console.log('dirty');
							break;
						}
					}
					// If this page isn't dirty, exit quietly
					if (!isDirty) {
						this.set("editing", false);
						this.dispatchEvent(ENUM.EVENT.TAB.ACTIVE_CHANGED);
						return true;
					}
					// The page is dirty, ask user to confirm losing changes
					var self = this;
					return navbar.showConfirm("Unsaved changes detected. All changes will be lost.", "Unsaved Changes", ["Cancel", "Ignore Changes"])
						.then(function(result) {
							switch (result) {
								case 0: // Cancel - do nothing
									break;
								case 1: // Ignore Changes - revert all params and stop editing
									for (var i = self.params.length - 1; i >= 0; i--) {
										self.params[i].revert();
									}
									self.set("editing", false);
									self.dispatchEvent(ENUM.EVENT.TAB.ACTIVE_CHANGED);
									break;
							}
						});
				} else {
					// Turn on editing
					this.set("editing", true);
					return false;
				}
			},
			// Save all the parameters on this page and turn off editing
			save: function() {
				var self = this;
				if (!this.canSave) {
					return;
				}
				// Write each parameter
				var proms = [];
				this.getParams().forEach(function(param) {
					if (param.isDirty) {
						proms.push(param.write().then(PB.noop, function() {
							console.warn("Failed to write " + this.debug, this);
						}.bind(param)));
					}
				});

				var self = this;
				navbar.showProgress(ENUM.STATUS.SAVED, "Saving parameters", proms).then(function() {
					navbar.setStatus(ENUM.STATUS.SAVED, "Save Successful", 4000);
					self.set("editing", false); // Turn off editing
					self.dispatchEvent(ENUM.EVENT.TAB.ACTIVE_CHANGED);
				}, function() {
					self.set("editing", true);
					navbar.setStatus(ENUM.STATUS.SAVED, "Save Failed!", 8000);
				});
			},
			created: function() {
				var self = this;
				this._editSaveParamListener = function(type, e) {
					var params = self.getParams();
					var i;
					if (!params) {
						return;
					}
					if (self.isTab) {
						var status;
						for (i = 0; i < params.length; i++) {
							if (params[i].isDirty) {
								status = "Unsaved changes detected";
								break;
							}
						}
						navbar.setStatus(ENUM.STATUS.UNSAVED_CHANGES, status);
					}
					var isValid = true;
					for (i = 0; i < params.length; i++) {
						if (!params[i].isValid) {
							console.log("Invalid Param: ", params[i]);
							isValid = false;
							break;
						}
					}
					self.set("canSave", isValid);
				};
			},
			attached: function() {
				this.addEventListener(ENUM.EVENT.PARAM.CHANGE, this._editSaveParamListener);
			},
			detached: function() {
				this.removeEventListener(ENUM.EVENT.PARAM.CHANGE, this._editSaveParamListener);
			}
		},

		/**
		 * SidebarOwner
		 * Adds behavior to this object's sidebar, and adds the sidebar to the navElem
		 * 
		 * sidebar.select(
		 * )
		 */
		sidebarOwner: function(sidebarSelector) {
			return {
				properties: {
					hideSidebar: {
						type: Boolean,
						value: true
					}
				},
				ready: function() {
					console.log('sidebarSelector', this.$[sidebarSelector]);
					this.navSidebar = this.$[sidebarSelector];
					this.navSidebar.testTap = function() {
						console.log('navSidebar');
					};
					var self = this;
					var sidebar = this.navSidebar;
					// Last selected value
					var lastElem;
					var lastData;
					var lastOnClose;
					// Current state
					var promise;
					var currentElement;
					//
					//  elem - The name of the element that should be created
					//  data - Values that should set on the resulting element


					// Helper functions
					var canClose = function() {
						return !(lastOnClose && !lastOnClose.apply(this, arguments));
					};

					/**
					 * Opens a component into the sidebar.
					 * @param  {Object} elem The sidebar element to add to the sidebar (e.g. sidebar-default)
					 * @param  {Object} data Values that should set on the resulting element
					 * @param  {Function} onClose Called when something attempts to close the sidebar. If false is returned, the new selection will be cancelled
					 * @return {Promise}      Resolves when closed or rejected if the selection was cancelled by the previous selection
					 */

					sidebar.open = function(elem, data, onClose) {
						var ret = PB.defer();
						var clean = function clean(node) {
							for (var n = 0; n < node.childNodes.length; n++) {
								var child = node.childNodes[n];
								if (
									child.nodeType === 8 ||
									(child.nodeType === 3 && !/\S/.test(child.nodeValue))
								) {
									node.removeChild(child);
									n--;
								} else if (child.nodeType === 1) {
									clean(child);
								}
							}
						}

						// Swap data and onClose if data was omitted
						if (PB.isFunction(data)) {
							var x = onClose;
							onClose = data;
							data = x;
						}

						// Do nothing if the selection is the same
						if (elem === lastElem) {
							if (!PB.equals(data, lastData)) {
								// Call lastOnClose to ensure we can close
								if (!canClose()) {
									ret.reject();
									return ret;
								}
								// Selection is only changing properties of the current Element
								//  Change those properties and leave
								//setData(data);
								PB.extend(lastData, data);
							} else {
								// Identical selection and/or nothing to edit
								ret.reject();
								return ret;
							}
						} else if (!canClose()) {
							// Call lastOnClose to ensure we can close						
							ret.reject();
							return ret;
						}

						sidebar.close();

						// Update the current selection info
						lastElem = elem;
						lastData = data;
						lastOnClose = onClose;

						// Create the selected element, if something is being selected
						if (elem) {
							console.log('sidebar', Polymer.dom(sidebar));
							Polymer.dom(sidebar).appendChild(elem.root);
							promise = PB.defer();
							self.set("hideSidebar", false);
						}
						return promise;
					};

					/**
					 * Attempts to close the sidebar, calling the lastOnClose function first
					 */
					sidebar.close = function() {
						self.set("hideSidebar", true);

						// Call lastOnClose to ensure we can deselect
						if (lastOnClose && !lastOnClose.apply(this, arguments)) {
							return;
						}

						// Resolve the promise
						if (promise) {
							promise.resolve();
							promise = undefined;
						}

						// Remove the element
						if (currentElement) {
							Polymer.dom(sidebar).innerHTML = '';
							currentElement = undefined;							
						}

						//Removes sidebar content
						var sidebarContent = Polymer.dom(this).children[0];
						if(sidebarContent){
							Polymer.dom(this).removeChild(sidebarContent);
						}

						lastElem = undefined;
						lastData = undefined;
						lastOnClose = undefined;
					};

					/**
					 * Returns true if the sidebar has content
					 */
					sidebar.isOpen = function() {
						return !!currentElement;
					};
				}
			};
		},

		/**
		 * Selectable Sidebar
		 * This object is selectable and shows a sidebar when selected.
		 * Automatically detects a sidebar among its decendents; the first sidebar found is the one shown when selected.
		 */
		selectableSidebar: {
			properties: {
				sidebar: Object, // A reference to the sidebar to be shown
				selectable: Boolean, // True if a sidebar exists and this object is selectable
				selected: Boolean // True when this object's sidebar is showing
			},
			created: function() {
				Polymer.dom(this).observeNodes(function(info) {
					var self = info.target;
					var children = [self.getEffectiveChildren()];
					while (children.length) {
						var c = children.pop();
						for (var i = 0; i < c.length; i++) {
							if (PB.isSidebar(c[i])) {
								self.set("sidebar", c[i]);
								self.set("selectable", true);
								return;
							}
							children.push(Polymer.Base.getEffectiveChildren.apply(c[i]));
						}
					}
					self.set("sidebar", undefined);
					self.set("selectable", false);
				});
			},
			select: function(e) {
				if (!this.selectable) {
					return;
				} else if (this.selected) {
					this.getSidebar().close();
					return;
				}
				var promise = this.getSidebar().open(this.sidebar);

				this.set('selected', true);

				promise.always(function() {
					this.set('selected', false);
				}.bind(this));
			}

		},

		/**
		 * Indicates the section is an action button that should appear in the action bar.
		 */
		action: {
			properties: {
				right: {
					type: Boolean,
					value: true,
					reflectToAttribute: true
				}
			},
			created: function() {
				this.classList.add("action");
				this._hideAction = this._hideAction.bind(this);
			},
			_hideAction: function() {
				if (this.showAction) {
					this.style.display = this.showAction() ? "" : "none";
				}
			},
			attached: function() {
				PB.currentUser.addEventListener(ENUM.EVENT.LOGIN, this._hideAction);
				this._hideAction();
			},
			detached: function() {
				PB.currentUser.removeEventListener(ENUM.EVENT.LOGIN, this._hideAction);
			}

		}

	};
})();