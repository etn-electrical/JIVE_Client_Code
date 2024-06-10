/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "AT25DF_Block.h"
#include "Exception.h"
#include "StdLib_MV.h"
#include "Ram.h"
#include "NV_Ctrl_Debug.h"
#include "string.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

// #define AT25DF_STATUS_BLOCK_PROTECT_0_TO_7FF		0x0CU
// #define AT25DF_STATUS_BLOCK_PROTECT_400_TO_7FF	0x08U
// #define AT25DF_STATUS_BLOCK_PROTECT_600_TO_7FF	0x04U
// #define AT25DF_STATUS_BLOCK_PROTECT_600_TO_7FF	0x00U

// The number of bits that the upper bits need to be shifted to get it into the
// right space in the write opcode.  This is only for the single byte address type.
#define Delay_Chip_Select_Change()      uC_Delay( 1U )

#define AT25DF_EPAGE_ALIGNMENT_MASK         ~( m_chip_cfg->erase_page_size - 1U )
#define AT25DF_WPAGE_ALIGNMENT_MASK         ~( m_chip_cfg->write_page_size - 1U )
#define AT25DF_SCRATCH_EPAGE_ADDRESS        ( m_chip_cfg->end_address - \
											  m_chip_cfg->erase_page_size )

// This is define to calculate the wait time in the msec.
#define MICROSECONDS_ADJUSTMENT         1000U
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
NV_Ctrl::semaphore_status_t AT25DF_Block::m_semaphore = NV_Ctrl::RELEASE;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
AT25DF_Block::AT25DF_Block( uC_SPI* spi_ctrl, uint8_t chip_select,
							AT25DF_BLOCK_CHIP_CONFIG_TD const* chip_config ) :
	NV_Ctrl(),
	m_spi_ctrl( spi_ctrl ),
	m_chip_select( chip_select ),
	m_chip_cfg( chip_config ),
	m_wpage_buff( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_start_buff( reinterpret_cast<BUFF_STRUCT_TD*>( nullptr ) ),
	m_checksum( 0U ),
	m_erase_val( 0U ),
	m_wait_time( 0U ),
	m_state( IDLE ),
	m_status( SUCCESS )
{
	uint8_t status = 0U;

	m_spi_ctrl->Configure_Chip_Select( static_cast<uint8_t>( uC_SPI_MODE_0 ), chip_select,
									   chip_config->max_clock_freq,
									   static_cast<uint8_t>( uC_SPI_BIT_COUNT_8 ), true );

	m_wpage_buff = reinterpret_cast<uint8_t*>( Ram::Allocate( chip_config->write_page_size ) );

	Read_Status( &status );

	if ( status != 0U )
	{
		status = 0U;
		Write_Status( status );
	}
	memset( &m_asynch_data, 0, sizeof( m_asynch_data ) );
	new CR_Tasker( &Serial_Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_IDLE_PRIORITY, "Serial_Flash_Routine" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
AT25DF_Block::~AT25DF_Block( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Read( uint8_t* data, uint32_t address, uint32_t length,
										 cback_func_t cback, cback_param_t param )
{
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, true ) && ( length != 0U ) )
		{
			// NV_CTRL_PRINT( DBG_MSG_INFO, "R: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_wait_time = Read_Time( const_cast<uint8_t*>( data ), address, length );
			m_state = READ;
			m_status = NV_Ctrl::PENDING_CALL_BACK;
		}
		else
		{
			m_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		m_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Write( const uint8_t* data, uint32_t address, uint32_t length,
										  cback_func_t cback, cback_param_t param )
{
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, true ) && ( length != 0U ) )
		{
			// NV_CTRL_PRINT( DBG_MSG_INFO, "W: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			bool include_erase_time = false;
			m_wait_time = Write_Time( const_cast<uint8_t*>( data ), address, length, include_erase_time );
			m_state = WRITE;
			m_status = NV_Ctrl::PENDING_CALL_BACK;
		}
		else
		{
			m_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		m_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Erase( uint32_t address, uint32_t length,
										  cback_func_t cback, cback_param_t param )
{
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, true ) && ( length != 0U ) )
		{
			// NV_CTRL_PRINT( DBG_MSG_INFO, "E: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = nullptr;
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_state = ERASE;
			m_wait_time = Erase_Time( address, length );
			m_status = NV_Ctrl::PENDING_CALL_BACK;
		}
		else
		{
			m_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		m_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Erase_All( cback_func_t cback, cback_param_t param )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					cback, param ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
										 bool use_protection )
{
	bool good_data;
	uint_fast8_t read_count;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, use_protection ) && ( length != 0U ) )
		{
			read_count = 0U;
			// NV_CTRL_PRINT( DBG_MSG_INFO, "r: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			do
			{
				read_count++;
				good_data = Read_Now( data, address, length, use_protection );
			} while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_READS ) );
			m_semaphore = NV_Ctrl::RELEASE;

			if ( good_data == true )
			{
				m_status = NV_Ctrl::SUCCESS;
			}
			else
			{
				m_status = NV_Ctrl::DATA_ERROR;
				// NV_CTRL_PRINT( DBG_MSG_ERROR,
				// "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
				// address, length, use_protection, return_status );
			}

		}
		else
		{
			m_status = NV_Ctrl::INVALID_ADDRESS;
			// NV_CTRL_PRINT( DBG_MSG_ERROR,
			// "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
			// address, length, use_protection, return_status );
		}
	}
	else
	{
		m_status = NV_Ctrl::BUSY_ERROR;
	}
	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF_Block::Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( AT25DF_READ_MEMORY_ADD_DATA_OPCODE, address );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->Set_XChange_DMA( data, data, static_cast<uint16_t>( length ) );
		m_spi_ctrl->Enable_XChange_DMA();

		while ( m_spi_ctrl->XChange_DMA_Busy() )
		{}		//! m_spi_ctrl->TX_Shift_Empty() ){};

		m_spi_ctrl->Disable_XChange_DMA();

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
NV_Ctrl::nv_status_t AT25DF_Block::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
												  NV_CTRL_LENGTH_TD length )
{
	return ( Read( reinterpret_cast<uint8_t*>( checksum_dest ), address + length,
				   AT25DF_NV_CHECKSUM_LEN, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
										  bool use_protection )
{
	bool good_data;
	uint8_t num_block = 0;	// number of blocks of each 256 bytes to be written
	uint8_t offset = 0;	// number of bytes remaining for writing
	uint16_t i = 0;

	num_block = length / ( AT25DF_MAX_PROGRAM_BYTE );
	offset = length % ( AT25DF_MAX_PROGRAM_BYTE );
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, use_protection ) )
		{
			bool_t erased_area = true;
			uint32_t counter = 0U;
			m_semaphore = NV_Ctrl::ACQUIRE;
			for ( counter = 0U; ( ( counter < num_block ) && ( true == erased_area ) ); counter++ )
			{
				uC_Watchdog::Force_Feed_Dog();
				Quick_Read( m_wpage_buff, ( address + ( counter * AT25DF_MAX_PROGRAM_BYTE ) ),
							AT25DF_MAX_PROGRAM_BYTE );
				for ( i = 0; i < AT25DF_MAX_PROGRAM_BYTE; i++ )
				{
					if ( m_wpage_buff[i] != 0xFF )
					{
						erased_area = false;
					}
				}
			}
			if ( ( offset != 0U ) && ( erased_area == true ) )
			{
				/* last block has been checked && till now the area is erased
				   Check the remaining byte of memory*/
				if ( counter == num_block )
				{
					Quick_Read( m_wpage_buff, ( address + ( ( num_block ) * AT25DF_MAX_PROGRAM_BYTE ) ), offset );
					for ( i = 0; i < offset; i++ )
					{
						if ( m_wpage_buff[i] != 0xFF )
						{
							erased_area = false;
						}
					}
				}
			}

			/* Once all the area that was to be written is erased then only proceed with write */
			if ( true == erased_area )
			{
				good_data = true;
				for ( counter = 0U; ( ( counter < num_block ) && ( good_data == true ) ); counter++ )
				{
					uC_Watchdog::Force_Feed_Dog();
					good_data = Write_Now( data, address + ( counter * AT25DF_MAX_PROGRAM_BYTE ),
										   AT25DF_MAX_PROGRAM_BYTE );
					data = data + AT25DF_MAX_PROGRAM_BYTE;
				}
				if ( ( offset != 0U ) && ( good_data == true ) )/* Write the remaining bytes */
				{
					if ( counter == num_block )
					{
						good_data = Write_Now( data, address + ( num_block * AT25DF_MAX_PROGRAM_BYTE ), offset );
					}
				}

				m_semaphore = NV_Ctrl::RELEASE;
				if ( good_data == true )
				{
					m_status = NV_Ctrl::SUCCESS;
				}
				else
				{
					m_status = NV_Ctrl::DATA_ERROR;
					// NV_CTRL_PRINT( DBG_MSG_ERROR,
					// "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					// address, length, use_protection, return_status );
				}
			}
		}
		else
		{
			m_status = NV_Ctrl::INVALID_ADDRESS;
			// NV_CTRL_PRINT( DBG_MSG_ERROR,
			// "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
			// address, length, use_protection, return_status );
		}
	}
	else
	{
		m_status = NV_Ctrl::BUSY_ERROR;
	}
	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF_Block::Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	bool_t status = true;
	uint8_t read_status = 0U;

	if ( m_spi_ctrl->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( AT25DF_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.
		Delay_Chip_Select_Change()

		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( AT25DF_WRITE_MEMORY_ADD_DATA_OPCODE, address );

		m_spi_ctrl->Set_TX_DMA( data, static_cast<uint16_t>( length ) );
		m_spi_ctrl->Enable_TX_DMA();

		while ( m_spi_ctrl->TX_DMA_Busy() )
		{}
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->Disable_TX_DMA();

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		m_spi_ctrl->Release_Semaphore();
	}
	else
	{
		status = false;
	}

	do
	{
		Read_Status( &read_status );
	} while ( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) );

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Erase( uint32_t address, NV_CTRL_LENGTH_TD length,
										  uint8_t erase_data,
										  bool protected_data )
{
	bool erase_status = false;

	m_erase_val = erase_data;
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		NV_CTRL_PRINT( DBG_MSG_INFO, "e: %x	%d", address, length );
		m_semaphore = NV_Ctrl::ACQUIRE;
		m_synchronous_erase = true;

		// Check the start address is at the beginning of sector or not.
		if ( ( address % BLOCK_4K == 0 ) || ( length % BLOCK_4K == 0 ) )
		{
			while ( ( length >= BLOCK_4K ) && ( m_status == NV_Ctrl::SUCCESS ) )
			{
				if ( ( length >= BLOCK_64K ) && ( Is_Aligned( address, BLOCK_64K ) ) )
				{
					erase_status = Erase_Pages( address, ERASE_BLOCK_64K );
					address += BLOCK_64K;
					length -= BLOCK_64K;

					if ( erase_status != true )
					{
						m_status = NV_Ctrl::BUSY_ERROR;
						break;
					}
				}
				else if ( ( length >= BLOCK_32K ) && ( Is_Aligned( address, BLOCK_32K ) ) )
				{
					erase_status = Erase_Pages( address, ERASE_BLOCK_32K );
					address += BLOCK_32K;
					length -= BLOCK_32K;

					if ( erase_status != true )
					{
						m_status = NV_Ctrl::BUSY_ERROR;
						break;
					}
				}
				else if ( ( length >= BLOCK_4K ) && ( Is_Aligned( address, BLOCK_4K ) ) )
				{
					erase_status = Erase_Pages( address, ERASE_BLOCK_4K );
					address += BLOCK_4K;
					length -= BLOCK_4K;

					if ( erase_status != true )
					{
						m_status = NV_Ctrl::BUSY_ERROR;
						break;
					}
				}
			}
		}
		else
		{
			/* For Fast write the address and length must be aligned to 4 KB*/
			m_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		m_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF_Block::Erase_Pages( uint32_t address, uint8_t erase_cmd )
{
	bool_t ret_status = true;
	uint8_t read_status = 0U;
	uint16_t max_time = 0U;
	TIMERS_TIME_TD begin_time = 0U;

	Read_Status( &read_status );
	if ( !( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
		{
			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->TX_Byte( AT25DF_SET_WRITE_ENABLE_LATCH_OPCODE );
			while ( !m_spi_ctrl->TX_Shift_Empty() )
			{}

			m_spi_ctrl->De_Select_Chip( m_chip_select );

			// An edge has to be sent to the Chip select pin on a write instruction.
			// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
			Delay_Chip_Select_Change()

			if ( ( erase_cmd != ERASE_BLOCK_4K ) && ( erase_cmd != ERASE_BLOCK_32K ) &&
				 ( erase_cmd != ERASE_BLOCK_64K ) )
			{
				ret_status = false;
			}

			if ( ret_status == true )
			{
				if ( erase_cmd == ERASE_BLOCK_4K )
				{
					max_time = AT25DF_ERASE_4K_MAX_TIME;
					address = Aligne_Address( address, BLOCK_4K );
				}
				else if ( erase_cmd == ERASE_BLOCK_32K )
				{
					max_time = AT25DF_ERASE_32K_MAX_TIME;
					address = Aligne_Address( address, BLOCK_32K );
				}
				else
				{
					max_time = AT25DF_ERASE_64K_MAX_TIME;
					address = Aligne_Address( address, BLOCK_64K );
				}
				m_spi_ctrl->Select_Chip( m_chip_select );
				Send_Data_Command( erase_cmd, address );
				m_spi_ctrl->De_Select_Chip( m_chip_select );
			}

			// An erase should happen.  We should wait.


			if ( true == m_synchronous_erase )
			{

				begin_time = BF_Lib::Timers::Get_Time();

				while ( true )
				{
					uC_Delay( 50 );

					if ( BF_Lib::Timers::Expired( begin_time, max_time ) )
					{
						ret_status = false;
						break;
					}

					Read_Status( &read_status );
					if ( !BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) )
					{
						if ( !BF_Lib::Bit::Test( read_status, AT25DF_STATUS_ERROR_BIT ) )
						{
							ret_status = true;
							break;
						}
						else
						{
							ret_status = false;
							break;
						}
					}
					uC_Watchdog::Force_Feed_Dog();
				}

			}
			m_spi_ctrl->Release_Semaphore();
		}
		else
		{
			ret_status = false;
		}
	}
	else
	{
		ret_status = false;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF_Block::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					AT25DF_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF_Block::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
							 bool use_protection )
{
	bool good_data = false;

	BF_Lib::SPLIT_UINT16 checksum;

	if ( m_spi_ctrl->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( AT25DF_READ_MEMORY_ADD_DATA_OPCODE, address );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		checksum.u16 = 0U;
		while ( length > 0U )
		{
			*data = m_spi_ctrl->RX_Byte();
			checksum.u16 += *data;
			data++;
			length--;
		}

		if ( use_protection == true )
		{
			good_data = true;
		}
		else
		{
			good_data = true;
		}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		m_spi_ctrl->Release_Semaphore();
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::Send_Data_Command( uint8_t command, uint32_t address )
{
	BF_Lib::SPLIT_UINT32 temp_address;

	temp_address.u32 = address;

	switch ( m_chip_cfg->address_length )
	{
		case 1:
			m_spi_ctrl->TX_Byte(
				command
				| ( static_cast<uint8_t>
					( temp_address.u8[1] << UPPER_ADDRESS_BIT_SHIFT ) ) );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			m_spi_ctrl->TX_Byte( temp_address.u8[0] );
			while ( !m_spi_ctrl->TX_Shift_Empty() )
			{}
			break;

		default:
			m_spi_ctrl->TX_Byte( command );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			for ( uint_fast8_t i = m_chip_cfg->address_length; i > 0U; i-- )
			{
				m_spi_ctrl->TX_Byte( temp_address.u8[i - 1U] );
				while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
				{}
			}
			while ( !m_spi_ctrl->TX_Shift_Empty() )
			{}
			break;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF_Block::Read_Status( uint8_t* status )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( AT25DF_READ_STATUS_REGISTER_OPCODE );
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
bool AT25DF_Block::Read_Device_ID( uint8_t* device_id_dest )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( AT25DF_READ_DEVICE_ID_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		device_id_dest[0] = m_spi_ctrl->RX_Byte();
		device_id_dest[1] = m_spi_ctrl->RX_Byte();
		device_id_dest[2] = m_spi_ctrl->RX_Byte();
		device_id_dest[3] = m_spi_ctrl->RX_Byte();

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
bool AT25DF_Block::Write_Status( uint8_t status )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( AT25DF_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
		Delay_Chip_Select_Change()

		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( AT25DF_WRITE_STATUS_REGISTER_OPCODE );
		while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
		{}
		m_spi_ctrl->TX_Byte( status );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

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
bool AT25DF_Block::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
								bool protection ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	if ( protection == true )
	{
		end_address = m_chip_cfg->mirror_start_address;
	}
	else
	{
		end_address = m_chip_cfg->end_address;
	}

	if ( ( address >= m_chip_cfg->start_address ) && ( ( address + length ) <= ( end_address + 1U ) ) )
	{
		range_good = true;
	}

	return ( range_good );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t AT25DF_Block::Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								  NV_CTRL_LENGTH_TD length ) const
{
	// Todo: Implementation pending
	uint32_t read_time = 0;
	uint32_t byte_read_time;

	byte_read_time = BYTE_READ_TIME_US * length;

	if ( byte_read_time < MICROSECONDS_ADJUSTMENT )
	{
		byte_read_time = MICROSECONDS_ADJUSTMENT;
	}
	read_time += Round_Div( byte_read_time, MICROSECONDS_ADJUSTMENT );	// Because the byte
	// read
	// time is in
	// microseconds we
	// have
	// to adjust it
	// down.
	return ( read_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t AT25DF_Block::Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool include_erase_time ) const
{
	uint32_t write_time = 0U;
	uint32_t byte_write_time;

	if ( data != NULL )
	{
		byte_write_time = BYTE_WRITE_TIME_US * length;
		if ( byte_write_time < MICROSECONDS_ADJUSTMENT )
		{
			byte_write_time = MICROSECONDS_ADJUSTMENT;
		}
		write_time += Round_Div( byte_write_time, MICROSECONDS_ADJUSTMENT );// Because the byte
																			// write
																			// time is in
																			// microseconds we
																			// have
																			// to adjust it
																			// down.
	}
	return ( write_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t AT25DF_Block::Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	uint32_t wait_time = 0U;
	uint32_t block_count = 0U;

	if ( ( address % BLOCK_4K == 0 ) || ( length % BLOCK_4K == 0 ) )
	{
		if ( length >= BLOCK_64K )
		{
			block_count = ( length / BLOCK_64K );

			wait_time += ( ( block_count * BLOCK_64K_ERASE_TIME_MS ) );
			length = ( length % BLOCK_64K );
		}
		if ( length >= BLOCK_32K )
		{
			block_count = ( length / BLOCK_32K );
			wait_time += ( ( block_count * BLOCK_32K_ERASE_TIME_MS ) );
			length = ( length % BLOCK_32K );
		}
		if ( length >= BLOCK_4K )
		{
			block_count = ( length / BLOCK_4K );
			wait_time += ( ( block_count * BLOCK_4K_ERASE_TIME_MS ) );
			length = ( length % BLOCK_4K );
		}
	}
	return ( wait_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::Serial_Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{

	( ( AT25DF_Block* )param )->Serial_Flash_Async_Task( cr_task );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::Serial_Flash_Async_Task( CR_Tasker* cr_task )
{
	switch ( m_state )
	{
		case IDLE:
			break;

		case ERASE:
			asynch_erase_handler();
			break;

		case READ:
			asynch_read_handler();
			break;

		case WRITE:
			asynch_write_handler();
			break;

		case COMPLETE:
			m_asynch_data.cback( m_asynch_data.param, m_status );
			m_state = IDLE;
			break;

		case ERROR:
			m_asynch_data.cback( m_asynch_data.param, m_status );
			m_state = IDLE;
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_asynch_data.cback = func_callback;
	m_asynch_data.param = func_param;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::Get_Memory_Info( mem_range_info_t* mem_range_info )
{
	mem_range_info->start_address = m_chip_cfg->start_address;
	mem_range_info->mirror_start_address = m_chip_cfg->mirror_start_address;
	mem_range_info->end_address = m_chip_cfg->end_address;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::asynch_erase_handler( void )
{
	/* Acquired semaphore for complete erase operation */
	bool erase_status = false;
	uint8_t read_status = 0U;

	Read_Status( &read_status );

	if ( !( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) ) )
	{
		NV_CTRL_PRINT( DBG_MSG_INFO, "Erase: %x	%d", m_asynch_data.address, m_asynch_data.length );
		if ( ( m_asynch_data.length >= BLOCK_64K ) && ( Is_Aligned( m_asynch_data.address, BLOCK_64K ) ) )
		{
			erase_status = Erase_Pages( m_asynch_data.address, ERASE_BLOCK_64K );
			m_asynch_data.address += BLOCK_64K;
			m_asynch_data.length -= BLOCK_64K;

			if ( erase_status != true )
			{
				m_status = NV_Ctrl::BUSY_ERROR;
				m_state = ERROR;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}
		else if ( ( m_asynch_data.length >= BLOCK_32K ) && ( Is_Aligned( m_asynch_data.address, BLOCK_32K ) ) )
		{
			erase_status = Erase_Pages( m_asynch_data.address, ERASE_BLOCK_32K );
			m_asynch_data.address += BLOCK_32K;
			m_asynch_data.length -= BLOCK_32K;

			if ( erase_status != true )
			{
				m_status = NV_Ctrl::BUSY_ERROR;
				m_state = ERROR;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}
		else if ( ( m_asynch_data.length >= BLOCK_4K ) && ( Is_Aligned( m_asynch_data.address, BLOCK_4K ) ) )
		{
			erase_status = Erase_Pages( m_asynch_data.address, ERASE_BLOCK_4K );
			m_asynch_data.address += BLOCK_4K;
			m_asynch_data.length -= BLOCK_4K;

			if ( erase_status != true )
			{
				m_status = NV_Ctrl::BUSY_ERROR;
				m_state = ERROR;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}
		if ( m_asynch_data.length < BLOCK_4K )
		{
			m_state = COMPLETE;
			m_status = SUCCESS;
			m_semaphore = NV_Ctrl::RELEASE;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::asynch_write_handler( void )
{
	NV_CTRL_LENGTH_TD length;
	uint8_t read_status = 0U;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		Read_Status( &read_status );
		if ( !( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) ) )
		{
			/* Acquire semaphore only for WRITE_CHUCK_SIZE transfer */
			if ( m_asynch_data.length > AT25DF_MAX_PROGRAM_BYTE )
			{
				length = AT25DF_MAX_PROGRAM_BYTE;
			}
			else
			{
				length = m_asynch_data.length;
			}
			uC_Watchdog::Force_Feed_Dog();

			if ( true == Write_Now( m_asynch_data.data, m_asynch_data.address, AT25DF_MAX_PROGRAM_BYTE ) )
			{
				m_asynch_data.data += length;
				m_asynch_data.address += length;
				m_asynch_data.length -= length;
				if ( m_asynch_data.length == 0 )
				{
					m_state = COMPLETE;
					m_status = SUCCESS;
					/* Release flash control after WRITE_CHUCK_SIZE transfer*/
					m_semaphore = NV_Ctrl::RELEASE;
				}
			}
			else
			{
				m_state = ERROR;
				m_status = DATA_ERROR;
				m_semaphore = NV_Ctrl::RELEASE;
				NV_CTRL_PRINT( DBG_MSG_ERROR, "Write Data Error" );
			}
		}
	}
	else
	{
		// misra
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF_Block::asynch_read_handler( void )
{
	NV_CTRL_LENGTH_TD length;
	uint8_t read_status = 0U;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		Read_Status( &read_status );
		if ( !( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) ) )
		{
			/* Acquire semaphore only for READ_CHUCK_SIZE transfer */
			if ( m_asynch_data.length > AT25DF_MAX_PROGRAM_BYTE )
			{
				length = AT25DF_MAX_PROGRAM_BYTE;
			}
			else
			{
				length = m_asynch_data.length;
			}

			if ( true == Read_Now( m_asynch_data.data,  m_asynch_data.address, length, true ) )
			{
				m_asynch_data.data += length;
				m_asynch_data.address += length;
				m_asynch_data.length -= length;

				if ( m_asynch_data.length == 0 )
				{
					m_state = COMPLETE;
					m_status = SUCCESS;
					/* Release flash control after READ_CHUCK_SIZE transfer*/
					m_semaphore = NV_Ctrl::RELEASE;
				}
			}
			else
			{
				m_state = ERROR;
				m_status = DATA_ERROR;
				m_semaphore = NV_Ctrl::RELEASE;
				NV_CTRL_PRINT( DBG_MSG_ERROR, "Read Data Error" );
			}
		}
	}
	else
	{
		// misra
	}
}

}
