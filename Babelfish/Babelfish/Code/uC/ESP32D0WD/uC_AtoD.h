/**
 *****************************************************************************************
 *	@file		uC_AtoD.h
 *
 *	@brief		The file contains uC_AtoD Class required for communication over ADC channels
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef uC_ATOD_H
	#define uC_ATOD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "uC_Interrupt.h"
#include "uC_Base_HW_IDs_ESP32.h"
#include "uC_IO_Config.h"
#include "esp_adc_cal.h"
#include "freertos/portmacro.h"
#include "driver/rtc_io.h"

/**
 ****************************************************************************************
 * @brief Various ADC resolution options
 *
 ****************************************************************************************
 */
enum uC_ATOD_RESOLUTION_ENUM
{
	ADC_RESOLUTION_BIT_9 = 0,								///< ADC capture width is 9Bit*/
	ADC_RESOLUTION_BIT_10 = 1,								///< ADC capture width is 10Bit*/
	ADC_RESOLUTION_BIT_11 = 2,								///< ADC capture width is 11Bit*/
	ADC_RESOLUTION_BIT_12 = 3,								///< ADC capture width is 12Bit*/
	ADC_RESOLUTION_MAX
};

/**
 **********************************************************************************************
 * @brief			AtoD Class Declaration. The class will handle all the
 * @n				functionalities related to A2D conversion.
 **********************************************************************************************
 */
class uC_AtoD
{

	public:
		/**
		 * @brief											Constructor to create instance of uC_AtoD class.
		 * @param[in] resolution							Resolution of the AtoD conversion. Possible values
		 * @n												are mentioned below :
		 * @n												uC_ATOD_8BIT_RESOLUTION : Selects 8 Bit Resolution
		 * @n												uC_ATOD_16BIT_RESOLUTION : Selects 16 Bit Resolution
		 * @param[in] active_channels_mask					Specifies ADC channels used in application. STM32F407
		 * @n												features total 19 ADC channels (channel 0 to channel 18).
		 * @n												For each active channel, corresponding bit should be set.
		 * @n												If channel 0 and 2 are to be used, value of
		 * @n												active_channels_mask should be 0x00000005.
		 * @param[in] time_reqd_to_scan_all_channels_us		The amount of time in microseconds required to scan all of
		 * @n												the channels so that one good data sample is acquired
		 * @n												before the constructor exits.Value must be non-zero
		 * @param[in] atod_block							Analog to Digital Channel control structure
		 * @return Void										None
		 */
		uC_AtoD( uC_ATOD_RESOLUTION_ENUM resolution,
				 uint32_t active_channels_mask,
				 uC_BASE_ATOD_CTRL_ENUM atod_block = uC_BASE_ATOD_CTRL_1,
				 uint16_t time_reqd_to_scan_all_channels_us = ADC_STABILIZATION_TIME_uS );

		/**
		 * @brief											Destructor to delete the uc_AtoD instance
		 * @n												when it goes out of scope
		 * @return Void										None
		 */
		~uC_AtoD( void ) {}

		/**
		 * @brief											This function turns on AtoD converter and starts conversion
		 * @return void										None
		 */
		void Start_Scan( void ) const;

		/**
		 * @brief											This function checks if AtoD conversion is finished
		 * @param[in] void									none
		 * @return bool										TRUE: AtoD conversion is finished
		 * @n												FALSE: AtoD conversion is not finished
		 */
		bool Scan_Done( void ) const;

		/**
		 * @brief											This function enables continuous AtoD conversion
		 * @return void										None
		 */
		void Start_Repeat_Scan( void ) const;

		/**
		 * @brief											This function gives AtoD value from a particular ADC channel
		 * @param[in] channel								ADC channel
		 * @param[in] unit									Which ADC control (ADC1 or ADC2)
		 * @return uint16_t									AtoD value read from ADC channel (Always returns a
		 * @n												16bit value no matter the resolution)
		 * @n												The 16bit value is always left justified. A 10bit ADC will
		 * @n												left justify the 10bit result to create a 16bit result
		 * @n												with the lower 6bits set to 0.
		 */
		uint16_t Get_Channel( uint8_t channel, uC_BASE_ATOD_CTRL_ENUM unit = uC_BASE_ATOD_MAX_CTRLS ) const;

		/**
		 * @brief											Characterize an ADC at a particular attenuation
		 * @n												This function will characterize the ADC at a particular
		 * @n												attenuation and generate the ADC-Voltage curve in the
		 * @n												form of [y = coeff_a * x + coeff_b]. Characterization can
		 * @n												be based on Two Point values, eFuse Vref, or default Vref
		 * @n												and calibration values will be prioritized in that order
		 * @n												Two Point values and eFuse Vref can be enabled/disabled
		 * @n												using menu config.
		 * @param[in] adc_num								ADC to characterize (ADC_UNIT_1 or ADC_UNIT_2)
		 * @param[in] atten									Attenuation to characterize
		 * @param[in] bit_width								Bit width configuration of ADC
		 * @param[in] default_vref							Default ADC reference voltage in mV (used if eFuse
		 * @n												values is not available)
		 * @param[out] chars								Pointer to empty structure used to store ADC characteristic
		 * @return esp_adc_cal_value_t						ESP_ADC_CAL_VAL_EFUSE_VREF: eFuse Vref used for
		 * @n												characterization
		 * @n												ESP_ADC_CAL_VAL_EFUSE_TP: Two Point value used for
		 * @n												characterization (only in Linear Mode)
		 * @n												ESP_ADC_CAL_VAL_DEFAULT_VREF: Default Vref used for
		 * @n												characterization
		 */
		esp_adc_cal_value_t ADC_Characterization( uC_BASE_ATOD_CTRL_ENUM adc_num, adc_atten_t atten,
												  uC_ATOD_RESOLUTION_ENUM bit_width, uint32_t default_vref,
												  esp_adc_cal_characteristics_t* chars );

		/*
		 *****************************************************************************************
		 *		Constants
		 *****************************************************************************************
		 */
		static const uint8_t ADC_STABILIZATION_TIME_uS = 5U;
		static const uint8_t CHANNEL_VALUE2 = 2U;
		static const uint8_t CHANNEL_VALUE3 = 3U;
		static const uint8_t CHANNEL_VALUE4 = 4U;
		static const uint8_t ENABLE_VALUE = 1U;
		static const uint8_t DISABLE_VALUE = 0U;

	private:

		/**
		 * @brief											This function dumps ADC regular data register value
		 * @n												into local variable
		 * @param[in] channel								ADC channel number
		 * @param[in] unit									Which ADC control (ADC1 or ADC2)
		 * @return void										none
		 */
		void Dump_AtoD_Data( uC_BASE_ATOD_CTRL_ENUM unit = uC_BASE_ATOD_MAX_CTRLS, int channel = 0 ) const;

		/**
		 * @brief											AtoD Control Definition Structure
		 */
		uC_BASE_ATOD_IO_STRUCT const* m_adc;

		uint8_t m_resolution;								///< Indicates the ADC resolution
		uint16_t* m_channel_buffer;
		uint8_t* m_channel_lookup;
		uint32_t m_active_channel;

		/**
		 * @brief											This function Gives ADC1 GPIO numbers.
		 * @param[in] channel								ADC channel.
		 * @param[in] gpio_num								GPIO number.
		 * @return esp_err_t								esp error type
		 */
		esp_err_t Get_ADC1_IO_Number( adc1_channel_t channel, gpio_num_t* gpio_num );

		/**
		 * @brief											Function to check the ADC
		 * @param[in] fun_ret								Return of an function of type esp_err_t
		 * @return esp_err_t								esp error type
		 */
		esp_err_t Check_ADC( esp_err_t fun_ret );

		/**
		 * @brief											Function to check the RTC module
		 * @param[in] validation_result						true if the validation is correct or else false will be sent
		 * @param[in] str									GPIO string used for displaying the error message
		 * @param[in] error_type							esp Error type
		 * @return esp_err_t								esp error type
		 */
		static esp_err_t Check_RTC_Module( bool validation_result, const char* str, esp_err_t error_type );

		/**
		 * @brief											This function intializes ADC input output pins
		 * @param[in] gpio_num								GPIO number
		 * @return esp_err_t								esp error type
		 */
		esp_err_t Set_ADC_IO_Pins( gpio_num_t gpio_num );

		/**
		 * @brief											This function intializes ADC input output pins
		 * @param[in] gpio_num								GPIO number
		 * @return esp_err_t								esp error type
		 */
		esp_err_t ADC_Output_disable( gpio_num_t gpio_num );

		/**
		 * @brief											This function intializes ADC input output pins
		 * @param[in] gpio_num								GPIO number
		 * @return esp_err_t								esp error type
		 */
		esp_err_t ADC_Input_enable( gpio_num_t gpio_num );

		/**
		 * @brief											This function intializes ADC input output pins
		 * @param[in] gpio_num								GPIO number
		 * @return esp_err_t								esp error type
		 */
		esp_err_t ADC_Output_enable( gpio_num_t gpio_num );

		/**
		 * @brief											This function intializes ADC input output pins
		 * @param[in] gpio_num								GPIO number
		 * @return esp_err_t								esp error type
		 */
		esp_err_t ADC_Input_disable( gpio_num_t gpio_num );

		/**
		 * @brief											This function performs IO configuration
		 * @param[in] gpio_num								GPIO number
		 * @return void										None
		 */
		void ADC_IO_Configuration( gpio_num_t gpio_num );

};

#ifdef __cplusplus
}
#endif

#endif
