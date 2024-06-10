/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_User_Block.h"
#include "Modbus_Net.h"


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
const DCI_CB_MSK_TD Modbus_User_Block::CBACK_MASK_DEFINITION =
	( DCI_ACCESS_GET_RAM_CMD_MSK | DCI_ACCESS_SET_RAM_CMD_MSK );

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint8_t Modbus_User_Block::m_dummy_owner_data = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_User_Block::Modbus_User_Block( DCI_ID_TD list_dcid, DCI_ID_TD data_block_dcid,
									  Modbus_DCI* modbus_dci_ctrl, bool writable ) :
	m_list_owner( new DCI_Owner( list_dcid ) ),
	m_data_owner( new DCI_Owner( data_block_dcid, &m_dummy_owner_data ) ),
	m_writable( writable ),
	m_modbus_dci_ctrl( modbus_dci_ctrl )
{
	m_data_owner->Attach_Callback( &Data_Callback_Static,
								   reinterpret_cast<DCI_CBACK_PARAM_TD>( this ), CBACK_MASK_DEFINITION );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_User_Block::~Modbus_User_Block( void )
{
	delete m_list_owner;
	delete m_data_owner;
	delete m_modbus_dci_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Modbus_User_Block::Data_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
	DCI_ERROR_TD dci_error;
	uint8_t* data_ptr;
	uint16_t num_registers;	// uint_fast16_t num_registers; // to check
	uint16_t working_reg;
	uint16_t list_index;// uint_fast16_t list_index; // to check
	uint8_t temp_byte;

	list_index = access_struct->data_access.offset >> DIV_BY_2;
	num_registers = access_struct->data_access.length >> DIV_BY_2;
	data_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );
	switch ( access_struct->command )
	{
		case DCI_ACCESS_GET_RAM_CMD:
			while ( num_registers > 0U )
			{
				m_list_owner->Check_Out_Index( &working_reg,
											   static_cast<DCI_LENGTH_TD>( list_index ) );
				if ( working_reg == EMPTY_LIST_REGISTER )
				{
					dci_error = DCI_ERR_INVALID_DATA_ID;
				}
				else
				{
					dci_error = m_modbus_dci_ctrl->Get_Reg_Data( ( working_reg - 1U ), 1U,
																 data_ptr, MB_ATTRIB_RAM_ACCESS );
				}
				if ( dci_error != DCI_ERR_NO_ERROR )
				{
					data_ptr[0] = 0U;
					data_ptr[1] = 0U;
				}
				else
				{
					// Do some swapping here since it will get reswapped once it leaves here.
					temp_byte = data_ptr[0];
					data_ptr[0] = data_ptr[1];
					data_ptr[1] = temp_byte;
				}
				data_ptr += 2U;
				list_index++;
				num_registers--;
			}
			break;

		case DCI_ACCESS_SET_RAM_CMD:
			if ( m_writable == true )
			{
				while ( num_registers > 0U )
				{
					m_list_owner->Check_Out_Index( &working_reg,
												   static_cast<DCI_LENGTH_TD>( list_index ) );

					if ( working_reg != EMPTY_LIST_REGISTER )
					{
						temp_byte = data_ptr[0];// Do some swapping here since it will get reswapped once it leaves
												// here.
						data_ptr[0] = data_ptr[1];
						data_ptr[1] = temp_byte;

						/// We don't really care about the error. Caveat emptor.
						/// We can set both the RAM and NV using normal access.
						/// We can set just the RAM by changing the MB_ATTRIB_RAM_ACCESS.
						m_modbus_dci_ctrl->Set_Reg_Data( ( working_reg - 1U ), 1U,
														 data_ptr, MB_ATTRIB_NORMAL_ACCESS );
					}
					data_ptr += 2U;
					list_index++;
					num_registers--;
				}
			}
			else
			{
				return_status = DCI_CBack_Encode_Error( DCI_ERR_RAM_READ_ONLY );
			}
			break;

		/* Following 'case' labels are added to get rid of PC-lint issue 'Info 788' while MISRAfication*/
		case DCI_ACCESS_GET_INIT_CMD:
		case DCI_ACCESS_SET_INIT_CMD:
		case DCI_ACCESS_GET_DEFAULT_CMD:
		case DCI_ACCESS_GET_LENGTH_CMD:
		case DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD:
		case DCI_ACCESS_GET_MIN_CMD:
		case DCI_ACCESS_GET_MAX_CMD:
		case DCI_ACCESS_GET_ENUM_LIST_LEN_CMD:
		case DCI_ACCESS_GET_ENUM_CMD:
		case DCI_ACCESS_RAM_TO_INIT_CMD:
		case DCI_ACCESS_INIT_TO_RAM_CMD:
		case DCI_ACCESS_DEFAULT_TO_INIT_CMD:
		case DCI_ACCESS_DEFAULT_TO_RAM_CMD:
		case DCI_ACCESS_POST_SET_RAM_CMD:
		case DCI_ACCESS_POST_SET_INIT_CMD:
		case DCI_ACCESS_UNDEFINED_COMMAND:
		default:

			break;
	}

	return ( return_status );
}
