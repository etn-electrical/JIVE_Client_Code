/**
 **********************************************************************************************
 *  @file           uC_I2C.cpp C++ Implementation File for I2C software module.
 *
 *  @brief          The file shall include the definitions of all the functions required for
 *                  I2C communication and which are declared in corresponding header file
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @remark         Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "Includes.h"
#include "uC_I2C.h"
#include "Babelfish_Assert.h"

/** @brief I2C channel status either free or busy */
uC_I2C::channel_state_t uC_I2C::channel_busy[] =
{ CHANNEL_FREE, CHANNEL_FREE, CHANNEL_FREE };

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::uC_I2C( uint8_t channel, uint16_t device_address,
				i2c_mode_t speed_mode, address_type_t addr_mode, bool_t enable_dma,
				duty_type_t fastmode_duty_cycle, uC_BASE_I2C_IO_PORT_STRUCT const* io_port ) :
	channel_number( channel ),
	chip_address( device_address ),
	m_i2c( reinterpret_cast<I2C_TypeDef*>( nullptr ) ),
	addressing_mode( addr_mode ),
	dma_transfer( enable_dma ),
	m_config( reinterpret_cast<uC_BASE_I2C_IO_STRUCT*>( nullptr ) ),
	m_io_ctrl( io_port ),
	f_duty_cycle( FASTMODE_DUTYCYCLE_2 ),
	i2c_speed_type( speed_mode ),
	write_status( NO_ERROR_INIT ),
	read_status( NO_ERROR_INIT ),
	rx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	tx_dma( reinterpret_cast<uC_DMA*>( nullptr ) )
{
	Init_I2C( channel, speed_mode, fastmode_duty_cycle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Init_I2C( uint8_t i2c_channel, i2c_mode_t i2c_mode, duty_type_t fmode_duty_cycle )
{
	uint32_t i2c_pclk1_freq;
	uint16_t i2c_ccr_value;
	uint16_t fast_mode_duty = 0U;
	__IO UINT32
	* config_reg;
	uint8_t cfg_reg_shift;

	m_config = ( uC_Base::Self() )->Get_I2C_Ctrl( i2c_channel );
	BF_ASSERT( m_config != 0 );

	/* Get the base address of I2C channel */
	m_i2c = m_config->reg_ctrl;
	/* Put I2C peripheral in reset state */
	uC_Base::Reset_Periph( &m_config->periph_def );
	/*I2C Peripheral clock enable */
	uC_Base::Enable_Periph_Clock( &m_config->periph_def );

	/* Configure GPIOs for I2C(Serial Clock and serial data) functionalities */
	if ( m_io_ctrl == NULL )
	{
		m_io_ctrl = m_config->default_port_io;
	}
	uC_IO_Config::Enable_Periph_Output_Pin( m_io_ctrl->scl_port, true );
	if ( ( ( m_io_ctrl->scl_port )->pio_num ) < 8U )
	{
		config_reg = &( ( ( m_io_ctrl->scl_port )->reg_ctrl )->CRL );
	}
	else
	{
		config_reg = &( ( ( m_io_ctrl->scl_port )->reg_ctrl )->CRH );
	}
	cfg_reg_shift = ( ( m_io_ctrl->scl_port )->pio_num & 0x7U ) << MULT_BY_4;
	*config_reg |= ( ( static_cast<uint32_t>( GPIO_CRL_MODE0_0 ) << cfg_reg_shift )
					 | ( static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift ) );

	uC_IO_Config::Enable_Periph_Output_Pin( m_config->default_port_io->sda_port, true );
	if ( ( ( m_io_ctrl->sda_port )->pio_num ) < 8U )
	{
		config_reg = &( ( ( m_io_ctrl->sda_port )->reg_ctrl )->CRL );
	}
	else
	{
		config_reg = &( ( ( m_io_ctrl->sda_port )->reg_ctrl )->CRH );
	}
	cfg_reg_shift = ( ( m_io_ctrl->sda_port )->pio_num & 0x7U ) << MULT_BY_4;
	*config_reg |= ( ( static_cast<uint32_t>( GPIO_CRL_MODE0_0 ) << cfg_reg_shift )
					 | ( static_cast<uint32_t>( GPIO_CRL_CNF0 ) << cfg_reg_shift ) );

	m_i2c->CR1 |= I2C_CR1_SWRST;
	/* Control register Settings */
	m_i2c->CR1 = 0U;/* Reset Control register 1 */
	m_i2c->CR2 = 0U;/* Reset Control register 2 */

	/*Peripheral clock frequency*/
	i2c_pclk1_freq =
		( uC_Base::Self() )->Get_PClock_Freq( m_config->periph_def.pclock_reg_index );
	Set_Mask( m_i2c->CR2, I2C_CR2_FREQ, ( i2c_pclk1_freq / 1000000U ) );

	/* For CCR configuration the PE bit must be 0*/
	if ( true == static_cast<bool_t>( m_i2c->CR1 & I2C_CR1_PE ) )
	{
		Clr_Mask( m_i2c->CR1, I2C_CR1_PE );
	}

	if ( I2C_FAST_MODE == i2c_mode )
	{
		if ( FASTMODE_DUTYCYCLE_2 == fmode_duty_cycle )
		{
			i2c_ccr_value = static_cast<uint16_t>
				( i2c_pclk1_freq / ( I2C_ClockSpeed_400K * 3U ) );
			fast_mode_duty = static_cast<uint16_t>( 0x8000U );
		}
		else	// FASTMODE_DUTYCYCLE_16_9
		{
			i2c_ccr_value = static_cast<uint16_t>
				( i2c_pclk1_freq / ( I2C_ClockSpeed_400K * 25U ) );
			fast_mode_duty = static_cast<uint16_t>( 0xC000U );
		}
		if ( i2c_ccr_value < 1U )
		{
			/* Set minimum allowed value */
			i2c_ccr_value = 1U;
		}
		m_i2c->CCR = ( i2c_ccr_value | fast_mode_duty );
		m_i2c->TRISE = static_cast<uint16_t>
			( ( i2c_pclk1_freq * 3U ) / ( 10000000U ) ) + 1U;									// in
																								// fast
																								// mode,
																								// the
																								// maximum
																								// allowed
																								// SCL
																								// rise
																								// time
																								// is
																								// 300
																								// ns
	}
	else
	{
		i2c_ccr_value = static_cast<uint16_t>
			( i2c_pclk1_freq / ( I2C_ClockSpeed_100K << 1U ) );
		if ( i2c_ccr_value < 4U )
		{
			/* Set minimum allowed value */
			i2c_ccr_value = 4U;
		}
		m_i2c->CCR = i2c_ccr_value;
		m_i2c->TRISE = static_cast<uint16_t>( ( i2c_pclk1_freq ) / ( 1000000U ) ) + 1U;		// in
																							// standard
																							// mode,
																							// the
																							// maximum
																							// allowed
																							// SCL
																							// rise
																							// time
																							// is
																							// 1000
																							// ns
	}

	Enable_I2C();
	m_i2c->CR1 |= I2C_CR1_ACK;

	if ( ( m_config->rx_dma_channel != uC_DMA_CHANNEL_DNE ) &&
		 ( m_config->tx_dma_channel != uC_DMA_CHANNEL_DNE ) && ( true == dma_transfer ) )
	{
		uC_DMA::Deallocate_DMA_Stream( m_config->rx_dma_channel );
		rx_dma = new uC_DMA( m_config->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH,
							 sizeof( uint8_t ),
							 uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );

		uC_DMA::Deallocate_DMA_Stream( m_config->tx_dma_channel );
		tx_dma = new uC_DMA( m_config->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY,
							 sizeof( uint8_t ),
							 uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE, uC_DMA_CIRC_FALSE );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::i2c_status_t uC_I2C::Write( uint16_t address, uint16_t length, uint8_t* datasource )
{
	return_type error = NO_ERROR;
	uint8_t number_of_bytes = 0U;
	i2c_states_t state = START_STATE;

	write_status = NO_ERROR_INIT;
	read_status = NO_ERROR_INIT;
	/* This is necessary because one can't read and write at the same time on bus */
	channel_busy[channel_number] = CHANNEL_BUSY;
	/** Implement the I2C Write State machine here */
	Clear_Error();
	m_i2c->CR1 |= I2C_CR1_ACK;
	Bus_Busy_TimeOut();

	while ( state < MAX_STATE )
	{
		switch ( state )
		{
			case START_STATE:
				error = Generate_Start();
				write_status = Set_State( error, &state, SET_DEVICE_ADDRESS,
										  START_GENERATION_ERROR );
				break;

			case SET_DEVICE_ADDRESS:
				error = Send_Device_Address( chip_address, I2C_WRITE );
				write_status = Set_State( error, &state, SET_COMMAND, ADDRESS_ERROR );
				break;

			case SET_COMMAND:
				error = Send_Data( static_cast<uint8_t>( address ) );
				write_status = Set_State( error, &state, SEND_DATA, BYTE_TX_ERROR );
				break;

			case SEND_DATA:
				if ( false == dma_transfer )
				{
					for ( number_of_bytes = 0U;
						  ( number_of_bytes < length ) && ( NO_ERROR == error );
						  number_of_bytes++ )
					{
						error = Send_Data( *datasource );
						datasource++;
					}
					write_status = Set_State( error, &state, STOP_CONDITION, BYTE_TX_ERROR );
				}
				else
				{
					Set_TX_DMA( datasource, length );
					Enable_TX_DMA();
					state = MAX_STATE;
					write_status = I2C_DMA_TX_BUSY;
				}
				break;

			case STOP_CONDITION:
				error = Set_Stop();
				write_status = Set_State( error, &state, MAX_STATE, STOP_GENERATION_ERROR );
				write_status = I2C_FINISH_TX;
				break;

			/* Below cases to satisfy MISRA*/
			case REPEATED_START_STATE:

			case REPEAT_DEVICE_ADDRESS:

			case RECEIVE_DATA:

			case MAX_STATE:
			default:
				state = MAX_STATE;
				break;
		}
	}
	if ( false == dma_transfer )
	{
		channel_busy[channel_number] = CHANNEL_FREE;
	}
	else
	{
		while ( I2C_FINISH_TX != DMA_Transfer_Status() )
		{
#ifndef MODULE_TESTS
			OS_Tasker::Delay( 1 );
#endif
		}

		write_status = I2C_FINISH_TX;
	}

	return ( write_status );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::i2c_status_t uC_I2C::Read( uint16_t address, uint16_t length, uint8_t* datasource )
{
	return_type error = NO_ERROR;
	i2c_states_t state = START_STATE;
	uint8_t number_of_bytes = 0U;

	read_status = NO_ERROR_INIT;
	write_status = NO_ERROR_INIT;
	channel_busy[channel_number] = CHANNEL_BUSY;
	/** Implement the I2C Read State machine here */
	Clear_Error();
	m_i2c->CR1 |= I2C_CR1_ACK;
	Bus_Busy_TimeOut();

	while ( state < MAX_STATE )
	{
		switch ( state )
		{
			case START_STATE:
				error = Generate_Start();
				read_status = Set_State( error, &state, SET_DEVICE_ADDRESS,
										 START_GENERATION_ERROR );
				break;

			case SET_DEVICE_ADDRESS:
				error = Send_Device_Address( chip_address, I2C_WRITE );
				read_status = Set_State( error, &state, SET_COMMAND, ADDRESS_ERROR );
				break;

			case SET_COMMAND:
				error = Send_Data( static_cast<uint8_t>( address ) );
				read_status = Set_State( error, &state, REPEATED_START_STATE, BYTE_TX_ERROR );
				break;

			case REPEATED_START_STATE:
				error = Generate_Start();
				read_status = Set_State( error, &state, REPEAT_DEVICE_ADDRESS,
										 RSTART_GENERATION_ERROR );
				break;

			case REPEAT_DEVICE_ADDRESS:
				if ( true == dma_transfer )
				{
					if ( length == 1U )
					{
						m_i2c->CR1 &= ( ~I2C_CR1_ACK );
					}

					Set_RX_DMA( datasource, length );
					Enable_RX_DMA();
					state = MAX_STATE;
					read_status = I2C_DMA_RX_BUSY;
					Send_Device_Address( chip_address, I2C_READ );
				}
				else
				{
					error = Send_Device_Address( chip_address, I2C_READ );
					read_status = Set_State( error, &state, RECEIVE_DATA, ADDRESS_ERROR );
				}
				break;

			case RECEIVE_DATA:
				for ( number_of_bytes = 0U; ( number_of_bytes < length ) && ( NO_ERROR == error );
					  number_of_bytes++ )
				{
					if ( number_of_bytes == ( length - 1U ) )
					{
						m_i2c->CR1 &= ( ~I2C_CR1_ACK );
					}

					error = Receive_Data( datasource );
					datasource++;
				}
				read_status = Set_State( error, &state, STOP_CONDITION, BYTE_RX_ERROR );

				break;

			case STOP_CONDITION:
				error = Set_Stop();
				read_status = Set_State( error, &state, MAX_STATE, STOP_GENERATION_ERROR );
				read_status = I2C_FINISH_RX;
				break;

			case SEND_DATA:

			case MAX_STATE:

			default:
				state = MAX_STATE;
				break;
		}
	}
	if ( false == dma_transfer )
	{
		channel_busy[channel_number] = CHANNEL_FREE;
	}
	else
	{
		while ( I2C_FINISH_RX != DMA_Transfer_Status() )
		{
#ifndef MODULE_TESTS
			OS_Tasker::Delay( 1 );
#endif
		}

		read_status = I2C_FINISH_RX;
	}

	return ( read_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Send_Device_Address( uint16_t devaddr, uint8_t read_write )
{
	uint16_t device_address = 0U;
	return_type addrs_ack_status;

	if ( ADDRESS_10_BIT == addressing_mode )
	{
		device_address = ( ( devaddr >> 8U ) & 0x03U );
		device_address = static_cast<uint16_t>( device_address << 1U );
		m_i2c->DR = ( ADDR_HEADER_10_BIT | device_address | read_write );
		addrs_ack_status = Wait_Flag_Timeout( I2C_SR1_ADD10 );

		Get_SR1();
		Get_SR2();

		if ( I2C_WRITE == read_write )
		{
			device_address = devaddr & 0xFFU;
			m_i2c->DR = device_address;
			addrs_ack_status = Wait_Flag_Timeout( I2C_SR1_ADDR );
		}
		else
		{}

		Get_SR1();
		Get_SR2();

	}
	else
	{
		device_address = devaddr & 0x7FU;
		m_i2c->DR = ( ( static_cast<uint16_t>( devaddr << 1U ) ) | read_write );
		addrs_ack_status = Wait_Flag_Timeout( I2C_SR1_ADDR );
		Get_SR1();
		Get_SR2();
	}

	return ( addrs_ack_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Generate_Start( void )
{
	m_i2c->CR1 |= I2C_CR1_START;
	return ( Wait_Flag_Timeout( I2C_SR1_SB ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Set_Stop( void )
{
	m_i2c->CR1 &= ( ~I2C_CR1_ACK );
	m_i2c->CR1 |= I2C_CR1_STOP;
	Nop();
	return ( NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Send_Data( uint8_t data )
{
	m_i2c->DR = data;
	return ( Wait_Flag_Timeout( I2C_SR1_BTF | I2C_SR1_TXE ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Receive_Data( uint8_t* data ) const
{
	return_type receive_status = Wait_Flag_Timeout( I2C_SR1_RXNE );

	if ( receive_status == NO_ERROR )
	{
		*data = static_cast<uint8_t>( m_i2c->DR );
	}
	return ( receive_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::i2c_status_t uC_I2C::Set_State( return_type error, i2c_states_t* current_state,
										i2c_states_t next_state,
										i2c_status_t state_specific_error ) const
{
	i2c_status_t retval;

	if ( NO_ERROR == error )
	{
		*current_state = next_state;
		retval = NO_ERROR_INIT;
	}
	else
	{
		retval = state_specific_error;
		*current_state = MAX_STATE;
	}
	return ( retval );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Wait_Flag_Timeout( uint32_t flag_mask ) const
{
	uint32_t timeout_count = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * I2C_TIMEOUT );

	while ( ( ( Get_SR1() & flag_mask ) == 0x00U ) && ( timeout_count > 0U ) )
	{
		__no_operation();
		timeout_count--;
	}

	return ( ( timeout_count == 0U ) ? I2C_ERROR : NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_I2C::Is_Channel_Free( uint8_t channel_no )
{
	return ( ( channel_busy[channel_no] == CHANNEL_FREE ) ? true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Reset_I2C( void )
{
	m_i2c->CR1 |= I2C_CR1_SWRST;
	Init_I2C( channel_number, i2c_speed_type, f_duty_cycle );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Enable_I2C( void )
{
	m_i2c->CR1 |= I2C_CR1_PE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_I2C::Get_SR1( void ) const
{
	return ( m_i2c->SR1 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_I2C::Get_SR2( void ) const
{
	return ( m_i2c->SR2 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Clear_Error( void )
{
	m_i2c->SR1 &= ( ~CPAL_I2C_STATUS_ERR_MASK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	tx_dma->Disable();
	tx_dma->Set_Buff( data, ( uint8_t* ) ( &m_i2c->DR ), length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	rx_dma->Disable();
	rx_dma->Set_Buff( data, ( uint8_t* ) ( &m_i2c->DR ), length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Enable_RX_DMA( void )
{
	Push_TGINT();
	rx_dma->Enable();
	m_i2c->CR2 |= I2C_CR2_DMAEN | I2C_CR2_LAST;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Disable_RX_DMA( void )
{
	Push_TGINT();
	m_i2c->CR2 &= ~I2C_CR2_DMAEN;
	rx_dma->Disable();
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Enable_TX_DMA( void )
{
	Push_TGINT();
	tx_dma->Enable();
	m_i2c->CR2 |= I2C_CR2_DMAEN;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Disable_TX_DMA( void )
{
	Push_TGINT();
	m_i2c->CR2 &= ~I2C_CR2_DMAEN;
	tx_dma->Disable();
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::i2c_status_t uC_I2C::DMA_Transfer_Status( void )
{
	uint8_t dma_busy = 0U;

	if ( I2C_DMA_TX_BUSY == write_status )
	{
		dma_busy = tx_dma->Busy();
	}
	else if ( I2C_DMA_RX_BUSY == read_status )
	{
		dma_busy = rx_dma->Busy();
	}
	else
	{
		dma_busy = 0U;
	}

	if ( true == static_cast<bool_t>( dma_busy ) )
	{
		return ( ( write_status == I2C_DMA_TX_BUSY ) ? I2C_DMA_TX_BUSY : I2C_DMA_RX_BUSY );
	}
	else
	{
		/* This timeout is required in master as DMA just transfers byte and says "my work is
		   done". However unless and untill DR is not empty transmision is not complete.
		   Hence this explicit check. Found issue during EEPROM testing */
		Wait_Flag_Timeout( I2C_SR1_TXE );
		channel_busy[channel_number] = CHANNEL_FREE;
		Set_Stop();
		return (
			( write_status == I2C_DMA_TX_BUSY ) ? ( write_status = I2C_FINISH_TX ) : ( read_status =
																						   I2C_FINISH_RX ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_I2C::Bus_Busy_TimeOut( void )
{
	// Function updated to fix ICDUNIKITY-417 (Ethernet port Auto negotiation value didn’t update
	// other than 4)
	uint32_t timeout_1uSec = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * 10U );
	/*
	   10 Clock cycles are required to execute the following while loop.
	   So (PROCESSOR_FREQ_MHZ * 100 / 10U) count is for 1 uSec Delay.
	   I2C @100KHz requires 10uSec timeout. So we overdesigned timeout to give 25 uSec delay.
	 */
	volatile uint32_t timeout;

	timeout = timeout_1uSec * 25U;
	while ( ( ( ( ( uint16_t )m_i2c->SR2 ) & I2C_SR2_BUSY ) != 0U ) && ( timeout-- > 0U ) )
	{}
}
