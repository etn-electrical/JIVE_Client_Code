/**
 *****************************************************************************************
 *	@file MSVObject.h
 *
 *	@brief This files declares a structure for Multi-state Value Object
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

#ifndef MSVOBJECT_H
#define MSVOBJECT_H

#include "includes.h"
#include "Base_DCI_BACnet_Data.h"

const dword MSValllist[] =
{ ALLX, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, NUMBER_OF_STATES, STATE_TEXT, DESCRIPTION,
  PRIORITY_ARRAY, RELINQUISH_DEFAULT, PROPERTY_LIST, 0xFFFFFFFFUL };

const dword MSVreqlist[] =
{ REQUIRED, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, NUMBER_OF_STATES, 0xFFFFFFFFUL };

const dword MSVoptlist[] =
{ OPTIONALX, DESCRIPTION, STATE_TEXT, PRIORITY_ARRAY, RELINQUISH_DEFAULT, PROPERTY_LIST,
  0xFFFFFFFFUL };

typedef struct
{
	dword objid;
	uint8_t present_value;
	float32_t reli_default;
	union priorityarray
	{
		float32_t f[16U];
		dword d[16U];
	} priority_array;
} BACnetMSV;

int32_t createMSVObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects );

// int32_t setPresentValueMSV(uint32_t objId,  float32_t val); // Nishchal D:  MSV is RO
uint32_t getPresentValueMSV( uint32_t objId, uint8_t& val );

uint32_t getPresentValueMSV_from_Instance( uint32_t instance, uint8_t& val );

#endif
