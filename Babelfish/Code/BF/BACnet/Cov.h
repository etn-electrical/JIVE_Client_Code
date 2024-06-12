/**
 *****************************************************************************************
 *   @file Cov.h
 *
 *   @brief This files declares a structure for COV List.
 *
 *   @details
 *
 *   @note
 *
 *   @version
 *   C++ Style Guide Version 1.0
 *
 *   @copyright 2016 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef COV_H
#define COV_H

#include "includes.h"
#include "CovChangeTracker.h"

typedef struct
{
	uint8_t covsubscribe;		// 0=none, else unconfirmedCOVNotification or confirmedCOVnotification
	uint32_t covprocessid;
	uint32_t covlifetime;						// 0=forever
	uint32_t covtimeremaining;					// 0=done
	union
	{
		float32_t udwData;
		uint8_t ubData[4];
	} old_present_value;
	frSource src;
	dword objid;
	uint32_t objType;
} COV;

int32_t createCOVList( uint8_t numOfcov );

CovChangeTracker* CovChangeTrackerPt( void );

#endif
