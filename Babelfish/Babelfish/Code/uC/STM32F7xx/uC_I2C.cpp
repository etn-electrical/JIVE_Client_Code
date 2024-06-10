/**
 **********************************************************************************************
 *  @file           uC_I2C.cpp C++ Implementation File for I2C software module.
 *
 *  @brief          The file shall include the definitions of all the functions required for
 *                  I2C communication and which are declared in corresponding header file
 *  @details
 *  @copyright      2021 Eaton Corporation. All Rights Reserved.
 *  @remark         Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "uC_I2C.h"
#include "Babelfish_Assert.h"
#include "uC_Time_nSec.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::uC_I2C( uint8_t channel,  uint16_t device_address, i2c_mode_t speed_mode, address_type_t addr_mode,
				bool_t synchronus_transfer, bool_t enable_dma, uC_BASE_I2C_IO_PORT_STRUCT const* io_port ) :
	m_channel_number( channel ),
	m_chip_address( device_address ),
	m_i2c(
		reinterpret_cast<I2C_TypeDef*>( nullptr ) ),
	m_addressing_mode(
		addr_mode ),
	m_dma_transfer( enable_dma ),
	m_config(
		reinterpret_cast<uC_BASE_I2C_IO_STRUCT*>( nullptr ) ),
	m_io_ctrl( io_port ),
	m_i2c_speed_type( speed_mode ),
	m_transfer_status( NO_ERROR_INIT ),
	m_rx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_tx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_call_back_ptr( nullptr ),
	m_cback_param( nullptr ),
	m_state_machine( START_STATE ),
	m_tx_data( nullptr ),
	m_rx_data( nullptr ),
	m_transfer_length( 0U ),
	m_reload_length( 0U ),
	m_reload_mode( false ),
	m_dma_vector_obj_tx( nullptr ),
	m_dma_vector_obj_rx( nullptr ),
	m_i2c_vector_obj( nullptr ),
	m_synchronous_type( synchronus_transfer )
{
	Init_I2C( m_channel_number, m_i2c_speed_type );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Init_I2C( uint8_t i2c_channel, i2c_mode_t i2c_mode )
{
	uint32_t control_register;

	m_config = ( uC_Base::Self() )->Get_I2C_Ctrl( i2c_channel );

	BF_ASSERT( m_config != nullptr );
	/* Get the base address of I2C channel */
	m_i2c = m_config->reg_ctrl;
	/* Put I2C peripheral in reset state */
	uC_Base::Reset_Periph( &m_config->periph_def );
	/*I2C Peripheral clock enable */
	uC_Base::Enable_Periph_Clock( &m_config->periph_def );

	/* Configure GPIOs for I2C(Serial Clock and serial data) functionalities */
	if ( m_io_ctrl == nullptr )
	{
		m_io_ctrl = m_config->default_port_io;
	}

	/* Output type configuration*/
	uC_IO_Config::Enable_Periph_Output_Pin( m_io_ctrl->scl_port, true );
	control_register = ( ( m_io_ctrl->scl_port )->reg_ctrl )->OTYPER;
	BF_Lib::Bit::Set( control_register, ( m_io_ctrl->scl_port )->pio_num );
	( ( m_io_ctrl->scl_port )->reg_ctrl )->OTYPER = control_register;

	/* Output type configuration */
	uC_IO_Config::Enable_Periph_Output_Pin( m_io_ctrl->sda_port, true );
	control_register = ( ( m_io_ctrl->sda_port )->reg_ctrl )->OTYPER;
	BF_Lib::Bit::Set( control_register, ( m_io_ctrl->sda_port )->pio_num );
	( ( m_io_ctrl->sda_port )->reg_ctrl )->OTYPER = control_register;

	/**
	 * Configure the internal pull up resister for I2C_1 SDA and SCL.
	 * If required internal pullups can be enabled using the below code
	 */

	/*cfg_reg_shift = ( m_io_ctrl->scl_port )->pio_num * 2U;
	   Set_Mask( ( ( m_io_ctrl->scl_port )->reg_ctrl )->PUPDR,
	          ( GPIO_PUPDR_PUPDR0 << cfg_reg_shift ),
	          ( GPIO_PUPDR_PUPDR0_0 << cfg_reg_shift ) );
	   cfg_reg_shift = ( m_io_ctrl->sda_port )->pio_num * 2U;
	   Set_Mask( ( ( m_io_ctrl->sda_port )->reg_ctrl )->PUPDR,
	          ( GPIO_PUPDR_PUPDR0 << cfg_reg_shift ),
	          ( GPIO_PUPDR_PUPDR0_0 << cfg_reg_shift ) );*/

	/* Control register Settings */
	m_i2c->CR1 = 0U;/* Reset Control register 1 */
	m_i2c->CR2 = 0U;/* Reset Control register 2 */

	/* For CCR configuration the PE bit must be 0*/
	if ( true == static_cast<bool_t>( m_i2c->CR1 & I2C_CR1_PE ) )
	{
		Clr_Mask( m_i2c->CR1, I2C_CR1_PE );
	}

	// Make sure analog filter is enabled and DNF disabled
	Clr_Mask( m_i2c->CR1, I2C_CR1_ANFOFF );
	Clr_Mask( m_i2c->CR1, I2C_CR1_DNF );

	// Timing
	m_i2c->TIMINGR = Get_I2C_Timing( i2c_mode );

	// Disable NOSTRETCH
	Clr_Mask( m_i2c->CR1, I2C_CR1_NOSTRETCH );

	Enable_I2C();

	if ( ( m_config->rx_dma_channel != uC_DMA_STREAM_DNE ) &&
		 ( m_config->tx_dma_channel != uC_DMA_STREAM_DNE ) &&
		 ( true == m_dma_transfer ) )
	{
		uC_DMA::Deallocate_DMA_Stream( m_config->rx_dma_channel );
		m_rx_dma = new uC_DMA( m_config->rx_dma_channel, uC_DMA_DIR_FROM_PERIPH,
							   sizeof( uint8_t ), uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE,
							   uC_DMA_CIRC_FALSE );

		m_rx_dma->Clear_Int();
		m_rx_dma->Attach_Callback( &RX_DMA_Int_Static,
								   static_cast<uC_DMA::cback_param_t>( this ),
								   static_cast<uint8_t>( uC_INT_HW_PRIORITY_11 ) );

		uC_DMA::Deallocate_DMA_Stream( m_config->tx_dma_channel );
		m_tx_dma = new uC_DMA( m_config->tx_dma_channel, uC_DMA_DIR_FROM_MEMORY,
							   sizeof( uint8_t ), uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_FALSE,
							   uC_DMA_CIRC_FALSE );

		m_tx_dma->Clear_Int();
		m_tx_dma->Attach_Callback( &TX_DMA_Int_Static,
								   static_cast<uC_DMA::cback_param_t>( this ),
								   static_cast<uint8_t>( uC_INT_HW_PRIORITY_11 ) );
	}

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
			m_i2c_vector_number, uC_INT_HW_PRIORITY_11 ); \
		/* Enable I2C interrupt vector event */
		uC_Interrupt::Clear_Int( m_i2c_vector_number );

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
	uint16_t reload_length = length;
	i2c_states_t state = START_STATE;

	m_transfer_status = NO_ERROR_INIT;

	error = Bus_Busy_TimeOut();
	m_transfer_status = Set_State( error, &state, START_STATE,
								   BUS_BUSY_ERROR );

	/** Master communication initialization */

	/** Addressing mode selection */
	if ( ADDRESS_10_BIT == m_addressing_mode )
	{
		// 10 bit addressing mode and set slave address
		m_i2c->CR2 |= I2C_CR2_ADD10;
		Set_Mask( m_i2c->CR2, I2C_CR2_SADD, address );
	}
	else
	{
		// 7 bit addressing mode and set slave address
		Clr_Mask( m_i2c->CR2, I2C_CR2_ADD10 );
		// SADD[7:1] should be written with the 7-bit slave address to be sent.
		// The bits SADD[9], SADD[8] and SADD[0] are don't care.
		Set_Mask( m_i2c->CR2, I2C_CR2_SADD, ( address << 1U ) );
	}

	/** Transfer direction */
	/**
	    RD_WRN: Transfer direction (master mode)
	    0: Master requests a write transfer.
	    1: Master requests a read transfer.
	    Changing this bit when the START bit is set is not allowed
	 */
	Clr_Mask( m_i2c->CR2, I2C_CR2_RD_WRN );

	/** Num bytes set and reload mode */
	if ( length > MAX_RELOAD_LENGTH )
	{
		Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( 0xFFU << I2C_CR2_NBYTES_Pos ) );
		m_i2c->CR2 |= I2C_CR2_RELOAD;
		m_reload_length = MAX_RELOAD_LENGTH;
		m_reload_mode = true;
	}
	else
	{
		Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( length << I2C_CR2_NBYTES_Pos ) );
		m_reload_length = length;
		m_reload_mode = false;
	}

	/** Implement the I2C Write State machine here */
	if ( ( false == m_synchronous_type ) && ( m_dma_transfer == false ) )
	{
		m_transfer_length = length;
		m_tx_data = datasource;
		m_i2c->CR1 |= I2C_CR1_TXIE | I2C_CR1_TCIE;
		uC_Interrupt::Clear_Int( m_i2c_vector_number );
		uC_Interrupt::Enable_Int( m_i2c_vector_number );
		m_state_machine = SEND_DATA;
		m_i2c->CR2 |= I2C_CR2_START;
	}
	else if ( true == m_dma_transfer )
	{
		Set_TX_DMA( datasource, length );
		Enable_TX_DMA();
		state = MAX_STATE;
		m_transfer_status = I2C_DMA_TX_BUSY;
		m_tx_dma->Clear_Int();
		m_tx_dma->Enable_End_Buff_Int();
		m_tx_dma->Enable_Int();
		Generate_Start();
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
					for ( number_of_bytes = 0U;
						  ( number_of_bytes < reload_length && number_of_bytes < MAX_RELOAD_LENGTH ) &&
						  ( NO_ERROR == error );
						  number_of_bytes++ )
					{
						if ( ( m_i2c->ISR & I2C_ISR_NACKF ) == ( 0x01 << I2C_ISR_NACKF_Pos ) )
						{
							Set_State( I2C_ERROR, &state, STOP_CONDITION, BYTE_TX_ERROR );
						}
						error = Send_Data( *datasource );

						datasource++;
					}

					if ( ( m_reload_mode == true ) && ( reload_length >= MAX_RELOAD_LENGTH ) )
					{
						while ( ( m_i2c->ISR & I2C_ISR_TCR ) != ( 0x01 << I2C_ISR_TCR_Pos ) )
						{}

						reload_length -= MAX_RELOAD_LENGTH;
						if ( reload_length > MAX_RELOAD_LENGTH )
						{
							Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( 0xFFU << I2C_CR2_NBYTES_Pos ) );
						}
						else
						{
							m_reload_mode = false;
							Clr_Mask( m_i2c->CR2, I2C_CR2_RELOAD );
							Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES,
									  ( static_cast<uint8_t>( reload_length ) << I2C_CR2_NBYTES_Pos ) );
						}
					}
					else
					{
						m_transfer_status = Set_State( error, &state, STOP_CONDITION,
													   BYTE_TX_ERROR );
					}

					break;

				case STOP_CONDITION:
					error = Set_Stop();
					m_transfer_status = Set_State( error, &state, MAX_STATE,
												   STOP_GENERATION_ERROR );
					m_transfer_status = I2C_FINISH_TX;
					break;

				/* Below cases to satisfy MISRA*/
				case SET_DEVICE_ADDRESS:

				case SET_COMMAND:

				case REPEATED_START_STATE:

				case REPEAT_DEVICE_ADDRESS:

				case RECEIVE_DATA:

				case MAX_STATE:
				default:
					state = MAX_STATE;
					break;
			}
		}

		if ( write_status != nullptr )
		{
			*write_status = m_transfer_status;
		}
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
	uint16_t reload_length = length;

	m_transfer_status = NO_ERROR_INIT;
	/** Implement the I2C Read State machine here */
	error = Bus_Busy_TimeOut();
	if ( error == I2C_ERROR )
	{
		Nop();
	}
	m_transfer_status = Set_State( error, &state, START_STATE,
								   BUS_BUSY_ERROR );

	/** Master communication initialization */

	/** Addressing mode selection */
	if ( ADDRESS_10_BIT == m_addressing_mode )
	{
		// 10 bit addressing mode and set slave address
		m_i2c->CR2 |= I2C_CR2_ADD10;
		Set_Mask( m_i2c->CR2, I2C_CR2_SADD, address );
	}
	else
	{
		// 7 bit addressing mode and set slave address
		Clr_Mask( m_i2c->CR2, I2C_CR2_ADD10 );
		// SADD[7:1] should be written with the 7-bit slave address to be sent.
		// The bits SADD[9], SADD[8] and SADD[0] are don't care.
		Set_Mask( m_i2c->CR2, I2C_CR2_SADD, ( address << 1U ) );
	}

	/** Transfer direction */
	/**
	    RD_WRN: Transfer direction (master mode)
	    0: Master requests a write transfer.
	    1: Master requests a read transfer.
	    Changing this bit when the START bit is set is not allowed
	 */
	Set_Mask( m_i2c->CR2, I2C_CR2_RD_WRN, 1U << I2C_CR2_RD_WRN_Pos );

	if ( length >= MAX_RELOAD_LENGTH )
	{
		Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( 0xFFU << I2C_CR2_NBYTES_Pos ) );
		m_i2c->CR2 |= I2C_CR2_RELOAD;
		m_reload_length = MAX_RELOAD_LENGTH;
		m_reload_mode = true;
	}
	else
	{
		Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( length << I2C_CR2_NBYTES_Pos ) );
		m_reload_length = length;
		m_reload_mode = false;
	}

	if ( ( false == m_synchronous_type ) && ( m_dma_transfer == false ) )
	{
		m_transfer_length = length;
		m_rx_data = datasource;
		m_i2c->CR1 |= I2C_CR1_RXIE | I2C_CR1_TCIE;
		uC_Interrupt::Clear_Int( m_i2c_vector_number );
		uC_Interrupt::Enable_Int( m_i2c_vector_number );
		m_state_machine = RECEIVE_DATA;
		m_i2c->CR2 |= I2C_CR2_START;
	}
	else if ( true == m_dma_transfer )
	{
		Set_RX_DMA( datasource, length );
		Enable_RX_DMA();
		state = MAX_STATE;
		m_transfer_status = I2C_DMA_RX_BUSY;
		m_rx_dma->Clear_Int();
		m_rx_dma->Enable_End_Buff_Int();
		m_rx_dma->Enable_Int();
		Generate_Start();
	}
	else
	{
		while ( state < MAX_STATE )
		{
			switch ( state )
			{
				case START_STATE:
					error = Generate_Start();
					m_transfer_status = Set_State( error, &state, RECEIVE_DATA,
												   START_GENERATION_ERROR );
					break;

				case RECEIVE_DATA:
					for ( number_of_bytes = 0U;
						  ( number_of_bytes < reload_length && number_of_bytes < MAX_RELOAD_LENGTH ) &&
						  ( NO_ERROR == error );
						  number_of_bytes++ )
					{
						if ( ( m_i2c->ISR & I2C_ISR_NACKF ) == ( 0x01 << I2C_ISR_NACKF_Pos ) )
						{
							Set_State( I2C_ERROR, &state, STOP_CONDITION, BYTE_RX_ERROR );
						}

						error = Receive_Data( datasource );
						datasource++;
					}

					if ( ( m_reload_mode == true ) && ( reload_length >= MAX_RELOAD_LENGTH ) )
					{
						while ( ( m_i2c->ISR & I2C_ISR_TCR ) != ( 0x01 << I2C_ISR_TCR_Pos ) )
						{}
						reload_length -= MAX_RELOAD_LENGTH;
						if ( reload_length > MAX_RELOAD_LENGTH )
						{
							Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( 0xFFU << I2C_CR2_NBYTES_Pos ) );
						}
						else
						{
							m_reload_mode = false;
							Clr_Mask( m_i2c->CR2, I2C_CR2_RELOAD );
							Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES,
									  ( static_cast<uint8_t>( reload_length ) << I2C_CR2_NBYTES_Pos ) );
						}
					}
					else
					{
						m_transfer_status = Set_State( error, &state, STOP_CONDITION,
													   BYTE_RX_ERROR );
					}
					break;

				case STOP_CONDITION:
					error = Set_Stop();
					m_transfer_status = Set_State( error, &state, MAX_STATE,
												   STOP_GENERATION_ERROR );
					m_transfer_status = I2C_FINISH_RX;
					break;

				case SET_DEVICE_ADDRESS:

				case SET_COMMAND:

				case REPEATED_START_STATE:

				case REPEAT_DEVICE_ADDRESS:

				case SEND_DATA:

				case MAX_STATE:

				default:
					state = MAX_STATE;
					break;
			}
		}
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
	return_type error = Start_Frame_Timeout();

	return ( error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Set_Stop( void )
{
	m_i2c->CR2 |= I2C_CR2_STOP;
	return ( NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Send_Data( uint8_t data )
{
	Set_Mask( m_i2c->TXDR, I2C_TXDR_TXDATA, data );
	return ( Wait_Flag_Timeout( I2C_ISR_TXE, &( m_i2c->ISR ), 0x00U ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Receive_Data( uint8_t* data ) const
{
	return_type receive_status = Wait_Flag_Timeout( I2C_ISR_RXNE, &( m_i2c->ISR ), 0x00U );

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
uC_I2C::i2c_status_t uC_I2C::Set_State( return_type error,
										i2c_states_t* current_state, i2c_states_t next_state,
										i2c_status_t state_specific_error )
{
	i2c_status_t retval;

	if ( NO_ERROR == error )
	{
		*current_state = next_state;
		retval = NO_ERROR_INIT;
	}
	else
	{
		if ( state_specific_error != BUS_BUSY_ERROR )
		{
			Set_Stop();
		}
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
uC_I2C::return_type uC_I2C::Wait_Flag_Timeout( uint32_t flag_mask, volatile uint32_t* reg, uint16_t val ) const
{
	//// Function updated to fix ICDUNIKITY-417 (Ethernet port Auto negotiation value didn�t update
	//// other than 4)
	uint32_t timeout_1uSec = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * 10U );
	uint32_t elapsed_time = 0;
	/// *
	// 10 Clock cycles are required to execute the following while loop.
	// So (PROCESSOR_FREQ_MHZ * 100 / 10U) count is for 1 uSec Delay.
	// I2C @100KHz requires 10uSec timeout. So we overdesigned to timeout give 25 uSec delay.
	// */
	//
	uint32_t sample_time = uC_TIME_NSEC_Sample();

	volatile uint32_t timeout;

	timeout = timeout_1uSec * 25U;


	while ( ( ( *reg & flag_mask ) == val ) && ( elapsed_time < timeout ) )
	{
		elapsed_time = uC_TIME_NSEC_Elapsed( sample_time );
	}
	return ( ( timeout <= elapsed_time ) ? I2C_ERROR : NO_ERROR );
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
void uC_I2C::Set_TX_DMA( uint8_t* data, uint16_t length )
{
	m_tx_dma->Disable();
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
	m_rx_dma->Set_Buff( data, ( uint8_t* ) ( &m_i2c->RXDR ), length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Enable_RX_DMA( void )
{
	Push_TGINT();
	m_rx_dma->Enable();
	m_i2c->CR1 |= I2C_CR1_RXDMAEN;
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
	Push_TGINT();
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
	Push_TGINT();
	m_i2c->CR1 &= ~I2C_CR1_TXDMAEN;
	m_tx_dma->Disable();
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Bus_Busy_TimeOut( void )
{
	//// Function updated to fix ICDUNIKITY-417 (Ethernet port Auto negotiation value didn�t update
	//// other than 4)
	uint32_t timeout_1uSec = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * 10U );
	uint32_t elapsed_time = 0;
	/// *
	// 10 Clock cycles are required to execute the following while loop.
	// So (PROCESSOR_FREQ_MHZ * 100 / 10U) count is for 1 uSec Delay.
	// I2C @100KHz requires 10uSec timeout. So we overdesigned to timeout give 25 uSec delay.
	// */
	//
	uint32_t sample_time = uC_TIME_NSEC_Sample();

	volatile uint32_t timeout;

	timeout = timeout_1uSec * 25U;


	while ( ( ( m_i2c->ISR & I2C_ISR_BUSY ) == ( 0x01U << I2C_ISR_BUSY_Pos ) ) && ( elapsed_time < timeout ) )
	{
		elapsed_time = uC_TIME_NSEC_Elapsed( sample_time );
	}
	return ( ( timeout <= elapsed_time ) ? I2C_ERROR : NO_ERROR );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::RX_DMA_Int( void )
{
	Wait_Flag_Timeout( I2C_ISR_RXNE, &( m_i2c->ISR ), 0x00U );
	Set_Stop();
	m_transfer_status = I2C_FINISH_RX;
	m_rx_dma->Clear_Int();
	m_rx_dma->Disable_Int();
	if ( ( false == m_synchronous_type ) && ( m_call_back_ptr != nullptr ) )
	{
		( *m_call_back_ptr )( m_cback_param );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::TX_DMA_Int( void )
{
	Wait_Flag_Timeout( I2C_ISR_TXE, &( m_i2c->ISR ), 0x00U );

	Set_Stop();
	m_transfer_status = I2C_FINISH_TX;
	m_tx_dma->Clear_Int();
	m_tx_dma->Disable_Int();
	if ( ( false == m_synchronous_type ) && ( m_call_back_ptr != nullptr ) )
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
	switch ( m_state_machine )
	{
		/* State for sending data to slave */
		case SEND_DATA:
			m_reload_length--;
			Set_Mask( m_i2c->TXDR, I2C_TXDR_TXDATA, *m_tx_data );
			m_tx_data += 1U;
			if ( m_reload_length == 0U )
			{
				if ( m_reload_mode == true )
				{
					m_transfer_length -= MAX_RELOAD_LENGTH;
					if ( m_transfer_length > MAX_RELOAD_LENGTH )
					{
						m_reload_length = MAX_RELOAD_LENGTH;
						Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( 0xFFU << I2C_CR2_NBYTES_Pos ) );
					}
					else
					{
						m_reload_mode = false;
						m_reload_length = m_transfer_length;
						Clr_Mask( m_i2c->CR2, I2C_CR2_RELOAD );
						Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES,
								  ( static_cast<uint8_t>( m_reload_length ) << I2C_CR2_NBYTES_Pos ) );
					}
				}
				else
				{
					m_i2c->CR1 &= ~( I2C_CR1_TXIE );
					m_i2c->CR1 |= I2C_CR1_TCIE;
					m_state_machine = STOP_CONDITION;
				}
			}
			break;

		/* State for receiving data from slave */
		case RECEIVE_DATA:
			m_reload_length--;
			*m_rx_data = static_cast<uint8_t>( m_i2c->RXDR );
			m_rx_data += 1U;
			if ( m_reload_length == 0U )
			{
				if ( m_reload_mode == true )
				{
					while ( ( m_i2c->ISR & I2C_ISR_TCR ) != ( 0x01 << I2C_ISR_TCR_Pos ) )
					{}
					m_transfer_length -= MAX_RELOAD_LENGTH;
					if ( m_transfer_length > MAX_RELOAD_LENGTH )
					{
						m_reload_length = MAX_RELOAD_LENGTH;
						Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES, ( 0xFFU << I2C_CR2_NBYTES_Pos ) );
					}
					else
					{
						m_reload_mode = false;
						m_reload_length = m_transfer_length;
						Clr_Mask( m_i2c->CR2, I2C_CR2_RELOAD );
						Set_Mask( m_i2c->CR2, I2C_CR2_NBYTES,
								  ( static_cast<uint8_t>( m_reload_length ) << I2C_CR2_NBYTES_Pos ) );
					}
				}
				else
				{
					m_i2c->CR1 &= ~( I2C_CR1_RXIE );
					m_i2c->CR1 |= I2C_CR1_TCIE;
					m_state_machine = STOP_CONDITION;
				}
			}

			break;

		/* State which sets the stop bit */
		case STOP_CONDITION:
			m_state_machine = MAX_STATE;
			m_i2c->CR1 &=
				~( I2C_CR1_ERRIE | I2C_CR1_TXIE | I2C_CR1_RXIE | I2C_CR1_TCIE );
			Set_Stop();

			if ( m_call_back_ptr != nullptr )
			{
				( *m_call_back_ptr )( m_cback_param );
			}

			break;

		case MAX_STATE:
		default:
			m_state_machine = MAX_STATE;
			break;

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Start_Frame_Timeout() const
{
	//// Function updated to fix ICDUNIKITY-417 (Ethernet port Auto negotiation value didn�t update
	//// other than 4)
	uint32_t timeout_1uSec = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * 10U );
	uint32_t elapsed_time = 0;
	/// *
	// 10 Clock cycles are required to execute the following while loop.
	// So (PROCESSOR_FREQ_MHZ * 100 / 10U) count is for 1 uSec Delay.
	// I2C @100KHz requires 10uSec timeout. So we overdesigned to timeout give 25 uSec delay.
	// */
	//
	uint32_t sample_time = uC_TIME_NSEC_Sample();

	volatile uint32_t timeout;

	timeout = timeout_1uSec * 25U;


	while ( ( ( m_i2c->CR2 & I2C_CR2_START ) == ( 0x01 << I2C_CR2_START_Pos ) ) && ( elapsed_time < timeout ) )
	{
		elapsed_time = uC_TIME_NSEC_Elapsed( sample_time );
	}
	return ( ( timeout <= elapsed_time ) ? I2C_ERROR : NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_I2C::Get_I2C_Timing( i2c_mode_t speed_mode )
{
	uint32_t timing = 0;
	/* Values calculated with STM32CubeMX tool
	 * Standard mode (up to 100 kHz)
	 * Fast mode (up to 400 kHz)
	    Below values obtained with:
	    - I2Cx clock source = APB1CLK depending on MASTER_CLOCK_FREQUENCY
	        = 120 MHz
	        = 100 MHz
	        = 90 MHz
	    - Analog filter delay = ON
	    - Digital filter coefficient = 0
	    - Rise time = 120ns
	    - Fall time = 120ns
	 */
	uint32_t i2c_clock_freq = ( uC_Base::Self() )->Get_PClock_Freq( m_config->periph_def.pclock_reg_index );

	switch ( i2c_clock_freq )
	{
		case I2C_CLOCK_FREQ_120_MHZ:
			switch ( speed_mode )
			{
				case I2C_STANDARD_MODE:
					timing = 0x40825F86;
					break;

				case I2C_FAST_MODE:
					timing = 0x2082153E;
					break;

				default:
					BF_ASSERT( false );
					break;
			}
			break;

		case I2C_CLOCK_FREQ_100_MHZ:
			switch ( speed_mode )
			{
				case I2C_STANDARD_MODE:
					timing = 0xC0212028;
					break;

				case I2C_FAST_MODE:
					timing = 0x10A21A4E;
					break;

				default:
					BF_ASSERT( false );
					break;
			}
			break;

		case I2C_CLOCK_FREQ_90_MHZ:
			switch ( speed_mode )
			{
				case I2C_STANDARD_MODE:
					timing = 0x40614665;
					break;

				case I2C_FAST_MODE:
					timing = 0x10921746;
					break;

				default:
					BF_ASSERT( false );
					break;
			}
			break;

		case I2C_CLOCK_FREQ_25_MHZ:
			switch ( speed_mode )
			{
				case I2C_STANDARD_MODE:
					timing = 0x0080608F;
					break;

				case I2C_FAST_MODE:
					timing = 0x00400BE4;
					break;

				default:
					BF_ASSERT( false );
					break;
			}
			break;

		default:
			BF_ASSERT( false );
			break;
	}
	return ( timing );
}
