@example  uC_Queued_I2C
@brief    The sample code to initialize the I2C of STM32Fx07 controller and sending commands to
		  24AA01 EEPROM chip using queue mechansism
@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run successfully the sample code.
@n        1. STM32Fx07 Card is fitted on the board.
@n        2. Enable SW14.1, SW14.2 switches. This connects I2C lines of controller to that of EEPROM chip.

@par Pin Configurations 
@details   Correctly configure the GPIO pins for I2C1(alternate function 4) in uC_IO_Define_STM32F207.cpp as below 
@n        1. const uC_PERIPH_IO_STRUCT 	I2C1_SCL_PIO_PB6    = { GPIOB, IO_PIN_6, uC_IO_ALTERNATE_MAP_4 }; //SCL pin
@n        2. const uC_PERIPH_IO_STRUCT 	I2C1_SDA_PIO_PB7   = { GPIOB, IO_PIN_7, uC_IO_ALTERNATE_MAP_4 }; //SDA Pin

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "uC_Queued_I2C.h"
#include "STM32F2xx_I2C.h"
#include "uC_Base_HW_IDs_STM32F207.h"
#include "Ram.h"
#include "Ram_Static.h"

#define EXAMPLE_I2C_PORT		uC_BASE_I2C_CTRL_1
uC_Queued_I2C* shared_i2c_ctrl;
/* These variables will become 1 when transaction is finished */
/* These variables are set o value 1 inside callbak function */
volatile uint8_t write_finish = 0;
volatile uint8_t read_finish = 0;
/* Call back function when exchange finishes */
static void Call_Back_Transmit_Finish( void* param );
static void Call_Back_Receive_Finish( void* param );

/*
 * brief                    Example Code for establishing I2C communication with EEPROM chip
 * note                     It queue up the segment for exchange with EEPROM. The exchange segments are
 *                          called one by and when all segments are exchanged callback function is called.
 * Remark                   Care should be taken while queue up the segments because some of the chips
 *                          requires some processing time before before they respond again.
 *                          These delays varies from chip to chip and should be taken care in application.
 */
void I2C_Example_Queue( void )
{
	/* Buffer transmit chunk 1 */
	static uint8_t tx_data_buffer1[10] =
			{ 0 };
	/* Buffer transmit chunk 2 */
	static uint8_t tx_data_buffer2[10] =
			{ 0 };
	/* Buffer receive chunk 1 */
	static uint8_t rx_data_buffer1[10] =
			{ 0 };
	/* Buffer receive chunk 2 */
	static uint8_t rx_data_buffer2[10] =
			{ 0 };
	static uint8_t constructor_init = 0;

	/* Structure for transmit exchange */
	uC_Queued_I2C::exchange_struct_t exchange_struct_tx;
	/* Structure for transmit exchange */
	uC_Queued_I2C::exchange_struct_t exchange_struct_rx;
	/* The chip address of EEPROM */
	const uint16_t EEPROM_ADDRESS = 0x50U;
#if defined (uC_STM32F207_USAGE) || defined (uC_STM32F205_USAGE)
	/* I2C Class config structure */
	static STM32F2xx_I2C::class_config_t ltk_eeprom_i2c_config;
	/* I2C channel settings structure */
	static STM32F2xx_I2C::select_config_t ltk_eeprom_i2c_selects;
#elif defined (uC_STM32F407_USAGE)
	/* I2C Class config structure */
	static STM32F4xx_I2C::class_config_t ltk_eeprom_i2c_config;
	/* I2C channel settings structure */
	static STM32F4xx_I2C::select_config_t ltk_eeprom_i2c_selects;
#endif
	/* Peripheral clock frequency */
	uint32_t peripheral_clock_freq;
	/* Transmit segment 1 */
	uC_Queued_I2C::exchange_segment_t exchange_segment_tx1;
	/* Transmit segment 2 */
	uC_Queued_I2C::exchange_segment_t exchange_segment_tx2;
	/* Receive segment 1 */
	uC_Queued_I2C::exchange_segment_t exchange_segment_rx1;
	/* Receive segment 2 */
	uC_Queued_I2C::exchange_segment_t exchange_segment_rx2;

	/* As an example we will write two chunks in EEPROM.
	 Each Chunk comprises of 8 bytes */

	/* We will write the first chunk at location 0x10 */
	tx_data_buffer1[0] = 0x20U; /* The first byte of tx chunk denotes location on EEPROM */
	tx_data_buffer1[1] = 0xFFU; /* Magic Byte 1 */
	tx_data_buffer1[2] = 0x88U; /* Magic Byte 2 */
	tx_data_buffer1[3] = 0x99U; /* Magic Byte 3 */
	tx_data_buffer1[4] = 0xEEU; /* Magic Byte 4 */
	tx_data_buffer1[5] = 0xBBU; /* Magic Byte 5 */
	tx_data_buffer1[6] = 0xCCU; /* Magic Byte 6 */
	tx_data_buffer1[7] = 0xDDU; /* Magic Byte 7 */
	tx_data_buffer1[8] = 0xAAU; /* Magic Byte 8 */

	/* We will write the second chunk at location 0x50 */
	tx_data_buffer2[0] = 0x30U; /* The first byte of tx chunk denotes location on EEPROM */
	tx_data_buffer2[1] = 0xEEU; /* Magic Byte 1 */
	tx_data_buffer2[2] = 0xAAU; /* Magic Byte 2 */
	tx_data_buffer2[3] = 0xCCU; /* Magic Byte 3 */
	tx_data_buffer2[4] = 0x55U; /* Magic Byte 4 */
	tx_data_buffer2[5] = 0x22U; /* Magic Byte 5 */
	tx_data_buffer2[6] = 0xBBU; /* Magic Byte 6 */
	tx_data_buffer2[7] = 0x44U; /* Magic Byte 7 */
	tx_data_buffer2[8] = 0x77U; /* Magic Byte 8 */

	/* Retrieve and store the I2C channel settings */
	/* Please provide the correct I2C channel number which will be used for interfacing */
	ltk_eeprom_i2c_config.i2c = ( uC_Base::Self() )->Get_I2C_Ctrl( uC_BASE_I2C_CTRL_1 );
	ltk_eeprom_i2c_config.i2c_io = ltk_eeprom_i2c_config.i2c->default_port_io;
	/* DMA channel settings */
	ltk_eeprom_i2c_config.rx_dma = ( uC_Base::Self() )->Get_DMA_Ctrl(
		Get_DMA_Stream_Num( ( ltk_eeprom_i2c_config.i2c->rx_dma_channel ) ) );
	ltk_eeprom_i2c_config.tx_dma = ( uC_Base::Self() )->Get_DMA_Ctrl(
		Get_DMA_Stream_Num( ( ltk_eeprom_i2c_config.i2c->tx_dma_channel ) ) );
	/* I2C interrupt priority */
	ltk_eeprom_i2c_config.int_priority = uC_INT_HW_PRIORITY_2;
	/* Get the PCLOCK frequency. I2C derives its clock from PCLOCK */
	peripheral_clock_freq = ( uC_Base::Self() )->Get_PClock_Freq(
		ltk_eeprom_i2c_config.i2c->periph_def.pclock_reg_index );
	/* Derive the register parameters based on Speed and PClock */
#if defined (uC_STM32F207_USAGE) || defined (uC_STM32F205_USAGE)
	ltk_eeprom_i2c_selects.
          Configure( STM32F2xx_I2C::I2C_STANDARD_SPEED, peripheral_clock_freq, STM32F2xx_I2C::DUTYCYCLE_FASTMODE_2, 0U );        
#elif defined (uC_STM32F407_USAGE)
	ltk_eeprom_i2c_selects.
          Configure( STM32F4xx_I2C::I2C_STANDARD_SPEED, peripheral_clock_freq, STM32F4xx_I2C::DUTYCYCLE_FASTMODE_2, 0U );
#endif

	/* Initialize the queued class */
#if defined (uC_STM32F207_USAGE) || defined (uC_STM32F205_USAGE)
		shared_i2c_ctrl = new STM32F2xx_I2C( &ltk_eeprom_i2c_config, &ltk_eeprom_i2c_selects );
#elif defined (uC_STM32F407_USAGE)
		shared_i2c_ctrl = new STM32F4xx_I2C( &ltk_eeprom_i2c_config, &ltk_eeprom_i2c_selects );
#endif

	/* We will queue up two transmit packets in transmit exchange */
	exchange_segment_tx1.tx_length = 9U;
	exchange_segment_tx1.rx_length = 0U;
	exchange_segment_tx1.tx_data = &tx_data_buffer1[0];
	exchange_segment_tx1.rx_data = reinterpret_cast<uint8_t *>( nullptr );
	exchange_segment_tx1.exchange_type = uC_Queued_I2C::I2C_WRITE_EXCHANGE;/*WriteExchange */
	/* The below statement appends/queue up next transmit segment for exchange */
	exchange_segment_tx1.next_exchange_segment = nullptr;
	exchange_segment_tx1.chip_address = EEPROM_ADDRESS;

	exchange_segment_tx2.tx_length = 9U;
	exchange_segment_tx2.rx_length = 0U;
	exchange_segment_tx2.tx_data = &tx_data_buffer2[0];
	exchange_segment_tx2.rx_data = reinterpret_cast<uint8_t *>( nullptr );
	/* The below statement appends/queue up next receive segment */
	exchange_segment_tx2.next_exchange_segment = nullptr;
	exchange_segment_tx2.exchange_type = uC_Queued_I2C::I2C_WRITE_EXCHANGE;/*Read Exchange */
	exchange_segment_tx2.chip_address = EEPROM_ADDRESS;

	exchange_struct_tx.exchange_segment = &exchange_segment_tx1;
	/* Atttch a callback function when transmit exchange finshes its transaction */
	exchange_struct_tx.post_exchange_cback = &Call_Back_Transmit_Finish;

	/* We will queue up two receive packets in one receive exchange */
	exchange_segment_rx1.tx_length = 1U;
	/* one byte which denotes location/address shall be sent to slave EEPROM*/
	exchange_segment_rx1.rx_length = 8U; /* 8 bytes from that location shall be received back */
	exchange_segment_rx1.tx_data = &tx_data_buffer1[0];
	exchange_segment_rx1.rx_data = &rx_data_buffer1[0];
	;
	exchange_segment_rx1.exchange_type = uC_Queued_I2C::I2C_READ_EXCHANGE;/*WriteExchange */
	/* The below statement appends/queue up next receive segment for exchange */
	exchange_segment_rx1.next_exchange_segment = &exchange_segment_rx2;
	exchange_segment_rx1.chip_address = EEPROM_ADDRESS;

	exchange_segment_rx2.tx_length = 1U;
	/* one byte which denotes location/address on EEPROM*/
	exchange_segment_rx2.rx_length = 8U;
	/* 8 bytes from that location shall be received back */
	exchange_segment_rx2.tx_data = &tx_data_buffer2[0];
	exchange_segment_rx2.rx_data = &rx_data_buffer2[0];
	exchange_segment_rx2.next_exchange_segment = nullptr;
	exchange_segment_rx2.exchange_type = uC_Queued_I2C::I2C_READ_EXCHANGE;/*Read Exchange */
	exchange_segment_rx2.chip_address = EEPROM_ADDRESS;
	/* Lets clear the transaction flag. The flag shall be set in callback function.
	 Callback function gets called when transaction is finished */
	write_finish = 0x00U;
	read_finish = 0x00;
	exchange_struct_rx.exchange_segment = &exchange_segment_rx1;
	/* Attach a callback function when receive exchange finishes its transaction */
	exchange_struct_rx.post_exchange_cback = &Call_Back_Receive_Finish;
	shared_i2c_ctrl->Process_Exchange( &exchange_struct_tx );
	/* The chip do not respond while it writes page */
	/* Hence this delay is required. However if you have more than one slaves on same I2C channel
	 then instead of this delay, you can queue up request for another slave */
	/* We can wait here until transaction is finished */
	while ( write_finish == 0x00U )
	{
	}
	uC_Delay( 6500U );
	/* page write delay */
	write_finish = 0x00;
	exchange_struct_tx.exchange_segment = &exchange_segment_tx2;
	shared_i2c_ctrl->Process_Exchange( &exchange_struct_tx );
	/* We can wait here until transaction is finished */
	while ( write_finish == 0x00U )
	{
	}
	uC_Delay( 6500U );

	shared_i2c_ctrl->Process_Exchange( &exchange_struct_rx );
	while ( read_finish == 0x00U )
	{
	}

}

/*
 * brief                    Callback function when transaction finishes
 */
static void Call_Back_Transmit_Finish( void* param )
{
	write_finish = 1U;
}

static void Call_Back_Receive_Finish( void* param )
{
	read_finish = 1U;
}

~~~~