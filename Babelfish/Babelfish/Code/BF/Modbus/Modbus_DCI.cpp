/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_DCI.h"
#include "Modbus_Defines.h"

const DCI_MBUS_TO_DCID_LKUP_ST_TD* const Modbus_DCI::LOOKUP_REG_TO_DCI_NOT_FOUND =
	reinterpret_cast<DCI_MBUS_TO_DCID_LKUP_ST_TD const*>( nullptr );
const DCI_MBUS_FROM_DCID_LKUP_ST_TD* const Modbus_DCI::LOOKUP_DCI_TO_REG_NOT_FOUND =
	reinterpret_cast<DCI_MBUS_FROM_DCID_LKUP_ST_TD const*>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_DCI::Modbus_DCI( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
						DCI_SOURCE_ID_TD unique_id,
						bool correct_out_of_range_sets ) :
	m_source_id( unique_id ),
	m_dci_access( new DCI_Patron( correct_out_of_range_sets ) ),
	m_dev_profile( modbus_dev_profile )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_DCI::Modbus_DCI( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
						DCI_Patron* patron_interface, DCI_SOURCE_ID_TD unique_id ) :
	m_source_id( unique_id ),
	m_dci_access( patron_interface ),
	m_dev_profile( modbus_dev_profile )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Check_Register_Range( uint16_t start_reg, uint16_t num_registers ) const
{
	return ( Interpret_DCI_Error( DCI_Check_Register_Range( start_reg, num_registers ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Modbus_DCI::DCI_Check_Register_Range( uint16_t start_reg,
												   uint16_t num_registers ) const
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_MBUS_TO_DCID_LKUP_ST_TD const* found_dcid_struct;

	found_dcid_struct = Find_MBus_DCID( start_reg );

	if ( ( Find_MBus_DCID( ( start_reg + num_registers ) - 1U ) == LOOKUP_REG_TO_DCI_NOT_FOUND ) ||
		 ( found_dcid_struct == LOOKUP_REG_TO_DCI_NOT_FOUND ) )
	{
		dci_error = DCI_ERR_INVALID_DATA_ID;
	}

	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Read_Bits( MB_BIT_GET_STRUCT* bit_get, MB_BIT_GET_RESP_STRUCT* bit_get_resp )
{
	BF_Lib::SPLIT_UINT16 temp_reg_0;
	BF_Lib::SPLIT_UINT16 temp_reg_1;
	DCI_ERROR_TD dci_error;
	uint8_t swap_byte;
	uint16_t rt_shift;
	uint16_t regs_to_get;
	uint16_t reg_get;
	uint16_t reg_counter;
	uint16_t num_bits_left;

	temp_reg_0.u16 = 0U;
	temp_reg_1.u16 = 0U;

	num_bits_left = bit_get->num_bits.u16;
	regs_to_get = ( ( bit_get->num_bits.u16 +
					  ( bit_get->start_bit.u16 & 0x0FU ) ) + 15U ) >> DIV_BY_16;
	reg_get = ( ( bit_get->start_bit.u16 + 16U ) >> DIV_BY_16 ) - 1U;
	rt_shift = bit_get->start_bit.u16 & 0x0FU;

	dci_error = DCI_Check_Register_Range( reg_get, regs_to_get );
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		reg_counter = 0U;
		while ( reg_counter < ( regs_to_get ) )
		{

			if ( 0 == reg_counter )
			{
				dci_error = Get_Reg_Data( reg_get, 1U, &temp_reg_0.u8[0], bit_get->attribute );	// Added for attribute
																								// retrieval
				swap_byte = temp_reg_0.u8[0];
				temp_reg_0.u8[0] = temp_reg_0.u8[1];
				temp_reg_0.u8[1] = swap_byte;
			}

			if ( ( DCI_ERR_NO_ERROR == dci_error ) && ( ( reg_counter < ( regs_to_get - 1 ) ) ) )
			{
				dci_error = Get_Reg_Data( ( reg_get + reg_counter + 1U ),
										  1U, &temp_reg_1.u8[0], bit_get->attribute );	// Added for attribute retrieval
				if ( ( dci_error == DCI_ERR_INVALID_DATA_ID ) &&
					 ( ( reg_counter + 1U ) == regs_to_get ) )
				{
					dci_error = DCI_ERR_NO_ERROR;
					temp_reg_1.u16 = 0U;
				}
				else
				{
					swap_byte = temp_reg_1.u8[0];
					temp_reg_1.u8[0] = temp_reg_1.u8[1];
					temp_reg_1.u8[1] = swap_byte;
				}
			}
			else
			{
				temp_reg_1.u16 = 0;
			}

			temp_reg_0.u16 = temp_reg_0.u16 >> rt_shift;
			temp_reg_0.u16 = static_cast<uint16_t>( temp_reg_1.u16 << ( 16U - rt_shift ) )
				| temp_reg_0.u16;

			if ( num_bits_left < 16U )
			{
				temp_reg_0.u16 &= ~static_cast<uint16_t>( 0xFFFFU << num_bits_left );
			}
			else if ( ( num_bits_left == bit_get->num_bits.u16 ) &&
					  ( rt_shift != 0U ) )
			{
				num_bits_left = num_bits_left - ( 16U - rt_shift );
			}
			else
			{
				num_bits_left -= 16U;
			}

			bit_get_resp->data[static_cast<uint16_t>(
								   static_cast<uint16_t>
								   ( reg_counter ) << 1U )] = temp_reg_0.u8[0];
			bit_get_resp->data[static_cast<uint16_t>( reg_counter << 1U ) + 1U] =
				temp_reg_0.u8[1];

			temp_reg_0.u16 = temp_reg_1.u16;
			reg_counter++;
		}

		uint16_t temp_byte_count = ( bit_get->num_bits.u16 + 7U ) >> DIV_BY_8;
		bit_get_resp->byte_count = static_cast<uint8_t>( temp_byte_count );
	}

	return ( Interpret_DCI_Error( dci_error ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Write_Bits( MB_BIT_SET_STRUCT* bit_set,
								MB_BIT_SET_RESP_STRUCT* bit_set_resp )
{
	BF_Lib::SPLIT_UINT16 net_data;
	BF_Lib::SPLIT_UINT16 temp_reg;
	BF_Lib::SPLIT_UINT16 reg_data;
	DCI_ERROR_TD dci_error;
	uint16_t lt_shift;
	uint16_t regs_to_set;
	uint16_t reg_set;
	uint16_t reg_counter;
	uint16_t num_bits_left;
	BF_Lib::SPLIT_UINT16 reg_mask;

	num_bits_left = bit_set->num_bits.u16;
	regs_to_set = ( ( bit_set->num_bits.u16 + ( bit_set->start_bit.u16 & 0x0FU ) ) + 15U )
		>> DIV_BY_16;
	reg_set = ( ( bit_set->start_bit.u16 + 16U ) >> DIV_BY_16 ) - 1U;
	lt_shift = bit_set->start_bit.u16 & 0x0FU;

	dci_error = DCI_Check_Register_Range( reg_set, regs_to_set );
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		reg_mask.u16 = ~static_cast<uint16_t>( 0xFFFFU << lt_shift );
		temp_reg.u16 = 0U;
		reg_counter = 0U;
		while ( ( reg_counter < regs_to_set ) && ( dci_error == DCI_ERR_NO_ERROR ) )
		{
			net_data.u8[0] =
				bit_set->data[static_cast<uint16_t>
							  ( static_cast<uint16_t>( reg_counter ) << 1U )];
			net_data.u8[1] =
				bit_set->data[static_cast<uint16_t>
							  ( static_cast<uint16_t>( reg_counter ) << 1U )
							  + 1U];
			temp_reg.u16 = static_cast<uint16_t>( net_data.u16 << lt_shift ) | temp_reg.u16;

			if ( ( num_bits_left < 16U ) && ( num_bits_left != 0U ) )
			{
				if ( reg_mask.u16 == 0U )
				{
					reg_mask.u16 = static_cast<uint16_t>( 0xFFFFU << num_bits_left );
				}
				else
				{
					reg_mask.u16 = reg_mask.u16 |
						static_cast<uint16_t>( ( 0xFFFFU << ( lt_shift + num_bits_left ) ) );
				}
			}

			if ( reg_mask.u16 == 0U )
			{
				reg_data.u8[1] = temp_reg.u8[0];
				reg_data.u8[0] = temp_reg.u8[1];
				dci_error = Set_Reg_Data( static_cast<uint16_t>( reg_set + reg_counter ), 1U,
										  &reg_data.u8[0], bit_set->attribute );		// Added for attribute retrieval
				num_bits_left -= 16U;	// This could go negative but it won't matter since we are
										// done.
			}
			else
			{
				// This could go negative but it won't matter since we are done.
				num_bits_left = num_bits_left - ( 16U - lt_shift );
				Get_Reg_Data( static_cast<uint16_t>( reg_set + reg_counter ), 1U,
							  &reg_data.u8[0] );
				temp_reg.u16 = temp_reg.u16 & static_cast<uint16_t>( ~reg_mask.u16 );
				reg_data.u8[0] &= reg_mask.u8[1];
				reg_data.u8[1] &= reg_mask.u8[0];
				reg_data.u8[0] |= temp_reg.u8[1];
				reg_data.u8[1] |= temp_reg.u8[0];
				dci_error = Set_Reg_Data( ( reg_set + reg_counter ), 1U,
										  &reg_data.u8[0], bit_set->attribute );		// Added for attribute retrieval
			}
			temp_reg.u16 = net_data.u16 >> ( 16U - lt_shift );
			reg_mask.u16 = 0U;

			reg_counter++;
		}

		bit_set_resp->start_bit.u16 = bit_set->start_bit.u16;
		bit_set_resp->num_bits.u16 = bit_set->num_bits.u16;
	}

	return ( Interpret_DCI_Error( dci_error ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Read_Registers( MB_REG_GET_STRUCT* reg_get,
									MB_REG_GET_RESP_STRUCT* reg_get_resp )
{
	DCI_ERROR_TD dci_error;

	dci_error = DCI_Check_Register_Range( reg_get->start_reg.u16, reg_get->num_regs.u16 );
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		dci_error = Get_Reg_Data( reg_get->start_reg.u16, reg_get->num_regs.u16,
								  reg_get_resp->data, reg_get->attribute );

		reg_get_resp->byte_count = static_cast<uint16_t>( reg_get->num_regs.u16 << MULT_BY_2 );
	}

	return ( Interpret_DCI_Error( dci_error ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Write_Registers( MB_REG_SET_STRUCT* reg_set,
									 MB_REG_SET_RESP_STRUCT* reg_set_resp )
{
	DCI_ERROR_TD dci_error;

	dci_error = DCI_Check_Register_Range( reg_set->start_reg.u16, reg_set->num_regs.u16 );
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		dci_error = Set_Reg_Data( reg_set->start_reg.u16, reg_set->num_regs.u16,
								  reg_set->data, reg_set->attribute );

		reg_set_resp->num_regs.u16 = reg_set->num_regs.u16;
		reg_set_resp->start_reg.u16 = reg_set->start_reg.u16;
	}

	return ( Interpret_DCI_Error( dci_error ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Modbus_DCI::Get_Reg_Data( uint16_t start_reg, uint16_t num_registers,
									   uint8_t* dest_data, uint8_t attribute )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	uint_fast8_t reg_counter = 0U;
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* modbus_struct;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_access.length = 0U;

	access_struct.source_id = m_source_id;
	switch ( attribute )
	{
		case MB_ATTRIB_LENGTH_ACCESS:
			access_struct.command = DCI_ACCESS_GET_LENGTH_CMD;
			break;

		case MB_ATTRIB_INIT_ACCESS:
			access_struct.command = DCI_ACCESS_GET_INIT_CMD;
			break;

		case MB_ATTRIB_DEFAULT_ACCESS:
			access_struct.command = DCI_ACCESS_GET_DEFAULT_CMD;
			break;

		case MB_ATTRIB_RANGE_MIN_ACCESS:
			access_struct.command = DCI_ACCESS_GET_MIN_CMD;
			break;

		case MB_ATTRIB_RANGE_MAX_ACCESS:
			access_struct.command = DCI_ACCESS_GET_MAX_CMD;
			break;

		case MB_ATTRIB_NORMAL_ACCESS:
		case MB_ATTRIB_RAM_ACCESS:
		default:
			access_struct.command = DCI_ACCESS_GET_RAM_CMD;
			break;
	}
	access_struct.data_access.data = dest_data;

	while ( ( reg_counter < static_cast<uint_fast8_t>( num_registers ) ) &&
			( dci_error == DCI_ERR_NO_ERROR ) )
	{
		modbus_struct = Find_MBus_DCID( static_cast<MODBUS_REG_TD>( start_reg + reg_counter ) );

		if ( modbus_struct != LOOKUP_REG_TO_DCI_NOT_FOUND )
		{
			if ( access_struct.command == DCI_ACCESS_GET_LENGTH_CMD )
			{
				access_struct.data_access.offset = modbus_struct->offset;
				access_struct.data_access.length = MODBUS_LENGTH_ATTRIB_RESP_LEN;
			}
			else
			{
				m_dci_access->Get_Length( modbus_struct->dcid,
										  &access_struct.data_access.length );
				access_struct.data_access.offset = modbus_struct->offset;
				access_struct.data_access.length -= modbus_struct->offset;
			}

			access_struct.data_id = modbus_struct->dcid;
			if ( access_struct.data_access.length >
				 ( ( num_registers - reg_counter ) << MULT_BY_2 ) )
			{
				access_struct.data_access.length = ( static_cast<DCI_LENGTH_TD>( num_registers
																				 - reg_counter )
													 << MULT_BY_2 );
			}
			dci_error = m_dci_access->Data_Access( &access_struct );

			reg_counter +=
				( static_cast<uint_fast8_t>( access_struct.data_access.length ) + 1U )
				>> DIV_BY_2;

			Create_MB_Reg_Data( static_cast<uint8_t*>( access_struct.data_access.data ),
								access_struct.data_access.length );
			access_struct.data_access.data = &dest_data[reg_counter << MULT_BY_2];
		}
		else
		{
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
DCI_ERROR_TD Modbus_DCI::Set_Reg_Data( uint16_t start_reg, uint16_t num_registers,
									   uint8_t* src_data, uint8_t attribute )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	uint_fast8_t reg_counter = 0U;
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* modbus_struct;
	DCI_ACCESS_ST_TD access_struct;
	DCI_ACCESS_CMD_TD command = DCI_ACCESS_SET_RAM_CMD;

	access_struct.source_id = m_source_id;
	access_struct.data_access.data = src_data;
	access_struct.data_access.length = 0U;

	switch ( attribute )
	{
		case MB_ATTRIB_INIT_ACCESS:
			command = DCI_ACCESS_SET_INIT_CMD;
			break;

		case MB_ATTRIB_LENGTH_ACCESS:
		case MB_ATTRIB_DEFAULT_ACCESS:
		case MB_ATTRIB_RANGE_MIN_ACCESS:
		case MB_ATTRIB_RANGE_MAX_ACCESS:
			dci_error = DCI_ERR_INVALID_DATA_ID;
			break;

		case MB_ATTRIB_NORMAL_ACCESS:
		case MB_ATTRIB_RAM_ACCESS:
		default:
			command = DCI_ACCESS_SET_RAM_CMD;
			break;
	}

	while ( ( reg_counter < num_registers ) && ( dci_error == DCI_ERR_NO_ERROR ) )
	{
		modbus_struct = Find_MBus_DCID( static_cast<MODBUS_REG_TD>( start_reg + reg_counter ) );

		if ( modbus_struct != LOOKUP_REG_TO_DCI_NOT_FOUND )
		{
			m_dci_access->Get_Length( modbus_struct->dcid, &access_struct.data_access.length );

			access_struct.command = command;	// DCI_ACCESS_SET_RAM_CMD;
			access_struct.data_id = modbus_struct->dcid;
			access_struct.data_access.offset = modbus_struct->offset;
			access_struct.data_access.length -= modbus_struct->offset;
			if ( access_struct.data_access.length >
				 ( ( num_registers - reg_counter ) << MULT_BY_2 ) )
			{
				access_struct.data_access.length = ( static_cast<DCI_LENGTH_TD>( num_registers
																				 - reg_counter )
													 << MULT_BY_2 );
			}
			Extract_MB_Reg_Data( static_cast<uint8_t*>( access_struct.data_access.data ),
								 access_struct.data_access.length );

			dci_error = m_dci_access->Data_Access( &access_struct );
			if ( ( dci_error == DCI_ERR_NO_ERROR ) &&
				 ( attribute == static_cast<uint8_t>( MB_ATTRIB_NORMAL_ACCESS ) ) )
			{
				access_struct.command = DCI_ACCESS_SET_INIT_CMD;
				dci_error = m_dci_access->Data_Access( &access_struct );

				if ( ( dci_error == DCI_ERR_NV_NOT_AVAILABLE ) ||
					 ( dci_error == DCI_ERR_NV_READ_ONLY ) )
				{
					dci_error = DCI_ERR_NO_ERROR;
				}
			}

			reg_counter +=
				( static_cast<uint_fast8_t>( access_struct.data_access.length ) + 1U )
				>> DIV_BY_2;
			access_struct.data_access.data = &src_data[reg_counter << MULT_BY_2];
		}
		else
		{
			dci_error = DCI_ERR_INVALID_DATA_ID;
		}
	}

	return ( dci_error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Get_Device_Identity( MB_DEV_ID_REQ_STRUCT* dev_id_req,
										 MB_DEV_ID_RESP_STRUCT* dev_id_resp )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t obj_counter;
	uint8_t end_object = 0U;
	uint8_t* dest_ptr;
	DCI_LENGTH_TD obj_length;
	DCI_LENGTH_TD obj_buff_space_left;

	if ( dev_id_req->mei_req == MB_GET_DEVICE_IDENTITY_MEI_CODE )
	{
		if ( dev_id_req->obj_id < m_dev_profile->total_mbus_objs )
		{
			obj_counter = dev_id_req->obj_id;
			switch ( dev_id_req->read_dev_id_code )
			{
				case MB_DEV_ID_READ_BASIC_DEV_ID_CODE:
					end_object = MB_DEV_ID_VENDOR_URL_OBJ_NUM;
					break;

				case MB_DEV_ID_READ_REGULAR_DEV_ID_CODE:
					end_object = m_dev_profile->total_mbus_objs;
					break;

				case MB_DEV_ID_READ_SINGLE_DEV_ID_CODE:
					end_object = obj_counter + 1U;
					break;

				case MB_DEV_ID_READ_EXTENDED_DEV_ID_CODE:
				default:
					return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
					end_object = m_dev_profile->total_mbus_objs;
					break;
			}
			if ( obj_counter >= end_object )
			{
				return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
			}

			if ( return_status == MB_NO_ERROR_CODE )
			{
				dest_ptr = dev_id_resp->obj_list_dest;
				obj_buff_space_left =
					static_cast<DCI_LENGTH_TD>( dev_id_req->max_obj_data_length ) -
					MB_DEV_ID_OBJ_DATA_INDEX;
				dev_id_resp->conformity_level = MB_DEV_ID_CONFORMITY_LEVEL_REGULAR_SINGLE;
				dev_id_resp->more_follows = 0x00U;
				dev_id_resp->next_obj_id = 0x00U;

				while ( ( obj_counter < end_object ) &&
						( return_status == MB_NO_ERROR_CODE ) )
				{
					return_status = Get_DCI_Data( m_dev_profile->mbus_obj_list[obj_counter],
												  &obj_length,
												  &dest_ptr[MB_DEV_ID_OBJ_DATA_INDEX],
												  obj_buff_space_left );
					if ( return_status == MB_NO_ERROR_CODE )
					{
						dest_ptr[MB_DEV_ID_OBJ_ID_INDEX] = obj_counter;
						dest_ptr[MB_DEV_ID_OBJ_LENGTH_INDEX] =
							static_cast<uint8_t>( obj_length );
						dest_ptr += obj_length + MB_DEV_ID_OBJ_DATA_INDEX;
						obj_buff_space_left = obj_buff_space_left
							- ( obj_length + MB_DEV_ID_OBJ_DATA_INDEX );
						/* TODO: Put Code to handle objects that span more than the available buffer
						   space.
						 * If get obj returns an error we need to decide whether we need to ask for
						 * a continuation.
						 */
					}
					obj_counter++;
				}
				dev_id_resp->num_of_objects = obj_counter - dev_id_req->obj_id;
				dev_id_resp->obj_list_len = dest_ptr - dev_id_resp->obj_list_dest;
			}
		}
		else
		{
			return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
		}
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Get_DCI_Data( DCI_ID_TD dcid, DCI_LENGTH_TD* length, uint8_t* dest_data,
								  DCI_LENGTH_TD max_buffer_length )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.data = dest_data;
	access_struct.data_access.length = 0U;
	m_dci_access->Get_Length( dcid, &access_struct.data_access.length );
	access_struct.data_access.offset = 0U;

	if ( max_buffer_length >= access_struct.data_access.length )
	{
		dci_error = m_dci_access->Data_Access( &access_struct );
		*length = access_struct.data_access.length;
	}
	else
	{
		dci_error = DCI_ERR_INVALID_DATA_LENGTH;
		*length = 0U;
	}

	return ( Interpret_DCI_Error( dci_error ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_LENGTH_TD Modbus_DCI::Get_Length( DCI_ID_TD dcid ) const
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
const DCI_MBUS_TO_DCID_LKUP_ST_TD* Modbus_DCI::Find_MBus_DCID( MODBUS_REG_TD modbus_reg ) const
{
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* search_struct;
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* return_value = LOOKUP_REG_TO_DCI_NOT_FOUND;
	uint_fast16_t begin = 0U;
	uint_fast16_t end;
	uint_fast16_t middle;

	search_struct = m_dev_profile->mbus_to_dcid;
	end = static_cast<uint_fast16_t>( m_dev_profile->total_registers ) - 1U;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1U;
		if ( search_struct[middle].modbus_reg < modbus_reg )
		{
			begin = middle + 1U;
		}
		else if ( search_struct[middle].modbus_reg > modbus_reg )
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
			return_value = ( &search_struct[middle] );
			begin = end + 1U;
		}
	}

	// only one return value as part of MISRA code changes
	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************

   const DCI_MBUS_FROM_DCID_LKUP_ST_TD* Modbus_DCI::Find_MBus_Reg( DCI_ID_TD dcid ) const
   {
   const DCI_MBUS_FROM_DCID_LKUP_ST_TD* search_struct;
   const DCI_MBUS_FROM_DCID_LKUP_ST_TD* return_value = LOOKUP_DCI_TO_REG_NOT_FOUND;
   uint_fast16_t begin = 0U;
   uint_fast16_t end;
   uint_fast16_t middle;

   search_struct = m_dev_profile->dcid_to_mbus;
   end = static_cast< uint_fast16_t >( m_dev_profile->total_dcids ) - 1U;
   while ( begin <= end )
   {
   middle = ( begin + end ) >> 1U;
   if ( search_struct[middle].dcid < dcid )
   {
   begin = middle + 1U;
   }
   else if ( search_struct[middle].dcid > dcid )
   {
   if ( middle > 0U )
   {
   end = middle - 1U;
   }
   else
   {
   //We need to exit out here.  Set Begin to end + 1 to exit.
   //We did not find what we were looking for.
   begin = end + 1;
   }
   }
   else
   {
   return_value = ( &search_struct[middle] );
   //We need to exit out here.  Set Begin to end + 1 to exit.
   //We did not find what we were looking for.
   begin = end + 1;
   }
   }

   //only one return value as part of MISRA code changes
   return ( return_value );
   }
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_DCI::Interpret_DCI_Error( DCI_ERROR_TD dci_error ) const
{
	uint8_t return_status;

	switch ( dci_error )
	{
		case DCI_ERR_NO_ERROR:
			return_status = MB_NO_ERROR_CODE;
			break;

		case DCI_ERR_INVALID_COMMAND:
			return_status = MB_ILLEGAL_FUNCTION_ERROR_CODE;
			break;

		case DCI_ERR_INVALID_DATA_ID:
			return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
			break;

		case DCI_ERR_INVALID_DATA_LENGTH:
		case DCI_ERR_EXCEEDS_RANGE:
		case DCI_ERR_DATA_INCORRECT:
			return_status = MB_ILLEGAL_DATA_VALUE_ERROR_CODE;
			break;

		/* Following 'case' labels are added to get rid of PC-lint
		   issue 'Info 788' while MISRAfication */
		case DCI_ERR_RAM_READ_ONLY:
		case DCI_ERR_NV_READ_ONLY:
		case DCI_ERR_NV_ACCESS_FAILURE:
		case DCI_ERR_ACCESS_VIOLATION:
		case DCI_ERR_NV_NOT_AVAILABLE:
		case DCI_ERR_DEFAULT_NOT_AVAILABLE:
		case DCI_ERR_DEFAULT_NOT_WRITABLE:
		case DCI_ERR_RANGE_NOT_AVAILABLE:
		case DCI_ERR_RANGE_NOT_WRITABLE:
		case DCI_ERR_ENUM_NOT_AVAILABLE:
		case DCI_ERR_ENUM_NOT_WRITABLE:
		case DCI_ERR_DATA_LENGTH_NOT_WRITABLE:
		case DCI_ERR_VALUE_LOCKED:
		default:
			return_status = MB_SLAVE_DEVICE_FAILURE_ERROR_CODE;
			break;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_DCI::Create_MB_Reg_Data( uint8_t* data, uint_fast16_t len ) const
{
	uint8_t temp;

	while ( len != 0U )
	{
		if ( len == 1U )
		{
			data[1] = data[0];
			data[0] = 0U;
			len--;
		}
		else
		{
			temp = data[1];
			data[1] = data[0];
			data[0] = temp;
			len -= 2U;
		}
		data += 2U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Modbus_DCI::Extract_MB_Reg_Data( uint8_t* data, uint_fast16_t len ) const
{
	uint8_t temp;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;

	while ( len != 0U )
	{
		temp = data[0];
		data[0] = data[1];
		if ( len > 1U )
		{
			data[1] = temp;
			len -= 2U;
		}
		else
		{
			len--;
			// The following checks to see if a value in the register is non-zero for an 8bit value.
			if ( temp != 0U )
			{
				dci_error = DCI_ERR_INVALID_DATA_LENGTH;
			}
		}
		data += 2U;
	}

	return ( dci_error );
}
