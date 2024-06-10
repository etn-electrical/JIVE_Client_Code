/**
 **********************************************************************************************
 *  @file           STM32F2xx_I2C.cpp C++ Implementation File for Queued I2C derived class.
 *
 *  @brief          The file shall include the definitions of all the functions described in
 *                  STM32F2xx_I2C.h file.
 *  @details
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *  @remark         Eaton Corporation claims proprietary rights to the material disclosed
 *                  hereon.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Includes.h"
#include "STM32F2xx_I2C.h"
#include "uC_IO_Config.h"
#include "Advanced_Vect.h"
#include "uC_Interrupt.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
STM32F2xx_I2C::STM32F2xx_I2C( class_config_t const* i2c_assy,
							  select_config_t const* const select_config ) :
	m_dummy_data( 0U ),
	m_select( select_config ),
	m_active_config( reinterpret_cast<select_config_t const*>( nullptr ) ),
	m_i2c( i2c_assy->i2c->reg_ctrl ),
	m_rx_stream( reinterpret_cast<DMA_Stream_TypeDef*>( nullptr ) ),
	m_tx_stream( reinterpret_cast<DMA_Stream_TypeDef*>( nullptr ) ),
	m_rx_dma_ctrl( reinterpret_cast<DMA_TypeDef*>( nullptr ) ),
	m_tx_dma_ctrl( reinterpret_cast<DMA_TypeDef*>( nullptr ) ),
	m_i2c_vector_obj( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_dma_vector_obj_tx( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_dma_vector_obj_rx( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_i2c_vector_number( NVIC_ENUM_SIZE ),
	m_dma_vector_number_tx( NVIC_ENUM_SIZE ),
	m_dma_vector_number_rx( NVIC_ENUM_SIZE ),
	m_rx_data( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_tx_data( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_tx_length( 0U ),
	m_rx_length( 0U ),
	m_rx_dma_id( 0U ),
	m_tx_dma_id( 0U ),
	m_data_sink( 0U ),
	state_machine( START_STATE_ACK ),
	transaction( I2C_WRITE_EXCHANGE ),
	tx_channel( 0U ),
	rx_channel( 0U ),
	dma_use( false ),
	device_address( 0x00U ),
	uC_Queued_I2C()
{

	m_i2c_vector_number = i2c_assy->i2c->irq_num;
	Initialize_Peripheral_Clock( &i2c_assy->i2c->periph_def );
	/* Configure the interrupt for I2C events */
	m_i2c_vector_obj = new Advanced_Vect();

	m_i2c_vector_obj->Set_Vect( &I2C_ISR_Static,
								static_cast<Advanced_Vect::cback_param_t>( this ) );
	uC_Interrupt::Clear_Vector( m_i2c_vector_number );
	uC_Interrupt::Set_Vector(
		static_cast<Advanced_Vect::int_cback_t>( m_i2c_vector_obj->Get_Int_Func() ),
		m_i2c_vector_number, i2c_assy->int_priority );

	m_i2c->CR1 = 0U;
	m_data_sink = m_i2c->DR;
	/* Enable I2C interrupt vector event */
	uC_Interrupt::Enable_Int( m_i2c_vector_number );
	/* Basic DMA configuration */
	if ( ( nullptr != i2c_assy->rx_dma ) && ( nullptr != i2c_assy->tx_dma ) )
	{
		m_dma_vector_number_tx = i2c_assy->tx_dma->irq_num;
		m_dma_vector_number_rx = i2c_assy->rx_dma->irq_num;
		m_rx_stream = i2c_assy->rx_dma->dma_channel_ctrl;
		m_rx_dma_id = i2c_assy->rx_dma->dma_num;
		m_rx_dma_ctrl = i2c_assy->rx_dma->dma_ctrl;
		m_tx_stream = i2c_assy->tx_dma->dma_channel_ctrl;
		m_tx_dma_id = i2c_assy->tx_dma->dma_num;
		m_tx_dma_ctrl = i2c_assy->tx_dma->dma_ctrl;
		/* Initialize DMA clock */
		Initialize_Peripheral_Clock( &i2c_assy->rx_dma->periph_def );
		Initialize_Peripheral_Clock( &i2c_assy->tx_dma->periph_def );
		/* Configure the interrupt for DMA transmission complete */
		m_dma_vector_obj_tx = new Advanced_Vect();

		m_dma_vector_obj_tx->Set_Vect( &DMA_TX_ISR_Static, this );
		uC_Interrupt::Set_Vector(
			static_cast<Advanced_Vect::int_cback_t>( m_dma_vector_obj_tx->Get_Int_Func() ),
			m_dma_vector_number_tx, i2c_assy->int_priority );
		/* Configure the interrupt for DMA transmission complete */
		m_dma_vector_obj_rx = new Advanced_Vect();

		m_dma_vector_obj_rx->Set_Vect( &DMA_RX_ISR_Static, this );
		uC_Interrupt::Set_Vector(
			static_cast<Advanced_Vect::int_cback_t>( m_dma_vector_obj_rx->Get_Int_Func() ),
			m_dma_vector_number_rx, i2c_assy->int_priority );

		Clear_DMA_Interrupts();
		uC_Interrupt::Enable_Int( m_dma_vector_number_tx );
		uC_Interrupt::Enable_Int( m_dma_vector_number_rx );
		/* Extract the DMA channel that shall be use for transmission and receptuion */
		tx_channel =
			( ( ( static_cast<uint32_t>( i2c_assy->i2c->tx_dma_channel ) ) & 0xF0U ) >> 4U );
		rx_channel =
			( ( ( static_cast<uint32_t>( i2c_assy->i2c->rx_dma_channel ) ) & 0xF0U ) >> 4U );
		/* The location in Control register is from bit 25-27 */
		tx_channel <<= 25U;
		rx_channel <<= 25U;
	}
	else
	{
		m_rx_stream = reinterpret_cast<DMA_Stream_TypeDef*>( nullptr );
		m_tx_stream = reinterpret_cast<DMA_Stream_TypeDef*>( nullptr );
		m_dma_vector_obj_tx = reinterpret_cast<Advanced_Vect*>( nullptr );
		m_dma_vector_obj_rx = reinterpret_cast<Advanced_Vect*>( nullptr );
		m_rx_dma_id = 0U;
	}
	/* GPIO initialization for I2C peripheral */
	Assume_IO_Control( i2c_assy->i2c_io );
	m_active_config = reinterpret_cast<select_config_t*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
STM32F2xx_I2C::~STM32F2xx_I2C( void )
{
	uC_Interrupt::Clear_Vector( m_i2c_vector_number );
	delete m_i2c_vector_obj;

	m_tx_stream->CR = 0U;
	uC_Interrupt::Clear_Vector( m_dma_vector_number_tx );
	delete m_dma_vector_obj_tx;
	delete m_tx_stream;
	delete m_tx_dma_ctrl;
	delete m_tx_data;

	m_rx_stream->CR = 0U;
	uC_Interrupt::Clear_Vector( m_dma_vector_number_rx );
	delete m_dma_vector_obj_rx;
	delete m_rx_stream;
	delete m_rx_dma_ctrl;
	delete m_rx_data;
	m_i2c->CR1 = 0U;
	m_i2c->CR2 = 0U;

	delete m_active_config;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::Start_Exchange( bool_t new_exchange )
{
	uint32_t rx_cr_value;
	uint32_t tx_cr_value;

	state_machine = START_STATE_ACK;
	dma_use = false;
	if ( false != new_exchange )
	{
		m_active_config = m_select;
	}

	if ( true == static_cast<bool_t>( m_i2c->CR1 & I2C_CR1_PE ) )
	{
		Clr_Mask( m_i2c->CR1, I2C_CR1_PE );
	}

	m_i2c->CR2 = m_active_config->i2c_cr2;
	m_i2c->CCR = m_active_config->i2c_ccr;
	m_i2c->TRISE = m_active_config->i2c_trise;
	m_i2c->CR1 |= I2C_CR1_PE;
	m_i2c->CR1 |= m_active_config->i2c_cr1;

	device_address = m_active_segment->chip_address;
	transaction = m_active_segment->exchange_type;

	m_rx_length = m_active_segment->rx_length;
	m_tx_length = m_active_segment->tx_length;
	/* Check if DMA can be used. If not it will be interrupt based transfer */
	if ( ( nullptr == m_dma_vector_obj_tx ) ||
		 ( m_active_segment->tx_length < m_active_config->dma_byte_count_theshold ) ||
		 ( m_active_segment->rx_length < m_active_config->dma_byte_count_theshold ) )
	{
		/* Enable the interrupt events and its all over. State Machine ill take care
		   of next things */

		m_rx_data = ( nullptr == m_active_segment->rx_data ) ?
			&m_dummy_data : m_active_segment->rx_data;
		m_tx_data = ( nullptr == m_active_segment->tx_data ) ?
			&m_dummy_data : m_active_segment->tx_data;
	}
	/* Transfer using DMA */
	else
	{
		dma_use = true;

		rx_cr_value = DMA_SxCR_TCIE | DMA_SxCR_TEIE | DMA_SxCR_DMEIE | rx_channel
			| DMA_SxCR_MINC;

		tx_cr_value = DMA_SxCR_DIR_0 | DMA_SxCR_TCIE | DMA_SxCR_TEIE
			| DMA_SxCR_DMEIE | tx_channel | DMA_SxCR_MINC;

		m_rx_stream->NDTR = m_active_segment->rx_length;
		m_tx_stream->NDTR = m_active_segment->tx_length;

		/* Check if we have to receive any data in receive buffer */
		if ( nullptr != m_active_segment->rx_data )
		{
			m_rx_stream->M0AR = reinterpret_cast<uint32_t>( m_active_segment->rx_data );
			rx_cr_value |= DMA_SxCR_MINC;
		}
		else
		{
			m_rx_stream->M0AR = reinterpret_cast<uint32_t>( &m_dummy_data );
		}

		/* Check if we have to transmit any data */
		if ( nullptr != m_active_segment->tx_data )
		{
			m_tx_stream->M0AR = reinterpret_cast<uint32_t>( m_active_segment->tx_data );
			tx_cr_value |= DMA_SxCR_MINC;
		}
		else
		{
			m_tx_stream->M0AR = reinterpret_cast<uint32_t>( &m_dummy_data );
		}

		/* Disable DMA Channel for Rx Stream and assign peripheral address */
		m_rx_stream->CR &= ~( DMA_SxCR_EN );
		m_rx_stream->CR = 0U;
		m_rx_stream->PAR = reinterpret_cast<uint32_t>( &( m_i2c->DR ) );
		/* Disable DMA Channel for Rx Stream and assign peripheral address */
		m_tx_stream->CR &= ~( DMA_SxCR_EN );
		m_tx_stream->CR = 0U;
		m_tx_stream->PAR = reinterpret_cast<uint32_t>( &( m_i2c->DR ) );
		/* Enable I2C DMA transfer */
		m_i2c->CR2 |= I2C_CR2_DMAEN;
		/* Fill in the control register for DMA both RX stram and TX stream */
		m_rx_stream->CR = rx_cr_value;
		m_tx_stream->CR = tx_cr_value;

	}
	m_i2c->CR2 |= I2C_CR2_ITERREN | I2C_CR2_ITEVTEN;
	/* Flush the data register */
	m_data_sink = m_i2c->DR;
	m_i2c->CR1 |= I2C_CR1_ACK;
	/* Generate the start condition to start state machine */
	Generate_Start();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::DMA_TX_ISR( void )
{
	bool_t failure;

	m_tx_stream->CR &= ~( static_cast<uint32_t>( DMA_SxCR_EN ) );
	m_tx_stream->CR = 0U;
	failure = Clear_DMA_Interrupts();
	/* Transmission Complete */
	if ( ( transaction == I2C_WRITE_EXCHANGE ) && ( MAX_STATE_ACK == state_machine ) )
	{
		Wait_Flag_Timeout( I2C_SR1_TXE );
		Bus_Busy_TimeOut();
		Set_Stop();
		Get_SR1();
		m_i2c->CR1 = 0U;
		m_i2c->CR2 = 0U;
		Exchange_Complete( failure );
	}
	else
	{
		m_i2c->CR2 |= ( I2C_CR2_ITERREN | I2C_CR2_ITEVTEN );
		Generate_Start();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::DMA_RX_ISR( void )
{
	bool_t failure;

	m_rx_stream->CR &= ~( static_cast<uint32_t>( DMA_SxCR_EN ) );
	m_rx_stream->CR = 0U;
	/* Transmission Complete */
	failure = Clear_DMA_Interrupts();
	if ( transaction == I2C_READ_EXCHANGE )
	{
		Set_Stop();
		Get_SR1();
		m_i2c->CR1 = 0U;
		m_i2c->CR2 = 0U;
		Exchange_Complete( failure );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::I2C_ISR( void )
{
	uint16_t i2c_address;

	/* It looks like a bulky ISR.
	   However from timing perspective only single  case shall be executed during one interrupt
	   The reading of SR1 and SR2 is crucial in I2C. Its not redundant code*/
	if ( dma_use == false )
	{
		switch ( state_machine )
		{	/* State for sending device address/ start frame ACK */
			case START_STATE_ACK:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_SB ) )
				{
					Get_SR2();
					/* To check if it is a 10 bit addressing mode or 7 bit addressing mode */
					if ( device_address < 0x7FU )
					{	/* 0 because its write operation */
						m_i2c->DR = static_cast<uint16_t>
							( ( device_address << 1U ) /* | 0x00U */ );
						state_machine = SET_DEVICE_ADDRESS_ACK;
					}
					else if ( ( device_address > 0x7FU ) && ( device_address < 0x3FFU ) )
					{
						i2c_address = ( ( device_address >> 8U ) & 0x03U );
						i2c_address = static_cast<uint16_t>( i2c_address << 1 );
						m_i2c->DR = ( ADDR_HEADER_10_BIT | i2c_address /* | 0x00U */ );
						state_machine = SET_ADDRESS_BYTE_2;
					}
					else
					{
						state_machine = MAX_STATE_ACK;
						Exchange_Complete( false );
					}
				}
				break;

			/* Send upper byte of an address in case of 10 bit addressing mode */
			case SET_ADDRESS_BYTE_2:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_ADD10 ) )
				{
					Get_SR2();
					i2c_address = device_address & 0xFFU;
					m_i2c->DR = i2c_address;
					state_machine = SET_DEVICE_ADDRESS_ACK;
				}
				break;

			/* Device address Acknowledge state. Also used for sendind command to device */
			case SET_DEVICE_ADDRESS_ACK:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_TXE ) )
				{
					Get_SR2();
					m_i2c->DR = *m_tx_data;
					m_tx_data += 1U;
					m_tx_length--;
					if ( transaction == I2C_WRITE_EXCHANGE )
					{
						state_machine = SEND_DATA_ACK;
					}
					else
					{
						state_machine = REPEATED_START_STATE_ACK;
					}
				}
				break;

			/* Repeated start condition in case of read mode */
			case REPEATED_START_STATE_ACK:
				if ( ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_BTF ) ) ) ||
					 ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_TXE ) ) ) )
				{
					Generate_Start();
					state_machine = REPEAT_DEVICE_ADDRESS_ACK;
				}
				break;

			/* Repeat Device address in case of I2C read mode */
			case REPEAT_DEVICE_ADDRESS_ACK:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_SB ) )
				{
					Get_SR2();
					if ( device_address < 0x7FU )
					{	/* 1 because its read operation */
						m_i2c->DR =
							( ( static_cast<uint16_t>( device_address << 1U ) ) | 0x01U );
						state_machine = RECEIVE_DATA_ACK;
					}
					else if ( ( device_address > 0x7FU ) && ( device_address < 0x3FFU ) )
					{
						i2c_address = ( ( device_address >> 8U ) & 0x03U );
						i2c_address = static_cast<uint16_t>( i2c_address << 1U );
						m_i2c->DR = ( ADDR_HEADER_10_BIT | i2c_address | 0x01U );
						state_machine = RECEIVE_DATA_ACK;
					}
					else
					{
						state_machine = MAX_STATE_ACK;
						Exchange_Complete( false );
					}
				}
				break;

			/* State for sending data to slave */
			case SEND_DATA_ACK:
				if ( ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_BTF ) ) ) ||
					 ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_TXE ) ) ) )
				{
					m_tx_length--;
					m_i2c->DR = *m_tx_data;
					m_tx_data += 1U;
					if ( m_tx_length == 0U )
					{
						state_machine = STOP_CONDITION_ACK;
					}
				}
				break;

			/* State for receiving data from slave */
			case RECEIVE_DATA_ACK:
				Get_SR2();
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_RXNE ) )
				{
					*m_rx_data = static_cast<uint8_t>( m_i2c->DR );
					m_rx_length--;
					m_rx_data += 1U;
					if ( m_rx_length == 0U )
					{
						state_machine = STOP_CONDITION_ACK;
					}
					Get_SR2();
				}
				break;

			/* State which sets the stop bit */
			case STOP_CONDITION_ACK:
				if ( ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_BTF ) ) ) ||
					 ( true == static_cast<bool_t>( ( Get_SR1() & I2C_SR1_TXE ) ) ) )
				{
					Get_SR2();
					state_machine = MAX_STATE_ACK;
					m_i2c->CR2 &= ~( I2C_CR2_ITERREN | I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN );
					Set_Stop();
					Exchange_Complete( false );
				}
				break;

			/* Below cases to satisfy MISRA*/
			case SET_COMMAND_ACK:

			case MAX_STATE_ACK:

			default:
				break;

		}
	}
	/* In case the transfer is through DMA */
	else
	{
		switch ( state_machine )
		{	/* State for sending device address/ start frame ACK */
			case START_STATE_ACK:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_SB ) )
				{
					Get_SR2();
					if ( device_address < 0x7FU )
					{
						m_i2c->DR = static_cast<uint16_t>
							( ( device_address << 1U ) /* | 0x00U */ );
						state_machine = SET_DEVICE_ADDRESS_ACK;
					}
					else if ( ( device_address > 0x7FU ) && ( device_address < 0x3FFU ) )
					{
						i2c_address = ( ( device_address >> 8U ) & 0x03U );
						i2c_address = static_cast<uint16_t>( i2c_address << 1 );
						m_i2c->DR = ( ADDR_HEADER_10_BIT | i2c_address /* | 0x00U */ );
						state_machine = SET_ADDRESS_BYTE_2;
					}
					else
					{
						state_machine = MAX_STATE_ACK;
						Exchange_Complete( false );
					}
				}
				break;

			/* Send upper byte of an address in case of 10 bit addressing mode */
			case SET_ADDRESS_BYTE_2:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_ADD10 ) )
				{
					Get_SR2();
					i2c_address = device_address & 0xFFU;
					m_i2c->DR = i2c_address;
					state_machine = SET_DEVICE_ADDRESS_ACK;
				}
				break;

			/* Device address Acknowledge state. Also used for initiating DMA transfer */
			case SET_DEVICE_ADDRESS_ACK:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_TXE ) )
				{
					Get_SR2();
					if ( transaction == I2C_WRITE_EXCHANGE )
					{
						state_machine = MAX_STATE_ACK;
					}
					else
					{
						state_machine = REPEAT_DEVICE_ADDRESS_ACK;
					}
					m_i2c->CR2 &= ~( I2C_CR2_ITERREN | I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN );
					m_tx_stream->CR |= DMA_SxCR_EN;
					/* Set TX DMA for Write Mode
					   Set TX DMA in Read Mode*/
					/* If Write mode statemachine will goto MAX State as STOP will be handled inside
					   DMA TX ISR */
					/* If Read mode statemachine will goto REPEAT_ADDRESS STATE as repeated start
					   shall be handled from DMA TX ISR*/
				}
				break;

			/* Repeat Device address in case of I2C read mode */
			case REPEAT_DEVICE_ADDRESS_ACK:
				if ( true == static_cast<bool_t>( Get_SR1() & I2C_SR1_SB ) )
				{
					Get_SR2();
					/* Diasbale RX DMA here Disable I2C interrupts */
					m_i2c->CR2 &= ~( I2C_CR2_ITERREN | I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN );
					m_i2c->CR2 |= I2C_CR2_LAST;
					m_rx_stream->CR |= DMA_SxCR_EN;
					if ( m_rx_length == 1U )
					{
						m_i2c->CR1 &= ( ~I2C_CR1_ACK );
					}

					if ( device_address < 0x7FU )
					{
						m_i2c->DR =
							( ( static_cast<uint16_t>( device_address << 1U ) ) | 0x01U );
						Wait_Flag_Timeout( I2C_SR1_ADDR );
					}
					else if ( ( device_address > 0x7FU ) && ( device_address < 0x3FFU ) )
					{
						i2c_address = ( ( device_address >> 8U ) & 0x03U );
						i2c_address = static_cast<uint16_t>( i2c_address << 1 );
						m_i2c->DR = ( ADDR_HEADER_10_BIT | i2c_address | 0x01U );
						Wait_Flag_Timeout( I2C_SR1_ADDR );
					}
					else
					{
						Exchange_Complete( false );
					}
					state_machine = RECEIVE_DATA_ACK;
				}
				Get_SR2();
				break;

			/* The below cases will not be used. But are defined here to suppress MISRA */
			case SET_COMMAND_ACK:

			case REPEATED_START_STATE_ACK:

			case SEND_DATA_ACK:

			case RECEIVE_DATA_ACK:

			case STOP_CONDITION_ACK:

			case MAX_STATE_ACK:

			default:
				break;

		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::Assume_IO_Control( const uC_BASE_I2C_IO_PORT_STRUCT* i2c_io ) const
{
	/* Enable serial clock pin for I2C */
	uC_IO_Config::Enable_Periph_Output_Pin( i2c_io->scl_port, true );
	Set_Bit( ( ( i2c_io->scl_port )->reg_ctrl )->OTYPER, ( i2c_io->scl_port )->pio_num );
	/* Enable serial data line for I2C */
	uC_IO_Config::Enable_Periph_Output_Pin( i2c_io->sda_port, true );
	Set_Bit( ( ( i2c_io->sda_port )->reg_ctrl )->OTYPER, ( i2c_io->sda_port )->pio_num );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::Initialize_Peripheral_Clock( const PERIPH_DEF_ST* periph_def ) const
{
	uC_Base::Reset_Periph( const_cast<PERIPH_DEF_ST*>( periph_def ) );
	uC_Base::Enable_Periph_Clock( const_cast<PERIPH_DEF_ST*>( periph_def ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t STM32F2xx_I2C::Clear_DMA_Interrupts()
{
	static const uint32_t dma_int_flags[] =
	{
		DMA_LISR_TCIF0 | DMA_LISR_HTIF0 | DMA_LISR_TEIF0 | DMA_LISR_DMEIF0
		| DMA_LISR_FEIF0,
		DMA_LISR_TCIF1 | DMA_LISR_HTIF1 | DMA_LISR_TEIF1 | DMA_LISR_DMEIF1
		| DMA_LISR_FEIF1,
		DMA_LISR_TCIF2 | DMA_LISR_HTIF2 | DMA_LISR_TEIF2 | DMA_LISR_DMEIF2
		| DMA_LISR_FEIF2,
		DMA_LISR_TCIF3 | DMA_LISR_HTIF3 | DMA_LISR_TEIF3 | DMA_LISR_DMEIF3
		| DMA_LISR_FEIF3,
		DMA_HISR_TCIF4 | DMA_HISR_HTIF4 | DMA_HISR_TEIF4 | DMA_HISR_DMEIF4
		| DMA_HISR_FEIF4,
		DMA_HISR_TCIF5 | DMA_HISR_HTIF5 | DMA_HISR_TEIF5 | DMA_HISR_DMEIF5
		| DMA_HISR_FEIF5,
		DMA_HISR_TCIF6 | DMA_HISR_HTIF6 | DMA_HISR_TEIF6 | DMA_HISR_DMEIF6
		| DMA_HISR_FEIF6,
		DMA_HISR_TCIF7 | DMA_HISR_HTIF7 | DMA_HISR_TEIF7 | DMA_HISR_DMEIF7
		| DMA_HISR_FEIF7
	};

	uint32_t flag_settings;

	if ( m_rx_dma_id < 4U )
	{
		flag_settings = m_rx_dma_ctrl->LISR & dma_int_flags[m_rx_dma_id];
		m_rx_dma_ctrl->LIFCR |= dma_int_flags[m_rx_dma_id];
	}
	else
	{
		flag_settings = m_rx_dma_ctrl->HISR & dma_int_flags[m_rx_dma_id];
		m_rx_dma_ctrl->HIFCR |= dma_int_flags[m_rx_dma_id];
	}

	if ( m_tx_dma_id < 4U )
	{
		flag_settings = m_tx_dma_ctrl->LISR & dma_int_flags[m_tx_dma_id];
		m_tx_dma_ctrl->LIFCR |= dma_int_flags[m_tx_dma_id];
	}
	else
	{
		flag_settings = m_tx_dma_ctrl->HISR & dma_int_flags[m_tx_dma_id];
		m_tx_dma_ctrl->HIFCR |= dma_int_flags[m_tx_dma_id];
	}

	return ( ( true == static_cast<bool_t>( flag_settings & ( DMA_LISR_TEIF0 | DMA_LISR_DMEIF0 |
															  DMA_LISR_TEIF1 | DMA_LISR_DMEIF1 |
															  DMA_LISR_TEIF2 | DMA_LISR_DMEIF2 |
															  DMA_LISR_TEIF3 |
															  DMA_LISR_DMEIF3 ) ) ) ?
			 true : false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::select_config_t::Configure( i2c_speed_mode_t i2c_mode,
												uint32_t i2c_pclk1_freq,
												duty_cycle_t fmode_duty_cycle,
												uint16_t byte_count_theshold )
{
	uint16_t i2c_ccr_value;
	uint16_t fast_mode_duty = 0U;

	i2c_cr1 = 0U;
	i2c_cr2 = 0U;
	dma_byte_count_theshold = byte_count_theshold;

	Set_Mask( i2c_cr2, I2C_CR2_FREQ, ( i2c_pclk1_freq / 1000000U ) );

	if ( I2C_FAST_SPEED == i2c_mode )
	{
		if ( DUTYCYCLE_FASTMODE_2 == fmode_duty_cycle )
		{
			i2c_ccr_value =
				static_cast<uint16_t>( i2c_pclk1_freq / ( I2C_ClockSpeed_400KHz * 3U ) );
			fast_mode_duty = static_cast<uint16_t>( 0x8000U );
		}
		else/* FASTMODE_DUTYCYCLE_16_9 */
		{
			i2c_ccr_value =
				static_cast<uint16_t>( i2c_pclk1_freq / ( I2C_ClockSpeed_400KHz * 25U ) );
			fast_mode_duty = static_cast<uint16_t>( 0xC000U );
		}
		if ( i2c_ccr_value < 1U )
		{
			/* Set minimum allowed value */
			i2c_ccr_value = 1U;
		}
		i2c_ccr = ( i2c_ccr_value | fast_mode_duty );
		i2c_trise = static_cast<uint16_t>( ( i2c_pclk1_freq * 3U ) / ( 10000000U ) ) + 1U;
		/*in fast mode, the maximum allowed SCL rise time is 300 ns */
	}
	else
	{
		i2c_ccr_value =
			static_cast<uint16_t>( ( i2c_pclk1_freq ) / ( I2C_ClockSpeed_100KHz << 1U ) );
		if ( i2c_ccr_value < 4U )
		{
			/* Set minimum allowed value */
			i2c_ccr_value = 4U;
		}
		i2c_ccr = i2c_ccr_value;
		i2c_trise = static_cast<uint16_t>( i2c_pclk1_freq / 1000000U ) + 1U;
		/*in standard mode, the maximum allowed SCL rise time is 1000 ns*/
	}

	i2c_cr1 |= I2C_CR1_ACK;	/* Acknowledge Enable */
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::Generate_Start( void )
{
	m_i2c->CR1 |= I2C_CR1_START;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::Set_Stop( void )
{
	m_i2c->CR1 &= ( ~I2C_CR1_ACK );
	m_i2c->CR1 |= I2C_CR1_STOP;
	Nop();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t STM32F2xx_I2C::Get_SR1( void ) const
{
	return ( m_i2c->SR1 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t STM32F2xx_I2C::Get_SR2( void ) const
{
	return ( m_i2c->SR2 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::Wait_Flag_Timeout( uint32_t flag_mask ) const
{
	uint32_t timeout_count = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * I2C_TIMEOUT );

	while ( ( ( Get_SR1() & flag_mask ) == 0x00U ) && ( timeout_count > 0U ) )
	{
		__no_operation();
		timeout_count--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void STM32F2xx_I2C::Bus_Busy_TimeOut( void )
{
	uint32_t timeout = static_cast<uint32_t>( PROCESSOR_FREQ_MHZ * 100U );

	while ( ( ( ( ( uint16_t )m_i2c->SR2 ) & I2C_SR2_BUSY ) != 0U ) && ( timeout > 0U ) )
	{
		__no_operation();
		timeout--;
	}
}
