/**
 *****************************************************************************************
 *	@file
 *
 *	@brief A file which contains a handler for the DCI which interfaces with the
 *	log.  Primarily to provide a basic DCI parameter for retrieval.  It is part
 *	of the Log functionality.
 *
 *	@n @b Definitions:
 *  @li Entity = A value contained in the DCI database.
 *	@li Entry = A single blob in the log.  Can contain one or multiple entity values.
 *
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LOG_DCI_HANDLER_H
	#define LOG_DCI_HANDLER_H

namespace BF_Misc
{

/*
 **************************************************************************************************
 *  @brief This class will construct a DCI parameter which will handle retrieval of the log
 *  entries utilizing one DCI for the log entry.
 *
 *  @details The basic function of a log is to take an entry and put it into a list in a first in
 *  order.  Each memory type (RAM, Flash, EEPROM, FRAM) requires a unique method of tracking the
 *  incremental entries.  This virtual class tries to capture the most common functions of a log
 *  queue.
 **************************************************************************************************
 */
class Log_DCI_Handler
{
	public:

		/**
		 *  @brief Constructs an instance of a DCI Log interface.
		 *  @details
		 *  @param[in] dcid_array: A list of DCIDs which will be logged in a single entry.
		 *  The DCID data will be logged in order and in a packed fashion within the blob.
		 *  @param[in] dcid_count: The length of the memory range to be checked.
		 *  @param[in] log_mem_handle:  The Log_Mem handle is the control which will interface
		 *  to the memory storage for the log.  This interface will handle the actual Queue.
		 */
		Log_DCI_Handler( DCI_ID_TD dcid_for_log_dci, DCI_ID_TD dcid_for_entry_count,
						 Log_Mem::entry_def_t const* entry_def );

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~Log_DCI_Handler( void );

	private:
		Log_DCI_Handler( const Log_DCI_Handler& );	///< Private copy constructor
		Log_DCI_Handler & operator =( const Log_DCI_Handler & );///< Private copy assignment operator

		DCI_CB_RET_TD Log_Access_CB( DCI_ACCESS_ST_TD* access_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Log_Access_Static_CB( DCI_CBACK_PARAM_TD param,
												   DCI_ACCESS_ST_TD* access_struct )
		{ return ( reinterpret_cast<Log_DCI_Handler*>( param )->Output_Change_CB( access_struct ) ); }

};

// Offer the method to retrieve the data from the absolute tail instead of going off of previous
// count.
// Retrieve based off of tail.
// Include index as well as data if utilizing this method.
}
#endif
