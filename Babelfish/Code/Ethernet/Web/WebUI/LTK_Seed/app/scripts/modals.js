"use strict";
(function() {

	// /////////////////
	// Built-in Tabs //
	//   and modals  //
	// /////////////////

	navbar.timeParam = PB.DCI.get("DCI_UNIX_EPOCH_TIME");

	// Create Login modal
	navbar.modals.login = navbar.modals.create("Login", "modal-login", "px-icons:user");
	navbar.modals.login.autoLogin = {
		username: "admin",
		password: "Admin*1"
	};

	// Create Message / Confirm modal
	navbar.modals.confirm = navbar.modals.create("Confirm", "modal-confirm", "px-icons:details");

	//Create Force Password Change model
	  navbar.modals.forcePasswordChange = navbar.modals.create("Password Change", "modal-force-password-change");
	  
	//License Information model
	  navbar.modals.licenseInformation = navbar.modals.create("Show License", "modal-license-information");

	// Create Authentication modal
	navbar.modals.sessionExpired = navbar.modals.create("Auth", "modal-session-expired", "px-icons:details");

	// Failure modal
	navbar.modals.failure = navbar.modals.create("Failure", "modal-failure", "px-icons:details");

	// Create Login History modal
	navbar.modals.loginHistory = navbar.modals.create("Login History", "modal-login-history");

	navbar.modals.connLost = navbar.modals.create("ConnLost", "modal-connection-loss", "px-icons:details");

	// Create Message / EULA modal
	navbar.modals.eula = navbar.modals.create("EULA", "modal-eula", "px-icons:details");

	// Create Message / Terms modal
	navbar.modals.terms = navbar.modals.create("TERMS", "modal-terms");

	// Create a time editing modal
	navbar.modals.time = navbar.modals.create("Time", "modal-time", "px-icons:time");

	// Create a Modbus Map modal
	navbar.modals.modbusMap = navbar.modals.create("Modbus Map", "modal-modbus-map");

	// Create a Firmware upgrade modal
	navbar.modals.firmwareUpgrade = navbar.modals.create("Firmware Upgrade", "modal-firmware-codepack");

	// Navigate to editTime
	navbar.editTime = navbar.modals.modbusMap.go;

	// Show a message dialog
	//  Returns a promise that is resolved with the index of the option selected
	//  If no options are given, 'OK' is the only option
	//  Promise is rejected if navigation changes without a selection
	navbar.showMessage = function(msg, title, options) {
		return navbar.modals.confirm.go({
			message: msg,
			title: title,
			options: options || ["OK"]
		});
	};

	// Shows an OK / Cancel dialog
	navbar.showConfirm = function(msg, title, options) {
		return navbar.showMessage(msg, title, options || ["Cancel", "OK"]);
	};

	// Occurs when PB.http fails to perform due to an authentication error.
	//  If the user is suggested to login, this is where it would happen
	PB.http.retryOnAuthError = false;
	PB.http.onAuthError = function() {
		var autoLogin = navbar.modals.login.autoLogin;

		// Temporary fix for the 100 second timeout
		if (autoLogin && autoLogin.used) {
			return PB.login(autoLogin.username, autoLogin.password);
		}

		return navbar.modals.login.go();
	};

	PB.addEventListener(ENUM.EVENT.HTTP, function(type, data) {
		var queueSize = data.queueSize;
		var retrySize = data.retrySize;
		if (queueSize === 0 && retrySize === 0) {
			navbar.setStatus(ENUM.STATUS.LOADING);
		} else if (retrySize > 0) {
			navbar.setStatus(ENUM.STATUS.LOADING, "Retrying " + retrySize + " parameters...");
		} else {
			navbar.setStatus(ENUM.STATUS.LOADING, "Loading " + queueSize + " parameters...");
		}
	})

}());
