/*

 ******************************* C++ Source File *******************************
 *
 *  Copyright (c) 2012 Eaton Corporation, All Rights Reserved.
 *
 *
 *	$Id: AT24C08.cpp 15175 2012-06-18 06:39:19Z Jingfang Yang $
 *
 *******************************************************************************
 */
#include "Includes.h"
#include "uC_SPI.h"
#include "CAT25128.h"
#include "NV_Address.h"
#include "uC_Watchdog.h"
#include "StdLib_MV.h"
#include "NV_Ctrl_Debug.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define CAT25128_WRITE_BUFFER_MAX               16U

#define CAT25128_WPAGE_ALIGNMENT_MASK           ( m_chip_cfg->write_page_size - 1 )
#define MAX_NUMBER_OF_READS                     2U
#define MAX_NUMBER_OF_WRITES                    2U
#define ONE_BYTE_OF_READS                       1U
#define CAT25128_MAX_OPERATE_TIME               5U
#define MAX_RETRY_TIME                          3U
#define SPI_TIMEOUT_VAL                         200U
#define UPPER_ADDRESS_BIT_SHIFT                 3U

#define WRITE_MEMORY_ADD_DATA_OPCODE            0x02U
#define READ_MEMORY_ADD_DATA_OPCODE             0x03U
#define READ_STATUS_REGISTER_OPCODE             0x05U
#define SET_WRITE_ENABLE_LATCH_OPCODE           0x06U


#define CAT25128_STATUS_RDY_BIT                 0U

#define Delay_Chip_Select_Change()      uC_Delay( 10U )

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
bool_t CAT25128::m_write_lock = true;

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

CAT25128::CAT25128( EEPROM_CHIP_CONFIG_TD const* chip_config, uint16_t buff_size, uC_SPI* spi_ctrl,
					uint8_t chip_select ) :
	m_spi_ctrl( spi_ctrl ),
	m_chip_select( chip_select ),
	m_chip_cfg( chip_config ),
	m_use_os( false ),
	m_retry_times( 0U ),
	m_use_protection( false ),
	m_use_checksum( false ),
	m_deal_length( 0U ),
	m_done_length( 0U ),
	m_length( 0U ),
	m_address( 0U ),
	m_start_time( 0U ),
	m_wpage_buffer( nullptr ),
	m_data_ptr( nullptr )
{
	m_max_buff_size = buff_size + CAT25128_NV_CHECKSUM_LEN;
	m_chip_cfg = chip_config;
	m_spi_ctrl->Configure_Chip_Select( uC_SPI_MODE_3, chip_select, chip_config->max_clock_freq, uC_SPI_BIT_COUNT_8,
									   true );

	m_data_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( m_max_buff_size ) );
	m_wpage_buffer = reinterpret_cast<uint8_t*>( Ram::Allocate( m_chip_cfg->write_page_size ) );
	m_cr_queue = new CR_Queue();
	new CR_Tasker( Eeprom_Handler_Static, ( CR_TASKER_PARAM )this, CR_TASKER_IDLE_PRIORITY, "CAT25128" );

	m_erase_val = CAT25128_DEFAULT_ERASE_VAL;
	m_status = EEPROM_START_TO_WRITE;
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

CAT25128::~CAT25128( void )
{
	Ram::De_Allocate( m_data_buffer );
	Ram::De_Allocate( m_wpage_buffer );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

NV_Ctrl::nv_status_t CAT25128::Read( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									 bool_t use_protection )
{
	nv_status_t return_status = BUSY_ERROR;
	bool_t good_data = false;
	uint32_t read_count;

	if ( Check_Range( address, length,  use_protection ) && ( length != 0 ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( SPI_TIMEOUT_VAL ) )
		{
			read_count = 0U;
			do
			{
				read_count++;
				good_data = Read_Now( data, address, length, true );
			}while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_READS ) );

			// if the data didn't read right, go check the backup location to see if
			// our good data is there.
			if ( ( !good_data ) && ( use_protection == true ) )
			{
				read_count = 0U;
				do
				{
					read_count++;
					good_data = Read_Now( data, ( address + m_chip_cfg->mirror_start_address ),
										  length, true );
				}while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_READS ) );

				if ( good_data == true )
				{
					good_data = Internal_Write( data, address, length, true, false );
				}
			}

			if ( good_data == true )
			{
				return_status = SUCCESS;
			}
			else
			{
				return_status = DATA_ERROR;
				NV_CTRL_PRINT( DBG_MSG_ERROR,
							   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
							   address, length, use_protection, return_status );
			}

			m_spi_ctrl->Release_Semaphore();
		}
		else
		{
			return_status = BUSY_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t CAT25128::Read_Now( uint8_t* data, uint32_t address, uint32_t length, bool_t use_checksum )
{
	bool_t good_data = false;
	uint_fast16_t rx_checksum_low;
	uint_fast16_t rx_checksum_high;
	uint_fast16_t calc_checksum;

	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( READ_MEMORY_ADD_DATA_OPCODE, address );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	calc_checksum = 0U;
	while ( length-- )
	{
		*data = m_spi_ctrl->RX_Byte();
		calc_checksum += *data;
		data++;
	}
	calc_checksum &= 0xFFFF;

	if ( use_checksum == true )
	{
		rx_checksum_low = m_spi_ctrl->RX_Byte();
		rx_checksum_high = m_spi_ctrl->RX_Byte();
		rx_checksum_low |= rx_checksum_high << 8;

		if ( rx_checksum_low == calc_checksum )
		{
			good_data = true;
		}
	}
	else
	{
		good_data = true;
	}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( good_data );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void CAT25128::Send_Data_Command( uint8_t spi_command, uint32_t address )
{
	BF_Lib::SPLIT_UINT32 temp_address;

	temp_address.u32 = address;

	switch ( m_chip_cfg->address_length )
	{
		case 1:
			m_spi_ctrl->TX_Byte( spi_command | ( temp_address.u8[1] << UPPER_ADDRESS_BIT_SHIFT ) );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			m_spi_ctrl->TX_Byte( temp_address.u8[0] );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			break;

		default:
			m_spi_ctrl->TX_Byte( spi_command );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			for ( uint_fast8_t i = m_chip_cfg->address_length; i > 0U; i-- )
			{
				m_spi_ctrl->TX_Byte( temp_address.u8[i - 1] );
				while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
				{}
			}
			break;
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t CAT25128::Write( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									  bool_t use_protection )
{
	nv_status_t return_status = SUCCESS;
	bool_t inter_write_result;

	if ( ( address == NV_CTRL_MEM_INIT_DONE_NVADD ) ||
		 ( address == DCI_NV_MEM_FACTORY_RESET_TRIG_NVADD )	/*|| address >= FAULT_LOG_INFO_END_LOG_ADDR_NVADD*/ ||
		 ( address == MSG_FOR_BOOT_NVADD ) )
	{
		CAT25128::Set_Write_Lock( false );
	}
	if ( CAT25128::Get_Write_Lock() )
	{
		return ( return_status );
	}

	if ( m_spi_ctrl->Get_Semaphore( SPI_TIMEOUT_VAL ) )
	{
		if ( ( m_use_os != false ) && ( data != &m_erase_val ) )
		{
			if ( length > m_max_buff_size )
			{
				return_status = DATA_ERROR;
				NV_CTRL_PRINT( DBG_MSG_ERROR,
							   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
							   address, length, use_protection, return_status );
			}
		}

		if ( return_status == SUCCESS )
		{
			if ( Check_Range( address, length,  use_protection ) )
			{
				inter_write_result = Internal_Write( data, address, length, true, use_protection );

				if ( inter_write_result == false )
				{
					return_status = DATA_ERROR;
					NV_CTRL_PRINT( DBG_MSG_ERROR,
								   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
								   address, length, use_protection, return_status );
				}
				else
				{
					return_status = SUCCESS;
				}
			}
			else
			{
				return_status = INVALID_ADDRESS;
				NV_CTRL_PRINT( DBG_MSG_ERROR,
							   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
							   address, length, use_protection, return_status );
			}
		}

		if ( return_status != SUCCESS )
		{
			m_spi_ctrl->Release_Semaphore();
		}
	}
	else
	{
		return_status = BUSY_ERROR;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t CAT25128::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											  NV_CTRL_LENGTH_TD length )
{
	bool_t good_data;
	nv_status_t nv_status;

	good_data = Read_Now( ( uint8_t* )checksum_dest, address + length, CAT25128_NV_CHECKSUM_LEN, false );

	if ( good_data == true )
	{
		nv_status = SUCCESS;
	}
	else
	{
		nv_status = DATA_ERROR;
	}

	return ( nv_status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void CAT25128::Write_Prep( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, NV_CTRL_LENGTH_TD* output_length )
{
	NV_CTRL_LENGTH_TD num = m_chip_cfg->write_page_size;

	if ( ( address & CAT25128_WPAGE_ALIGNMENT_MASK ) != 0 )
	{
		num = address & CAT25128_WPAGE_ALIGNMENT_MASK;
		num = m_chip_cfg->write_page_size - num;
	}

	if ( length < num )
	{
		num = length;
	}

	*output_length = num;
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t CAT25128::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, bool_t protection )
{
	bool_t range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	if ( protection == true )
	{
		// Usually when we use the checksum, the data length should add the checksum length, but actually for the EERPOM
		// which was used for store parameter, use the protection means has mirror area and checksum.
		length += CAT25128_NV_CHECKSUM_LEN;
		end_address = m_chip_cfg->mirror_start_address;
	}
	else
	{
		end_address = m_chip_cfg->end_address;
	}

	if ( ( address >= m_chip_cfg->start_address ) && ( ( address + length ) <= end_address ) )
	{
		range_good = true;
	}

	return ( range_good );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t CAT25128::Check_CheckSum( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	bool_t good_data = false;
	NV_Ctrl::nv_status_t nv_status = NV_Ctrl::SUCCESS;
	BF_Lib::SPLIT_UINT16 checksum;
	BF_Lib::SPLIT_UINT16 read_checksum;
	uint8_t* pdata = data;
	NV_CTRL_LENGTH_TD len = length;

	checksum.u16 = 0U;
	read_checksum.u16 = 0U;

	while ( len-- )
	{
		checksum.u16 += *pdata;

		if ( pdata != &m_erase_val )
		{
			pdata++;
		}
	}

	nv_status = Read_Checksum( &read_checksum.u16, address, length );

	if ( nv_status == NV_Ctrl::SUCCESS )
	{
		if ( read_checksum.u16 == checksum.u16 )
		{
			good_data = true;
		}
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t CAT25128::Writting( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	uC_Watchdog::Force_Feed_Dog();

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( SET_WRITE_ENABLE_LATCH_OPCODE );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	// An edge has to be sent to the Chip select pin on a write instruction.
	// Nops are there to give at least a 5Mhz pulse width.
	Delay_Chip_Select_Change();	// if we can decrease this time, then writting eeprom can be quicker,maybe 1us instead
								// of 1ms

	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( WRITE_MEMORY_ADD_DATA_OPCODE, address );

	for ( uint32_t i = 0U; i < length; i++ )
	{
		m_spi_ctrl->TX_Byte( *data );
		data++;
		while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
		{}
	}
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( true );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t CAT25128::Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
							 bool_t use_checksum )
{
	bool_t good_data = true;
	uint8_t read_data;
	BF_Lib::SPLIT_UINT16 checksum;
	uint8_t* pdata = check_data;
	NV_CTRL_LENGTH_TD len = length;
	NV_CTRL_ADDRESS_TD addr = address;

	checksum.u16 = 0U;
	while ( len-- )
	{
		good_data = Read_Now( &read_data, addr, ONE_BYTE_OF_READS, false );

		checksum.u16 = read_data + checksum.u16;

		if ( *pdata != read_data )
		{
			good_data = false;
		}

		if ( good_data == false )
		{
			return ( good_data );
		}

		addr++;

		if ( pdata != &m_erase_val )
		{
			pdata++;
		}
	}

	if ( ( use_checksum == true ) && ( good_data == true ) )
	{
		good_data = Check_CheckSum( check_data, address, length );
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void CAT25128::Eeprom_Handler( CR_Tasker* cr_task )
{
	static RESULT_STATUS_ST result_pair;
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_task );

	while ( true )
	{
		CR_Queue_Receive( cr_task, m_cr_queue, NULL, OS_TASKER_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			/**
			 * Wait for SPI ready
			 */
			if ( CAT25128::Get_Write_Lock() )
			{
				m_spi_ctrl->Release_Semaphore();
				continue;
			}
			while ( Is_Ready() != true )
			{
				CR_Tasker_Yield( cr_task );
			}

			m_spi_ctrl->Select_Chip( m_chip_select );
			m_retry_times = 0U;

			do
			{
				do
				{
					result_pair = Write_Now( m_data_ptr, m_address, m_length );
					if ( result_pair.status == EEPROM_WRITTING_PAGE )
					{
						CR_Tasker_Yield( cr_task );
					}
				}while( result_pair.status != EEPROM_WRITE_FINISH );
				m_retry_times++;
			}while( ( result_pair.result == false ) && ( m_retry_times < MAX_RETRY_TIME ) );

			if ( ( result_pair.result != false ) && ( m_use_protection != false ) )
			{
				do
				{
					do
					{
						result_pair =
							Write_Now( m_data_ptr,
									   ( m_address - m_chip_cfg->start_address + m_chip_cfg->mirror_start_address ),
									   m_length );
						if ( result_pair.status == EEPROM_WRITTING_PAGE )
						{
							CR_Tasker_Yield( cr_task );
						}
					}while( result_pair.status != EEPROM_WRITE_FINISH );
					m_retry_times++;
				}while( ( result_pair.result == false ) && ( m_retry_times < MAX_RETRY_TIME ) );
			}

			m_spi_ctrl->Release_Semaphore();
		}
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t CAT25128::Internal_Write( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
								 bool_t use_checksum, bool_t use_protection )
{
	bool_t result;
	RESULT_STATUS_ST result_pair;
	uint8_t retry_times = 10U;

	if ( CAT25128::Get_Write_Lock() )
	{
		return ( SUCCESS );
	}

	if ( data == &m_erase_val )
	{
		m_data_ptr = data;
	}
	else
	{
		BF_Lib::Copy_String( m_data_buffer, data, length );
		m_data_ptr = m_data_buffer;
	}

	m_address = address;
	m_length = length;
	m_done_length = 0U;
	m_use_checksum = use_checksum;
	m_use_protection = use_protection;

	if ( m_use_checksum == true )
	{
		NV_CTRL_LENGTH_TD data_index;
		BF_Lib::SPLIT_UINT16 checksum;
		checksum.u16 = 0U;

		for ( data_index = 0U; data_index < m_length; data_index++ )
		{
			checksum.u16 += m_data_buffer[data_index];
		}

		m_data_buffer[data_index] = checksum.u8[0];
		m_data_buffer[++data_index] = checksum.u8[1];
		m_length += CAT25128_NV_CHECKSUM_LEN;
	}

	if ( m_use_os != false )
	{
		m_cr_queue->Post_From_ISR( false );
		result = true;
	}
	else
	{
		while ( !Is_Ready() && ( --retry_times ) > 0 )
		{}
		if ( retry_times == 0 )
		{
			return ( false );
		}
		m_spi_ctrl->Select_Chip( m_chip_select );
		m_retry_times = 0U;

		do
		{
			do
			{
				result_pair = Write_Now( m_data_ptr, m_address, m_length );
			}while( result_pair.status != EEPROM_WRITE_FINISH );
			m_retry_times++;
		}while( ( result_pair.result == false ) && ( m_retry_times < MAX_RETRY_TIME ) );

		if ( ( result_pair.result != false ) && ( m_use_protection != false ) )
		{
			m_retry_times = 0U;
			do
			{
				do
				{
					result_pair =
						Write_Now( m_data_ptr,
								   ( m_address - m_chip_cfg->start_address + m_chip_cfg->mirror_start_address ),
								   m_length );
				}while( result_pair.status != EEPROM_WRITE_FINISH );
				m_retry_times++;
			}while( ( result_pair.result == false ) && ( m_retry_times < MAX_RETRY_TIME ) );
		}

		result = result_pair.result;
		m_spi_ctrl->Release_Semaphore();
	}

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
RESULT_STATUS_ST CAT25128::Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	uint32_t count = 0U;
	bool_t result = false;
	RESULT_STATUS_ST return_pair;

	switch ( m_status )
	{
		case EEPROM_START_TO_WRITE:
			Write_Prep( ( address + m_done_length ), ( length - m_done_length ), &m_deal_length );

			if ( data != &m_erase_val )
			{
				for ( NV_CTRL_LENGTH_TD i = 0U; i < m_deal_length; i++ )
				{
					m_wpage_buffer[i] = ( data + m_done_length )[i];
				}
			}
			else
			{
				for ( NV_CTRL_LENGTH_TD i = 0U; i < m_deal_length; i++ )
				{
					m_wpage_buffer[i] = m_erase_val;
				}
			}

			result = Writting( m_wpage_buffer, ( address + m_done_length ), m_deal_length );
			if ( result != false )
			{
				if ( m_use_os != false )
				{
					m_start_time = BF_Lib::Timers::Get_Time();
				}
				m_status = EEPROM_WRITTING_PAGE;
			}
			else
			{
				m_status = EEPROM_WRITE_FINISH;
			}
			break;

		case EEPROM_WRITTING_PAGE:
			if ( m_use_os == false )
			{
				count = CONVERT_MS_TO_COUNT( CAT25128_MAX_OPERATE_TIME );
				do
				{
					count--;
				}while( ( Is_Writting() == true ) && ( count != 0 ) );

				if ( Is_Writting() == false )
				{
					m_status = EEPROM_WRITE_PAGE_FINISH;
					result = true;
				}
				else
				{
					m_status = EEPROM_WRITE_FINISH;
					result = false;
				}
			}
			else
			{
				if ( Is_Writting() == false )
				{
					m_status = EEPROM_WRITE_PAGE_FINISH;
					result = true;
				}
				else if ( BF_Lib::Timers::Expired( m_start_time, CAT25128_MAX_OPERATE_TIME ) )
				{
					m_status = EEPROM_WRITE_FINISH;
					result = false;
				}
			}
			break;

		case EEPROM_WRITE_PAGE_FINISH:
			m_done_length += m_deal_length;
			if ( m_done_length == length )
			{
				result = true;
				m_status = EEPROM_CHECK_DATA;
			}
			else
			{
				result = true;
				m_status = EEPROM_START_TO_WRITE;
			}
			break;

		case EEPROM_CHECK_DATA:
			if ( m_use_checksum != false )
			{
				m_deal_length = length - CAT25128_NV_CHECKSUM_LEN;
			}
			else
			{
				m_deal_length = length;
			}
			result = Check_Data( data, address, m_deal_length, m_use_checksum );
			m_status = EEPROM_WRITE_FINISH;
			break;

		case EEPROM_WRITE_FINISH:
			break;
	}

	return_pair.result = result;
	return_pair.status = m_status;

	if ( m_status == EEPROM_WRITE_FINISH )
	{
		m_status = EEPROM_START_TO_WRITE;
		m_done_length = 0U;
	}

	return ( return_pair );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t CAT25128::Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, uint8_t erase_data,
									  bool_t protected_data )
{
	m_erase_val = erase_data;
	NV_Ctrl::nv_status_t status;

	if ( CAT25128::Get_Write_Lock() )
	{
		CAT25128::Set_Write_Lock( false );
	}
	status = Write( &m_erase_val, address, length, protected_data );
	if ( !CAT25128::Get_Write_Lock() )
	{
		CAT25128::Set_Write_Lock( true );
	}

	if ( SUCCESS != status )
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to Erase from Address: %X, Length: %u, ProtectedData: %d, ErrorStatus: %d",
					   address, length, protected_data, status );
	}

	return ( status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t CAT25128::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ), 0, false ) );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t CAT25128::Is_Ready( void )
{
	bool_t result;
	uint8_t status = 0U;

	Push_TGINT();

	Read_Status( &status );

	if ( !Test_Bit( status, CAT25128_STATUS_RDY_BIT ) )
	{
		result = true;
	}
	else
	{
		result = false;
	}
	Pop_TGINT();

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t CAT25128::Is_Writting( void )
{
	bool_t result;
	uint8_t status = 0U;

	Push_TGINT();

	Read_Status( &status );

	if ( !Test_Bit( status, CAT25128_STATUS_RDY_BIT ) )
	{
		result = false;
	}
	else
	{
		result = true;
	}
	Pop_TGINT();

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t CAT25128::Read_Status( uint8_t* status )
{
	bool_t success = false;

	if ( m_spi_ctrl->Get_Semaphore( SPI_TIMEOUT_VAL ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( READ_STATUS_REGISTER_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		*status = m_spi_ctrl->RX_Byte();

		success = true;

		m_spi_ctrl->De_Select_Chip( m_chip_select );
		m_spi_ctrl->Release_Semaphore();
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CAT25128::Set_Write_Lock( bool_t write_lock )
{
	m_write_lock = write_lock;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t CAT25128::Get_Write_Lock()
{
	return ( m_write_lock );
}

}
