/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "CIEC61850_GOOSE_Interface.h"
#include "CIEC61850_GOOSE_DCI_Manager.h"

#include "OS_Mutex.h"

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
uint16_t CIEC61850_GOOSE_Interface::m_state_num = DISABLE_VALUE;
bool CIEC61850_GOOSE_Interface::m_subscribe_notify = false;
bool CIEC61850_GOOSE_Interface::m_sub_timeout = false;
char_t CIEC61850_GOOSE_Interface::m_sub_goose_cb_ref[] = {DISABLE_VALUE};
CIEC61850_GOOSE_Interface::notify_sub_indication_t CIEC61850_GOOSE_Interface::Subscribtion_Callback = nullptr;

CIEC61850_GOOSE_Interface::goose_strategy_t tst_strategy = {
	IEC_GOOSE_Profile,
	{
		STRATEGY_MULTIPLIER,
		STRATEGY_FIRST_INTERVAL,
		STRATEGY_MAX_INTERVAL,
	},
	{
		STRATEGY_NUM_PROFILES,
		STRATEGY_CURRENT_INDEX,
		{STRATEGY_FIRST_INTERVAL, STRATEGY_SECOND_INTERVAL}
	}
};

static OS_Mutex* m_proess_wake_mutex = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_GOOSE_Interface::CIEC61850_GOOSE_Interface( void )
{
	m_goose_context = nullptr;
	m_gcb_config_param_pub = nullptr;
	m_gcb_config_param_sub = nullptr;
	m_num_gcb_config_param_pub = DISABLE_VALUE;
	m_num_gcb_config_param_sub = DISABLE_VALUE;
	m_initial_goose_wait_ms = IEC61850_INITIAL_WAIT_TIME;
	m_goose_state = GOOSE_STATE_INIT;
	m_subscribe_notify = false;
	m_sub_timeout = false;
	m_state_num = DISABLE_VALUE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_GOOSE_Interface::~CIEC61850_GOOSE_Interface( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Initialize( void )
{
	bool retun_type = TRUE;
	bool loop_break = false;
	goose_context_t* pIecGooseContext;

	if ( m_goose_context == nullptr )
	{
		pIecGooseContext = reinterpret_cast<goose_context_t*>( TMW_TARG_MALLOC( sizeof( goose_context_t ) ) );

		if ( pIecGooseContext == nullptr )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Unable to malloc Goosecontext" );
			retun_type = FALSE;
		}
		else
		{
			memset( pIecGooseContext, DISABLE_VALUE, sizeof( goose_context_t ) );
			if ( ( loop_break == false ) && ( !Open_Context( pIecGooseContext ) ) )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Goose context failed to open!" );
				retun_type = FALSE;
				loop_break = true;
			}

			if ( loop_break == false )
			{
				m_goose_context = pIecGooseContext;
				retun_type = TRUE;
			}
		}
	}
	else
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Goose context structure is already initialized" );
		retun_type = TRUE;
	}
	return ( retun_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_GOOSE_Interface::Set_Publish_Config( user_gcb_config_param_t* user_gcb_config_param )
{
	iec61850_error_t return_error = IEC61850_ERR_NO_ERROR;

	return_error = Set_Config( ( user_gcb_config_param_t* ) user_gcb_config_param, IEC61850_MTYPE_PUBLISH );

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_GOOSE_Interface::Set_Subscribe_Config( user_gcb_config_param_t* user_gcb_config_param )
{
	iec61850_error_t return_error = IEC61850_ERR_NO_ERROR;

	return_error = Set_Config( ( user_gcb_config_param_t* ) user_gcb_config_param, IEC61850_MTYPE_SUBSCRIBE );

	return ( return_error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Send_Publish_Update_Data( uint8_t gcb_num )
{
	uint8_t index = DISABLE_VALUE;
	gcb_config_param_t* ptr_goose_params = nullptr;
	unsigned char buffer[IEC_GOOSE_MAX_DATA_LEN];
	uint16_t member = DISABLE_VALUE;
	uint16_t offset = DISABLE_VALUE;
	uint16_t length = DISABLE_VALUE;
	bool return_type = TRUE;
	bool loop_break = false;
	IEC_GooseHeader header;

	gcb_report_param_t* report_params;

	if ( gcb_num > m_num_gcb_config_param_pub )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Check Goose Control Block param number" );
		loop_break = true;
		return_type = FALSE;
	}

	ptr_goose_params = m_gcb_config_param_pub;

	if ( ptr_goose_params != nullptr )
	{
		for ( index = 1; index < gcb_num; index++ )
		{
			if ( ( loop_break == false ) && ( ptr_goose_params->next != nullptr ) )
			{
				ptr_goose_params = ptr_goose_params->next;
			}
			else
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Goose Control Block is not present or gcb_no is not valid" );
				loop_break = true;
				return_type = FALSE;
			}
		}
	}
	if ( ptr_goose_params != nullptr )
	{
		report_params = ( gcb_report_param_t* )( &ptr_goose_params->gcb_report_param );

		if ( ( loop_break == false ) && ( report_params == nullptr ) )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "report param of dataset %s identified failed!\n",
								  ptr_goose_params->datSetName );
			loop_break = true;
			return_type = FALSE;
		}

		if ( ( loop_break == false ) && ( !Encode_Dataset( report_params, buffer, sizeof( buffer ),
														   &member, &offset, &length ) ) )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Encode of dataset %s failed!\n", ptr_goose_params->datSetName );
			loop_break = true;
			return_type = FALSE;
		}
		// Start Goose_Publish
		if ( ( loop_break == false ) &&
			 ( !IEC_Goose_Publish( ptr_goose_params->ptr_goose_context, ptr_goose_params, &header, member,
								   &buffer[offset],
								   length ) != IEC_GOOSE_SUCCESS ) )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Goose Publish for %s failed\n", ptr_goose_params->goID );
			return_type = FALSE;
		}
	}

	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Open_Context( goose_context_t* ptr_goose_context_t )
{
	const char* name = "GOOSE adapter is not specified, ";

	ETH_adapter_dialog( name, ptr_goose_context_t->adapter_name );
	IEC_Goose_Initialize( ptr_goose_context_t );

	ptr_goose_context_t->getElementNumResFcn = nullptr;
	ptr_goose_context_t->getElementNumResParam = nullptr;
	ptr_goose_context_t->getReferenceResFcn = nullptr;
	ptr_goose_context_t->getReferenceResParam = nullptr;
	ptr_goose_context_t->notSupportedResFcn = nullptr;
	ptr_goose_context_t->notSupportedResParam = nullptr;
	ptr_goose_context_t->getElementNumReqFcn = nullptr;
	ptr_goose_context_t->getElementNumReqParam = nullptr;
	ptr_goose_context_t->getReferenceReqFcn = nullptr;
	ptr_goose_context_t->getReferenceReqParam = nullptr;

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Start( bool rx_task_init, notify_sub_indication_t callback )
{
	gcb_config_param_t* temp_gcb_param;

	if ( m_goose_state == GOOSE_STATE_INIT )
	{
		if ( callback != nullptr )
		{
			CIEC61850_GOOSE_Interface::Subscribtion_Callback = ( notify_sub_indication_t )callback;
		}

		if ( m_goose_context == nullptr )
		{
			Initialize();
		}
		temp_gcb_param = m_gcb_config_param_pub;///< start goose publish init

		while ( temp_gcb_param != nullptr )
		{
			Create( temp_gcb_param );
			temp_gcb_param = temp_gcb_param->next;
		}
		temp_gcb_param = m_gcb_config_param_sub;///< Start goose subscribe init

		while ( temp_gcb_param != nullptr )
		{
			Subscribe( temp_gcb_param );
			temp_gcb_param = temp_gcb_param->next;
		}

		if ( rx_task_init == true )
		{
			Init_Receive_Tasks();
		}
	}
	m_goose_state = GOOSE_STATE_RUN;

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
iec61850_error_t CIEC61850_GOOSE_Interface::Set_Config( user_gcb_config_param_t* user_gcb_config_param,
														message_type_t message_type )
{
	iec61850_error_t return_error = IEC61850_ERR_NO_ERROR;
	uint8_t index_data_obj = DISABLE_VALUE;
	uint8_t index_data_attr = DISABLE_VALUE;
	gcb_config_param_t* gcb_node = m_gcb_config_param_sub;
	data_type_t data_type;
	void* data_value;
	gcb_config_param_t* temp_node;
	uint8_t num_data_obj = DISABLE_VALUE;
	data_obj_t* data_obj = nullptr;
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
			gcb_node = m_gcb_config_param_pub;
		}

		while ( gcb_node != nullptr )
		{
			gcb_node = gcb_node->next;
		}

		if ( ( loop_break == false ) && ( gcb_node == nullptr ) )
		{
			gcb_node = reinterpret_cast<gcb_config_param_t*>( TMW_TARG_MALLOC( sizeof ( gcb_config_param_t ) ) );

			if ( ( loop_break == false ) && ( gcb_node == nullptr ) )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Unable to malloc gcb_node structure\n" );
				return_error = IEC61850_ERR_MEMORY_ALLOC;
				loop_break = true;
			}
		}

		// Copy Vlan info and Dataset info
		if ( gcb_node != nullptr )
		{
			memset( gcb_node, DISABLE_VALUE, sizeof( gcb_config_param_t ) );

			gcb_node->vlan_priority = user_gcb_config_param->vlan_priority;
			gcb_node->vlan_vID = user_gcb_config_param->vlan_vID;
			gcb_node->vlan_appID = user_gcb_config_param->vlan_appID;

			// copy multi cast addr
			gcb_node->multicast_addr = user_gcb_config_param->multicast_addr;

			// copy dataset config
			str_len = ( int ) strlen( user_gcb_config_param->goID ) + ENABLE_VALUE;
			if ( str_len > CB_IDENT_SIZE_GOOSE )
			{
				str_len = CB_IDENT_SIZE_GOOSE;
			}
			memcpy( gcb_node->goID, user_gcb_config_param->goID, str_len );

			str_len = ( int ) strlen( user_gcb_config_param->gocbRef ) + ENABLE_VALUE;
			if ( str_len > CB_IDENT_SIZE_GOOSE )
			{
				str_len = CB_IDENT_SIZE_GOOSE;
			}
			memcpy( gcb_node->gocbRef, user_gcb_config_param->gocbRef, str_len );

			str_len = ( int ) strlen( user_gcb_config_param->datSetName ) + ENABLE_VALUE;
			if ( str_len > CB_IDENT_SIZE_GOOSE )
			{
				str_len = CB_IDENT_SIZE_GOOSE;
			}
			memcpy( gcb_node->datSetName, user_gcb_config_param->datSetName, str_len );

			gcb_node->confRev = user_gcb_config_param->confRev;
			gcb_node->ndsComm = user_gcb_config_param->ndsComm;
			gcb_node->test = user_gcb_config_param->test;
			gcb_node->num_entries = user_gcb_config_param->num_entries;
			gcb_node->ptr_goose_context = m_goose_context;
			gcb_node->strategy = user_gcb_config_param->strategy;
			if ( gcb_node->strategy == nullptr )
			{
				gcb_node->strategy = &tst_strategy;
			}
			gcb_node->gcb_report_param.data_obj->next = nullptr;

			num_data_obj = user_gcb_config_param->user_report_param.num_data_obj;
			for ( index_data_obj = 0; index_data_obj < num_data_obj; index_data_obj++ )
			{
				data_obj = reinterpret_cast<data_obj_t*>( TMW_TARG_MALLOC( sizeof ( data_obj_t ) ) );
				if ( ( data_obj == nullptr ) || ( loop_break == true ) )
				{
					IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Unable to malloc data_obj structure" );
					return_error = IEC61850_ERR_MEMORY_ALLOC;
					loop_break = true;
				}
				else
				{
					data_obj->next = gcb_node->gcb_report_param.data_obj;
					gcb_node->gcb_report_param.data_obj = data_obj;
					num_dat_attr =
						user_gcb_config_param->user_report_param.data_obj[num_data_obj - ENABLE_VALUE -
																		  index_data_obj].num_data_attr;
					gcb_node->gcb_report_param.data_obj->data_attr = nullptr;
					gcb_node->gcb_report_param.data_obj->data_attr->next = nullptr;

					for ( index_data_attr = 0; index_data_attr < num_dat_attr; index_data_attr++ )
					{
						data_type =
							user_gcb_config_param->user_report_param.data_obj[num_data_obj - ENABLE_VALUE -
																			  index_data_obj].
							data_attr[num_dat_attr - ENABLE_VALUE - index_data_attr].data_type;
						data_value =
							user_gcb_config_param->user_report_param.data_obj[num_data_obj - ENABLE_VALUE -
																			  index_data_obj].
							data_attr[num_dat_attr - ENABLE_VALUE - index_data_attr].data_value;
						data_attr = Push_Node( &gcb_node, data_type, data_value );

						if ( ( loop_break == false ) && ( data_attr == nullptr ) )
						{
							IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Unable to malloc data_attr structure\n" );
							return_error = IEC61850_ERR_MEMORY_ALLOC;
							loop_break = true;
						}

						if ( ( loop_break == false ) && ( gcb_node->gcb_report_param.data_obj->data_attr == nullptr ) )
						{
							data_attr->next = nullptr;
							gcb_node->gcb_report_param.data_obj->data_attr = data_attr;
						}
						else
						{
							data_attr->next = gcb_node->gcb_report_param.data_obj->data_attr;
							gcb_node->gcb_report_param.data_obj->data_attr = data_attr;
						}
					}

					data_obj->data_attr = data_attr;
					data_obj->num_data_attr = num_dat_attr;
					gcb_node->gcb_report_param.data_obj = data_obj;
				}
			}
			if ( loop_break == false )
			{
				gcb_node->gcb_report_param.num_data_obj = num_data_obj;

				if ( message_type == IEC61850_MTYPE_PUBLISH )
				{
					m_num_gcb_config_param_pub++;
					if ( m_gcb_config_param_pub == nullptr )
					{
						m_gcb_config_param_pub = gcb_node;
						gcb_node->next = nullptr;
					}
					else
					{
						temp_node = m_gcb_config_param_pub;
						while ( temp_node->next != nullptr )
						{
							temp_node = temp_node->next;
						}
						temp_node->next = gcb_node;
						gcb_node->next = nullptr;
					}
				}
				else
				{
					m_num_gcb_config_param_sub++;
					if ( m_gcb_config_param_sub == nullptr )
					{
						m_gcb_config_param_sub = gcb_node;
						gcb_node->next = nullptr;
					}
					else
					{
						temp_node = m_gcb_config_param_sub;
						while ( temp_node->next != nullptr )
						{
							temp_node = temp_node->next;
						}
						temp_node->next = gcb_node;
						gcb_node->next = nullptr;
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
bool CIEC61850_GOOSE_Interface::Create( gcb_config_param_t* ptr_goose_params )
{
	unsigned char buffer[IEC_GOOSE_MAX_DATA_LEN];
	uint16_t member = DISABLE_VALUE;
	uint16_t offset = DISABLE_VALUE;
	uint16_t length = DISABLE_VALUE;
	gcb_report_param_t* report_params;
	bool return_val = TRUE;
	bool loop_break = false;

	report_params = ( gcb_report_param_t* )( &ptr_goose_params->gcb_report_param );

	/* Encode data for initial values
	   compare datset name for identying goose control data params */
	if ( report_params == nullptr )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "report param of dataset %s identified failed!\n",
							  ptr_goose_params->datSetName );
		return_val = FALSE;
		loop_break = true;
	}

	if ( ( loop_break == false ) && ( !Encode_Dataset( report_params, buffer, sizeof( buffer ),
													   &member, &offset, &length ) ) )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Encode of dataset %s failed!\n", ptr_goose_params->datSetName );
		return_val = FALSE;
	}

	// Start Goose_create
	if ( ( loop_break == false ) &&
		 ( IEC_Goose_CreateOut( ptr_goose_params->ptr_goose_context, ptr_goose_params->goID, ptr_goose_params->gocbRef,
								ptr_goose_params->datSetName, &ptr_goose_params->multicast_addr,
								ptr_goose_params->strategy,
								ptr_goose_params->confRev, ptr_goose_params->ndsComm, ptr_goose_params->test, member,
								FALSE, &buffer[offset],
								( MMSd_length ) length, ( void* ) ptr_goose_params, ptr_goose_params->vlan_appID,
								ptr_goose_params->vlan_vID,
								ptr_goose_params->vlan_priority ) != IEC_GOOSE_SUCCESS ) )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Create for %s failed\n", ptr_goose_params->goID );
		return_val = FALSE;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Subscribe( gcb_config_param_t* ptr_goose_params )
{
	goose_context_t* pIecGooseContext;

	pIecGooseContext = ptr_goose_params->ptr_goose_context;
	bool return_type = TRUE;

	if ( !IEC_Goose_Subscribe( pIecGooseContext, ptr_goose_params->goID,  ptr_goose_params->gocbRef,
							   &ptr_goose_params->multicast_addr, ptr_goose_params->vlan_appID,
							   ( void* ) ptr_goose_params, Iec_Goose_Indication,
							   ( void* ) ptr_goose_params ) )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_CRITICAL, "Subscription for %s failed\n", ptr_goose_params->goID );
		return_type = FALSE;
	}
	/* This call sets up the time to wait for an initial Goose
	   from the publisher before signalling a timeout*/

	IEC_Goose_SetInitialWait( pIecGooseContext, ( void* ) ptr_goose_params, m_initial_goose_wait_ms );
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Stop( void )
{
	m_goose_state = GOOSE_STATE_STOP;

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Delete( void )
{
	goose_context_t* cntxt;

	cntxt = m_goose_context;

	while ( cntxt->active_out != nullptr )
	{
		IEC_Goose_Delete( cntxt, ( void* ) cntxt->active_out->handle );
	}
	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Unsubscribe( void )
{
	goose_context_t* cntxt;

	cntxt = m_goose_context;

	while ( cntxt->active_in != nullptr )
	{
		IEC_Goose_Unsubscribe( cntxt, ( void* ) cntxt->active_in->handle );
	}
	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Close( void )
{
	IEC_Goose_Close( m_goose_context );

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Encode_Dataset( gcb_report_param_t* ptr_goose_param, uint8_t* ptr_buffer,
												uint16_t buffer_Size, uint16_t* ptr_count,
												uint16_t* ptr_offset, uint16_t* ptr_length )
{
	uint8_t index = DISABLE_VALUE;
	uint8_t num_data_obj = ptr_goose_param->num_data_obj;
	MMSd_descriptor_t descriptor;
	int16_t offset = DISABLE_VALUE;
	int16_t length = DISABLE_VALUE;

	MMSd_makeDescriptor( descriptor, ptr_buffer, ( MMSd_ushort ) buffer_Size,
						 ( MMSd_ushort ) ( buffer_Size - ENABLE_VALUE ) );

	for ( index = 0; index < num_data_obj; index++ )
	{
		Encode_Data_Attribute( ptr_goose_param->data_obj, num_data_obj - ENABLE_VALUE - index, &descriptor );
	}

	MMSd_takeDescriptor( descriptor, offset, length );
	*ptr_count = num_data_obj;
	*ptr_offset = offset;
	*ptr_length = length;

	return ( TRUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Encode_Data_Attribute( data_obj_t* ptr_data, uint8_t offset,
													   MMSd_descriptor_t* ptr_output )
{
	uint8_t num_data_attr = DISABLE_VALUE;
	uint8_t index = DISABLE_VALUE;
	int16_t length = DISABLE_VALUE;
	int16_t tot_length = DISABLE_VALUE;
	bool return_type = TRUE;

	while ( offset )
	{
		ptr_data = ptr_data->next;
		offset--;
	}
	num_data_attr = ptr_data->num_data_attr;

	for ( index = 0; index < num_data_attr; index++ )
	{
		length += Encode_Emit_Data( ptr_data->data_attr, num_data_attr - ENABLE_VALUE - index, ptr_output );
	}
	tot_length = MMSd_emitStructure( length, ptr_output );

	if ( tot_length > DISABLE_VALUE )
	{
		return_type = TRUE;
	}
	else
	{
		return_type = FALSE;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int16_t CIEC61850_GOOSE_Interface::Encode_Emit_Data( data_attr_t* ptr_data, uint8_t offset,
													 MMSd_descriptor_t* ptr_output )
{
	data_type_t data_type;
	int16_t length = DISABLE_VALUE;

	while ( offset )
	{
		ptr_data = ptr_data->next;
		offset--;
	}

	data_type = ptr_data->data_type;

	switch ( data_type )
	{
		case TAG_TYPE_BOOL:
			length = MMSd_emitUSER_BOOLEAN( *( reinterpret_cast<bool*>( ptr_data->data_value ) ), ptr_output );
			break;

		case TAG_TYPE_SINT16:
			length = MMSd_emitUSER_INTEGER( *( reinterpret_cast<int16_t*>( ptr_data->data_value ) ), ptr_output );
			break;

		case TAG_TYPE_UINT16:
			length =
				MMSd_emitUSER_UNSIGNEDINTEGER( *( reinterpret_cast<uint16_t*>( ptr_data->data_value ) ), ptr_output );
			break;

		case TAG_TYPE_SINT32:
			length = MMSd_emitUSER_INTEGER( *( reinterpret_cast<int32_t*>( ptr_data->data_value ) ), ptr_output );
			break;

		case TAG_TYPE_UINT32:
			length =
				MMSd_emitUSER_UNSIGNEDINTEGER( *( reinterpret_cast<uint32_t*>( ptr_data->data_value ) ), ptr_output );
			break;

		case TAG_TYPE_T:
			length = MMSd_emitUSER_UTCTIME( reinterpret_cast<MMSd_UtcBTime*>( ptr_data->data_value ), 8, ptr_output );
			break;

		case TAG_TYPE_Q:
			length = MMSd_emitUSER_BITSTRING( reinterpret_cast<quality_t*>( ptr_data->data_value ), 13, ptr_output );
			break;

		case TAG_TYPE_FLOAT:
			length = MMSd_emitUSER_FLOAT( *( reinterpret_cast<float*>( ptr_data->data_value ) ), ptr_output );
			length = MMSd_emitStructure( length, ptr_output );
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
void CIEC61850_GOOSE_Interface::Service( void )
{
	if ( m_goose_state == GOOSE_STATE_RUN )
	{
		IEC_Goose_Service( m_goose_context );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_GOOSE_Interface::Transmit( void )
{
	if ( m_goose_state == GOOSE_STATE_RUN )
	{
		if ( m_goose_context->link_context != nullptr )
		{
			IEC_Goose_Service_Short( m_goose_context );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Receive( void )
{
	int ret_val = 0;

	if ( m_goose_state == GOOSE_STATE_RUN )
	{
		if ( m_goose_context->link_context != nullptr )
		{
			ret_val = TMW_DL_service( m_goose_context->link_context );
		}
	}
	else
	{
		OS_Tasker::Delay( IEC61850_RX_SLEEP_TIME_MS );
	}

	if ( ret_val != 0 )
	{
		return ( true );
	}
	else
	{
		return ( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_GOOSE_Interface::Process( void )
{
	TMW_Packet packet;
	TMW_Mac_Address mac;

	if ( m_goose_state == GOOSE_STATE_RUN )
	{
		if ( m_goose_context->link_context != nullptr )
		{
			TMW_DL_receive( &packet, &mac, m_goose_context->link_context );
		}
	}
	else
	{
		OS_Tasker::Delay( IEC61850_RX_SLEEP_TIME_MS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
CIEC61850_GOOSE_Interface::data_attr_t* CIEC61850_GOOSE_Interface::Push_Node( gcb_config_param_t** gcb_node1,
																			  data_type_t data_type, void* data_value )
{
	data_attr_t* ref_node = nullptr;
	gcb_config_param_t* temp_node = *gcb_node1;
	data_attr_t* new_node = reinterpret_cast<data_attr_t*>( TMW_TARG_MALLOC( sizeof( data_attr_t ) ) );

	if ( new_node == nullptr )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "MALLOC creation faliure in Push_Node function" );
	}
	else
	{
		ref_node = temp_node->gcb_report_param.data_obj->data_attr;

		new_node->data_type = data_type;
		new_node->data_value = data_value;
		new_node->next = ( ref_node );

		ref_node = reinterpret_cast<data_attr_t*>( new_node );
	}

	return ( ref_node );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_GOOSE_Interface::Iec_Goose_Indication( void* ptr_handle, void* ptr_param, IEC_GooseHeader* ptr_header,
													  unsigned char* ptr_data, MMSd_length data_length, int timed_out )
{
	bool err_type;
	gcb_config_param_t* ptr_goose_params = ( gcb_config_param_t* ) ptr_param;
	MMSd_descriptor_t descriptor;
	bool loop_break = false;

	IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, " Iec_Goose_Indication Called: %s ", ptr_goose_params->datSetName );
	memset( &descriptor, DISABLE_VALUE, sizeof( MMSd_descriptor_t ) );

	while ( ptr_goose_params != nullptr )
	{
		if ( DISABLE_VALUE == strcmp( ptr_goose_params->gocbRef, ptr_header->gocbRef ) )
		{
			break;
		}
		else
		{
			ptr_goose_params = ptr_goose_params->next;
		}
	}
	if ( ptr_goose_params == nullptr )
	{
		IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Goose input data is invalid: %s", ptr_header->gocbRef );
		loop_break = true;
	}

	if ( loop_break == false )
	{
		if ( timed_out )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Goose input %s timed out\n", ptr_goose_params->datSetName );

			strcpy( m_sub_goose_cb_ref, ptr_header->gocbRef );
			m_sub_timeout = true;
			m_subscribe_notify = false;
		}
		else
		{
			MMSd_makeDescriptor( descriptor, ptr_data, ( unsigned short ) data_length, DISABLE_VALUE );
			descriptor.tokenLength = data_length;

			// Find the Dataset name and call decode function to decode goose packet
			err_type = Decode_Dataset( &descriptor, ( gcb_report_param_t* )&ptr_goose_params->gcb_report_param );

			if ( err_type == FALSE )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Decode message Failed" );
			}

			m_subscribe_notify = true;
			m_sub_timeout = false;
			strcpy( m_sub_goose_cb_ref, ptr_header->gocbRef );
			if ( Subscribtion_Callback != nullptr )
			{
				Subscribtion_Callback( m_sub_goose_cb_ref );
			}

			m_state_num = ptr_header->StNum;

			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "State Number: %d \n", m_state_num );

			CIEC61850_GOOSE_DCI_Manager::m_sub_ind_flag = true;
			strcpy( CIEC61850_GOOSE_DCI_Manager::m_cb_ref, ptr_header->gocbRef );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Decode_Dataset( MMSd_descriptor_t* const ptr_input,
												gcb_report_param_t const* const ptr_goose_params )
{
	bool return_type = TRUE;
	uint8_t index, count = DISABLE_VALUE;
	uint8_t num_data_obj = ptr_goose_params->num_data_obj;
	MMSd_descriptor_t save = *ptr_input;

	for ( index = 0; index < num_data_obj; index++ )
	{
		if ( !MMSd_fetchCOMPLETE( &save, ptr_input ) )
		{
			MMSd_nextToken( ptr_input );
			MMSd_beginHandlerStruct( ptr_input );

			return_type = Decode_Data_Attribute( ptr_input, ptr_goose_params->data_obj, index );

			MMSd_endHandlerStruct( ptr_input );
			count++;

			if ( return_type == FALSE )
			{
				break;
			}
		}
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Decode_Data_Attribute( MMSd_descriptor_t* const ptr_input,
													   data_obj_t const* const data_obj, uint8_t offset )
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
		if ( loop_index != DISABLE_VALUE )
		{
			MMSd_nextToken( ptr_input );
		}
		return_type = Decode_Fetch_Data( ptr_input, data_attr );
		data_attr = data_attr->next;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool CIEC61850_GOOSE_Interface::Decode_Fetch_Data( MMSd_descriptor_t* const ptr_input,
												   data_attr_t* const ptr_data )
{
	bool return_type = TRUE;
	data_type_t data_type;
	int data = 0;

	data_type = ptr_data->data_type;
	switch ( data_type )
	{
		case TAG_TYPE_BOOL:
			if ( !MMSd_fetchUSER_BOOLEAN( ptr_input, reinterpret_cast<int*>( &data ) ) )
			{
				return_type = FALSE;
			}
			*( bool* )ptr_data->data_value = ( bool )( data );
			break;

		case TAG_TYPE_SINT16:
		case TAG_TYPE_SINT32:
			if ( !MMSd_fetchUSER_INTEGER( ptr_input, reinterpret_cast<int*>( ptr_data->data_value ) ) )
			{
				return_type = FALSE;
			}
			break;

		case TAG_TYPE_UINT16:
		case TAG_TYPE_UINT32:
			if ( !MMSd_fetchUSER_UNSIGNEDINTEGER( ptr_input, reinterpret_cast<unsigned int*>( ptr_data->data_value ) ) )
			{
				return ( DISABLE_VALUE );
			}
			break;

		case TAG_TYPE_T:
			if ( !MMSd_fetchUSER_UTCTIME( ptr_input, reinterpret_cast<MMSd_UtcBTime*>( ptr_data->data_value ) ) )
			{
				return_type = FALSE;
			}
			break;

		case TAG_TYPE_Q:
			if ( !MMSd_fetchUSER_BITSTRING( ptr_input, reinterpret_cast<unsigned char*>( ptr_data->data_value ), 13 ) )
			{
				return_type = FALSE;
			}
			break;

		case TAG_TYPE_FLOAT:
			MMSd_beginHandlerStruct( ptr_input );
			if ( !MMSd_fetchUSER_FLOAT( ptr_input, reinterpret_cast<float*>( ptr_data->data_value ) ) )
			{
				return_type = FALSE;
			}
			else
			{
				MMSd_endHandlerStruct( ptr_input );
			}
			break;

		default:
			IEC61850_DEBUG_PRINT( DBG_MSG_ERROR, "DataType is not supported to fetch value " );
			break;
	}
	return ( return_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_GOOSE_Interface::Init_Receive_Tasks( void )
{
	if ( m_proess_wake_mutex == nullptr )
	{
		m_proess_wake_mutex = new OS_Mutex();
		if ( m_proess_wake_mutex == nullptr )
		{
			IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "m_proess_wake_mutex not created \n" );
		}
		else
		{
			if ( sys_thread_new( ( char* )IEC61850_RECEIVE_THREAD_NAME, &Receive_OS_Task_Static,
								 reinterpret_cast<void*>( this ),
								 IEC61850_RECEIVE_THREAD_STACKSIZE, IEC61850_RECEIVE_THREAD_PRIO ) == nullptr )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Receive_OS_Task_Static not created \n" );
			}

			if ( sys_thread_new( ( char* )IEC61850_PROCESS_THREAD_NAME, &Process_OS_Task_Static,
								 reinterpret_cast<void*>( this ),
								 IEC61850_PROCESS_THREAD_STACKSIZE, IEC61850_PROCESS_THREAD_PRIO ) == nullptr )
			{
				IEC61850_DEBUG_PRINT( DBG_MSG_DEBUG, "Process_OS_Task_Static not created \n" );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_GOOSE_Interface::Receive_OS_Task( void )
{
	bool ret_val = false;

	while ( true )
	{
		ret_val = Receive();

		if ( m_proess_wake_mutex != nullptr )
		{
			if ( ret_val == true )
			{
				m_proess_wake_mutex->Release();
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIEC61850_GOOSE_Interface::Process_OS_Task( void )
{
	while ( true )
	{
		if ( m_proess_wake_mutex != nullptr )
		{
			m_proess_wake_mutex->Acquire( OS_MUTEX_INDEFINITE_TIMEOUT );
			Process();
		}
	}
}
