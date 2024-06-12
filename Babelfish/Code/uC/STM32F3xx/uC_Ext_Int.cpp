/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
 * casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts
 * are required for all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions
 * shall not change the signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as
 * is The note is re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_Ext_Int.h"
#include "uC_IO_Config.h"
#include "Exception.h"
#include "Ram.h"
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

	m_ext_int_ctrl = ( uC_Base::Self() )->Get_Ext_Int_Ctrl( pio_ctrl );
	BF_ASSERT( m_ext_int_ctrl != NULL );

	activated_input_mask = static_cast<uint32_t>( 1U ) << static_cast<uint32_t>
		( pio_ctrl->pio_num );

	BF_ASSERT( ( activated_input_mask & m_activated_inputs ) == 0U );

	m_ext_pio_ctrl = pio_ctrl;
	m_activated_inputs |= static_cast<uint16_t>( activated_input_mask );

	EXTI->IMR &= ~activated_input_mask;

	SYSCFG->EXTICR[pio_ctrl->pio_num >> 0x02U] &=
		~( ( static_cast<uint32_t>( 0x0FU ) ) <<
			( 0x04U * ( pio_ctrl->pio_num & static_cast<uint8_t>( 0x03U ) ) ) );
	SYSCFG->EXTICR[pio_ctrl->pio_num >> 0x02U] |=
		( ( static_cast<uint32_t>( pio_ctrl->port_index ) ) <<
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

	activated_input_mask = static_cast<uint32_t>( 1U ) << static_cast<uint32_t>
		( m_ext_pio_ctrl->pio_num );

	if ( m_ext_int_ctrl->irq_num == EXTI9_5_IRQn )
	{
		if ( m_ext_int_vect_5to9 == NULL )
		{
			uC_Interrupt::Set_Vector( &External_Int_Vect_5to9,
									  m_ext_int_ctrl->irq_num, priority );
			m_ext_int_vect_5to9 = reinterpret_cast<INT_CALLBACK_FUNC**>( Ram::Allocate(
																			 ( sizeof(
																				   INT_CALLBACK_FUNC
																				   * ) *
																			   uC_EXT_INT_5to9_VECT_COUNT ) ) );
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
		}
	}
	else
	{
		uC_Interrupt::Set_Vector( ext_int_callback, m_ext_int_ctrl->irq_num, priority );
	}

	if ( ( edge_select == static_cast<uint8_t>( EXT_INT_RISING_EDGE_TRIG ) ) ||
		 ( edge_select == static_cast<uint8_t>( EXT_INT_CHANGE_TRIG ) ) )
	{
		EXTI->RTSR |= activated_input_mask;
	}
	else
	{
		EXTI->RTSR &= ~activated_input_mask;
	}

	if ( ( edge_select == static_cast<uint8_t>( EXT_INT_FALLING_EDGE_TRIG ) ) ||
		 ( edge_select == static_cast<uint8_t>( EXT_INT_CHANGE_TRIG ) ) )
	{
		EXTI->FTSR |= activated_input_mask;
	}
	else
	{
		EXTI->FTSR &= ~activated_input_mask;
	}

	uC_IO_Config::Set_Pin_To_Input( m_ext_pio_ctrl, input_cfg );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Enable_Int( void ) const
{
	uint32_t activated_input_mask;

	uC_Interrupt::Enable_Int( m_ext_int_ctrl->irq_num );

	activated_input_mask = static_cast<uint32_t>( 1U ) << static_cast<uint32_t>
		( m_ext_pio_ctrl->pio_num );
	EXTI->IMR |= activated_input_mask;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Disable_Int( void ) const
{
	uint32_t activated_input_mask;

	activated_input_mask = static_cast<uint32_t>( 1U ) << static_cast<uint32_t>
		( m_ext_pio_ctrl->pio_num );
	EXTI->IMR &= ~activated_input_mask;

	uC_Interrupt::Disable_Int( m_ext_int_ctrl->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Clear_Int( void ) const
{
	uint32_t activated_input_mask;

	activated_input_mask = static_cast<uint32_t>( 1U ) << static_cast<uint32_t>
		( m_ext_pio_ctrl->pio_num );
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
	EXTI->PR |= uC_EXT_INT_5to9_CLR_MASK;

	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 5U ) ) ) != 0U )
	{
		( *m_ext_int_vect_5to9[0] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 1U + 5U ) ) ) != 0U )
	{
		( *m_ext_int_vect_5to9[1] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 2U + 5U ) ) ) != 0U )
	{
		( *m_ext_int_vect_5to9[2] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 3U + 5U ) ) ) != 0U )
	{
		( *m_ext_int_vect_5to9[3] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 4U + 5U ) ) ) != 0U )
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
	EXTI->PR |= uC_EXT_INT_10to15_CLR_MASK;

	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 10U ) ) ) != 0U )
	{
		( *m_ext_int_vect_10to15[0] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 1U + 10U ) ) ) != 0U )
	{
		( *m_ext_int_vect_10to15[1] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 2U + 10U ) ) ) != 0U )
	{
		( *m_ext_int_vect_10to15[2] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 3U + 10U ) ) ) != 0U )
	{
		( *m_ext_int_vect_10to15[3] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 4U + 10U ) ) ) != 0U )
	{
		( *m_ext_int_vect_10to15[4] )();
	}
	if ( ( int_request_trigger & ( static_cast<uint32_t>( 1U ) << ( 5U + 10U ) ) ) != 0U )
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
	BF_ASSERT( m_ext_int_ctrl != 0U );
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
/* lint +e1924
  lint +e1960*/
