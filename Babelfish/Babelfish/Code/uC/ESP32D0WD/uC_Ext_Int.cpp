/**
 *****************************************************************************************
 *	@file      uC_Ext_Int.cpp
 *
 *	@details   See header file for module overview.
 *
 *	@copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Ext_Int.h"
#include "uC_IO_Config.h"

/*
 *****************************************************************************************
 * Constants
 *****************************************************************************************
 */
static const uint8_t MAX_GPIO_LEVEL = 32;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Ext_Int::uC_Ext_Int( uC_USER_IO_STRUCT const* pio_ctrl ) :
	m_ext_pio_ctrl( reinterpret_cast<uC_USER_IO_STRUCT const*>( nullptr ) )
{
	m_ext_pio_ctrl = pio_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Attach_IRQ( INT_CALLBACK_FUNC* ext_int_callback,
							 uint8_t edge_select, uC_IO_INPUT_CFG_EN input_cfg,
							 uint8_t priority ) const
{
	interrupt_config_t* interrupt_config = reinterpret_cast<interrupt_config_t*>( ext_int_callback );

	uC_IO_Config::Set_Pin_To_Input( m_ext_pio_ctrl );

	gpio_set_intr_type( static_cast<gpio_num_t>( m_ext_pio_ctrl->io_num ),
						static_cast<gpio_int_type_t>( edge_select ) );

	gpio_install_isr_service( 0 );

	gpio_isr_handler_add( static_cast<gpio_num_t>( m_ext_pio_ctrl->io_num ), interrupt_config->handler,
						  reinterpret_cast<void*>( m_ext_pio_ctrl->io_num ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Enable_Int( void ) const
{
	gpio_intr_enable( static_cast<gpio_num_t>( m_ext_pio_ctrl->io_num ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Disable_Int( void ) const
{
	gpio_intr_disable( static_cast<gpio_num_t>( m_ext_pio_ctrl->io_num ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Clear_Int( void ) const
{
	/*GPIO0~31 interrupt status clear */
	if ( m_ext_pio_ctrl->io_num < MAX_GPIO_LEVEL )
	{
		GPIO.status_w1tc = ( 1 << m_ext_pio_ctrl->io_num );
	}
	else
	{
		/*GPIO32~39 interrupt status clear*/
		GPIO.status1_w1tc.val = ( 1 << ( m_ext_pio_ctrl->io_num - MAX_GPIO_LEVEL ) );
	}
}
