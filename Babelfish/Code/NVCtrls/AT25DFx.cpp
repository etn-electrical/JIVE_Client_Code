/**
 *****************************************************************************************
 *	@file AT25DFx.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "AT25DFx.h"
#include "NV_Ctrl_Debug.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{

BF_Lib::Timers* AT25DFx::m_wait_timer = reinterpret_cast<BF_Lib::Timers*>( nullptr );
bool AT25DFx::m_callback_pending = false;
uint16_t AT25DFx::m_delay_step = 0U;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
AT25DFx::AT25DFx( const AT25DFx_attribute_struct_t* attrib, uC_Queued_SPI* spi_ctrl,
				  uint8_t select ) :
	NV_Ctrl( &attrib->base ),
	m_spi_ctrl( reinterpret_cast<uC_Queued_SPI*>( nullptr ) ),
	m_cback( reinterpret_cast<NV_Ctrl::cback_func_t>( nullptr ) ),
	m_cback_param( reinterpret_cast<NV_Ctrl::cback_param_t>( nullptr ) ),
	m_address( 0U ),
	m_length( 0U ),
	m_max_address( 0U ),
	m_write_address( 0U ),
	m_command{ 0U },
	m_write_buffer{ 0U },
	m_write_length( 0U ),
	m_write_data( reinterpret_cast<const uint8_t*>( nullptr ) ),
	m_block_erase_opcode( 0U ),
	m_read_opcode( 0U ),
	m_read_command_length( 0U ),
	m_state( 0U ),
	m_delayed_result( SUCCESS ),
	m_data_seg{ nullptr, nullptr, nullptr, 0U }
{

	uC_Queued_SPI::Initialize_Exchange_Struct( &m_exchange );
	m_spi_ctrl = spi_ctrl;
	/* The AT25DF operates in SPI mode 0, frequency is 10MHz and we will assign
	   the chip slave id 0.i.e. chip 0 on that particular SPI channel
	 */
	m_spi_ctrl->SPI_Chip_Configuration( uC_Queued_SPI::MODE_0, 10000000U, select );
	m_max_address = attrib->base.block_count * attrib->base.block_size;
	m_read_opcode = attrib->read_opcode;

	switch ( m_read_opcode )
	{
		case FAST_READ_OPCODE:
			m_read_command_length = 6U;
			break;

		case MEDIUM_READ_OPCODE:
			m_read_command_length = 5U;
			break;

		case SLOW_READ_OPCODE:
			m_read_command_length = 4U;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	switch ( attrib->base.erase_block_size )
	{
		case 4096:
			m_block_erase_opcode = static_cast<uint8_t>( BLOCK_ERASE_4K_OPCODE );
			break;

		case 32768:
			m_block_erase_opcode = static_cast<uint8_t>( BLOCK_ERASE_32K_OPCODE );
			break;

		case 65536:
			m_block_erase_opcode = static_cast<uint8_t>( BLOCK_ERASE_64K_OPCODE );
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	m_exchange.select_id = select;
	m_exchange.exchange_segment = &m_command_seg;
	m_exchange.post_exchange_cback = &Continue_Call_Back;
	m_exchange.call_back_arg = this;

	m_state = static_cast<uint8_t>( IDLE );

	m_wait_timer = new BF_Lib::Timers( &Wait_Time_Expired_Static,
									   reinterpret_cast<BF_Lib::Timers::TIMERS_CBACK_PARAM_TD>( this ),
									   BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
									   "Wait Time Expired" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DFx::Check_Address( uint32_t address, uint32_t length )
{
	NV_Ctrl::nv_status_t result;

	if ( 0U == length )
	{
		result = NV_Ctrl::DATA_ERROR;
	}
	else if ( m_max_address < ( address + length ) )
	{
		result = NV_Ctrl::INVALID_ADDRESS;
	}
	else
	{
		m_address = address;
		m_length = length;
		result = NV_Ctrl::SUCCESS;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DFx::Process_Call_Back( void )
{
	uint32_t length;
	uint32_t index;

	switch ( m_state )
	{
		case PAGE_PROGRAM:
		case ERASE_BLOCK:
		case WRITE_BLOCK_ERASE:
		case WRITE_BLOCK_WRITE:
			if ( static_cast<uint8_t>( PAGE_PROGRAM ) == m_state )
			{
				m_command[0] = static_cast<uint8_t>( PAGE_PROGRAM_OPCODE );
				length = m_mem_attributes->block_size
					- ( m_address & ( m_mem_attributes->block_size - 1U ) );

				m_command_seg.next_exchange_segment = &m_data_seg;
			}
			else if ( static_cast<uint8_t>( ERASE_BLOCK ) == m_state )
			{
				m_command[0] = m_block_erase_opcode;
				length = m_mem_attributes->erase_block_size
					- ( m_address & ( m_mem_attributes->erase_block_size - 1U ) );

				m_command_seg.next_exchange_segment =
					reinterpret_cast<uC_Queued_SPI::exchange_segment_t*>( nullptr );
			}
			else if ( static_cast<uint8_t>( WRITE_BLOCK_ERASE ) == m_state )
			{
				index = m_write_address & ( sizeof ( m_write_buffer ) - 1U );
				length = sizeof ( m_write_buffer ) - index;

				if ( length > m_write_length )
				{
					length = m_write_length;
				}

				m_write_address += length;
				m_write_length -= length;

				while ( length > 0U )
				{
					length--;
					m_write_buffer[index] = *m_write_data;
					m_write_data += 1;
					index += 1U;
				}

				m_command[0] = static_cast<uint8_t>( BLOCK_ERASE_4K_OPCODE );
				length = 0U;
				m_state = static_cast<uint8_t>( WRITE_BLOCK_WRITE );
				m_length = sizeof ( m_write_buffer );

				m_command_seg.next_exchange_segment =
					reinterpret_cast<uC_Queued_SPI::exchange_segment_t*>( nullptr );
				m_data_seg.tx_data = &m_write_buffer[0];
			}
			else
			{
				m_command[0] = static_cast<uint8_t>( PAGE_PROGRAM_OPCODE );
				length = m_mem_attributes->block_size;
				m_command_seg.next_exchange_segment = &m_data_seg;
			}

			if ( length > m_length )
			{
				length = m_length;
			}

			m_data_seg.length = static_cast<uint16_t>( length );
			m_data_seg.rx_data = reinterpret_cast<uint8_t*>( nullptr );

			m_command[1] = static_cast<uint8_t>( ( m_address >> 16U ) & 0xFFU );
			m_command[2] = static_cast<uint8_t>( ( m_address >> 8U ) & 0xFFU );
			m_command[3] = static_cast<uint8_t>( ( m_address >> 0U ) & 0xFFU );

			m_command_seg.length = 4U;
			break;

		case ERASE_ALL:
			m_command[0] = static_cast<uint8_t>( CHIP_ERASE_OPCODE );
			m_command_seg.length = 1U;
			m_command_seg.next_exchange_segment =
				reinterpret_cast<uC_Queued_SPI::exchange_segment_t*>( nullptr );
			m_data_seg.length = 0U;
			break;

		case WRITE_STATUS:
			m_command[0] = static_cast<uint8_t>( WRITE_STATUS_OPCODE );
			m_command[1] = *m_data_seg.tx_data;
			m_command_seg.length = 2U;
			m_command_seg.next_exchange_segment =
				reinterpret_cast<uC_Queued_SPI::exchange_segment_t*>( nullptr );
			m_data_seg.length = 0U;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	m_state |= static_cast<uint8_t>( OP_IN_PROGRESS_FLAG );
	m_command_seg.tx_data = &m_command[0];
	m_command_seg.rx_data = reinterpret_cast<uint8_t*>( nullptr );

	m_spi_ctrl->Process_Exchange( &m_exchange );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DFx::Initiate_Exchange( void )
{
	m_delayed_result = NV_Ctrl::PENDING_CALL_BACK;

	m_spi_ctrl->Process_Exchange( &m_exchange );

	if ( nullptr == m_cback )
	{
		while ( NV_Ctrl::PENDING_CALL_BACK == m_delayed_result )
		{}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DFx::Read( uint8_t* data, uint32_t address, uint32_t length,
									cback_func_t cback, cback_param_t param )
{
	m_delayed_result = Check_Address( address, length );

	if ( NV_Ctrl::SUCCESS == m_delayed_result )
	{
		m_cback = cback;
		m_cback_param = param;

		m_command[0] = m_read_opcode;
		m_command[1] = static_cast<uint8_t>( ( address >> 16U ) & 0xFFU );
		m_command[2] = static_cast<uint8_t>( ( address >> 8U ) & 0xFFU );
		m_command[3] = static_cast<uint8_t>( ( address >> 0U ) & 0xFFU );
		m_command[4] = 0U;
		m_command[5] = 0U;

		m_command_seg.tx_data = &m_command[0];
		m_command_seg.rx_data = reinterpret_cast<uint8_t*>( nullptr );
		m_command_seg.length = m_read_command_length;
		m_command_seg.next_exchange_segment = &m_data_seg;

		m_data_seg.tx_data = reinterpret_cast<uint8_t*>( nullptr );
		m_data_seg.rx_data = data;
		m_data_seg.length = static_cast<uint16_t>( length );

		Initiate_Exchange();
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to read from Address: %X, Length: %u, ErrorStatus: %d",
					   address, length, m_delayed_result );
	}

	return ( m_delayed_result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DFx::Write( const uint8_t* data, uint32_t address, uint32_t length,
									 cback_func_t cback, cback_param_t param )
{
	m_delayed_result = Check_Address( address, length );

	if ( NV_Ctrl::SUCCESS == m_delayed_result )
	{
		m_write_data = data;
		m_write_address = address;
		m_write_length = length;
		m_state = static_cast<uint8_t>( WRITE_BLOCK_ERASE );
		Read( &m_write_buffer[0], m_write_address & ~( sizeof ( m_write_buffer ) - 1U ),
			  sizeof ( m_write_buffer ), cback, param );
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to write from Address: %X, Length: %u, ErrorStatus: %d",
					   address, length, m_delayed_result );
	}

	return ( m_delayed_result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DFx::Erase( uint32_t address, uint32_t length,
									 cback_func_t cback, cback_param_t param )
{
	m_delayed_result = Check_Address( address, length );

	if ( NV_Ctrl::SUCCESS == m_delayed_result )
	{
		m_cback = cback;
		m_cback_param = param;

		m_state = static_cast<uint8_t>( ERASE_BLOCK );

		Enable_Writes( true );
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to Erase from Address: %X, Length: %u, ErrorStatus: %d",
					   address, length, m_delayed_result );
	}

	return ( m_delayed_result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DFx::Erase_All( cback_func_t cback, cback_param_t param )
{
	m_cback = cback;
	m_cback_param = param;
	m_length = 0U;

	m_state = static_cast<uint8_t>( ERASE_ALL );

	Enable_Writes( true );

	return ( m_delayed_result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DFx::Ready( cback_func_t cback, cback_param_t param )
{
	m_cback = cback;
	m_cback_param = param;

	Get_Status();

	return ( m_delayed_result );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DFx::Get_Status( void )
{
	m_command[0] = static_cast<uint8_t>( READ_STATUS_OPCODE );
	m_command[1] = 0U;

	m_command_seg.tx_data = &m_command[0];
	m_command_seg.rx_data = &m_command[2];
	m_command_seg.length = 2U;
	m_command_seg.next_exchange_segment =
		reinterpret_cast<uC_Queued_SPI::exchange_segment_t*>( nullptr );

	m_data_seg.rx_data = &m_command[1];

	Initiate_Exchange();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DFx::Set_Status( const uint8_t* data, cback_func_t cback,
										  cback_param_t param )
{
	m_cback = cback;
	m_cback_param = param;
	m_length = 0U;

	m_data_seg.tx_data = const_cast<uint8_t*>( data );
	m_data_seg.rx_data = reinterpret_cast<uint8_t*>( nullptr );

	m_state = static_cast<uint8_t>( WRITE_STATUS );

	Enable_Writes( true );

	return ( m_delayed_result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DFx::Enable_Writes( bool initial_exchange )
{
	m_command[0] = static_cast<uint8_t>( WRITE_ENABLE_OPCODE );
	m_command_seg.length = 1U;
	m_command_seg.tx_data = &m_command[0];
	m_command_seg.rx_data = reinterpret_cast<uint8_t*>( nullptr );
	m_command_seg.next_exchange_segment =
		reinterpret_cast<uC_Queued_SPI::exchange_segment_t*>( nullptr );

	if ( false == initial_exchange )
	{
		m_delayed_result = NV_Ctrl::PENDING_CALL_BACK;
		m_spi_ctrl->Process_Exchange( &m_exchange );
	}
	else
	{
		Initiate_Exchange();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DFx::Continue()
{

	static uint16_t delay_counter = 0U;

	if ( uC_Queued_SPI::EXCHANGE_FAILED == m_exchange.status )
	{
		m_state = static_cast<uint8_t>( IDLE );
		m_delayed_result = NV_Ctrl::DATA_ERROR;
	}
	else
	{
		switch ( m_command[0] )
		{
			case WRITE_STATUS_OPCODE:
				m_delayed_result = NV_Ctrl::SUCCESS;
				break;

			case FAST_READ_OPCODE:
			case MEDIUM_READ_OPCODE:
			case SLOW_READ_OPCODE:
				if ( static_cast<uint8_t>( IDLE ) == m_state )
				{
					m_delayed_result = NV_Ctrl::SUCCESS;
				}
				else
				{
					Enable_Writes( false );
				}
				break;

			case WRITE_ENABLE_OPCODE:
				Process_Call_Back();
				break;

			case CHIP_ERASE_OPCODE:
			case BLOCK_ERASE_4K_OPCODE:
			case BLOCK_ERASE_32K_OPCODE:
			case BLOCK_ERASE_64K_OPCODE:
			case PAGE_PROGRAM_OPCODE:
				if ( m_callback_pending == false )
				{
					m_callback_pending = true;
					if ( m_command[0] == PAGE_PROGRAM_OPCODE )
					{
						m_delay_step = PAGE_PROGRAM_DELAY_STEP;
						m_wait_timer->Start( PAGE_PROGRAM_TYP_DELAY, false );
					}
					else if ( m_command[0] == BLOCK_ERASE_4K_OPCODE )
					{
						m_delay_step = BLOCK_ERASE_4K_DELAY_STEP;
						m_wait_timer->Start( BLOCK_ERASE_4K_TYP_DELAY, false );
					}
					else if ( m_command[0] == BLOCK_ERASE_32K_OPCODE )
					{
						m_delay_step = BLOCK_ERASE_32K_DELAY_STEP;
						m_wait_timer->Start( BLOCK_ERASE_32K_TYP_DELAY, false );
					}
					else if ( m_command[0] == BLOCK_ERASE_64K_OPCODE )
					{
						m_delay_step = BLOCK_ERASE_64K_DELAY_STEP;
						m_wait_timer->Start( BLOCK_ERASE_32K_TYP_DELAY, false );
					}
					else if ( m_command[0] == CHIP_ERASE_OPCODE )
					{
						m_delay_step = CHIP_ERASE_DELAY_STEP;
						m_wait_timer->Start( CHIP_ERASE_TYP_DELAY, false );
					}
				}
				// m_delayed_result	= NV_Ctrl::BUSY_ERROR;

				break;

			case READ_STATUS_OPCODE:
				*m_data_seg.rx_data = m_command[3];

				if ( static_cast<uint8_t>( IDLE ) == m_state )
				{
					m_delayed_result =
						( 0U != ( m_command[3] & static_cast<uint8_t>( STATUS_BSY ) ) ) ?
						NV_Ctrl::BUSY_ERROR : NV_Ctrl::SUCCESS;
				}
				else
				{
					if ( 0U == ( m_command[3] & static_cast<uint8_t>( STATUS_WPP ) ) )
					{
						m_delayed_result = NV_Ctrl::WRITE_PROTECTED;
						m_state = static_cast<uint8_t>( IDLE );
					}
					else if ( 0U != ( m_command[3] & static_cast<uint8_t>( STATUS_EPE ) ) )
					{
						m_delayed_result = NV_Ctrl::DATA_ERROR;
						m_state = static_cast<uint8_t>( IDLE );
					}
					else
					{
						if ( 0U == ( m_command[3] & static_cast<uint8_t>( STATUS_BSY ) ) )
						{
							delay_counter = 0;
							m_length -= m_data_seg.length;
							m_data_seg.tx_data += m_data_seg.length;
							m_address += m_data_seg.length;

							if ( 0U < m_length )
							{
								m_state &= ~OP_IN_PROGRESS_FLAG;
								m_delayed_result = NV_Ctrl::BUSY_ERROR;

								Enable_Writes( false );
							}
							else
							{
								if ( ( 0U < m_write_length ) &&
									 ( static_cast<uint8_t>( WRITE_BLOCK_WRITE_WAIT ) ==
									   m_state ) )
								{
									m_state = static_cast<uint8_t>( WRITE_BLOCK_ERASE );
									m_address = m_write_address;
									m_command[0] = m_read_opcode;
									m_command[1] =
										static_cast<uint8_t>( ( m_address >> 16 ) & 0xFFU );
									m_command[2] =
										static_cast<uint8_t>( ( m_address >> 8 ) & 0xFFU );
									m_command[3] =
										static_cast<uint8_t>( ( m_address >> 0 ) & 0xFFU );
									m_command[4] = 0U;
									m_command[5] = 0U;

									m_command_seg.tx_data = &m_command[0];
									m_command_seg.rx_data =
										reinterpret_cast<uint8_t*>( nullptr );
									m_command_seg.length = m_read_command_length;
									m_command_seg.next_exchange_segment = &m_data_seg;

									m_data_seg.tx_data = reinterpret_cast<uint8_t*>( nullptr );
									m_data_seg.rx_data = &m_write_buffer[0];
									m_data_seg.length =
										static_cast<uint16_t>( sizeof ( m_write_buffer ) );

									m_spi_ctrl->Process_Exchange( &m_exchange );
								}
								else
								{
									m_state = static_cast<uint8_t>( IDLE );
									m_delayed_result = NV_Ctrl::SUCCESS;
								}
							}
						}
						else
						{
							delay_counter++;
							if ( delay_counter <= NUMBER_OF_DELAY_POLLS )
							{
								if ( m_callback_pending == false )
								{
									m_callback_pending = true;
									m_wait_timer->Start( m_delay_step, false );
								}
							}
							else
							{
								delay_counter = 0;
								m_delayed_result = NV_Ctrl::BUSY_ERROR;
							}
						}
					}
				}
				break;

			default:
				m_delayed_result = NV_Ctrl::DATA_ERROR;
				break;
		}
	}

	if ( ( NV_Ctrl::PENDING_CALL_BACK != m_delayed_result ) &&
		 ( nullptr != m_cback ) )
	{
		m_cback( m_cback_param, m_delayed_result );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DFx::Wait_Time_Expired( void )
{
	m_callback_pending = false;

	Get_Status();
}

}

