/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "CIEC61850_SAV_DCI_Manager.h"
#include "StdLib_MV.h"
#include "IEC61850_SAV_DCI_Data.h"
#include "IEC61850_Defines.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
bool CIEC61850_SAV_DCI_Manager::m_sub_ind_flag = false;
CIEC61850_SAV_DCI_Manager* CIEC61850_SAV_DCI_Manager::m_dci_manager = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_SAV_DCI_Manager* CIEC61850_SAV_DCI_Manager::Get_Instance( CIEC61850_SAV_Interface* sav_interface )
{
	if ( nullptr == m_dci_manager )
	{
		m_dci_manager = new CIEC61850_SAV_DCI_Manager( sav_interface );
	}
	return ( m_dci_manager );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_SAV_DCI_Manager::CIEC61850_SAV_DCI_Manager( CIEC61850_SAV_Interface* sav_interface ) :
	m_sav_interface( sav_interface ),
	m_pub_array_index( DISABLE_VALUE ),
	m_sub_array_index( DISABLE_VALUE ),
	m_pub_ind_flag( false )
{
	m_individual_readvalue =
		reinterpret_cast<uint8_t*>( Ram::Allocate( static_cast<size_t>( MAX_RAM_ALLOCATION ) ) );
	m_dci_access = new DCI_Patron( false );
	m_source_id = DCI_SOURCE_IDS_Get();
	m_sav_dci = new IEC61850_SAV_DCI( &iec61850_sav_msg_info, m_source_id );
	m_data_id = DCI_ID_UNDEFINED;

	memset( m_pub_config_array, DISABLE_VALUE, sizeof( m_pub_config_array ) );
	memset( m_sub_config_array, DISABLE_VALUE, sizeof( m_sub_config_array ) );

	memset( m_sub_group_index, DISABLE_VALUE, sizeof( m_sub_group_index ) );
	memset( m_pub_group_index, DISABLE_VALUE, sizeof( m_pub_group_index ) );

	Create_Pub_Config_Dci_Owners();	///< Creation of DCI owners for SAV publish config parameters
	Create_Sub_Config_Dci_Owners();	///< Creation of DCI owners for SAV Subscribe config parameters
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_SAV_DCI_Manager::~CIEC61850_SAV_DCI_Manager( void )
{
	delete m_dci_access;
	Ram::De_Allocate( m_individual_readvalue );
	delete m_sav_dci;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_SAV_DCI_Manager::DCI_Manager_Init( void )
{
	group_param_t group_index;
	user_config_t temp_user_config;
	iec61850_error_t error_type = IEC61850_ERR_NO_ERROR;

	m_pub_array_index = DISABLE_VALUE;

	m_sav_interface->Initialize();
	for ( group_index = 0; group_index < iec61850_sav_msg_info.pub_group_profile->total_group; group_index++ )
	{
		if ( error_type == IEC61850_ERR_NO_ERROR )
		{
			Get_User_Config_Pub( &temp_user_config, group_index );	///< Update all SAV publish messages config
			///< and
			///< send to sav interface
			error_type = m_sav_interface->Set_Publish_Config( &temp_user_config );
		}
	}

	for ( group_index = 0; group_index < iec61850_sav_msg_info.sub_group_profile->total_group; group_index++ )
	{
		if ( error_type == IEC61850_ERR_NO_ERROR )
		{
			Get_User_Config_Sub( &temp_user_config, group_index );	///< Update all SAV Subscribe messages config
			///< and
			///< send to sav interface
			error_type = m_sav_interface->Set_Subscribe_Config( &temp_user_config );
		}
	}

	if ( error_type == IEC61850_ERR_NO_ERROR )
	{
		m_sav_interface->Start();
	}
	else
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Error in Sav configuration \n" );
	}
	return ( error_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Get_User_Config_Pub( user_config_t* user_config, group_param_t group_index )
{
	sub_group_param_t sub_group_index = DISABLE_VALUE;
	uint8_t index = DISABLE_VALUE;
	uint8_t array_index = DISABLE_VALUE;
	const IEC61850_SAV_DCI::sav_group_info_t* search_struct = nullptr;
	CIEC61850_SAV_Interface::sav_user_report_param_t temp_user_report;
	DCI_LENGTH_TD data_type_len[MAX_PARAM_CONTROL_BLOCK_SAV];
	DCI_LENGTH_TD sub_group_len = DISABLE_VALUE;
	DCI_LENGTH_TD main_group_len = DISABLE_VALUE;
	mac_address_t temp_mac_addr;

	memset( &temp_user_report, DISABLE_VALUE, sizeof( temp_user_report ) );
	memset( &temp_mac_addr, DISABLE_VALUE, sizeof( temp_mac_addr ) );
	memset( data_type_len, DISABLE_VALUE, sizeof( DCI_LENGTH_TD ) * MAX_PARAM_CONTROL_BLOCK_SAV );

	search_struct = iec61850_sav_msg_info.pub_group_profile;
	m_pub_group_index[group_index].main_group_start = m_pub_array_index;

	m_sav_dci->Get_Group_Assembly_Parameters( group_index, &main_group_len,
											  &m_pub_config_array[m_pub_array_index],
											  IEC61850_SAV_DCI::PUBLISH );
	m_sav_dci->Get_Maingroup_Params_Length( group_index, data_type_len, IEC61850_SAV_DCI::PUBLISH );

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->vlan_priority ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->vlan_vID ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->vlan_appID ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &temp_mac_addr.addr ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];
	temp_mac_addr.len = MAX_MAC_ADDRESS_SIZE;
	user_config->multicast_addr = temp_mac_addr;

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->svID ), &m_pub_config_array[m_pub_array_index],
						 data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->svcbRef ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->data_set_name ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->conf_rev ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->samp_sync ),
						 &m_pub_config_array[m_pub_array_index], data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->samp_rate ), &m_pub_config_array[m_pub_array_index],
						 data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->samp_mode ), &m_pub_config_array[m_pub_array_index],
						 data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->num_cycles_per_sec ),
						 &m_pub_config_array[m_pub_array_index],
						 data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->num_asdu ), &m_pub_config_array[m_pub_array_index],
						 data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->num_entries ),
						 &m_pub_config_array[m_pub_array_index],
						 data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	temp_user_report.num_data_obj = search_struct->dci_group_struct[group_index].total_subgroups;
	m_pub_group_index[group_index].sub_group_start = m_pub_array_index;
	for ( sub_group_index = 0;
		  sub_group_index < search_struct->dci_group_struct[group_index].total_subgroups;
		  sub_group_index++ )
	{
		m_sav_dci->Get_Subgroup_Assembly_Parameters( group_index, sub_group_index, &sub_group_len,
													 &m_pub_config_array[m_pub_array_index],
													 IEC61850_SAV_DCI::PUBLISH );
		m_sav_dci->Get_Subgroup_Param_Length( group_index, sub_group_index, data_type_len,
											  IEC61850_SAV_DCI::PUBLISH );
		temp_user_report.data_obj[sub_group_index].num_data_attr =
			search_struct->dci_group_struct[group_index].subgroup_array[sub_group_index].
			subgroup_total_params;

		for ( index = 0; index < search_struct->dci_group_struct[group_index].subgroup_array[sub_group_index].
			  subgroup_total_params; index++ )
		{
			temp_user_report.data_obj[sub_group_index].data_attr[index].data_type =
				search_struct->dci_group_struct[group_index].subgroup_array[sub_group_index].
				subgroup_param_tag_id[index];

			for ( array_index = 0; array_index < MAX_NUM_ASDU; array_index++ )
			{
				temp_user_report.data_obj[sub_group_index].data_attr[index].data_value[array_index] =
					( ( &m_pub_config_array[m_pub_array_index] ) );
				m_pub_array_index += data_type_len[index] / MAX_NUM_ASDU;
			}
		}
	}
	user_config->user_report_param = temp_user_report;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Get_User_Config_Sub( user_config_t* user_config, group_param_t group_index )
{
	sub_group_param_t sub_group_index = DISABLE_VALUE;
	uint8_t index = DISABLE_VALUE;
	uint8_t array_index = DISABLE_VALUE;
	const IEC61850_SAV_DCI::sav_group_info_t* search_struct = nullptr;
	CIEC61850_SAV_Interface::sav_user_report_param_t temp_user_report;
	DCI_LENGTH_TD data_type_len[MAX_PARAM_CONTROL_BLOCK_SAV];
	mac_address_t temp_mac_addr;
	DCI_LENGTH_TD sub_group_len = DISABLE_VALUE;
	DCI_LENGTH_TD main_group_len = DISABLE_VALUE;

	memset( &temp_user_report, DISABLE_VALUE, sizeof( temp_user_report ) );
	memset( &temp_mac_addr, DISABLE_VALUE, sizeof( temp_mac_addr ) );
	memset( data_type_len, DISABLE_VALUE, sizeof( DCI_LENGTH_TD ) * MAX_PARAM_CONTROL_BLOCK_SAV );


	search_struct = iec61850_sav_msg_info.sub_group_profile;
	m_sub_group_index[group_index].main_group_start = m_sub_array_index;

	m_sav_dci->Get_Group_Assembly_Parameters( group_index, &main_group_len,
											  &m_sub_config_array[m_sub_array_index],
											  IEC61850_SAV_DCI::SUBSCRIBE );
	m_sav_dci->Get_Maingroup_Params_Length( group_index, data_type_len, IEC61850_SAV_DCI::SUBSCRIBE );

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->vlan_priority ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->vlan_vID ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->vlan_appID ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &temp_mac_addr.addr ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];
	temp_mac_addr.len = MAX_MAC_ADDRESS_SIZE;
	user_config->multicast_addr = temp_mac_addr;

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->svID ), &m_sub_config_array[m_sub_array_index],
						 data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->svcbRef ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->data_set_name ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->conf_rev ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->samp_sync ),
						 &m_sub_config_array[m_sub_array_index], data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->samp_rate ), &m_sub_config_array[m_sub_array_index],
						 data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->samp_mode ), &m_sub_config_array[m_sub_array_index],
						 data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->num_cycles_per_sec ),
						 &m_sub_config_array[m_sub_array_index],
						 data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->num_asdu ), &m_sub_config_array[m_sub_array_index],
						 data_type_len[index] );
	m_sub_array_index += data_type_len[index++];

	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &user_config->num_entries ),
						 &m_sub_config_array[m_sub_array_index],
						 data_type_len[index] );
	m_pub_array_index += data_type_len[index++];

	temp_user_report.num_data_obj = search_struct->dci_group_struct[group_index].total_subgroups;
	m_sub_group_index[group_index].sub_group_start = m_sub_array_index;

	for ( sub_group_index = 0;
		  sub_group_index < search_struct->dci_group_struct[group_index].total_subgroups;
		  sub_group_index++ )
	{
		m_sav_dci->Get_Subgroup_Assembly_Parameters( group_index, sub_group_index, &sub_group_len,
													 &m_sub_config_array[m_sub_array_index],
													 IEC61850_SAV_DCI::SUBSCRIBE );
		m_sav_dci->Get_Subgroup_Param_Length( group_index, sub_group_index, data_type_len,
											  IEC61850_SAV_DCI::SUBSCRIBE );

		temp_user_report.data_obj[sub_group_index].num_data_attr =
			search_struct->dci_group_struct[group_index].subgroup_array[sub_group_index].
			subgroup_total_params;

		for ( index = 0; index < search_struct->dci_group_struct[group_index].subgroup_array[sub_group_index].
			  subgroup_total_params; index++ )
		{
			temp_user_report.data_obj[sub_group_index].data_attr[index].data_type =
				search_struct->dci_group_struct[group_index].subgroup_array[sub_group_index].
				subgroup_param_tag_id[index];

			for ( array_index = 0; array_index < MAX_NUM_ASDU; array_index++ )
			{
				temp_user_report.data_obj[sub_group_index].data_attr[index].data_value[array_index] =
					( ( &m_sub_config_array[m_sub_array_index] ) );
				m_sub_array_index += data_type_len[index] / MAX_NUM_ASDU;
			}
		}
	}
	user_config->user_report_param = temp_user_report;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD CIEC61850_SAV_DCI_Manager::Publish_Change_Tracker_Static( DCI_CBACK_PARAM_TD handle,
																		DCI_ACCESS_ST_TD* access_struct )
{
	CIEC61850_SAV_DCI_Manager* object_ptr = static_cast<CIEC61850_SAV_DCI_Manager*>( handle );

	return ( object_ptr->Publish_Change_Tracker( access_struct ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD CIEC61850_SAV_DCI_Manager::Publish_Change_Tracker( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;

	m_pub_ind_flag = true;
	m_data_id = access_struct->data_id;

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_SAV_DCI_Manager::Send_Publish_Update_Data( void )
{
	iec61850_error_t dci_error = IEC61850_ERR_NO_ERROR;
	group_param_t group_id = DISABLE_VALUE;
	sub_group_param_t sub_group_id = DISABLE_VALUE;
	bool loop_break = false;
	DCI_LENGTH_TD pub_length = DISABLE_VALUE;
	DCI_LENGTH_TD sub_length = DISABLE_VALUE;
	uint16_t array_index = DISABLE_VALUE;
	sub_group_param_t loop_index = DISABLE_VALUE;

	if ( m_pub_ind_flag == true )
	{
		if ( IEC61850_ERR_NO_ERROR !=
			 ( dci_error =
				   m_sav_dci->Get_Group_Id_Details( m_data_id, &group_id, &sub_group_id,
													IEC61850_SAV_DCI::PUBLISH ) ) )
		{
			loop_break = true;
		}
		array_index = m_pub_group_index[group_id].main_group_start;

		// Get main group parameters
		if ( ( loop_break == false ) &&
			 ( IEC61850_ERR_NO_ERROR !=
			   ( dci_error =
					 m_sav_dci->Get_Group_Assembly_Parameters( group_id, &pub_length,
															   &m_pub_config_array[array_index],
															   IEC61850_SAV_DCI::PUBLISH ) ) ) )
		{
			loop_break = true;
		}

		// Get sub group parameters
		array_index = m_pub_group_index[group_id].sub_group_start;
		for ( loop_index = 0;
			  loop_index < iec61850_sav_msg_info.pub_group_profile->dci_group_struct[group_id].total_subgroups;
			  loop_index++ )
		{
			if ( ( loop_break == false ) &&
				 ( IEC61850_ERR_NO_ERROR !=
				   ( dci_error =
						 m_sav_dci->Get_Subgroup_Assembly_Parameters( group_id, loop_index, &sub_length,
																	  &m_pub_config_array[array_index],
																	  IEC61850_SAV_DCI::PUBLISH ) ) ) )
			{
				loop_break = true;
			}
			array_index += sub_length;
		}
		m_data_id = DCI_ID_UNDEFINED;
		m_pub_ind_flag = false;
	}
	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Stop( void )
{
	m_sav_interface->Stop();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Delete( void )
{
	m_sav_interface->Delete();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Subscribe_Change_Tracker( void )
{
	group_param_t group_id = DISABLE_VALUE;
	sub_group_param_t sub_group_id = DISABLE_VALUE;
	uint16_t array_index = DISABLE_VALUE;
	uint16_t owner_array_index = DISABLE_VALUE;
	DCI_LENGTH_TD length = DISABLE_VALUE;
	sub_group_param_t sub_loop_index = DISABLE_VALUE;
	DCI_ACCESS_ST_TD access_struct;
	DCI_ID_TD temp_dcid;
	iec61850_tagtype_t temp_tag_type;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;

	if ( m_sub_ind_flag == true )
	{
		array_index = m_sub_group_index[group_id].sub_group_start;

		for ( sub_group_id = 0;
			  sub_group_id < iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].total_subgroups;
			  sub_group_id++ )
		{
			Get_Owner_Array_Index( IEC61850_SAV_DCI::SUBSCRIBE, group_id, sub_group_id, &owner_array_index, 0 );
			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "checkedindata for group ID = %d and sub group ID = %d", group_id,
								  sub_group_id );
			for ( sub_loop_index = 0;
				  sub_loop_index <
				  iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
				  subgroup_total_params;
				  sub_loop_index++ )
			{
				temp_dcid =
					iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
					subgroup_param_id[sub_loop_index];

				access_struct.source_id = m_source_id;
				access_struct.command = DCI_ACCESS_SET_RAM_CMD;
				access_struct.data_id = temp_dcid;
				access_struct.data_access.data = &m_sub_config_array[array_index];
				access_struct.data_access.length = DISABLE_VALUE;
				dci_error = m_dci_access->Data_Access( &access_struct );
				m_dci_access->Get_Length( temp_dcid, &length );
				temp_tag_type =
					iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
					subgroup_param_tag_id[sub_loop_index];

				Print_DCI_Checked_Data( temp_tag_type,
										iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].subgroup_array[
											sub_group_id].
										subgroup_param_id[sub_loop_index] );
				array_index += length;
				owner_array_index++;
			}
		}
		m_sub_ind_flag = false;
	}
	if ( dci_error != DCI_ERR_NO_ERROR )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_ERROR, "DCI checkout failed" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Print_DCI_Checked_Data( iec61850_tagtype_t tag_type, DCI_ID_TD dcid )
{
	uint64_t temp_checked_out_data_64[MAX_NUM_ASDU] = {DISABLE_VALUE};
	uint16_t temp_checked_out_data_16[MAX_NUM_ASDU] = { DISABLE_VALUE };
	uint8_t temp_checked_out_data_8[MAX_NUM_ASDU] = {DISABLE_VALUE};
	uint32_t temp_checked_out_data_32[MAX_NUM_ASDU] = { DISABLE_VALUE };
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;

	access_struct.source_id = m_source_id;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.data_access.length = DISABLE_VALUE;
	access_struct.data_access.offset = DISABLE_VALUE;

	switch ( tag_type )
	{
		case TAG_TYPE_BOOL:
		case TAG_TYPE_UINT8:
			access_struct.data_access.data = &temp_checked_out_data_8;
			dci_error = m_dci_access->Data_Access( &access_struct );

			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG,
								  "checkedin data to status is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
								  temp_checked_out_data_8[0],
								  temp_checked_out_data_8[1],
								  temp_checked_out_data_8[2],
								  temp_checked_out_data_8[3],
								  temp_checked_out_data_8[4],
								  temp_checked_out_data_8[5],
								  temp_checked_out_data_8[6],
								  temp_checked_out_data_8[7] );
			break;

		case TAG_TYPE_Q:
		case TAG_TYPE_UINT16:
			access_struct.data_access.data = &temp_checked_out_data_16;
			dci_error = m_dci_access->Data_Access( &access_struct );

			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG,
								  "checkedin data to status is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
								  temp_checked_out_data_16[0],
								  temp_checked_out_data_16[1],
								  temp_checked_out_data_16[2],
								  temp_checked_out_data_16[3],
								  temp_checked_out_data_16[4],
								  temp_checked_out_data_16[5],
								  temp_checked_out_data_16[6],
								  temp_checked_out_data_16[7] );
			break;

		case TAG_TYPE_UINT32:
			access_struct.data_access.data = &temp_checked_out_data_32;
			dci_error = m_dci_access->Data_Access( &access_struct );

			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG,
								  "checkedin data to status is 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx",
								  temp_checked_out_data_32[0],
								  temp_checked_out_data_32[1],
								  temp_checked_out_data_32[2],
								  temp_checked_out_data_32[3],
								  temp_checked_out_data_32[4],
								  temp_checked_out_data_32[5],
								  temp_checked_out_data_32[6],
								  temp_checked_out_data_32[7] );
			break;

		case TAG_TYPE_T:
			access_struct.data_access.data = &temp_checked_out_data_64;
			dci_error = m_dci_access->Data_Access( &access_struct );

			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG,
								  "checkedin data to status is 0x%llx, 0x%llx, 0x%llx, 0x%llx, 0x%llx, 0x%llx, 0x%llx, 0x%llx",
								  temp_checked_out_data_64[0],
								  temp_checked_out_data_64[1],
								  temp_checked_out_data_64[2],
								  temp_checked_out_data_64[3],
								  temp_checked_out_data_64[4],
								  temp_checked_out_data_64[5],
								  temp_checked_out_data_64[6],
								  temp_checked_out_data_64[7] );
			break;

		default:
			IEC61850_DEBUG_PRINT( DBG_MSG_ERROR, "Data type is not supported" );
			break;
	}
	if ( dci_error != DCI_ERR_NO_ERROR )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_ERROR, "DCI checkout failed" );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD CIEC61850_SAV_DCI_Manager::Create_Pub_Config_Dci_Owners( void )
{
	group_param_t group_id = DISABLE_VALUE;
	sub_group_param_t sub_group_id = DISABLE_VALUE;
	DCI_ID_TD dcid;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	uint16_t array_index = DISABLE_VALUE;
	group_param_t loop_index = DISABLE_VALUE;
	bool loop_break = false;

	for ( group_id = 0; group_id < iec61850_sav_msg_info.pub_group_profile->total_group; group_id++ )
	{
		for ( loop_index = 0;
			  loop_index < iec61850_sav_msg_info.pub_group_profile->dci_group_struct[group_id].maingroup_total_params;
			  loop_index++ )
		{
			dcid = iec61850_sav_msg_info.pub_group_profile->dci_group_struct[group_id].maingroup_param_id[loop_index];
			if ( ( dcid != DCI_ID_UNDEFINED ) && ( loop_break == false ) )
			{
				m_pub_group_owner[group_id][array_index] = new DCI_Owner( dcid );
				array_index++;
			}
			else
			{
				dci_error = DCI_ERR_INVALID_DATA_ID;
				loop_break = true;
			}
		}

		for ( sub_group_id = 0;
			  sub_group_id < iec61850_sav_msg_info.pub_group_profile->dci_group_struct[group_id].total_subgroups;
			  sub_group_id++ )
		{
			for ( loop_index = 0;
				  loop_index <
				  iec61850_sav_msg_info.pub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
				  subgroup_total_params;
				  loop_index++ )
			{
				dcid =
					iec61850_sav_msg_info.pub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
					subgroup_param_id[
						loop_index];
				if ( ( dcid != DCI_ID_UNDEFINED ) && ( loop_break == false ) )
				{
					m_pub_group_owner[group_id][array_index] = new DCI_Owner( dcid );
					m_pub_group_owner[group_id][array_index]->Attach_Callback(
						&Publish_Change_Tracker_Static,
						reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
						DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_SET_DEFAULT_CMD_MSK );
					array_index++;
				}
				else
				{
					dci_error = DCI_ERR_INVALID_DATA_ID;
					loop_break = true;
				}
			}
		}
	}
	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD CIEC61850_SAV_DCI_Manager::Create_Sub_Config_Dci_Owners( void )
{
	group_param_t group_id = DISABLE_VALUE;
	sub_group_param_t sub_group_id = DISABLE_VALUE;
	DCI_ID_TD dcid;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	uint16_t array_index = DISABLE_VALUE;
	group_param_t loop_index = DISABLE_VALUE;
	bool loop_break = false;

	for ( group_id = 0; group_id < iec61850_sav_msg_info.sub_group_profile->total_group; group_id++ )
	{
		for ( loop_index = 0;
			  loop_index < iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].maingroup_total_params;
			  loop_index++ )
		{
			dcid = iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].maingroup_param_id[loop_index];
			if ( ( dcid != DCI_ID_UNDEFINED ) && ( loop_break == false ) )
			{
				m_sub_group_owner[group_id][array_index] = new DCI_Owner( dcid );
				array_index++;
			}
			else
			{
				dci_error = DCI_ERR_INVALID_DATA_ID;
				loop_break = true;
			}
		}

		for ( sub_group_id = 0;
			  sub_group_id < iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].total_subgroups;
			  sub_group_id++ )
		{

			for ( loop_index = 0;
				  loop_index <
				  iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
				  subgroup_total_params;
				  loop_index++ )
			{
				dcid =
					iec61850_sav_msg_info.sub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
					subgroup_param_id[
						loop_index];

				if ( ( dcid != DCI_ID_UNDEFINED ) && ( loop_break == false ) )
				{
					m_sub_group_owner[group_id][array_index] = new DCI_Owner( dcid );
					array_index++;

				}
				else
				{
					dci_error = DCI_ERR_INVALID_DATA_ID;
					loop_break = true;
				}
			}
		}
	}
	return ( dci_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Write_Pub_Attr( void* state_change, group_param_t group_id,
												sub_group_param_t sub_group_id,
												uint8_t data_atribute_num )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ID_TD temp_dcid;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;

	temp_dcid = iec61850_sav_msg_info.pub_group_profile->dci_group_struct[group_id].subgroup_array[sub_group_id].
		subgroup_param_id[data_atribute_num];
	access_struct.source_id = m_source_id;
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.data_id = temp_dcid;
	access_struct.data_access.data = state_change;
	access_struct.data_access.length = DISABLE_VALUE;
	dci_error = m_dci_access->Data_Access( &access_struct );
	if ( dci_error != DCI_ERR_NO_ERROR )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_ERROR, "DCI checkout failed" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_DCI_Manager::Get_Owner_Array_Index( uint8_t message_type, group_param_t group_id,
													   sub_group_param_t sub_group_id, uint16_t* index,
													   uint8_t data_atribute_num )
{
	const IEC61850_SAV_DCI::sav_group_info_t* search_struct = iec61850_sav_msg_info.sub_group_profile;
	sub_group_param_t loop_index;
	sub_group_param_t sub_loop_index;
	uint16_t array_index = DISABLE_VALUE;
	bool loop_break = false;

	if ( !( ( message_type == IEC61850_SAV_DCI::PUBLISH ) || ( message_type == IEC61850_SAV_DCI::SUBSCRIBE ) ) )
	{
		loop_break = true;
	}

	if ( ( loop_break == false ) && ( message_type == IEC61850_SAV_DCI::PUBLISH ) )
	{
		search_struct = iec61850_sav_msg_info.pub_group_profile;
	}

	for ( loop_index = 0;
		  loop_index < search_struct->dci_group_struct[group_id].total_subgroups;
		  loop_index++ )
	{
		if ( loop_break == false )
		{
			for ( sub_loop_index = 0;
				  sub_loop_index <
				  search_struct->dci_group_struct[group_id].subgroup_array[sub_group_id].
				  subgroup_total_params; sub_loop_index++ )
			{
				if ( ( sub_group_id == loop_index ) && ( sub_loop_index == data_atribute_num ) )
				{
					loop_break = true;
					break;
				}
				array_index++;
			}
		}
	}
	*index = array_index;
}
