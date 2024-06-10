/**
 **********************************************************************************************
 * @file			uC_AtoD.cpp
 * @brief			The file contains definitions of all the functions required for
 * @n				AtoD conversion and which are declared in corresponding header file
 * @details			See header file for module overview.
 * @copyright		2019 Eaton Corporation. All Rights Reserved.
 **********************************************************************************************
 */

#include "uC_AtoD.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "freertos/portmacro.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "soc/rtc_io_reg.h"
#include "sys/lock.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/rtc_io_struct.h"
#include "soc/syscon_struct.h"
#include "soc/sens_reg.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const char* RTC_MODULE_TAG = "RTC_MODULE";

/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static _lock_t adc1_i2s_lock;	///< prevent ADC1 being used by I2S dma and other tasks at the same time.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_AtoD::uC_AtoD( uC_ATOD_RESOLUTION_ENUM resolution, uint32_t active_channels_mask,
				  uC_BASE_ATOD_CTRL_ENUM atod_block, uint16_t time_reqd_to_scan_all_channels_us ) :
	m_adc( static_cast<uC_BASE_ATOD_IO_STRUCT const*>( nullptr ) ),
	m_resolution( 0U ),
	m_channel_buffer( static_cast<uint16_t*>( nullptr ) ),
	m_channel_lookup( static_cast<uint8_t*>( nullptr ) )
{
	gpio_num_t gpio_num;

	m_adc = &uC_Base::m_atod_io[atod_block];
	m_active_channel = active_channels_mask;

	if ( ( m_adc != DISABLE_VALUE ) && ( m_active_channel != DISABLE_VALUE ) )
	{
		Check_RTC_Module( resolution < ADC_RESOLUTION_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG );
		Check_RTC_Module( m_adc->max_channels < ADC_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG );
		Check_RTC_Module( m_adc->atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG );

		gpio_num = gpio_num_t( DISABLE_VALUE );
		if ( atod_block == uC_BASE_ATOD_CTRL_1 )
		{
			SENS.sar_start_force.sar1_bit_width = resolution;
			SENS.sar_read_ctrl.sar1_sample_bit = resolution;

			// Enable ADC data invert
			SENS.sar_read_ctrl.sar1_data_inv = true;

			// Configuring ADC input output pins
			Check_RTC_Module( ( adc1_channel_t ) m_active_channel < ADC1_CHANNEL_MAX, "ADC1 channel error",
							  ESP_ERR_INVALID_ARG );
			Check_ADC( Get_ADC1_IO_Number( ( adc1_channel_t ) m_active_channel, &gpio_num ) );
			ADC_IO_Configuration( gpio_num );

			// Configuration of attenuation for ADC1
			SET_PERI_REG_BITS( SENS_SAR_ATTEN1_REG, SENS_SAR1_ATTEN_VAL_MASK, m_adc->atten,
							   ( m_active_channel * CHANNEL_VALUE2 ) );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Start_Scan( void ) const
{

	Check_RTC_Module( ( ( adc1_channel_t ) m_active_channel < ADC1_CHANNEL_MAX ), "ADC Channel Err",
					  ESP_ERR_INVALID_ARG );

	// lazy initialization for adc1, block until acquire the lock
	_lock_acquire( &adc1_i2s_lock );
	ESP_LOGD( RTC_MODULE_TAG, "adc mode takes adc1 lock." );

	/* For now the WiFi would use ADC2 and set xpd_sar force on.
	    so we can not reset xpd_sar to fsm mode directly.
	    We should handle this after the synchronization mechanism is established.
	    switch SARADC into RTC channel */
	SENS.sar_read_ctrl.sar1_dig_force = DISABLE_VALUE;

	// Set the power always on to increase precision.
	SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;

	// Disable other peripherals
	RTCIO.hall_sens.xpd_hall = false;

	/*	Currently the LNA is not open, close it by default
	    channel is set in the  convert function */
	SENS.sar_meas_wait2.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;

	// Disable FSM, it's only used by the LNA.
	SENS.sar_meas_ctrl.amp_rst_fb_fsm = DISABLE_VALUE;
	SENS.sar_meas_ctrl.amp_short_ref_fsm = DISABLE_VALUE;
	SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = DISABLE_VALUE;
	SENS.sar_meas_wait1.sar_amp_wait1 = ENABLE_VALUE;
	SENS.sar_meas_wait1.sar_amp_wait2 = ENABLE_VALUE;
	SENS.sar_meas_wait2.sar_amp_wait3 = ENABLE_VALUE;

	// set controller
	SENS.sar_read_ctrl.sar1_dig_force = false;			///< RTC controller controls the ADC, not digital controller
	SENS.sar_meas_start1.meas1_start_force = true;		///< RTC controller controls the ADC,not ulp coprocessor
	SENS.sar_meas_start1.sar1_en_pad_force = true;		///< RTC controller controls the data port,not ulp coprocessor
	SENS.sar_touch_ctrl1.xpd_hall_force = true;		///< RTC controller controls hall sensor power,not ulp coprocessor
	SENS.sar_touch_ctrl1.hall_phase_force = true;	///< RTC controller control hall sensor phase,not ulp coprocessor

	// start conversion
	SENS.sar_meas_start1.sar1_en_pad = ( 1 << m_active_channel );		///< Only one channel is selected.
	while ( SENS.sar_slave_addr1.meas_status != DISABLE_VALUE )
	{}
	SENS.sar_meas_start1.meas1_start_sar = DISABLE_VALUE;
	SENS.sar_meas_start1.meas1_start_sar = ENABLE_VALUE;
	while ( SENS.sar_meas_start1.meas1_done_sar == DISABLE_VALUE )
	{}

	Check_RTC_Module( ( uint32_t* )adc1_i2s_lock != NULL,
					  "adc1 lock release called before acquire", ESP_ERR_INVALID_STATE );

	/*For now the WiFi would use ADC2 and set xpd_sar force on.
	   so we can not reset xpd_sar to fsm mode directly.
	   We should handle this after the synchronization mechanism is established.*/
	_lock_release( &adc1_i2s_lock );
	ESP_LOGD( RTC_MODULE_TAG, "returns adc1 lock." );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_AtoD::Scan_Done( void ) const
{
	if ( REG_GET_FIELD( SENS_SAR_START_FORCE_REG, SENS_SAR1_STOP ) != DISABLE_VALUE )
	{
		return ( true );
	}
	else
	{
		return ( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Dump_AtoD_Data( uC_BASE_ATOD_CTRL_ENUM unit, int channel ) const
{

	if ( unit == uC_BASE_ATOD_CTRL_1 )
	{
		SENS.sar_meas_start1.sar1_en_pad = ( 1 << channel );	///< Only one channel is selected.
		while ( SENS.sar_slave_addr1.meas_status != DISABLE_VALUE )
		{}
		SENS.sar_meas_start1.meas1_start_sar = DISABLE_VALUE;
		SENS.sar_meas_start1.meas1_start_sar = 1;
		while ( SENS.sar_meas_start1.meas1_done_sar == DISABLE_VALUE )
		{}
	}
	else
	{
		ESP_LOGE( RTC_MODULE_TAG, "invalid adc unit" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::Start_Repeat_Scan( void ) const
{
	// Set the Continuous conversion bit so that conversions will take place continuously.
	uC_IO_Config::Set_Pin_Input( SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD_FORCE_M );

	Start_Scan();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_AtoD::Get_Channel( uint8_t channel, uC_BASE_ATOD_CTRL_ENUM unit ) const
{
	int32_t return_val;

	if ( unit == uC_BASE_ATOD_CTRL_1 )
	{
		return_val = adc1_get_raw( adc1_channel_t( channel ) );
	}
	else
	{
		return_val = DISABLE_VALUE;
	}

	return ( uint16_t( return_val ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::Get_ADC1_IO_Number( adc1_channel_t channel, gpio_num_t* gpio_num )
{
	switch ( channel )
	{
		case ADC1_CHANNEL_0:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_0_GPIO_NUM );
			break;

		case ADC1_CHANNEL_1:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_1_GPIO_NUM );
			break;

		case ADC1_CHANNEL_2:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_2_GPIO_NUM );
			break;

		case ADC1_CHANNEL_3:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_3_GPIO_NUM );
			break;

		case ADC1_CHANNEL_4:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_4_GPIO_NUM );
			break;

		case ADC1_CHANNEL_5:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_5_GPIO_NUM );
			break;

		case ADC1_CHANNEL_6:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_6_GPIO_NUM );
			break;

		case ADC1_CHANNEL_7:
			*gpio_num = gpio_num_t( ADC1_CHANNEL_7_GPIO_NUM );
			break;

		default:
			return ( ESP_ERR_INVALID_ARG );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::Check_ADC( esp_err_t fun_ret )
{
	if ( fun_ret != ESP_OK )
	{
		ESP_LOGE( RTC_MODULE_TAG, "%s:%d\n", __FUNCTION__, __LINE__ );
	}
	return ( ESP_FAIL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::Check_RTC_Module( bool validation_result, const char* str, esp_err_t error_type )
{
	if ( !( validation_result ) )
	{
		ESP_LOGE( RTC_MODULE_TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str );
		return ( error_type );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::Set_ADC_IO_Pins( gpio_num_t gpio_num )
{
	Check_RTC_Module( ( gpio_num < GPIO_PIN_COUNT && ( rtc_gpio_desc[gpio_num].reg != DISABLE_VALUE ) ),
					  "RTC_GPIO number error", ESP_ERR_INVALID_ARG );

	// 0: GPIO connected to digital GPIO module. 1: GPIO connected to analog RTC module.
	uC_IO_Config::Set_Pin_Input( rtc_gpio_desc[gpio_num].reg, ( rtc_gpio_desc[gpio_num].mux ) );

	// 0:RTC FUNCIOTN 1,2,3:Reserved
	uC_IO_Config::Set_Peri_Reg_Bit( rtc_gpio_desc[gpio_num].reg, RTC_IO_TOUCH_PAD1_FUN_SEL_V,
									0x0, rtc_gpio_desc[gpio_num].func );
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::ADC_Output_disable( gpio_num_t gpio_num )
{
	gpio_num = gpio_num_t( rtc_gpio_desc[gpio_num].rtc_num );
	Check_RTC_Module( gpio_num != -1, "RTC_GPIO number error", ESP_ERR_INVALID_ARG );
	uC_IO_Config::Reset_Pin_Input( RTC_GPIO_ENABLE_W1TS_REG, ( 1 << ( gpio_num + RTC_GPIO_ENABLE_W1TS_S ) ) );
	uC_IO_Config::Set_Pin_Input( RTC_GPIO_ENABLE_W1TC_REG, ( 1 << ( gpio_num + RTC_GPIO_ENABLE_W1TC_S ) ) );

	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::ADC_Input_enable( gpio_num_t gpio_num )
{
	Check_RTC_Module( ( gpio_num < GPIO_PIN_COUNT && ( rtc_gpio_desc[gpio_num].reg != DISABLE_VALUE ) ),
					  "RTC_GPIO number error", ESP_ERR_INVALID_ARG );
	uC_IO_Config::Set_Pin_Input( rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie );

	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::ADC_Output_enable( gpio_num_t gpio_num )
{
	gpio_num = gpio_num_t( rtc_gpio_desc[gpio_num].rtc_num );
	Check_RTC_Module( gpio_num != -1, "RTC_GPIO number error", ESP_ERR_INVALID_ARG );
	uC_IO_Config::Set_Pin_Input( RTC_GPIO_ENABLE_W1TS_REG, ( 1 << ( gpio_num + RTC_GPIO_ENABLE_W1TS_S ) ) );
	uC_IO_Config::Reset_Pin_Input( RTC_GPIO_ENABLE_W1TC_REG, ( 1 << ( gpio_num + RTC_GPIO_ENABLE_W1TC_S ) ) );

	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_AtoD::ADC_Input_disable( gpio_num_t gpio_num )
{
	Check_RTC_Module( ( gpio_num < GPIO_PIN_COUNT && ( rtc_gpio_desc[gpio_num].reg != DISABLE_VALUE ) ),
					  "RTC_GPIO number error", ESP_ERR_INVALID_ARG );

	uC_IO_Config::Reset_Pin_Input( rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie );

	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_AtoD::ADC_IO_Configuration( gpio_num_t gpio_num )
{
	uC_USER_IO_STRUCT Tempstruct;

	Tempstruct.io_num = ( uint32_t )gpio_num;
	Check_ADC( Set_ADC_IO_Pins( gpio_num ) );
	Check_ADC( ADC_Output_disable( gpio_num ) );
	Check_ADC( ADC_Input_disable( gpio_num ) );
	uC_IO_Config::Set_Pin_To_High_Z( &Tempstruct );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_adc_cal_value_t uC_AtoD::ADC_Characterization( uC_BASE_ATOD_CTRL_ENUM adc_num, adc_atten_t atten,
												   uC_ATOD_RESOLUTION_ENUM bit_width, uint32_t default_vref,
												   esp_adc_cal_characteristics_t* chars )
{
	esp_adc_cal_value_t val_type = ESP_ADC_CAL_VAL_DEFAULT_VREF;

	if ( adc_num == uC_BASE_ATOD_CTRL_1 )
	{
		val_type = esp_adc_cal_characterize( ADC_UNIT_1, atten, adc_bits_width_t(
												 bit_width ), default_vref, chars );
	}
	return ( val_type );
}
