/**
 *****************************************************************************************
 *	@file AVObject.h
 *
 *	@brief This files declares a structure for Analog Value Object
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

#ifndef AVOBJECT_H
#define AVOBJECT_H

#include "includes.h"
#include "Base_DCI_BACnet_Data.h"

// Below Arrays are bought from .cpp file and converted to const as they are not modifiable for DG1, GSB
const dword AValllist[] =
{ ALLX, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, UNITSX, DESCRIPTION, COV_INCREMENT,
  PRIORITY_ARRAY, RELINQUISH_DEFAULT, 0xFFFFFFFFUL };

const dword AVreqlist[] =
{ REQUIRED, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, UNITSX, 0xFFFFFFFFUL };

const dword AVoptlist[] =
{ OPTIONALX, DESCRIPTION, COV_INCREMENT, PRIORITY_ARRAY, RELINQUISH_DEFAULT, 0xFFFFFFFFUL };
// GSB Addition Ends

typedef struct
{
	float32_t covincrement;
	dword objid;
	float32_t present_value;
	float32_t reli_default;
	union priorityarray		//
	{
		float32_t f[16U];
		dword d[16U];
	} priority_array;
} BACnetAV;

int32_t createAvObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects );

int32_t setPresentValueAV( uint32_t objId, float32_t val );

int32_t getPresentValueAV( uint32_t objId, float32_t& val );

int32_t setPresentValueAV_using_Instance( uint32_t instance, float32_t val );

int32_t getPresentValueAV_from_Instance( uint32_t instance, float32_t& val );

#endif
