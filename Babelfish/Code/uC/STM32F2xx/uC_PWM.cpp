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
#include "uC_PWM.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "Timers_Lib.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MAX_PWM_TICKS           0xFFFFU

// This turns the period into a percentage of full range.
#define PWM_DUTY_CYCLE_PERCENT_SHIFT        16U

// This is a minimum duty cycle setting.
#define MIN_PWM_DUTY_CYCLE_TICKS            1U

#define PWM_MODE_ENABLED 0x3U

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
uC_PWM::uC_PWM( uint8_t pwm_ctrl_block, uint8_t pwm_channel,
				uC_PERIPH_IO_STRUCT const* port_io ) :
	m_pwm_ctrl( reinterpret_cast<uC_BASE_TIMER_IO_STRUCT const*>( nullptr ) ),
	m_pwm_ch( 0U ),
	m_def_state_mask( 0U ),
	m_compare_reg( reinterpret_cast<uint32_t volatile*>( nullptr ) ),
	m_percent_adj( 0U ),
	m_port_io( reinterpret_cast<uC_PERIPH_IO_STRUCT const*>( nullptr ) )
{
	m_pwm_ctrl = ( uC_Base::Self() )->Get_PWM_Ctrl( pwm_ctrl_block );

	BF_ASSERT( ( m_pwm_ctrl != 0U ) &&
			   ( pwm_channel < m_pwm_ctrl->num_compares ) );

	m_pwm_ch = pwm_channel;
	m_def_state_mask = 0U;

	if ( port_io == NULL )
	{
		port_io = m_pwm_ctrl->timer_pio[m_pwm_ch];
	}

	m_port_io = port_io;

	if ( !uC_Base::Is_Periph_Clock_Enabled( &m_pwm_ctrl->periph_def ) )
	{
		uC_Base::Reset_Periph( &m_pwm_ctrl->periph_def );
		uC_Base::Enable_Periph_Clock( &m_pwm_ctrl->periph_def );
	}

	// Because timer 1 is special we have to configure the dead band time output control.
	// For timer 8 as well needs BDTR to update to get PWM w/f
	if ( ( pwm_ctrl_block == static_cast<uint8_t>( uC_BASE_PWM_CTRL_1 ) ) ||
		 ( pwm_ctrl_block == static_cast<uint8_t>( uC_BASE_PWM_CTRL_8 ) ) )
	{
		m_pwm_ctrl->reg_ctrl->BDTR |= TIM_BDTR_MOE;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Turn_On( void )
{
	uint16_t temp_val;
	uint32_t volatile* temp_ccmr;
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );

	if ( m_pwm_ch < static_cast<uint8_t>( uC_BASE_TIMER_CAP_COMP_CH3 ) )
	{
		temp_ccmr = &m_pwm_ctrl->reg_ctrl->CCMR1;
	}
	else
	{
		temp_ccmr = &m_pwm_ctrl->reg_ctrl->CCMR2;
	}
	if ( Is_Number_Even( m_pwm_ch ) )
	{
		temp_val = static_cast<uint16_t>
			( *temp_ccmr & ~( static_cast<uint32_t>( TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE ) ) );
		temp_val = temp_val | ( TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE );
		temp_val |= static_cast<uint16_t>( *temp_ccmr & 0xFF00U );
	}
	else
	{
		temp_val = static_cast<uint16_t>
			( *temp_ccmr &
			  ( ~( static_cast<uint32_t>
				   ( TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE ) << MULT_BY_256 ) ) );
		temp_val = temp_val | static_cast<uint16_t>
			( ( ( static_cast<uint16_t>
				  ( TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE ) ) << MULT_BY_256 ) );
		temp_val |= static_cast<uint16_t>( *temp_ccmr & 0x00FFU );
	}

	*temp_ccmr = temp_val;

	Pop_GINT( temp_gint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Turn_Off( void )
{
	GINT_TDEF temp_gint;
	uint32_t volatile* temp_ccmr;
	uint16_t temp_val;

	Push_GINT( temp_gint );

	if ( m_pwm_ch < static_cast<uint8_t>( uC_BASE_TIMER_CAP_COMP_CH3 ) )
	{
		temp_ccmr = &m_pwm_ctrl->reg_ctrl->CCMR1;
	}
	else
	{
		temp_ccmr = &m_pwm_ctrl->reg_ctrl->CCMR2;
	}
	if ( Is_Number_Even( m_pwm_ch ) )
	{
		temp_val = static_cast<uint16_t>
			( *temp_ccmr & ~static_cast<uint32_t>( TIM_CCMR1_OC1M ) );
	}
	else
	{
		temp_val = static_cast<uint16_t>
			( *temp_ccmr & ( ~( static_cast<uint32_t>( TIM_CCMR1_OC1M ) << MULT_BY_256 ) ) );
	}

	temp_val |= m_def_state_mask;

	*temp_ccmr = temp_val;

	Pop_GINT( temp_gint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_PWM::Is_On( void )
{
	GINT_TDEF temp_gint;
	uint32_t volatile* temp_ccmr;
	uint16_t temp_val;
	bool status = FALSE;

	Push_GINT( temp_gint );

	if ( m_pwm_ch < static_cast<uint8_t>( uC_BASE_TIMER_CAP_COMP_CH3 ) )
	{
		temp_ccmr = &m_pwm_ctrl->reg_ctrl->CCMR1;
	}
	else
	{
		temp_ccmr = &m_pwm_ctrl->reg_ctrl->CCMR2;
	}
	if ( Is_Number_Even( m_pwm_ch ) )
	{
		temp_val = static_cast<uint16_t>( *temp_ccmr & TIM_CCMR1_OC1M );
	}
	else
	{
		temp_val = static_cast<uint16_t>
			( *temp_ccmr & ( static_cast<uint32_t>( TIM_CCMR1_OC1M ) << MULT_BY_256 ) );
		temp_val = temp_val >> DIV_BY_256;
	}

	temp_val = temp_val >> DIV_BY_32;		// SHIFT BY 5 BITS

	Pop_GINT( temp_gint );

	if ( temp_val == PWM_MODE_ENABLED )		// 0C1M bits ( 2:0 bits , consider 2:1  bits should be 3
											// )
	{
		status = true;

	}
	else
	{
		status = false;
	}

	return  ( status );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Configure( uint8_t polarity, uint8_t alignment,
						uint32_t desired_freq, bool default_state )
{
	uint16_t temp_cr1;

	BF_Lib::SPLIT_UINT32 temp_calc;
	uint32_t prescaler;
	uint32_t count;
	uint32_t period_ticks;
	uint32_t master_clock_freq;
	GINT_TDEF temp_gint;

	Push_GINT( temp_gint );

	master_clock_freq = ( uC_Base::Self() )->Get_PClock_Freq( m_pwm_ctrl->pclock_freq_index );

	m_pwm_ctrl->reg_ctrl->CR1 &= ~TIM_CR1_CEN;	// Have to turn off the timer to configure some of
												// this stuff.

	temp_cr1 = TIM_CR1_CEN | TIM_CR1_ARPE;

	/// Setup the Polarity and IO.
	if ( polarity == static_cast<uint8_t>( uC_PWM_DUTY_CYCLE_HIGH ) )
	{
		m_pwm_ctrl->reg_ctrl->CCER &=
			~( static_cast<uint32_t>
			   ( TIM_CCER_CC1P ) << ( static_cast<uint32_t>( m_pwm_ch ) << MULT_BY_8 ) );
		if ( default_state == false )
		{
			m_def_state_mask = TIM_CCMR1_OC1M_2;
		}
		else
		{
			m_def_state_mask = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0;
		}
	}
	else
	{
		m_pwm_ctrl->reg_ctrl->CCER |=
			( static_cast<uint32_t>
			  ( TIM_CCER_CC1P ) << ( static_cast<uint32_t>( m_pwm_ch ) << MULT_BY_8 ) );
		// If we have a negative polarity then this needs to be opposite.
		if ( default_state == true )
		{
			m_def_state_mask = TIM_CCMR1_OC1M_2;
		}
		else
		{
			m_def_state_mask = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0;
		}
	}

	if ( Is_Number_Odd( m_pwm_ch ) )
	{
		m_def_state_mask = m_def_state_mask << MULT_BY_256;
	}
	Turn_Off();		// This should get the states right for the bumpless switch-over from gen IO to
					// periph.

	m_pwm_ctrl->reg_ctrl->CCER |=
		( static_cast<uint32_t>( TIM_CCER_CC1E ) << ( m_pwm_ch * 4U ) );

	uC_IO_Config::Enable_Periph_Output_Pin( m_port_io, default_state );

	/// Calculate the running frequency.
	if ( alignment == static_cast<uint8_t>( uC_PWM_CENTER_ALIGNED ) )
	{
		temp_cr1 |= TIM_CR1_CMS_0;	// This turns it into center aligned.
		master_clock_freq = master_clock_freq / 2U;
	}

	temp_calc.u32 = Round_Div( master_clock_freq, desired_freq );
	prescaler = temp_calc.u16[1];
	count = Round_Div( master_clock_freq, ( prescaler + 1U ) ) / desired_freq;

	m_pwm_ctrl->reg_ctrl->CNT = count;
	m_pwm_ctrl->reg_ctrl->ARR = count;
	m_pwm_ctrl->reg_ctrl->PSC = prescaler;

	// This gets the correct compare register.
	switch ( m_pwm_ch )
	{
		case uC_BASE_TIMER_CAP_COMP_CH1:
			m_compare_reg = &m_pwm_ctrl->reg_ctrl->CCR1;
			break;

		case uC_BASE_TIMER_CAP_COMP_CH2:
			m_compare_reg = &m_pwm_ctrl->reg_ctrl->CCR2;
			break;

		case uC_BASE_TIMER_CAP_COMP_CH3:
			m_compare_reg = &m_pwm_ctrl->reg_ctrl->CCR3;
			break;

		case uC_BASE_TIMER_CAP_COMP_CH4:
			m_compare_reg = &m_pwm_ctrl->reg_ctrl->CCR4;
			break;

		default:
			m_compare_reg = &m_pwm_ctrl->reg_ctrl->CCR1;
			break;
	}

	/// This calculates the percent adjustment value.
	period_ticks = ( master_clock_freq / ( prescaler + 1U ) );
	period_ticks = period_ticks / desired_freq;

	m_percent_adj = ( period_ticks << PWM_DUTY_CYCLE_PERCENT_SHIFT );
	m_percent_adj = m_percent_adj / MAX_PWM_TICKS;

	*m_compare_reg = MIN_PWM_DUTY_CYCLE_TICKS;

	m_pwm_ctrl->reg_ctrl->CR1 = temp_cr1;		// The clock is running after this write.

	Pop_GINT( temp_gint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Set_Duty_Cycle_Pct( uint16_t percent )
{
	uint32_t ticks_required;

	ticks_required =
		( static_cast<uint32_t>( percent ) * m_percent_adj ) >> PWM_DUTY_CYCLE_PERCENT_SHIFT;			// PWM_DUTY_CYCLE_PERCENT_CALC;

	if ( ticks_required == 0U )
	{
		ticks_required = MIN_PWM_DUTY_CYCLE_TICKS;
	}

	*m_compare_reg = ticks_required;
}

/* lint +e1924
  lint +e1960*/
