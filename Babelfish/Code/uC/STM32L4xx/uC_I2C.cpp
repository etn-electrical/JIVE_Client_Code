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
#include "uC_I2C.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::uC_I2C( uint8_t channel, uint16_t device_address, i2c_mode_t speed_mode,
				address_type_t addr_mode, bool_t synchronus_trnasfer, bool_t enable_dma,
				duty_type_t fastmode_duty_cycle, uC_BASE_I2C_IO_PORT_STRUCT const* io_port ) :
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
	m_f_duty_cycle( FASTMODE_DUTYCYCLE_2 ),
	m_i2c_speed_type( speed_mode ),
	m_transfer_status( NO_ERROR_INIT ),
	m_rx_dma( reinterpret_cast<uC_DMA*>( nullptr ) ),
	m_tx_dma( reinterpret_cast<uC_DMA*>( nullptr ) )
{
	m_i2c_speed_type = speed_mode;
	m_f_duty_cycle = fastmode_duty_cycle;
	m_channel_number = channel;
	m_chip_address = device_address;
	m_addressing_mode = addr_mode;
	m_dma_transfer = enable_dma;
	m_synchronous_type = synchronus_trnasfer;
	Init_I2C( m_channel_number, m_i2c_speed_type, m_f_duty_cycle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Init_I2C( uint8_t i2c_channel, i2c_mode_t i2c_mode,
					   duty_type_t fmode_duty_cycle )
{
	uint32_t i2c_pclk1_freq;
	uint16_t i2c_ccr_value;
	uint16_t fast_mode_duty = 0U;
	uint32_t control_register;

	m_config = ( uC_Base::Self() )->Get_I2C_Ctrl( i2c_channel );
	BF_ASSERT( m_config != nullptr );
	i2c_ccr_value = m_config->irq_num;
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
	/* Output type configuration*/
	control_register = ( ( m_io_ctrl->scl_port )->reg_ctrl )->OTYPER;
	BF_Lib::Bit::Set( control_register, ( m_io_ctrl->scl_port )->pio_num );
	( ( m_io_ctrl->scl_port )->reg_ctrl )->OTYPER = control_register;
	uC_IO_Config::Enable_Periph_Output_Pin( m_config->default_port_io->sda_port, true );
	control_register = ( ( m_io_ctrl->sda_port )->reg_ctrl )->OTYPER;
	BF_Lib::Bit::Set( control_register, ( m_io_ctrl->sda_port )->pio_num );
	( ( m_io_ctrl->sda_port )->reg_ctrl )->OTYPER = control_register;
	m_i2c->CR1 |= I2C_CR1_SWRST;
	/* Control register Settings */
	m_i2c->CR1 = 0U;/* Reset Control register 1 */
	m_i2c->CR2 = 0U;/* Reset Control register 2 */

	/*Peripheral clock frequency*/
	i2c_pclk1_freq = ( uC_Base::Self() )->Get_PClock_Freq(
		m_config->periph_def.pclock_reg_index );
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
			i2c_ccr_value = static_cast<uint16_t>( i2c_pclk1_freq
												   / ( I2C_ClockSpeed_400K * 3U ) );
			fast_mode_duty = static_cast<uint16_t>( 0x8000U );
		}
		else// FASTMODE_DUTYCYCLE_16_9
		{
			i2c_ccr_value = static_cast<uint16_t>( i2c_pclk1_freq
												   / ( I2C_ClockSpeed_400K * 25U ) );
			fast_mode_duty = static_cast<uint16_t>( 0xC000U );
		}
		if ( i2c_ccr_value < 1U )
		{
			/* Set minimum allowed value */
			i2c_ccr_value = 1U;
		}
		m_i2c->CCR = ( i2c_ccr_value | fast_mode_duty );
		m_i2c->TRISE =
			static_cast<uint16_t>( ( i2c_pclk1_freq * 3U ) / ( 10000000U ) ) + 1U;		// in fast
																						// mode, the
																						// maximum
																						// allowed SCL
																						// rise time
																						// is 300 ns
	}
	else
	{
		i2c_ccr_value = static_cast<uint16_t>( i2c_pclk1_freq
											   / ( I2C_ClockSpeed_100K << 1U ) );
		if ( i2c_ccr_value < 4U )
		{
			/* Set minimum allowed value */
			i2c_ccr_value = 4U;
		}
		m_i2c->CCR = i2c_ccr_value;
		m_i2c->TRISE = static_cast<uint16_t>( ( i2c_pclk1_freq ) / ( 1000000U ) )
			+ 1U;	// in standard mode, the maximum allowed SCL rise time is 1000 ns
	}

	Enable_I2C();
	m_i2c->CR1 |= I2C_CR1_ACK;

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
			m_i2c_vector_number, uC_INT_HW_PRIORITY_11 );
		m_i2c->CR2 |= I2C_CR2_ITERREN | I2C_CR2_ITEVTEN;
		m_data_sink = m_i2c->DR;
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
	i2c_states_t state = START_STATE;

	m_transfer_status = NO_ERROR_INIT;
	/** Implement the I2C Write State machine here */
	Clear_Error();
	m_i2c->CR1 |= I2C_CR1_ACK;
	Bus_Busy_TimeOut();

	m_command_t = address;
	if ( ( false == m_synchronous_type ) && ( m_dma_transfer == false ) )
	{
		m_transfer_length = length;
		m_write_exchange = true;
		m_tx_data = datasource;
		m_i2c->CR2 |= I2C_CR2_ITERREN | I2C_CR2_ITEVTEN;
		uC_Interrupt::Clear_Int( m_i2c_vector_number );
		uC_Interrupt::Enable_Int( m_i2c_vector_number );
		m_i2c->CR1 |= I2C_CR1_START;
		m_state_machine = START_STATE;
	}
	else
	{

		while ( state < MAX_STATE )
		{
			switch ( state )
			{
				case START_STATE:
					error = Generate_Start();
					m_transfer_status = Set_State( error, &state, SET_DEVICE_ADDRESS,
												   START_GENERATION_ERROR );
					break;

				case SET_DEVICE_ADDRESS:
					error = Send_Device_Address( m_chip_address, I2C_WRITE );
					m_transfer_status = Set_State( error, &state, SET_COMMAND,
												   ADDRESS_ERROR );
					break;

				case SET_COMMAND:
					error = Send_Data( static_cast<uint8_t>( address ) );
					m_transfer_status = Set_State( error, &state, SEND_DATA,
												   BYTE_TX_ERROR );
					break;

				case SEND_DATA:
					if ( false == m_dma_transfer )
					{

						for ( number_of_bytes = 0U;
							  ( number_of_bytes < length ) && ( NO_ERROR == error );
							  number_of_bytes++ )
						{
							error = Send_Data( *datasource );
							datasource++;
						}
						m_transfer_status = Set_State( error, &state, STOP_CONDITION,
													   BYTE_TX_ERROR );

					}
					else
					{
						Set_TX_DMA( datasource, length );
						Enable_TX_DMA();
						state = MAX_STATE;
						m_transfer_status = I2C_DMA_TX_BUSY;
						m_tx_dma->Clear_Int();
						m_tx_dma->Enable_End_Buff_Int();
						m_tx_dma->Enable_Int();
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

	m_transfer_status = NO_ERROR_INIT;
	/** Implement the I2C Read State machine here */
	Clear_Error();
	m_i2c->CR1 |= I2C_CR1_ACK;
	Bus_Busy_TimeOut();

	m_command_t = address;
	if ( ( false == m_synchronous_type ) && ( m_dma_transfer == false ) )
	{
		m_transfer_length = length;
		m_write_exchange = false;
		m_rx_data = datasource;
		m_i2c->CR2 |= I2C_CR2_ITERREN | I2C_CR2_ITEVTEN;
		uC_Interrupt::Clear_Int( m_i2c_vector_number );
		uC_Interrupt::Enable_Int( m_i2c_vector_number );
		m_i2c->CR1 |= I2C_CR1_START;
		m_state_machine = START_STATE;
	}
	else
	{

		while ( state < MAX_STATE )
		{
			switch ( state )
			{
				case START_STATE:
					error = Generate_Start();
					m_transfer_status = Set_State( error, &state, SET_DEVICE_ADDRESS,
												   START_GENERATION_ERROR );
					break;

				case SET_DEVICE_ADDRESS:
					error = Send_Device_Address( m_chip_address, I2C_WRITE );
					m_transfer_status = Set_State( error, &state, SET_COMMAND,
												   ADDRESS_ERROR );
					break;

				case SET_COMMAND:
					error = Send_Data( static_cast<uint8_t>( address ) );
					m_transfer_status = Set_State( error, &state, REPEATED_START_STATE,
												   BYTE_TX_ERROR );
					break;

				case REPEATED_START_STATE:
					error = Generate_Start();
					m_transfer_status = Set_State( error, &state,
												   REPEAT_DEVICE_ADDRESS, RSTART_GENERATION_ERROR );
					break;

				case REPEAT_DEVICE_ADDRESS:
					if ( true == m_dma_transfer )
					{
						if ( length == 1U )
						{
							m_i2c->CR1 &= ( ~I2C_CR1_ACK );
						}

						Set_RX_DMA( datasource, length );
						Enable_RX_DMA();
						state = MAX_STATE;
						Send_Device_Address( m_chip_address, I2C_READ );
						m_transfer_status = I2C_DMA_RX_BUSY;
						m_rx_dma->Clear_Int();
						m_rx_dma->Enable_End_Buff_Int();
						m_rx_dma->Enable_Int();
					}
					else
					{
						error = Send_Device_Address( m_chip_address, I2C_READ );
						m_transfer_status = Set_State( error, &state, RECEIVE_DATA,
													   ADDRESS_ERROR );
					}
					break;

				case RECEIVE_DATA:
					for ( number_of_bytes = 0U;
						  ( number_of_bytes < length ) && ( NO_ERROR == error );
						  number_of_bytes++ )
					{
						if ( number_of_bytes == ( length - 1U ) )
						{
							m_i2c->CR1 &= ( ~I2C_CR1_ACK );
						}

						error = Receive_Data( datasource );
						datasource++;
					}
					m_transfer_status = Set_State( error, &state, STOP_CONDITION,
												   BYTE_RX_ERROR );

					break;

				case STOP_CONDITION:
					error = Set_Stop();
					m_transfer_status = Set_State( error, &state, MAX_STATE,
												   STOP_GENERATION_ERROR );
					m_transfer_status = I2C_FINISH_RX;
					break;

				case SEND_DATA:

				case MAX_STATE:

				default:
					state = MAX_STATE;
					break;
			}
		}

		if ( read_status != nullptr )
		{
			*read_status = m_transfer_status;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Send_Device_Address( uint16_t devaddr,
												 uint8_t read_write )
{
	uint16_t device_address = 0U;
	return_type addrs_ack_status;

	if ( ADDRESS_10_BIT == m_addressing_mode )
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
		device_address = devaddr & MAX_END_ADDR_7_BIT;
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
uC_I2C::i2c_status_t uC_I2C::Set_State( return_type error,
										i2c_states_t* current_state, i2c_states_t next_state,
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
	uint32_t timeout_count = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ
													* I2C_TIMEOUT );

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
	m_tx_dma->Disable();
	m_tx_dma->Set_Buff( data, ( uint8_t* ) ( &m_i2c->DR ), length );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Set_RX_DMA( uint8_t* data, uint16_t length )
{
	m_rx_dma->Disable();
	m_rx_dma->Set_Buff( data, ( uint8_t* ) ( &m_i2c->DR ), length );
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
	// Function updated to fix ICDUNIKITY-417 (Ethernet port Auto negotiation value didn�t update
	// other than 4)
	uint32_t timeout_1uSec = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * 10U );
	/*
	   10 Clock cycles are required to execute the following while loop.
	   So (PROCESSOR_FREQ_MHZ * 100 / 10U) count is for 1 uSec Delay.
	   I2C @100KHz requires 10uSec timeout. So we overdesigned to timeout give 25 uSec delay.
	 */
	volatile uint32_t timeout;

	timeout = timeout_1uSec * 25U;
	while ( ( ( ( ( uint16_t )m_i2c->SR2 ) & I2C_SR2_BUSY ) != 0U ) && ( timeout-- > 0U ) )
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::RX_DMA_Int( void )
{
	Wait_Flag_Timeout( I2C_SR1_TXE );
	Set_Stop();
	m_transfer_status = I2C_FINISH_RX;
	m_rx_dma->Clear_Int();
	m_rx_dma->Disable_Int();
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
void uC_I2C::TX_DMA_Int( void )
{
	Wait_Flag_Timeout( I2C_SR1_TXE );
	Set_Stop();
	m_transfer_status = I2C_FINISH_TX;
	m_tx_dma->Clear_Int();
	m_tx_dma->Disable_Int();
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
	uint16_t i2c_address;

	/* It looks like a bulky ISR.
	   However from timing perspective only single  case shall be executed during one interrupt
	   The reading of SR1 and SR2 is crucial in I2C. Its not redundant code*/
	switch ( m_state_machine )
	{	/* State for sending device address/ start frame ACK */
		case START_STATE:
			if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_SB ) )
			{
				Get_SR2();
				/* To check if it is a 10 bit addressing mode or 7 bit addressing mode */
				if ( m_chip_address < MAX_END_ADDR_7_BIT )
				{	/* 0 because its write operation */
					m_i2c->DR =
						static_cast<uint16_t>( ( m_chip_address << 1U )	/* | 0x00U */ );
					m_state_machine = SET_COMMAND;
				}
				else if ( ( m_chip_address > MAX_END_ADDR_7_BIT ) &&
						  ( m_chip_address < MAX_END_ADDR_10_BIT ) )
				{
					i2c_address = ( ( m_chip_address >> 8U ) & 0x03U );
					/* Take out 8th and 9th bit */
					i2c_address = static_cast<uint16_t>( i2c_address << 1 );
					m_i2c->DR = ( ADDR_HEADER_10_BIT | i2c_address /* | 0x00U */ );
					m_state_machine = SET_DEVICE_ADDRESS;
				}
				else
				{
					m_state_machine = MAX_STATE;
				}
			}
			break;

		/* Send upper byte of an address in case of 10 bit addressing mode */
		case SET_DEVICE_ADDRESS:
			if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_ADD10 ) )
			{
				Get_SR2();
				i2c_address = m_chip_address & 0xFFU;
				m_i2c->DR = i2c_address;
				m_state_machine = SET_COMMAND;
			}
			break;

		/* Device address Acknowledge state. Also used for sending command to device */
		case SET_COMMAND:
			if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_TXE ) )
			{
				Get_SR2();
				m_i2c->DR = m_command_t;
				if ( m_write_exchange == true )
				{
					m_state_machine = SEND_DATA;
				}
				else
				{
					m_state_machine = REPEATED_START_STATE;
				}
			}
			break;

		/* Repeated start condition in case of read mode */
		case REPEATED_START_STATE:
			if ( ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_BTF ) ) ) ||
				 ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_TXE ) ) ) )
			{
				Generate_Start();
				m_state_machine = REPEAT_DEVICE_ADDRESS;
			}
			break;

		/* Repeat Device address in case of I2C read mode */
		case REPEAT_DEVICE_ADDRESS:
			if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_SB ) )
			{
				Get_SR2();
				if ( m_chip_address < MAX_END_ADDR_7_BIT )
				{	/* 1 because its read operation */
					m_i2c->DR =
						( ( static_cast<uint16_t>( m_chip_address << 1U ) ) | 0x01U );
					m_state_machine = RECEIVE_DATA;
				}
				else if ( ( m_chip_address > MAX_END_ADDR_7_BIT ) &&
						  ( m_chip_address < MAX_END_ADDR_10_BIT ) )
				{
					i2c_address = ( ( m_chip_address >> 8U ) & 0x03U );
					/* Take out 8th and 9th bit */
					i2c_address = static_cast<uint16_t>( i2c_address << 1U );
					m_i2c->DR = ( ADDR_HEADER_10_BIT | i2c_address | 0x01U );
					m_state_machine = RECEIVE_DATA;
				}
				else
				{
					m_state_machine = MAX_STATE;
				}
			}
			break;

		/* State for sending data to slave */
		case SEND_DATA:
			if ( ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_BTF ) ) ) ||
				 ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_TXE ) ) ) )
			{
				m_transfer_length--;
				m_i2c->DR = *m_tx_data;
				m_tx_data += 1U;
				if ( m_transfer_length == 0U )
				{
					m_state_machine = STOP_CONDITION;
				}
			}
			break;

		/* State for receiving data from slave */
		case RECEIVE_DATA:
			Get_SR2();
			if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_RXNE ) )
			{
				*m_rx_data = static_cast<uint8_t>( m_i2c->DR );
				m_transfer_length--;
				m_rx_data += 1U;
				if ( m_transfer_length == 0U )
				{
					m_state_machine = STOP_CONDITION;
				}
				Get_SR2();
				/* In I2C interrupt driven system it is required to read the SR2 register
				   after each byte transmission to clear the flags like ADDR if they are set . */
			}
			break;

		/* State which sets the stop bit */
		case STOP_CONDITION:
			if ( ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_BTF ) ) ) ||
				 ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_TXE ) ) ) )
			{
				Get_SR2();
				m_state_machine = MAX_STATE;
				m_i2c->CR2 &=
					~( I2C_CR2_ITERREN | I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN );
				Set_Stop();

				( *m_call_back_ptr )( m_cback_param );
			}
			break;

		default:
			break;

	}
}
