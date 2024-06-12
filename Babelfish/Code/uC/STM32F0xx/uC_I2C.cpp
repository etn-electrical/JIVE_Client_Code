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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::uC_I2C( uint8_t channel, uint16_t device_address,
				i2c_mode_t speed_mode, address_type_t addr_mode, bool_t synchronus_trnasfer,
				bool_t enable_dma, uC_BASE_I2C_IO_PORT_STRUCT const* io_port ) :
	m_channel_number( channel ),
	m_chip_address( device_address ),
	m_i2c( reinterpret_cast<I2C_TypeDef*>( nullptr ) ),
	m_addressing_mode( addr_mode ),
	m_dma_transfer( enable_dma ),
	m_config( reinterpret_cast<uC_BASE_I2C_IO_STRUCT*>( nullptr ) ),
	m_io_ctrl( io_port ),
	m_i2c_speed_type( speed_mode ),
	m_transfer_status( NO_ERROR_INIT ),
	read_status( NO_ERROR_INIT ),
	m_rx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_tx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_call_back_ptr( nullptr ),
	m_cback_param( nullptr ),
	m_state_machine( START_STATE ),
	m_tx_data( nullptr ),
	m_rx_data( nullptr ),
	m_command_t( 0U ),
	m_write_exchange( false ),
	m_transfer_length( 0U ),
	write_status( NO_ERROR_INIT ),
	m_i2c_vector_obj( nullptr )
{
	m_i2c_speed_type = speed_mode;
	m_channel_number = channel;
	m_chip_address = device_address;
	m_addressing_mode = addr_mode;
	m_dma_transfer = enable_dma;
	m_synchronous_type = synchronus_trnasfer;
	Init_I2C( m_channel_number, m_i2c_speed_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Init_I2C( uint8_t i2c_channel, i2c_mode_t i2c_mode )
{
	__IO UINT32* config_reg;

	m_config = ( uC_Base::Self() )->Get_I2C_Ctrl( i2c_channel );
	BF_ASSERT( m_config != 0 );

	/*****************************************************************************************
	*                      Port configuration for I2C
	*****************************************************************************************/

	// ==============================================================================
	// I2C Clock  Configuration
	// ==============================================================================
	/* Get the base address of I2C channel */
	m_i2c = m_config->reg_ctrl;
	if ( i2c_channel == uC_BASE_I2C_CTRL_1 )
	{
		/* I2C1 clock source selection
		 * I2C1 can be clocked from HSI(Default) or SYCCLK
		 */
		RCC->CFGR3 |= RCC_CFGR3_I2C1SW_SYSCLK;
	}
	/* Put I2C peripheral in reset state */
	uC_Base::Reset_Periph( &m_config->periph_def );
	/*I2C Peripheral clock enable */
	uC_Base::Enable_Periph_Clock( &m_config->periph_def );

	/* Configure GPIOs for I2C(Serial Clock and serial data) functionalities */
	if ( m_io_ctrl == NULL )
	{
		m_io_ctrl = m_config->default_port_io;
	}

	// ==============================================================================
	// I2C SCL port  Configuration
	// ==============================================================================

	uC_IO_Config::Enable_Periph_Output_Pin( m_io_ctrl->scl_port, true );
	/* Get the access to output type register */
	config_reg = &( ( ( m_io_ctrl->scl_port )->reg_ctrl )->OTYPER );
	/* Set the pin to Open drain */
	*config_reg &= ( ~( 0x00000001 << ( ( m_io_ctrl->scl_port )->pio_num ) ) );
	*config_reg |= ( 0x00000001 << ( ( m_io_ctrl->scl_port )->pio_num ) );

	/* Get the access to Speed register */
	config_reg = &( m_io_ctrl->scl_port )->reg_ctrl->OSPEEDR;
	/* Clear the pin speed configuration*/
	*config_reg &= ~( 0x00000003 << ( ( m_io_ctrl->scl_port )->pio_num * MULT_BY_4 ) );
	/* Set the port pin to High speed mode */
	*config_reg |= ( 0x00000003 << ( ( m_io_ctrl->scl_port )->pio_num * MULT_BY_4 ) );

	/* Get the access to Pull up pull down register */
	config_reg = &( m_io_ctrl->scl_port )->reg_ctrl->PUPDR;
	/* Clear the pin Pull up pull down configuration*/
	*config_reg &= ~( 0x00000003 << ( ( m_io_ctrl->scl_port )->pio_num * MULT_BY_4 ) );
	/* Set the port pin to pull up */
	// *config_reg |= ( 0x00000001 << ( ( m_io_ctrl->scl_port )->pio_num * MULT_BY_4 ) );

	// ==============================================================================
	// I2C SDA port  Configuration
	// ==============================================================================

	uC_IO_Config::Enable_Periph_Output_Pin( m_config->default_port_io->sda_port, true );
	/* Get the access to output type register */
	config_reg = &( ( ( m_io_ctrl->sda_port )->reg_ctrl )->OTYPER );
	/* Set the pin to Open drain */
	*config_reg &= ~( 0x00000001 << ( ( m_io_ctrl->sda_port )->pio_num ) );
	*config_reg |= ( 0x00000001 << ( ( m_io_ctrl->sda_port )->pio_num ) );

	/* Get the access to Speed register */
	config_reg = &( m_io_ctrl->sda_port )->reg_ctrl->OSPEEDR;
	/* Clear the pin speed configuration*/
	*config_reg &= ~( 0x00000003 << ( ( m_io_ctrl->sda_port )->pio_num * MULT_BY_4 ) );
	/* Set the port pin to High speed mode */
	*config_reg |= ( 0x00000003 << ( ( m_io_ctrl->sda_port )->pio_num * MULT_BY_4 ) );

	/* Get the access to Pull up pull down register */
	config_reg = &( m_io_ctrl->sda_port )->reg_ctrl->PUPDR;
	/* Clear the pin ull up pull down configuration*/
	*config_reg &= ~( 0x00000003 << ( ( m_io_ctrl->sda_port )->pio_num * MULT_BY_4 ) );
	/* Set the port pin to pull up speed mode */
	// *config_reg |= ( 0x00000001 << ( ( m_io_ctrl->sda_port )->pio_num * MULT_BY_4 ) );

	/*****************************************************************************************
	*                      I2C initialization
	*****************************************************************************************/

	/* Control register Settings */
	m_i2c->CR1 = 0U;/* Reset Control register 1 */
	m_i2c->CR2 = 0U;/* Reset Control register 2 */

	/*---------------------------- I2Cx TIMINGR Configuration ------------------*/
	m_i2c->TIMINGR = ( m_i2c_speed_type == I2C_STANDARD_MODE ) ?
		( uint32_t )I2C_TIMINGR_100K : ( uint32_t )I2C_TIMINGR_400K;

	/*---------------------------- I2Cx CR2 Configuration ----------------------*/
	if ( m_addressing_mode == ADDRESS_10_BIT )
	{
		m_i2c->CR2 |= I2C_CR2_ADD10_Msk;
	}
	else
	{
		/* 7-bit addressing mode selected */
		m_i2c->CR2 &= ~( I2C_CR2_ADD10_Msk );
	}

	m_transfer_status = NO_ERROR_INIT;
	Enable_I2C();

	if ( ( false == m_synchronous_type ) && ( false == m_dma_transfer ) )
	{

		m_i2c_vector_number = m_config->irq_num;
		/* Configure the interrupt for I2C events */
		m_i2c_vector_obj = new Advanced_Vect();

		m_i2c_vector_obj->Set_Vect( &I2C_ISR_Static,
									static_cast<Advanced_Vect::cback_param_t>( this ) );
		uC_Interrupt::Clear_Vector( m_i2c_vector_number );
		uC_Interrupt::Set_Vector(
			static_cast<Advanced_Vect::int_cback_t>( m_i2c_vector_obj->Get_Int_Func() ),
			m_i2c_vector_number, uC_INT_HW_PRIORITY_11 );
		m_data_sink = m_i2c->TXDR;
		m_data_sink = m_i2c->RXDR;
		/* Enable I2C interrupt vector event */
		uC_Interrupt::Enable_Int( m_i2c_vector_number );

	}
	if ( true == m_dma_transfer )
	{
		uC_DMA::Deallocate_DMA_Stream( m_config->rx_dma_channel );
		m_rx_dma = new uC_DMA( m_config->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH,
							   sizeof( uint8_t ), uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE,
							   uC_DMA_CIRC_FALSE );

		m_rx_dma->Clear_Int();

		/* We need only one call back function for both TX and RX as both the DMA
		 * channel we are going to use is sharing a single interrupt line. */
		/*m_rx_dma->Attach_Callback( &RX_DMA_Int_Static,
		                           static_cast < uC_DMA::cback_param_t > ( this ),
		                           static_cast < uint8_t > ( uC_INT_HW_PRIORITY_11 ) );*/

		uC_DMA::Deallocate_DMA_Stream( m_config->tx_dma_channel );
		m_tx_dma = new uC_DMA( m_config->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY,
							   sizeof( uint8_t ), uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE,
							   uC_DMA_CIRC_FALSE );

		m_tx_dma->Clear_Int();
		m_tx_dma->Attach_Callback( &TXRX_DMA_Int_Static,
								   static_cast<uC_DMA::cback_param_t>( this ),
								   static_cast<uint8_t>( uC_INT_HW_PRIORITY_11 ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_I2C::Write( uint16_t address, uint16_t length, uint8_t* datasource,
					i2c_status_t* write_status )
{
	return_type error = NO_ERROR;
	uint8_t number_of_bytes = 0U;
	i2c_states_t state = START_STATE;

	m_transfer_status = NO_ERROR_INIT;
	m_command_t = address;

	/** Check for Bus Busy Time out */
	Bus_Busy_TimeOut();
	Clear_Error();

	/* Clear CR2 configuration */
	m_i2c->CR2 &= ~( I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R |
					 I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_NBYTES | I2C_CR2_SADD );
	/* Slave Address */
	m_i2c->CR2 |= ( m_chip_address << 1U ) & I2C_CR2_SADD;
	/* Load the number of bytes to be transferred. Two byte extra for Slave address and CMD address*/
	m_i2c->CR2 |= ( static_cast<uint32_t>( length + 2U ) << I2C_CR2_NBYTES_Pos ) & I2C_CR2_NBYTES;
	/* Master requests a write transfer */
	m_i2c->CR2 &= ~( I2C_CR2_RD_WRN );

	if ( ( m_dma_transfer == false ) && ( m_synchronous_type == false ) )
	{
		m_transfer_length = length;
		m_write_exchange = true;
		m_tx_data = datasource;
		m_transfer_status = I2C_INT_COMM_BUSY;
		m_state_machine = START_STATE;

		/* Error detection interrupt & Transmit interrupt enabled */
		// uC_Interrupt::Clear_Int( m_i2c_vector_number );
		m_i2c->CR1 &= ~( I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE
						 | I2C_CR1_ERRIE );
		m_i2c->CR1 |= I2C_CR1_TXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE | I2C_CR1_ERRIE;
		// uC_Interrupt::Enable_Int( m_i2c_vector_number );

		/* START bit is set in the I2C_CR2 register */
		m_i2c->CR2 |= I2C_CR2_START;
	}
	else
	{
		while ( state < MAX_STATE )
		{
			switch ( state )
			{
				case START_STATE:
					error = Generate_Start();
					m_transfer_status = Set_State( error, &state, SEND_DATA,
												   START_GENERATION_ERROR );
					break;

				case SEND_DATA:
					if ( false == m_dma_transfer )
					{

						for ( number_of_bytes = 0U;
							  ( number_of_bytes < ( length + 1U ) ) && ( NO_ERROR == error );
							  number_of_bytes++ )
						{
							if ( number_of_bytes == 0U )
							{
								error = Send_Data( static_cast<uint8_t>( address ) );
							}
							else
							{
								error = Send_Data( *datasource );
								datasource++;
							}
						}
						m_transfer_status =
							Set_State( error, &state, STOP_CONDITION, BYTE_TX_ERROR );
					}
					else
					{
						error = Send_Data( static_cast<uint8_t>( address ) );
						if ( NO_ERROR == error )
						{
							/* Enable transmission request and set buffer for data*/
							Set_TX_DMA( datasource, length );
							state = MAX_STATE;
							m_transfer_status = I2C_DMA_TX_BUSY;
							/*Clear Channel Global interrupt flag(GIF,TEIF,HTIF and TCIF)*/
							m_tx_dma->Clear_Int();
							/*Transmission complete interrupt enable*/
							m_tx_dma->Enable_End_Buff_Int();
							/* Enable DMA interrupt*/
							m_tx_dma->Enable_Int();
							/* Enable transmission request */
							Enable_TX_DMA();
						}
						else
						{
							m_transfer_status =
								Set_State( error, &state, MAX_STATE, BYTE_TX_ERROR );
						}
					}
					break;

				case STOP_CONDITION:
					error = Set_Stop();
					m_transfer_status = Set_State( error, &state, MAX_STATE,
												   STOP_GENERATION_ERROR );
					m_transfer_status = I2C_FINISH_TX;
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
	}

	if ( write_status != nullptr )
	{
		*write_status = m_transfer_status;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Read( uint16_t address, uint16_t length, uint8_t* datasource,
				   i2c_status_t* read_status )
{
	return_type error = NO_ERROR;
	i2c_states_t state = START_STATE;
	uint8_t number_of_bytes = 0U;

	m_transfer_status = NO_ERROR_INIT;
	m_command_t = address;

	Bus_Busy_TimeOut();
	Clear_Error();

	/* Clear CR2 configuration */
	m_i2c->CR2 &= ~( I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R |
					 I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_NBYTES | I2C_CR2_SADD );
	/* Set the slave address */
	m_i2c->CR2 |= ( m_chip_address << 1U ) & I2C_CR2_SADD;
	/* Load the number of bytes to be transferred.Two byte extra for Slave address and CMD address */
	m_i2c->CR2 |= ( ( 2U << I2C_CR2_NBYTES_Pos ) & I2C_CR2_NBYTES );
	/* Master requests a read transfer But First we have to write the register address */
	// m_i2c->CR2 |= I2C_CR2_RD_WRN;
	m_i2c->CR2 &= ~( I2C_CR2_RD_WRN );

	if ( ( m_dma_transfer == false ) && ( m_synchronous_type == false ) )
	{
		m_transfer_length = length;
		m_write_exchange = false;
		m_rx_data = datasource;
		m_transfer_status = I2C_INT_COMM_BUSY;
		m_state_machine = START_STATE;

		/*uC_Interrupt::Clear_Int( m_i2c_vector_number );*/
		m_i2c->CR1 &= ~( I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE
						 | I2C_CR1_ERRIE );
		m_i2c->CR1 |= ( I2C_CR1_TXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE
						| I2C_CR1_ERRIE );
		/* uC_Interrupt::Enable_Int( m_i2c_vector_number );
		   Start the transmission and request for read transmission*/
		m_i2c->CR2 |= ( I2C_CR2_START );
	}
	/** Implement the I2C Read State machine here */
	else
	{
		while ( state < MAX_STATE )
		{
			switch ( state )
			{
				case START_STATE:
					error = Generate_Start();
					m_transfer_status = Set_State( error, &state, SET_COMMAND,
												   START_GENERATION_ERROR );
					break;

				case SET_COMMAND:
					error = Send_Data( static_cast<uint8_t>( address ) );
					m_transfer_status = Set_State( error, &state, REPEATED_START_STATE,
												   BYTE_TX_ERROR );
					break;

				case REPEATED_START_STATE:
					error = Set_Stop();
					if ( error == NO_ERROR )
					{
						m_i2c->ICR |= I2C_ICR_ADDRCF;
						m_i2c->CR2 &= ~( I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R |
										 I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_NBYTES );
						/* Load the number of bytes to be transferred */
						m_i2c->CR2 |= ( static_cast<uint32_t>( length ) << I2C_CR2_NBYTES_Pos ) & I2C_CR2_NBYTES;
						m_i2c->CR2 |= I2C_CR2_RD_WRN;

						error = Generate_Start();
						m_transfer_status = Set_State( error, &state, RECEIVE_DATA,
													   RSTART_GENERATION_ERROR );
					}
					else
					{
						m_transfer_status = Set_State( error, &state, MAX_STATE,
													   RSTART_GENERATION_ERROR );
					}
					break;

				case RECEIVE_DATA:
					if ( true == m_dma_transfer )
					{
						Set_RX_DMA( datasource, length );
						state = MAX_STATE;
						m_transfer_status = I2C_DMA_RX_BUSY;
						m_rx_dma->Clear_Int();
						m_rx_dma->Enable_End_Buff_Int();
						m_rx_dma->Enable_Int();
						Enable_RX_DMA();
					}
					else
					{
						for ( number_of_bytes = 0U;
							  ( number_of_bytes < length ) && ( NO_ERROR == error );
							  number_of_bytes++ )
						{
							error = Receive_Data( datasource );
							datasource++;
						}
						m_transfer_status = Set_State( error, &state, STOP_CONDITION, BYTE_RX_ERROR );
					}
					break;

				case STOP_CONDITION:
					error = Set_Stop();
					m_transfer_status = Set_State( error, &state, MAX_STATE, STOP_GENERATION_ERROR );
					m_transfer_status = I2C_FINISH_RX;
					break;

				default:
					state = MAX_STATE;
					break;
			}
		}
	}
	if ( read_status != nullptr )
	{
		*read_status = m_transfer_status;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Generate_Start( void )
{
	m_i2c->CR2 |= I2C_CR2_START;
	return ( Wait_Flag_Timeout( I2C_ISR_BUSY ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Set_Stop( void )
{
	m_i2c->CR2 |= I2C_CR2_STOP;
	m_i2c->ICR |= I2C_ICR_STOPCF;
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
	m_i2c->TXDR = data;
	return ( Wait_Flag_Timeout( I2C_ISR_TXIS ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Receive_Data( uint8_t* data ) const
{
	return_type receive_status = Wait_Flag_Timeout( I2C_ISR_RXNE );

	if ( receive_status == NO_ERROR )
	{
		*data = static_cast<uint8_t>( m_i2c->RXDR );
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

	while ( ( ( Get_ISR() & flag_mask ) == 0x00 ) && ( timeout_count > 0U ) )
	{
		__no_operation();
		timeout_count--;

		if ( flag_mask == I2C_ISR_RXNE )
		{
			if ( ( Get_ISR() & ( static_cast<uint32_t>( I2C_ISR_NACKF ) ) ) == 1 )
			{
				timeout_count = 0;
			}
			else if ( ( Get_ISR() & ( static_cast<uint32_t>( I2C_ISR_STOPF ) ) ) == 1 )
			{
				timeout_count = 0;
				/* Clear STOP Flag */
				m_i2c->ICR |= I2C_ICR_STOPCF;
			}
			else
			{
				/* Do Nothing*/
			}
		}
		else if ( flag_mask == I2C_ISR_TXIS )
		{
			if ( ( Get_ISR() & ( static_cast<uint32_t>( I2C_ISR_NACKF ) ) ) == 1 )
			{
				timeout_count = 0;
			}
			else
			{
				/* Do Nothing*/
			}
		}
		else
		{
			/* Do Nothing*/
		}
	}
	return ( ( timeout_count == 0U ) ? I2C_ERROR : NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Reset_I2C( void )
{
	m_i2c->CR1 |= I2C_CR1_SWRST;
	Init_I2C( m_channel_number, m_i2c_speed_type );

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
uint16_t uC_I2C::Get_ISR( void ) const
{
	return ( m_i2c->ISR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Clear_Error( void )
{
	m_i2c->ICR |= ( CPAL_I2C_ERR_MASK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	m_tx_dma->Disable();
	/*Enable transmission request */
	m_i2c->CR1 |= I2C_CR1_TXDMAEN;
	m_tx_dma->Set_Buff( data, ( uint8_t* ) ( &m_i2c->TXDR ), length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	m_rx_dma->Disable();
	/* Enable DMA request */
	m_i2c->CR1 |= I2C_CR1_RXDMAEN;
	m_rx_dma->Set_Buff( data, ( uint8_t* ) ( &m_i2c->RXDR ), length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Enable_RX_DMA( void )
{
	Push_TGINT()
	;
	m_rx_dma->Enable();
	m_i2c->CR1 |= I2C_CR1_RXDMAEN;
	m_i2c->CR2 |= I2C_CR2_NACK;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Disable_RX_DMA( void )
{
	Push_TGINT()
	;
	m_i2c->CR1 &= ~I2C_CR1_RXDMAEN;
	m_rx_dma->Disable();
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Enable_TX_DMA( void )
{
	Push_TGINT()
	;
	m_tx_dma->Enable();
	m_i2c->CR1 |= I2C_CR1_TXDMAEN;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Disable_TX_DMA( void )
{
	Push_TGINT()
	;
	m_i2c->CR1 &= ~I2C_CR1_TXDMAEN;
	m_tx_dma->Disable();
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_I2C::Bus_Busy_TimeOut( void )
{
	uint32_t timeout_1uSec = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * 10U );
	/*
	   10 Clock cycles are required to execute the following while loop.
	   So (PROCESSOR_FREQ_MHZ * 100 / 10U) count is for 1 uSec Delay.
	   I2C @100KHz requires 10uSec timeout. So we overdesigned timeout to give 25 uSec delay.
	 */
	volatile uint32_t timeout;

	timeout = timeout_1uSec * 1000U;
	while ( ( ( ( ( uint16_t )m_i2c->ISR ) & I2C_ISR_BUSY ) != 0U ) && ( timeout-- > 0U ) )
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::TXRX_DMA_Int( void )
{
	if ( m_transfer_status == I2C_DMA_TX_BUSY )
	{
		Wait_Flag_Timeout( I2C_ISR_TXIS );
		Set_Stop();
		m_transfer_status = I2C_FINISH_TX;
		m_tx_dma->Clear_Int();
		m_tx_dma->Disable_Int();
		Disable_TX_DMA();
	}
	else
	{
		Set_Stop();
		m_transfer_status = I2C_FINISH_RX;
		m_rx_dma->Clear_Int();
		m_rx_dma->Disable_Int();
		Disable_RX_DMA();
	}
	if ( false == m_synchronous_type )
	{
		( *m_call_back_ptr )( m_cback_param );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::i2c_status_t uC_I2C::I2C_Transfer_Status( void )
{
	return ( m_transfer_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Attach_Callback( cback_func_t cback_func, cback_param_t param )
{
	m_call_back_ptr = cback_func;
	m_cback_param = param;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::I2C_ISR( void )
{
	/* It looks like a bulky ISR.
	   However from timing perspective only single  case shall be executed during one interrupt
	   The reading of SR1 and SR2 is crucial in I2C. Its not redundant code*/
	switch ( m_state_machine )
	{
		/* State for sending register/location address */
		case START_STATE:
			if ( ( m_i2c->ISR & I2C_ISR_TXIS ) != 0 )
			{
				m_i2c->TXDR = m_command_t;
				if ( m_write_exchange != false )
				{
					m_state_machine = SEND_DATA;
				}
				else
				{
					m_state_machine = REPEATED_START_STATE;
				}
			}
			else
			{
				m_state_machine = MAX_STATE;
			}
			break;

		/* Repeated start condition in case of read mode */
		case REPEATED_START_STATE:
			if ( ( ( m_i2c->ISR & I2C_ISR_TXIS ) != 0 ) || ( ( m_i2c->ISR & I2C_ISR_TC ) != 0 ) )
			{
				/* Generate Stop */
				m_i2c->CR2 |= I2C_CR2_STOP;
				m_i2c->ICR |= I2C_ICR_STOPCF;
				Nop();
				/* Reload the new configuration for read */
				m_i2c->ICR |= I2C_ICR_ADDRCF;
				m_i2c->CR2 &= ~( I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R |
								 I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_NBYTES );
				/* Load the number of bytes to be transferred */
				m_i2c->CR2 |= ( ( static_cast<uint32_t>( m_transfer_length ) << I2C_CR2_NBYTES_Pos )
								& I2C_CR2_NBYTES );
				m_i2c->CR2 |= I2C_CR2_RD_WRN;
				/* Configure Interrupt */
				m_i2c->CR1 &= ~( I2C_CR1_TXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE
								 | I2C_CR1_ERRIE );
				m_i2c->CR1 |= ( I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_STOPIE | I2C_CR1_TCIE
								| I2C_CR1_ERRIE );
				/* Generate Start */
				m_i2c->CR2 |= I2C_CR2_START;
				m_state_machine = RECEIVE_DATA;
			}
			break;


		/* State for sending data to slave */
		case SEND_DATA:
			if ( ( ( m_i2c->ISR & I2C_ISR_TCR ) != 0 ) || ( ( m_i2c->ISR & I2C_ISR_TXIS ) != 0 ) )
			{
				m_transfer_length--;
				m_i2c->TXDR = *m_tx_data;
				m_tx_data += 1U;
				if ( m_transfer_length == 0U )
				{
					m_state_machine = STOP_CONDITION;
				}
			}
			break;

		/* State for receiving data from slave */
		case RECEIVE_DATA:
			if ( ( m_i2c->ISR & I2C_ISR_RXNE ) != 0 )
			{
				*m_rx_data = static_cast<uint8_t>( m_i2c->RXDR );
				m_transfer_length--;
				m_rx_data += 1U;
				if ( m_transfer_length == 0U )
				{
					m_state_machine = STOP_CONDITION;
				}
			}
			break;

		/* State which sets the stop bit */
		case STOP_CONDITION:
			if ( ( ( m_i2c->ISR & I2C_ISR_TC ) != 0 ) || ( ( m_i2c->ISR & I2C_ISR_TXIS ) != 0 ) ||
				 ( ( m_i2c->ISR & I2C_ISR_RXNE ) != 0 ) )
			{
				if ( ( m_i2c->CR2 & I2C_CR2_AUTOEND ) != 0 )
				{
					m_transfer_status = I2C_FINISH_TX;
					m_i2c->CR1 &= ~( I2C_CR1_TXIE | I2C_CR1_TCIE | I2C_CR1_STOPIE
									 | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_ERRIE );
				}
				else
				{
					m_i2c->CR2 |= I2C_CR2_STOP;
					m_i2c->ICR |= I2C_ICR_STOPCF;
					Nop();
					m_i2c->CR1 &= ~( I2C_CR1_TXIE | I2C_CR1_TCIE | I2C_CR1_STOPIE
									 | I2C_CR1_RXIE | I2C_CR1_NACKIE | I2C_CR1_ERRIE );
					m_transfer_status = ( m_write_exchange != false ) ? I2C_FINISH_TX : I2C_FINISH_RX;
					m_state_machine = MAX_STATE;
					( *m_call_back_ptr )( m_cback_param );
				}
			}
			break;

		default:
			break;

	}
}
