/**
 *****************************************************************************************
 *	@file AIObject.h
 *
 *	@brief This files declares a structure for Analog Input Object
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

#ifndef AIOBJECT_H
#define AIOBJECT_H

#include "includes.h"
#include "Base_DCI_BACnet_Data.h"

const dword AIalllist[] =
{ ALLX, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, UNITSX, DESCRIPTION, COV_INCREMENT,
  0xFFFFFFFFUL };

const dword AIreqlist[] =
{ REQUIRED, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, UNITSX, 0xFFFFFFFFUL };

const dword AIoptlist[] =
{ OPTIONALX, DESCRIPTION, COV_INCREMENT, 0xFFFFFFFFUL };

typedef struct
{
	float32_t covincrement;		// COV increment value for object
	dword objid;
	float32_t present_value;
	float32_t reli_default;
	// union   priorityarray //not supported AI object property
	// {
	// float32_t f[16];
	// dword d[16];
	// }priority_array;
} BACnetAI;

int32_t createAIObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects );

int32_t setPresentValueAI( uint32_t objId, float32_t val );	// AI is RO, still kept for future

int32_t getPresentValueAI( uint32_t objId, float32_t& val );

int32_t getPresentValueAI_from_Instance( uint32_t instance, float32_t& val );

#endif
