/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_BASE_H
#define uC_BASE_H

#include "uC_Base_HW_IDs.h"

/*
 ********************************
 *  Peripheral Power Definition Structure
 */
static const uint8_t RCC_PERIPH_CLCK_AHB = 0U;
static const uint8_t RCC_PERIPH_CLCK_APB1 = 1U;
static const uint8_t RCC_PERIPH_CLCK_APB2 = 2U;
static const uint8_t RCC_PERIPH_CLCK_MAX = 3U;

typedef struct PERIPH_DEF_ST
{
	uint8_t pclock_reg_index;
	uint32_t pid;
} PERIPH_DEF_ST;

typedef struct
{
	uint16_t TIM_Channel;
	uint16_t TIM_ICPolarity;
	uint16_t TIM_ICSelection;
	uint16_t TIM_ICPrescaler;
	uint16_t TIM_ICFilter;
	uC_PERIPH_IO_STRUCT* port_io;
} uC_PERIPH_TIM_CAPTURE_STRUCT;

/**
 * @brief  TIM Time Base Init structure definition
 * @note   This sturcture is used with all TIMx.
 */

typedef struct
{
	uint16_t TIM_Prescaler;			/*!< Specifies the prescaler value used to divide the TIM clock.
									     This parameter can be a number between 0x0000 and 0xFFFF */

	uint16_t TIM_CounterMode;		/*!< Specifies the counter mode.
									     This parameter can be a value of @ref TIM_Counter_Mode */

	uint32_t TIM_Period;			/*!< Specifies the period value to be loaded into the active
									     Auto-Reload Register at the next update event.
									     This parameter must be a number between 0x0000 and 0xFFFF.  */

	uint16_t TIM_ClockDivision;		/*!< Specifies the clock division.
									    This parameter can be a value of @ref TIM_Clock_Division_CKD */

	uint8_t TIM_RepetitionCounter;	/*!< Specifies the repetition counter value. Each time the RCR downcounter
									     reaches zero, an update event is generated and counting restarts
									     from the RCR value (N).
									     This means in PWM mode that (N+1) corresponds to:
									        - the number of PWM periods in edge-aligned mode
									        - the number of half PWM period in center-aligned mode
									     This parameter must be a number between 0x00 and 0xFF.
									     @note This parameter is valid only for TIM1. */
} uC_PERIPH_TIM_CONFIG_STRUCT;

/*
 ********************************
 *  Watchdog Controller Definition Structure
 */
typedef struct uC_BASE_WDOG_CTRL_STRUCT
{
	IWDG_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph;
} uC_BASE_WDOG_CTRL_STRUCT;

/*
 ********************************
 *  PIO Controller Definition Structure
 */
typedef struct uC_BASE_PIO_CTRL_STRUCT
{
	GPIO_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	uint32_t const* set_output;
	uint32_t const* output_init;
	uint32_t const* set_input;
	uint32_t const* input_pullup;
	uint32_t const* input_pulldown;
} uC_BASE_PIO_CTRL_STRUCT;

/*
 ********************************
 *  Timer Block Definition Structure
 */
typedef struct uC_BASE_TIMER_IO_STRUCT
{
	TIM_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	uint8_t pclock_freq_index;
	IRQn_Type irq_num;
	IRQn_Type irq_num_cc;
	uC_PERIPH_IO_STRUCT const* const* timer_pio;
	uint8_t cnt_size_bytes;
	uint8_t num_compares;
	uC_PERIPH_TIM_CAPTURE_STRUCT Timer_Channel_Config[uC_BASE_MAX_TIMER_CAP_COMP_CH];
} uC_BASE_TIMER_IO_STRUCT;

/*
 ********************************
 *  USART Definition Structure
 */
typedef struct uC_BASE_USART_IO_PORT_STRUCT
{
	const uC_PERIPH_IO_STRUCT* tx_port;
	const uC_PERIPH_IO_STRUCT* rx_port;
} uC_BASE_USART_IO_PORT_STRUCT;
typedef void* uC_BASE_USART_OBJ_PARAM_TD;
typedef void uC_BASE_USART_INT_OBJ_CBACK_FUNC ( uC_BASE_USART_OBJ_PARAM_TD param );
typedef void uC_BASE_USART_INT_CBACK_FUNC ( void );
typedef struct uC_BASE_USART_IO_STRUCT
{
	USART_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	IRQn_Type irq_num;
	uint8_t usart_id;
	uint8_t rx_dma_channel;
	uint8_t tx_dma_channel;

	uC_BASE_USART_IO_PORT_STRUCT const* default_port_io;
} uC_BASE_USART_IO_STRUCT;

#if !defined ( STM32F100xB )
/*
 ********************************
 *  USB Definition Structure
 */
typedef struct uC_BASE_USB_IO_PORT_STRUCT
{
	uC_PERIPH_IO_STRUCT const* dm_pio;
	uC_PERIPH_IO_STRUCT const* dp_pio;
	uC_PERIPH_IO_STRUCT const* id_pio;
	uC_USER_IO_STRUCT const* vbus_pio;
} uC_BASE_USB_IO_PORT_STRUCT;

typedef struct uC_BASE_USB_IO_STRUCT
{
	PERIPH_DEF_ST periph_def;
	IRQn_Type irq_num;
	uC_BASE_USB_IO_PORT_STRUCT const* port_io;
} uC_BASE_USB_IO_STRUCT;
#endif

/********************************
 *  SPI Control Definition Structure
 */
typedef struct uC_BASE_SPI_IO_PORT_STRUCT
{
	const uC_PERIPH_IO_STRUCT* mosi_pio;
	const uC_PERIPH_IO_STRUCT* miso_pio;
	const uC_PERIPH_IO_STRUCT* clock_pio;
	const uC_PERIPH_IO_STRUCT* ss_in_pio;
} uC_BASE_SPI_IO_PORT_STRUCT;
/*
 ********************************
 *  SPI Control Definition Structure
 */
typedef struct uC_BASE_SPI_IO_STRUCT
{
	SPI_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	IRQn_Type irq_num;
	uint8_t rx_dma_channel;
	uint8_t tx_dma_channel;

	uC_BASE_SPI_IO_PORT_STRUCT const* default_port_io;

} uC_BASE_SPI_IO_STRUCT;
/*
 ********************************
 *  I2C Control Definition Structure
 */
typedef struct uC_BASE_I2C_IO_PORT_STRUCT
{
	const uC_PERIPH_IO_STRUCT* scl_port;
	const uC_PERIPH_IO_STRUCT* sda_port;
} uC_BASE_I2C_IO_PORT_STRUCT;

typedef void* uC_BASE_I2C_OBJ_PARAM_TD;
typedef void uC_BASE_I2C_INT_OBJ_CBACK_FUNC ( uC_BASE_I2C_OBJ_PARAM_TD param );
typedef void uC_BASE_I2C_INT_CBACK_FUNC ( void );

typedef struct uC_BASE_I2C_IO_STRUCT
{
	I2C_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	IRQn_Type irq_num;
	uint8_t rx_dma_channel;
	uint8_t tx_dma_channel;

	uC_BASE_I2C_IO_PORT_STRUCT const* default_port_io;

	uC_BASE_I2C_INT_OBJ_CBACK_FUNC** obj_func_Tx;
	uC_BASE_I2C_OBJ_PARAM_TD* obj_param_Tx;
	uC_BASE_I2C_INT_CBACK_FUNC* i2c_dma_int_cback_Tx;

	uC_BASE_I2C_INT_OBJ_CBACK_FUNC** obj_func_Rx;
	uC_BASE_I2C_OBJ_PARAM_TD* obj_param_Rx;
	uC_BASE_I2C_INT_CBACK_FUNC* i2c_dma_int_cback_Rx;

	uC_BASE_I2C_INT_OBJ_CBACK_FUNC** obj_func_i2c;
	uC_BASE_I2C_OBJ_PARAM_TD* obj_param_i2c;
	uC_BASE_I2C_INT_CBACK_FUNC* i2c_int_cback;

} uC_BASE_I2C_IO_STRUCT;

/*
 ********************************
 *  External Int Control Definition Structure
 */
typedef struct uC_BASE_EXT_INT_IO_STRUCT
{
	IRQn_Type irq_num;		// SSN: This number shows the interrupt comes from which Peripheral or
							// which external int line
	uint16_t irq_assignment;	// This is a bitfield.  It equates to the vector associated with the
								// IRQn.
} uC_BASE_EXT_INT_IO_STRUCT;

/*
 ********************************
 *  AtoD Control Definition Structure
 */
typedef struct uC_BASE_ATOD_IO_STRUCT
{
	ADC_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	uint8_t pclock_freq_index;
	IRQn_Type irq_num;
	uint8_t max_channels;
	uC_PERIPH_IO_STRUCT const** adc_pio;
	uint8_t max_io_channels;
	uint8_t dma_channel;
} uC_BASE_ATOD_IO_STRUCT;

/*
 ********************************
 *  DMA Control Definition Structure
 */
typedef struct uC_BASE_DMA_IO_STRUCT
{
	DMA_TypeDef* dma_ctrl;
	DMA_Channel_TypeDef* dma_channel_ctrl;
	PERIPH_DEF_ST periph_def;
	IRQn_Type irq_num;
	uint8_t dma_num;		// Used for shifting.
} uC_BASE_DMA_IO_STRUCT;

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Base
{
	protected:
		uC_Base( void );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Base( void )
		{}

	public:

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static uC_Base* Self( void )
		{
			return ( m_myself );
		}

		// Enable Run Mode Clocking behavior.  This will active the bit associated with the
		// peripheral id passed in.
		static void Enable_Periph_Clock( PERIPH_DEF_ST const* peripheral );

		// Enable Run Mode Clocking behavior.  This will active the bit associated with the
		// peripheral id passed in.
		static void Disable_Periph_Clock( PERIPH_DEF_ST const* peripheral );

		// Enable Run Mode Clocking behavior.  This will active the bit associated with the
		// peripheral id passed in.
		static bool Is_Periph_Clock_Enabled( PERIPH_DEF_ST const* peripheral );

		// Reset the peripheral.
		static void Reset_Periph( PERIPH_DEF_ST const* peripheral );

		static uint32_t* Get_Periph_Clk_Enable_Reg( PERIPH_DEF_ST const* peripheral );

		virtual uC_BASE_WDOG_CTRL_STRUCT const* Get_WDOG_Ctrl( uint8_t block = 0 ) = 0;

		virtual uC_BASE_PIO_CTRL_STRUCT const* Get_PIO_Ctrl( uint32_t io_block ) = 0;

		virtual uC_BASE_TIMER_IO_STRUCT const* Get_Timer_Ctrl( uint32_t timer_block ) = 0;

		virtual uC_BASE_USART_IO_STRUCT const* Get_USART_Ctrl( uint32_t port ) = 0;

		#if !defined ( STM32F100xB )
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual uC_BASE_USB_IO_STRUCT const* Get_USB_Ctrl( uint8_t port )
		{
			return ( ( uC_BASE_USB_IO_STRUCT const* )NULL );
		}

		#endif
		virtual uC_BASE_SPI_IO_STRUCT const* Get_SPI_Ctrl( uint32_t spi_block ) = 0;

		virtual uC_BASE_I2C_IO_STRUCT const* Get_I2C_Ctrl( uint32_t i2c_block ) = 0;

		virtual uC_BASE_EXT_INT_IO_STRUCT const* Get_Ext_Int_Ctrl(
			uC_USER_IO_STRUCT const* pio_ctrl ) = 0;

		virtual uC_BASE_TIMER_IO_STRUCT const* Get_PWM_Ctrl( uint32_t pwm_block ) = 0;

		virtual uC_BASE_ATOD_IO_STRUCT const* Get_AtoD_Ctrl( uint32_t atod_block ) = 0;

		virtual uC_BASE_DMA_IO_STRUCT const* Get_DMA_Ctrl( uint32_t dma_channel ) = 0;

		// virtual uC_BASE_EMAC_IO_STRUCT const*		Get_EMAC_Ctrl( uint8_t ether_block ) = 0;

		virtual uint32_t Get_PClock_Freq( uint8_t pclock_selection ) = 0;

	// virtual uC_BASE_ATOD_IO_STRUCT const*		Get_AtoD_Ctrl( UINT32 atod_block ) = 0;
	// virtual uC_BASE_TWI_IO_STRUCT const*		Get_TWI_Ctrl( UINT32 twi_block ) = 0;
	// virtual uC_BASE_EXT_MEM_IO_STRUCT const*	Get_Ext_Mem_Ctrl( void ) = 0;

	private:
		static uC_Base* m_myself;
		static uint32_t m_active_rmc;
};

/*
 ********************************
 *  SPI Control Definition Structure

   typedef struct uC_BASE_SPI_IO_STRUCT
   {
   _AT91S_SPI* reg_ctrl;
   uint8_t pid;

   uC_PERIPH_IO_STRUCT const*   mosi_pio;
   uC_PERIPH_IO_STRUCT const*   miso_pio;
   uC_PERIPH_IO_STRUCT const*   clock_pio;

   uC_PERIPH_IO_STRUCT const** slave_select_pio;
   uint8_t slave_select_pin_count;
   } uC_BASE_SPI_IO_STRUCT;
 */

#endif
