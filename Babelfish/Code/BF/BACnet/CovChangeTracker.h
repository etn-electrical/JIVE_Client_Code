/**
 *****************************************************************************************
 *   @file CovChangeTracker.h
 *
 *   @brief This files Tracks the DCID's registered by COV subscription.
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
#ifndef COVCHANGETRACKER_H
#define COVCHANGETRACKER_H

#include "Change_Tracker.h"

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class CovChangeTracker
{
	public:
		CovChangeTracker();
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~CovChangeTracker( void ){}

		void Track_ID( DCI_ID_TD dci_id );

		void UnTrack_ID( DCI_ID_TD dci_id );

	private:

		Change_Tracker* m_change_tracker;

		void Change_Track_Handler( DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Change_Track_Handler_Static( Change_Tracker::cback_param_t param,
												 DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask )
		{
			( reinterpret_cast<CovChangeTracker*>( param ) )->Change_Track_Handler( dci_id,
																					attribute_mask );
		}

};

#endif
