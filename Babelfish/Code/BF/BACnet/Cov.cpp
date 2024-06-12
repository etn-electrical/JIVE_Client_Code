/**
 *****************************************************************************************
 *   @file Cov.cpp
 *   @details See header file for module overview.
 *   @copyright 2016 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "vsbhp.h"
#include "Cov.h"
#include "BACnetStackInterface.h"
#include "Ram.h"

BACnetObject covList;
CovChangeTracker* covChangeTr;

/** @fn createCOVList(uint8_t numOfcov)
 *   @brief :create  COV list of object instances
 *   @details :This function has to call before any COV subscription. This function creates the instances of
 *             the COV.
 *   @param[in] numObjects: number of cov's
 *   @return:0 on success and -1 otherwise
 */

int32_t createCOVList( uint8_t numOfcov )
{
	uint8_t i = 0U;

	covList.numObjects = numOfcov;
	covChangeTr = new CovChangeTracker();
	int32_t retVal = 0;

	covList.objects = ( uint32_t* )Ram::Allocate( ( uint16_t )numOfcov * sizeof( uint32_t ) );
	if ( covList.objects != NULL )
	{
		for ( i = 0U; i < numOfcov; i++ )
		{
			covList.objects[( uint16_t )i] = ( uint32_t )Ram::Allocate( sizeof( COV ) );
			if ( covList.objects[( uint16_t )i] == NULL )
			{
				retVal = -1;
			}
		}
	}
	else
	{
		retVal = -1;
	}
	return ( retVal );
}

/** @fn CovChangeTrackerPt()
 *   @brief : Returns the pointer of COV change tracker object
 *   @details :This function has to be called to use CovChangeTracker
 *   @param[in]
 *   @return:0 on success and -1 otherwise
 */

CovChangeTracker* CovChangeTrackerPt()
{
	return ( covChangeTr );
}
