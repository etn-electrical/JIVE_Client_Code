/**
 *****************************************************************************************
 *	@file		uC_IO_Config.h
 *
 *	@brief		Contains the uC_IO_Config Class
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef uC_IO_CONFIG_H
#define uC_IO_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "uC_Base.h"
#include "soc/soc.h"
#include "esp_log.h"
#include "uC_IO_Define_ESP32.h"
#include "esp_err.h"
#include "uC_Base.h"
#include "uC_Base_HW_IDs_ESP32.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_periph.h"


/**
 * @brief Commandment: GPIO modes
 */
enum uC_IO_INPUT_CFG_EN
{
	uC_IO_IN_CFG_HIZ,
	uC_IO_IN_CFG_PULLUP,
	uC_IO_IN_CFG_PULLDOWN
};

/*
 **************************************************************************************************
 *  Class Declaration
 **************************************************************************************************
 */
class uC_IO_Config
{
	public:

		/**
		 * @brief							Constructor to create instance of uC_IO_Config class.
		 * @return	void					None
		 */
		uC_IO_Config( void );

		/**
		 * @brief							Destructor to delete the uC_IO_Config instance
		 * @n								when it goes out of scope
		 * @param[in] void					none
		 * @return
		 */
		~uC_IO_Config( void ) {}

		/**
		 * @brief                           Function to set the GPIO pin as output
		 * @param[in] uC_USER_IO_STRUCT     GPIO user structure
		 * @return void                     none
		 */

		static inline void Set_Out( uC_USER_IO_STRUCT const* pio_ctrl )
		{ GPIO.out_w1ts = static_cast<uint32_t>( 1 << pio_ctrl->io_num ); }

		/**
		 * @brief                           Function to clear the GPIO pin as output
		 * @param[in] uC_USER_IO_STRUCT     GPIO user structure
		 * @return void                     none
		 */

		static inline void Clr_Out( uC_USER_IO_STRUCT const* pio_ctrl )
		{ GPIO.out_w1tc = static_cast<uint32_t>( 1 << pio_ctrl->io_num ); }

		/**
		 * @brief                           Function to get the GPIO pin as output
		 * @param[in] uC_USER_IO_STRUCT     GPIO user structure
		 * @return bool                     none
		 */

		static inline bool Get_Out( uC_USER_IO_STRUCT const* pio_ctrl )
		{
			if ( pio_ctrl->io_num < 32 )
			{
				return ( ( GPIO.in >> pio_ctrl->io_num ) & 0x1 );
			}
			else
			{
				return ( ( GPIO.in1.data >> ( pio_ctrl->io_num - 32 ) ) & 0x1 );
			}

		}

		/**
		 * @brief							Function to set the GPIO pin as input
		 * @param[in] uC_USER_IO_STRUCT		GPIO user structure
		 * @param[in] uC_IO_INPUT_CFG_EN	GPIO input configuration
		 * @return void						none
		 */
		static void Set_Pin_To_Input( uC_USER_IO_STRUCT const* pio_ctrl,
									  uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ );

		/**
		 * @brief							Function to set the GPIO pin as Output
		 * @param[in] uC_USER_IO_STRUCT		GPIO user structure
		 * @param[in] initial_state			Initial state of GPIO pin
		 * @return void						none
		 */
		static void Set_Pin_To_Output( uC_USER_IO_STRUCT const* pio_ctrl, bool initial_state = uC_IO_OUT_INIT_LOW );

		/**
		 * @brief							Function to set the GPIO pin as High (Z)
		 * @param[in] uC_USER_IO_STRUCT		GPIO user structure
		 * @return void						none
		 */
		static void Set_Pin_To_High_Z( uC_USER_IO_STRUCT const* pio_ctrl );

		/**
		 * @brief							Function to Enable the GPIO pin as Input pin
		 * @param[in] uC_PERIPH_IO_STRUCT	GPIO peripheral structure
		 * @param[in] uC_IO_INPUT_CFG_EN	Input configuration type
		 * @return void						none
		 */
		static void Enable_Periph_Input_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl,
											 uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ );

		/**
		 * @brief							Function to Enable the GPIO pin as Output pin
		 * @param[in] uC_PERIPH_IO_STRUCT	GPIO peripheral structure
		 * @param[in] initial_state			Initial state of GPIO pin
		 * @return void						none
		 */
		static void Enable_Periph_Output_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl,
											  bool initial_state = uC_IO_OUT_INIT_LOW );

		/**
		 * @brief							Function to Disable the GPIO pin
		 * @param[in] uC_PERIPH_IO_STRUCT	GPIO peripheral structure
		 * @return void						none
		 */
		static void Disable_Periph_Pin( uC_PERIPH_IO_STRUCT const* pio_ctrl )
		{ Disable_Periph_Pin_Set_To_Input( pio_ctrl ); }

		/**
		 * @brief							Function to Disable the GPIO pin as Output pin
		 * @param[in] uC_PERIPH_IO_STRUCT	GPIO peripheral structure
		 * @param[in] initial_state			Initial state of GPIO pin
		 * @return void						none
		 */
		static void Disable_Periph_Pin_Set_To_Output( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													  bool initial_state = uC_IO_OUT_INIT_LOW );

		/**
		 * @brief							Function to Disable the GPIO pin as Input pin
		 * @param[in] uC_PERIPH_IO_STRUCT	GPIO peripheral structure
		 * @param[in] uC_IO_INPUT_CFG_EN	Input configuration type
		 * @return void						none
		 */
		static void Disable_Periph_Pin_Set_To_Input( uC_PERIPH_IO_STRUCT const* pio_ctrl,
													 uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ );

		/**
		 * @brief							Function sets the pin to input
		 * @param[in] pin_name				Pin to be set
		 * @param[in] Input					pin input
		 * @return void						None
		 */
		static inline void Set_Pin_Input( uint32_t pin_name, uint32_t Input )
		{   SET_PERI_REG_MASK( pin_name, Input ); }

		/**
		 * @brief							Function resets the pin to input
		 * @param[in] pin_name				Pin to be reset
		 * @param[in] Input					pin input
		 * @return  void					None
		 */
		static inline void Reset_Pin_Input( uint32_t pin_name, uint32_t Input )
		{  CLEAR_PERI_REG_MASK( pin_name, Input ); }

		/**
		 * @brief							Function set bits of register controlled by mask and shift used only for
		 * @n								ESP32
		 * @param[in] reg					Register details
		 * @param[in] bit_map				Bit to be set
		 * @param[in] value					Value to be set.
		 * @param[in] shift					Value to be shifted.
		 * @return void						None
		 */
		static inline void Set_Peri_Reg_Bit( uint32_t reg, uint32_t bit_map, uint8_t value, uint32_t shift )
		{
			SET_PERI_REG_BITS( reg, bit_map, value, shift );
		}

		/**
		 * @brief								Function validates the GPIO number
		 * @param[in] gpio_num					GPIO number which is required to be validated
		 * @return bool							true if it is a valid GPIO number
		 * @n									false if it is an invalid GPIO number
		 */
		static inline bool GPIO_Validation( gpio_num_t gpio_num )
		{ return ( ( ( gpio_num < GPIO_PIN_COUNT ) && ( GPIO_PIN_MUX_REG[gpio_num] != 0 ) ) );}

		/**
		 * @brief								Function validates the output GPIO number
		 * @param[in] gpio_num					Output GPIO number which is required to be validated
		 * @return bool							true if it is a valid output GPIO number
		 * @n									false if it is an invalid output GPIO number
		 */
		static inline bool GPIO_Output_Validation( gpio_num_t gpio_num )
		{ return ( ( GPIO_Validation( gpio_num ) ) && ( gpio_num < MAX_GPIO_OUTPUT ) );}

		/**
		 * @brief								Function to set the GPIO level
		 * @param[in] gpio_num					GPIO number setting of requries pin level
		 * @param[in] level						Level value
		 * @return esp_err_t					esp error type
		 */
		static esp_err_t Set_GPIO_Level( gpio_num_t gpio_num, uint32_t level );


		/**
		 * @brief								Function to set the direction
		 * @param[in] gpio_num					GPIO number setting of requries pin level
		 * @param[in] mode						GPIO mode
		 * @return esp_err_t					esp error type
		 */
		static esp_err_t Set_GPIO_Direction( gpio_num_t gpio_num, gpio_mode_t mode );

		/**
		 * @brief								Function reads the register value
		 * @param[in] reg						Register to be read
		 * @return uint32_t						Register value
		 */
		static inline uint32_t Read_Reg_Value( uint32_t reg )
		{
			ASSERT_IF_DPORT_REG( ( reg ), REG_READ );                                                                    \
			return ( *( volatile uint32_t* )( reg ) );
		}

		/**
		 * @brief								Function sets the register value
		 * @param[in] reg						Register to be set
		 * @param[in] value						Value to be set or written to register
		 * @return void							None
		 */
		static inline void Set_Reg_Value( uint32_t reg, uint32_t value )
		{
			ASSERT_IF_DPORT_REG( ( reg ), REG_WRITE );                                                                      \
			( *( volatile uint32_t* )( reg ) ) = ( value );
		}

	private:

		/**
		 * @brief	constant variables
		 */
		static const bool uC_IO_OUT_INIT_LOW = false;
		static const bool uC_IO_OUT_INIT_HIGH = true;

		/**
		 * @brief	GPIO pin numbers from 32 onwards should not be used as output pins
		 */
		static const uint8_t GPIO_OUTPUT_NUMBER = 32;

		/**
		 * @brief								GPIO pins bit mask value should not be more than or equal to
		 * @n									( uint64_t ) 1 ) << GPIO_PIN_COUNT ) = 0x10000000000
		 */
		static const uint64_t MAX_GPIO_NUMBER = 0x10000000000L;

		/**
		 * @brief								Function to set GPIO pin as Open drain configuration
		 * @param[in] uC_BASE_PIO_CTRL_STRUCT	Base GPIO peripheral structure
		 * @param[in] io_num					GPIO number
		 * @return uint8_t						returns 1 or 0.
		 */
		uint8_t Set_Open_Drain( uC_BASE_PIO_CTRL_STRUCT const* pio_ctrl, uint32_t io_num );

		/**
		 * @brief								Function to check the GPIO
		 * @param[in] validation_result			true if the validation is correct or else false will be sent
		 * @param[in] str						GPIO string used for displaying the error message
		 * @param[in] error_type				esp Error type
		 * @return esp_err_t					esp error type
		 */
		static esp_err_t Check_GPIO( bool validation_result, const char* str, esp_err_t error_type );

		/**
		 * @brief								Function sets the bit of an register
		 * @param[in] reg						Register to be set
		 * @param[in] bit_num					bit to be set
		 * @return void							None
		 */
		static inline void Set_Reg_Bit( uint32_t reg, uint16_t bit_num )
		{
			ASSERT_IF_DPORT_REG( ( reg ), REG_SET_BIT );                                                                    \
			( *( volatile uint32_t* )( reg ) |= ( bit_num ) );
		}

		/**
		 * @brief								Function resets the bit of an register
		 * @param[in] reg						Register to be reset
		 * @param[in] bit_num					bit to be reset
		 * @return void							None
		 */
		static inline void Reset_Reg_Bit( uint32_t reg, uint16_t bit_num )
		{
			ASSERT_IF_DPORT_REG( ( reg ), REG_CLR_BIT );                                                                    \
			( *( volatile uint32_t* )( reg ) &= ~( bit_num ) );
		}

		/**
		 * @brief								Function to check the GPIO
		 * @param[in] validation_result			true if the validation is correct or else false will be sent
		 * @param[in] str						GPIO string used for displaying the error message
		 * @param[in] error_type				esp Error type
		 * @return esp_err_t					esp error type
		 */
		static inline bool RTC_GPIO_Validation( gpio_num_t gpio_num )
		{
#ifdef RTC_ENABLE
			if ( ( gpio_num < GPIO_PIN_COUNT ) &&
				 ( rtc_gpio_desc[gpio_num].reg != 0 ) )
			{
				return ( true );
			}
			else
			{
				return ( false );
			}
#else
			return ( false );

#endif
		}

};

#ifdef __cplusplus
}
#endif

#endif
