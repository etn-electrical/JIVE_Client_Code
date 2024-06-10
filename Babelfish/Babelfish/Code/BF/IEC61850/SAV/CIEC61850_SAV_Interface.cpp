/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "CIEC61850_SAV_Interface.h"
#include "CIEC61850_SAV_DCI_Manager.h"

#include "OS_Task_Config.h"

#include "uC_Timers.h"
#include "uC_Timers_HW.h"
#include "uC_Single_Timer.h"
uC_Single_Timer* s_timer;

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::m_little_endian = false;
TaskHandle_t CIEC61850_SAV_Interface::m_sav_thread_wakeup = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_SAV_Interface::CIEC61850_SAV_Interface( void )
{
	m_sav_context = NULL;
	m_sav_config_param_pub = NULL;
	m_sav_config_param_sub = NULL;
	m_num_sav_config_param_pub = DISABLE_VALUE;
	m_num_sav_config_param_sub = DISABLE_VALUE;

	m_sav_state = SAV_STATE_INIT;
	m_little_endian = false;
	m_sampling_task = nullptr;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Initialize( void )
{
	bool retun_type = TRUE;
	int16_t value = ENABLE_VALUE;

	if ( m_sav_context == NULL )
	{
		sav_context_t* ptr_sav_context_t;
		ptr_sav_context_t = reinterpret_cast<sav_context_t*>( TMW_TARG_MALLOC( sizeof( sav_context_t ) ) );

		if ( ptr_sav_context_t == NULL )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Unable to malloc Goosecontext" );
			retun_type = FALSE;
		}
		else
		{
			memset( ptr_sav_context_t, DISABLE_VALUE, sizeof( sav_context_t ) );
			if ( !Open_Context( ptr_sav_context_t ) )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Sav context failed to open!" );
				retun_type = FALSE;
			}
			else
			{
				m_sav_context = ptr_sav_context_t;
				retun_type = TRUE;
			}
		}
	}
	else
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Sav context structure is already initialized" );
		retun_type = TRUE;
	}

	/* Test 'endianness' of the machine */
	if ( *( ( uint8_t* ) &value ) == ENABLE_VALUE )
	{
		m_little_endian = true;
	}
	else
	{
		m_little_endian = false;
	}

	return ( retun_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_SAV_Interface::Set_Publish_Config( user_sav_config_param_t* user_sav_pub_config_param )
{
	iec61850_error_t return_error = IEC61850_ERR_NO_ERROR;

	return_error = Set_Config( ( user_sav_config_param_t* ) user_sav_pub_config_param, IEC61850_MTYPE_PUBLISH );

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_SAV_Interface::Set_Subscribe_Config( user_sav_config_param_t* user_sav_sub_config_param )
{
	iec61850_error_t return_error = IEC61850_ERR_NO_ERROR;

	return_error = Set_Config( ( user_sav_config_param_t* ) user_sav_sub_config_param, IEC61850_MTYPE_SUBSCRIBE );

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Start( void )
{
	if ( m_sav_state == SAV_STATE_INIT )
	{
		if ( m_sav_context == NULL )
		{
			Initialize();
		}

		sav_config_param_t* temp_param;
		// start sav publish init
		temp_param = m_sav_config_param_pub;
		while ( temp_param != NULL )
		{
			Create( temp_param );
			temp_param = temp_param->next;
		}

		// Start sav subscribe init
		temp_param = m_sav_config_param_sub;
		while ( temp_param != NULL )
		{
			Subscribe( temp_param );
			temp_param = temp_param->next;
		}
	}

	m_sav_state = SAV_STATE_RUN;

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_Interface::Service( void )
{
	if ( m_sav_state == SAV_STATE_RUN )
	{
		TMW_SAV_Service( m_sav_context );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Stop( void )
{
	m_sav_state = SAV_STATE_STOP;
	delete m_sampling_task;
	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Delete( void )
{
	sav_context_t* cntxt;

	cntxt = m_sav_context;
	while ( cntxt->active_out != NULL )
	{
		TMW_SAV_Delete( cntxt, ( void* ) cntxt->active_out->handle );
	}

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Unsubscribe( void )
{
	sav_context_t* cntxt;

	cntxt = m_sav_context;
	while ( cntxt->active_in != NULL )
	{
		TMW_SAV_Unsubscribe( cntxt, ( void* ) cntxt->active_in->handle );
	}

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Close( void )
{
	TMW_SAV_Close( m_sav_context );

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Open_Context( sav_context_t* ptr_sav_context_t )
{
	sav_error_t err_type;
	bool return_type;

	if ( ptr_sav_context_t->adapter_name[0] == DISABLE_VALUE )
	{
		ETH_adapter_dialog( "SAV adapter is not specified, ", ptr_sav_context_t->adapter_name );
	}

	err_type = TMW_SAV_Initialize( ptr_sav_context_t );
	if ( err_type != TMW_SAV_SUCCESS )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "TMW_SAV_Initialize returned error %d", err_type );
		return_type = FALSE;
	}
	else
	{
		return_type = TRUE;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_SAV_Interface::Set_Config( user_sav_config_param_t* user_savcb_config_param,
													  message_type_t message_type )
{
	iec61850_error_t return_error = IEC61850_ERR_NO_ERROR;
	uint8_t index_data_obj = DISABLE_VALUE;
	uint8_t index_data_attr = DISABLE_VALUE;
	sav_config_param_t* sav_node = m_sav_config_param_sub;
	data_type_t data_type;
	void* data_value[MAX_NUM_ASDU];
	sav_config_param_t* temp_node;
	uint8_t num_data_obj = DISABLE_VALUE;
	data_obj_t* data_obj = NULL;
	data_attr_t* data_attr;
	uint8_t num_dat_attr = DISABLE_VALUE;
	bool loop_break = false;
	uint8_t str_len = DISABLE_VALUE;

	if ( !( ( message_type == IEC61850_MTYPE_PUBLISH ) || ( message_type == IEC61850_MTYPE_SUBSCRIBE ) ) )
	{
		return_error = IEC61850_ERR_MTYPE_INVALID;
		loop_break = true;
	}
	if ( loop_break == false )
	{
		if ( message_type == IEC61850_MTYPE_PUBLISH )
		{
			sav_node = m_sav_config_param_pub;
		}

		while ( sav_node != NULL )
		{
			sav_node = sav_node->next;
		}

		if ( ( loop_break == false ) && ( sav_node == NULL ) )
		{
			sav_node = reinterpret_cast<sav_config_param_t*>( TMW_TARG_MALLOC( sizeof ( sav_config_param_t ) ) );

			if ( ( loop_break == false ) && ( sav_node == NULL ) )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Unable to malloc sav_node structure" );
				return_error = IEC61850_ERR_MEMORY_ALLOC;
				loop_break = true;
			}
		}

		if ( ( sav_node != NULL ) )
		{
			memset( sav_node, DISABLE_VALUE, sizeof( sav_config_param_t ) );

			sav_node->vlan_priority = user_savcb_config_param->vlan_priority;
			sav_node->vlan_vID = user_savcb_config_param->vlan_vID;
			sav_node->vlan_appID = user_savcb_config_param->vlan_appID;

			// copy multi cast addr
			sav_node->multicast_addr = user_savcb_config_param->multicast_addr;

			// copy dataset config
			str_len = ( int16_t ) strlen( user_savcb_config_param->svID ) + ENABLE_VALUE;
			if ( str_len > CB_IDENT_SIZE_SAV )
			{
				str_len = CB_IDENT_SIZE_SAV;
			}
			memcpy( sav_node->svID, user_savcb_config_param->svID, str_len );

			str_len = ( int16_t ) strlen( user_savcb_config_param->data_set_name ) + ENABLE_VALUE;
			if ( str_len > CB_IDENT_SIZE_SAV )
			{
				str_len = CB_IDENT_SIZE_SAV;
			}
			memcpy( sav_node->data_set_name, user_savcb_config_param->data_set_name, str_len );

			sav_node->conf_rev = user_savcb_config_param->conf_rev;
			sav_node->samp_sync = user_savcb_config_param->samp_sync;
			sav_node->samp_rate = user_savcb_config_param->samp_rate;
			sav_node->samp_mode = user_savcb_config_param->samp_mode;
			sav_node->num_cycles_per_sec = user_savcb_config_param->num_cycles_per_sec;
			sav_node->num_asdu = user_savcb_config_param->num_asdu;

			sav_node->useSecurity = 0;
			sav_node->security[DISABLE_VALUE] = DISABLE_VALUE;
			sav_node->nSecurityLen = 0;
			sav_node->valid_members = TMW_SAV_APDU_MINIMUM;

			sav_node->ptr_sav_context = m_sav_context;
			sav_node->ptr_apdu = NULL;
			sav_node->sav_report_param.data_obj->next = NULL;

			num_data_obj = user_savcb_config_param->user_report_param.num_data_obj;
			for ( index_data_obj = 0; index_data_obj < num_data_obj; index_data_obj++ )
			{
				data_obj = reinterpret_cast<data_obj_t*>( TMW_TARG_MALLOC( sizeof ( data_obj_t ) ) );
				if ( ( data_obj == NULL ) || ( loop_break == true ) )
				{
					IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Unable to malloc data_obj structure" );
					return_error = IEC61850_ERR_MEMORY_ALLOC;
					loop_break = true;
				}
				else
				{
					data_obj->next = sav_node->sav_report_param.data_obj;
					sav_node->sav_report_param.data_obj = data_obj;
					num_dat_attr =
						user_savcb_config_param->user_report_param.data_obj[num_data_obj - ENABLE_VALUE -
																			index_data_obj].num_data_attr;
					sav_node->sav_report_param.data_obj->data_attr = NULL;
					sav_node->sav_report_param.data_obj->data_attr->next = NULL;

					for ( index_data_attr = 0; index_data_attr < num_dat_attr; index_data_attr++ )
					{
						data_type =
							user_savcb_config_param->user_report_param.data_obj[num_data_obj - ENABLE_VALUE -
																				index_data_obj].
							data_attr[num_dat_attr - ENABLE_VALUE - index_data_attr].data_type;
						data_value[0] =
							user_savcb_config_param->user_report_param.data_obj[num_data_obj - ENABLE_VALUE -
																				index_data_obj].
							data_attr[num_dat_attr - ENABLE_VALUE - index_data_attr].data_value[0];

						data_attr = Push_Node( &sav_node, data_type, data_value[0] );

						if ( ( loop_break == false ) && ( data_attr == NULL ) )
						{
							IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Unable to malloc data_attr structure" );
							return_error = IEC61850_ERR_MEMORY_ALLOC;
							loop_break = true;
						}

						if ( ( loop_break == false ) && ( ( sav_node->sav_report_param.data_obj->data_attr == NULL ) ) )
						{
							data_attr->next = NULL;
							sav_node->sav_report_param.data_obj->data_attr = data_attr;
						}
						else
						{
							data_attr->next = sav_node->sav_report_param.data_obj->data_attr;
							sav_node->sav_report_param.data_obj->data_attr = data_attr;
						}
					}
					data_obj->data_attr = data_attr;
					data_obj->num_data_attr = num_dat_attr;
					sav_node->sav_report_param.data_obj = data_obj;
				}
			}


			if ( ( loop_break == false ) )
			{
				sav_node->sav_report_param.num_data_obj = num_data_obj;

				if ( message_type == IEC61850_MTYPE_PUBLISH )
				{
					m_num_sav_config_param_pub++;
					if ( m_sav_config_param_pub == NULL )
					{
						m_sav_config_param_pub = sav_node;
						sav_node->next = NULL;
					}
					else
					{
						temp_node = m_sav_config_param_pub;
						while ( temp_node->next != NULL )
						{
							temp_node = temp_node->next;
						}
						temp_node->next = sav_node;
						sav_node->next = NULL;
					}
				}
				else
				{
					m_num_sav_config_param_sub++;
					if ( m_sav_config_param_sub == NULL )
					{
						m_sav_config_param_sub = sav_node;
						sav_node->next = NULL;
					}
					else
					{
						temp_node = m_sav_config_param_sub;
						while ( temp_node->next != NULL )
						{
							temp_node = temp_node->next;
						}
						temp_node->next = sav_node;
						sav_node->next = NULL;
					}
				}
			}
		}
	}
	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_SAV_Interface::data_attr_t* CIEC61850_SAV_Interface::Push_Node( sav_config_param_t** sav_node1,
																		  data_type_t data_type, void* data_value )
{
	data_attr_t* ref_node = NULL;
	sav_config_param_t* temp_node = *sav_node1;
	uint8_t index = DISABLE_VALUE;

	ref_node = temp_node->sav_report_param.data_obj->data_attr;

	// Allocate memory for node
	data_attr_t* new_node = reinterpret_cast<data_attr_t*>( TMW_TARG_MALLOC( sizeof( data_attr_t ) ) );

	if ( new_node == NULL )
	{}
	else
	{
		new_node->data_type = data_type;
		switch ( data_type )
		{
			case TAG_TYPE_BOOL:
				for ( index = 0; index < MAX_NUM_ASDU; index++ )
				{
					new_node->data_value[index] = ( uint8_t* )data_value + index;
				}
				break;

			case TAG_TYPE_SINT16:
			case TAG_TYPE_UINT16:
			case TAG_TYPE_Q:
				for ( index = 0; index < MAX_NUM_ASDU; index++ )
				{
					new_node->data_value[index] = ( uint16_t* )data_value + index;
				}
				break;

			case TAG_TYPE_SINT32:
			case TAG_TYPE_UINT32:
				for ( index = 0; index < MAX_NUM_ASDU; index++ )
				{
					new_node->data_value[index] = ( uint32_t* )data_value + index;
				}
				break;

			case TAG_TYPE_T:
				for ( index = 0; index < MAX_NUM_ASDU; index++ )
				{
					new_node->data_value[index] = ( uint64_t* )data_value + index;
				}
				break;

			case TAG_TYPE_FLOAT:
				for ( index = 0; index < MAX_NUM_ASDU; index++ )
				{
					new_node->data_value[index] = ( float* )data_value + index;
				}
				break;

			default:
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "DataType is not supported to fetch value " );
				break;
		}

		new_node->next = ( ref_node );
		ref_node = new_node;
	}
	return ( ref_node );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Create( sav_config_param_t* ptr_sav_params )
{
	bool loop_break = false;
	bool return_type = TRUE;
	sav_apdu_t* ptr_apdu;
	sav_error_t error;
	sav_asdu_t* ptr_asdu;
	int16_t index;

	error = TMW_SAV_Create( ptr_sav_params->ptr_sav_context, ( void* ) ptr_sav_params, ptr_sav_params->security,
							ptr_sav_params->nSecurityLen, &ptr_sav_params->multicast_addr, ptr_sav_params->vlan_appID,
							ptr_sav_params->vlan_vID, ptr_sav_params->vlan_priority );

	if ( error != TMW_SAV_SUCCESS )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "TMW_SAV_Create returned error: %d", error );
		loop_break = true;
		return_type = FALSE;
	}

	ptr_apdu = TMW_SAV_GetApdu( ptr_sav_params->ptr_sav_context, ( void* ) ptr_sav_params );
	if ( ( loop_break == false ) && ( ptr_apdu == NULL ) )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "TMW_SAV_GetApdu returned NULL" );
		return_type = FALSE;
	}
	else if ( loop_break == false )
	{
		ptr_apdu->pUserData = ( void* ) ptr_sav_params;
		ptr_apdu->smpRate = ptr_sav_params->samp_rate;
		ptr_apdu->smpCnt = DISABLE_VALUE;
		ptr_apdu->simulateBit = DISABLE_VALUE;

		ptr_apdu->start_call_back = NULL;
		ptr_apdu->start_call_back_data = NULL;
		ptr_apdu->stop_call_back = NULL;
		ptr_apdu->stop_call_back_data = NULL;

		for ( index = 0; index < ptr_sav_params->num_asdu; index++ )
		{
			ptr_asdu = TMW_SAV_Add_ASDU( ptr_apdu, NULL, ( void* ) NULL );
			if ( ( loop_break == false ) && ( ptr_asdu == NULL ) )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "TMW_SAV_Add_ASDU returned NULL" );
				loop_break = true;
				return_type = FALSE;
			}
			else
			{
				ptr_asdu->smpCnt = DISABLE_VALUE;
				ptr_asdu->confRev = ptr_sav_params->conf_rev;
				MMSd_adjusted_utc_time_stamp( &ptr_asdu->refrTm );
				ptr_asdu->smpSynch = ptr_sav_params->samp_sync;
				ptr_asdu->smpRate = ptr_apdu->smpRate;
				ptr_asdu->smpMod = ptr_sav_params->samp_mode;
				ptr_asdu->validMembers = ptr_sav_params->valid_members;
				ptr_asdu->encode_callback = Encode_Sav;
				ptr_asdu->encode_callback_data = ( void* ) ptr_sav_params;
				strcpy( ptr_asdu->svID, ptr_sav_params->svID );
				strcpy( ptr_asdu->datSet, ptr_sav_params->data_set_name );
				ptr_asdu->validMembers = TMW_SAV_APDU_MINIMUM;
			}
		}
		ptr_sav_params->ptr_apdu = ptr_apdu;

		if ( ( loop_break == false ) && ( Compute_Publish_Intervals( ptr_sav_params ) == FALSE ) )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Unable to compute SAV output intervals" );
			return_type = FALSE;
		}

		Start_Sampling_Thread( ptr_sav_params );
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Subscribe( sav_config_param_t* ptr_sav_params )
{
	bool retrun_type = TRUE;
	sav_error_t error = TMW_SAV_SUCCESS;
	sav_context_t* ptr_sav_context_t = ptr_sav_params->ptr_sav_context;

	error = TMW_SAV_Subscribe( ptr_sav_context_t, &ptr_sav_params->multicast_addr, ptr_sav_params->vlan_appID,
							   ( void* ) ptr_sav_params, ptr_sav_params->num_asdu, Sampled_Values_Indication,
							   ( void* ) ptr_sav_params );
	if ( error != TMW_SAV_SUCCESS )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Subscription for %s failed", ptr_sav_params->svID );
		retrun_type = FALSE;
	}
	else
	{
		retrun_type = TRUE;
	}
	return ( retrun_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t CIEC61850_SAV_Interface::Encode_Sav( sav_apdu_t* ptr_apdu, sav_asdu_t* ptr_asdu, int samp_count,
											  void* encode_callback_data, MMSd_descriptor_t* ptr_output )
{
	MMSd_length length = DISABLE_VALUE;
	sav_config_param_t* ptr_sav_params = ( sav_config_param_t* ) encode_callback_data;
	sav_report_param_t* report_params;

	if ( ptr_sav_params != NULL )
	{
		report_params = ( sav_report_param_t* )( &ptr_sav_params->sav_report_param );
		ptr_sav_params->running_asdu = ptr_sav_params->running_asdu % MAX_NUM_ASDU;
		length = Encode_Dataset( report_params, ptr_output,
								 ( MAX_NUM_ASDU - ENABLE_VALUE - ptr_sav_params->running_asdu ) );
		ptr_sav_params->running_asdu++;

		if ( length > DISABLE_VALUE )
		{
			length = ( TMW_SAV_EmitTagAndLength( sample7, length, ptr_output ) );
		}
	}

	return ( length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t CIEC61850_SAV_Interface::Encode_Dataset( sav_report_param_t* ptr_sav_param, MMSd_descriptor_t* ptr_output,
												  uint8_t running_asdu )
{
	uint8_t index;
	uint8_t num_data_obj = ptr_sav_param->num_data_obj;
	uint16_t length = DISABLE_VALUE;

	for ( index = 0; index < num_data_obj; index++ )
	{
		length += Encode_Data_Attribute( ptr_sav_param->data_obj, num_data_obj - ENABLE_VALUE - index, ptr_output,
										 running_asdu );
	}

	return ( length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t CIEC61850_SAV_Interface::Encode_Data_Attribute( data_obj_t* ptr_data, uint8_t offset,
														 MMSd_descriptor_t* ptr_output, uint8_t running_asdu )
{
	uint8_t num_data_attr = DISABLE_VALUE;
	uint8_t index = DISABLE_VALUE;
	int16_t length = DISABLE_VALUE;

	while ( offset )
	{
		ptr_data = ptr_data->next;
		offset--;
	}
	num_data_attr = ptr_data->num_data_attr;

	for ( index = 0; index < num_data_attr; index++ )
	{
		length +=
			Encode_Emit_Data( ptr_data->data_attr, num_data_attr - ENABLE_VALUE - index, ptr_output, running_asdu );
	}

	return ( length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t CIEC61850_SAV_Interface::Encode_Emit_Data( data_attr_t* ptr_data, uint8_t offset,
													MMSd_descriptor_t* ptr_output, uint8_t running_asdu )
{
	data_type_t data_type;
	int16_t length = DISABLE_VALUE;
	data_attr_t* temp_ptr;
	uint16_t value_16 = DISABLE_VALUE;
	uint16_t* ptr_16 = NULL;
	uint32_t value_32 = DISABLE_VALUE;
	uint32_t* ptr_32 = NULL;
	uint64_t value_64 = DISABLE_VALUE;
	uint64_t* ptr_64 = NULL;

	temp_ptr = ( data_attr_t* )ptr_data;
	while ( offset )
	{
		temp_ptr = temp_ptr->next;
		offset--;
	}

	data_type = temp_ptr->data_type;
	switch ( data_type )
	{
		case TAG_TYPE_BOOL:
		case TAG_TYPE_SINT16:
		case TAG_TYPE_UINT16:
		case TAG_TYPE_Q:
			ptr_16 = ( uint16_t* )temp_ptr->data_value[running_asdu];
			value_16 = *( uint16_t* ) ptr_16;
			if ( m_little_endian == true )
			{
				TMW_SAV_byteorder( ( uint8_t* ) &value_16, sizeof( value_16 ) );
			}
			length = TMW_SAV_EmitOctets( ( uint8_t* ) &value_16, sizeof( value_16 ), ptr_output );
			break;

		case TAG_TYPE_UINT32:
		case TAG_TYPE_SINT32:
		case TAG_TYPE_FLOAT:
			ptr_32 = ( uint32_t* )temp_ptr->data_value[running_asdu];
			value_32 = *( uint32_t* ) ptr_32;
			if ( m_little_endian == true )
			{
				TMW_SAV_byteorder( ( uint8_t* ) &value_32, sizeof( value_32 ) );
			}
			length = TMW_SAV_EmitOctets( ( uint8_t* ) &value_32, sizeof( value_32 ), ptr_output );
			break;

		case TAG_TYPE_T:
			ptr_64 = ( uint64_t* )temp_ptr->data_value[running_asdu];
			value_64 = *( uint64_t* ) ptr_64;
			if ( m_little_endian == true )
			{
				TMW_SAV_byteorder( ( uint8_t* ) &value_64, sizeof( value_64 ) );
			}
			length = TMW_SAV_EmitOctets( ( uint8_t* ) &value_64, sizeof( value_64 ), ptr_output );
			break;

		default:
			IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Data Type is not supported to emit value " );
			break;
	}
	return ( length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Decode_Dataset( uint8_t** ptr_input,
											  sav_report_param_t const* const ptr_sav_params,
											  uint8_t running_asdu )
{
	bool return_type = TRUE;
	uint8_t index = DISABLE_VALUE;
	uint8_t num_data_obj = ptr_sav_params->num_data_obj;

	for ( index = 0; index < num_data_obj; index++ )
	{
		return_type = Decode_Data_Attribute( ptr_input, ptr_sav_params->data_obj, index, running_asdu );
		if ( return_type == FALSE )
		{
			break;
		}
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Decode_Data_Attribute( uint8_t** ptr_input,
													 data_obj_t const* const data_obj, uint8_t offset,
													 uint8_t running_asdu )
{
	bool return_type = TRUE;
	uint8_t loop_index = DISABLE_VALUE;
	uint8_t num_data_attr = DISABLE_VALUE;
	data_attr_t* data_attr;
	data_obj_t* temp_dat_obj;

	temp_dat_obj = ( data_obj_t* ) data_obj;

	while ( offset )
	{
		temp_dat_obj = temp_dat_obj->next;
		offset--;
	}
	num_data_attr = temp_dat_obj->num_data_attr;
	data_attr = temp_dat_obj->data_attr;

	for ( loop_index = 0; loop_index < num_data_attr; loop_index++ )
	{
		return_type = Decode_Fetch_Data( ptr_input, data_attr, running_asdu );
		data_attr = data_attr->next;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Decode_Fetch_Data( uint8_t** ptr_input1, data_attr_t* const ptr_data,
												 uint8_t running_asdu )
{
	bool return_type = TRUE;
	data_type_t data_type = TAG_TYPE_BOOL;
	uint16_t value_u16 = DISABLE_VALUE;
	uint32_t value_u32 = DISABLE_VALUE;
	uint64_t value_u64 = DISABLE_VALUE;
	uint8_t* ptr_input = *ptr_input1;

	data_type = ptr_data->data_type;
	switch ( data_type )
	{
		case TAG_TYPE_BOOL:
		case TAG_TYPE_SINT16:
		case TAG_TYPE_UINT16:
		case TAG_TYPE_Q:
			value_u16 = *( uint16_t* )ptr_input;
			ptr_input += sizeof( value_u16 );

			if ( m_little_endian == true )
			{
				TMW_SAV_byteorder( ( uint8_t* )&value_u16, sizeof( value_u16 ) );
			}

			*( uint16_t* )( ptr_data->data_value[running_asdu] ) = value_u16;
			break;

		case TAG_TYPE_SINT32:
		case TAG_TYPE_UINT32:
		case TAG_TYPE_FLOAT:
			value_u32 = *( uint32_t* )ptr_input;
			ptr_input += sizeof( value_u32 );

			if ( m_little_endian == true )
			{
				TMW_SAV_byteorder( ( uint8_t* ) &value_u32, sizeof( value_u32 ) );
			}

			*( uint32_t* ) ( ptr_data->data_value[running_asdu] ) = value_u32;
			break;

		case TAG_TYPE_T:
			value_u64 = *( uint64_t* )ptr_input;
			ptr_input += sizeof( value_u64 );

			if ( m_little_endian == true )
			{
				TMW_SAV_byteorder( ( uint8_t* ) &value_u64, sizeof( value_u64 ) );
			}

			*( uint64_t* ) ( ptr_data->data_value[running_asdu] ) = value_u64;
			break;

		default:
			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "DataType is not supported to fetch value " );
			break;
	}

	*ptr_input1 = ptr_input;
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_SAV_Interface::Compute_Publish_Intervals( sav_config_param_t* ptr_sav_params )
{
	bool return_type = TRUE;

	if ( ptr_sav_params == NULL )
	{
		return_type = FALSE;
	}
	else if ( ptr_sav_params->num_cycles_per_sec * ptr_sav_params->samp_rate <
			  ( unsigned int ) ptr_sav_params->num_asdu )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL,
							  "Invalid SAV parameters: %dHz * %d samples per second < %d ASDUs per APDU",
							  ptr_sav_params->num_cycles_per_sec, ptr_sav_params->samp_rate, ptr_sav_params->num_asdu );
		return_type = FALSE;
	}
	else
	{
		ptr_sav_params->usec_per_publish = SAV_MEG_VALUE /
			( ptr_sav_params->num_cycles_per_sec * ptr_sav_params->samp_rate / ptr_sav_params->num_asdu );
		ptr_sav_params->publishes_per_interval = ENABLE_VALUE;
		return_type = TRUE;
	}

	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_Interface::Start_Sampling_Thread( sav_config_param_t* ptr_sav_params )
{
	new CR_Tasker( Iec61850_Sav_CR_Tasker, reinterpret_cast<CR_TASKER_PARAM>( ptr_sav_params ),
				   static_cast<uint8_t>( IEC61850_SAV_PUB_THREAD_PRIO ),
				   reinterpret_cast<char_t const*>( IEC61850_SAV_PUB_THREAD_NAME ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_Interface::Iec61850_Sav_CR_Tasker( CR_Tasker* cr_task, CR_TASKER_PARAM pThreadArgs )
{
	CR_Tasker_Begin( cr_task );

	while ( true )
	{
		CR_Tasker_Delay( cr_task, IEC61850_SAV_PUB_SLEEP_TIME_MS );	///< \todo to replace function for uSec wait delay
		Sav_Thread_Function( pThreadArgs );
	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_Interface::Sav_Thread_Function( void* pThreadArgs )
{
	int16_t index = CIEC61850_SAV_Interface::DISABLE_VALUE;
	bool loop_break = false;
	CIEC61850_SAV_Interface::sav_config_param_t* ptr_sav_params =
		( CIEC61850_SAV_Interface::sav_config_param_t* ) pThreadArgs;
	CIEC61850_SAV_Interface::sav_error_t error;

	if ( ptr_sav_params == NULL )
	{
		loop_break = true;
	}

	if ( ( loop_break == false ) && ( ptr_sav_params->ptr_apdu == NULL ) )
	{
		loop_break = true;
	}

	if ( loop_break == false )
	{
		for ( index = 0; index < ptr_sav_params->publishes_per_interval; index++ )
		{
			ptr_sav_params->running_asdu = CIEC61850_SAV_Interface::DISABLE_VALUE;
			error = TMW_SAV_Publish( ptr_sav_params->ptr_sav_context, ( void* ) ptr_sav_params );
			if ( error != TMW_SAV_SUCCESS )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "SAV Publish failed!" );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_Interface::Sampled_Values_Indication( void* ptr_handle, sav_apdu_t* ptr_apdu )
{
	int32_t samp_count = DISABLE_VALUE;
	uint8_t* ptr_input = NULL;
	int index;
	sav_asdu_t* ptr_asdu = NULL;
	bool loop_break = false;
	int16_t haveError = DISABLE_VALUE;

	IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "sampled_values_indication is called " );
	sav_config_param_t* ptr_sav_params = ( sav_config_param_t* )ptr_handle;

	if ( ptr_sav_params == NULL )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Indication handle NULL!!" );
		loop_break = true;
	}
	if ( ( loop_break == false ) && ( ptr_sav_params->num_asdu != ptr_apdu->noASDUs ) )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Number of ASDUs mismatch: expected %d found %d!",
							  ptr_sav_params->num_asdu, ptr_apdu->noASDUs );
		loop_break = true;
	}
	ptr_asdu = ptr_apdu->pHead;

	if ( loop_break == false )
	{
		for ( index = 0; index < ptr_apdu->noASDUs; index++ )
		{
			if ( ( loop_break == false ) && ( ptr_asdu == NULL ) )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Insufficient number of ASDUs in Sampled Values message" );
				break;
			}

			if ( ptr_sav_params->have_seen_one )
			{
				samp_count = ptr_sav_params->samp_cnt + ENABLE_VALUE;
				samp_count = samp_count & 0xFFFF;
				if ( samp_count != ptr_asdu->smpCnt )
				{
					haveError = ENABLE_VALUE;
					IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Sample missing: expected %d, found %d", samp_count,
										  ptr_asdu->smpCnt );
				}
			}
			ptr_sav_params->have_seen_one = ENABLE_VALUE;
			ptr_sav_params->samp_cnt = ptr_asdu->smpCnt;

			if ( ptr_asdu->dataLen == SAV_SUB_SIZE_OF_DATALEN )
			{
				ptr_input = ( uint8_t* ) ptr_asdu->dataPtr;

				if ( ptr_input != NULL )
				{
					Decode_Dataset( &ptr_input, ( sav_report_param_t* )&ptr_sav_params->sav_report_param, index );
					CIEC61850_SAV_DCI_Manager::m_sub_ind_flag = true;
				}
			}
			else
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Sample wrong size for 9-2LE: %d", ptr_asdu->dataLen );
				haveError = ENABLE_VALUE;
			}
			ptr_sav_params->errors += haveError;
			ptr_sav_params->samples_received++;

			if ( ptr_sav_params->samples_received % SAV_KILO_VALUE == DISABLE_VALUE )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "%ld samples received", ptr_sav_params->samples_received );
			}

			ptr_asdu = ptr_asdu->pNext;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_SAV_Interface::Single_Timer_Cback( uC_Single_Timer::cback_param_t param )
{
	BaseType_t higher_priority_task_woken = false;

	vTaskNotifyGiveFromISR( m_sav_thread_wakeup, &higher_priority_task_woken );

	OS_Tasker::Yield_From_ISR( higher_priority_task_woken );
}
