/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "AT25DF.h"
#include "StdLib_MV.h"
#include "Ram.h"
#include "NV_Ctrl_Debug.h"
#include "Babelfish_Assert.h"

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

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
AT25DF::AT25DF( uC_SPI* spi_ctrl, uint8_t chip_select,
				AT25DF_CHIP_CONFIG_TD const* chip_config,
				bool_t synchronus_erase, cback_func_t cback_func,
				cback_param_t param, bool_t fast_write ) :
	NV_Ctrl(),
	m_spi_ctrl( reinterpret_cast<uC_SPI*>( nullptr ) ),
	m_chip_select( 0U ),
	m_chip_cfg( reinterpret_cast<AT25DF_CHIP_CONFIG_TD const*>( nullptr ) ),
	m_wpage_buff( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_start_buff( reinterpret_cast<BUFF_STRUCT_TD*>( nullptr ) ),
	m_checksum( 0U ),
	m_erase_val( 0U ),
	m_serial_flash_busy( false ),
	m_synchronous_erase( synchronus_erase ),
	m_fast_write( fast_write ),
	m_call_back_ptr( cback_func ),
	m_cback_param( param ),
	m_src_buff{ nullptr, nullptr, 0U },
	m_checksum_buff{ nullptr, nullptr, 0U },
	m_begin_rec_buff{ nullptr, nullptr, 0U },
	m_end_rec_buff{ nullptr, nullptr, 0U }
{
	uint8_t status = 0U;

	m_spi_ctrl = spi_ctrl;

	m_chip_cfg = chip_config;

	m_chip_select = chip_select;

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
	if ( false == m_synchronous_erase )
	{
		BF_ASSERT( NULL != cback_func );

		new CR_Tasker( &Serial_Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
					   CR_TASKER_IDLE_PRIORITY, "Serial_Flash_Routine" );

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
AT25DF::~AT25DF( void )
{
	BF_ASSERT( false );	// this class is not supposed to be destructed
	Ram::De_Allocate( m_wpage_buff );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t AT25DF::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
								   bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;
	uint_fast8_t read_count;

	if ( Check_Range( address, length, use_protection ) && ( length != 0U ) )
	{
		read_count = 0U;
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

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF::Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
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
NV_Ctrl::nv_status_t AT25DF::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
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
NV_Ctrl::nv_status_t AT25DF::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;
	uint8_t num_block = 0;	// number of blocks of each 256 bytes to be written
	uint8_t offset = 0;	// number of bytes remaining for writing
	uint16_t i = 0;

	num_block = length / ( AT25DF_MAX_PROGRAM_BYTE );
	offset = length % ( AT25DF_MAX_PROGRAM_BYTE );

	if ( Check_Range( address, length, use_protection ) )
	{
		if ( m_fast_write == true )
		{
			bool_t erased_area = true;
			uint32_t counter = 0U;

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
					// Checking if the data length croses page border while writing.
					if ( ( address % AT25DF_MAX_PROGRAM_BYTE ) == 0U )
					{
						good_data = Write_Now( data, address + ( counter * AT25DF_MAX_PROGRAM_BYTE ),
											   AT25DF_MAX_PROGRAM_BYTE );
					}
					else
					{
						// If data length croses the page border data will be splitted into two.
						// First part to write till page end.
						good_data =
							Write_Now( data, address + ( counter * AT25DF_MAX_PROGRAM_BYTE ),
									   ( AT25DF_MAX_PROGRAM_BYTE - ( address % AT25DF_MAX_PROGRAM_BYTE ) ) );
						// Only if first data write is good.
						// Second part to write remaining bytes from page start.
						if ( good_data == true )
						{
							good_data =
								Write_Now(
									( data + ( AT25DF_MAX_PROGRAM_BYTE - ( address % AT25DF_MAX_PROGRAM_BYTE ) ) ),
									address + ( ( counter + 1 ) * AT25DF_MAX_PROGRAM_BYTE ) -
									( address % AT25DF_MAX_PROGRAM_BYTE ),
									( address % AT25DF_MAX_PROGRAM_BYTE ) );
						}
					}
					data = data + AT25DF_MAX_PROGRAM_BYTE;
				}
				if ( ( offset != 0U ) && ( good_data == true ) )				/* Write the remaining bytes */
				{
					if ( counter == num_block )
					{
						// Checking if the data length croses page border while writing.
						if ( ( static_cast<uint32_t>( address / AT25DF_MAX_PROGRAM_BYTE ) ) ==
							 ( static_cast<uint32_t>( ( address + offset ) / AT25DF_MAX_PROGRAM_BYTE ) ) )
						{
							good_data = Write_Now( data, address + ( num_block * AT25DF_MAX_PROGRAM_BYTE ), offset );
						}
						else
						{
							// If data length croses the page border data will be splitted into two.
							// First part to write till page end.
							good_data =
								Write_Now( data, ( address + ( num_block * AT25DF_MAX_PROGRAM_BYTE ) ),
										   ( offset - ( ( address + offset ) % AT25DF_MAX_PROGRAM_BYTE ) ) );
							// Only if first data write is good.
							// Second part to write remaining bytes from page start.
							if ( good_data == true )
							{
								good_data =
									Write_Now(
										( data + ( offset - ( ( address + offset ) % AT25DF_MAX_PROGRAM_BYTE ) ) ),
										( address + ( num_block * AT25DF_MAX_PROGRAM_BYTE ) +
										  ( offset - ( ( address + offset ) % AT25DF_MAX_PROGRAM_BYTE ) ) ),
										( ( address + offset ) % AT25DF_MAX_PROGRAM_BYTE ) );
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
				recovery_address = ( address & ~AT25DF_EPAGE_ALIGNMENT_MASK )
					+ AT25DF_SCRATCH_EPAGE_ADDRESS;
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
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					   address, length, use_protection, return_status );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF::Write_Prep( uint8_t* data, uint32_t recovery_address, uint32_t dest_address,
						 NV_CTRL_LENGTH_TD length, bool use_protection )
{
	BUFF_STRUCT_TD* buff_walker;
	NV_CTRL_LENGTH_TD total_length;

	m_src_buff.data = data;
	m_src_buff.length = length;
	m_src_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
	m_start_buff = &m_src_buff;

	total_length = length;

	if ( use_protection == true )
	{
		total_length += 2U;

		if ( data == &m_erase_val )		// If we are writing an erase value and we have to sum it up.
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
		m_checksum_buff.length = AT25DF_NV_CHECKSUM_LEN;
		m_checksum_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
		m_src_buff.next = &m_checksum_buff;
	}

	if ( ( dest_address & ~AT25DF_EPAGE_ALIGNMENT_MASK ) != 0U )	// recovery_address !=
																	// recovery_page_address )
	{
		m_begin_rec_buff.data = reinterpret_cast<uint8_t*>( nullptr );		// (uint8_t*)(
																			// recovery_address &
																			// AT25DF_EPAGE_ALIGNMENT_MASK
																			// );
		m_begin_rec_buff.length = dest_address & ~AT25DF_EPAGE_ALIGNMENT_MASK;
		m_begin_rec_buff.next = m_start_buff;
		m_start_buff = &m_begin_rec_buff;
	}

	if ( ( ( dest_address + total_length ) & ~AT25DF_EPAGE_ALIGNMENT_MASK ) != 0U )		// ( recovery_address
																						// + total_length
																						// ) !=
																						// end_page_address
																						// )
	{
		m_end_rec_buff.data = reinterpret_cast<uint8_t*>( nullptr );	// (uint8_t*)(
																		// recovery_address +
																		// total_length );
		m_end_rec_buff.length = m_chip_cfg->erase_page_size
			- ( ( dest_address + total_length ) & ~AT25DF_EPAGE_ALIGNMENT_MASK );		// end_page_address
																						// - (
																						// dest_address
																						// + total_length
																						// );
		buff_walker = m_start_buff;
		m_end_rec_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
		while ( buff_walker->next != NULL )
		{
			buff_walker = buff_walker->next;
		}
		buff_walker->next = &m_end_rec_buff;
	}

	Write_Buff( m_start_buff, ( dest_address & AT25DF_EPAGE_ALIGNMENT_MASK ),
				( recovery_address & AT25DF_EPAGE_ALIGNMENT_MASK ), use_protection );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF::Write_Buff( BUFF_STRUCT_TD* buff_list, uint32_t dest_page,
						 uint32_t source_page, bool use_protection )
{
	BUFF_STRUCT_TD* buff_walker;
	NV_CTRL_LENGTH_TD wpage_counter;
	NV_CTRL_LENGTH_TD max_count;
	bool success = true;
	uint8_t* dest_ptr;

	buff_walker = buff_list;

	while ( ( buff_walker != NULL ) && ( success == true ) )
	{
		if ( ( dest_page & ~AT25DF_EPAGE_ALIGNMENT_MASK ) == 0U )
		{
			// If we are using the scratch space we need to move the page from scratch to the
			// dest area.
			// We skip this operation if this is the first time in here.
			if ( use_protection == false )
			{
				source_page = AT25DF_SCRATCH_EPAGE_ADDRESS;
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
					Write_Now( m_wpage_buff, source_page + i, m_chip_cfg->write_page_size );
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
bool AT25DF::Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	bool_t status = true;
	uint8_t read_status = 0U;

	// Added check to validate length
	if ( length == 0 )
	{
		return ( status );
	}

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

		do
		{
			Read_Status( &read_status );
		} while ( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) );
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
NV_Ctrl::nv_status_t AT25DF::Erase( uint32_t address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data,
									bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::SUCCESS;
	bool_t erase_status = false;

	m_erase_val = erase_data;

	if ( m_fast_write != true )
	{
		uint16_t index = 0U;
		bool good_data = true;
		for ( index = 0; index < AT25DF_MAX_PROGRAM_BYTE; index++ )
		{
			m_wpage_buff[index] = m_erase_val;
		}
		while ( ( length != 0U ) && ( good_data != false ) )
		{
			if ( length >= AT25DF_MAX_PROGRAM_BYTE )
			{
				good_data = Write_Now( m_wpage_buff, address, AT25DF_MAX_PROGRAM_BYTE );
				length -= AT25DF_MAX_PROGRAM_BYTE;
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
			erase_status = true;
		}
	}
	else
	{
		// Check the start address is at the beginning of sector or not.
		if ( ( address % BLOCK_4K == 0 ) || ( length % BLOCK_4K == 0 ) )
		{
			while ( ( length >= BLOCK_4K ) && ( return_status == NV_Ctrl::SUCCESS ) )
			{
				if ( ( length >= BLOCK_64K ) && ( Is_Aligned( address, BLOCK_64K ) ) )
				{
					erase_status = Erase_Pages( address, ERASE_BLOCK_64K );
					address += BLOCK_64K;
					length -= BLOCK_64K;

					if ( erase_status != true )
					{
						return_status = NV_Ctrl::BUSY_ERROR;
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
						return_status = NV_Ctrl::BUSY_ERROR;
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
						return_status = NV_Ctrl::BUSY_ERROR;
						break;
					}
				}
			}
		}
		else
		{
			/* For Fast write the address and length must be aligned to 4 KB*/
			return_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}

	if ( return_status == NV_Ctrl::SUCCESS )
	{
		if ( true == erase_status )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::BUSY_ERROR;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool AT25DF::Erase_Page( uint32_t address )
{
	bool_t ret_status = false;
	uint8_t read_status = 0U;

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

			m_spi_ctrl->Select_Chip( m_chip_select );

			Send_Data_Command( AT25DF_ERASE_4K_PAGE_OPCODE, address );

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
			} while ( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_BUSY_BIT ) );

			if ( BF_Lib::Bit::Test( read_status, AT25DF_STATUS_ERROR_BIT ) )
			{
				ret_status = false;
			}
			else
			{
				ret_status = true;
			}
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
bool AT25DF::Erase_Pages( uint32_t address, uint8_t erase_cmd )
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
					uC_Watchdog::Force_Feed_Dog();
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
				}
			}
			else
			{
				m_serial_flash_busy = true;
				ret_status = false;
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
NV_Ctrl::nv_status_t AT25DF::Erase_All( void )
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
bool AT25DF::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
					   bool use_protection )
{
	bool good_data = false;

	BF_Lib::SPLIT_UINT16 checksum;
	BF_Lib::SPLIT_UINT16 read_checksum;

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
bool AT25DF::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length,
						 bool use_protection )
{
	bool good_data = true;
	uint8_t read_data;

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
void AT25DF::Send_Data_Command( uint8_t command, uint32_t address )
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
bool AT25DF::Read_Status( uint8_t* status )
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
bool AT25DF::Read_Device_ID( uint8_t* device_id_dest )
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
bool AT25DF::Write_Status( uint8_t status )
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
bool AT25DF::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						  bool protection ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	if ( protection == true )
	{
		length += AT25DF_NV_CHECKSUM_LEN;
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
void AT25DF::Serial_Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{

	( ( AT25DF* )param )->Serial_Flash_Async_Task();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF::Serial_Flash_Async_Task( void )
{
	nv_status_t result = BUSY_ERROR;
	uint8_t status = 0U;

	if ( true == m_serial_flash_busy )
	{
		Read_Status( &status );
		if ( !( BF_Lib::Bit::Test( status, AT25DF_STATUS_BUSY_BIT ) ) )
		{
			if ( BF_Lib::Bit::Test( status, AT25DF_STATUS_ERROR_BIT ) )
			{
				m_serial_flash_busy = false;
				result = DATA_ERROR;
			}
			else
			{
				m_serial_flash_busy = false;
				result = SUCCESS;
			}
		}
		else
		{
			/* Do Nothing */
		}
		m_call_back_ptr( m_cback_param, result );
	}
	else
	{
		/* Do Nothing */
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void AT25DF::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_cback_param = func_param;
	m_call_back_ptr = func_callback;
}

}
