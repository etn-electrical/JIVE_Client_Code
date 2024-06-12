/**
 *****************************************************************************************
 *	@file BVObject.h
 *
 *	@brief This files declares a structure for Binary Value Object
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
#ifndef BVOBJECT_H
#define BVOBJECT_H

#include "includes.h"
#include "Base_DCI_BACnet_Data.h"

// Below Arrays are bought from .cpp file and converted to const as they are not modifiable for DG1, GSB
const dword BValllist[] =
{ ALLX, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, ACTIVE_TEXT, INACTIVE_TEXT, DESCRIPTION,
  PRIORITY_ARRAY, RELINQUISH_DEFAULT, 0xFFFFFFFFUL };

const dword BVreqlist[] =
{ REQUIRED, OBJECT_IDENTIFIER, OBJECT_TYPE, OBJECT_NAME, PRESENT_VALUE,
  STATUS_FLAGS, EVENT_STATE, OUT_OF_SERVICE, 0xFFFFFFFFUL };

const dword BVoptlist[] =
{ OPTIONALX, DESCRIPTION, ACTIVE_TEXT, INACTIVE_TEXT, RELINQUISH_DEFAULT, PRIORITY_ARRAY,
  0xFFFFFFFFUL };
// GSB Addition Ends

typedef struct
{
	uint8_t reli_default;
	uint8_t present_value;
	dword objid;
	uint32_t priority_array[16U];
} BACnetBV;

int32_t createBvObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects );

int32_t setPresentValueBV( uint32_t objId, uint8_t val );

int32_t setPresentValueBV_using_Instance( uint32_t instance, uint8_t val );

uint8_t getPresentValueBV_from_Instance( uint32_t instance );

uint8_t getPresentValueBV( uint32_t objId );

#endif
