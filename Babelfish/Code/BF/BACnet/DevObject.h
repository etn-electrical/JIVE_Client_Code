/**
 *****************************************************************************************
 *	@file DevObject.h
 *
 *	@brief This files declares a structure for Device Object
 *
 *	@details
 *
 *	@note
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DEVOBJECT_H
#define DEVOBJECT_H

#include "core.h"
#include "Base_DCI_BACnet_Data.h"
#include "Format_Handler.h"

typedef struct appDevInfo
{
	dword app_deviceinstance;
	word app_vendor_id;
	byte app_system_status;

	byte object_types_supported[7];
	byte app_maxmaster;
	byte app_mstpstation;
	byte app_maxinfoframes;
	byte app_whoisinterval;
	byte app_apduretries;
	byte app_apdu_timeout;
	frString* app_password;
	word app_bipudp;
	short utcoffset;
	dword app_bipipaddr;
	dword app_bipipsubnet;
	dword app_bbmdipaddr;				// BBMD specific paramter enabled //Prateek
	word app_bbmdudp;					// BBMD specific paramter enabled //Prateek
	word app_bbmdttl;				// BBMD specific paramter enabled //Prateek
	octet lastrestartreason;			//

	frString* app_device_name;
	frString* app_vendor_name;
	frString* app_model_name;
	frString* app_firmware_rev;
	frString* serial_number;
	frString* app_application_software_ver;
	frString* app_device_description;
	frString* app_device_location;
	frString* app_device_profile;
} deviceInfo;
#endif
