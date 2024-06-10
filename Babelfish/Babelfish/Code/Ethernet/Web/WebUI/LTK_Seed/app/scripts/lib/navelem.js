(function() {
	"use strict";

	// ///////////
	// NavElem //
	// ///////////
	// NavElem is the parent class of TabBar, Tab, Section, Group, and Pelem
	//      Note that TabBar and Tab override many of the base properties
	var NavElem = function NavElem(parent, name, layout, icon, hashtag) {
		this.parent = parent;
		this.name = name;
		this.layout = layout;
		this.icon = icon;
		this.hashtag = hashtag;
		this.children = [];

		// Handles the case where name is actually a Param
		if (PB.isNumber(name)) {
			this.param = PB.DCI.get(name);
			this.name = this.param.name;
		}
	};

	Object.defineProperties(NavElem.prototype, {
		// Used to determine the element that is created
		createObject: {
			get: function() {
				return NavElem;
			}
		},

		createElement: {
			value: function(ext) {
				var ret = Polymer.Base.create(this.layout, PB.extend({
					navElem: this
				}, ext));
				return ret;
			}
		},

		// Used to get the parent Tab 
		tab: {
			get: function() {
				return this.parent.tab;
			}
		},

		// Returns true if this element is a tab
		isTab: {
			value: function() {
				return this.tab === this;
			}
		},

		// Used to get the parent Tab's Sidebar
		sidebar: {
			get: function() {
				return this.tab.sidebar;
			}
		},

		// Creates a child NavElem in this NavElem
		create: {
			value: function(name, layout, icon, hashtag) {
				var ret = new this.createObject(this, name, layout, icon, hashtag);
				this.children.push(ret);
				return ret;
			}
		},

		// Go applies to the parent, since the base NavElem doesn't understand "go"
		go: {
			value: function() {
				this.parent.go.apply(this.parent, arguments);
			},
			writable: true
		},

		// Aliases for children
		sections: {
			get: function() {
				return this.children;
			}
		},
		groups: {
			get: function() {
				return this.children;
			}
		},

		// Hacky convenience function to determine if a pelem is an input
		isInput: {
			get: function() {
				return this.layout === "param-input";
			}
		},

		getParams: {
			value: function() {
				// Returns an array of params used in this NavElem and all its children
				//  Returned value is cached and will retain all changes.
				if (!this.params) {
					if (this.param) {
						// This NavElem is a Pelem (and thus only has one param)
						this.params = [this.param];
						if (this.param.userParam) {
							this.params.push(this.param.userParam);
							this.params.push(this.param.passParam1);
							this.params.push(this.param.passParam2);
						}
					} else {
						var j = 0;
						this.params = [];
						for (var i = this.children.length - 1; i >= 0; i--) {
							var pArr = this.children[i].getParams();
							for (var k = pArr.length - 1; k >= 0; k--) {
								var p = pArr[k];
								if (this.params.indexOf(p) === -1) {
									this.params[j++] = p;
								}
							}
						}
					}
				}
				return this.params;
			}
		}

	});

	// /////////////////////
	// TabBar Definition //
	// /////////////////////
	var TabBar = function TabBar() {
		NavElem.apply(this, arguments);

		// The active tab when navigating to this tabbar
		this.tab = undefined;

		// Sets the coloration of this tab
		this.isApp = (this.layout === "nav-app") ? true : false;

		// Navigate to this TabBar's active tab
		//  Uses the first tab available if no active tab
		this.go = function() {
			if (!this.tab) {
				return this.children[0].go();
			} else {
				return this.tab.go();
			}
		}.bind(this);
	};

	TabBar.prototype = Object.create(NavElem.prototype);

	Object.defineProperties(TabBar.prototype, {
		// Used to determine the element that is created
		createObject: {
			get: function() {
				return Tab;
			}
		},

		// Each TabBar has its own View Container
		getViewContainer: {
			value: function() {
				if (this.hashtag === "modal") {
					return document.querySelector("div[hashtag=\"" + this.hashtag + "\"]").firstChild;
				} else {
					return document.querySelector("div[hashtag=\"" + this.hashtag + "\"]");
				}
			}
		},

		tab: {
			value: undefined,
			writable: true
		}
	});

	// //////////////////
	// Tab Definition //
	// //////////////////
	var Tab = function Tab() {
		NavElem.apply(this, arguments);

		// Add function to go to this tab
		this.go = go.bind(this, this);

	};

	Tab.prototype = Object.create(NavElem.prototype);

	Object.defineProperties(Tab.prototype, {

		// Property that returns the TabBar owner of this Tab
		tabBar: {
			get: function() {
				if (this._tabBar) {
					return this._tabBar;
				}

				for (var i = 0; i < navbar.length; i++) {
					if (navbar[i].children.indexOf(this) >= 0) {
						this._tabBar = navbar[i];
						return navbar[i];
					}
				}
				this._tabBar = navbar.modals;
				return navbar.modals;
			},

			set: function(tabBar) {
				this._tabBar = tabBar;
			}
		},

		// Overrides NavElem.tab: returns this as the tab
		tab: {
			get: function() {
				return this;
			}
		},

		sidebar: {
			value: undefined,
			writable: true
		},

		isModal: {
			value: function() {
				return this.tabBar === navbar.modals;
			}
		}
	});


	// ////////////////////////////////////
	// Navbar / Tabbar / Tab navigation //
	// ////////////////////////////////////

	// All TabBars are in navbar
	var navbar = window.navbar = [];

	// Populate navbar Event handling
	var props = Object.getOwnPropertyNames(PB.EventTarget.prototype);
	for (var n in props) {
		var name = props[n];
		Object.defineProperty(navbar, name, Object.getOwnPropertyDescriptor(PB.EventTarget.prototype, name));
	}

	// Locks the navbar, preventing navigation (except for modals)
	var lockFunction;
	navbar.lock = function(func) {
		lockFunction = func || true;
	};
	navbar.isLocked = function() {
		return !!lockFunction;
	};
	navbar.unlock = function() {
		lockFunction = undefined;
	};

	// The index.html main template; set after DOM is created
	var main;

	navbar.create = function(name, layout, icon, hashtag) {
		var tabBar = new TabBar(this, name, layout, icon, hashtag);
		this.unshift(tabBar);
		return tabBar;
	};

	// All other "tabs" will appear in the modals tabbar
	navbar.modals = new TabBar(navbar, "modal", undefined, undefined, "modal");


	// This function navigates to a specific tab and shows the view
	//  Also accepts a hashtag representation of the tab
	//  
	//  The tab is bound to the view, so parameters may be passed via the tab
	var go = function(newTab, data) {

		//Remove the view-container child element while switching between TabBars (Settings, Seed User Interface)
		if (navbar.activeTab && navbar.activeTab.tabBar.hashtag != newTab.tabBar.hashtag && !newTab.isModal()) {
			var selector = "div[hashtag=\"" + navbar.activeTab.tabBar.hashtag + "\"]";
			var elem = document.querySelector(selector);
			elem.removeChild(elem.firstChild);
			navbar.activeTab = undefined;
		}


		// The currently active tab that we're switching from
		var oldActiveTab = navbar.activeTab;

		// Returns if the active tab is this tab already
		if (newTab === oldActiveTab && !data) {
			return;
		}


		// Let's try to unload the old tab and load the new one
		//  Because views are divided by tabBars, we only need to unload the one in the new tab's tabBar

		// The new tab's tabBar
		var newTabBar = newTab.tabBar;

		// The tabBar's old active tab
		var newBarOldTab = newTabBar.tab;

		// Replace the active tab in the tabbar
		if (newBarOldTab !== newTab) {
			newTabBar.tab = newTab;
		}

		// Set the active tab in the main document
		var q = navbar.waitForMain().then(function() {
			return main.setActiveTab(newTab);
		});

		// Set the hash
		if (newTab.hashtag) {
			location.hash = newTab.hashtag;
		}

		// If we're switching between tabbars, then there is no need to reload anything
		if (!(!data && oldActiveTab && newTabBar !== oldActiveTab.tabBar && newBarOldTab === newTab) || newTab.name.toLowerCase() === "time" ) {
			q = q.then(function(view) {


				// var elem = document.createElement(newTab.layout);
				// elem.tab = newTab;
				var elem = newTab.createElement({
					tab: newTab,
					data: data
				});

				if (newTab.isModal()) {
					view.appendChild(elem);

					// Modals only clear when their q resolves
					newTab.promise = PB.defer();
					newTab.promise.always(function() {
						view.removeChild(elem);
						main.removeActiveTab(newTab);
						if (navbar.activeModal === newTab) {
							navbar.activeModal = undefined;
						}
					});
					navbar.activeModal = newTab;
					navbar.dispatchEvent(ENUM.EVENT.GO);
					return newTab.promise;
				} else {
					// Clear the tabBar's view
					while (view.firstChild) {
						view.removeChild(view.firstChild);
					}

					navbar.activeTab = newTab;
					navbar.activeTabBar = newTab.parent;
					navbar.dispatchEvent(ENUM.EVENT.GO);
					view.appendChild(elem);
				}

			});
			return q;
		}

		// Set the active tab in navbar and issue the "GO" event
		q.then(function() {
			if (newTab.isModal()) {
				navbar.activeModal = newTab;
			} else {
				navbar.activeTab = newTab;
				navbar.activeTabBar = newTab.parent;
			}
			navbar.dispatchEvent(ENUM.EVENT.GO);
		});

		return q;
	};

	// Function that "goes" to the tab indicated by the current hashtag
	//  tabBar will handle forwarding if needed
	var onhashchange = function() {
		try {
			var hash = location.hash;
			if (main.activeTab && hash === main.activeTab.hashtag) {
				return;
			}

			// Search for the location
			for (var i = 0; i < navbar.length; i++) {
				var tabBar = navbar[i];

				// Try the tabBar itself first
				if (tabBar.hashtag === hash) {
					tabBar.go();
					return;
				}

				// Try the Tabs in the TabBar
				for (var j = 0; j < tabBar.children.length; j++) {
					if (tabBar.children[j].hashtag === hash) {
						tabBar.children[j].go();
						return;
					}
				}
			}

			// Location hash couldn't be found, load a default state
			navbar[navbar.length - 1].go();
		} catch (ex) {

		}
	};

	// Handle tab switching when click "back" or "forward"
	window.addEventListener("hashchange", onhashchange);


	// //////////////////
	// Misc functions //
	// //////////////////
	var statusMap = {};
	var statusTimer = {};

	// Sets a status message by its id for an amount of time.
	// If milliseconds is omitted, the message is shown indefinitely
	navbar.setStatus = function(id, message, milliseconds) {
		if (message) {
			statusMap[id] = message;
		} else {
			delete statusMap[id];
		}
		main.status = navbar.getStatus();
		if (statusTimer[id]) {
			clearTimeout(statusTimer[id]);
			statusTimer[id] = undefined;
		}
		if (milliseconds) {
			statusTimer[id] = setTimeout(function() {
				if (navbar.getStatus(id) === message) {
					navbar.setStatus(id);
				}
			}, milliseconds);
		}
	};

	// Gets a status message by its id.
	// If no id is given, the message with the highest id (most important) is returned.
	navbar.getStatus = function(id) {
		if (id !== undefined) {
			return statusMap[id];
		}

		var highestId;
		for (var n in statusMap) {
			if (highestId === undefined || n > highestId) {
				highestId = n;
			}
		}
		return statusMap[highestId];
	};

	// Shows a status progress bar based on a number of promises that resolve
	// Clears the message when complete
	// Returns PB.all(promises)
	navbar.showProgress = function(id, message, promises) {
		var progress = 0;
		var display = function() {
			if (progress >= 0) {
				progress++;
				navbar.setStatus(id, message + " " + progress + "/" + promises.length);
			}
		};
		var proms = promises.map(function(p) {
			return p.then(display);
		});

		return PB.all(proms).always(function() {
			progress = -1;
			navbar.setStatus(id);
		});
	};

	var mainQ = PB.defer();
	navbar.waitForMain = function() {
		if (main) {
			var q = PB.defer();
			q.resolve(main);
			return q;
		} else {
			return mainQ;
		}
	};



	// Start the timer after the navbar.elem is loaded
	var loadWhenReadyCount = 0;
	var loadWhenReady = function() {
		// This function gets called twice; ignore the first time
		if (++loadWhenReadyCount < 2) {
			return;
		}

		// Setup properties of main on index.html
		main = document.querySelector("#main");
		main.navbar = navbar;
		main.modals = [];

		// Calls the navbar's edit time function
		main.editTime = navbar.editTime;

		// Login shows the login modal
		main.login = navbar.modals.login.go;

		main.isLogo = "";

		// Go is a shortcut to calling go on the model item
		main.go = function(e) {
			if (e.model.tab) {
				e.model.tab.go();
			} else {
				e.model.tabbar.go();
			}
		};

		// Returns "active" or undefined depending on if the two values are equal
		main.isActive = function(active, tabbar) {
			return active === tabbar ? "active" : "";
		};

		main.removeActiveTab = function(tab) {
			var index = main.modals.indexOf(tab);
			if (index >= 0) {
				main.modals.splice(index, 1);
			} else {
				console.error("Cannot remove active tab ", tab);
			}
			if (main.modals.length === 0) {
				main.modal = false;
				main.mask = "mask";
			}
		};

		// Sets the active tab or modal, updating everything as needed
		//  Returns a promise that evaluates when the Tabbar's view container is ready to be populated
		main.setActiveTab = function(tab) {
			if (!(tab instanceof Tab)) {
				console.error("Cannot set Active Tab to a non-tab");
			}

			var q = PB.defer();
			var selector;
			if (tab.isModal()) {
				main.modals.push(tab);
				main.modal = true;
				main.mask = "mask visible";
				selector = "div[hashtag=\"modal\"]";
			} else {
				// Set the active tabBar, but only if it isn't modals
				this.activeTab = tab;
				this.activeTabBar = tab.parent;
				selector = "div[hashtag=\"" + tab.parent.hashtag + "\"]";
			}

			var waitForContainer = function() {
				var ret = document.querySelector(selector);
				if (!ret) {
					setTimeout(waitForContainer, 1);
				} else {
					q.resolve(ret);

					// Special class handling to add the active state to the currently active tab
					//  This is done after the container is loaded to ensure the tabs exist
					if (!tab.isModal()) {
						var tabs = document.querySelectorAll("px-tab");
						for (var i = tabs.length - 1; i >= 0; i--) {
							if (tabs[i].tab === tab || tabs[i].tabbar === tab.parent) {
								tabs[i].classList.add("active");
							} else {
								tabs[i].classList.remove("active");
							}
						}
					}
				}
			};
			waitForContainer();

			return q;
		};

		main.PB = PB;

		//Need global variable on setTimeOut function.
		//This will be used for clearTimeOut call.
		window.TimerTimeOut = [];

		var dciEpoch = PB.DCI.get("DCI_UNIX_EPOCH_TIME").pid;
		var dciOffset = PB.DCI.get("DCI_TIME_OFFSET").pid;
		var dciDateFormat = PB.DCI.get("DCI_DATE_FORMAT").pid;
		var dciTimeFormat = PB.DCI.get("DCI_TIME_FORMAT").pid;

		function updateTime(assemblyResponse) {
			for (var i = 0; i < assemblyResponse.ValueList.Value.length; i++) {
				switch (parseInt(assemblyResponse.ValueList.Value[i].pid)) {
					case dciEpoch:
						var epoch = assemblyResponse.ValueList.Value[i].value;
						break;
					case dciOffset:
						var timeOffset = assemblyResponse.ValueList.Value[i].value;
						break;
					case dciDateFormat:
						var dateFormat = PB.dateFormatType(parseInt(assemblyResponse.ValueList.Value[i].value));
						break;
					case dciTimeFormat:
						var timeFormat = PB.timeFormatType(parseInt(assemblyResponse.ValueList.Value[i].value));
						break;
					default:
						console.log(assemblyResponse.ValueList.Value[i].value);
				}
			}

			epoch = parseInt(epoch); //As it is string data parse to integer. 
			var epochTime = epoch;
			var epochDate = new Date(epochTime * 1000);
			var today = new Date(epochDate.valueOf());

			var myTime;

			if(timeOffset == 0) { //this lines indicates that user wants to ignore timezone.
					myTime = today;
			}
			else {
				//getTimezoneOffset automatically consider Daylight saving.
			
				/*The time-zone offset is the difference, in minutes, between UTC and local time. 
				Note that this means that the offset is positive if the local timezone is behind UTC and negative if it is ahead. 
				For example, for time zone UTC+10:00 (Australian Eastern Standard Time), -600 will be returned. */
				var userOffset = today.getTimezoneOffset(); // -330 of india.
				var serverOffset = timeOffset; // CDT Time Zone - Chicago.

				//To convert from normal/standard time to daylight saving time add 1 hour ( or +3600 seconds ).
				if(today.dst()) {
					serverOffset = serverOffset - 60; 
				}
				myTime = new Date(epochDate.valueOf()  + (60000)*(userOffset - serverOffset));
			}
			main.timeString = PB.formatDate(myTime, timeFormat);
			main.dateString = PB.formatDate(myTime, dateFormat);
			var nextEpoch = epoch + 1;
			window.TimerTimeOut.push(setTimeout(function() {
				for (var i = 0; i < assemblyResponse.ValueList.Value.length; i++) {
					if (dciEpoch == assemblyResponse.ValueList.Value[i].pid) {
						assemblyResponse.ValueList.Value[i].value = nextEpoch;
						break;
					}	
				}
				updateTime(assemblyResponse);
			}, 1000));
		}

		window.deviceTimeCallback = function(data) {
			var json = data;
			updateTime(json);
		}

		Date.prototype.stdTimezoneOffset = function() {
			var jan = new Date(this.getFullYear(), 0, 1);
			var jul = new Date(this.getFullYear(), 6, 1);
			return Math.max(jan.getTimezoneOffset(), jul.getTimezoneOffset());
		}

		Date.prototype.dst = function() {
			return this.getTimezoneOffset() < this.stdTimezoneOffset();
		}

		
		//updateTime(1499431339);
		//updateTime(946685543);

		main.addEventListener(ENUM.EVENT.LOGIN, function(type, user) {
			main.set("currentUser", PB.currentUser);
		});

		navbar.modals.login.go().then(function() {
			mainQ.resolve(main);
			PB.getDeviceTime();
			PB.idleLogout();
			// Add listener for current user to main
			main.currentUser = PB.currentUser;
			main.welcomeMsg = 'Welcome ' + main.currentUser.FullName.value + '!'; 	
			if (PB.currentProductCode != "") {
				var imageUrl = PB.urlBase + "/if/images/logo_" + PB.currentProductCode + ".svg";
				$.ajax({
					method: "GET",
					url: imageUrl,
				}).then(function () {
					main.isLogo = imageUrl;
				})
			}
			onhashchange();
		});
	};

	window.addEventListener("WebComponentsReady", loadWhenReady);
	document.addEventListener("DOMContentLoaded", loadWhenReady);
})();