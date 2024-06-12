/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "MX25L.h"
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

// The number of bits that the upper bits need to be shifted to get it into the
// right space in the write opcode.  This is only for the single byte address type.
#define Delay_Chip_Select_Change()      uC_Delay( 1U )

#define MX25L_EPAGE_ALIGNMENT_MASK         ~( m_chip_cfg->erase_page_size - 1U )
#define MX25L_WPAGE_ALIGNMENT_MASK         ~( m_chip_cfg->write_page_size - 1U )
#define MX25L_SCRATCH_EPAGE_ADDRESS        ( m_chip_cfg->end_address - \
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
MX25L::MX25L( uC_SPI* spi_ctrl, uint8_t chip_select,
			  MX25L_CHIP_CONFIG_TD const* chip_config,
			  bool_t synchronus_erase, cback_func_t cback_func,
			  cback_param_t param, bool_t fast_write ) :
	NV_Ctrl(),
	m_spi_ctrl( reinterpret_cast<uC_SPI*>( nullptr ) ),
	m_chip_select( 0U ),
	m_chip_cfg( reinterpret_cast<MX25L_CHIP_CONFIG_TD const*>( nullptr ) ),
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
MX25L::~MX25L( void )
{
	BF_ASSERT( false );	// this class is not supposed to be destructed
	Ram::De_Allocate( m_wpage_buff );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t MX25L::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
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
void MX25L::Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( MX25L_READ_MEMORY_ADD_DATA_OPCODE, address );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	for ( uint_fast16_t i = 0; i < length; i++ )
	{
		data[i] = m_spi_ctrl->RX_Byte();
	}

	m_spi_ctrl->De_Select_Chip( m_chip_select );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t MX25L::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
										   NV_CTRL_LENGTH_TD length )
{
	return ( Read( reinterpret_cast<uint8_t*>( checksum_dest ), address + length,
				   MX25L_NV_CHECKSUM_LEN, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t MX25L::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
								   bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	if ( Check_Range( address, length, use_protection ) )
	{
		if ( m_fast_write == true )
		{
			bool_t erased_area = true;
			uint32_t counter = 0U;
			NV_Ctrl::nv_status_t good_data = NV_Ctrl::BUSY_ERROR;
			for ( counter = 0U; ( ( counter < length ) && ( true == erased_area ) ); counter++ )
			{
				Quick_Read( m_wpage_buff, ( address + counter ), 1U );
				if ( *m_wpage_buff != 0xFF )
				{
					erased_area = false;
				}
			}
			if ( true == erased_area )
			{
				good_data = NV_Ctrl::SUCCESS;
				while ( ( length > 0U ) && ( good_data == NV_Ctrl::SUCCESS ) )
				{
					good_data = Write_Now( data, address, 1U );
					length = length - 1U;
					address = address + 1U;
					data = data + 1U;
				}
				if ( good_data == NV_Ctrl::SUCCESS )
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
			bool good_data;
			write_count = 0U;

			if ( use_protection == true )
			{
				recovery_address = ( address - m_chip_cfg->start_address )
					+ m_chip_cfg->mirror_start_address;
			}
			else
			{
				recovery_address = ( address & ~MX25L_EPAGE_ALIGNMENT_MASK )
					+ MX25L_SCRATCH_EPAGE_ADDRESS;
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
							   "Failed to write from : %X, Length: %u, Protection: %d, ErrorStatus: %d",
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
bool MX25L::Write_Prep( uint8_t* data, uint32_t recovery_address, uint32_t dest_address,
						NV_CTRL_LENGTH_TD length, bool use_protection )
{
	BUFF_STRUCT_TD* buff_walker;
	NV_CTRL_LENGTH_TD total_length;

	Push_TGINT();

	m_src_buff.data = data;
	m_src_buff.length = length;
	m_src_buff.next = nullptr;
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
		m_checksum_buff.length = MX25L_NV_CHECKSUM_LEN;
		m_checksum_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
		m_src_buff.next = &m_checksum_buff;
	}

	if ( ( dest_address & ~MX25L_EPAGE_ALIGNMENT_MASK ) != 0U )		// recovery_address !=
																	// recovery_page_address )
	{
		m_begin_rec_buff.data = reinterpret_cast<uint8_t*>( nullptr );		// (uint8_t*)(
																			// recovery_address &
																			// MX25L_EPAGE_ALIGNMENT_MASK
																			// );
		m_begin_rec_buff.length = dest_address & ~MX25L_EPAGE_ALIGNMENT_MASK;
		m_begin_rec_buff.next = m_start_buff;
		m_start_buff = &m_begin_rec_buff;
	}

	if ( ( ( dest_address + total_length ) & ~MX25L_EPAGE_ALIGNMENT_MASK ) != 0U )		// ( recovery_address
																						// + total_length
																						// ) !=
																						// end_page_address
																						// )
	{
		m_end_rec_buff.data = reinterpret_cast<uint8_t*>( nullptr );	// (uint8_t*)(
																		// recovery_address +
																		// total_length );
		m_end_rec_buff.length = m_chip_cfg->erase_page_size
			- ( ( dest_address + total_length ) & ~MX25L_EPAGE_ALIGNMENT_MASK );	// end_page_address
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

	Write_Buff( m_start_buff, ( dest_address & MX25L_EPAGE_ALIGNMENT_MASK ),
				( recovery_address & MX25L_EPAGE_ALIGNMENT_MASK ), use_protection );

	Pop_TGINT();

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool MX25L::Write_Buff( BUFF_STRUCT_TD* buff_list, uint32_t dest_page,
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
		if ( ( dest_page & ~MX25L_EPAGE_ALIGNMENT_MASK ) == 0U )
		{
			// If we are using the scratch space we need to move the page from scratch to the
			// dest area.
			// We skip this operation if this is the first time in here.
			if ( use_protection == false )
			{
				source_page = MX25L_SCRATCH_EPAGE_ADDRESS;
				Erase_Page( source_page, MX25L_ERASE_4K_PAGE_OPCODE );		// We need to erase the page we took the
																			// data from so
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
			Erase_Page( dest_page, MX25L_ERASE_4K_PAGE_OPCODE );
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
NV_Ctrl::nv_status_t MX25L::Write_Now( const uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t ret_status = NV_Ctrl::BUSY_ERROR;
	uint8_t read_status = 0U;

	Read_Status( &read_status );
	if ( !( BF_Lib::Bit::Test( read_status, MX25L_STATUS_BUSY_BIT ) ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( MX25L_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.
		// Delay_Chip_Select_Change()
		do
		{
			Read_Status( &read_status );
		} while ( !( BF_Lib::Bit::Test( read_status, MX25L_STATUS_WRITES_ENABLED_LATCH_BIT ) ) );

		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( MX25L_WRITE_MEMORY_ADD_DATA_OPCODE, address );

		for ( uint_fast16_t i = 0U; i < length; i++ )
		{
			m_spi_ctrl->TX_Byte( data[i] );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
		}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		ret_status = NV_Ctrl::SUCCESS;
	}

	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t MX25L::Erase( uint32_t address, NV_CTRL_LENGTH_TD length,
								   uint8_t erase_data,
								   bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	m_erase_val = erase_data;
	return_status = Erase_Page( address, MX25L_ERASE_4K_PAGE_OPCODE );
	if ( NV_Ctrl::SUCCESS == return_status )
	{
		return ( return_status );
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to erase from Address: %X, Length: %u, ErrorStatus: %d",
					   address, length, return_status );
		return ( return_status );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t MX25L::Erase_Page( uint32_t address, uint8_t eraseCmd )
{
	NV_Ctrl::nv_status_t ret_status = NV_Ctrl::BUSY_ERROR;
	uint8_t read_status = 0U;

	Read_Status( &read_status );
	if ( !( BF_Lib::Bit::Test( read_status, MX25L_STATUS_BUSY_BIT ) ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( MX25L_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
		// Delay_Chip_Select_Change()
		// can't write until the write enable bit has been latched.
		do
		{
			Read_Status( &read_status );
		} while ( !( BF_Lib::Bit::Test( read_status, MX25L_STATUS_WRITES_ENABLED_LATCH_BIT ) ) );

		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( eraseCmd, address );

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		ret_status = NV_Ctrl::SUCCESS;
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t MX25L::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					MX25L_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool MX25L::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
					  bool use_protection )
{
	bool good_data = false;

	BF_Lib::SPLIT_UINT16 checksum;
	BF_Lib::SPLIT_UINT16 read_checksum;

	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( MX25L_READ_MEMORY_ADD_DATA_OPCODE, address );
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

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool MX25L::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length,
						bool use_protection )
{
	bool good_data = true;
	uint8_t read_data;

	BF_Lib::SPLIT_UINT16 checksum;

	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( MX25L_READ_MEMORY_ADD_DATA_OPCODE, address );
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

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void MX25L::Send_Data_Command( uint8_t command, uint32_t address )
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
bool MX25L::Read_Status( uint8_t* status )
{
	bool success = false;

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( MX25L_READ_STATUS_REGISTER_OPCODE );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	*status = m_spi_ctrl->RX_Byte();

	success = true;

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool MX25L::Read_Device_ID( uint8_t* device_id_dest )
{
	bool success;

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( MX25L_READ_DEVICE_ID_OPCODE );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	device_id_dest[0] = m_spi_ctrl->RX_Byte();
	device_id_dest[1] = m_spi_ctrl->RX_Byte();
	device_id_dest[2] = m_spi_ctrl->RX_Byte();
	device_id_dest[3] = m_spi_ctrl->RX_Byte();

	success = true;

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool MX25L::Write_Status( uint8_t status )
{
	bool success;

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( MX25L_SET_WRITE_ENABLE_LATCH_OPCODE );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	// An edge has to be sent to the Chip select pin on a write instruction.
	// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
	Delay_Chip_Select_Change()

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( MX25L_WRITE_STATUS_REGISTER_OPCODE );
	while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
	{}
	m_spi_ctrl->TX_Byte( status );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	success = true;

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool MX25L::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						 bool protection ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	if ( protection == true )
	{
		length += MX25L_NV_CHECKSUM_LEN;
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
void MX25L::Serial_Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{

	( ( MX25L* )param )->Serial_Flash_Async_Task();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void MX25L::Serial_Flash_Async_Task( void )
{
	nv_status_t result = BUSY_ERROR;
	uint8_t status = 0U;

	if ( true == m_serial_flash_busy )
	{
		Read_Status( &status );
		if ( !( BF_Lib::Bit::Test( status, MX25L_STATUS_BUSY_BIT ) ) )
		{
			if ( BF_Lib::Bit::Test( status, MX25L_STATUS_ERROR_BIT ) )
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
void MX25L::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_cback_param = func_param;
	m_call_back_ptr = func_callback;
}

}
