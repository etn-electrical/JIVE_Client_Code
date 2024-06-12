/**
 *****************************************************************************************
 *	@file		uC_Single_Timer.cpp
 *	@details	See header file for module overview.
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_Single_Timer.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const char* TIMER_TAG = "timer_group";	///< TIMER_TAG tag used for displaying error message
// Error messages used for timers
static const char* TIMER_GROUP_NUM_ERROR = "TIMER GROUP NUM ERROR";
static const char* TIMER_NUM_ERROR = "HW TIMER NUM ERROR";
static const char* TIMER_PARAM_ADDR_ERROR = "HW TIMER PARAM ADDR ERROR";
static const char* TIMER_COUNT_DIR_ERROR = "HW TIMER COUNTER DIR ERROR";
static const char* TIMER_AUTORELOAD_ERROR = "HW TIMER AUTORELOAD ERROR";
static const char* DIVIDER_RANGE_ERROR = "HW TIMER divider outside of [2, 65536] range error";

// Timer Registers
static const uint8_t TIMERS_REG0 = 0;
static const uint8_t TIMERS_REG1 = 1;

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static timg_dev_t* TG[2] = { &TIMERG0, &TIMERG1 };	///< TIMER_TAG tag used for displaying error message
static portMUX_TYPE timer_spinlock[TIMER_GROUP_MAX] =
{ portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED };	///< port mux type for timers

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Single_Timer::uC_Single_Timer( uint8_t timer_num, uC_BASE_TIMER_IO_STRUCT const* timer_io )
{
	m_tmr_ctrl = nullptr;
	if ( timer_io == NULL )
	{
		m_tmr_ctrl = &( uC_Base::m_timer_io[timer_num] );
	}
	else
	{
		m_tmr_ctrl = timer_io;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Set_Callback( cback_func_t func_callback,
									cback_param_t func_param, uint8_t int_priority )
{
	interrupt_config_t* interrupt_config = reinterpret_cast<interrupt_config_t*>( func_param );

	Check_Timer( m_tmr_ctrl->group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( m_tmr_ctrl->timer_idx < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( interrupt_config->handler != NULL, TIMER_PARAM_ADDR_ERROR, ESP_ERR_INVALID_ARG );

	switch ( m_tmr_ctrl->group_num )
	{
		case TIMER_GROUP_0:
		default:
			if ( ( interrupt_config->priority & ESP_INTR_FLAG_EDGE ) == 0 )
			{
				interrupt_config->source = ETS_TG0_T0_LEVEL_INTR_SOURCE + m_tmr_ctrl->timer_idx;
			}
			else
			{
				interrupt_config->source = ETS_TG0_T0_EDGE_INTR_SOURCE + m_tmr_ctrl->timer_idx;
			}
			interrupt_config->intrstatusreg = TIMG_INT_ST_TIMERS_REG( 0 );
			interrupt_config->intrstatusmask = 1 << m_tmr_ctrl->timer_idx;
			break;

		case TIMER_GROUP_1:
			if ( ( interrupt_config->priority & ESP_INTR_FLAG_EDGE ) == 0 )
			{
				interrupt_config->source = ETS_TG1_T0_LEVEL_INTR_SOURCE + m_tmr_ctrl->timer_idx;
			}
			else
			{
				interrupt_config->source = ETS_TG1_T0_EDGE_INTR_SOURCE + m_tmr_ctrl->timer_idx;
			}
			interrupt_config->intrstatusreg = TIMG_INT_ST_TIMERS_REG( 1 );
			interrupt_config->intrstatusmask = 1 << m_tmr_ctrl->timer_idx;
			break;
	}
	uC_Interrupt::Set_Vector( reinterpret_cast<INT_CALLBACK_FUNC*>( interrupt_config ), MAX_IRQTYPE,
							  int_priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Single_Timer::Set_Timeout( uint32_t desired_frequency, bool repeat )
{
	int32_t timer_en = 0;

	Check_Timer( m_tmr_ctrl->group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( m_tmr_ctrl->timer_idx < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( m_tmr_ctrl->counter_dir < TIMER_COUNT_MAX, TIMER_COUNT_DIR_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( ( m_tmr_ctrl->divider > 1 && m_tmr_ctrl->divider < 65537 ), DIVIDER_RANGE_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( m_tmr_ctrl->auto_reload < TIMER_AUTORELOAD_MAX, TIMER_AUTORELOAD_ERROR, ESP_ERR_INVALID_ARG );

	portENTER_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.increase = m_tmr_ctrl->counter_dir;
	timer_en = m_tmr_ctrl->counter_en;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.enable = 0;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.divider = ( uint16_t ) m_tmr_ctrl->divider;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.enable = timer_en;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.autoreload = ( uint32_t ) m_tmr_ctrl->auto_reload;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].load_high = ( uint32_t ) ( m_tmr_ctrl->load_val >> 32 );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].load_low = ( uint32_t ) m_tmr_ctrl->load_val;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].reload = 1;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].alarm_high =
		( uint32_t ) ( m_tmr_ctrl->alarm_value >> 32 );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].alarm_low = ( uint32_t ) m_tmr_ctrl->alarm_value;
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.alarm_en = m_tmr_ctrl->alarm_en;
	TG[m_tmr_ctrl->group_num]->int_ena.val |= BIT( m_tmr_ctrl->timer_idx );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.level_int_en =
		( m_tmr_ctrl->intr_type == TIMER_INTR_LEVEL ? 1 : 0 );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.edge_int_en =
		( m_tmr_ctrl->intr_type == TIMER_INTR_LEVEL ? 0 : 1 );
	portEXIT_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
	return ( 1 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Start_Timeout( void )
{
	Check_Timer( m_tmr_ctrl->group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( m_tmr_ctrl->timer_idx < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG );

	portENTER_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.enable = 1;
	TG[m_tmr_ctrl->group_num]->int_ena.val |= BIT( m_tmr_ctrl->timer_idx );
	portEXIT_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Restart_Timeout( void )
{
	uint32_t disable_mask;

	Check_Timer( m_tmr_ctrl->group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( m_tmr_ctrl->timer_idx < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG );

	portENTER_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.enable = 0;
	disable_mask = BIT( m_tmr_ctrl->timer_idx );
	TG[m_tmr_ctrl->group_num]->int_ena.val &= ( ~disable_mask );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].reload = 1;
	TG[m_tmr_ctrl->group_num]->int_ena.val |= BIT( m_tmr_ctrl->timer_idx );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.enable = 1;
	portEXIT_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Stop_Timeout( void )
{
	uint32_t disable_mask;

	Check_Timer( m_tmr_ctrl->group_num < TIMER_GROUP_MAX, TIMER_GROUP_NUM_ERROR, ESP_ERR_INVALID_ARG );
	Check_Timer( m_tmr_ctrl->timer_idx < TIMER_MAX, TIMER_NUM_ERROR, ESP_ERR_INVALID_ARG );

	portENTER_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
	disable_mask = BIT( m_tmr_ctrl->timer_idx );
	TG[m_tmr_ctrl->group_num]->int_ena.val &= ( ~disable_mask );
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.enable = 0;
	portEXIT_CRITICAL( &timer_spinlock[m_tmr_ctrl->group_num] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Single_Timer::Timeout_Running( void ) const
{
	return ( TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.enable );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Single_Timer::Reload_Timer( void )
{
	uint64_t timer_counter_value = 0;
	// Retrieve the interrupt status and the counter value
	// from the timer that reported the interrupt.
	uint32_t intr_status = TG[m_tmr_ctrl->group_num]->int_st_timers.val;

	if ( TIMER_AUTORELOAD_DIS == m_tmr_ctrl->auto_reload )
	{
		TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].update = 1;

		timer_counter_value =
			( ( uint64_t ) TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].cnt_high ) << 32
				| TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].cnt_low;

		timer_counter_value += ( uint64_t ) ( m_tmr_ctrl->alarm_value );
		TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].alarm_high =
			( uint32_t ) ( timer_counter_value >> 32 );
		TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].alarm_low = ( uint32_t ) timer_counter_value;
	}

	// Clear the interrupt and update the alarm time for the timer with without reload.
	if ( ( intr_status & BIT( m_tmr_ctrl->timer_idx ) ) && ( m_tmr_ctrl->timer_idx == TIMER_0 ) )
	{
		TG[m_tmr_ctrl->group_num]->int_clr_timers.t0 = 1;
	}
	else if ( ( intr_status & BIT( m_tmr_ctrl->timer_idx ) ) && ( m_tmr_ctrl->timer_idx == TIMER_1 ) )
	{
		TG[m_tmr_ctrl->group_num]->int_clr_timers.t1 = 1;
	}

	// After the alarm has been triggered we need enable
	// it again, so it is triggered the next time.
	TG[m_tmr_ctrl->group_num]->hw_timer[m_tmr_ctrl->timer_idx].config.alarm_en = TIMER_ALARM_EN;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_Single_Timer::Check_Timer( bool validation_result, const char* str, esp_err_t error_type )
{
	if ( !( validation_result ) )
	{
		ESP_LOGE( TIMER_TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str );
		return ( error_type );
	}
	return ( ESP_OK );
}
