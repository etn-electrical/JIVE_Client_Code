/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_Intercom.h"
#include "Modbus_DCI.h"
#include "System_Reset.h"
#include "Faults.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MODBUS_INTERCOM_MONITOR_REG_SIZE    2
const uint16_t MODBUS_INTERCOM_MONITOR_DEQ = 0xFFFF;
const uint16_t MODBUS_INTERCOM_MON_NO_DATA_CHANGE = 0xFFFF;

// This is the time to wait in milliseconds between connection attempts.
#define CONNECT_INTER_MESSAGE_TIME              10

#define DEVICE_NOT_CONNECTED_WARNING_TIME       3000

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
Modbus_Intercom::Modbus_Intercom( uint8_t slave_address, Modbus_Master* modbus_master,
								  const MODBUS_INTERCOM_TARGET_INFO_ST* modbus_target_def,
								  Device_Identity* dev_identity_handler )
{
	m_mbus_atomiblock_list = modbus_target_def->mbus_atomiblock;
	m_intercom_atomiblock = modbus_target_def->intercom_atomiblock;
	m_monitor_reg = modbus_target_def->change_que_reg;
	m_inter_message_delay = modbus_target_def->inter_message_delay;

	m_mb_master = modbus_master;
	m_slave_address = slave_address;

	m_dev_identity_handler = dev_identity_handler;
	m_modbus_dci = new Modbus_DCI( modbus_target_def->modbus_target_def, DCI_SOURCE_IDS_Get() );
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
bool Modbus_Intercom::Connect( BF_Lib::Timers::TIMERS_TIME_TD timeout_ms )
{
	bool return_success = true;
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t prod_search_index = 0U;
	uint16_t rx_prod_code = 0U;
	BF_Lib::Timers::TIMERS_TIME_TD timeout_tmr;
	DEV_ID_DEF_PROD_SEARCH_ST const* prod_search_struct;

	if ( m_dev_identity_handler != NULL )
	{
		prod_search_struct = m_dev_identity_handler->Get_Product_Search_Info( prod_search_index );

		// If the pointer is NULL then we have no products to search for.
		if ( prod_search_struct != NULL )
		{
			/// By using the -1 we are able to have it start out with 0 on the first run through.
			/// See the while loop and it should be clear.  I want to exit the while with the index intact.
			prod_search_index = -1;
			timeout_tmr = Timers::Get_Time();

			do
			{
				OS_Tasker::Delay( CONNECT_INTER_MESSAGE_TIME );

				prod_search_index++;
				prod_search_struct = m_dev_identity_handler->Get_Product_Search_Info( prod_search_index );

				if ( prod_search_index == 0 )
				{
					m_mb_master->Enable_Port( MODBUS_RTU_TX_MODE, MODBUS_PARITY_EVEN,
											  prod_search_struct->married_baud_rate,
											  true, MODBUS_1_STOP_BIT );
				}
				else
				{
					m_mb_master->Enable_Port( MODBUS_RTU_TX_MODE, MODBUS_PARITY_EVEN,
											  prod_search_struct->baud_rate,
											  true, MODBUS_1_STOP_BIT );
				}
				return_status = m_mb_master->Get_Reg(
					prod_search_struct->address,
					prod_search_struct->prod_code_reg,
					sizeof( rx_prod_code ), ( uint8_t* )&rx_prod_code, INTERCOM_ATTRIB_RAM_VAL );
				if ( return_status == MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE )
				{
					OS_Tasker::Delay( CONNECT_INTER_MESSAGE_TIME );

					m_mb_master->Enable_Port( MODBUS_RTU_TX_MODE, MODBUS_PARITY_EVEN,
											  prod_search_struct->married_baud_rate,
											  true, MODBUS_1_STOP_BIT );
					return_status = m_mb_master->Get_Reg(
						prod_search_struct->address,
						prod_search_struct->prod_code_reg,
						sizeof( rx_prod_code ), ( uint8_t* )&rx_prod_code, INTERCOM_ATTRIB_RAM_VAL );
				}

				// The unit has not powered up or has not responded in a reasonable amount of time.  We should indicate
				// such.
				if ( !Faults::Test( FAULT_INTERCOM_COMMUNICATION_MSG_ERROR_FLT ) &&
					 Timers::Expired( timeout_tmr, DEVICE_NOT_CONNECTED_WARNING_TIME ) &&
					 ( Device_Identity::Product_DCI_Married_To() != DEVICE_IDENT_PRODUCT_DCI_MARRIAGE_STANDALONE_IO ) )
				{
					Faults::Set( FAULT_INTERCOM_COMMUNICATION_MSG_ERROR_FLT );
				}
			} while ( ( return_status != MB_NO_ERROR_CODE ) ||
					  ( ( rx_prod_code > prod_search_struct->target_prod_code_max ) ||
						( rx_prod_code < prod_search_struct->target_prod_code_min ) ) &&
					  ( ( timeout_ms == 0 ) || !Timers::Expired( timeout_tmr, timeout_ms ) ) );

			if ( ( rx_prod_code <= prod_search_struct->target_prod_code_max ) &&
				 ( rx_prod_code >= prod_search_struct->target_prod_code_min ) )
			{
				// We are not attached to the original device.
				if ( prod_search_index != 0 )
				{
					if ( prod_search_struct->baud_rate != prod_search_struct->married_baud_rate )
					{
						return_status = m_mb_master->Set_Reg(
							prod_search_struct->address,
							prod_search_struct->set_baud_rate_reg,
							sizeof( prod_search_struct->set_baud_rate ),
							( uint8_t* )&prod_search_struct->set_baud_rate,
							INTERCOM_ATTRIB_RAM_AND_INIT_VAL );
					}
					m_dev_identity_handler->Set_New_Init_Prod_Search_Index( prod_search_index );

					BF::System_Reset::Reset_Device( BF::System_Reset::APP_PARAM_RESET );
					return_success = false;
				}
				else
				{
					Faults::Clr( FAULT_INTERCOM_COMMUNICATION_MSG_ERROR_FLT );		// We are attached to the original
																					// unit and things are good.
				}
			}
			else
			{
				Faults::Set( FAULT_INTERCOM_COMMUNICATION_MSG_ERROR_FLT );
				return_success = false;
			}
		}
	}

	return ( return_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Intercom::Disconnect( void )
{
	m_dev_identity_handler->Set_New_Init_Prod_Search_Index(
		m_dev_identity_handler->DISCONNECTED_PRODUCT_SEARCH_INDEX );

	BF::System_Reset::Reset_Device( BF::System_Reset::APP_PARAM_RESET );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::Get_Data( DCI_ID_TD dcid, uint8_t* data, INTERCOM_ATTRIB_ENUM attribute )
{
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* get_struct;
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t error_count;
	uint8_t mbus_attribute;

	get_struct = m_modbus_dci->Find_MBus_Reg( dcid );

	if ( get_struct != 0 )
	{
		mbus_attribute = Get_Modbus_Attribute( attribute );

		error_count = 0U;
		do
		{
			OS_Tasker::Delay( m_inter_message_delay );

			return_status = m_mb_master->Get_Reg( m_slave_address,
												  get_struct->modbus_reg, get_struct->length,
												  data, mbus_attribute );
			error_count++;
		} while ( ( error_count <= MODBUS_INTERCOM_MAX_MSG_RETRIES ) &&
				  ( return_status != MB_NO_ERROR_CODE ) );
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
	}

	return ( Interpret_Modbus_Error( return_status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::Set_Data( DCI_ID_TD dcid, uint8_t* data, INTERCOM_ATTRIB_ENUM attribute )
{
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* set_struct;
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t error_count;
	uint8_t mbus_attribute;

	set_struct = m_modbus_dci->Find_MBus_Reg( dcid );

	if ( set_struct != 0 )
	{
		mbus_attribute = Get_Modbus_Attribute( attribute );

		error_count = 0U;
		do
		{
			OS_Tasker::Delay( m_inter_message_delay );

			return_status = m_mb_master->Set_Reg( m_slave_address,
												  set_struct->modbus_reg, set_struct->length,
												  data, mbus_attribute );
			error_count++;
		} while ( ( error_count <= MODBUS_INTERCOM_MAX_MSG_RETRIES ) &&
				  ( return_status != MB_NO_ERROR_CODE ) );
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
	}

	return ( Interpret_Modbus_Error( return_status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::GetSet_Data( DCI_ID_TD get_dcid, uint8_t* get_data,
												  DCI_ID_TD set_dcid, uint8_t* set_data,
												  INTERCOM_ATTRIB_ENUM get_attribute,
												  INTERCOM_ATTRIB_ENUM set_attribute )
{
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* get_struct;
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* set_struct;
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t error_count;
	uint8_t mbus_get_attribute;
	uint8_t mbus_set_attribute;

	get_struct = m_modbus_dci->Find_MBus_Reg( get_dcid );
	set_struct = m_modbus_dci->Find_MBus_Reg( set_dcid );

	if ( ( get_struct != 0 ) && ( set_struct != 0 ) )
	{
		mbus_get_attribute = Get_Modbus_Attribute( get_attribute );
		mbus_set_attribute = Get_Modbus_Attribute( set_attribute );

		error_count = 0U;
		do
		{
			OS_Tasker::Delay( m_inter_message_delay );

			return_status = m_mb_master->GetSet_Reg( m_slave_address,
													 get_struct->modbus_reg, get_struct->length, get_data,
													 set_struct->modbus_reg, set_struct->length, set_data,
													 mbus_get_attribute, mbus_set_attribute );
			error_count++;
		} while ( ( error_count <= MODBUS_INTERCOM_MAX_MSG_RETRIES ) &&
				  ( return_status != MB_NO_ERROR_CODE ) );
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
	}

	return ( Interpret_Modbus_Error( return_status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::System_Cmd( INTRCM_SYSCMD_ENUM command, uint8_t* data )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	uint8_t tx_data;
	uint8_t error_count;

	error_count = 0U;
	do
	{
		OS_Tasker::Delay( m_inter_message_delay );

		switch ( command )
		{
			case INTERCOM_SOFT_RESET_SYSCMD:
				tx_data = MB_SYS_CMD_SOFT_RESET;
				return_status = m_mb_master->User_Func_Code( m_slave_address,
															 MB_SYS_CMD_USER_FUNC_CODE, &tx_data,
															 MB_SYS_CMD_USER_FUNC_CODE_LENGTH );
				break;

			case INTERCOM_FACTORY_RESET_SYSCMD:
				tx_data = MB_SYS_CMD_FACTORY_RESET;
				return_status = m_mb_master->User_Func_Code( m_slave_address,
															 MB_SYS_CMD_USER_FUNC_CODE, &tx_data,
															 MB_SYS_CMD_USER_FUNC_CODE_LENGTH );
				break;

			case INTERCOM_APP_PARAM_RESET_SYSCMD:
				tx_data = MB_SYS_CMD_APP_PARAM_RESET;
				return_status = m_mb_master->User_Func_Code( m_slave_address,
															 MB_SYS_CMD_USER_FUNC_CODE, &tx_data,
															 MB_SYS_CMD_USER_FUNC_CODE_LENGTH );
				break;

			case INTERCOM_COMM_LOSS_TRIGGER_SYSCMD:
				tx_data = MB_SYS_CMD_TRIG_COMM_LOSS_ACTION;
				return_status = m_mb_master->User_Func_Code( m_slave_address,
															 MB_SYS_CMD_USER_FUNC_CODE, &tx_data,
															 MB_SYS_CMD_USER_FUNC_CODE_LENGTH );
				break;

			default:
				break;
		}
		error_count++;
	} while ( ( error_count <= MODBUS_INTERCOM_MAX_MSG_RETRIES ) &&
			  ( return_status != MB_NO_ERROR_CODE ) );

	return ( Interpret_Modbus_Error( return_status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::Set_DCID_Monitor( DCI_ID_TD dcid_to_monitor )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* mon_struct;
	uint8_t error_count;

	mon_struct = m_modbus_dci->Find_MBus_Reg( dcid_to_monitor );

	if ( mon_struct != 0 )
	{
		error_count = 0U;
		do
		{
			OS_Tasker::Delay( m_inter_message_delay );

			return_status = m_mb_master->Set_Reg( m_slave_address, m_monitor_reg,
												  MODBUS_INTERCOM_MONITOR_REG_SIZE,
												  ( uint8_t* )&mon_struct->modbus_reg );
			error_count++;
		} while ( ( error_count <= MODBUS_INTERCOM_MAX_MSG_RETRIES ) &&
				  ( return_status != MB_NO_ERROR_CODE ) );
	}
	else
	{
		return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
	}

	return ( Interpret_Modbus_Error( return_status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::Get_Changed_DCID( DCI_ID_TD* changed_dcid )
{
	uint8_t return_status = MB_NO_ERROR_CODE;
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* mon_struct;
	uint16_t changed_reg;
	uint8_t error_count;

	error_count = 0U;
	do
	{
		OS_Tasker::Delay( m_inter_message_delay );

		return_status = m_mb_master->Get_Reg( m_slave_address, m_monitor_reg,
											  MODBUS_INTERCOM_MONITOR_REG_SIZE, ( uint8_t* )&changed_reg );
		error_count++;
	} while ( ( error_count <= MODBUS_INTERCOM_MAX_MSG_RETRIES ) &&
			  ( return_status != MB_NO_ERROR_CODE ) );

	if ( return_status == MB_NO_ERROR_CODE )
	{
		if ( changed_reg != MODBUS_INTERCOM_MON_NO_DATA_CHANGE )
		{
			mon_struct = m_modbus_dci->Find_MBus_DCID( changed_reg );

			if ( mon_struct != NULL )
			{
				*changed_dcid = mon_struct->dcid;
				if ( return_status == MB_NO_ERROR_CODE )
				{
					error_count = 0U;
					do
					{
						OS_Tasker::Delay( m_inter_message_delay );

						return_status = m_mb_master->Set_Reg( m_slave_address, m_monitor_reg,
															  MODBUS_INTERCOM_MONITOR_REG_SIZE,
															  ( uint8_t* )&MODBUS_INTERCOM_MONITOR_DEQ );
						error_count++;
					} while ( ( error_count <= MODBUS_INTERCOM_MAX_MSG_RETRIES ) &&
							  ( return_status != MB_NO_ERROR_CODE ) );
				}
			}
			else
			{
				return_status = MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE;
			}
		}
	}

	if ( ( return_status != MB_NO_ERROR_CODE ) ||
		 ( changed_reg == MODBUS_INTERCOM_MON_NO_DATA_CHANGE ) )
	{
		*changed_dcid = INTERCOM_NO_CHANGED_ID;
	}

	return ( Interpret_Modbus_Error( return_status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Intercom::Get_Atomiblock_Spec( const INTERCOM_ATOMIBLOCK_SPEC** atomiblock_spec )
{
	*atomiblock_spec = m_intercom_atomiblock;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::Get_Atomiblock( uint8_t block_counter, uint8_t* dest_data )
{
	uint8_t* dest_ptr;
	uint8_t* src_ptr;
	uint16_t dci_id_index;
	DCI_ID_TD* dci_id_list;
	uint16_t dci_id_counter;
	uint16_t total_items;
	DCI_LENGTH_TD dci_id_length;
	uint8_t return_status = MB_NO_ERROR_CODE;

	if ( block_counter < m_intercom_atomiblock->total_blocks )
	{
		OS_Tasker::Delay( m_inter_message_delay );

		return_status = m_mb_master->Get_Reg( m_slave_address, m_mbus_atomiblock_list[block_counter].start_reg,
											  ( m_mbus_atomiblock_list[block_counter].num_registers * 2 ), dest_data,
											  MB_ATTRIB_RAM_ACCESS );

		if ( return_status == MB_NO_ERROR_CODE )
		{
			dci_id_list = m_intercom_atomiblock->atomiblock_list[block_counter].dcid_list;

			dest_ptr = dest_data;
			src_ptr = dest_data;
			// Let the packing begin.
			total_items = m_intercom_atomiblock->atomiblock_list[block_counter].total_items_in_block;
			for ( dci_id_counter = 0U; dci_id_counter < total_items; dci_id_counter++ )
			{
				dci_id_length = m_modbus_dci->Get_Length( dci_id_list[dci_id_counter] );
				for ( dci_id_index = 0U; dci_id_index < dci_id_length; dci_id_index++ )
				{
					*dest_ptr = *src_ptr;
					dest_ptr++;
					src_ptr++;
				}
				if ( Is_Number_Odd( dci_id_length ) )
				{
					src_ptr++;
				}
			}
		}
	}
	else
	{
		return_status = MB_INVALID_INTERCOM_BLOCK_ERROR_CODE;
	}

	return ( Interpret_Modbus_Error( return_status ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
INTERCOM_ERROR_ENUM Modbus_Intercom::Interpret_Modbus_Error( uint8_t modbus_error )
{
	INTERCOM_ERROR_ENUM intercom_response;

	switch ( modbus_error )
	{
		case MB_NO_ERROR_CODE:
			intercom_response = INTERCOM_NO_ERROR;
			break;

		case MB_ILLEGAL_FUNCTION_ERROR_CODE:
		case MB_ILLEGAL_DATA_VALUE_ERROR_CODE:
		case MB_SLAVE_DEVICE_FAILURE_ERROR_CODE:
			intercom_response = INTERCOM_ACCESS_ERROR;
			break;

		case MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE:
			intercom_response = INTERCOM_DATA_DOES_NOT_EXIST;
			break;

		case MB_SLAVE_DEVICE_BUSY_ERROR_CODE:
			intercom_response = INTERCOM_SLAVE_BUSY_ERROR;
			break;

		case MB_INVALID_INTERCOM_BLOCK_ERROR_CODE:
			intercom_response = INTERCOM_INTERNAL_ERROR;
			break;

		case MB_MASTER_NO_SLAVE_RESPONSE_ERROR_CODE:
		default:
			intercom_response = INTERCOM_NO_RESPONSE_ERROR;
			break;
	}

	return ( intercom_response );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Modbus_Intercom::Get_Modbus_Attribute( INTERCOM_ATTRIB_ENUM intercom_attrib )
{
	uint8_t modbus_attrib;

	switch ( intercom_attrib )
	{
		case INTERCOM_ATTRIB_RAM_VAL:
			// modbus_attrib = MB_ATTRIB_NORMAL_ACCESS;	//This was used for when the DSP couldn't do attributes.
			modbus_attrib = MB_ATTRIB_RAM_ACCESS;
			break;

		default:
		case INTERCOM_ATTRIB_RAM_AND_INIT_VAL:
			modbus_attrib = MB_ATTRIB_NORMAL_ACCESS;
			break;

		case INTERCOM_ATTRIB_INIT_VAL:
			modbus_attrib = MB_ATTRIB_INIT_ACCESS;
			break;

		case INTERCOM_ATTRIB_DEFAULT_VAL:
			modbus_attrib = MB_ATTRIB_DEFAULT_ACCESS;
			break;

		case INTERCOM_ATTRIB_MIN_VAL:
			modbus_attrib = MB_ATTRIB_RANGE_MIN_ACCESS;
			break;

		case INTERCOM_ATTRIB_MAX_VAL:
			modbus_attrib = MB_ATTRIB_RANGE_MAX_ACCESS;
			break;

		case INTERCOM_ATTRIB_LENGTH_VAL:
			modbus_attrib = MB_ATTRIB_LENGTH_ACCESS;
			break;
	}

	return ( modbus_attrib );
}
