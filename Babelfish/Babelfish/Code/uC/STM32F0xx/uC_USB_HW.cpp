/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_USB_HW.h"
#include "Exception.h"
#include "uC_IO_Config.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
extern void HAL_USB_Config( void );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USB_HW::uC_USB_HW( uint8_t usb_port_id,
					  cback_func_t int_handler, cback_param_t param, uint16_t priority ) :
	m_io_ctrl( reinterpret_cast<uC_BASE_USB_IO_STRUCT const*>( nullptr ) ),
	m_int_vect_cback( reinterpret_cast<Advanced_Vect*>( nullptr ) )
{
	m_io_ctrl = ( uC_Base::Self() )->Get_USB_Ctrl( usb_port_id );

	BF_ASSERT( m_io_ctrl != 0 );
	// uC_Base::Reset_Periph( &m_io_ctrl->periph_def );

	HAL_USB_Config();
	// uC_IO_Config::Set_Pin_To_Input(m_io_ctrl->port_io->vbus_pio);
	// uC_IO_Config::Enable_Periph_Input_Pin(m_io_ctrl->port_io->id_pio);
	// uC_IO_Config::Enable_Periph_Input_Pin(m_io_ctrl->port_io->dm_pio);
	// uC_IO_Config::Enable_Periph_Output_Pin(m_io_ctrl->port_io->dp_pio);

	// uC_Base::Enable_Periph_Clock( &m_io_ctrl->periph_def );

	m_int_vect_cback = new Advanced_Vect();
	m_int_vect_cback->Set_Vect( int_handler, param );
	uC_Interrupt::Set_Vector( reinterpret_cast<INT_CALLBACK_FUNC*>( m_int_vect_cback->
																	Get_Int_Func() ),
							  m_io_ctrl->irq_num, priority );
	uC_Interrupt::Enable_Int( m_io_ctrl->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_USB_HW::~uC_USB_HW()
{
	m_io_ctrl = reinterpret_cast<uC_BASE_USB_IO_STRUCT const*>( nullptr );
	delete m_int_vect_cback;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_USB_HW::USB_Power_Is_Present( void ) const
{
	return ( uC_IO_Config::Get_In( m_io_ctrl->port_io->vbus_pio ) );
}
