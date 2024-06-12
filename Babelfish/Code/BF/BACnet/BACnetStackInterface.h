/**
 *****************************************************************************************
 *	@file BACnetStackinterface.h
 *
 *	@brief This files declares a class for BACnet interface with USARTSoftDMA
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

#ifndef BACNETSTACKINTERFACE_H
#define BACNETSTACKINTERFACE_H

#include "includes.h"
#include "Base_DCI_BACnet_Data.h"

#define OBJ_INSTANCE_MASK 0x003FFFFFUL
#define SHIFT_GET_OBJECT_ID 22U

typedef int32_t (* callback_t)( uint32_t );

static const uint8_t IS_READABLE = 0U;
static const uint8_t IS_COMMENDABLE = 2U;
/***********************App interface********************/

typedef struct
{
	uint32_t numObjects;
	uint32_t* objects;
} BACnetObject;

typedef union
{
	float32_t fl;
	uint16_t u16;
} FL_TO_U16;

int32_t createBACnetObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info,
							 callback_t msv_user_callback );

int32_t createBACnetCOVList();

bool_t isObjectPresent( uint32_t objid );

void DoSolicitedCOV( DCI_ID_TD dci_id );

void Track_COV_registration( void );

/***********************App interface********************/

#endif	// BACNETSTACKINTERFACE_H
