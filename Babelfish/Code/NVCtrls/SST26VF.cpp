/**
 *****************************************************************************************
 *	@file 		SST26VF.cpp
 *	@details 	See header file for module overview.
 *	@copyright 	2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "SST26VF.h"
#include "Exception.h"
#include "StdLib_MV.h"
#include "Ram.h"
#include "NV_Ctrl_Debug.h"
#include "string.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

// The number of bits that the upper bits need to be shifted to get it into the
// right space in the write opcode.  This is only for the single byte address type.
#define Delay_Chip_Select_Change()      uC_Delay( 1U )

#define SST26VF_EPAGE_ALIGNMENT_MASK         ~( m_chip_cfg->erase_page_size - 1U )
#define SST26VF_WPAGE_ALIGNMENT_MASK         ~( m_chip_cfg->write_page_size - 1U )
#define SST26VF_SCRATCH_EPAGE_ADDRESS        ( m_chip_cfg->end_address - \
											   m_chip_cfg->erase_page_size )
// This is define to calculate the wait time in the msec.
#define MICROSECONDS_ADJUSTMENT         1000U
#define ERASE_PAGE_DELAY_US             50U
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
NV_Ctrl::semaphore_status_t SST26VF::m_semaphore = NV_Ctrl::RELEASE;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
SST26VF::SST26VF( uC_SPI* spi_ctrl, uint8_t chip_select,
				  SST26VF_CHIP_CONFIG_TD const* chip_config,
				  bool_t synchronus_erase, cback_func_t cback_func,
				  cback_param_t param, bool_t fast_write ) :
	NV_Ctrl(),
	m_spi_ctrl( spi_ctrl ),
	m_chip_select( chip_select ),
	m_chip_cfg( chip_config ),
	m_wpage_buff( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_erase_val( 0U ),
	m_synchronous_erase( synchronus_erase ),
	m_wait_time( 0U ),
	m_state( IDLE ),
	m_status( SUCCESS ),
	m_call_back_ptr( cback_func ),
	m_cback_param( param ),
	m_fast_write( fast_write ),
	m_start_buff( reinterpret_cast<BUFF_STRUCT_TD*>( nullptr ) ),
	m_checksum( 0U ),
	m_serial_flash_busy( false ),
	m_src_buff{ nullptr, nullptr, 0U },
	m_checksum_buff{ nullptr, nullptr, 0U },
	m_begin_rec_buff{ nullptr, nullptr, 0U },
	m_end_rec_buff{ nullptr, nullptr, 0U }
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

	m_spi_ctrl->Select_Chip( m_chip_select );
	m_spi_ctrl->TX_Byte( SST26VF_SET_WRITE_ENABLE_LATCH_OPCODE );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{
		uC_Watchdog::Force_Feed_Dog();

	}
	m_spi_ctrl->De_Select_Chip( m_chip_select );

	/* Clear global block protection. */
	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( SST26VF_ULBPR_OPCODE );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	m_spi_ctrl->De_Select_Chip( m_chip_select );


	/* Read block protection */
	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( SST26VF_RBPR_OPCODE );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	for ( uint8_t i = 0; i < sizeof( _block_protection ); ++i )
	{
		_block_protection[i] = m_spi_ctrl->RX_Byte();
	}

	m_spi_ctrl->De_Select_Chip( m_chip_select );


	memset( &m_asynch_data, 0, sizeof( m_asynch_data ) );
	// coverity[leaked_storage]
	new CR_Tasker( &Serial_Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
				   CR_TASKER_IDLE_PRIORITY, "Serial_Flash_Routine" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
SST26VF::~SST26VF( void )
{
	m_spi_ctrl = reinterpret_cast<uC_SPI*>( nullptr );
	m_chip_cfg = reinterpret_cast<SST26VF_CHIP_CONFIG_TD const*>( nullptr );
	Ram::De_Allocate( m_wpage_buff );
	m_wpage_buff = reinterpret_cast<uint8_t*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SST26VF::Read( uint8_t* data, uint32_t address, uint32_t length,
									cback_func_t cback, cback_param_t param )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, false ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "R: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_wait_time = Read_Time( const_cast<uint8_t*>( data ), address, length );
			m_state = READ;
			return_status = NV_Ctrl::PENDING_CALL_BACK;
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SST26VF::Write( const uint8_t* data, uint32_t address, uint32_t length,
									 cback_func_t cback, cback_param_t param )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, false ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "W: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			bool include_erase_time = false;
			m_wait_time = Write_Time( const_cast<uint8_t*>( data ), address, length, include_erase_time );
			m_state = WRITE;
			return_status = NV_Ctrl::PENDING_CALL_BACK;
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SST26VF::Erase( uint32_t address, uint32_t length,
									 cback_func_t cback, cback_param_t param )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, false ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "E: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = nullptr;
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_state = ERASE;
			m_wait_time = Erase_Time( address, length );
			return_status = NV_Ctrl::PENDING_CALL_BACK;
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SST26VF::Erase_All( cback_func_t cback, cback_param_t param )
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
NV_Ctrl::nv_status_t SST26VF::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data = false;
	uint_fast8_t read_count = 0;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, use_protection ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "r: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			do
			{
				read_count++;
				good_data = Read_Now( data, address, length, use_protection );
			} while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_READS ) );

			// if the data didn't read right, go check the backup location to see if
			// our good data is there.
			if ( ( !good_data ) && ( use_protection == true ) )
			{
				read_count = 0U;
				do
				{
					read_count++;
					good_data = Read_Now( data, ( address + m_chip_cfg->mirror_start_address ),
										  length, use_protection );
				} while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_READS ) );

				if ( good_data == true )
				{
					good_data = Write_Prep( data, ( address + m_chip_cfg->mirror_start_address ),
											address, length, use_protection );
				}
			}
			m_semaphore = NV_Ctrl::RELEASE;

			if ( good_data == true )
			{
				return_status = NV_Ctrl::SUCCESS;
			}
			else
			{
				return_status = NV_Ctrl::DATA_ERROR;
				NV_CTRL_PRINT( DBG_MSG_ERROR,
							   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
							   address, length, use_protection, return_status );
			}
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( SST26VF_READ_MEMORY_OPCODE, address );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->Set_XChange_DMA( data, data, static_cast<uint16_t>( length ) );
		m_spi_ctrl->Enable_XChange_DMA();

		while ( m_spi_ctrl->XChange_DMA_Busy() )
		{}

		m_spi_ctrl->Disable_XChange_DMA();

		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		success = true;

		m_spi_ctrl->Release_Semaphore();
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SST26VF::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											 NV_CTRL_LENGTH_TD length )
{
	return ( Read( reinterpret_cast<uint8_t*>( checksum_dest ), address + length,
				   SST26VF_NV_CHECKSUM_LEN, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SST26VF::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									 bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data = true;
	uint8_t num_block = 0;	// number of blocks of each 256 bytes to be written
	uint8_t offset = 0;	// number of bytes remaining for writing
	uint16_t i = 0;

	num_block = length / ( SST26VF_MAX_PROGRAM_BYTE );
	offset = length % ( SST26VF_MAX_PROGRAM_BYTE );
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length, use_protection ) )
		{
			m_semaphore = NV_Ctrl::ACQUIRE;
			if ( m_fast_write == true )
			{
				bool_t erased_area = true;
				uint32_t counter = 0U;
				for ( counter = 0U; ( ( counter < num_block ) && ( true == erased_area ) ); counter++ )
				{
					uC_Watchdog::Force_Feed_Dog();
#if defined( DATA_CACHE_ENABLE )
					SCB_CleanInvalidateDCache_by_Addr( reinterpret_cast<uint32_t*>( m_wpage_buff ),
													   SST26VF_MAX_PROGRAM_BYTE );
#endif	// DATA_CACHE_ENABLE

#if defined ( INSTRUCTION_CACHE_ENABLE )
					SCB_InvalidateICache();
#endif	// INSTRUCTION_CACHE_ENABLE
					Quick_Read( m_wpage_buff, ( address + ( counter * SST26VF_MAX_PROGRAM_BYTE ) ),
								SST26VF_MAX_PROGRAM_BYTE );
					for ( i = 0; i < SST26VF_MAX_PROGRAM_BYTE; i++ )
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
#if defined( DATA_CACHE_ENABLE )
						SCB_CleanInvalidateDCache_by_Addr( reinterpret_cast<uint32_t*>( m_wpage_buff ),
														   SST26VF_MAX_PROGRAM_BYTE );
#endif	// DATA_CACHE_ENABLE

#if defined ( INSTRUCTION_CACHE_ENABLE )
						SCB_InvalidateICache();
#endif	// INSTRUCTION_CACHE_ENABLE
						Quick_Read( m_wpage_buff, ( address + ( ( num_block ) * SST26VF_MAX_PROGRAM_BYTE ) ), offset );
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
						// Checking if the data length croses page border while writing.
						if ( ( address % SST26VF_MAX_PROGRAM_BYTE ) == 0U )
						{
							good_data = Write_Now( data, address + ( counter * SST26VF_MAX_PROGRAM_BYTE ),
												   SST26VF_MAX_PROGRAM_BYTE );
						}
						else
						{
							// If data length croses the page border data will be splitted into two.
							// First part to write till page end.
							good_data =
								Write_Now( data, address + ( counter * SST26VF_MAX_PROGRAM_BYTE ),
										   ( SST26VF_MAX_PROGRAM_BYTE - ( address % SST26VF_MAX_PROGRAM_BYTE ) ) );
							// Only if first data write is good.
							// Second part to write remaining bytes from page start.
							if ( good_data == true )
							{
								good_data =
									Write_Now(
										( data + ( SST26VF_MAX_PROGRAM_BYTE - ( address % SST26VF_MAX_PROGRAM_BYTE ) ) ),
										address + ( ( counter + 1 ) * SST26VF_MAX_PROGRAM_BYTE ) -
										( address % SST26VF_MAX_PROGRAM_BYTE ),
										( address % SST26VF_MAX_PROGRAM_BYTE ) );
							}
						}
						data = data + SST26VF_MAX_PROGRAM_BYTE;
					}
					if ( ( offset != 0U ) && ( good_data == true ) )				/* Write the remaining bytes */
					{
						if ( counter == num_block )
						{
							// Checking if the data length croses page border while writing.
							if ( ( static_cast<uint32_t>( address / SST26VF_MAX_PROGRAM_BYTE ) ) ==
								 ( static_cast<uint32_t>( ( address + offset ) / SST26VF_MAX_PROGRAM_BYTE ) ) )
							{
								good_data =
									Write_Now( data, address + ( num_block * SST26VF_MAX_PROGRAM_BYTE ), offset );
							}
							else
							{
								// If data length croses the page border data will be splitted into two.
								// First part to write till page end.
								good_data =
									Write_Now( data, ( address + ( num_block * SST26VF_MAX_PROGRAM_BYTE ) ),
											   ( offset - ( ( address + offset ) % SST26VF_MAX_PROGRAM_BYTE ) ) );
								// Only if first data write is good.
								// Second part to write remaining bytes from page start.
								if ( good_data == true )
								{
									good_data =
										Write_Now(
											( data + ( offset - ( ( address + offset ) % SST26VF_MAX_PROGRAM_BYTE ) ) ),
											( address + ( num_block * SST26VF_MAX_PROGRAM_BYTE ) +
											  ( offset - ( ( address + offset ) % SST26VF_MAX_PROGRAM_BYTE ) ) ),
											( ( address + offset ) % SST26VF_MAX_PROGRAM_BYTE ) );
								}
							}
						}
					}

					if ( good_data == true )
					{
						return_status = NV_Ctrl::SUCCESS;
					}
					else
					{
						return_status = NV_Ctrl::DATA_ERROR;
						NV_CTRL_PRINT( DBG_MSG_ERROR,
									   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
									   address, length, use_protection, return_status );
					}
				}
				else
				{
					return_status = NV_Ctrl::DATA_ERROR;
				}
			}
			else
			{
				uint_fast8_t write_count;
				uint32_t recovery_address;	// Contains the data to be moved over to the dest.

				write_count = 0U;

				if ( use_protection == true )
				{
					recovery_address = ( address - m_chip_cfg->start_address )
						+ m_chip_cfg->mirror_start_address;
				}
				else
				{
					recovery_address = ( address & ~SST26VF_EPAGE_ALIGNMENT_MASK )
						+ SST26VF_SCRATCH_EPAGE_ADDRESS;
				}

				do
				{
					write_count++;

					good_data = Write_Prep( data, recovery_address, address, length, use_protection );
					if ( good_data == true )
					{
						good_data = Check_Data( data, address, length, use_protection );
					}
				} while ( ( good_data == false ) && ( write_count < MAX_NUMBER_OF_WRITES ) );

				if ( ( good_data == true ) && ( use_protection == true ) )
				{
					write_count = 0U;
					do
					{
						write_count++;

						good_data =
							Write_Prep( data, address, recovery_address, length, use_protection );
						if ( good_data == true )
						{
							good_data = Check_Data( data, recovery_address, length, use_protection );
						}
					} while ( ( good_data == false ) && ( write_count < MAX_NUMBER_OF_WRITES ) );
				}

				if ( good_data == true )
				{
					return_status = NV_Ctrl::SUCCESS;
				}
				else
				{
					return_status = NV_Ctrl::DATA_ERROR;
					NV_CTRL_PRINT( DBG_MSG_ERROR,
								   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
								   address, length, use_protection, return_status );
				}

			}
			m_semaphore = NV_Ctrl::RELEASE;
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	bool_t status = true;
	uint8_t read_status = 0U;

	// Added check to validate length
	if ( length == 0 )
	{
		return ( status );
	}

	if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( SST26VF_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.
		Delay_Chip_Select_Change();

		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( SST26VF_WRITE_MEMORY_OPCODE, address );

		m_spi_ctrl->Set_TX_DMA( data, static_cast<uint16_t>( length ) );
		m_spi_ctrl->Enable_TX_DMA();

		while ( m_spi_ctrl->TX_DMA_Busy() )
		{}
		m_spi_ctrl->Disable_TX_DMA();

		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}
		m_spi_ctrl->De_Select_Chip( m_chip_select );

		m_spi_ctrl->Release_Semaphore();

		do
		{
			Read_Status( &read_status );
		} while ( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) );

	}
	else
	{
		status = false;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Write_Prep( uint8_t* data, uint32_t recovery_address, uint32_t dest_address,
						  NV_CTRL_LENGTH_TD length, bool use_protection )
{
	BUFF_STRUCT_TD* buff_walker = nullptr;
	NV_CTRL_LENGTH_TD total_length = 0;

	m_src_buff.data = data;
	m_src_buff.length = length;
	m_src_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
	m_start_buff = &m_src_buff;

	total_length = length;

	if ( use_protection == true )
	{
		total_length += 2U;

		if ( data == &m_erase_val )	// If we are writing an erase value and we have to sum it up.
		{
			m_checksum = static_cast<uint16_t>( m_erase_val * m_src_buff.length );
		}
		else
		{
			m_checksum = 0U;
			for ( NV_CTRL_LENGTH_TD i = 0U; i < m_src_buff.length; i++ )
			{
				m_checksum += m_src_buff.data[i];
			}
		}
		m_checksum_buff.data = reinterpret_cast<uint8_t*>( &m_checksum );
		m_checksum_buff.length = SST26VF_NV_CHECKSUM_LEN;
		m_checksum_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
		m_src_buff.next = &m_checksum_buff;
	}

	if ( ( dest_address & ~SST26VF_EPAGE_ALIGNMENT_MASK ) != 0U )
	{
		m_begin_rec_buff.data = reinterpret_cast<uint8_t*>( nullptr );
		m_begin_rec_buff.length = dest_address & ~SST26VF_EPAGE_ALIGNMENT_MASK;
		m_begin_rec_buff.next = m_start_buff;
		m_start_buff = &m_begin_rec_buff;
	}

	if ( ( ( dest_address + total_length ) & ~SST26VF_EPAGE_ALIGNMENT_MASK ) != 0U )
	{
		m_end_rec_buff.data = reinterpret_cast<uint8_t*>( nullptr );
		m_end_rec_buff.length = m_chip_cfg->erase_page_size
			- ( ( dest_address + total_length ) & ~SST26VF_EPAGE_ALIGNMENT_MASK );
		buff_walker = m_start_buff;
		m_end_rec_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
		while ( buff_walker->next != NULL )
		{
			buff_walker = buff_walker->next;
		}
		buff_walker->next = &m_end_rec_buff;
	}

	Write_Buff( m_start_buff, ( dest_address & SST26VF_EPAGE_ALIGNMENT_MASK ),
				( recovery_address & SST26VF_EPAGE_ALIGNMENT_MASK ), use_protection );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Write_Buff( BUFF_STRUCT_TD* buff_list, uint32_t dest_page,
						  uint32_t source_page, bool use_protection )
{
	BUFF_STRUCT_TD* buff_walker = nullptr;
	NV_CTRL_LENGTH_TD wpage_counter = 0;
	NV_CTRL_LENGTH_TD max_count = 0;
	bool success = true;
	uint8_t* dest_ptr = nullptr;

	buff_walker = buff_list;

	while ( ( buff_walker != NULL ) && ( success == true ) )
	{
		if ( ( dest_page & ~SST26VF_EPAGE_ALIGNMENT_MASK ) == 0U )
		{
			// If we are using the scratch space we need to move the page from scratch to the
			// dest area.
			// We skip this operation if this is the first time in here.
			if ( use_protection == false )
			{
				source_page = SST26VF_SCRATCH_EPAGE_ADDRESS;
				Erase_Page( source_page );		// We need to erase the page we took the data from so
												// we can write it back.

				// Yes this seems incorrect but it shouldn't be.
				// We need to take the destination data and copy it to the scratch space so we
				// can copy it
				// back.  It is a bit silly but it works.  Plus I can make this use a ram buffer
				// at some point
				// with little headache I believe.
				for ( uint32_t i = 0U; i < m_chip_cfg->erase_page_size;
					  i += m_chip_cfg->write_page_size )
				{
					Quick_Read( m_wpage_buff, dest_page + i, m_chip_cfg->write_page_size );
					success = Write_Now( m_wpage_buff, source_page + i, m_chip_cfg->write_page_size );
					if ( success == false )
					{
						break;
					}
				}
			}
			Erase_Page( dest_page );
		}
		Quick_Read( m_wpage_buff, source_page, m_chip_cfg->write_page_size );
		source_page += m_chip_cfg->write_page_size;

		wpage_counter = m_chip_cfg->write_page_size;
		while ( wpage_counter > 0U )
		{
			if ( buff_walker->length >= wpage_counter )
			{
				max_count = wpage_counter;
			}
			else
			{
				max_count = buff_walker->length;
			}
			if ( buff_walker->data != NULL )
			{
				dest_ptr = &m_wpage_buff[m_chip_cfg->write_page_size - wpage_counter];
				if ( buff_walker->data != &m_erase_val )
				{
					BF_Lib::Copy_String( dest_ptr, buff_walker->data, max_count );
					buff_walker->data += max_count;
				}
				else
				{
					for ( NV_CTRL_LENGTH_TD i = 0U; i < max_count; i++ )
					{
						dest_ptr[i] = *buff_walker->data;
					}
				}
			}
			wpage_counter -= max_count;
			buff_walker->length -= max_count;
			if ( buff_walker->length == 0U )
			{
				buff_walker = buff_walker->next;
			}
		}

		success = Write_Now( m_wpage_buff, dest_page, m_chip_cfg->write_page_size );
		dest_page += m_chip_cfg->write_page_size;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length,
						  bool use_protection )
{
	bool good_data = true;
	uint8_t read_data = 0;

	BF_Lib::SPLIT_UINT16 checksum;

	if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( SST26VF_READ_MEMORY_OPCODE, address );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		checksum.u16 = 0U;
		while ( length > 0U )
		{
			read_data = m_spi_ctrl->RX_Byte();

			checksum.u16 = read_data + checksum.u16;

			if ( *check_data != read_data )
			{
				good_data = false;
			}
			if ( check_data != &m_erase_val )
			{
				check_data++;
			}
			length--;
		}

		if ( ( use_protection == true ) && ( good_data == true ) )
		{
			read_data = m_spi_ctrl->RX_Byte();
			if ( read_data != checksum.u8[0] )
			{
				good_data = false;
			}

			read_data = m_spi_ctrl->RX_Byte();
			if ( read_data != checksum.u8[1] )
			{
				good_data = false;
			}
		}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		m_spi_ctrl->Release_Semaphore();
	}
	else
	{
		good_data = false;
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Erase_Page( uint32_t address )
{
	bool_t ret_status = false;
	uint8_t read_status = 0U;

	Read_Status( &read_status );
	if ( !( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
		{
			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->TX_Byte( SST26VF_SET_WRITE_ENABLE_LATCH_OPCODE );
			while ( !m_spi_ctrl->TX_Shift_Empty() )
			{}

			m_spi_ctrl->De_Select_Chip( m_chip_select );

			// An edge has to be sent to the Chip select pin on a write instruction.
			// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
			Delay_Chip_Select_Change()

			m_spi_ctrl->Select_Chip( m_chip_select );

			Send_Data_Command( SST26VF_ERASE_4K_PAGE_OPCODE, address );

			m_spi_ctrl->De_Select_Chip( m_chip_select );

			m_spi_ctrl->Release_Semaphore();
		}
		else
		{
			return ( false );
		}

		// An erase should happen.  We should wait.
		if ( true == m_synchronous_erase )
		{
			uC_Watchdog::Force_Feed_Dog();
			do
			{
				Read_Status( &read_status );
			} while ( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) );
			ret_status = true;
		}
		else
		{
			m_serial_flash_busy = true;
			ret_status = false;
		}
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t SST26VF::Get_Block_Size( uint32_t address ) const
{
#ifdef SST26VF_LOOKUP_TABLE
	uint32_t page_index = 0U;
	uint32_t test_address = 0U;

	test_address = EXTERNAL_FLASH_START_ADDRESS +
		EXTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	while ( ( address >= test_address ) &&
			( EXTERNAL_FLASH_PAGE_SIZES[page_index].sector_size != 0U ) )
	{
		page_index++;
		test_address += EXTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	}

	return ( EXTERNAL_FLASH_PAGE_SIZES[page_index].sector_size );

#else
	uint32_t block_size = 0;
	if ( ( address >= BLOCK_64K_LOWER_THRESHOLD ) &&
		 ( address <= BLOCK_64K_UPPER_THRESHOLD ) )
	{
		block_size = BLOCK_64K;
	}
	else if ( ( ( address >= BLOCK_32K_LOWER_ADDRESS_LOWER_THRESHOLD ) &&
				( address <= BLOCK_32K_LOWER_ADDRESS_UPPER_THRESHOLD ) ) ||
			  ( ( address >= BLOCK_32K_UPPER_ADDRESS_LOWER_THRESHOLD ) &&
				( address <= BLOCK_32K_UPPER_ADDRESS_UPPER_THRESHOLD ) ) )
	{
		block_size = BLOCK_32K;
	}
	else if ( ( address <= BLOCK_8K_LOWER_ADDRESS_UPPER_THRESHOLD ) ||
			  ( ( address >= BLOCK_8K_UPPER_ADDRESS_LOWER_THRESHOLD ) &&
				( address <= BLOCK_8K_UPPER_ADDRESS_UPPER_THRESHOLD ) ) )
	{
		block_size = BLOCK_8K;
	}
	else
	{
		BF_ASSERT( false );
	}
	return ( block_size );

#endif	/// SST26VF_LOOKUP_TABLE
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SST26VF::Erase( uint32_t address, NV_CTRL_LENGTH_TD length,
									 uint8_t erase_data,
									 bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool erase_status = false;
	uint32_t sector_size = 0;

	m_erase_val = erase_data;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		NV_CTRL_PRINT( DBG_MSG_INFO, "e: %x	%d", address, length );
		m_semaphore = NV_Ctrl::ACQUIRE;
		return_status = NV_Ctrl::SUCCESS;

		if ( m_fast_write != true )
		{
			uint16_t index = 0U;
			bool good_data = true;
			for ( index = 0; index < SST26VF_MAX_PROGRAM_BYTE; index++ )
			{
				m_wpage_buff[index] = m_erase_val;
			}
			while ( ( length != 0U ) && ( good_data != false ) )
			{
				if ( length >= SST26VF_MAX_PROGRAM_BYTE )
				{
					good_data = Write_Now( m_wpage_buff, address, SST26VF_MAX_PROGRAM_BYTE );
					length -= SST26VF_MAX_PROGRAM_BYTE;
					uC_Watchdog::Force_Feed_Dog();
				}
				else
				{
					good_data = Write_Now( m_wpage_buff, address, length );
					length = 0U;
					break;
				}
			}
			if ( ( good_data != false ) && ( length == 0U ) )
			{
				return_status = NV_Ctrl::SUCCESS;
			}
			else
			{
				return_status = NV_Ctrl::BUSY_ERROR;
			}
		}
		else
		{
			// Check the start address is at the beginning of sector or not.
			while ( ( length >= BLOCK_8K ) && ( return_status == NV_Ctrl::SUCCESS ) )
			{
				sector_size = Get_Block_Size( address );
				erase_status = Erase_Pages( address, SST26VF_ERASE_BLOCK_OPCODE );
				address += sector_size;
				length -= sector_size;

				if ( erase_status != true )
				{
					return_status = NV_Ctrl::BUSY_ERROR;
					break;
				}
			}	// end of while loop()
			if ( ( length >= BLOCK_4K ) &&
				 ( Is_Aligned( address, BLOCK_4K ) ) && ( return_status == NV_Ctrl::SUCCESS ) )
			{
				erase_status = Erase_Pages( address, SST26VF_ERASE_4K_PAGE_OPCODE );
				length -= BLOCK_4K;

				if ( erase_status != true )
				{
					return_status = NV_Ctrl::BUSY_ERROR;
				}
			}
			if ( ( length < BLOCK_4K ) && ( length != 0 ) )
			{
				return_status = NV_Ctrl::OPERATION_NOT_SUPPORTED;
			}
		}
		m_semaphore = NV_Ctrl::RELEASE;
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Erase_Pages( uint32_t address, uint8_t erase_cmd )
{
	bool_t ret_status = true;
	uint8_t read_status = 0U;
	uint16_t max_time = 0U;
	BF_Lib::Timers::TIMERS_TIME_TD begin_time = 0U;

	Read_Status( &read_status );
	if ( !( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
		{
			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->TX_Byte( SST26VF_SET_WRITE_ENABLE_LATCH_OPCODE );
			while ( !m_spi_ctrl->TX_Shift_Empty() )
			{}

			m_spi_ctrl->De_Select_Chip( m_chip_select );

			// An edge has to be sent to the Chip select pin on a write instruction.
			// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
			Delay_Chip_Select_Change();

			if ( ( erase_cmd != SST26VF_ERASE_4K_PAGE_OPCODE ) && ( erase_cmd != SST26VF_ERASE_BLOCK_OPCODE ) )
			{
				ret_status = false;
			}

			if ( ret_status == true )
			{
				if ( erase_cmd == SST26VF_ERASE_4K_PAGE_OPCODE )
				{
					max_time = SST26VF_ERASE_4K_MAX_TIME_MS;
				}
				else if ( erase_cmd == SST26VF_ERASE_BLOCK_OPCODE )
				{
					max_time = SST26VF_ERASE_BLOCK_MAX_TIME_MS;
				}
				m_spi_ctrl->Select_Chip( m_chip_select );
				Send_Data_Command( erase_cmd, address );
				m_spi_ctrl->De_Select_Chip( m_chip_select );
			}

			// An erase should happen.  We should wait.
			if ( true == m_synchronous_erase )
			{
				ret_status = false;
				begin_time = BF_Lib::Timers::Get_Time();
				while ( ret_status == false )
				{
					uC_Delay( ERASE_PAGE_DELAY_US );
					if ( BF_Lib::Timers::Expired( begin_time, max_time ) )
					{
						ret_status = true;
					}
					uC_Watchdog::Force_Feed_Dog();
				}
			}
			else
			{
				uC_Watchdog::Force_Feed_Dog();
				uC_Delay( max_time * MICROSECONDS_ADJUSTMENT );	// 25 ms delay
				ret_status = true;
			}
			m_spi_ctrl->Release_Semaphore();
			do
			{
				Read_Status( &read_status );
			} while ( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) );
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
bool SST26VF::Erase_Chip( uint32_t address, uint8_t erase_cmd )
{
	bool_t ret_status = false;
	uint8_t read_status = 0U;
	uint16_t max_time = 0U;
	BF_Lib::Timers::TIMERS_TIME_TD begin_time = 0U;

	if ( !( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
		{
			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->TX_Byte( SST26VF_SET_WRITE_ENABLE_LATCH_OPCODE );
			while ( !m_spi_ctrl->TX_Shift_Empty() )
			{}

			m_spi_ctrl->De_Select_Chip( m_chip_select );

			// An edge has to be sent to the Chip select pin on a write instruction.
			// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
			Delay_Chip_Select_Change();

			max_time = SST26VF_ERASE_CHIP_MAX_TIME_MS;
			m_spi_ctrl->Select_Chip( m_chip_select );
			Send_Data_Command( SST26VF_ERASE_CHIP_OPCODE, address );
			m_spi_ctrl->De_Select_Chip( m_chip_select );

			// An erase should happen.  We should wait.
			if ( true == m_synchronous_erase )
			{
				begin_time = BF_Lib::Timers::Get_Time();
				while ( ret_status == false )
				{
					uC_Delay( ERASE_PAGE_DELAY_US );
					if ( BF_Lib::Timers::Expired( begin_time, max_time ) )
					{
						ret_status = true;
					}
					uC_Watchdog::Force_Feed_Dog();
				}
			}
			else
			{
				uC_Watchdog::Force_Feed_Dog();
				uC_Delay( max_time * MICROSECONDS_ADJUSTMENT );	// 25 ms delay
				ret_status = true;
			}
			m_spi_ctrl->Release_Semaphore();
			do
			{
				Read_Status( &read_status );
			} while ( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) );
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

NV_Ctrl::nv_status_t SST26VF::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					SST26VF_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SST26VF::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
						bool use_protection )
{
	bool good_data = false;

	BF_Lib::SPLIT_UINT16 checksum;
	BF_Lib::SPLIT_UINT16 read_checksum;

	if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( SST26VF_READ_MEMORY_OPCODE, address );
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
			read_checksum.u8[0] = m_spi_ctrl->RX_Byte();
			read_checksum.u8[1] = m_spi_ctrl->RX_Byte();
			if ( read_checksum.u16 == checksum.u16 )
			{
				good_data = true;
			}
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
void SST26VF::Send_Data_Command( uint8_t command, uint32_t address )
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
bool SST26VF::Read_Status( uint8_t* status )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( SST26VF_READ_STATUS_REGISTER_OPCODE );
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
bool SST26VF::Read_Device_ID( uint8_t* device_id_dest )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( SST26VF_READ_DEVICE_ID_OPCODE );
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
bool SST26VF::Write_Status( uint8_t status )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( SST26VF_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( SST26VF_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
		Delay_Chip_Select_Change();

		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( SST26VF_WRITE_STATUS_REGISTER_OPCODE );
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
bool SST26VF::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						   bool protection ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address = 0U;

	if ( protection == true )
	{
		length += SST26VF_NV_CHECKSUM_LEN;
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
uint32_t SST26VF::Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							 NV_CTRL_LENGTH_TD length ) const
{
	// Todo: Implementation pending
	uint32_t read_time = 0U;
	uint32_t byte_read_time = 0U;

	byte_read_time = BYTE_READ_TIME_US * length;

	if ( byte_read_time < MICROSECONDS_ADJUSTMENT )
	{
		byte_read_time = MICROSECONDS_ADJUSTMENT;
	}
	// Because the byte read time is in microseconds we have to adjust it down.
	read_time += Round_Div( byte_read_time, MICROSECONDS_ADJUSTMENT );
	return ( read_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t SST26VF::Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							  NV_CTRL_LENGTH_TD length, bool include_erase_time ) const
{
	uint32_t write_time = 0U;
	uint32_t byte_write_time = 0U;

	if ( data != NULL )
	{
		byte_write_time = BYTE_WRITE_TIME_US * length;
		if ( byte_write_time < MICROSECONDS_ADJUSTMENT )
		{
			byte_write_time = MICROSECONDS_ADJUSTMENT;
		}
		// Because the byte write time is in microseconds we have to adjust it down.
		write_time += Round_Div( byte_write_time, MICROSECONDS_ADJUSTMENT );
	}
	return ( write_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t SST26VF::Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	uint32_t wait_time = 0U;
	uint32_t block_count = 0U;

	if ( ( address % BLOCK_4K == 0 ) || ( length % BLOCK_4K == 0 ) )
	{
		if ( length >= BLOCK_8K )
		{
			block_count = ( length / BLOCK_8K );
			wait_time += ( ( block_count * SST26VF_ERASE_BLOCK_MAX_TIME_MS ) );
			length = ( length % BLOCK_8K );
		}
		if ( length >= BLOCK_4K )
		{
			block_count = ( length / BLOCK_4K );
			wait_time += ( ( block_count * SST26VF_ERASE_4K_MAX_TIME_MS ) );
		}
	}
	return ( wait_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SST26VF::Serial_Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{

	( ( SST26VF* )param )->Serial_Flash_Async_Task( cr_task );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SST26VF::Serial_Flash_Async_Task( CR_Tasker* cr_task )
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
void SST26VF::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_asynch_data.cback = func_callback;
	m_asynch_data.param = func_param;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SST26VF::Get_Memory_Info( mem_range_info_t* mem_range_info )
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
void SST26VF::asynch_erase_handler( void )
{
	/* Acquired semaphore for complete erase operation */
	bool erase_status = true;
	uint8_t read_status = 0U;

	m_synchronous_erase = true;
	uint32_t sector_size = 0;

	Read_Status( &read_status );

	if ( !( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) ) )
	{
		NV_CTRL_PRINT( DBG_MSG_INFO, "Erase: %x	%d", m_asynch_data.address, m_asynch_data.length );

		if ( m_asynch_data.length >= BLOCK_8K )
		{
			sector_size = Get_Block_Size( m_asynch_data.address );
			erase_status = Erase_Pages( m_asynch_data.address, SST26VF_ERASE_BLOCK_OPCODE );
			m_asynch_data.address += sector_size;
			m_asynch_data.length -= sector_size;
			if ( erase_status != true )
			{
				m_state = ERROR;
				m_status = NV_Ctrl::BUSY_ERROR;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}
		else if ( m_asynch_data.length >= BLOCK_4K )
		{
			erase_status = Erase_Pages( m_asynch_data.address, SST26VF_ERASE_4K_PAGE_OPCODE );
			m_asynch_data.address += BLOCK_4K;
			m_asynch_data.length -= BLOCK_4K;
			if ( erase_status != true )
			{
				m_state = ERROR;
				m_status = NV_Ctrl::BUSY_ERROR;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}

		if ( ( m_asynch_data.length < BLOCK_4K ) && ( erase_status == true ) )
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
void SST26VF::asynch_write_handler( void )
{
	NV_CTRL_LENGTH_TD length = 0U;
	uint8_t read_status = 0U;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		Read_Status( &read_status );
		if ( !( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) ) )
		{
			/* Acquire semaphore only for WRITE_CHUCK_SIZE transfer */
			if ( m_asynch_data.length > SST26VF_MAX_PROGRAM_BYTE )
			{
				length = SST26VF_MAX_PROGRAM_BYTE;
			}
			else
			{
				length = m_asynch_data.length;
			}
			uC_Watchdog::Force_Feed_Dog();

			if ( true == Write_Now( m_asynch_data.data, m_asynch_data.address, SST26VF_MAX_PROGRAM_BYTE ) )
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
void SST26VF::asynch_read_handler( void )
{
	NV_CTRL_LENGTH_TD length = 0U;
	uint8_t read_status = 0U;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		Read_Status( &read_status );
		if ( !( BF_Lib::Bit::Test( read_status, SST26VF_STATUS_BUSY_BIT ) ) )
		{
			/* Acquire semaphore only for READ_CHUCK_SIZE transfer */
			if ( m_asynch_data.length > SST26VF_MAX_PROGRAM_BYTE )
			{
				length = SST26VF_MAX_PROGRAM_BYTE;
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
