/*
*****************************************************************************************
*		Copyright Eaton Corporation. All Rights Reserved.
*		Creator: Mark A Verheyen
*
*		Description:
*		This file indicates to the outside world all the available application specific
*		services.
*
*****************************************************************************************
*/
#ifndef SERVICES_LIST_APP_H
	#define SERVICES_LIST_APP_H

///Application specific services
///These service numbers are passed in as 128 + the number from below.  This is to allow space for standard
///services in the <128 range.  If the below numbers are to be passed to the Service Execute command, 128 must be
///added.  A macro is provided below to accomplish this.
enum	///Application Service Enums
{
	SERVICES_APP_SPECIFIC_START = 127,		// Must be kept here to indicate where the app specific starts.
	SERVICES_APP_WIPE_NV_MEM,				// This will wipe NV memory.
	SERVICES_APP_RUN_BOOTLOADER,			// This will cause the bootloader to run.
	SERVICES_APP_SPECIFIC_END				// This must be kept as well to indicate the end of the app specific list.
};
#define SERVICES_MAX_APP_SPECIFIC_ARRAY			( SERVICES_APP_SPECIFIC_END - SERVICES_APP_SPECIFIC_START )

#endif
