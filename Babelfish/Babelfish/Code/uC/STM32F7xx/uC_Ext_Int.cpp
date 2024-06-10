/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Ext_Int.h"
#include "uC_IO_Config.h"
#include "Ram.h"
#include "Legacy/stm32_hal_legacy.h"
#include "stm32F7xx_hal_rcc.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define uC_EXT_INT_5to9_CLR_MASK     0x03E0U
#define uC_EXT_INT_10to15_CLR_MASK   0xFC00U

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint16_t uC_Ext_Int::m_activated_inputs = 0U;
INT_CALLBACK_FUNC** uC_Ext_Int::m_ext_int_vect_5to9 =
	reinterpret_cast<INT_CALLBACK_FUNC**>( nullptr );
INT_CALLBACK_FUNC** uC_Ext_Int::m_ext_int_vect_10to15 =
	reinterpret_cast<INT_CALLBACK_FUNC**>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Ext_Int::uC_Ext_Int( uC_USER_IO_STRUCT const* pio_ctrl ) :
	m_ext_int_ctrl( reinterpret_cast<uC_BASE_EXT_INT_IO_STRUCT const*>( nullptr ) ),
	m_ext_pio_ctrl( reinterpret_cast<uC_USER_IO_STRUCT const*>( nullptr ) )
{
	uint32_t activated_input_mask;

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	m_ext_int_ctrl = ( uC_Base::Self() )->Get_Ext_Int_Ctrl( pio_ctrl );
	BF_ASSERT( m_ext_int_ctrl != NULL );
	activated_input_mask = static_cast<uint32_t>( 1U ) << static_cast<uint32_t>
		( pio_ctrl->pio_num );

	BF_ASSERT( ( activated_input_mask & m_activated_inputs ) == 0U );
	m_ext_pio_ctrl = pio_ctrl;
	m_activated_inputs |= static_cast<uint16_t>( activated_input_mask );

	EXTI->IMR &= ~activated_input_mask;

	SYSCFG->EXTICR[pio_ctrl->pio_num >>
				   0x02U] &=
		~( static_cast<uint32_t>
		   ( 0x0FU ) ) <<
			( 0x04U * ( pio_ctrl->pio_num & static_cast<uint8_t>( 0x03U ) ) );
	SYSCFG->EXTICR[pio_ctrl->pio_num >>
				   0x02U] |=
		( ( static_cast<uint32_t>
			( pio_ctrl->port_index ) ) <<
			( 0x04U * ( pio_ctrl->pio_num & static_cast<uint8_t>( 0x03U ) ) ) );
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
	uint32_t activated_input_mask;

	activated_input_mask = static_cast<uint32_t>( 1UL << m_ext_pio_ctrl->pio_num );

	if ( m_ext_int_ctrl->irq_num == EXTI9_5_IRQn )
	{
		if ( m_ext_int_vect_5to9 == NULL )
		{
			// m_ext_int_vect_5to9= &ext_int_callback; //Added by Pranav for testing

			uC_Interrupt::Set_Vector( &External_Int_Vect_5to9,
									  m_ext_int_ctrl->irq_num, priority );
			/*
			   uC_Interrupt::Set_Vector( ext_int_callback,
			     m_ext_int_ctrl->irq_num, priority ); //Added by Pranav for testing
			 */

			m_ext_int_vect_5to9 = reinterpret_cast<INT_CALLBACK_FUNC**>( Ram::Allocate(
																			 ( sizeof(
																				   INT_CALLBACK_FUNC
																				   * ) *
																			   uC_EXT_INT_5to9_VECT_COUNT ) ) );
			// (*m_ext_int_vect_5to9[2])();
			// m_ext_int_vect_5to9[2] = ext_int_callback;
			Set_Vector_5to9( ext_int_callback );
		}
	}
	else if ( m_ext_int_ctrl->irq_num == EXTI15_10_IRQn )
	{
		if ( m_ext_int_vect_10to15 == NULL )
		{
			uC_Interrupt::Set_Vector( &External_Int_Vect_10to15,
									  m_ext_int_ctrl->irq_num, priority );
			m_ext_int_vect_10to15 = reinterpret_cast<INT_CALLBACK_FUNC**>( Ram::Allocate(
																			   ( sizeof(
																					 INT_CALLBACK_FUNC
																					 * ) *
																				 uC_EXT_INT_10to15_VECT_COUNT ) ) );
			Set_Vector_10to15( ext_int_callback );
		}
	}
	else
	{
		uC_Interrupt::Set_Vector( ext_int_callback, m_ext_int_ctrl->irq_num, priority );
	}

	EXTI->RTSR &= ~activated_input_mask;
	EXTI->FTSR &= ~activated_input_mask;
	if ( ( edge_select == EXT_INT_RISING_EDGE_TRIG ) ||
		 ( edge_select == EXT_INT_CHANGE_TRIG ) )
	{
		EXTI->RTSR |= activated_input_mask;
	}
	if ( ( edge_select == EXT_INT_FALLING_EDGE_TRIG ) ||
		 ( edge_select == EXT_INT_CHANGE_TRIG ) )
	{
		EXTI->FTSR |= activated_input_mask;
	}

	uC_IO_Config::Set_Pin_To_Input( m_ext_pio_ctrl, input_cfg );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Set_Vector_5to9( INT_CALLBACK_FUNC* ext_int_callback ) const
{
	if ( m_ext_pio_ctrl->pio_num == IO_PIN_5 )
	{
		m_ext_int_vect_5to9[0] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_6 )
	{
		m_ext_int_vect_5to9[1] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_7 )
	{
		m_ext_int_vect_5to9[2] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_8 )
	{
		m_ext_int_vect_5to9[3] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_9 )
	{
		m_ext_int_vect_5to9[4] = ext_int_callback;
	}
	else
	{
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Set_Vector_10to15( INT_CALLBACK_FUNC* ext_int_callback ) const
{
	if ( m_ext_pio_ctrl->pio_num == IO_PIN_10 )
	{
		m_ext_int_vect_10to15[0] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_11 )
	{
		m_ext_int_vect_10to15[1] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_12 )
	{
		m_ext_int_vect_10to15[2] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_13 )
	{
		m_ext_int_vect_10to15[3] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_14 )
	{
		m_ext_int_vect_10to15[4] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_15 )
	{
		m_ext_int_vect_10to15[5] = ext_int_callback;
	}
	else
	{
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_Ext_Int::Enable_Int( void ) const
{
	uint32_t activated_input_mask;
	static bool Init_Once = false;

	if ( Init_Once == false )
	{
		activated_input_mask = static_cast<uint32_t>( 1UL << m_ext_pio_ctrl->pio_num );
		EXTI->IMR |= activated_input_mask;
		Init_Once = true;
	}
	uC_Interrupt::Enable_Int( m_ext_int_ctrl->irq_num );// SSN just Enable the NVIC here


}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Disable_Int( void ) const
{
	// UINT32 activated_input_mask;

	// activated_input_mask = 1<<m_ext_pio_ctrl->pio_num;
	// EXTI->IMR &= ~activated_input_mask; //SSN Ori: IF you uncomment this statement the external
	// interrupt is never captured

	uC_Interrupt::Disable_Int( m_ext_int_ctrl->irq_num );	// SSN just disable the NVIC here
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Clear_Int( void ) const
{
	uint32_t activated_input_mask;

	activated_input_mask = static_cast<uint32_t>( 1UL << m_ext_pio_ctrl->pio_num );
	EXTI->PR |= activated_input_mask;
	uC_Interrupt::Clear_Int( m_ext_int_ctrl->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::External_Int_Vect_5to9( void )
{
	uint32_t int_request_trigger;

	int_request_trigger = EXTI->PR;
	EXTI->PR |= static_cast<uint32_t>( uC_EXT_INT_5to9_CLR_MASK );

	if ( ( int_request_trigger & ( 1UL << ( 0U + 5U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_5to9[0] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 1U + 5U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_5to9[1] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 2U + 5U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_5to9[2] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 3U + 5U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_5to9[3] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 4U + 5U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_5to9[4] )();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::External_Int_Vect_10to15( void )
{
	uint32_t int_request_trigger;

	int_request_trigger = EXTI->PR;
	EXTI->PR |= static_cast<uint32_t>( uC_EXT_INT_10to15_CLR_MASK );

	if ( ( int_request_trigger & ( 1UL << ( 0U + 10U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_10to15[0] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 1U + 10U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_10to15[1] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 2U + 10U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_10to15[2] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 3U + 10U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_10to15[3] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 4U + 10U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_10to15[4] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 5U + 10U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_10to15[5] )();
	}
}

#if 0
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Disable_Int( void )
{
	uC_Interrupt::Disable_Int( m_ext_int_ctrl->pid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Disable_Int( void )
{
	uC_Interrupt::Disable_Int( m_ext_int_ctrl->pid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Ext_Int::uC_Ext_Int( uint32_t irq_input )
{
	m_ext_int_ctrl = ( uC_Base::Self() )->Get_Ext_Int_Ctrl( irq_input );
	BF_ASSERT( m_ext_int_ctrl != 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Attach_IRQ( INT_CALLBACK_FUNC* ext_int_callback,
							 uint8_t edge_select, bool pullup_enable, uint8_t priority )
{
	uC_IO_Config::Enable_Periph_Input_Pin( m_ext_int_ctrl->ext_int_pio, pullup_enable );
	uC_Interrupt::Set_Vector( ext_int_callback, m_ext_int_ctrl->pid, priority, edge_select );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Attach_FIQ( FAST_INT_CALLBACK_FUNC* ext_int_callback,
							 uint8_t edge_select, bool pullup_enable, uint8_t priority )
{
	uC_IO_Config::Enable_Periph_Input_Pin( m_ext_int_ctrl->ext_int_pio, pullup_enable );
	uC_Interrupt::Set_FIQ_Vector( ext_int_callback, edge_select );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Enable_Int( void )
{
	uC_Interrupt::Enable_Int( m_ext_int_ctrl->pid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Disable_Int( void )
{
	uC_Interrupt::Disable_Int( m_ext_int_ctrl->pid );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Clear_Int( void )
{
	uC_Interrupt::Clear_Int( m_ext_int_ctrl->pid );
}

#endif
