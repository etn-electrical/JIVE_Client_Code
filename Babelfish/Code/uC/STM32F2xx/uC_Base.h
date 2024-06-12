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
enum
{
	RCC_PERIPH_CLCK_AHB1,
	RCC_PERIPH_CLCK_AHB2,
	RCC_PERIPH_CLCK_AHB3,
	RCC_PERIPH_CLCK_APB1,
	RCC_PERIPH_CLCK_APB2,
	RCC_PERIPH_CLCK_MAX
};

typedef struct	// PERIPH_DEF_ST
{
	uint8_t pclock_reg_index;
	uint32_t pid;
} PERIPH_DEF_ST;


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
} uC_BASE_TIMER_IO_STRUCT;


/*
 ********************************
 *  USART Definition Structure
 */
typedef struct uC_BASE_USART_IO_PORT_STRUCT
{
	uC_PERIPH_IO_STRUCT const* tx_port;
	uC_PERIPH_IO_STRUCT const* rx_port;
} uC_BASE_USART_IO_PORT_STRUCT;


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


/*
 *********************************  SPI Control Definition Structure
 */
typedef struct uC_BASE_SPI_IO_PORT_STRUCT
{
	const uC_PERIPH_IO_STRUCT* mosi_pio;
	const uC_PERIPH_IO_STRUCT* miso_pio;
	const uC_PERIPH_IO_STRUCT* clock_pio;
	const uC_PERIPH_IO_STRUCT* ss_in_pio;
} uC_BASE_SPI_IO_PORT_STRUCT;

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
typedef void uC_BASE_I2C_INT_OBJ_CBACK_FUNC( uC_BASE_I2C_OBJ_PARAM_TD param );
typedef void uC_BASE_I2C_INT_CBACK_FUNC( void );

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
	IRQn_Type irq_num;
	uint16_t irq_assignment;		// This is a bitfield.  It equates to the vector associated with the IRQn.
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
	uC_PERIPH_IO_STRUCT const* const* adc_pio;
	uint8_t dma_channel;
} uC_BASE_ATOD_IO_STRUCT;

/*
 ********************************
 *  DMA Control Definition Structure
 */
typedef struct uC_BASE_DMA_IO_STRUCT
{
	DMA_TypeDef* dma_ctrl;
	DMA_Stream_TypeDef* dma_channel_ctrl;
	PERIPH_DEF_ST periph_def;
	IRQn_Type irq_num;
	uint8_t dma_num;				// Used for shifting.
} uC_BASE_DMA_IO_STRUCT;



/**************************************
 *  CAN Controller Definition Structure
 */
typedef struct uC_BASE_CAN_IO_PORT_STRUCT
{
	const uC_PERIPH_IO_STRUCT* tx_pio;
	const uC_PERIPH_IO_STRUCT* rx_pio;
} uC_BASE_CAN_IO_PORT_STRUCT;

typedef struct uC_BASE_CAN_IO_STRUCT
{
	CAN_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	IRQn_Type rx_irq_num;
	IRQn_Type tx_irq_num;
	uC_BASE_CAN_IO_PORT_STRUCT const* default_port_io;
} uC_BASE_CAN_IO_STRUCT;
/*
 ********************************
 *  EMAC Controller Definition Structure
 */

#define uC_BASE_MII_PIO_OUT_COUNT           7U
#define uC_BASE_MII_PIO_IN_COUNT            10U
#define uC_BASE_RMII_PIO_OUT_COUNT          5U
#define uC_BASE_RMII_PIO_IN_COUNT           4U

typedef struct uC_BASE_EMAC_IO_PORT_STRUCT
{
	uC_PERIPH_IO_STRUCT const* const* mii_pio_out;
	uC_PERIPH_IO_STRUCT const* const* mii_pio_in;
	uC_PERIPH_IO_STRUCT const* const* rmii_pio_out;
	uC_PERIPH_IO_STRUCT const* const* rmii_pio_in;
} uC_BASE_EMAC_IO_PORT_STRUCT;

typedef struct uC_BASE_EMAC_IO_STRUCT
{
#ifdef uC_STM32F207_USAGE
	ETH_TypeDef* reg_ctrl;
	PERIPH_DEF_ST periph_def;
	IRQn_Type irq_num;
	uint32_t rmii_sel_mask;
	// uC_PERIPH_IO_STRUCT const* mii_mco_out;
	uint8_t mii_pio_out_count;
	uint8_t mii_pio_in_count;
	uint8_t rmii_pio_out_count;
	uint8_t rmii_pio_in_count;
	uC_BASE_EMAC_IO_PORT_STRUCT const* default_port_io;

#endif
} uC_BASE_EMAC_IO_STRUCT;


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
		~uC_Base( void ) {}

	public:

		static uC_Base* Self( void )        { return ( m_myself ); }

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

		virtual uC_BASE_WDOG_CTRL_STRUCT const* Get_WDOG_Ctrl( uint8_t block = 0 ) = 0;

		virtual uC_BASE_PIO_CTRL_STRUCT const* Get_PIO_Ctrl( uint32_t io_block ) = 0;

		virtual uC_BASE_TIMER_IO_STRUCT const* Get_Timer_Ctrl( uint32_t timer_block ) = 0;

		virtual uC_BASE_USART_IO_STRUCT const* Get_USART_Ctrl( uint32_t port ) = 0;

		virtual uC_BASE_USB_IO_STRUCT const* Get_USB_Ctrl( uint8_t port ) {return ( ( uC_BASE_USB_IO_STRUCT const* )NULL );
		}

		virtual uC_BASE_SPI_IO_STRUCT const* Get_SPI_Ctrl( uint32_t spi_block ) = 0;

		virtual uC_BASE_I2C_IO_STRUCT const* Get_I2C_Ctrl( uint32_t i2c_block ) = 0;

		virtual uC_BASE_EXT_INT_IO_STRUCT const* Get_Ext_Int_Ctrl( uC_USER_IO_STRUCT const* pio_ctrl ) = 0;

		virtual uC_BASE_TIMER_IO_STRUCT const* Get_PWM_Ctrl( uint32_t pwm_block ) = 0;

		virtual uC_BASE_ATOD_IO_STRUCT const* Get_AtoD_Ctrl( uint32_t atod_block ) = 0;

		virtual uC_BASE_DMA_IO_STRUCT const* Get_DMA_Ctrl( uint32_t dma_channel ) = 0;

		virtual uC_BASE_EMAC_IO_STRUCT const* Get_EMAC_Ctrl( uint8_t ether_block ) = 0;

		virtual uint32_t Get_PClock_Freq( uint8_t pclock_selection ) = 0;

		// virtual uC_BASE_ATOD_IO_STRUCT const*		Get_AtoD_Ctrl( UINT32 atod_block ) = 0;
		// virtual uC_BASE_TWI_IO_STRUCT const*		Get_TWI_Ctrl( UINT32 twi_block ) = 0;
		virtual uC_BASE_CAN_IO_STRUCT const* Get_CAN_Ctrl( UINT32 can_block ) = 0;

	// virtual uC_BASE_EXT_MEM_IO_STRUCT const*	Get_Ext_Mem_Ctrl( void ) = 0;

	private:
		static uC_Base* m_myself;
		static uint32_t m_active_rmc;
		static volatile uint32_t* Get_Periph_Clk_Enable_Reg( PERIPH_DEF_ST const* peripheral );

};




/*
 ********************************
 *  SPI Control Definition Structure

   typedef struct uC_BASE_SPI_IO_STRUCT
   {
    _AT91S_SPI* reg_ctrl;
    uint8_t pid;

    uC_PERIPH_IO_STRUCT const* 	mosi_pio;
    uC_PERIPH_IO_STRUCT const* 	miso_pio;
    uC_PERIPH_IO_STRUCT const* 	clock_pio;

    uC_PERIPH_IO_STRUCT const** slave_select_pio;
    uint8_t slave_select_pin_count;
   } uC_BASE_SPI_IO_STRUCT;
 */
/*
 ********************************
 *  External Int Control Definition Structure

   typedef struct
   {
    uint8_t pid;

    uC_PERIPH_IO_STRUCT const* 		ext_int_pio;
   } uC_BASE_EXT_INT_IO_STRUCT;
 */
/*
 ********************************
 *  TWI Control Definition Structure

   typedef struct uC_BASE_TWI_IO_STRUCT
   {
    _AT91S_TWI* reg_ctrl;
    uint8_t pid;

    uC_PERIPH_IO_STRUCT const* 		data_pio;
    uC_PERIPH_IO_STRUCT const* 		clock_pio;
   } uC_BASE_TWI_IO_STRUCT;
 */
/*
 ********************************
 *  CAN Controller Definition Structure

   typedef struct uC_BASE_CAN_IO_STRUCT
   {
    _AT91S_CAN* reg_ctrl;
    _AT91S_CAN_MB* mailbox_ctrl;
    uint8_t pid;

    uC_PERIPH_IO_STRUCT const* 		rx_pio;
    uC_PERIPH_IO_STRUCT const* 		tx_pio;
   } uC_BASE_CAN_IO_STRUCT;
 */

/*
 ********************************
 *  External Memory Control Definition Structure

 #ifndef uC_BASE_EXT_MEM_DATA_LINES
 #define uC_BASE_EXT_MEM_DATA_LINES 		32
 #endif
 #ifndef uC_BASE_EXT_MEM_ADD_LINES
 #define uC_BASE_EXT_MEM_ADD_LINES			22
 #endif
 #ifndef uC_BASE_EXT_MEM_CHIPSEL_LINES
 #define uC_BASE_EXT_MEM_CHIPSEL_LINES		1
 #endif
   typedef struct uC_BASE_EXT_MEM_IO_STRUCT
   {
    _AT91S_EBI* ext_mem_ctrl;
    _AT91S_SMC2* static_mem_ctrl;
    _AT91S_SDRC* sdram_mem_ctrl;

    uC_PERIPH_IO_STRUCT const* 		read_select_pio;
    uC_PERIPH_IO_STRUCT const* 		write_select_pio;
    uC_PERIPH_IO_STRUCT const* 		wait_sig_pio;

    uC_PERIPH_IO_STRUCT const* 		chip_select_pio[uC_BASE_EXT_MEM_CHIPSEL_LINES];
    uC_PERIPH_IO_STRUCT const* 		data_lines_pio[uC_BASE_EXT_MEM_DATA_LINES];
    uC_PERIPH_IO_STRUCT const* 		address_lines_pio[uC_BASE_EXT_MEM_ADD_LINES];
   } uC_BASE_EXT_MEM_IO_STRUCT;
 */


#endif
