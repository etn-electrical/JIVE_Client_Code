/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "IoT_DCI.h"
#include <stdlib.h>
#include "DCI_Defines.h"
#include "INT_ASCII_Conversion.h"
#include "ASCII_Conversion.h"
#include "iot_device_platform.h"
#include "Babelfish_Assert.h"
#include "Timers_Lib.h"
#include "IOT_Debug.h"
#ifdef ESP32_SETUP
#include "Test_Example.h"
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_DCI::IOT_DCI( const iot_target_info_st_t* iot_profile, DCI_Patron* patron,
				  DCI_SOURCE_ID_TD unique_id ) :
	m_dci_access( patron ),
	m_iot_struct( iot_profile ),
	m_source_id( unique_id )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IOT_DCI::~IOT_DCI( void )
{
	// Nothing to do on destruct
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const dci_iot_to_dcid_lkup_st_t* IOT_DCI::Find_Index_IOT( const uint16_t tag_id_index )
{
	const dci_iot_to_dcid_lkup_st_t* search_struct = m_iot_struct->channel_list;
	const dci_iot_to_dcid_lkup_st_t* return_val = nullptr;

	if ( tag_id_index < m_iot_struct->total_channels )
	{
		return_val = ( &search_struct[tag_id_index] );
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_DCI::Find_Index_From_Tag_Str( const char_t* tag_str, uint8_t length )
{
	uint8_t return_val;
	uint16_t index = IOT_INPUT_ERROR;
	bool_t loop_break = false;
	iot_tag_id_t tag_id = 0U;

	return_val = INT_ASCII_Conversion::str_to_uint32(
		reinterpret_cast<const uint8_t*>( tag_str ),
		0U, ( length - 1 ),
		reinterpret_cast<uint32_t*>( &tag_id ),
		false );
	if ( return_val == ERR_PARSING_OK )
	{
		for ( uint16_t i = 0U; ( i < m_iot_struct->total_channels && loop_break == false ); i++ )
		{
			const iot_channel_config_struct_t* channel_config = Get_Channel_Config( i );
			if ( ( channel_config != nullptr ) && ( channel_config->channel_tag == tag_id ) )
			{
				index = i;
				loop_break = true;
			}
		}
	}
	return ( index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const char_t* IOT_DCI::Find_Tag_Str_From_Index( uint16_t tag_index )
{
	const char_t* return_val = nullptr;
	const iot_channel_config_struct_t* channel_config = Get_Channel_Config( tag_index );

	if ( channel_config != nullptr )
	{
		return_val = channel_config->channel_tag_str;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_DCI::Find_Index_From_DCID( DCI_ID_TD iot_dcid )
{
	uint16_t index = IOT_INPUT_ERROR;
	bool_t loop_break = false;

	for ( uint16_t i = 0U; ( i < m_iot_struct->total_channels && loop_break == false ); i++ )
	{
		const iot_channel_config_struct_t* channel_config = Get_Channel_Config( i );
		if ( ( channel_config != nullptr ) && ( channel_config->dcid == iot_dcid ) )
		{
			index = i;
			loop_break = true;
		}
	}
	return ( index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_LENGTH_TD IOT_DCI::Get_Length( DCI_ID_TD dcid ) const
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
DCI_DATATYPE_TD IOT_DCI::Get_Datatype( DCI_ID_TD dcid ) const
{
	DCI_DATATYPE_TD return_datatype = DCI_DTYPE_MAX_TYPES;
	DCI_ERROR_TD error = DCI_ERR_NO_ERROR;

	error = m_dci_access->Get_Datatype( dcid, &return_datatype );
	if ( error != DCI_ERR_NO_ERROR )
	{
		return_datatype = DCI_DTYPE_MAX_TYPES;
	}
	return ( return_datatype );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t IOT_DCI::Get_Total_Channels( void ) const
{
	return ( m_iot_struct->total_channels );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD IOT_DCI::Get_Tag_Value( uint16_t tag_id_index, uint8_t* dest_data,
									 uint16_t* data_length )
{
	DCI_ERROR_TD error = DCI_ERR_INVALID_DATA_ID;
	const dci_iot_to_dcid_lkup_st_t* tag_index_addr = Find_Index_IOT( tag_id_index );

	if ( tag_index_addr != nullptr )
	{
		DCI_ID_TD dcid = tag_index_addr->dcid;
		error = Get_DCI_Value( dcid, tag_index_addr->array_offset, dest_data, data_length );
	}
	return ( error );
}

// Read the data value for the given DCI ID.
DCI_ERROR_TD IOT_DCI::Get_DCI_Value( DCI_ID_TD dcid, DCI_LENGTH_TD array_offset,
									 uint8_t* dest_data, uint16_t* data_length )
{
	DCI_ERROR_TD error = DCI_ERR_NO_ERROR;
	DCI_DATATYPE_TD datatype;
	uint8_t get_buffer[sizeof( uint64_t )];

	error = m_dci_access->Get_Datatype( dcid, &datatype );

	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.offset = array_offset;

	if ( error == DCI_ERR_NO_ERROR )
	{
		if ( datatype == DCI_DTYPE_STRING8 )
		{
			DCI_LENGTH_TD length = 0U;
			m_dci_access->Get_Length( dcid, &length );
			access_struct.data_access.length = length - 1U;
			access_struct.data_access.data = dest_data;
		}
		else
		{
			access_struct.data_access.length = DCI::Get_Datatype_Size( datatype );
			access_struct.data_access.data = get_buffer;
		}

		error = m_dci_access->Data_Access( &access_struct );

		if ( error == DCI_ERR_NO_ERROR )
		{
			switch ( datatype )
			{
				case DCI_DTYPE_BOOL:
				case DCI_DTYPE_BYTE:
				case DCI_DTYPE_UINT8:
				case DCI_DTYPE_UINT16:
				case DCI_DTYPE_UINT32:
				case DCI_DTYPE_UINT64:
				case DCI_DTYPE_WORD:
				case DCI_DTYPE_DWORD:
				case DCI_DTYPE_SINT8:
				case DCI_DTYPE_SINT16:
				case DCI_DTYPE_SINT32:
				case DCI_DTYPE_SINT64:
				case DCI_DTYPE_FLOAT:
				case DCI_DTYPE_DFLOAT:
					*data_length = Convert_To_String( get_buffer, dest_data, datatype, 0U, TRUE );
					break;

				case DCI_DTYPE_STRING8:
					*data_length = access_struct.data_access.length;
					break;

				default:
					break;
			}
		}
	}

	return ( error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD IOT_DCI::Set_Tag_Value( uint16_t tag_id_index, uint8_t* src_data,
									 uint16_t* data_length )
{
	DCI_ERROR_TD error = DCI_ERR_NO_ERROR;
	DCI_ERROR_TD attrib_error = DCI_ERR_NO_ERROR;
	uint8_t return_val = ERR_PARSING_INPUT_ERROR;
	DCI_PATRON_ATTRIB_SUPPORT_TD attrib_support;
	DCI_ID_TD dcid;
	DCI_DATATYPE_TD datatype;
	uint8_t set_buffer[sizeof( uint64_t )];

	uint16_t first_digit = 0U;
	uint16_t last_digit = *data_length - 1U;

	const dci_iot_to_dcid_lkup_st_t* tag_index_addr = Find_Index_IOT( tag_id_index );

	if ( tag_index_addr != nullptr )
	{
		dcid = tag_index_addr->dcid;

		error = m_dci_access->Get_Datatype( dcid, &datatype );

		DCI_ACCESS_ST_TD access_struct;

		access_struct.data_id = dcid;
		access_struct.source_id = m_source_id;
		access_struct.command = DCI_ACCESS_SET_RAM_CMD;
		access_struct.data_access.offset = tag_index_addr->array_offset;

		if ( error == DCI_ERR_NO_ERROR )
		{
			switch ( datatype )
			{
				case DCI_DTYPE_BOOL:
					if ( ( ( src_data[0] == 'T' ) || ( src_data[0] == 't' ) ) &&
						 ( ( src_data[1] == 'R' ) || ( src_data[1] == 'r' ) ) &&
						 ( ( src_data[2] == 'U' ) || ( src_data[2] == 'u' ) ) &&
						 ( ( src_data[3] == 'E' ) || ( src_data[3] == 'e' ) ) )
					{
						set_buffer[0] = 1U;
						return_val = ERR_PARSING_OK;
					}
					else if ( ( ( src_data[0] == 'F' ) || ( src_data[0] == 'f' ) ) &&
							  ( ( src_data[1] == 'A' ) || ( src_data[1] == 'a' ) ) &&
							  ( ( src_data[2] == 'L' ) || ( src_data[2] == 'l' ) ) &&
							  ( ( src_data[3] == 'S' ) || ( src_data[3] == 's' ) ) &&
							  ( ( src_data[4] == 'E' ) || ( src_data[4] == 'e' ) ) )
					{
						set_buffer[0] = 0U;
						return_val = ERR_PARSING_OK;
					}
					else
					{
						return_val = ERR_PARSING_INPUT_ERROR;
					}

					break;

				case DCI_DTYPE_BYTE:
				case DCI_DTYPE_UINT8:
					return_val = INT_ASCII_Conversion::str_to_uint8( src_data, first_digit,
																	 last_digit,
																	 set_buffer, false );
					break;

				case DCI_DTYPE_SINT8:
					return_val = INT_ASCII_Conversion::str_to_sint8( src_data, first_digit,
																	 last_digit,
																	 reinterpret_cast<int8_t*>(
																		 set_buffer ),
																	 false );
					break;

				case DCI_DTYPE_WORD:
				case DCI_DTYPE_UINT16:
					return_val = INT_ASCII_Conversion::str_to_uint16( src_data, first_digit,
																	  last_digit,
																	  reinterpret_cast<uint16_t*>(
																		  set_buffer ),
																	  false );
					break;

				case DCI_DTYPE_SINT16:
					return_val = INT_ASCII_Conversion::str_to_sint16( src_data, first_digit,
																	  last_digit,
																	  reinterpret_cast<int16_t*>(
																		  set_buffer ),
																	  false );
					break;

				case DCI_DTYPE_DWORD:
				case DCI_DTYPE_UINT32:
					return_val = INT_ASCII_Conversion::str_to_uint32( src_data, first_digit,
																	  last_digit,
																	  reinterpret_cast<uint32_t*>(
																		  set_buffer ),
																	  false );
					break;

				case DCI_DTYPE_SINT32:
					return_val = INT_ASCII_Conversion::str_to_sint32( src_data, first_digit,
																	  last_digit,
																	  reinterpret_cast<int32_t*>(
																		  set_buffer ),
																	  false );
					break;

				case DCI_DTYPE_UINT64:
					return_val = INT_ASCII_Conversion::str_to_uint64( src_data, first_digit,
																	  last_digit,
																	  reinterpret_cast<uint64_t*>(
																		  set_buffer ),
																	  false );
					break;

				case DCI_DTYPE_SINT64:
					return_val = INT_ASCII_Conversion::str_to_sint64( src_data, first_digit,
																	  last_digit,
																	  reinterpret_cast<int64_t*>(
																		  set_buffer ),
																	  false );
					break;

				case DCI_DTYPE_FLOAT:
					float32_t* float_dest;
					float_dest = reinterpret_cast<float32_t*>( set_buffer );
					*float_dest = strtod( reinterpret_cast<char*>( src_data ), nullptr );
					return_val = ERR_PARSING_OK;
					break;

				case DCI_DTYPE_DFLOAT:
					float64_t* double_dest;
					double_dest = reinterpret_cast<float64_t*>( set_buffer );
					*double_dest = strtod( reinterpret_cast<char*>( src_data ), nullptr );
					return_val = ERR_PARSING_OK;
					break;

				case DCI_DTYPE_STRING8:
					// No conversion required for string type, hence return NO ERROR.
					return_val = ERR_PARSING_OK;

				// no break
				default:
					break;
			}

			if ( return_val == ERR_PARSING_OK )
			{
				if ( datatype == DCI_DTYPE_STRING8 )
				{
					DCI_LENGTH_TD length = 0U;
					m_dci_access->Get_Length( dcid, &length );
					if ( *data_length > ( length - 1 ) )
					{
						return_val = ERR_PARSING_INPUT_ERROR;
						error = DCI_ERR_INVALID_DATA_LENGTH;
					}
					else
					{
						src_data[*data_length] = '\0';
						access_struct.data_access.length = *data_length + 1;
						access_struct.data_access.data = src_data;
					}
				}
				else
				{
					access_struct.data_access.length = DCI::Get_Datatype_Size( datatype );
					access_struct.data_access.data = set_buffer;
				}

				if ( return_val == ERR_PARSING_OK )
				{
					error = m_dci_access->Data_Access( &access_struct );

					if ( error == DCI_ERR_NO_ERROR )
					{
#ifdef ESP32_SETUP		// Test code for getting time when command is received on device from cloud
						#ifdef IOT_C2D_TIMING_TEST
						BF_Lib::Timers::TIMERS_TIME_TD begin_time = 0U;
						begin_time = BF_Lib::Timers::Get_Time();
						IOT_DEBUG_PRINT( DBG_MSG_INFO, "Received value = %s\n", src_data );
						IOT_DEBUG_PRINT( DBG_MSG_INFO, "Received value time = %u\n", begin_time );
						#endif
#endif	// #ifdef ESP32_SETUP
						attrib_error = m_dci_access->Get_Attribute_Support( dcid, &attrib_support );
						if ( Test_Bit( attrib_support, DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) &&
							 ( attrib_error == DCI_ERR_NO_ERROR ) )
						{
							access_struct.command = DCI_ACCESS_SET_INIT_CMD;
							error = m_dci_access->Data_Access( &access_struct );
						}
					}
				}
			}
			else
			{
				error = DCI_ERR_DATA_INCORRECT;
			}
		}
	}
	else
	{
		error = DCI_ERR_INVALID_DATA_ID;
	}

	return ( error );
}

// Get the IoT channel's configuration, given its index.
const iot_channel_config_struct_t* IOT_DCI::Get_Channel_Config( uint16_t index ) const
{
	static const iot_channel_config_struct_t* channel_list = m_iot_struct->channel_list;
	static const iot_channel_config_struct_t* channel_config = nullptr;

	BF_ASSERT( index < m_iot_struct->total_channels );

	if ( index < m_iot_struct->total_channels )
	{
		channel_config = &channel_list[index];
		// IOT_DEBUG_PRINT( DBG_MSG_DEBUG, "Channel idx: %u, tag: %u ?= %s, DCID: %u, groups: %u", index,
		// channel_config->channel_tag,
		// channel_config->channel_tag_str, channel_config->dcid, channel_config->group_memberships );
	}
	return ( channel_config );
}

// Get the IoT Cadence Group configuration, given its index.
const iot_cadence_group_struct_t* IOT_DCI::Get_Group_Config( uint16_t index ) const
{
	const iot_cadence_group_struct_t* group_list = m_iot_struct->group_list;
	const iot_cadence_group_struct_t* group_config = nullptr;

	BF_ASSERT( index < m_iot_struct->total_groups );

	if ( index < m_iot_struct->total_groups )
	{
		group_config = &group_list[index];
	}
	return ( group_config );
}
