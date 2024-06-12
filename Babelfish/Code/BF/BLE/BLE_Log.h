/**
 *****************************************************************************************
 *	@file BLE_Log.h
 *
 *	@brief A file is for logging protocol for BLE.
 *	@n @b Definitions:
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BLE_LOG_H
	#define BLE_LOG_H

#include "DCI_Defines.h"
#include "DCI_Owner.h"
#include "Log_Mem.h"

/**
 **************************************************************************************************
 *  @brief This class is responsible for transfering the logged data tO BLE stack interfaces.
 *  @details The callback for the Log DCIDs reads the data from Logging files.
 **************************************************************************************************
 */
class BLE_Log
{
	public:
		/**
		 *  @brief Constructs an instance of a BLE_Log.
		 *  @param[in] log_id: Log id
		 *  @param[in] log_dcid: dcid for log retrival.
		 *  @param[in] index_dcid: dcid for log index.
		 *  @param[in] max_client_data_buf: max client data buffer size.
		 */
		BLE_Log( log_id_t log_id, DCI_ID_TD log_dcid, DCI_ID_TD index_dcid, uint16_t max_client_data_buf,
				 BF_Misc::Log_Mem* log_mem );

		/**
		 *  @brief Updates tx buffer size according to client capacity.
		 *  @param[in] buff_size: Buffer size.
		 */
		void Update_Tx_Buff_Size( BF_Misc::Log_Mem::entry_size_t buff_size );

	private:
		BLE_Log( const BLE_Log& );				///< Private copy constructor
		BLE_Log & operator =( const BLE_Log & );			///< Private copy assignment operator

		DCI_CB_RET_TD Ble_Log_Process_Callback( DCI_ACCESS_ST_TD* access_struct );

		static DCI_CB_RET_TD Ble_Log_Process_Callback_Static( DCI_CBACK_PARAM_TD handle,
															  DCI_ACCESS_ST_TD* access_struct )
		{
			return ( ( ( BLE_Log* )handle )->Ble_Log_Process_Callback( access_struct ) );
		}

		DCI_CB_RET_TD Index_Update_Callback( DCI_ACCESS_ST_TD* access_struct );

		static DCI_CB_RET_TD Index_Update_Callback_Static( DCI_CBACK_PARAM_TD handle,
														   DCI_ACCESS_ST_TD* access_struct )
		{
			return ( ( ( BLE_Log* )handle )->Index_Update_Callback( access_struct ) );
		}

		DCI_Owner* m_log_owner;
		DCI_Owner* m_index_owner;
		log_id_t m_log_id;
		BF_Misc::Log_Mem::entry_size_t m_tx_buff_size;
		BF_Misc::Log_Mem::entry_index_t m_last_read_index;

		BF_Misc::Log_Mem* m_log_mem;

		static const BF_Misc::Log_Mem::entry_index_t NEXT_INDEX_LOC = 0U;
		static const BF_Misc::Log_Mem::entry_index_t RETRIEVED_INDEX_LOC = NEXT_INDEX_LOC +
			static_cast<BF_Misc::Log_Mem::entry_index_t>( sizeof ( NEXT_INDEX_LOC ) );
		static const BF_Misc::Log_Mem::entry_index_t REMAINING_INDEX_LOC = RETRIEVED_INDEX_LOC +
			static_cast<BF_Misc::Log_Mem::entry_index_t>( sizeof( RETRIEVED_INDEX_LOC ) );
		static const BF_Misc::Log_Mem::entry_index_t LOG_DATA_LOC = REMAINING_INDEX_LOC +
			static_cast<BF_Misc::Log_Mem::entry_index_t>( sizeof ( REMAINING_INDEX_LOC ) );
		static const BF_Misc::Log_Mem::entry_index_t LOG_HEADER_SIZE = LOG_DATA_LOC;
};

#endif	/* end namespace BF_Misc for this module */

