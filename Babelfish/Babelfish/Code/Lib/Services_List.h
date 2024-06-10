/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details The intention of this module is to act like a redirector of common or
 *			even non-common services.  An example service would be a factory reset.
 *			This provides a central place for services to activated.
 *			Standard services reside in the 0->127 range.
 *			Application specific services reside in the 128->255 range.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SERVICES_LIST_H
#define SERVICES_LIST_H

/// The following contains all the application specific services.
#include "Services_List_App.h"

/*
 **************************************************************************************************
 *  Constants. The Standard services.
   --------------------------------------------------------------------------------------------------
 */
/// This reset will simply do a hardware reset.
static const uint8_t SERVICES_SOFT_RESET = 0U;
/// This reset brings the device to out of box state.
static const uint8_t SERVICES_FACTORY_RESET = 1U;
/// This reset service will bring all application values to defaults. The application parameters are defined in the app
/// column of the spreadshet.
static const uint8_t SERVICES_APP_PARAM_RESET = 2U;
/// This reset will sit in a loop until the watchdog catches it.
static const uint8_t SERVICES_WATCHDOG_TEST_RESET = 3U;
/// This will trigger a communication loss.  It is handled elsewhere in the system.
/// DEPRECATED static const uint8_t SERVICES_TRIGGER_COM_LOSS = 4U;
/// Send one byte of true in the data to unlock.  If no data will return the current state of unlock.
static const uint8_t SERVICES_MFG_ACCESS_UNLOCK = 5U;
/// This is the last of the default service function codes.
static const uint8_t SERVICES_MAX_STANDARD = 6U;

static const uint16_t SERVICES_MFG_ACCESS_UNLOCK_DATA_LEN = 1U;

#endif
