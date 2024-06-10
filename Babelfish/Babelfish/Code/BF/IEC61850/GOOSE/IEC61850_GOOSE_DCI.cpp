/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "IEC61850_GOOSE_DCI.h"
#include "StdLib_MV.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IEC61850_GOOSE_DCI::IEC61850_GOOSE_DCI( const goose_message_info_t* goose_message_info,
										DCI_SOURCE_ID_TD unique_id ) :
	m_source_id( unique_id ),
	m_dci_access( new DCI_Patron( false ) ),
	m_goose_message_info( goose_message_info )
{
	m_individual_readvalue =
		reinterpret_cast<uint8_t*>( Ram::Allocate( static_cast<size_t>( IEC61850_PACKET_PAYLOAD ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
IEC61850_GOOSE_DCI::~IEC61850_GOOSE_DCI( void )
{
	delete m_dci_access;
	Ram::De_Allocate( m_individual_readvalue );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD IEC61850_GOOSE_DCI::Get_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD* length_ptr,
												uint8_t* dest_data_ptr )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.data = dest_data_ptr;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	dci_error = m_dci_access->Data_Access( &access_struct );
	*length_ptr = access_struct.data_access.length;

	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD IEC61850_GOOSE_DCI::Set_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD length, uint8_t* src_data_ptr )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	DCI_PATRON_ATTRIB_SUPPORT_TD attrib_support;
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
		if ( ( attrib_error == DCI_ERR_NO_ERROR ) &&
			 Test_Bit( attrib_support, DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
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
iec61850_error_t IEC61850_GOOSE_DCI::Get_Group_Id_Details( DCI_ID_TD dcid, group_param_t* group_id,
														   sub_group_param_t* sub_group_id,
														   uint8_t message_type )
{
	DCI_ERROR_TD dci_error = DCI_ERR_INVALID_DATA_ID;
	group_param_t group_index;
	sub_group_param_t sub_group_counter;
	group_param_t loop_index;
	const goose_group_info_t* search_struct = nullptr;
	sub_group_param_t index;
	iec61850_error_t return_type = IEC61850_ERR_NO_ERROR;

	*sub_group_id = IEC61850_GOOSE_INVALID_SUBGROUP_ID;
	if ( message_type == PUBLISH )
	{
		search_struct = m_goose_message_info->pub_group_profile;
	}
	else if ( message_type == SUBSCRIBE )
	{
		search_struct = m_goose_message_info->sub_group_profile;
	}
	else
	{
		return_type = IEC61850_ERR_MTYPE_INVALID;
	}

	if ( return_type == IEC61850_ERR_NO_ERROR )
	{
		for ( group_index = 0; group_index < search_struct->total_group; group_index++ )
		{
			for ( loop_index = 0;
				  loop_index < search_struct->dci_group_struct[group_index].maingroup_total_params;
				  loop_index++ )
			{
				if ( dcid == search_struct->dci_group_struct[group_index].maingroup_param_id[loop_index] )
				{
					*group_id = group_index;
					dci_error = DCI_ERR_NO_ERROR;
					break;
				}
			}
		}
		for ( group_index = 0; group_index < search_struct->total_group; group_index++ )
		{
			for ( sub_group_counter = 0;
				  sub_group_counter < search_struct->dci_group_struct[group_index].total_subgroups;
				  sub_group_counter++ )
			{
				for ( index = 0;
					  index <
					  search_struct->dci_group_struct[group_index].subgroup_array[sub_group_counter].
					  subgroup_total_params;
					  index++ )
				{
					if ( dcid ==
						 search_struct->dci_group_struct[group_index].subgroup_array[sub_group_counter].
						 subgroup_param_id[index] )
					{
						*group_id = group_index;
						*sub_group_id = sub_group_counter;
						dci_error = DCI_ERR_NO_ERROR;
						break;
					}
				}
			}
		}
	}

	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		return_type = IEC61850_ERR_NO_ERROR;
	}
	else
	{
		return_type = IEC61850_ERR_DCI_ERR;
	}

	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t IEC61850_GOOSE_DCI::Get_Group_Assembly_Parameters( group_param_t group_id, DCI_LENGTH_TD* length_ptr,
																	uint8_t* dest_data_ptr, uint8_t message_type )
{
	DCI_ERROR_TD dci_error = DCI_ERR_INVALID_DATA_ID;
	uint16_t length = DISABLE_VALUE;
	const goose_group_info_t* search_struct;
	group_param_t index = DISABLE_VALUE;
	group_param_t loop_index;
	DCI_ID_TD dcid;
	iec61850_error_t return_type = IEC61850_ERR_NO_ERROR;

	if ( message_type == PUBLISH )
	{
		search_struct = m_goose_message_info->pub_group_profile;
	}
	else if ( message_type == SUBSCRIBE )
	{
		search_struct = m_goose_message_info->sub_group_profile;
	}
	else
	{
		return_type = IEC61850_ERR_MTYPE_INVALID;
	}

	if ( return_type == IEC61850_ERR_NO_ERROR )
	{
		for ( loop_index = 0;
			  loop_index < search_struct->dci_group_struct[group_id].maingroup_total_params;
			  loop_index++ )
		{
			dcid = search_struct->dci_group_struct[group_id].maingroup_param_id[loop_index];
			if ( dcid != DCI_ID_UNDEFINED )
			{
				dci_error =
					Get_Dcid_Data( dcid,  reinterpret_cast<DCI_LENGTH_TD*>( &length ),
								   reinterpret_cast<uint8_t*>( m_individual_readvalue ) );
				if ( dci_error == DCI_ERR_NO_ERROR )
				{
					BF_Lib::Copy_String( dest_data_ptr + index, m_individual_readvalue, length );
					index += length;
				}
				else
				{
					break;
				}
			}
			else
			{
				dci_error = DCI_ERR_INVALID_DATA_ID;
			}
		}
	}
	*length_ptr = index;
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		return_type = IEC61850_ERR_NO_ERROR;
	}
	else
	{
		return_type = IEC61850_ERR_DCI_ERR;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t IEC61850_GOOSE_DCI::Get_Subgroup_Assembly_Parameters( group_param_t group_id,
																	   sub_group_param_t sub_group_id,
																	   DCI_LENGTH_TD* length_ptr,
																	   uint8_t* dest_data_ptr,
																	   uint8_t message_type )
{
	DCI_ERROR_TD dci_error = DCI_ERR_INVALID_DATA_ID;
	uint16_t length = DISABLE_VALUE;
	const goose_group_info_t* search_struct;
	uint16_t index;
	sub_group_param_t loop_index;
	DCI_ID_TD dcid;
	iec61850_error_t return_type = IEC61850_ERR_NO_ERROR;

	index = DISABLE_VALUE;

	if ( message_type == PUBLISH )
	{
		search_struct = m_goose_message_info->pub_group_profile;
	}
	else if ( message_type == SUBSCRIBE )
	{
		search_struct = m_goose_message_info->sub_group_profile;
	}
	else
	{
		return_type = IEC61850_ERR_MTYPE_INVALID;
	}

	if ( return_type == IEC61850_ERR_NO_ERROR )
	{
		for ( loop_index = 0;
			  loop_index <
			  search_struct->dci_group_struct[group_id].subgroup_array[sub_group_id].subgroup_total_params;
			  loop_index++ )
		{
			dcid =
				search_struct->dci_group_struct[group_id].subgroup_array[sub_group_id].subgroup_param_id[
					loop_index];
			if ( dcid != DCI_ID_UNDEFINED )
			{
				dci_error =
					Get_Dcid_Data( dcid,  reinterpret_cast<DCI_LENGTH_TD*>( &length ),
								   reinterpret_cast<uint8_t*>( m_individual_readvalue ) );
				if ( dci_error == DCI_ERR_NO_ERROR )
				{
					BF_Lib::Copy_String( dest_data_ptr + index, m_individual_readvalue, length );
					index += length;
				}
				else
				{
					break;
				}
			}
			else
			{
				dci_error = DCI_ERR_INVALID_DATA_ID;
			}
		}
	}
	*length_ptr = index;

	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		return_type = IEC61850_ERR_NO_ERROR;
	}
	else
	{
		return_type = IEC61850_ERR_DCI_ERR;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t IEC61850_GOOSE_DCI::Set_SubGroup_Assembly_Parameters( group_param_t group_id,
																	   sub_group_param_t sub_group_id,
																	   uint8_t* src_data_ptr,
																	   uint8_t message_type, DCI_LENGTH_TD* length_ptr )
{
	DCI_ERROR_TD dci_error = DCI_ERR_INVALID_DATA_ID;
	uint16_t length = DISABLE_VALUE;
	const goose_group_info_t* search_struct;
	sub_group_param_t loop_index;
	DCI_ID_TD dcid;
	DCI_LENGTH_TD index;
	iec61850_error_t return_type = IEC61850_ERR_NO_ERROR;

	index = DISABLE_VALUE;

	if ( message_type == PUBLISH )
	{
		search_struct = m_goose_message_info->pub_group_profile;
	}
	else if ( message_type == SUBSCRIBE )
	{
		search_struct = m_goose_message_info->sub_group_profile;
	}
	else
	{
		return_type = IEC61850_ERR_MTYPE_INVALID;
	}

	if ( return_type == IEC61850_ERR_NO_ERROR )
	{
		for ( loop_index = 0;
			  loop_index <
			  search_struct->dci_group_struct[group_id].subgroup_array[sub_group_id].subgroup_total_params;
			  loop_index++ )
		{
			dcid =
				search_struct->dci_group_struct[group_id].subgroup_array[sub_group_id].subgroup_param_id[
					loop_index];
			if ( dcid != DCI_ID_UNDEFINED )
			{
				m_dci_access->Get_Length( dcid, &length );
				dci_error =
					Set_Dcid_Data( dcid, length, src_data_ptr + index );
				index += length;
				if ( dci_error != DCI_ERR_NO_ERROR )
				{
					break;
				}
			}
			else
			{
				dci_error = DCI_ERR_INVALID_DATA_ID;
			}
		}
	}
	*length_ptr = index;

	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		return_type = IEC61850_ERR_NO_ERROR;
	}
	else
	{
		return_type = IEC61850_ERR_DCI_ERR;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t IEC61850_GOOSE_DCI::Get_Subgroup_Param_Length( group_param_t group_id,
																sub_group_param_t sub_group_id,
																DCI_LENGTH_TD* length_ptr,
																uint8_t message_type )
{
	DCI_ERROR_TD dci_error = DCI_ERR_INVALID_DATA_ID;
	uint16_t length = DISABLE_VALUE;
	const goose_group_info_t* search_struct;
	sub_group_param_t loop_index;
	DCI_ID_TD dcid;
	iec61850_error_t return_type = IEC61850_ERR_NO_ERROR;

	if ( message_type == PUBLISH )
	{
		search_struct = m_goose_message_info->pub_group_profile;
	}
	else if ( message_type == SUBSCRIBE )
	{
		search_struct = m_goose_message_info->sub_group_profile;
	}
	else
	{
		return_type = IEC61850_ERR_MTYPE_INVALID;
	}

	if ( return_type == IEC61850_ERR_NO_ERROR )
	{
		for ( loop_index = 0;
			  loop_index <
			  search_struct->dci_group_struct[group_id].subgroup_array[sub_group_id].subgroup_total_params;
			  loop_index++ )
		{
			dcid =
				search_struct->dci_group_struct[group_id].subgroup_array[sub_group_id].subgroup_param_id[
					loop_index];
			if ( dcid != DCI_ID_UNDEFINED )
			{
				dci_error =
					m_dci_access->Get_Length( dcid, reinterpret_cast<DCI_LENGTH_TD*>( &length ) );
				if ( dci_error == DCI_ERR_NO_ERROR )
				{
					BF_Lib::Copy_String( ( uint8_t* )length_ptr + ( loop_index * sizeof( length ) ),
										 ( uint8_t* )&length,
										 sizeof( length ) );
				}
				else
				{
					break;
				}
			}
			else
			{
				dci_error = DCI_ERR_INVALID_DATA_ID;
			}
		}
	}
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		return_type = IEC61850_ERR_NO_ERROR;
	}
	else
	{
		return_type = IEC61850_ERR_DCI_ERR;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t IEC61850_GOOSE_DCI::Get_Maingroup_Params_Length( group_param_t group_id, DCI_LENGTH_TD* length_ptr,
																  uint8_t message_type )
{
	DCI_ERROR_TD dci_error = DCI_ERR_INVALID_DATA_ID;
	uint16_t length = DISABLE_VALUE;
	const goose_group_info_t* search_struct;
	group_param_t loop_index;
	DCI_ID_TD dcid;
	iec61850_error_t return_type = IEC61850_ERR_NO_ERROR;

	if ( message_type == PUBLISH )
	{
		search_struct = m_goose_message_info->pub_group_profile;
	}
	else if ( message_type == SUBSCRIBE )
	{
		search_struct = m_goose_message_info->sub_group_profile;
	}
	else
	{
		return_type = IEC61850_ERR_MTYPE_INVALID;
	}

	if ( return_type == IEC61850_ERR_NO_ERROR )
	{
		for ( loop_index = 0;
			  loop_index < search_struct->dci_group_struct[group_id].maingroup_total_params;
			  loop_index++ )
		{
			dcid = search_struct->dci_group_struct[group_id].maingroup_param_id[loop_index];
			if ( dcid != DCI_ID_UNDEFINED )
			{
				dci_error = m_dci_access->Get_Length( dcid,  reinterpret_cast<DCI_LENGTH_TD*>( &length ) );
				if ( dci_error == DCI_ERR_NO_ERROR )
				{
					BF_Lib::Copy_String( ( uint8_t* )length_ptr + ( loop_index * sizeof( length ) ),
										 ( uint8_t* )&length,
										 sizeof( length ) );
				}
				else
				{
					break;
				}
			}
			else
			{
				dci_error = DCI_ERR_INVALID_DATA_ID;
			}
		}
	}
	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		return_type = IEC61850_ERR_NO_ERROR;
	}
	else
	{
		return_type = IEC61850_ERR_DCI_ERR;
	}
	return ( return_type );
}
