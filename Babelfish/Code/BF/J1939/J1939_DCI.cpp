/**
 **********************************************************************************************
 *	@file            J1939_DCI_Interface.cpp C++ Implementation File j1939 applicaition
 *					 interface with DCI.
 *
 *	@brief           The file shall include the definitions of all the functions required for
 *                   updating and fetching the dcid parameters via J1939 application
 *	@details
 *	@copyright 		 2019 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */


#include "J1939_DCI.h"
#include "J1939_DCI_Data.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
J1939_DCI::J1939_DCI()
{
	m_patron = new DCI_Patron( false );
	m_source_id = DCI_SOURCE_IDS_Get();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD J1939_DCI::Get_DCID_Value( DCI_ID_TD dcid, uint64_t* data )
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = NULL;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD J1939_DCI::Set_DCID_Value( DCI_ID_TD dcid, uint64_t* data )
{
	DCI_ERROR_TD return_status;		// = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
J1939_DCI::j1939_error_codes_t J1939_DCI::Get_J1939_Message( j1939_t* msg )
{
	j1939_error_codes_t ret_status = NO_ERROR;
	DCI_ERROR_TD dci_read_status = DCI_ERR_NO_ERROR;
	J1939_DCI_STRUCT* current_pgn = nullptr;
	uint16_t total_dcid_count = 0U;
	uint64_t temp_dcid_value = 0U;

	current_pgn = Find_J1939_Msg_Struct( msg->pgn );

	if ( current_pgn != nullptr )
	{
		total_dcid_count = current_pgn->dcid_count;
		uint16_t bit_pos = 0;
		/* Make sure to clear all CAN message buffer */
		for ( uint8_t i = 0U; i < 8U; i++ )
		{
			msg->buf[i] = 0x00;
		}

		// we will go through all the dcids and start filling them into the buffer.
		for ( DCI_ID_TD dcid_count = 0;
			  ( ( dcid_count < total_dcid_count ) && ( dci_read_status == DCI_ERR_NO_ERROR ) );
			  dcid_count++ )
		{
			dci_read_status = Get_DCID_Value( current_pgn->dcid_array[dcid_count], &temp_dcid_value );
			Mask_And_Insert( &temp_dcid_value, msg->buf, current_pgn->bitlength[dcid_count], &bit_pos );
			bit_pos += current_pgn->bitlength[dcid_count];
			temp_dcid_value = 0U;
		}

		if ( dci_read_status != DCI_ERR_NO_ERROR )
		{
			ret_status = DCID_GET_FAILED;
		}
	}
	else
	{
		ret_status = PGN_NOT_FOUND;
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
J1939_DCI_STRUCT* J1939_DCI::Find_J1939_Msg_Struct( uint32_t pgn )
{
	J1939_DCI_STRUCT* pgn_handler = nullptr;
	uint16_t msg_count = 0U;
	bool_t pgn_found = false;

	while ( ( msg_count < TOTAL_J1939_MESSAGES ) && ( pgn_found != true ) )
	{
		if ( base_dci_j1939_msg_list[msg_count].pgn == pgn )
		{
			pgn_handler = const_cast<J1939_DCI_STRUCT*>( &base_dci_j1939_msg_list[msg_count] );
			pgn_found = true;
		}
		msg_count++;
	}
	return ( pgn_handler );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
J1939_DCI::j1939_error_codes_t J1939_DCI::Set_J1939_Message( j1939_t* msg )
{
	j1939_error_codes_t ret_status = NO_ERROR;
	DCI_ERROR_TD dci_read_status = DCI_ERR_NO_ERROR;
	J1939_DCI_STRUCT* current_pgn = nullptr;
	uint16_t total_dcid_count = 0U;
	uint64_t temp_dcid_value = 0U;

	current_pgn = Find_J1939_Msg_Struct( msg->pgn );

	if ( current_pgn != nullptr )
	{
		total_dcid_count = current_pgn->dcid_count;
		uint16_t bit_pos = 0;

		// We will go through all the dcids and start extracting them from the buffer.
		for ( DCI_ID_TD dcid_count = 0;
			  ( ( dcid_count < total_dcid_count ) && ( dci_read_status == DCI_ERR_NO_ERROR ) );
			  dcid_count++ )
		{
			Mask_And_Extract( msg->buf, &temp_dcid_value, current_pgn->bitlength[dcid_count], &bit_pos );
			dci_read_status = Set_DCID_Value( current_pgn->dcid_array[dcid_count], &temp_dcid_value );
			bit_pos += current_pgn->bitlength[dcid_count];
			temp_dcid_value = 0U;
		}

		if ( dci_read_status != DCI_ERR_NO_ERROR )
		{
			ret_status = DCID_SET_FAILED;
		}
	}
	else
	{
		ret_status = PGN_NOT_FOUND;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void J1939_DCI::Mask_And_Extract( uint8_t src[], uint64_t* dest, uint8_t bit_len, uint16_t* bit_offset )
{
	uint8_t current_byte = 0;
	uint8_t pos_in_byte = 0;
	uint8_t total_read_bit = 0;

	current_byte = ( *bit_offset / 8 );		/* Find out the byte number from the total 8 byte */
	pos_in_byte = ( *bit_offset % 8 );		/* Find the position in that byte */

	if ( ( 8 - pos_in_byte ) >= bit_len )
	{
		/* data is present in a single byte */
		( *dest ) = static_cast<uint64_t>( src[current_byte] & ( ( 0xFF >> ( 8U - bit_len ) ) << pos_in_byte ) ) >>
			pos_in_byte;
		bit_len = 0;
	}
	else
	{
		/* When data is present in multiple byte then read 1st byte*/
		( *dest ) |= static_cast<uint64_t>( src[current_byte] & ( ( 0xFF >> pos_in_byte ) << pos_in_byte ) ) >>
			pos_in_byte;
		bit_len -= ( 8U - pos_in_byte );
		total_read_bit += ( 8U - pos_in_byte );
		pos_in_byte = 0;
		current_byte++;

		while ( bit_len != 0U )
		{
			if ( bit_len <= 8U )
			{
				/* This is the last byte we need to read */
				( *dest ) |= static_cast<uint64_t>( src[current_byte] & ( 0xFF >> ( 8U - bit_len ) ) ) <<
					total_read_bit;
				bit_len -= bit_len;
			}
			else
			{
				/* We have to read complete byte as the length is greater than 8 */
				( *dest ) |= static_cast<uint64_t>( src[current_byte] & 0xFF ) << total_read_bit;
				bit_len -= 8U;
				total_read_bit += 8U;
				current_byte++;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void J1939_DCI::Mask_And_Insert( uint64_t* src, uint8_t dest[], uint8_t bit_len, uint16_t* bit_offset )
{
	uint8_t current_byte = 0;
	uint8_t pos_in_byte = 0;
	uint8_t total_write_bit = 0;

	current_byte = ( *bit_offset / 8 );		/* Find out the byte number from the total 8 byte */
	pos_in_byte = ( *bit_offset % 8 );		/* Find the position in current byte */

	if ( ( 8 - pos_in_byte ) >= bit_len )
	{
		/* data is going to be in a single byte */
		dest[current_byte] |= static_cast<uint8_t>( ( *src ) & ( 0xFF >> ( 8U - bit_len ) ) ) << pos_in_byte;
		bit_len = 0;
	}
	else
	{
		/* When data is going to be in multiple byte then write 1st byte*/
		dest[current_byte] |= static_cast<uint8_t>( ( *src ) & ( 0xFF >> pos_in_byte ) ) << pos_in_byte;
		bit_len -= ( 8U - pos_in_byte );
		total_write_bit += ( 8U - pos_in_byte );
		pos_in_byte = 0;
		current_byte++;

		while ( bit_len != 0U )
		{
			if ( bit_len <= 8U )
			{
				/* This is the last byte we need to write */
				dest[current_byte] |=
					static_cast<uint8_t>( ( ( *src ) >> total_write_bit ) & ( 0xFF >> ( 8U - bit_len ) ) );
				bit_len -= bit_len;
			}
			else
			{
				/* We have to write complete byte as the length is greater than 8 */
				dest[current_byte] |= static_cast<uint8_t>( ( ( *src ) >> total_write_bit ) & 0xFF );
				bit_len -= 8U;
				total_write_bit += 8U;
				current_byte++;
			}
		}
	}
}
