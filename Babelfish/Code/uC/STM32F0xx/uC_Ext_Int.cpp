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
#include "stm32f0xx_hal_rcc.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define uC_EXT_INT_0to1_CLR_MASK     0x0003UL
#define uC_EXT_INT_2to3_CLR_MASK     0x000CUL
#define uC_EXT_INT_4to15_CLR_MASK    0xFFF0UL

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint16_t uC_Ext_Int::m_activated_inputs = 0U;
INT_CALLBACK_FUNC** uC_Ext_Int::m_ext_int_vect_0to1 =
	reinterpret_cast<INT_CALLBACK_FUNC**>( nullptr );
INT_CALLBACK_FUNC** uC_Ext_Int::m_ext_int_vect_2to3 =
	reinterpret_cast<INT_CALLBACK_FUNC**>( nullptr );
INT_CALLBACK_FUNC** uC_Ext_Int::m_ext_int_vect_4to15 =
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
	PERIPH_DEF_ST periph_def;

	periph_def.pclock_reg_index = RCC_PERIPH_CLCK_AHB;
	periph_def.pid = RCC_AHBENR_GPIOAEN;

	switch ( pio_ctrl->port_index )
	{
		case uC_PIO_INDEX_A:
			periph_def.pid = RCC_AHBENR_GPIOAEN;
			break;

		case uC_PIO_INDEX_B:
			periph_def.pid = RCC_AHBENR_GPIOBEN;
			break;

		case uC_PIO_INDEX_C:
			periph_def.pid = RCC_AHBENR_GPIOCEN;
			break;

		case uC_PIO_INDEX_D:
			periph_def.pid = RCC_AHBENR_GPIODEN;
			break;

		case uC_PIO_INDEX_F:
			periph_def.pid = RCC_AHBENR_GPIOFEN;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	m_ext_int_ctrl = ( uC_Base::Self() )->Get_Ext_Int_Ctrl( pio_ctrl );

	BF_ASSERT( m_ext_int_ctrl != NULL )
	if ( m_activated_inputs == 0U )
	{
		uC_Base::Reset_Periph( &periph_def );
		uC_Base::Enable_Periph_Clock( &periph_def );

		periph_def.pid = RCC_APB2ENR_SYSCFGCOMPEN;
		periph_def.pclock_reg_index = RCC_PERIPH_CLCK_APB2;
		uC_Base::Reset_Periph( &periph_def );
		uC_Base::Enable_Periph_Clock( &periph_def );
	}

	activated_input_mask = static_cast<uint32_t>( 1UL << pio_ctrl->pio_num );

	BF_ASSERT( ( activated_input_mask & m_activated_inputs ) == 0U );
	m_ext_pio_ctrl = pio_ctrl;
	m_activated_inputs |= static_cast<uint16_t>( activated_input_mask );

	EXTI->IMR &= ~activated_input_mask;

	SYSCFG->EXTICR[pio_ctrl->pio_num >>
				   0x02U] &= ~( ( static_cast<uint16_t>( 0x0F ) ) << ( 0x04U *
																	   (
																		   pio_ctrl->
																		   pio_num &
																		   static_cast<
																			   uint8_t>( 0x03 ) ) ) );

	SYSCFG->EXTICR[pio_ctrl->pio_num >> 0x02U] |=
		static_cast<uint32_t>( ( static_cast<uint32_t>( pio_ctrl->port_index ) )
							   << ( 0x04U *
									( pio_ctrl->pio_num & static_cast<uint8_t>
									  ( 0x03 ) ) ) );
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

	if ( m_ext_int_ctrl->irq_num == EXTI0_1_IRQn )
	{
		if ( m_ext_int_vect_0to1 == NULL )
		{

			uC_Interrupt::Set_Vector( &External_Int_Vect_0to1,
									  m_ext_int_ctrl->irq_num, priority );
			/*
			   uC_Interrupt::Set_Vector( ext_int_callback,
			   m_ext_int_ctrl->irq_num, priority ); //Added by Pranav for testing
			 */

			m_ext_int_vect_0to1 = reinterpret_cast<INT_CALLBACK_FUNC**>( Ram::Allocate(
																			 ( sizeof(
																				   INT_CALLBACK_FUNC
																				   * ) *
																			   uC_EXT_INT_0to1_VECT_COUNT ) ) );
			// (*m_ext_int_vect_5to9[2])();
			// m_ext_int_vect_5to9[2] = ext_int_callback;
		}
		Set_Vector_0to1( ext_int_callback );

	}
	else if ( m_ext_int_ctrl->irq_num == EXTI2_3_IRQn )
	{
		if ( m_ext_int_vect_2to3 == NULL )
		{
			uC_Interrupt::Set_Vector( &External_Int_Vect_2to3,
									  m_ext_int_ctrl->irq_num, priority );
			m_ext_int_vect_2to3 = reinterpret_cast<INT_CALLBACK_FUNC**>( Ram::Allocate(
																			 ( sizeof(
																				   INT_CALLBACK_FUNC
																				   * ) *
																			   uC_EXT_INT_2to3_VECT_COUNT ) ) );
		}
		Set_Vector_2to3( ext_int_callback );
	}
	else if ( m_ext_int_ctrl->irq_num == EXTI4_15_IRQn )
	{
		if ( m_ext_int_vect_4to15 == NULL )
		{
			uC_Interrupt::Set_Vector( &External_Int_Vect_4to15,
									  m_ext_int_ctrl->irq_num, priority );
			m_ext_int_vect_4to15 = reinterpret_cast<INT_CALLBACK_FUNC**>( Ram::Allocate(
																			  ( sizeof(
																					INT_CALLBACK_FUNC
																					* ) *
																				uC_EXT_INT_4to15_VECT_COUNT ) ) );
		}
		Set_Vector_4to15( ext_int_callback );
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
void uC_Ext_Int::Set_Vector_0to1( INT_CALLBACK_FUNC* ext_int_callback ) const
{
	if ( m_ext_pio_ctrl->pio_num == IO_PIN_0 )
	{
		m_ext_int_vect_0to1[0] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_1 )
	{
		m_ext_int_vect_0to1[1] = ext_int_callback;
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
void uC_Ext_Int::Set_Vector_2to3( INT_CALLBACK_FUNC* ext_int_callback ) const
{
	if ( m_ext_pio_ctrl->pio_num == IO_PIN_2 )
	{
		m_ext_int_vect_2to3[0] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_3 )
	{
		m_ext_int_vect_2to3[1] = ext_int_callback;
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
void uC_Ext_Int::Set_Vector_4to15( INT_CALLBACK_FUNC* ext_int_callback ) const
{
	if ( m_ext_pio_ctrl->pio_num == IO_PIN_4 )
	{
		m_ext_int_vect_4to15[0] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_5 )
	{
		m_ext_int_vect_4to15[1] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_6 )
	{
		m_ext_int_vect_4to15[2] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_7 )
	{
		m_ext_int_vect_4to15[3] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_8 )
	{
		m_ext_int_vect_4to15[4] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_9 )
	{
		m_ext_int_vect_4to15[5] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_10 )
	{
		m_ext_int_vect_4to15[6] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_11 )
	{
		m_ext_int_vect_4to15[7] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_12 )
	{
		m_ext_int_vect_4to15[8] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_13 )
	{
		m_ext_int_vect_4to15[9] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_14 )
	{
		m_ext_int_vect_4to15[10] = ext_int_callback;
	}
	else if ( m_ext_pio_ctrl->pio_num == IO_PIN_15 )
	{
		m_ext_int_vect_4to15[11] = ext_int_callback;
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

	// static bool Init_Once = false;

	// if ( Init_Once == false )
	// {
	activated_input_mask = static_cast<uint32_t>( 1UL << m_ext_pio_ctrl->pio_num );
	EXTI->IMR |= activated_input_mask;
	// Init_Once = true;
	// }
	uC_Interrupt::Enable_Int( m_ext_int_ctrl->irq_num );		// SSN just Enable the NVIC here

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
void uC_Ext_Int::External_Int_Vect_0to1( void )
{
	uint32_t int_request_trigger;

	int_request_trigger = EXTI->PR;
	EXTI->PR |= static_cast<uint32_t>( uC_EXT_INT_0to1_CLR_MASK );

	if ( ( int_request_trigger & ( 1UL << ( 0U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_0to1[0] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 1U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_0to1[1] )();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::External_Int_Vect_2to3( void )
{
	uint32_t int_request_trigger;

	int_request_trigger = EXTI->PR;
	EXTI->PR |= static_cast<uint32_t>( uC_EXT_INT_2to3_CLR_MASK );

	if ( ( int_request_trigger & ( 1UL << ( 0U + 2U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_2to3[0] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 1U + 2U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_2to3[1] )();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::External_Int_Vect_4to15( void )
{
	uint32_t int_request_trigger;

	int_request_trigger = EXTI->PR;
	EXTI->PR |= static_cast<uint32_t>( uC_EXT_INT_4to15_CLR_MASK );

	if ( ( int_request_trigger & ( 1UL << ( 0U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[0] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 1U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[1] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 2U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[2] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 3U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[3] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 4U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[4] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 5U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[5] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 6U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[6] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 7U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[7] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 8U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[8] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 9U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[9] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 10U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[10] )();
	}
	if ( ( int_request_trigger & ( 1UL << ( 11U + 4U ) ) ) > 0UL )
	{
		( *m_ext_int_vect_4to15[11] )();
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
