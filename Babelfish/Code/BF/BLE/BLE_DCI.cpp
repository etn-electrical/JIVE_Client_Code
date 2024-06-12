/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Ble_DCI.h"
#include "StdLib_MV.h"
#include "BLE_Debug.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ble_DCI::Ble_DCI( const ble_target_info_st_td_t* ble_dev_profile,
				  const ble_group_info_st_td_t* ble_group_profile,
				  DCI_SOURCE_ID_TD unique_id ) :
	m_source_id( unique_id ),
	m_dci_access( new DCI_Patron( false ) ),
	m_dev_profile( ble_dev_profile ),

	m_group_profile( ble_group_profile )
{
	m_individual_readvalue = reinterpret_cast<uint8_t*>( Ram::Allocate( static_cast<size_t>( BLE_PACKET_PAYLOAD ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ble_DCI::~Ble_DCI( void )
{
	delete m_dci_access;
	if ( m_individual_readvalue != nullptr )
	{
		Ram::De_Allocate( m_individual_readvalue );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Ble_DCI::Get_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD* length_ptr,
									 uint8_t* dest_data_ptr, DCI_LENGTH_TD offset )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.data = dest_data_ptr;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	access_struct.data_access.offset = offset;
	dci_error = m_dci_access->Data_Access( &access_struct );
	*length_ptr = access_struct.data_access.length;

	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_LENGTH_TD Ble_DCI::Get_Length( DCI_ID_TD dcid ) const
{
	DCI_LENGTH_TD return_length = 0U;

	m_dci_access->Get_Length( dcid, &return_length );

	return ( return_length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Ble_DCI::Set_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD length,
									 uint8_t* src_data_ptr )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	DCI_PATRON_ATTRIB_SUPPORT_TD attrib_support = 0U;
	DCI_ERROR_TD attrib_error = DCI_ERR_NO_ERROR;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.data_access.data = src_data_ptr;
	access_struct.data_access.length = length;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	dci_error = m_dci_access->Data_Access( &access_struct );
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		attrib_error = m_dci_access->Get_Attribute_Support( dcid, &attrib_support );
		if ( Test_Bit( attrib_support, DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) &&
			 ( attrib_error == DCI_ERR_NO_ERROR ) )
		{
			access_struct.command = DCI_ACCESS_SET_INIT_CMD;
			dci_error = m_dci_access->Data_Access( &access_struct );
		}
	}
	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#ifdef ESP32_SETUP
DCI_ID_TD Ble_DCI::Find_Ble_Mod_DCID( CHAR_ID_TD ble_unique_id, uint8_t* read_access,
									  uint8_t* write_access )
{
	const dci_ble_to_dcid_lkup_st_td_t* search_struct;
	DCI_ID_TD dcid = DCI_ID_UNDEFINED;
	uint_fast16_t begin = 0U;
	uint_fast16_t end;
	uint_fast16_t index;

	search_struct = m_dev_profile->ble_to_dcid_struct;
	end = static_cast<uint_fast16_t>( m_dev_profile->total_char_ids );
	for ( index = begin; index < end; index++ )
	{
		if ( search_struct[index].ble_char_id == ble_unique_id )
		{
			dcid = search_struct[index].dcid;
			*read_access = search_struct[index].read;
			*write_access = search_struct[index].write;
			break;
		}
	}

	return ( dcid );
}

#else
DCI_ID_TD Ble_DCI::Find_Ble_Mod_DCID( CHAR_ID_TD ble_unique_id, uint8_t* read_access,
									  uint8_t* write_access )
{
	const dci_ble_to_dcid_lkup_st_td_t* search_struct;
	DCI_ID_TD dcid = DCI_ID_UNDEFINED;
	bool end_loop = false;
	uint_fast16_t begin = 0U;
	uint_fast16_t end;
	uint_fast16_t middle;

	search_struct = m_dev_profile->ble_to_dcid_struct;
	end = static_cast<uint_fast16_t>( m_dev_profile->total_char_ids ) - 1U;
	while ( ( begin <= end ) && ( end_loop == FALSE ) )
	{
		middle = ( begin + end ) >> 1U;

		if ( search_struct[middle].ble_char_id < ble_unique_id )
		{
			begin = middle + 1U;
		}
		else if ( search_struct[middle].ble_char_id > ble_unique_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				end_loop = TRUE;
			}
		}
		else
		{
			end_loop = TRUE;
			dcid = search_struct[middle].dcid;
			*read_access = search_struct[middle].read;
			*write_access = search_struct[middle].write;
		}
	}
	return ( dcid );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Ble_DCI::Group_Parameters_Assembly( CHAR_ID_TD ble_unique_id,
												 DCI_LENGTH_TD* length_ptr, uint8_t* dest_data_ptr,
												 uint8_t active_access_level )
{
	uint16_t ble_unique_id_each_par, length = 0U;
	uint8_t group_index_count = 0U, index = 0U;
	DCI_ID_TD dcid;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	const uint8_t ACCESS_INVALID = 0xFF;
	const uint8_t NO_ACCESS = 0U;
	uint8_t read_access = ACCESS_INVALID, write_access = ACCESS_INVALID;

	// This condition will check whether the unique_id belongs to group or to an individual
	// parameter and retrive all parameter data belongs to the group/assembly
	if ( ble_unique_id >= m_group_profile->ble_dci_group_struct[0].group_dcid )
	{
		// This will calculate the group index in the structure array BLE_GROUP_ASSEMBLY.
		group_index_count = ( ble_unique_id - ( m_group_profile->ble_dci_group_struct[0].group_dcid ) ) / 2U;

		// This condition will cross verify the ble_unique_id and group_dcid are same or not.
		if ( m_group_profile->ble_dci_group_struct[group_index_count].group_dcid == ble_unique_id )
		{
			// This loop will run till the total parameters inside the assembly/group taken from the
			// structure based on the group index count.
			for ( int i = 0; i < m_group_profile->ble_dci_group_struct[group_index_count].total_parameters; i++ )
			{
				ble_unique_id_each_par = m_group_profile->ble_dci_group_struct[group_index_count].ble_dcid[i];
				dcid = Find_Ble_Mod_DCID( ble_unique_id_each_par, &read_access, &write_access );
				if ( dcid != DCI_ID_UNDEFINED )
				{
					if ( ( ( read_access != NO_ACCESS ) && ( read_access != ACCESS_INVALID ) ) &&
						 ( active_access_level >= read_access ) )
					{
						dci_error = Get_Dcid_Data( dcid,
												   reinterpret_cast<DCI_LENGTH_TD*>( &length ),
												   reinterpret_cast<uint8_t*>( m_individual_readvalue ) );
						if ( dci_error != DCI_ERR_NO_ERROR )
						{
							BLE_DEBUG_PRINT( DBG_MSG_INFO, "Read Access Not Permitted\n" );
							dci_error = DCI_ERR_NO_ERROR;
						}
						else
						{
							BF_Lib::Copy_String( dest_data_ptr + index, m_individual_readvalue, length );
							index += length;
						}
					}
					else
					{
						BLE_DEBUG_PRINT( DBG_MSG_INFO, "Read Access Not Permitted\n" );
					}
				}
				else
				{
					BLE_DEBUG_PRINT( DBG_MSG_ERROR, "DCID is invalid\n" );
					dci_error = DCI_ERR_INVALID_DATA_ID;
				}
			}
			*length_ptr = index;
		}
		else
		{
			dci_error = DCI_ERR_INVALID_DATA_ID;
		}
	}
	else// This else path will help in retriving data as individual parameter
	{
		dcid = Find_Ble_Mod_DCID( ble_unique_id, &read_access, &write_access );
		if ( dcid != DCI_ID_UNDEFINED )
		{
			if ( ( ( read_access != NO_ACCESS ) && ( read_access != ACCESS_INVALID ) ) &&
				 ( active_access_level >= read_access ) )
			{
				dci_error = Get_Dcid_Data( dcid,
										   reinterpret_cast<DCI_LENGTH_TD*>( &length ),
										   reinterpret_cast<uint8_t*>( m_individual_readvalue ) );
				BF_Lib::Copy_String( dest_data_ptr, m_individual_readvalue, length );
				*length_ptr = length;
			}
			else
			{
				BLE_DEBUG_PRINT( DBG_MSG_INFO, "Read Access Not Permitted\n" );
				dci_error = DCI_ERR_ACCESS_VIOLATION;
			}
		}
		else
		{
			BLE_DEBUG_PRINT( DBG_MSG_ERROR, "DCID is invalid\n" );
			dci_error = DCI_ERR_INVALID_DATA_ID;
		}
	}

	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Ble_DCI::Interpret_DCI_Error( DCI_ERROR_TD dci_error ) const
{
	uint8_t return_status;

	switch ( dci_error )
	{
		case DCI_ERR_NO_ERROR:
			return_status = BLE_NO_ERROR_CODE;
			break;

		case DCI_ERR_EXCEEDS_RANGE:
			return_status = BLE_EXCEEDS_RANGE_ERROR_CODE;
			break;

		case DCI_ERR_INVALID_DATA_LENGTH:
			return_status = BLE_INVALID_DATA_LENGTH_ERROR_CODE;
			break;

		case DCI_ERR_INVALID_DATA_ID:
			return_status = BLE_INVALID_DATA_ID_ERROR_CODE;
			break;

		case DCI_ERR_INVALID_COMMAND:
			return_status = BLE_INVALID_COMMAND_ERROR_CODE;
			break;

		case DCI_ERR_RAM_READ_ONLY:
			return_status = BLE_RAM_READ_ONLY_ERROR_CODE;
			break;

		case DCI_ERR_NV_READ_ONLY:
			return_status = BLE_NV_READ_ONLY_ERROR_CODE;
			break;

		case DCI_ERR_NV_ACCESS_FAILURE:
			return_status = BLE_NV_ACCESS_FAILURE_ERROR_CODE;
			break;

		case DCI_ERR_ACCESS_VIOLATION:
			return_status = BLE_ACCESS_VIOLATION_ERROR_CODE;
			break;

		case DCI_ERR_NV_NOT_AVAILABLE:
			return_status = BLE_NV_NOT_AVAILABLE_ERROR_CODE;
			break;

		case DCI_ERR_DEFAULT_NOT_AVAILABLE:
			return_status = BLE_DEFAULT_NOT_AVAILABLE_ERROR_CODE;
			break;

		case DCI_ERR_RANGE_NOT_AVAILABLE:
			return_status = BLE_RANGE_NOT_AVAILABLE_ERROR_CODE;
			break;

		case DCI_ERR_ENUM_NOT_AVAILABLE:
			return_status = BLE_ENUM_NOT_AVAILABLE_ERROR_CODE;
			break;

		case DCI_ERR_VALUE_LOCKED:
			return_status = BLE_VALUE_LOCKED_ERROR_CODE;
			break;

		case DCI_ERR_TOTAL_ERRORS:
			return_status = BLE_TOTAL_ERRORS_ERROR_CODE;
			break;

		default:
			return_status = BLE_OTHER_DCID_ERROR_CODE;
			break;
	}

	return ( return_status );
}
