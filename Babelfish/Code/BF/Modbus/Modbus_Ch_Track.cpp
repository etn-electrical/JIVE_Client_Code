/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Modbus_Ch_Track.h"
#include "Modbus_DCI.h"

namespace BF_Mbus
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Modbus_Ch_Track::Modbus_Ch_Track( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
								  DCI_Owner* change_que_owner ) :
	m_change_tracker( reinterpret_cast<Change_Tracker*>( nullptr ) ),
	m_current_dcid( 0U ),
	m_change_shadow( false ),
	m_change_owner( reinterpret_cast<DCI_Owner*>( nullptr ) ),
	m_change_que( reinterpret_cast<BF_Lib::Bit_List*>( nullptr ) ),
	m_dev_profile( reinterpret_cast<MODBUS_TARGET_INFO_ST_TD*>( nullptr ) )
{
	m_change_tracker = new Change_Tracker( &Change_Track_Handler_Static,
										   reinterpret_cast<Change_Tracker::cback_param_t>( this ) );

	m_change_owner = change_que_owner;
	m_change_owner->Attach_Callback( &Change_Que_Reg_Callback_Static,
									 reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
									 ( DCI_ACCESS_GET_RAM_CMD_MSK |
									   DCI_ACCESS_SET_RAM_CMD_MSK ) );

	m_change_que = new BF_Lib::Bit_List( DCI_TOTAL_DCIDS );

	m_current_dcid = BF_Lib::Bit_List::NO_BIT_FOUND;
	m_change_shadow = false;

	m_dev_profile = modbus_dev_profile;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Modbus_Ch_Track::Change_Track_Handler( DCI_ID_TD dci_id,
											Change_Tracker::attrib_mask_t attribute_mask )
{
	// Set the bit that matches the dcid location in bit location.
	m_change_que->Set( dci_id );

	// Check to see if the active bit (DCID) is a changed value.
	// If it is not a changed bit then we need to go find one.  Since we know
	// that there has to be at least one now.
	if ( m_current_dcid == BF_Lib::Bit_List::NO_BIT_FOUND )		//! m_change_que->Test(
	// m_current_dcid ) )
	{
		m_current_dcid = m_change_que->Get_Next();
	}

	// If the changed id is the active id then we should keep a shadow of the value.
	// Just in case the request misses the new change.
	if ( dci_id == m_current_dcid )
	{
		m_change_shadow = true;
	}
	else
	{
		m_change_shadow = false;
	}
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'attribute_mask' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD Modbus_Ch_Track::Change_Que_Reg_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;

	BF_Lib::SPLIT_UINT16 reg_changed;
	DCI_ID_TD temp_dcid;

	switch ( access_struct->command )
	{
		case DCI_ACCESS_GET_RAM_CMD:
			if ( m_current_dcid == BF_Lib::Bit_List::NO_BIT_FOUND )
			{
				reg_changed.u16 = MB_CH_NO_REG_CHANGE;
			}
			else
			{
				reg_changed.u16 = Find_Modbus_Reg( m_current_dcid );
			}
			reinterpret_cast<uint8_t*>( access_struct->data_access.data )[0] =
				reg_changed.u8[0];
			reinterpret_cast<uint8_t*>( access_struct->data_access.data )[1] =
				reg_changed.u8[1];
			break;

		case DCI_ACCESS_SET_RAM_CMD:
			reg_changed.u8[0] =
				reinterpret_cast<uint8_t*>( access_struct->data_access.data )[0];
			reg_changed.u8[1] =
				reinterpret_cast<uint8_t*>( access_struct->data_access.data )[1];
			if ( reg_changed.u16 == MB_CH_NO_REG_CHANGE )
			{
				if ( m_change_shadow == true )
				{
					m_change_que->Set( m_current_dcid );
				}
				m_current_dcid = m_change_que->Get_Next();
				m_change_shadow = false;
			}
			else
			{
				temp_dcid = Find_DCID( reg_changed.u16 );
				if ( temp_dcid != MB_CH_NO_DCID_VAL )
				{
					m_change_tracker->Track_ID( temp_dcid );
				}
			}
			break;

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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Modbus_Ch_Track::Find_Modbus_Reg( DCI_ID_TD dcid ) const
{
	int_fast16_t result;
	uint16_t return_result = MB_CH_NO_REG_CHANGE;
	uint_fast16_t index = 1U;
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* struct_start = m_dev_profile->dcid_to_mbus;

	struct_start--;		// Since the index is 1 we must step back one to get the right index later.
	// It is a bit of nastyness but it must be done because the algorithm expects data to start
	// after 0.

	// m_total_dcids should equal the number of nodes in the search tree.
	while ( index <= m_dev_profile->total_dcids )
	{
		result = static_cast<int_fast16_t>( struct_start[index].dcid ) - static_cast<
			int_fast16_t>( dcid );
		if ( 0 == result )
		{
			return_result = ( struct_start[index].modbus_reg );		// REDFLAG - I am returning in
																	// the middle of a function -
																	// unclean.
			index = static_cast<uint_fast16_t>( m_dev_profile->total_dcids ) + 1U;
		}
		else
		{
			if ( result < 0 )
			{
				index = ( index << 1U ) + 1U;
			}
			else
			{
				index = ( index << 1U );
			}
		}
	}

	return ( return_result );		// Look up above.  I have a return buried in there.  This is
									// supposed to be very fast.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ID_TD Modbus_Ch_Track::Find_DCID( uint16_t modbus_reg ) const
{
	int_fast16_t result;
	uint_fast16_t index = 1U;
	DCI_ID_TD return_status = MB_CH_NO_DCID_VAL;
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* struct_start = m_dev_profile->mbus_to_dcid;

	struct_start--;		// Since the index is 1 we must step back one to get the right index later.

	// m_total_registers should equal the number of nodes in the search tree.
	while ( index <= m_dev_profile->total_registers )
	{
		result = static_cast<int_fast16_t>( struct_start[index].modbus_reg ) - static_cast<
			int_fast16_t>( modbus_reg );
		if ( 0 == result )
		{
			return_status = ( struct_start[index].dcid );	// REDFLAG - I am returning in the
															// middle of a function - unclean.
			index = static_cast<uint_fast16_t>( m_dev_profile->total_registers ) + 1U;
		}
		else
		{
			if ( result < 0 )
			{
				index = ( index << 1U ) + 1U;
			}
			else
			{
				index = ( index << 1U );
			}
		}
	}

	return ( return_status );	// Look up above.  I have a return buried in there.  This is
								// supposed to be very fast.
}

}	/* End namespace BF_Mbus for this module*/
