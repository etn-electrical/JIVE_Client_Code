/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Delta_Data.h"
#include "DCI_Owner.h"
#include "Babelfish_Assert.h"

#include "Data_Align.hpp"

const uint8_t Delta_Data::DCOM_TO_CHANGE_TRACKER_ATTRIB[END_OF_ATTRIBUTE_LIST] =
{
	Change_Tracker::RAM_ATTRIB,					// CURRENT_VALUE = 0U;
	Change_Tracker::NV_ATTRIB,					// INITIAL_VALUE = 1U;
	Change_Tracker::DEFAULT_ATTRIB,				// DEFAULT_VALUE = 2U;
	Change_Tracker::LENGTH_ATTRIB,				// LENGTH = 3U;
	Change_Tracker::TOTAL_ATTRIBS,				// DATATYPE = 4U;
	Change_Tracker::TOTAL_ATTRIBS,				// ATTRIBUTE_INFORMATION = 5U;
	Change_Tracker::MIN_ATTRIB,					// MIN = 6U;
	Change_Tracker::MAX_ATTRIB,					// MAX = 7U;
	Change_Tracker::TOTAL_ATTRIBS,				// ENUM_LIST_LENGTH = 8U;
	Change_Tracker::TOTAL_ATTRIBS,				// ENUM_LIST = 9U;
	Change_Tracker::TOTAL_ATTRIBS				// SYSTEM_COMMAND_PARAMETER = 10U;
};

// The below array actually decides priority of how things are loaded into the
// change frame.  The priority starts at 0.  We will load all attributes from
// "0" first then move up to the next one in the list until all are complete.
const uint8_t Delta_Data::GET_DELTA_DATA_ATTRIB_LIST_LEN = 5U;
const Delta_Com::attribute_t
Delta_Data::GET_DELTA_DATA_ATTRIB_LIST[GET_DELTA_DATA_ATTRIB_LIST_LEN] =
{
	MAX,
	MIN,
	DEFAULT_VALUE,
	INITIAL_VALUE,
	CURRENT_VALUE
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Data::Delta_Data( data_def_t const* data_def, bool owner_access,
						bool correct_out_range ) :
	m_data_def( data_def ),
	m_val_tracker( new Change_Tracker_Plus( true, CHANGE_TRACKER_ATTRIB_TRACK_MASK ) ),
	m_workers( reinterpret_cast<DCI_Workers*>( nullptr ) ),
	m_sub_req_list_owner( new BF_Lib::Bit_List( data_def->pid_dcid_list_len,
												data_def->subscription_bit_list ) ),
	m_client_sot_sub_req_list_owner( new BF_Lib::Bit_List( data_def->pid_dcid_list_len,
														   data_def->
														   client_sot_subscription_bit_list ) ),
	m_sub_req_list_patron( new BF_Lib::Bit_List( data_def->pid_dcid_list_len,
												 data_def->general_subscription_bit_list ) ),
	m_client_sot_sub_req_list_patron( new BF_Lib::Bit_List( data_def->pid_dcid_list_len,
															data_def->
															general_client_sot_subscription_bit_list ) ),
	m_sub_req_list( reinterpret_cast<BF_Lib::Bit_List*>( nullptr ) ),
	m_client_sot_sub_req_list( reinterpret_cast<BF_Lib::Bit_List*>( nullptr ) ),
	m_sub_req_ctr( 0U ),
	m_client_sot_sub_req_ctr( 0U ),
	m_startup_sync( new BF_Lib::Bit_List( data_def->pid_dcid_list_len ) ),
	m_owner_access( owner_access )
{
	uint16_t num_owners_to_create;
	uint16_t bit_index;

	BF_Lib::Bit_List owner_bit_list( m_data_def->pid_dcid_list_len,
									 m_data_def->owner_patron_not_list );

	Set_Access_Level( owner_access );

	num_owners_to_create = owner_bit_list.Active_Bits();
	bit_index = 0U;
	while ( num_owners_to_create > 0U )
	{
		if ( owner_bit_list.Test( bit_index ) )
		{
			// The following will check to see if it has already been created elsewhere.
			// If it has been we skip over creating it again.
			if ( !DCI_Owner::Is_Created( m_data_def->pid_to_dcid_map[bit_index].dcid ) )
			{
				new DCI_Owner( m_data_def->pid_to_dcid_map[bit_index].dcid );

			}
			num_owners_to_create--;
		}
		bit_index++;
	}

	m_workers = new DCI_Workers( correct_out_range, owner_access, m_val_tracker );

	Reset_Subscription_Req_List_Que();
	Reset_Publish_Req_List();

	DCOM_Print_Active();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Data::~Delta_Data( void )
{
	m_data_def = reinterpret_cast<data_def_t const*>( nullptr );
	delete m_val_tracker;
	delete m_workers;
	delete m_sub_req_list;
	delete m_sub_req_list_owner;
	delete m_sub_req_list_patron;
	delete m_client_sot_sub_req_list;
	delete m_client_sot_sub_req_list_owner;
	delete m_client_sot_sub_req_list_patron;
	delete m_startup_sync;

	// The destructor is not supported because we would need to keep a log of all the
	// owners which were created for the DCI parameters.
	BF_Lib::Exception::Destructor_Not_Supported();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Data::Publish_Request( uint8_t* rx_data, uint16_t param_count, uint8_t tracking_type )
{
	bool tracking = true;
	uint16_t param_ctr = 0U;
	uint16_t buffer_index = 0U;

	Delta_Com::param_id_t pid;

	while ( param_ctr < param_count )
	{
		pid = Data_Align<uint16_t>::From_Array( &rx_data[buffer_index] );

		if ( Publish_PID_Request( pid, tracking_type ) == false )
		{
			tracking = false;
		}

		buffer_index += PARAMETER_LENGTH;
		param_ctr++;
	}

	return ( tracking );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Data::Sync_Complete( void )
{
	bool sync_complete = false;

	if ( m_startup_sync->Active_Bits() == 0U )
	{
		sync_complete = true;
	}

	return ( sync_complete );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Data::Set_Access_Level( bool owner_access )
{
	m_owner_access = owner_access;

	if ( owner_access == true )
	{
		m_sub_req_list = m_sub_req_list_owner;
		m_client_sot_sub_req_list = m_client_sot_sub_req_list_owner;
	}
	else
	{
		m_sub_req_list = m_sub_req_list_patron;
		m_client_sot_sub_req_list = m_client_sot_sub_req_list_patron;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Data::Reset_Full( void )
{
	Reset_Subscription_Req_List_Que();
	Reset_Publish_Req_List();
	Reset_Data();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Data::Reset_Data( void ) const
{
	BF_Lib::Bit_List val_clear_bit_list( m_data_def->pid_dcid_list_len,
										 m_data_def->value_clear );

	uint16_t num_parameters;
	uint16_t index;

	num_parameters = val_clear_bit_list.Active_Bits();

	while ( num_parameters > 0U )
	{
		index = val_clear_bit_list.Get_Next();
		m_workers->Initialize_Current(
			m_data_def->pid_to_dcid_map[index].dcid );

		// TODO: Update the following code.
		////If we made a change and we are not the source of truth then we should probably have
		////the server tell us what is up.  This means we need to initialize and then clear
		////the change.
		// if ( m_client_sot_sub_req_list->Test( index ) == false )
		// {
		// m_val_tracker->Clear_Change(
		// m_data_def->pid_to_dcid_map[m_data_def->dcid_to_pid_map[index]].dcid,
		// Change_Tracker::RAM_ATTRIB_MASK );
		// }
		num_parameters--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Data::Publish_PID_Request( Delta_Com::param_id_t param_id, uint8_t tracking_type )
{
	uint16_t index;
	DCI_ID_TD dcid;
	bool tracking = false;
	bool in_sub_req_list;
	bool in_client_sot_sub_req_list;

	index = Get_PID_DCID_Map_Index_From_PID( param_id );
	if ( index < m_data_def->pid_dcid_list_len )
	{
		dcid = m_data_def->pid_to_dcid_map[index].dcid;

		// Checks to make sure the dcid is available on the server
		switch ( tracking_type )
		{
			case STOP_TRACKING_PARAMETER_FOR_CHANGES:
				m_val_tracker->UnTrack_ID( dcid );
				break;

			case CLIENT_PUB_REQ_CLIENT_SOURCE_OF_TRUTH:
			case CLIENT_PUB_REQ_CLIENT_SOURCE_OF_TRUTH_GEN_ACCESS:
				m_val_tracker->Track_ID( dcid, false );
				tracking = true;
				// DCOM_SubPub_Print( "Cmd:, Sub, PID:,%d, Index:, %d, Truth:, Client\n", param_id,
				// index );
				break;

			case CLIENT_PUB_REQ_SERVER_SOURCE_OF_TRUTH:
			case CLIENT_PUB_REQ_SERVER_SOURCE_OF_TRUTH_GEN_ACCESS:
				m_val_tracker->Track_ID( dcid, true );
				m_startup_sync->Set( index );
				tracking = true;
				// DCOM_SubPub_Print( "Cmd:, Sub, PID:,%d, Index:, %d, Truth:, Server\n", param_id,
				// index );
				break;

			case SERVER_PUB_REQ:
				in_sub_req_list = m_sub_req_list->Test( index );
				in_client_sot_sub_req_list = m_client_sot_sub_req_list->Test( index );
				if ( in_client_sot_sub_req_list || ( in_sub_req_list == false ) )
				{
					m_val_tracker->Track_ID( dcid, true );
					m_startup_sync->Set( index );
				}
				else
				{
					m_val_tracker->Track_ID( dcid, false );
				}
				tracking = true;
				break;

			default:
				break;
		}
	}
	else
	{
		DCOM_Error_Print(
			"DCOM Error:, PubReq, PID:,%d, Error:, PID_Not_Found\n", param_id );
	}

	return ( tracking );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Data::Reset_Publish_Req_List( void )
{
	m_val_tracker->UnTrack_All_IDs();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Delta_Data::Subscription_Req_List( uint8_t* tx_data, uint16_t* tx_length_used,
											uint16_t tx_length_available, uint8_t tracking_type )
{
	uint16_t buffer_index;
	uint16_t start_param_count;
	uint16_t* param_counter;
	uint16_t parameters_loaded;

	BF_Lib::Bit_List* subscription_list;
	uint16_t total_subs;

	switch ( tracking_type )
	{
		case CLIENT_PUB_REQ_CLIENT_SOURCE_OF_TRUTH:
		case CLIENT_PUB_REQ_CLIENT_SOURCE_OF_TRUTH_GEN_ACCESS:
			param_counter = &m_client_sot_sub_req_ctr;
			subscription_list = m_client_sot_sub_req_list;
			break;

		case CLIENT_PUB_REQ_SERVER_SOURCE_OF_TRUTH:
		case CLIENT_PUB_REQ_SERVER_SOURCE_OF_TRUTH_GEN_ACCESS:
		case SERVER_PUB_REQ:
			param_counter = &m_sub_req_ctr;
			subscription_list = m_sub_req_list;
			break;

		case STOP_TRACKING_PARAMETER_FOR_CHANGES:
		default:
			param_counter = &m_sub_req_ctr;
			subscription_list = m_sub_req_list;
			// This shall never happen.  It would have to happen deep inside the server/client.
			BF_ASSERT(false);
			break;
	}

	*tx_length_used = 0U;

	// Do we have enough space to load at least one parameter?
	// Do we have more than 0 parameters to load?
	total_subs = subscription_list->Active_Bits();
	if ( ( ( ATTRIBUTE_LENGTH + COUNT_LENGTH + PARAMETER_LENGTH ) < tx_length_available ) &&
		 ( *param_counter < total_subs ) )	// Are there subscriptions left to send?
	{
		start_param_count = *param_counter;
		buffer_index = ATTRIBUTE_LENGTH + COUNT_LENGTH;
		while ( ( *param_counter < total_subs ) &&
				( ( tx_length_available - buffer_index ) >= PARAMETER_LENGTH ) )
		{
			Data_Align<uint16_t>::To_Array(
				m_data_def->pid_to_dcid_map[subscription_list->Get_Next()].parameter_id,
				&tx_data[buffer_index] );
			buffer_index += PARAMETER_LENGTH;
			*param_counter = *param_counter + 1U;
		}
		parameters_loaded = *param_counter - start_param_count;

		tx_data[0] = tracking_type;

		Data_Align<uint16_t>::To_Array( parameters_loaded,
										&tx_data[ATTRIBUTE_LENGTH] );

		*tx_length_used = buffer_index;
	}

	return ( total_subs - *param_counter );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Delta_Data::Reset_Subscription_Req_List_Que( void )
{
	m_sub_req_ctr = 0U;
	m_client_sot_sub_req_ctr = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Data::Subscription_Complete( void )
{
	bool fully_subscribed = true;

	if ( m_sub_req_ctr != m_sub_req_list->Active_Bits() )
	{
		fully_subscribed = false;
	}

	if ( m_client_sot_sub_req_ctr != m_client_sot_sub_req_list->Active_Bits() )
	{
		fully_subscribed = false;
	}

	return ( fully_subscribed );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Data::Get_Delta_Data( uint8_t* tx_data, uint16_t* tx_length_used,
											  uint16_t tx_length_available )
{
	static const uint16_t MIN_HEADER_LEN = ATTRIBUTE_LENGTH + COUNT_LENGTH + MIN_DATA_LENGTH;
	uint16_t number_of_parameters;
	bool keep_reading;
	DCI_ID_TD dcid;
	uint16_t data_length;
	uint16_t tx_frame_itr;
	delta_com_error_t access_status;

	Delta_Com::param_id_t pid;
	Delta_Com::attribute_t attribute;
	uint8_t change_tracker_attrib;
	Delta_Com::attribute_t attrib_index;
	uint16_t param_list_itr;
	uint16_t bit_index;
	Change_Tracker::attrib_mask_t attrib_mask;

	tx_frame_itr = 0U;
	attrib_index = 0U;
	keep_reading = true;
	// No point in doing anything if there is not enough room for even a basic frame.
	// It also makes sure that the frame does not go negative.
	// Need to account for the attribute and count that goes in the front;
	while ( ( attrib_index < GET_DELTA_DATA_ATTRIB_LIST_LEN ) &&
			( ( tx_frame_itr + MIN_HEADER_LEN ) < tx_length_available ) &&
			( keep_reading == true ) )
	{
		attribute = GET_DELTA_DATA_ATTRIB_LIST[attrib_index];
		param_list_itr = tx_frame_itr + COUNT_LENGTH + ATTRIBUTE_LENGTH;

		change_tracker_attrib = DCOM_TO_CHANGE_TRACKER_ATTRIB[attribute];
		number_of_parameters = 0U;
		while ( m_val_tracker->Peek_Next( &dcid, change_tracker_attrib ) &&
				( keep_reading == true ) )
		{
			m_workers->Get_Length( dcid, &data_length );
			if ( ( param_list_itr + data_length + PARAMETER_LENGTH ) < tx_length_available )
			{
				attrib_mask = 0U;
				BF_Lib::Bit::Set( attrib_mask, change_tracker_attrib );
				m_val_tracker->Clear_Change( dcid, attrib_mask );

				access_status = Get_DCI_Data( dcid, attribute,
											  &tx_data[param_list_itr + PARAMETER_LENGTH],
											  &data_length );

				pid = Convert_DCID_to_PID( dcid );
				if ( access_status == NO_DCOM_ERROR )
				{
					BF_ASSERT( pid != Delta_Com::UNDEFINED_PARAMETER );
					// Exception should not happen unless during development a value gets
					// mismatched.  We will catch that here.  We will not track IDs that
					// we don't already know.  This would be an indication of trouble elsewhere.
					// Maybe the change tracker.

					Data_Align<uint16_t>::To_Array( pid, &tx_data[param_list_itr] );
					param_list_itr += ( PARAMETER_LENGTH + data_length );
					number_of_parameters++;

					DCOM_Access_Print(
						"Cmd:, Get, PID:,%d, DCID:, %d, Attrib:, %d, InitSync:, %d\n",
						pid, dcid, attribute,
						( m_startup_sync->Active_Bits() > 0U ) );
				}
				else
				{
					DCOM_Error_Print(
						"DCOM Error:, Get, PID:,%d, DCID:, %d, Attrib:, %d, InitSync:, %d, Error:, %d\n",
						pid, dcid, attribute, ( m_startup_sync->Active_Bits() > 0U ),
						access_status );
					// We should not get here.  This indicates that something went
					// Wrong with regard to the get function.  Typically this
					// would be because someone forgot to do a new on a
					// the appropriate owner.
					// BF_ASSERT(false);
				}
				if ( ( m_startup_sync->Active_Bits() > 0U ) &&
					 ( attribute == SYNC_COMPLETE_ATTRIBUTE ) )
				{
					bit_index = Get_PID_DCID_Map_Index_From_PID( pid );
					m_startup_sync->Dump( bit_index );
				}

			}
			else
			{
				keep_reading = false;
			}
		}

		if ( number_of_parameters > 0U )
		{
			tx_data[tx_frame_itr] = attribute;
			tx_frame_itr += ATTRIBUTE_LENGTH;
			Data_Align<uint16_t>::To_Array( number_of_parameters, &tx_data[tx_frame_itr] );
			tx_frame_itr = param_list_itr;
		}
		attrib_index++;
	}

	*tx_length_used = tx_frame_itr;

	return ( NO_DCOM_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Data::Set_Delta_Data( uint8_t* rx_data, uint16_t rx_data_len )
{
	delta_com_error_t access_status = NO_DCOM_ERROR;

	Delta_Com::param_id_t pid;
	uint16_t current_parameter;
	uint16_t total_params;
	uint16_t rx_frame_itr;
	DCI_LENGTH_TD amount_set;
	DCI_LENGTH_TD length;
	Delta_Com::attribute_t attribute;

	DCI_ID_TD dcid;
	DCI_LENGTH_TD length_length;
	uint8_t change_tracker_attrib;
	Change_Tracker::attrib_mask_t attrib_mask;

	rx_frame_itr = 0U;

	while ( ( rx_frame_itr < rx_data_len ) && ( access_status == NO_DCOM_ERROR ) )
	{
		attribute = rx_data[rx_frame_itr];
		rx_frame_itr += ATTRIBUTE_LENGTH;

		total_params = Data_Align<uint16_t>::From_Array( &rx_data[rx_frame_itr] );
		rx_frame_itr += COUNT_LENGTH;

#ifdef DEBUG
		// The following line does a check of the total parameters left and the length remaining.
		// If there is not enough length left for a minimum amount of parameters then
		// this is an issue.
		BF_ASSERT( ( total_params * MIN_CHANGE_PARAM_ENTRY_LENGTH ) <= ( rx_data_len - rx_frame_itr ) );
#endif

		current_parameter = 0U;
		while ( ( current_parameter < total_params ) && ( access_status == NO_DCOM_ERROR ) )
		{
			pid = Data_Align<uint16_t>::From_Array( &rx_data[rx_frame_itr] );
			rx_frame_itr += PARAMETER_LENGTH;

			access_status = Set_Data( pid, attribute, &rx_data[rx_frame_itr], &amount_set );

			DCOM_Access_Print( "Cmd:, Set, PID:,%d, Attrib:, %d\n", pid, attribute );

			if ( access_status != NO_DCOM_ERROR )
			{
				// If we are out of range or the value was locked or generally inaccessible then
				// we should really set the value as being in need of being synced back to whoever
				// was trying to change it.
				dcid = Convert_PID_To_DCID( pid );

				DCOM_Error_Print(
					"DCOM Error:, SetBounce, PID:,%d, DCID:, %d, Attrib:, %d, InitSync:, %d, Error:, %d\n",
					pid, dcid, attribute, ( m_startup_sync->Active_Bits() > 0U ), access_status );

				change_tracker_attrib = DCOM_TO_CHANGE_TRACKER_ATTRIB[attribute];
				if ( m_val_tracker->ID_Tracked( dcid ) )
				{
					attrib_mask = 0U;
					BF_Lib::Bit::Set( attrib_mask, change_tracker_attrib );
					m_val_tracker->Set_Change( dcid, attrib_mask );
				}

				length_length = static_cast<DCI_LENGTH_TD>( sizeof( DCI_LENGTH_TD ) );
				access_status = Get_DCI_Data( dcid, LENGTH, reinterpret_cast<uint8_t*>( &length ),
											  &length_length );

				BF_ASSERT(access_status == NO_DCOM_ERROR );
				rx_frame_itr += length;
				// Try and choose what we do here.  In the past we would return the invalid
				// parameter.
				// This of course the client did nothing with so it was sort of pointless.
				// It also caused us to lose complete synch with the client who would have to
				// hunt
				// through and figure out what in the original message was wrong.
				// Plus the client should never send an invalid parameter that we did
				// not subscribe to.
				// *failed_param = pid;
				// return_status = INVALID_PARAMETER;
				}
			}
			else
			{
				rx_frame_itr += amount_set;
			}
			current_parameter++;
		}
	}

	return ( access_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Data::Get_Data( Delta_Com::param_id_t pid, delta_com_attrib_t attribute,
										uint8_t* data, DCI_LENGTH_TD* amount_gotten,
										DCI_LENGTH_TD offset, DCI_LENGTH_TD amount )
{
	return ( Get_DCI_Data( Convert_PID_To_DCID( pid ), attribute, data, amount_gotten,
						   offset, amount ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Data::Get_DCI_Data( DCI_ID_TD dcid, delta_com_attrib_t attribute,
											uint8_t* data, DCI_LENGTH_TD* amount_gotten,
											DCI_LENGTH_TD offset, DCI_LENGTH_TD amount )
{
	DCI_ERROR_TD status = DCI_ERR_NO_ERROR;

	*amount_gotten = 0U;

	if ( dcid != DCI_ID_UNDEFINED )
	{
		if ( amount == 0U )
		{
			status = m_workers->Get_Length( dcid, &amount );
			offset = 0U;
		}

		if ( status == DCI_ERR_NO_ERROR )
		{
			switch ( attribute )
			{
				case CURRENT_VALUE:
					status = m_workers->Get_Current( dcid, data, amount, offset );
					break;

				case INITIAL_VALUE:
					status = m_workers->Get_Init( dcid, data, amount, offset );
					break;

				case DEFAULT_VALUE:
					status = m_workers->Get_Default( dcid, data, amount, offset );
					break;

				case LENGTH:
					// TODO:  Clarify what is happening a little better here.  We are getting
					// the parameter length as a DCI Length type then storing it into amount.
					// From there we take that and load it into the buffer.  Then we reload it
					// with the actual amount "gotten".
					status = m_workers->Get_Length( dcid, &amount );
					Data_Align<uint16_t>::To_Array( amount, data );
					amount = LENGTH_LEN;
					break;

				case ATTRIBUTE_INFORMATION:
					// TODO:  This needs to be de-dcied.  It should be specific to DCOM not DCI.
					// Data_Align<DCI_LENGTH_TD>::To_Array( temp_owner->Get_Owner_Attrib(), data );
					status = DCI_ERR_INVALID_COMMAND;
					break;

				case MIN:
					status = m_workers->Get_Min( dcid, data, amount, offset );
					break;

				case MAX:
					status = m_workers->Get_Max( dcid, data, amount, offset );
					break;

				default:
					status = DCI_ERR_INVALID_COMMAND;
					break;
			}

			if ( status == DCI_ERR_NO_ERROR )
			{
				*amount_gotten = amount;
			}
		}
	}
	else
	{
		status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( Convert_DCI_Status( status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Data::Set_Data( Delta_Com::param_id_t pid, delta_com_attrib_t attribute,
										uint8_t* data, DCI_LENGTH_TD* amount_set,
										DCI_LENGTH_TD offset, DCI_LENGTH_TD amount )
{
	DCI_ERROR_TD status = DCI_ERR_NO_ERROR;
	DCI_ID_TD dcid;

	*amount_set = 0U;

	dcid = Convert_PID_To_DCID( pid );

	if ( dcid != DCI_ID_UNDEFINED )
	{
		if ( amount == 0U )
		{
			status = m_workers->Get_Length( dcid, &amount );
			offset = 0U;
		}

		if ( status == DCI_ERR_NO_ERROR )
		{
			switch ( attribute )
			{
				case CURRENT_VALUE:
					status = m_workers->Set_Current( dcid, data, amount, offset );
					break;

				case INITIAL_VALUE:
					status = m_workers->Set_Init( dcid, data, amount, offset );
					break;

				case DEFAULT_VALUE:
					status = m_workers->Set_Default( dcid, data, amount, offset );
					break;

				case LENGTH:
					amount = Data_Align<uint16_t>::From_Array( data );
					status = m_workers->Set_Length( dcid, amount );
					amount = LENGTH_LEN;
					break;

				case ATTRIBUTE_INFORMATION:
					// TODO:  This needs to be de-dcied.  It should be specific to DCOM not DCI.
					// Data_Align<DCI_LENGTH_TD>::To_Array( temp_owner->Get_Owner_Attrib(), data );
					status = DCI_ERR_INVALID_COMMAND;
					break;

				case MIN:
					status = m_workers->Set_Min( dcid, data, amount, offset );
					break;

				case MAX:
					status = m_workers->Set_Max( dcid, data, amount, offset );
					break;

				default:
					status = DCI_ERR_INVALID_COMMAND;
					break;
			}

			if ( status == DCI_ERR_NO_ERROR )
			{
				*amount_set = amount;
			}
		}
	}
	else
	{
		status = DCI_ERR_INVALID_DATA_ID;
	}

	return ( Convert_DCI_Status( status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Delta_Data::Get_PID_DCID_Map_Index_From_PID( Delta_Com::param_id_t parameter_id ) const
{
	Delta_Com::pid_to_dcid_map_t const* pid_to_dcid_map;
	uint16_t return_value = m_data_def->pid_dcid_list_len;
	uint_fast16_t begin = 0U;
	uint_fast16_t end;
	uint_fast16_t middle;

	pid_to_dcid_map = m_data_def->pid_to_dcid_map;
	end = static_cast<uint_fast16_t>( m_data_def->pid_dcid_list_len ) - 1U;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1U;
		if ( pid_to_dcid_map[middle].parameter_id < parameter_id )
		{
			begin = middle + 1U;
		}
		else if ( pid_to_dcid_map[middle].parameter_id > parameter_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				// We need to exit out here.  Set Begin to end + 1 to exit.
				// We did not find what we were looking for.
				begin = end + 1U;
			}
		}
		else
		{
			// We need to exit out here.  Set Begin to end + 1 to exit.
			return_value = static_cast<uint16_t>( middle );
			begin = end + 1U;
		}
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD Delta_Data::Convert_PID_To_DCID( Delta_Com::param_id_t parameter_id ) const
{
	Delta_Com::pid_to_dcid_map_t const* pid_to_dcid_map;
	DCI_ID_TD return_value = DCI_ID_UNDEFINED;
	uint_fast16_t begin = 0U;
	uint_fast16_t end;
	uint_fast16_t middle;

	pid_to_dcid_map = m_data_def->pid_to_dcid_map;
	end = static_cast<uint_fast16_t>( m_data_def->pid_dcid_list_len ) - 1U;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1U;
		if ( pid_to_dcid_map[middle].parameter_id < parameter_id )
		{
			begin = middle + 1U;
		}
		else if ( pid_to_dcid_map[middle].parameter_id > parameter_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				// We need to exit out here.  Set Begin to end + 1 to exit.
				// We did not find what we were looking for.
				begin = end + 1U;
			}
		}
		else
		{
			// We need to exit out here.  Set Begin to end + 1 to exit.
			return_value = pid_to_dcid_map[middle].dcid;
			begin = end + 1U;
		}
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com::param_id_t Delta_Data::Convert_DCID_to_PID( DCI_ID_TD dcid ) const
{
	Delta_Com::pid_to_dcid_map_t const* pid_to_dcid_map;
	uint16_t const* dcid_to_pid_map;
	Delta_Com::param_id_t return_value = Delta_Com::UNDEFINED_PARAMETER;
	uint_fast16_t begin = 0U;
	uint_fast16_t end;
	uint_fast16_t middle;

	pid_to_dcid_map = m_data_def->pid_to_dcid_map;
	dcid_to_pid_map = m_data_def->dcid_to_pid_map;

	end = static_cast<uint_fast16_t>( m_data_def->pid_dcid_list_len ) - 1U;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1U;
		if ( pid_to_dcid_map[dcid_to_pid_map[middle]].dcid < dcid )
		{
			begin = middle + 1U;
		}
		else if ( pid_to_dcid_map[dcid_to_pid_map[middle]].dcid > dcid )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				// We need to exit out here.  Set Begin to end + 1 to exit.
				// We did not find what we were looking for.
				begin = end + 1U;
			}
		}
		else
		{
			// We need to exit out here.  Set Begin to end + 1 to exit.
			return_value = pid_to_dcid_map[dcid_to_pid_map[middle]].parameter_id;
			begin = end + 1U;
		}
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
delta_com_error_t Delta_Data::Convert_DCI_Status( DCI_ERROR_TD dci_access_result ) const
{
	delta_com_error_t dcom_error;

	switch ( dci_access_result )
	{
		case DCI_ERR_NO_ERROR:
			dcom_error = NO_DCOM_ERROR;
			break;

		case DCI_ERR_INVALID_COMMAND:		///< Invalid command was requested.
			dcom_error = INVALID_COMMAND;
			break;

		case DCI_ERR_INVALID_DATA_ID:		///< Data ID does not exist.
			dcom_error = INVALID_PARAMETER;
			break;

		case DCI_ERR_ACCESS_VIOLATION:		///< Target data does not exist.  Null ram ptr.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_EXCEEDS_RANGE:			///< The set value resulted in a range error.
			dcom_error = BAD_FRAME_LENGTH;
			break;

		case DCI_ERR_RAM_READ_ONLY:		///< This value is read only.  No write access is allowed.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_NV_NOT_AVAILABLE:		///< NV Value does not exist or is inaccessible.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_NV_ACCESS_FAILURE:		///< NV is Busy or Broken.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_NV_READ_ONLY:			///< The NV value is listed as read only.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_DEFAULT_NOT_AVAILABLE:	///< The Default is not available.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_RANGE_NOT_AVAILABLE:	///< The Range is not available.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_ENUM_NOT_AVAILABLE:		///< The Enum is not available.
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_INVALID_DATA_LENGTH:	///< The data length is invalid.
			dcom_error = INVALID_PARAMETER_LENGTH;
			break;

		case DCI_ERR_VALUE_LOCKED:
			dcom_error = ACCESS_VIOLATION;
			break;

		case DCI_ERR_TOTAL_ERRORS:
		default:
			dcom_error = UNKNOWN_ERROR;
			break;
	}

	return ( dcom_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
   //DEPRECATE.  This function is no longer used.  Kept here for reference in future.
   uint16_t Delta_Data::Get_PID_DCID_Map_Index_From_DCID( DCI_ID_TD dcid )
   {
    uint16_t index = m_data_def->pid_dcid_list_len;	//This is considered invalid.
    int_fast16_t low = 0;
    int_fast16_t high = static_cast<int_fast16_t>( m_data_def->pid_dcid_list_len );
    int_fast16_t middle = static_cast<int_fast16_t>( m_data_def->pid_dcid_list_len ) / 2;
    Delta_Com::pid_to_dcid_map_t const* pid_to_dcid_map;
    uint16_t const* dcid_to_pid_map;

    pid_to_dcid_map = m_data_def->pid_to_dcid_map;
    dcid_to_pid_map = m_data_def->dcid_to_pid_map;

    while ( low <= high )
    {
        if ( pid_to_dcid_map[dcid_to_pid_map[middle]].dcid == dcid )
        {
            index = dcid_to_pid_map[middle];
            low = middle + 1;
            high = middle - 1;
        }
        else if ( pid_to_dcid_map[dcid_to_pid_map[middle]].dcid < dcid )
        {
            low = middle + 1;
            middle = ( high + low ) / 2;
        }
        else
        {
            high = middle - 1;
            middle = ( high + low ) / 2;
        }
    }
    return ( index );
   }
 */
