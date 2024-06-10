/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "EtherNetIP_DCI.h"
#include "EtherNetIP_Defines.h"
#include "eipinc.h"
#include "StdLib_MV.h"
#include "Bit.hpp"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define OFFSET_BYTE_COMP_MASK       0x7U

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
EtherNetIP_DCI::EtherNetIP_DCI( const EIP_TARGET_INFO_ST_TD* eip_dev_profile,
								DCI_SOURCE_ID_TD unique_id ) :
	m_source_id( static_cast<DCI_SOURCE_ID_TD>( 0U ) ),
	m_dci_access( reinterpret_cast<DCI_Patron*>( nullptr ) ),
	m_dev_profile( reinterpret_cast<EIP_TARGET_INFO_ST_TD const*>( nullptr ) )
{
	m_source_id = unique_id;
	m_dci_access = new DCI_Patron( false );

	m_dev_profile = eip_dev_profile;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t EtherNetIP_DCI::Get( CIP_GET_STRUCT* req_struct )
{
	uint8_t return_status;

	if ( req_struct->cia.cip_instance == static_cast<CIP_INSTANCE_TD>( 0U ) )
	{
		return_status = Get_Inst0( req_struct );
	}
	else
	{
		return_status = Get_DCI_Data( req_struct );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t EtherNetIP_DCI::Get_Inst0( CIP_GET_STRUCT* req_struct ) const
{
	CIP_STRUCT_PACK temp_cip;
	const CIP_CLASS_ST_TD* class_struct;
	const uint8_t* src_to_copy;
	uint8_t return_status = static_cast<uint8_t>( ROUTER_ERROR_SUCCESS );

	if ( Find_Class( &temp_cip, req_struct->cia.cip_class ) == TRUE )			// Was it found?
	{
		class_struct = temp_cip.class_struct;
		switch ( req_struct->cia.cip_attrib )
		{
			case CIP_INST0_REV:
				src_to_copy = reinterpret_cast<const uint8_t*>( &class_struct->revision );
				req_struct->length = static_cast<uint16_t>( sizeof( class_struct->revision ) );
				break;

			case CIP_INST0_MAX_INST:
			case CIP_INST0_NUM_INST:
				src_to_copy = reinterpret_cast<const uint8_t*>( &class_struct->num_instances );			// Max
																										// instances
																										// in
																										// our
																										// case
																										// is
																										// the
																										// same
																										// as
																										// the
																										// number
																										// of
																										// instances.
				req_struct->length = static_cast<uint16_t>
					( sizeof( class_struct->num_instances ) );
				break;

			default:
				// The attribute was not found.
				return_status = static_cast<uint8_t>( ROUTER_ERROR_ATTR_NOT_SUPPORTED );
				break;
		}

		if ( return_status == static_cast<uint8_t>( ROUTER_ERROR_SUCCESS ) )
		{
			BF_Lib::Copy_Const_String( req_struct->dest_data, src_to_copy, req_struct->length );
		}
	}
	else
	{
		// The class was not found.
		return_status = static_cast<uint8_t>( ROUTER_ERROR_INVALID_DESTINATION );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t EtherNetIP_DCI::Get_DCI_Data( CIP_GET_STRUCT* req_struct )
{
	CIP_STRUCT_PACK cip_struct;
	const CIP_ATTRIB_ST_TD* attrib_struct;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	uint8_t return_status;
	DCI_ACCESS_ST_TD access_struct;
	DCI_DATATYPE_SIZE_TD datatype_len = 0U;
	uint8_t* shifted_data;
	uint_fast8_t string_counter;

	access_struct.data_access.length = 0U;

	// cip_struct = Find_Def( cip_struct, req_struct->cia );
	if ( Find_Def( &cip_struct, &req_struct->cia ) == TRUE )
	{
		attrib_struct = cip_struct.attrib_struct;
		access_struct.data_id = attrib_struct->dcid;
		access_struct.source_id = m_source_id;
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;
		access_struct.data_access.data = req_struct->dest_data;

		if ( Test_Bit( attrib_struct->special, EIP_SPECIAL_BITWISE_TO_BOOL ) )
		{
			access_struct.data_access.offset = static_cast<DCI_LENGTH_TD>
				( attrib_struct->offset ) >> static_cast<DCI_LENGTH_TD>( DIV_BY_8 );
			access_struct.data_access.length = 1U;

			dci_error = m_dci_access->Data_Access( &access_struct );
			if ( DCI_ERR_NO_ERROR == dci_error )
			{
				shifted_data = static_cast<uint8_t*>( access_struct.data_access.data );
				*shifted_data = ( *shifted_data ) >>
					( attrib_struct->offset & OFFSET_BYTE_COMP_MASK );
				*shifted_data &= 0x01U;
				req_struct->length = 1U;
			}
		}
		else if ( Test_Bit( attrib_struct->special, EIP_SPECIAL_SHORT_STRING_DTYPE ) )
		{
			access_struct.data_access.offset = 0U;
			access_struct.data_access.data = &req_struct->dest_data[1U];
			if ( attrib_struct->alt_length != 0U )
			{
				access_struct.data_access.length = attrib_struct->alt_length;
			}
			else
			{
				m_dci_access->Get_Length( attrib_struct->dcid, &access_struct.data_access.length );
				if ( access_struct.data_access.length > req_struct->length )
				{
					access_struct.data_access.length = req_struct->length;
				}
			}

			dci_error = m_dci_access->Data_Access( &access_struct );
			string_counter = 1U;
			while ( ( string_counter < access_struct.data_access.length ) &&
					( req_struct->dest_data[string_counter] != 0U ) )
			{
				string_counter++;
			}
			// Making sure to wipe out any garbage that may remain in the string after a set.
			for (; string_counter < access_struct.data_access.length; string_counter++ )
			{
				req_struct->dest_data[string_counter] = 0U;
			}
			req_struct->dest_data[access_struct.data_access.length - 1U] = 0U;	// Just to guarantee
																				// that we have a
																				// NULL termination.
			*req_struct->dest_data = static_cast<uint8_t>( access_struct.data_access.length );			// string_counter;		//Gotta
																										// load
																										// the
																										// byte
																										// length
																										// in
																										// the
																										// first
																										// byte
																										// of
																										// the
																										// short
																										// string.
			req_struct->length = access_struct.data_access.length + 1U;	// string_counter + 1;
		}
		else
		{
			m_dci_access->Get_Datatype_Length( attrib_struct->dcid, &datatype_len );
			access_struct.data_access.offset = static_cast<DCI_LENGTH_TD>
				( attrib_struct->offset ) * static_cast<DCI_LENGTH_TD>( datatype_len );
			if ( attrib_struct->alt_length != 0U )
			{
				access_struct.data_access.length = attrib_struct->alt_length;
			}
			else if ( Test_Bit( attrib_struct->special, EIP_SPECIAL_USE_DATATYPE_LEN ) )
			{
				access_struct.data_access.length = datatype_len;
			}
			else
			{
				m_dci_access->Get_Length( attrib_struct->dcid, &access_struct.data_access.length );
				if ( access_struct.data_access.length > req_struct->length )
				{
					access_struct.data_access.length = req_struct->length;
				}
			}

			dci_error = m_dci_access->Data_Access( &access_struct );
			req_struct->length = access_struct.data_access.length;
		}
		return_status = Interpret_DCI_Error( dci_error );
	}
	else
	{
		return_status = Interpret_Missing_CIA( &cip_struct );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t EtherNetIP_DCI::Set( CIP_SET_STRUCT* req_struct )
{
	CIP_STRUCT_PACK cip_struct;
	const CIP_ATTRIB_ST_TD* attrib_struct;
	DCI_ERROR_TD dci_error;
	uint8_t return_status = static_cast<uint8_t>( ROUTER_ERROR_SUCCESS );
	DCI_ACCESS_ST_TD access_struct;
	DCI_DATATYPE_SIZE_TD datatype_len = 0U;
	DCI_DATATYPE_TD datatype = DCI_DTYPE_BOOL;
	uint8_t temp_data;

	if ( req_struct->cia.cip_instance == 0U )
	{
		return_status = static_cast<uint8_t>( ROUTER_ERROR_BAD_SERVICE );
	}
	else if ( Find_Def( &cip_struct, &req_struct->cia ) == TRUE )
	{
		attrib_struct = cip_struct.attrib_struct;
		access_struct.data_id = attrib_struct->dcid;
		access_struct.source_id = m_source_id;

		dci_error = m_dci_access->Get_Datatype( attrib_struct->dcid, &datatype );
		if ( dci_error == static_cast<uint32_t>( DCI_ERR_INVALID_DATA_ID ) )
		{
			return_status = Interpret_DCI_Error( dci_error );
		}
		else if ( Test_Bit( attrib_struct->special, EIP_SPECIAL_BITWISE_TO_BOOL ) ||
				  ( datatype == DCI_DTYPE_BOOL ) )
		{
			if ( req_struct->length == 0U )
			{
				return_status = static_cast<uint8_t>( ROUTER_ERROR_NOT_ENOUGH_DATA );
			}
			else if ( req_struct->length > 1U )
			{
				return_status = static_cast<uint8_t>( ROUTER_ERROR_TOO_MUCH_DATA );
			}
			else
			{
				access_struct.data_access.offset = static_cast<DCI_LENGTH_TD>
					( attrib_struct->offset ) >> static_cast<DCI_LENGTH_TD>( DIV_BY_8 );
				access_struct.data_access.length = 1U;

				access_struct.command = DCI_ACCESS_GET_RAM_CMD;
				access_struct.data_access.data = &temp_data;
				dci_error = m_dci_access->Data_Access( &access_struct );
				if ( DCI_ERR_NO_ERROR == dci_error )
				{
					if ( *req_struct->src_data == 1U )
					{
						BF_Lib::Bit::Set( temp_data,
										  ( static_cast<uint_fast8_t>
											( attrib_struct->offset ) & static_cast<
												uint_fast8_t>( OFFSET_BYTE_COMP_MASK ) ) );
					}
					else if ( *req_struct->src_data == 0U )
					{
						BF_Lib::Bit::Clr( temp_data,
										  ( static_cast<uint_fast8_t>
											( attrib_struct->offset ) & static_cast<
												uint_fast8_t>( OFFSET_BYTE_COMP_MASK ) ) );
					}
					else
					{
						return_status = Interpret_DCI_Error( DCI_ERR_EXCEEDS_RANGE );
					}
				}
				else
				{
					return_status = Interpret_DCI_Error( DCI_ERR_EXCEEDS_RANGE );
				}
			}
		}
		else if ( Test_Bit( attrib_struct->special, EIP_SPECIAL_SHORT_STRING_DTYPE ) )
		{
			access_struct.data_access.data = &req_struct->src_data[1U];	// because the first byte
																		// contains the string
																		// length.
			access_struct.data_access.offset = 0U;
			if ( attrib_struct->alt_length != 0U )
			{
				access_struct.data_access.length = attrib_struct->alt_length;
			}
			else
			{
				m_dci_access->Get_Length( attrib_struct->dcid, &access_struct.data_access.length );
			}

			// If the req size is greater than the length then we return an error OR if the
			// set string length does not match the string data length being passed in.
			// If the actual size is greater than the req size then we force it to be only the length
			// of the actual sent data.
			if ( req_struct->src_data[0U] != ( req_struct->length - 1U ) )
			{
				return_status = static_cast<uint8_t>( ROUTER_ERROR_BAD_ATTR_DATA );
			}
			else if ( req_struct->length == 0U )
			{
				return_status = static_cast<uint8_t>( ROUTER_ERROR_NOT_ENOUGH_DATA );
			}
			else if ( access_struct.data_access.length < ( req_struct->length - 1U ) )
			{
				return_status = static_cast<uint8_t>( ROUTER_ERROR_TOO_MUCH_DATA );
			}
			else if ( req_struct->length == 1U )		// If we have a zero length short string we
														// should write a null termination to the
														// string.
			{
				access_struct.data_access.length = 1U;
				access_struct.data_access.data = req_struct->src_data;
			}
			else if ( access_struct.data_access.length > ( req_struct->length - 1U ) )
			{
				access_struct.data_access.length = ( req_struct->length - 1U );
			}
			else
			{
				// do nothing
			}
		}
		else
		{
			access_struct.data_access.data = req_struct->src_data;
			m_dci_access->Get_Datatype_Length( attrib_struct->dcid, &datatype_len );
			access_struct.data_access.offset = static_cast<DCI_LENGTH_TD>
				( attrib_struct->offset ) * static_cast<DCI_LENGTH_TD>( datatype_len );
			if ( attrib_struct->alt_length != 0U )
			{
				access_struct.data_access.length = attrib_struct->alt_length;
			}
			else if ( Test_Bit( attrib_struct->special, EIP_SPECIAL_USE_DATATYPE_LEN ) )
			{
				access_struct.data_access.length = datatype_len;
			}
			else
			{
				m_dci_access->Get_Length( attrib_struct->dcid, &access_struct.data_access.length );
			}

			// If the actual size is greater than the req size then we force it to be only the length
			// of the actual sent data.  If the req size is greater than the length then we return an
			// error.
			if ( access_struct.data_access.length > req_struct->length )
			{
				return_status = static_cast<uint8_t>( ROUTER_ERROR_NOT_ENOUGH_DATA );
			}
			else if ( access_struct.data_access.length < req_struct->length )
			{
				return_status = static_cast<uint8_t>( ROUTER_ERROR_TOO_MUCH_DATA );
			}
			else
			{
				// do nothing
			}
		}

		if ( return_status == static_cast<uint8_t>( ROUTER_ERROR_SUCCESS ) )
		{
			access_struct.command = DCI_ACCESS_SET_RAM_CMD;
			dci_error = m_dci_access->Data_Access( &access_struct );
			if ( DCI_ERR_NO_ERROR == dci_error )
			{
				access_struct.command = DCI_ACCESS_SET_INIT_CMD;
				dci_error = m_dci_access->Data_Access( &access_struct );

				if ( ( dci_error == DCI_ERR_NV_NOT_AVAILABLE ) ||
					 ( dci_error == DCI_ERR_NV_READ_ONLY ) )
				{
					dci_error = DCI_ERR_NO_ERROR;
				}
			}
			return_status = Interpret_DCI_Error( dci_error );
		}
	}
	else
	{
		return_status = Interpret_Missing_CIA( &cip_struct );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t EtherNetIP_DCI::Get_Length( CIA_ID_TD* cia, uint16_t* length ) const
{
	CIP_STRUCT_PACK cip_struct;
	const CIP_ATTRIB_ST_TD* attrib_struct;
	uint8_t return_error = static_cast<uint8_t>( ROUTER_ERROR_SUCCESS );
	DCI_LENGTH_TD temp_length = 0U;

	if ( cia->cip_instance == 0U )
	{
		if ( Find_Class( &cip_struct, cia->cip_class ) == TRUE )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
			 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting
			 * pointers, not enums.
			 */
			/*lint -e{930} # MISRA Deviation */
			if ( cia->cip_attrib < static_cast<CIP_ATTRIB_TD>( CIP_INST0_ENUM_MAX ) )
			{
				*length = 0U;
				return_error = static_cast<uint8_t>( ROUTER_ERROR_ATTR_NOT_SUPPORTED );
			}
			else
			{
				*length = static_cast<DCI_LENGTH_TD>( sizeof( uint16_t ) );
			}
		}
		else
		{
			*length = 0U;
			return_error = static_cast<uint8_t>( ROUTER_ERROR_INVALID_DESTINATION );
		}
	}
	else
	{
		if ( Find_Def( &cip_struct, cia ) == TRUE )
		{
			attrib_struct = cip_struct.attrib_struct;
			if ( ( attrib_struct->special & static_cast<uint8_t>
				   ( 1U << EIP_SPECIAL_BITWISE_TO_BOOL ) ) != 0U )
			{
				*length = 1U;
			}
			else if ( attrib_struct->alt_length != 0U )
			{
				*length = attrib_struct->alt_length;
			}
			else
			{
				m_dci_access->Get_Length( attrib_struct->dcid, &temp_length );
				*length = temp_length;
				if ( Test_Bit( attrib_struct->special, EIP_SPECIAL_SHORT_STRING_DTYPE ) )
				{
					*length = *length + 1U;	// To make room for the byte length before the string.
				}
			}
		}
		else
		{
			*length = 0U;
			return_error = Interpret_Missing_CIA( &cip_struct );
		}
	}

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool EtherNetIP_DCI::Find_Class( CIP_STRUCT_PACK* struct_pack, CIP_CLASS_TD cip_class ) const
{
	const CIP_CLASS_ST_TD* search_struct;
	CIP_CLASS_TD begin = 0U;
	CIP_CLASS_TD end;
	CIP_CLASS_TD middle;
	bool result = FALSE;

	search_struct = m_dev_profile->class_list;
	end = m_dev_profile->total_classes - 1U;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1U;
		if ( search_struct[middle].cip_class < cip_class )
		{
			begin = middle + 1U;
		}
		else if ( search_struct[middle].cip_class > cip_class )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				struct_pack->class_struct = reinterpret_cast<CIP_CLASS_ST_TD const*>( nullptr );
				result = FALSE;
				begin = end + 1U;
			}
		}
		else
		{
			struct_pack->class_struct = &search_struct[middle];
			result = TRUE;
			begin = end + 1U;
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool EtherNetIP_DCI::Find_Inst( CIP_STRUCT_PACK* struct_pack, CIP_INSTANCE_TD cip_instance ) const
{
	bool found = FALSE;
	uint_fast16_t instance_counter;

	if ( ( cip_instance > 0U ) && ( struct_pack->class_struct != NULL ) )
	{
		instance_counter = 0U;
		while ( ( instance_counter < struct_pack->class_struct->num_instances ) &&
				( cip_instance !=
				  struct_pack->class_struct->instances_list[instance_counter].instance ) )
		{
			instance_counter++;
		}
		if ( instance_counter < struct_pack->class_struct->num_instances )
		{
			struct_pack->instance_struct =
				&struct_pack->class_struct->instances_list[instance_counter];
			found = TRUE;
		}
	}

	if ( found == FALSE )
	{
		struct_pack->instance_struct = reinterpret_cast<CIP_INSTANCE_ST_TD const*>( nullptr );
	}

	return ( found );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool EtherNetIP_DCI::Find_Attrib( CIP_STRUCT_PACK* struct_pack, CIP_ATTRIB_TD cip_attrib ) const
{
	const CIP_ATTRIB_ST_TD* search_struct;
	CIP_ATTRIB_TD begin = 0U;
	CIP_ATTRIB_TD end;
	CIP_ATTRIB_TD middle;
	bool result = FALSE;

	if ( struct_pack->instance_struct != reinterpret_cast<CIP_INSTANCE_ST_TD const*>( nullptr ) )
	{
		search_struct = struct_pack->instance_struct->attrib_list;
		end = struct_pack->class_struct->num_attributes - 1U;
		while ( begin <= end )
		{
			middle = ( begin + end ) >> 1U;
			if ( search_struct[middle].cip_attrib < cip_attrib )
			{
				begin = middle + 1U;
			}
			else if ( search_struct[middle].cip_attrib > cip_attrib )
			{
				if ( middle > 0U )
				{
					end = middle - 1U;
				}
				else
				{
					struct_pack->attrib_struct =
						reinterpret_cast<CIP_ATTRIB_ST_TD const*>( nullptr );
					result = FALSE;
					begin = end + 1U;
				}
			}
			else
			{
				struct_pack->attrib_struct = &search_struct[middle];
				result = TRUE;
				begin = end + 1U;
			}
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool EtherNetIP_DCI::Find_Def( CIP_STRUCT_PACK* struct_pack, CIA_ID_TD* cia ) const
{
	bool result = FALSE;

	if ( Find_Class( struct_pack, cia->cip_class ) == TRUE )
	{
		if ( Find_Inst( struct_pack, cia->cip_instance ) == TRUE )
		{
			if ( Find_Attrib( struct_pack, cia->cip_attrib ) == TRUE )
			{
				result = TRUE;
			}
			else
			{
				struct_pack->attrib_struct = reinterpret_cast<CIP_ATTRIB_ST_TD const*>( nullptr );
			}
		}
		else
		{
			struct_pack->instance_struct = reinterpret_cast<CIP_INSTANCE_ST_TD const*>( nullptr );
			struct_pack->attrib_struct = reinterpret_cast<CIP_ATTRIB_ST_TD const*>( nullptr );
		}
	}
	else
	{
		struct_pack->class_struct = reinterpret_cast<CIP_CLASS_ST_TD const*>( nullptr );
		struct_pack->instance_struct = reinterpret_cast<CIP_INSTANCE_ST_TD const*>( nullptr );
		struct_pack->attrib_struct = reinterpret_cast<CIP_ATTRIB_ST_TD const*>( nullptr );
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t EtherNetIP_DCI::Interpret_DCI_Error( DCI_ERROR_TD dci_error ) const
{
	uint8_t return_status;

	switch ( dci_error )
	{
		case DCI_ERR_NO_ERROR:
			return_status = static_cast<uint8_t>( ROUTER_ERROR_SUCCESS );
			break;

		case DCI_ERR_INVALID_COMMAND:
			return_status = static_cast<uint8_t>( ROUTER_ERROR_BAD_SERVICE );
			break;

		case DCI_ERR_INVALID_DATA_ID:
			return_status = static_cast<uint8_t>( ROUTER_ERROR_ATTR_NOT_SUPPORTED );
			break;

		case DCI_ERR_INVALID_DATA_LENGTH:
			return_status = static_cast<uint8_t>( ROUTER_ERROR_NOT_ENOUGH_DATA );
			break;

		case DCI_ERR_RAM_READ_ONLY:
		case DCI_ERR_NV_READ_ONLY:
			return_status = static_cast<uint8_t>( ROUTER_ERROR_ATTR_NOT_SETTABLE );
			break;

		case DCI_ERR_EXCEEDS_RANGE:
			// return_status = ROUTER_ERROR_INVALID_PARAMETER;
			return_status = static_cast<uint8_t>( ROUTER_ERROR_BAD_ATTR_DATA );
			break;

		case DCI_ERR_NV_ACCESS_FAILURE:
		case DCI_ERR_ACCESS_VIOLATION:
		case DCI_ERR_NV_NOT_AVAILABLE:
		case DCI_ERR_DEFAULT_NOT_AVAILABLE:
		case DCI_ERR_RANGE_NOT_AVAILABLE:
		case DCI_ERR_ENUM_NOT_AVAILABLE:
		case DCI_ERR_VALUE_LOCKED:
		case DCI_ERR_TOTAL_ERRORS:
		default:
			return_status = static_cast<uint8_t>( ROUTER_ERROR_SERVICE_ERROR );
			break;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t EtherNetIP_DCI::Interpret_Missing_CIA( CIP_STRUCT_PACK* cip_struct ) const
{
	uint8_t return_status;

	if ( cip_struct->class_struct == NULL )
	{
		return_status = static_cast<uint8_t>( ROUTER_ERROR_INVALID_DESTINATION );
	}
	else if ( cip_struct->instance_struct == NULL )
	{
		return_status = static_cast<uint8_t>( ROUTER_ERROR_INVALID_DESTINATION );
	}
	else if ( cip_struct->attrib_struct == NULL )
	{
		return_status = static_cast<uint8_t>( ROUTER_ERROR_ATTR_NOT_SUPPORTED );
	}
	else
	{
		return_status = static_cast<uint8_t>( ROUTER_ERROR_SERVICE_ERROR );
	}

	return ( return_status );
}
