/**
 *****************************************************************************************
 *	@file		uC_PWM.cpp Implementation File for PWM functionality
 *	@details    See header file for module overview.
 *	@copyright  2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "uC_PWM.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "soc/mcpwm_struct.h"
#include "driver/mcpwm.h"
#include "driver/gpio.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"
#include "freertos/portmacro.h"
#include "driver/periph_ctrl.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint8_t MCPWM_CLK_PRESCL = 15U;				///< MCPWM clock prescale
static const uint8_t TIMER_CLK_PRESCALE = 9U;				///< MCPWM timer prescales
static const uint8_t PWM_TURN_ON = 2U;
static const uint8_t PWM_TURN_OFF = 0U;
static const uint8_t SET_ZERO = 0U;
static const uint8_t SET_ONE = 1U;
static const uint8_t SET_TWO = 2U;
static const uint8_t HUNDRED = 100U;

#define MCPWM_BASE_CLK ( 2 * APB_CLK_FREQ )					///< 2*APB_CLK_FREQ 160Mhz
#define MCPWM_CLK ( MCPWM_BASE_CLK / ( MCPWM_CLK_PRESCL + 1 ) )

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
static mcpwm_dev_t* MCPWM[2] = {&MCPWM0, &MCPWM1};			///< MCPWM global structure
static portMUX_TYPE mcpwm_spinlock = portMUX_INITIALIZER_UNLOCKED;	///< Spin Lock for MCPWM
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_PWM::uC_PWM( uint8_t pwm_ctrl_block, uint8_t pwm_channel,
				uC_PERIPH_IO_STRUCT const* port_io ) :
	m_pwm_ctrl( reinterpret_cast<uC_BASE_PWM_STRUCT*>( nullptr ) )
{
	gpio_num_t gpio_num;

	m_pwm_ctrl = &uC_Base::m_pwm_ctrls[pwm_ctrl_block];


	if ( ( m_pwm_ctrl != 0U ) &&
		 ( pwm_channel < uC_BASE_PWM_MAX_CHANNELS ) )
	{
		m_pwm_ctrl->pwm_ctrl_block = pwm_ctrl_block;
		m_pwm_ctrl->pwm_channel = pwm_channel;
		m_pwm_ctrl->pwm_timer_no = Timer_Select( pwm_channel );
		m_pwm_ctrl->pwm_op = Operating_Channel_Select( pwm_channel );

		if ( port_io != NULL )
		{
			m_pwm_ctrl->pwm_pio[m_pwm_ctrl->pwm_channel - 1] = port_io;
		}

		gpio_num = m_pwm_ctrl->pwm_pio[m_pwm_ctrl->pwm_channel - 1]->io_num;

		uC_Base::Periph_Module_Enable( ( periph_module_t ) ( PERIPH_PWM0_MODULE + m_pwm_ctrl->pwm_ctrl_block ) );

		portENTER_CRITICAL( &mcpwm_spinlock );

		MCPWM[m_pwm_ctrl->pwm_ctrl_block]->clk_cfg.prescale = MCPWM_CLK_PRESCL;
		MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.mode = MCPWM_UP_COUNTER;

		portEXIT_CRITICAL( &mcpwm_spinlock );

		// PWM  GPIO initialization each channels

		PIN_FUNC_SELECT( GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO );

		if ( m_pwm_ctrl->pwm_ctrl_block == MCPWM_UNIT_0 )
		{

			uC_IO_Config::Set_GPIO_Direction( gpio_num,
											  GPIO_MODE_OUTPUT );

			gpio_matrix_out( ( uint32_t ) m_pwm_ctrl->pwm_pio[m_pwm_ctrl->pwm_channel - 1]->io_num,
							 PWM0_OUT0A_IDX + ( m_pwm_ctrl->pwm_channel - 1 ), SET_ZERO, SET_ZERO );

		}
		else if ( m_pwm_ctrl->pwm_ctrl_block == MCPWM_UNIT_1 )
		{
			uC_IO_Config::Set_GPIO_Direction( gpio_num,
											  GPIO_MODE_OUTPUT );

			gpio_matrix_out( ( uint32_t ) m_pwm_ctrl->pwm_pio[m_pwm_ctrl->pwm_channel - 1]->io_num,
							 PWM1_OUT0A_IDX + ( m_pwm_ctrl->pwm_channel - 1 ), SET_ZERO, SET_ZERO );

		}

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Turn_On( void )
{
	portENTER_CRITICAL( &mcpwm_spinlock );
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.start = PWM_TURN_ON;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer_sel.operator0_sel = SET_ZERO;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer_sel.operator1_sel = SET_ONE;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer_sel.operator2_sel = SET_TWO;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->update_cfg.global_up_en = SET_ONE;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->update_cfg.global_force_up = SET_ONE;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->update_cfg.global_force_up = SET_ZERO;
	portEXIT_CRITICAL( &mcpwm_spinlock );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Turn_Off( void )
{
	portENTER_CRITICAL( &mcpwm_spinlock );
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.start = PWM_TURN_OFF;
	portEXIT_CRITICAL( &mcpwm_spinlock );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_PWM::Is_On( void )
{
	uint32_t pwm_start_status;
	bool pwm_status = false;

	pwm_start_status = MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.start;

	if ( pwm_start_status == PWM_TURN_ON )
	{
		pwm_status = true;
	}
	return ( pwm_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Configure( uint8_t polarity, uint8_t counter_mode,
						uint32_t desired_freq, bool default_state )
{

	uint32_t mcpwm_num_of_pulse;
	uint32_t previous_period;
	uint32_t set_duty_a, set_duty_b;


	mcpwm_num_of_pulse = MCPWM_CLK / ( desired_freq * ( TIMER_CLK_PRESCALE + 1 ) );
	previous_period = MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].period.period;
	set_duty_a =
		( ( ( MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_value[0].cmpr_val ) *
			mcpwm_num_of_pulse ) / previous_period );
	set_duty_b =
		( ( ( MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_value[1].cmpr_val ) *
			mcpwm_num_of_pulse ) / previous_period );

	portENTER_CRITICAL( &mcpwm_spinlock );
	// Set PWM Frequency
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].period.prescale = TIMER_CLK_PRESCALE;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].period.period = mcpwm_num_of_pulse;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].period.upmethod = SET_ZERO;

	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_value[0].cmpr_val = set_duty_a;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_value[1].cmpr_val = set_duty_b;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_cfg.a_upmethod = SET_ZERO;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_cfg.b_upmethod = SET_ZERO;

	// Set PWM duty mode
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.mode = counter_mode;

	if ( m_pwm_ctrl->pwm_op == MCPWM_OPR_A )
	{
		if ( MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.mode == MCPWM_UP_COUNTER )
		{
			if ( polarity == MCPWM_DUTY_MODE_1 )
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utea
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utep
					=
						SET_ZERO;
			}
			else			// MCPWM_DUTY_MODE_0
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utea
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utep
					=
						SET_ZERO;
			}
		}
		else if ( MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.mode == MCPWM_DOWN_COUNTER )
		{
			if ( polarity == MCPWM_DUTY_MODE_1 )
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtep
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtea
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtez
					=
						SET_ZERO;
			}
			else			// MCPWM_DUTY_MODE_0
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtep
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtea
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtez
					=
						SET_ZERO;
			}
		}
		else			// Timer count up-down
		{
			if ( polarity == MCPWM_DUTY_MODE_1 )
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utea
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtea
					=
						SET_ONE;
			}
			else			// MCPWM_DUTY_MODE_0
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utea
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtea
					=
						SET_TWO;
			}
		}
	}
	if ( m_pwm_ctrl->pwm_op == MCPWM_OPR_B )
	{
		if ( MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.mode == MCPWM_UP_COUNTER )
		{
			if ( polarity == MCPWM_DUTY_MODE_1 )
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].uteb
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utep
					=
						SET_ZERO;
			}
			else			// MCPWM_DUTY_MODE_0
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].uteb
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utep
					=
						SET_ZERO;
			}
		}
		else if ( MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].mode.mode == MCPWM_DOWN_COUNTER )
		{
			if ( polarity == MCPWM_DUTY_MODE_1 )
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtep
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dteb
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtez
					=
						SET_ZERO;
			}
			else			// MCPWM_DUTY_MODE_0
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtep
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dteb
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dtez
					=
						SET_ZERO;
			}
		}
		else			// Timer count up-down
		{
			if ( polarity == MCPWM_DUTY_MODE_1 )
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].uteb
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dteb
					=
						SET_ONE;
			}
			else			// MCPWM_DUTY_MODE_0
			{
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].utez
					=
						SET_TWO;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].uteb
					=
						SET_ONE;
				MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].generator[m_pwm_ctrl->pwm_op].dteb
					=
						SET_TWO;
			}
		}
	}

	portEXIT_CRITICAL( &mcpwm_spinlock );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_PWM::Set_Duty_Cycle_Pct( float32_t duty_percent )
{

	uint32_t set_duty;

	portENTER_CRITICAL( &mcpwm_spinlock );

	set_duty = ( MCPWM[m_pwm_ctrl->pwm_ctrl_block]->timer[m_pwm_ctrl->pwm_timer_no].period.period ) * ( duty_percent ) /
		HUNDRED;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_value[m_pwm_ctrl->pwm_op].cmpr_val =
		set_duty;
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_cfg.a_upmethod = BIT( 0 );
	MCPWM[m_pwm_ctrl->pwm_ctrl_block]->channel[m_pwm_ctrl->pwm_timer_no].cmpr_cfg.b_upmethod = BIT( 0 );

	portEXIT_CRITICAL( &mcpwm_spinlock );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_PWM::Timer_Select( uint8_t pwm_channel )
{
	uint8_t timer_no = MCPWM_TIMER_0;

	switch ( pwm_channel )
	{
		case uC_BASE_PWM_CHANNEL_1:
		case uC_BASE_PWM_CHANNEL_2:
			timer_no = MCPWM_TIMER_0;
			break;

		case uC_BASE_PWM_CHANNEL_3:
		case uC_BASE_PWM_CHANNEL_4:
			timer_no = MCPWM_TIMER_1;
			break;

		case uC_BASE_PWM_CHANNEL_5:
		case uC_BASE_PWM_CHANNEL_6:
			timer_no = MCPWM_TIMER_2;
			break;

		default:
			timer_no = MCPWM_TIMER_0;
			break;
	}

	return ( timer_no );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_PWM::Operating_Channel_Select( uint8_t pwm_channel )
{

	uint8_t op_channel = MCPWM_OPR_A;

	switch ( pwm_channel )
	{
		case uC_BASE_PWM_CHANNEL_1:
		case uC_BASE_PWM_CHANNEL_3:
		case uC_BASE_PWM_CHANNEL_5:
			op_channel = MCPWM_OPR_A;
			break;

		case uC_BASE_PWM_CHANNEL_2:
		case uC_BASE_PWM_CHANNEL_4:
		case uC_BASE_PWM_CHANNEL_6:
			op_channel = MCPWM_OPR_B;
			break;

		default:
			op_channel = MCPWM_OPR_A;
			break;
	}

	return ( op_channel );
}
