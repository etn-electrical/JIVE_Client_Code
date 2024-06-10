/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Dip_Switch.h"
#include "ProfiBus_Address.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define PROFI_ADDRESS_MASK          0x7FU
#define DEFAULT_PROFI_ADDRESS       125U

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
ProfiBus_Address::ProfiBus_Address( Dip_Switch* dipswitch_ctrl, DCI_Owner* profi_address_owner ) :
	m_dipswitch_ctrl( dipswitch_ctrl ),
	m_profi_address_owner( profi_address_owner )
{
	m_dipswitch_ctrl = dipswitch_ctrl;
	m_profi_address_owner = profi_address_owner;
	Update_Address();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ProfiBus_Address::Update_Address( void )
{
	uint8_t profi_address;

	profi_address = ( static_cast<uint8_t>( m_dipswitch_ctrl->Get_Dip_Value() ) & PROFI_ADDRESS_MASK );

	if ( ( profi_address > DEFAULT_PROFI_ADDRESS ) || ( profi_address == 0U ) )
	{
		profi_address = DEFAULT_PROFI_ADDRESS;
	}

	m_profi_address_owner->Check_In( &profi_address );

}
