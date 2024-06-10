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
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 * signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 * re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_Ext_Int.h"
#include "uC_IO_Config.h"
#include "Ram.h"
#include "OS_Tasker.h"
#include "uC_OS_Device_Defines.h"
#include "Babelfish_Assert.h"


/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint32_t uC_Ext_Int::m_activated_inputs[] = {0, 0, 0, 0, 0};
INT_CALLBACK_FUNC* uC_Ext_Int::m_ext_int_vect_a[] = {NULL};
INT_CALLBACK_FUNC* uC_Ext_Int::m_ext_int_vect_b[] = {NULL};
INT_CALLBACK_FUNC* uC_Ext_Int::m_ext_int_vect_c[] = {NULL};
INT_CALLBACK_FUNC* uC_Ext_Int::m_ext_int_vect_d[] = {NULL};
INT_CALLBACK_FUNC* uC_Ext_Int::m_ext_int_vect_e[] = {NULL};
bool uC_Ext_Int::m_attached[] = { false, false, false, false, false };

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

	m_edge_select = EXT_INT_DISABLED;

	m_ext_int_ctrl = ( uC_Base::Self() )->Get_Ext_Int_Ctrl( pio_ctrl );
	BF_ASSERT( m_ext_int_ctrl != NULL );
	activated_input_mask = static_cast<uint32_t>( 1U ) << static_cast<uint32_t>( pio_ctrl->pio_num );

	if ( ( activated_input_mask & m_activated_inputs[pio_ctrl->port_index] ) == 0U )
	{
		m_ext_pio_ctrl = pio_ctrl;
		m_activated_inputs[pio_ctrl->port_index] |= activated_input_mask;
	}
	else
	{
		BF_ASSERT( false );	// Can't assign the same pin to an interrupt on a different port.
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Attach_IRQ( INT_CALLBACK_FUNC* ext_int_callback,
							 uint8_t edge_select, uC_IO_INPUT_CFG_EN input_cfg, uint8_t priority )
{
	// First attached callback sets the priority for the port
	// This could probably be changed to increase the priority if necessary,
	// but that is not the way this works.
	if ( !( m_attached[m_ext_pio_ctrl->port_index] ) )
	{
		switch ( m_ext_int_ctrl->irq_num )
		{
			case PORTA_IRQn:
				uC_Interrupt::Set_Vector( External_Int_Vect_A,
										  PORTA_IRQn,
										  priority );
				break;

			case PORTB_IRQn:
				uC_Interrupt::Set_Vector( External_Int_Vect_B,
										  PORTB_IRQn,
										  priority );
				break;

			case PORTC_IRQn:
				uC_Interrupt::Set_Vector( External_Int_Vect_C,
										  PORTC_IRQn,
										  priority );
				break;

			case PORTD_IRQn:
				uC_Interrupt::Set_Vector( External_Int_Vect_D,
										  PORTD_IRQn,
										  priority );
				break;

			case PORTE_IRQn:
				uC_Interrupt::Set_Vector( External_Int_Vect_E,
										  PORTE_IRQn,
										  priority );
				break;

			default:
				BF_ASSERT( false );
				break;
		}
		m_attached[m_ext_pio_ctrl->port_index] = true;
		uC_Interrupt::Enable_Int( m_ext_int_ctrl->irq_num );
	}

	// if it's set to a non pin irq state, don't attach a callback
	if ( ( edge_select == EXT_INT_DISABLED ) ||
		 ( edge_select == EXT_INT_DMA_RISING_EDGE_TRIG ) ||
		 ( edge_select == EXT_INT_DMA_FALLING_EDGE_TRIG ) ||
		 ( edge_select == EXT_INT_DMA_CHANGE_TRIG ) )
	{
		// no need to do anything here
	}
	else
	{
		switch ( m_ext_int_ctrl->irq_num )
		{
			case PORTA_IRQn:
				m_ext_int_vect_a[m_ext_pio_ctrl->pio_num] = ext_int_callback;
				break;

			case PORTB_IRQn:
				m_ext_int_vect_b[m_ext_pio_ctrl->pio_num] = ext_int_callback;
				break;

			case PORTC_IRQn:
				m_ext_int_vect_c[m_ext_pio_ctrl->pio_num] = ext_int_callback;
				break;

			case PORTD_IRQn:
				m_ext_int_vect_d[m_ext_pio_ctrl->pio_num] = ext_int_callback;
				break;

			case PORTE_IRQn:
				m_ext_int_vect_e[m_ext_pio_ctrl->pio_num] = ext_int_callback;
				break;

			default:
				BF_ASSERT( false );
				break;
		}
	}



	// Configuring does not automatically enable the interrupt pin specifically.
	// It will, however, enable the interrupt vector.
	// Set pin to disabled state to start with.
	uC_IO_Config::Set_Pin_Interrupt_State( m_ext_pio_ctrl, EXT_INT_DISABLED );
	uC_IO_Config::Set_Pin_To_Input( m_ext_pio_ctrl, input_cfg );
	m_edge_select = edge_select;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Enable_Int( void )const
{
	uC_IO_Config::Set_Pin_Interrupt_State( m_ext_pio_ctrl, m_edge_select );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Disable_Int( void )const
{
	uC_IO_Config::Set_Pin_Interrupt_State( m_ext_pio_ctrl, EXT_INT_DISABLED );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::Clear_Int( void ) const
{
	uC_Interrupt::Clear_Int( m_ext_int_ctrl->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::External_Int_Vect_A( void )
{
	volatile uint32_t interrupts_detected = 0;
	volatile uint32_t interrupt_counter = 0;

	// read in the interrupts enabled for this port that have fired
	interrupts_detected = m_activated_inputs[uC_PIO_INDEX_A] & PORTA->ISFR;
	// clear their flags before moving on
	PORTA->ISFR = interrupts_detected;

	interrupt_counter = 0;

	// count through the interrupts until we've processed them all
	while ( ( interrupts_detected != 0 ) && ( interrupt_counter < uC_EXT_INT_VECT_COUNT ) )
	{
		// if the current interrupt is one that fired, call its callback if it's not NULL
		if ( ( interrupts_detected & 0x1 ) != 0 )
		{
			if ( m_ext_int_vect_a[interrupt_counter] != NULL )
			{
				( *m_ext_int_vect_a[interrupt_counter] )();
			}
		}
		interrupts_detected = interrupts_detected >> 1;
		interrupt_counter++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::External_Int_Vect_B( void )
{
	volatile uint32_t interrupts_detected = 0;
	volatile uint32_t interrupt_counter = 0;

	// read in the interrupts enabled for this port that have fired
	interrupts_detected = m_activated_inputs[uC_PIO_INDEX_B] & PORTB->ISFR;
	// clear their flags before moving on
	PORTB->ISFR = interrupts_detected;

	interrupt_counter = 0;

	// count through the interrupts until we've processed them all
	while ( ( interrupts_detected != 0 ) && ( interrupt_counter < uC_EXT_INT_VECT_COUNT ) )
	{
		// if the current interrupt is one that fired, call its callback if it's not NULL
		if ( ( interrupts_detected & 0x1 ) != 0 )
		{
			if ( m_ext_int_vect_b[interrupt_counter] != NULL )
			{
				( *m_ext_int_vect_b[interrupt_counter] )();
			}
		}
		interrupts_detected = interrupts_detected >> 1;
		interrupt_counter++;
	}

	/* DDB HACK */
	// s_phy_interrupt_handler();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Ext_Int::External_Int_Vect_C( void )
{
	volatile uint32_t interrupts_detected = 0;
	volatile uint32_t interrupt_counter = 0;

	// read in the interrupts enabled for this port that have fired
	interrupts_detected = m_activated_inputs[uC_PIO_INDEX_C] & PORTC->ISFR;
	// clear their flags before moving on
	PORTC->ISFR = interrupts_detected;

	interrupt_counter = 0;

	// count through the interrupts until we've processed them all
	while ( ( interrupts_detected != 0 ) && ( interrupt_counter < uC_EXT_INT_VECT_COUNT ) )
	{
		// if the current interrupt is one that fired, call its callback if it's not NULL
		if ( ( interrupts_detected & 0x1 ) != 0 )
		{
			if ( m_ext_int_vect_c[interrupt_counter] != NULL )
			{
				( *m_ext_int_vect_c[interrupt_counter] )();
			}
		}
		interrupts_detected = interrupts_detected >> 1;
		interrupt_counter++;
	}
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
void uC_Ext_Int::External_Int_Vect_D( void )
{
	volatile uint32_t interrupts_detected = 0;
	volatile uint32_t interrupt_counter = 0;

	// read in the interrupts enabled for this port that have fired
	interrupts_detected = m_activated_inputs[uC_PIO_INDEX_D] & PORTD->ISFR;
	// clear their flags before moving on
	PORTD->ISFR = interrupts_detected;

	interrupt_counter = 0;

	// count through the interrupts until we've processed them all
	while ( ( interrupts_detected != 0 ) && ( interrupt_counter < uC_EXT_INT_VECT_COUNT ) )
	{
		// if the current interrupt is one that fired, call its callback if it's not NULL
		if ( ( interrupts_detected & 0x1 ) != 0 )
		{
			if ( m_ext_int_vect_d[interrupt_counter] != NULL )
			{
				( *m_ext_int_vect_d[interrupt_counter] )();
			}
		}
		interrupts_detected = interrupts_detected >> 1;
		interrupt_counter++;
	}
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
void uC_Ext_Int::External_Int_Vect_E( void )
{
	uint32_t interrupts_detected = 0;
	volatile uint32_t interrupt_counter = 0;

	// read in the interrupts enabled for this port that have fired
	interrupts_detected = m_activated_inputs[uC_PIO_INDEX_E] & PORTE->ISFR;
	// clear their flags before moving on
	PORTE->ISFR |= interrupts_detected;

	interrupt_counter = 0;

	// count through the interrupts until we've processed them all
	while ( ( interrupts_detected != 0 ) && ( interrupt_counter < uC_EXT_INT_VECT_COUNT ) )
	{
		// if the current interrupt is one that fired, call its callback if it's not NULL
		if ( ( interrupts_detected & 0x1 ) != 0 )
		{
			if ( m_ext_int_vect_e[interrupt_counter] != NULL )
			{
				( *m_ext_int_vect_e[interrupt_counter] )();
			}
		}
		interrupts_detected = interrupts_detected >> 1;
		interrupt_counter++;
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
/* lint +e1924
   lint +e1960*/
