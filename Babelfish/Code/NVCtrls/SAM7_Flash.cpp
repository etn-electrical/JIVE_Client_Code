/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "SAM7_Flash.h"
#include "OS_Tasker.h"
#include "uC_Base.h"
#include "NV_Ctrl_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define SAM7_FLASH_CHECKSUM_LENGTH          2
#define SAM7_FLASH_WR_COMPLETE_COUNT        2

#define SAM7_FLASH_COMMAND_KEY              0x5A000000		// This is required for all writes.

#define INIT_CRC_VALUE_FOR_SAM7_FLASH       0

#define SAM7_FLASH_ERASE_VAL                0xFF

// Page Calculation
#define SAM7_FLASH_PAGE_SIZE            ( UINT32 )AT91C_IFLASH_PAGE_SIZE
#define SAM7_FLASH_PAGE_SIZE_MASK       ( SAM7_FLASH_PAGE_SIZE - 1 )

#define Get_Page_Count( size )          ( ( size + SAM7_FLASH_PAGE_SIZE_MASK ) / SAM7_FLASH_PAGE_SIZE )
#define Get_Page_Num( address )         ( ( ( UINT32 )address ) / SAM7_FLASH_PAGE_SIZE )

// This is basically the beginning and end of codespace with the computed CRC sitting
// at the end of the code.  The Code CRC check routine runs through flash memory and
// calculates the crc by running the full length of memory and checking for a valid CRC.
#define FLASH_START_ADDRESS_AFTER_REMAP     0x100000
#define APP_CODE_SIZE                       ( uC_App_Fw_End_Address() - uC_App_Fw_Start_Address() )

#define APP_CODE_START_ADDRESS              uC_App_Fw_Start_Address()	// ( FLASH_START_ADDRESS_AFTER_REMAP +
																		// BOOTLOADER_CODE_SIZE )
																		// 	//BOOTLOADER_CODE_END_ADDRESS
#define APP_CODE_END_ADDRESS                uC_App_Fw_End_Address()	// ( FLASH_START_ADDRESS_AFTER_REMAP + (
																	// TOTAL_FLASH_SIZE - BOOTLOADER_CODE_SIZE )
																	// )//0x100FFFF

#define SAM7_FLASH_PAGE_ALIGNMENT_MASK      ( ~( SAM7_FLASH_PAGE_SIZE_MASK ) )
#define SAM7_FLASH_WORD_ALIGNMENT_MASK      ( ~( sizeof( uint_fast8_t ) - 1 ) )

/*
 *****************************************************************************************
 *		Macros
 *****************************************************************************************
 */
#define SAM7_Flash_Busy( status_reg )   ( ( status_reg & AT91C_MC_FRDY ) == 0 )

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
SAM7_Flash::SAM7_Flash( NV_CTRL_ADDRESS_TD start_address,
						NV_CTRL_LENGTH_TD size, uint8_t flash_ctrl_id )
{
	m_efc = AT91C_BASE_MC;
	m_start_address = start_address;
	m_size = size;
	m_mirror_address = start_address + ( ( ( start_address + size ) - start_address ) >> DIV_BY_2 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SAM7_Flash::Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									   NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status;
	bool good_data;
	uint8_t* read_ptr;

	if ( Check_Range( address, length, use_protection ) && ( length != 0 ) )
	{
		read_ptr = ( uint8_t* )( m_start_address + address );

		if ( length != 0 )
		{
			good_data = Read_Now( data, read_ptr, length, use_protection );

			// if the data didn't read right, go check the backup location to see if
			// our good data is there.
			if ( ( !good_data ) && ( use_protection == true ) )
			{
				read_ptr = ( uint8_t* )( m_mirror_address + address );
				good_data = Read_Now( data, read_ptr, length, use_protection );
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
NV_Ctrl::nv_status_t SAM7_Flash::Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
										NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status;
	BF_Lib::SPLIT_UINT16 checksum;
	uint8_t* src_ptr;
	uint8_t* dest_ptr;
	uint8_t write_count;
	NV_CTRL_LENGTH_TD write_length;

	if ( Check_Range( address, length, use_protection ) && ( length != 0 ) )
	{
		dest_ptr = ( uint8_t* )( address + m_start_address );

		write_count = 0U;
		while ( write_count < SAM7_FLASH_WR_COMPLETE_COUNT )
		{
			write_length = 0U;
			src_ptr = data;
			checksum.u16 = 0U;
			while ( write_length < length )
			{
				checksum.u16 += *src_ptr;
				if ( data == NULL )	// This means that they want to do an erase.
				{
					Write_Byte( SAM7_FLASH_ERASE_VAL, dest_ptr, ( write_length == 0U ),
								( ( write_length == ( length - 1U ) ) && ( use_protection == false ) ) );
				}
				else
				{
					Write_Byte( *src_ptr, dest_ptr, ( write_length == 0U ),
								( ( write_length == ( length - 1U ) ) && ( use_protection == false ) ) );

				}
				dest_ptr++;
				src_ptr++;
				write_length++;
			}
			if ( use_protection == true )
			{
				Write_Byte( checksum.u8[0], dest_ptr, false, false );
				dest_ptr++;
				Write_Byte( checksum.u8[1], dest_ptr, false, true );

				dest_ptr = ( uint8_t* )( address + m_mirror_address );
				write_count++;
			}
			else
			{
				write_count = SAM7_FLASH_WR_COMPLETE_COUNT;
			}
		}

		return_status = NV_Ctrl::SUCCESS;
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
NV_Ctrl::nv_status_t SAM7_Flash::Erase( NV_CTRL_ADDRESS_TD address,
										NV_CTRL_LENGTH_TD length, uint8_t erase_data, bool protected_data )
{
	return ( Write( NULL, address, length, protected_data ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t SAM7_Flash::Erase_All( void )
{
	return ( Erase( m_start_address, m_size, 0, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SAM7_Flash::Write_Byte( uint8_t data, uint8_t* dest_byte_ptr, bool start, bool finish )
{
	volatile uint32_t* recovery_ptr;
	uint8_t* temp_byte_ptr;
	uint_fast8_t* dest_word_ptr;

	if ( ( start == true ) || ( ( ( uint32_t )dest_byte_ptr ) & SAM7_FLASH_WORD_ALIGNMENT_MASK ) )
	{
		dest_word_ptr = ( uint_fast8_t* )( ( ( uint32_t )dest_byte_ptr ) & ~SAM7_FLASH_WORD_ALIGNMENT_MASK );
		m_temp_word = *dest_word_ptr;

		recovery_ptr = dest_word_ptr;
		while ( recovery_ptr < dest_word_ptr )
		{
			*recovery_ptr = *recovery_ptr;
			recovery_ptr++;
		}
	}

	temp_byte_ptr = ( uint8_t* )&m_temp_word;
	temp_byte_ptr += ( ( ( uint32_t )dest_byte_ptr ) & SAM7_FLASH_WORD_ALIGNMENT_MASK );
	*temp_byte_ptr = data;

	if ( ( finish == true ) ||		// Should we finish the page? or
		 ( ( ( uint32_t )( dest_byte_ptr + 1 ) & SAM7_FLASH_WORD_ALIGNMENT_MASK ) == 0U ) )			// Are we crossing a
																									// word boundary?
	{
		*dest_word_ptr = m_temp_word;
		if ( ( finish == true ) ||	// Has a finish been requested? or
			 ( ( ( uint32_t )( dest_byte_ptr + 1 ) & SAM7_FLASH_PAGE_ALIGNMENT_MASK ) == 0U ) )		// Are we crossing a
																									// page boundary.
		{
			recovery_ptr = ( uint_fast8_t* )( ( uint32_t )( dest_byte_ptr + 1 ) & SAM7_FLASH_WORD_ALIGNMENT_MASK );
			while ( ( ( uint32_t )( recovery_ptr ) & ~SAM7_FLASH_PAGE_ALIGNMENT_MASK ) != 0U )
			{
				*recovery_ptr = *recovery_ptr;
				recovery_ptr++;
			}
			Write_Page_Now( Get_Page_Num( dest_byte_ptr ) );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SAM7_Flash::Read_Now( uint8_t* dest_data, uint8_t* read_ptr,
						   NV_CTRL_LENGTH_TD length, bool use_protection )
{
	bool good_data;
	BF_Lib::SPLIT_UINT16 checksum;
	BF_Lib::SPLIT_UINT16 read_checksum;

	checksum.u16 = 0;
	while ( length-- )
	{
		*dest_data = *read_ptr;
		checksum.u16 += *dest_data;
		dest_data++;
		read_ptr++;
	}

	if ( use_protection == true )
	{
		read_checksum.u8[0] = *read_ptr;
		read_ptr++;
		read_checksum.u8[1] = *read_ptr;
		read_ptr++;
		if ( read_checksum.u16 == checksum.u16 )
		{
			good_data = true;
		}
		else
		{
			good_data = false;
		}
	}
	else
	{
		good_data = true;
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
__ramfunc __arm void SAM7_Flash::Write_Page_Now( uint16_t page )
{
	Push_TGINT();

#ifdef uC_AT91SAM7SE256_USAGE
	Clr_Mask( m_efc->MC0_FMR, AT91C_MC_NEBP );		// force an erase before each program.
	m_efc->MC0_FCR = ( page << 8 ) | SAM7_FLASH_COMMAND_KEY | AT91C_MC_FCMD_START_PROG;
	while ( SAM7_Flash_Busy( m_efc->MC0_FSR ) )
	{}																	// Should probably put a timeout on here instead
																		// of just waiting.
#else

	#ifdef uC_AT91SAM7SE512_USAGE
		#error "We need to define the 512 interface.  ie it will cross the magical 256 boundary"
	#else
	Clr_Mask( m_efc->MC_FMR, AT91C_MC_NEBP );			// force an erase before each program.
	m_efc->MC_FCR = ( page << 8 ) | SAM7_FLASH_COMMAND_KEY | AT91C_MC_FCMD_START_PROG;
	while ( SAM7_Flash_Busy( m_efc->MC_FSR ) )
	{}																		// Should probably put a timeout on here
																			// instead of just waiting.
	#endif
#endif

	Pop_TGINT;		// uC_Interrupt::Pop_Int( temp_gint );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool SAM7_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, bool protection )
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address = m_start_address + m_size;

	address = address + m_start_address;

	if ( protection == true )
	{
		length += SAM7_FLASH_CHECKSUM_LENGTH;
		end_address = m_mirror_address;
	}

	if ( ( address >= m_start_address ) && ( ( address + length ) <= end_address ) )
	{
		range_good = true;
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed range check for Address: %X, Length: %u, Protection: %d",
					   address, length, use_protection );

		// If you hit this point then you have run outside of
		// the allocated nv_ram.  You likely need to check your address or increase the
		// nv_ram ram size.
		// This is used primarily for debug purposes.
		BF_ASSERT( false );
	}

	return ( range_good );
}
