/**
 *****************************************************************************************
 *	@file		uC_IO_Config.cpp
 *	@details    See header file for module overview.
 *	@copyright  2018 Eaton Corporation. All Rights Reserved.
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
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions
 * shall not change the signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as
 * is The note is re-enabled at the end of this file
 */
#include "uC_IO_Config.h"
#include "uC_Base.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "esp_log.h"
#include "uC_Base_HW_IDs_ESP32.h"
#include "driver/rtc_io.h"

/*
 *****************************************************************************************
 *	Constant Variable
 *****************************************************************************************
 */
static const char* GPIO_TAG = "gpio";
static const uint8_t MAX_GPIO_LEVEL = 32;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_IO_Config::uC_IO_Config( void )
{
	uC_BASE_PIO_CTRL_STRUCT const* temp_pio;
	uC_USER_IO_STRUCT temp_user_io;
	uC_IO_INPUT_CFG_EN input_cfg;
	uC_PERIPH_IO_STRUCT temp_PERIPH_IO_STRUCT;
	uint64_t gpio_pin_mask = 0;
	uint32_t io_reg, io_num;
	uint8_t input_en;
	uint8_t output_en;
	uint8_t od_en;
	uint8_t pu_en;
	uint8_t pd_en;

	for ( io_num = 0U; io_num < static_cast<uint32_t>( uC_BASE_MAX_PIO_CTRLS ); io_num++ )
	{
		pd_en = 0;
		pu_en = 0;
		input_en = 0;
		output_en = 0;
		od_en = 0;
		// temp_pio = ( uC_Base::Self() )->Get_PIO_Ctrl( io_num );
		temp_pio = &uC_Base::m_pio_ctrls[io_num];
		gpio_pin_mask = temp_pio->pin_bit_mask;

		temp_PERIPH_IO_STRUCT.mode = temp_user_io.mode = temp_pio->mode;
		temp_user_io.io_num = io_num;
		temp_PERIPH_IO_STRUCT.io_num = static_cast<gpio_num_t>( io_num );
		temp_PERIPH_IO_STRUCT.intr_type = temp_user_io.intr_type = temp_pio->intr_type;

		// Checking whether the selected GPIO is within the limit or not
		if ( ( temp_pio->pin_bit_mask == 0 ) || ( temp_pio->pin_bit_mask >= MAX_GPIO_NUMBER ) )
		{
			ESP_LOGE( GPIO_TAG, "GPIO_PIN mask error " );
		}

		// Checking whether GPIOs 34/35/36/37/38/39 are configured as output
		if ( temp_pio->mode == GPIO_MODE_OUTPUT )
		{
			if ( ( gpio_pin_mask == GPIO_SEL_34 ) || ( gpio_pin_mask == GPIO_SEL_35 ) ||
				 ( gpio_pin_mask == GPIO_SEL_36 ) || ( gpio_pin_mask == GPIO_SEL_37 ) ||
				 ( gpio_pin_mask == GPIO_SEL_38 ) || ( gpio_pin_mask == GPIO_SEL_39 ) )
			{
				ESP_LOGE( GPIO_TAG, "GPIO34-39 can only be used as input mode" );
			}
		}

		io_reg = GPIO_PIN_MUX_REG[io_num];

		if ( ( ( gpio_pin_mask >> io_num ) & BIT( 0 ) ) && io_reg )
		{
			if ( RTC_GPIO_Validation( static_cast<gpio_num_t>( io_num ) ) )
			{
				rtc_gpio_deinit( static_cast<gpio_num_t>( io_num ) );
			}
			// GPIO is configured as Input
			if ( temp_pio->mode == GPIO_MODE_INPUT )
			{
				input_en = 1;
			}
			// GPIO configured as output
			else if ( temp_pio->mode == GPIO_MODE_OUTPUT )
			{
				output_en = 1;
			}

			// GPIO is configured as Input
			Set_Pin_To_Input( &temp_user_io );

			// Open drain output configuration
			od_en = Set_Open_Drain( temp_pio, io_num );

			// GPIO configured as output
			Set_Pin_To_Output( &temp_user_io );

			input_cfg = uC_IO_IN_CFG_PULLUP;
			if ( temp_pio->pull_up_en )
			{
				pu_en = 1;
				Enable_Periph_Input_Pin( &temp_PERIPH_IO_STRUCT, input_cfg );
			}
			else
			{
				Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, input_cfg );
			}

			input_cfg = uC_IO_IN_CFG_PULLDOWN;
			if ( temp_pio->pull_down_en )
			{
				pd_en = 1;
				Enable_Periph_Input_Pin( &temp_PERIPH_IO_STRUCT, input_cfg );
			}
			else
			{
				Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, input_cfg );
			}

			ESP_LOGI( GPIO_TAG,
					  "GPIO[%d]| InputEn: %d| OutputEn: %d| OpenDrain: %d| Pullup: %d| Pulldown: %d| Intr:%d ", io_num,
					  input_en, output_en, od_en, pu_en, pd_en, temp_pio->intr_type );

			// function number 2 is GPIO_FUNC for each pin
			PIN_FUNC_SELECT( io_reg, PIN_FUNC_GPIO );
		}
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_Input( uC_USER_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg )
{
	if ( pio_ctrl->mode == GPIO_MODE_INPUT )
	{
		Set_Pin_Input( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_IE );
	}
	else
	{
		Reset_Pin_Input( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_IE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_Output( uC_USER_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	uC_PERIPH_IO_STRUCT temp_PERIPH_IO_STRUCT;

	temp_PERIPH_IO_STRUCT.intr_type = pio_ctrl->intr_type;
	temp_PERIPH_IO_STRUCT.mode = pio_ctrl->mode;
	temp_PERIPH_IO_STRUCT.io_num = static_cast<gpio_num_t>( pio_ctrl->io_num );

	if ( pio_ctrl->mode == GPIO_MODE_OUTPUT )
	{
		Enable_Periph_Output_Pin( &temp_PERIPH_IO_STRUCT );
	}
	else
	{
		Disable_Periph_Pin_Set_To_Output( &temp_PERIPH_IO_STRUCT );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Set_Pin_To_High_Z( uC_USER_IO_STRUCT const* pio_ctrl )
{
	uC_PERIPH_IO_STRUCT Temp_Periph_IO;

	Temp_Periph_IO.intr_type = pio_ctrl->intr_type;
	Temp_Periph_IO.io_num = static_cast<gpio_num_t>( pio_ctrl->io_num );
	Temp_Periph_IO.mode = pio_ctrl->mode;

	Disable_Periph_Pin_Set_To_Input( &Temp_Periph_IO, uC_IO_IN_CFG_HIZ );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_IO_Config::Set_Open_Drain( uC_BASE_PIO_CTRL_STRUCT const* pio_ctrl, uint32_t io_num )
{
	uint8_t od_en = 0;

	if ( ( pio_ctrl->mode == GPIO_MODE_OUTPUT_OD ) || ( pio_ctrl->mode == GPIO_MODE_INPUT_OUTPUT_OD ) )
	{
		od_en = 1;
		GPIO.pin[io_num].pad_driver = 1;/*0x01 Open-drain */
	}
	else
	{
		GPIO.pin[io_num].pad_driver = 0;/*0x00 Normal gpio output */
	}
	return ( od_en );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Enable_Periph_Output_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	Check_GPIO( GPIO_Validation( pio_ctrl->io_num ), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG );
	if ( pio_ctrl->io_num < GPIO_OUTPUT_NUMBER )
	{
		GPIO.enable_w1ts = ( 0x1 << pio_ctrl->io_num );
	}
	else
	{
		GPIO.enable1_w1ts.data = ( 0x1 << ( pio_ctrl->io_num - GPIO_OUTPUT_NUMBER ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Enable_Periph_Input_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg )
{
	// Enable pull up
	if ( input_cfg == uC_IO_IN_CFG_PULLUP )
	{
		Check_GPIO( GPIO_Validation( pio_ctrl->io_num ), "GPIO number error", ESP_ERR_INVALID_ARG );
		if ( RTC_GPIO_Validation( pio_ctrl->io_num ) )
		{
			rtc_gpio_pullup_en( pio_ctrl->io_num );
		}
		else
		{
			Set_Reg_Bit( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_PU );
		}
	}
	// Enable pull down
	else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
	{
		Check_GPIO( GPIO_Validation( pio_ctrl->io_num ), "GPIO number error", ESP_ERR_INVALID_ARG );
		if ( RTC_GPIO_Validation( pio_ctrl->io_num ) )
		{
			rtc_gpio_pulldown_en( pio_ctrl->io_num );
		}
		else
		{
			Set_Reg_Bit( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_PD );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Disable_Periph_Pin_Set_To_Output( uC_PERIPH_IO_STRUCT const* pio_ctrl, bool initial_state )
{
	Check_GPIO( GPIO_Validation( pio_ctrl->io_num ), "GPIO number error", ESP_ERR_INVALID_ARG );
	if ( pio_ctrl->io_num < GPIO_OUTPUT_NUMBER )
	{
		GPIO.enable_w1tc = ( 0x1 << pio_ctrl->io_num );
	}
	else
	{
		GPIO.enable1_w1tc.data = ( 0x1 << ( pio_ctrl->io_num - GPIO_OUTPUT_NUMBER ) );
	}
	// Ensure no other output signal is routed via GPIO matrix to this pin
	Set_Reg_Value( GPIO_FUNC0_OUT_SEL_CFG_REG + ( pio_ctrl->io_num * 4 ),
				   SIG_GPIO_OUT_IDX );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_IO_Config::Disable_Periph_Pin_Set_To_Input( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													uC_IO_INPUT_CFG_EN input_cfg )
{
	// Disable pull up
	if ( input_cfg == uC_IO_IN_CFG_PULLUP )
	{
		Check_GPIO( GPIO_Validation( pio_ctrl->io_num ), "GPIO number error", ESP_ERR_INVALID_ARG );
		if ( RTC_GPIO_Validation( pio_ctrl->io_num ) )
		{
			rtc_gpio_pullup_dis( pio_ctrl->io_num );
		}
		else
		{
			Reset_Reg_Bit( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_PU );
		}
	}
	// Disable pull down
	else if ( input_cfg == uC_IO_IN_CFG_PULLDOWN )
	{
		Check_GPIO( GPIO_Validation( pio_ctrl->io_num ), "GPIO number error", ESP_ERR_INVALID_ARG );
		if ( RTC_GPIO_Validation( pio_ctrl->io_num ) )
		{
			rtc_gpio_pulldown_dis( pio_ctrl->io_num );
		}
		else
		{
			Reset_Reg_Bit( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_PD );
		}
	}
	// Disable both Pull up and Pull down
	else
	{
		Check_GPIO( GPIO_Validation( pio_ctrl->io_num ), "GPIO number error", ESP_ERR_INVALID_ARG );
		Reset_Reg_Bit( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_PU );
		Reset_Reg_Bit( GPIO_PIN_MUX_REG[pio_ctrl->io_num], FUN_PD );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_IO_Config::Check_GPIO( bool validation_result, const char* str, esp_err_t error_type )
{
	if ( !( validation_result ) )
	{
		ESP_LOGE( GPIO_TAG, "%s(%d): %s", __FUNCTION__, __LINE__, str );
		return ( error_type );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_IO_Config::Set_GPIO_Level( gpio_num_t gpio_num, uint32_t level )
{
	Check_GPIO( GPIO_Validation( gpio_num ), "GPIO output gpio_num error", ESP_ERR_INVALID_ARG );
	if ( level )
	{
		if ( gpio_num < MAX_GPIO_LEVEL )
		{
			GPIO.out_w1ts = ( 1 << gpio_num );
		}
		else
		{
			GPIO.out1_w1ts.data = ( 1 << ( gpio_num - MAX_GPIO_LEVEL ) );
		}
	}
	else
	{
		if ( gpio_num < MAX_GPIO_LEVEL )
		{
			GPIO.out_w1tc = ( 1 << gpio_num );
		}
		else
		{
			GPIO.out1_w1tc.data = ( 1 << ( gpio_num - MAX_GPIO_LEVEL ) );
		}
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_IO_Config::Set_GPIO_Direction( gpio_num_t gpio_num, gpio_mode_t mode )
{
	uC_PERIPH_IO_STRUCT Temp_IO_Struct;

	Check_GPIO( GPIO_Validation( gpio_num ), "GPIO number error", ESP_ERR_INVALID_ARG );
	if ( ( gpio_num >= MAX_GPIO_OUTPUT ) && ( mode == GPIO_MODE_DEF_OUTPUT ) )
	{
		ESP_LOGE( GPIO_TAG, "io_num=%d can only be input", gpio_num );
		return ( ESP_ERR_INVALID_ARG );
	}

	if ( mode == GPIO_MODE_DEF_INPUT )
	{
		PIN_INPUT_ENABLE( GPIO_PIN_MUX_REG[gpio_num] );
	}
	else
	{
		PIN_INPUT_DISABLE( GPIO_PIN_MUX_REG[gpio_num] );
	}

	Temp_IO_Struct.io_num = gpio_num;
	if ( mode == GPIO_MODE_DEF_OUTPUT )
	{
		Enable_Periph_Output_Pin( &Temp_IO_Struct );
	}
	else
	{
		Disable_Periph_Pin_Set_To_Output( &Temp_IO_Struct );
	}
	if ( mode == GPIO_MODE_DEF_OD )
	{
		GPIO.pin[gpio_num].pad_driver = 1;
	}
	else
	{
		GPIO.pin[gpio_num].pad_driver = 0;
	}
	return ( ESP_OK );
}
