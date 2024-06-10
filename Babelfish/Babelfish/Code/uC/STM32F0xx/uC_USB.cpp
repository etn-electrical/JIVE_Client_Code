/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_USB.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USB::uC_USB( uint8_t usb_port_id )
{


	m_io_ctrl = ( uC_Base::Self() )->Get_USB_Ctrl( usb_port_id );
	BF_ASSERT( m_io_ctrl != 0 );
	uC_Base::Reset_Periph( &m_io_ctrl->periph_def );

	uC_IO_Config::Set_Pin_To_Input( m_io_ctrl->port_io->vbus_pio );
	uC_IO_Config::Enable_Periph_Input_Pin( m_io_ctrl->port_io->id_pio );
	uC_IO_Config::Enable_Periph_Input_Pin( m_io_ctrl->port_io->dm_pio );
	uC_IO_Config::Enable_Periph_Input_Pin( m_io_ctrl->port_io->dp_pio );

	uC_Base::Enable_Periph_Clock( &m_io_ctrl->periph_def );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USB::~uC_USB()
{}
