/**
 **********************************************************************************************
 * @file            Prod_Specific_BACnet.h  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to BACNET
 * component.
 *
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef PROD_SPECIFIC_BACNET_H
	#define PROD_SPECIFIC_BACNET_H
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "vsbhp.h"
#include "BACnet_Config.h"


static const dword DEalllist[] = {ALLX, OBJECT_IDENTIFIER, OBJECT_NAME, OBJECT_TYPE, DESCRIPTION, SYSTEM_STATUS,
								  VENDOR_NAME, VENDOR_IDENTIFIER, MODEL_NAME, FIRMWARE_REVISION,
								  APPLICATION_SOFTWARE_VERSION, PROTOCOL_VERSION, PROTOCOL_REVISION,
								  PROTOCOL_SERVICES_SUPPORTED, PROTOCOL_OBJECT_TYPES_SUPPORTED, OBJECT_LIST,
								  ACTIVE_COV_SUBSCRIPTIONS, MAX_APDU_LENGTH_ACCEPTED,
								  SEGMENTATION_SUPPORTED, APDU_TIMEOUT,
								  NUMBER_OF_APDU_RETRIES, MAX_MASTER, MAX_INFO_FRAMES,
								  DEVICE_ADDRESS_BINDING, DATABASE_REVISION,
								  SERIAL_NUMBER, PROFILE_NAME,
								  EATON_DEFAULT_PASSWORD, PROPERTY_LIST,					// ***1206
								  LOCATION, STRUCTURED_OBJECT_LIST, UTC_OFFSET, DAYLIGHT_SAVINGS_STATUS, LOCAL_TIME,
								  LOCAL_DATE,
								  LAST_RESTART_REASON, TIME_OF_DEVICE_RESTART, RESTART_NOTIFICATION_RECIPIENTS,
								  0xFFFFFFFF};	// Important!  This list must end with 0xFFFFFFFF		// ***243,
												// ***400, ***600
static const dword DEreqlist[] = {REQUIRED, OBJECT_IDENTIFIER, OBJECT_NAME, OBJECT_TYPE, SYSTEM_STATUS,
								  VENDOR_NAME, VENDOR_IDENTIFIER, MODEL_NAME, FIRMWARE_REVISION,
								  APPLICATION_SOFTWARE_VERSION, PROTOCOL_VERSION, PROTOCOL_REVISION,
								  PROTOCOL_SERVICES_SUPPORTED, PROTOCOL_OBJECT_TYPES_SUPPORTED, OBJECT_LIST,
								  MAX_APDU_LENGTH_ACCEPTED, SEGMENTATION_SUPPORTED,
								  APDU_TIMEOUT, NUMBER_OF_APDU_RETRIES,
								  DEVICE_ADDRESS_BINDING, DATABASE_REVISION,
								  PROPERTY_LIST,					// ***1206
								  0xFFFFFFFF};	// Important!  This list must end with 0xFFFFFFFF	// ***002
static const dword DEoptlist[] = {OPTIONALX,
								  DESCRIPTION, MAX_MASTER, MAX_INFO_FRAMES,
								  ACTIVE_COV_SUBSCRIPTIONS,
								  SERIAL_NUMBER, PROFILE_NAME, EATON_DEFAULT_PASSWORD,
								  LOCAL_TIME, LOCAL_DATE, UTC_OFFSET, DAYLIGHT_SAVINGS_STATUS,
								  LOCATION, STRUCTURED_OBJECT_LIST, LAST_RESTART_REASON, TIME_OF_DEVICE_RESTART,
								  RESTART_NOTIFICATION_RECIPIENTS,
								  0xFFFFFFFF};	// Important!  This list must end with 0xFFFFFFFF	// ***400 ***404
												// ***600

static const dword listofarrays[] = {OBJECT_LIST, CONFIGURATION_FILES, PRIORITY_ARRAY,						// ***400
																											// Begin
									 STATE_TEXT, EVENT_TIME_STAMPS, PRIORITY,
									 WEEKLY_SCHEDULE, EXCEPTION_SCHEDULE,
									 ACTION, ACTION_TEXT, SLAVE_PROXY_ENABLE, AUTO_SLAVE_DISCOVERY,
									 STRUCTURED_OBJECT_LIST, SUBORDINATE_ANNOTATIONS,
									 PROPERTY_LIST,					// ***1206
									 0xFFFFFFFF};	// Important!  This list must end with 0xFFFFFFFF	//	***400 End
static const dword listoflists[] = {DATE_LIST, RECIPIENT_LIST, LIST_OF_OBJECT_PROPERTY_REFERENCE,
									LOG_DEVICE_OBJECT_PROPERTY, LOG_BUFFER, ACTIVE_COV_SUBSCRIPTIONS,
									0xFFFFFFFF};

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */
int32_t Check_Prod_Spec_Property( dword pid, frVbag* vb, octet iswrite, bool* musthave );

#endif
