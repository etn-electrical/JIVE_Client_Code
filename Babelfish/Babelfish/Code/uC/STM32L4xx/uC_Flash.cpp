/*
 *****************************************************************************************
 *
 *		Copyright 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Flash.h"
#include "Exception.h"
#include "StdLib_MV.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// #define DO_NOT_CHANGE_MEM

#define FLASH_KEY1               ( static_cast<uint32_t>( 0x45670123U ) )
#define FLASH_KEY2               ( static_cast<uint32_t>( 0xCDEF89ABU ) )

#define BANK_MASK                   ( ( uint32_t )0xFFFFF7FF )
#define PAGE_MASK                   ( ( uint32_t )0xFFFFF807 )
static const uint32_t BANK2_STRTADDR = 0x08040000U;
#define M64( adr )  ( *( ( volatile uint64_t* ) ( adr ) ) )

#define uC_FLASH_PAGE_ALIGNMENT_MASK        ~( m_chip_cfg->page_size - 1 )
#define uC_FLASH_SCRATCH_PAGE_ADDRESS       ( m_chip_cfg->end_address - ( m_chip_cfg->page_size * 2 ) )
#define uC_FLASH_WORD_ALIGNMENT_MASK        ~( 0x01 )

#define uC_FLASH_CR_PER_RESET               ( ( uint32_t )0x00001FFD )
#define FLASH_ERASED_VALUE                  0xFFU

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
uC_Flash::uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FLASH_TypeDef* flash_ctrl ) :
	m_erase_val( 0U )
{
	m_chip_cfg = chip_config;
	m_flash_ctrl = flash_ctrl;

	/*if( (m_flash_ctrl->OPTR & FLASH_OPTCR_nDBANK) == FLASH_OPTCR_nDBANK )
	   {
	   m_flash_ctrl->SR |= ( FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_ERSERR );
	   m_flash_ctrl->OPTKEYR = uC_BASE_OPT_KEY_1;
	   m_flash_ctrl->OPTKEYR = uC_BASE_OPT_KEY_2;
	   m_flash_ctrl->OPTCR &= ~( FLASH_OPTCR_OPTLOCK );
	   m_flash_ctrl->OPTCR &= ~( FLASH_OPTCR_nDBANK );
	   m_flash_ctrl->OPTCR |= ( FLASH_OPTCR_OPTSTRT );
	   Wait_For_Complete();
	   m_flash_ctrl->OPTCR |= ( FLASH_OPTCR_OPTLOCK );
	   }*/
	Unlock_Flash();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									 bool_t use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool_t good_data;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		good_data = Read_Now( data, address, length );

		if ( good_data == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}
	if ( use_protection )
	{}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									  bool_t use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool_t good_data;

	if ( Check_Range( address, length ) )
	{
		good_data = Write_Now( data, address, length );
		if ( good_data == true )
		{
			good_data = Check_Data( data, address, length );
		}

		if ( good_data == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}

	if ( use_protection )
	{}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_Flash::Write_Now( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length )
{
	bool_t return_val = true;
	uint32_t page_mask;
	bool_t erased_area = true;
	bool_t erase_status = false;

	if ( true == erased_area )
	{
		Unlock_Flash();
		Wait_For_Complete();
		while ( ( length > 0U ) && ( return_val == true ) )
		{
			uC_FLASH_STATUS_EN flash_status = uC_FLASH_TIMEOUT;
			page_mask = Get_Page_Mask( dest_address );

			erase_status = true;
			// If we are on the page boundary we erase the current page.
			if ( ( ( dest_address & ~page_mask ) == 0U ) ||
				 ( *( uint8_t* )dest_address != FLASH_ERASED_VALUE ) )
			{
				erase_status = Erase_Page( dest_address );	// We need to erase the page we took the data from so we can
															// write it back.
				Wait_For_Complete();
				if ( erase_status == false )
				{
					/* Erase failure */
					return_val = false;
					break;
				}
			}
			Unlock_Flash();
			if ( erase_status == true )
			{
				if ( *( uint64_t* )dest_address != *( ( uint64_t* )data ) )
				{
#ifndef DO_NOT_CHANGE_MEM
					m_flash_ctrl->CR |= FLASH_CR_PG;/* Write 1 byte */

					uint8_t test_count = 0U;
					while ( ( flash_status != uC_FLASH_COMPLETE ) && ( test_count < 2 ) )
					{
						M64( dest_address ) = *( ( uint64_t* )data );

						flash_status = Wait_For_Complete();
						test_count++;
					}
#endif
				}
				if ( data != &m_erase_val )
				{
					data += 8U;
				}
				dest_address = dest_address + 8;
				if ( length < 8 )
				{
					length = 0U;
				}
				else
				{
					length -= 8;
				}

				if ( ( flash_status != uC_FLASH_COMPLETE ) && ( flash_status != uC_FLASH_TIMEOUT ) )
				{
					return_val = false;
				}
				m_flash_ctrl->CR &= ( ~FLASH_CR_PG );
			}
		}
		Lock_Flash();
	}
	else
	{
		return_val = false;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read_Checksum( uint16_t* checksum_dest,
											  NV_CTRL_ADDRESS_TD address,
											  NV_CTRL_LENGTH_TD length )
{
	*checksum_dest = 0U;
	if ( address == length )
	{}
	// return ( NV_Ctrl::SUCCESS );
	return ( NV_Ctrl::DATA_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, NV_CTRL_LENGTH_TD length, uint8_t erase_data,
									  bool_t protected_data )
{
	m_erase_val = erase_data;

	return ( Write( &m_erase_val, address, length, protected_data ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_Flash::Erase_Page( uint32_t address )
{
	uint8_t status;
	uint32_t sector_num;
	uint8_t erase_retry = 0U;

	do
	{
		erase_retry++;
		status = Wait_For_Complete();

		Unlock_Flash();
		if ( status == uC_FLASH_COMPLETE )
		{
			sector_num = Get_Sector( address );
#ifndef DO_NOT_CHANGE_MEM
			if ( address < BANK2_STRTADDR )
			{
				/* if the previous operation is completed, proceed to erase the sector */
				// m_flash_ctrl->CR &= CR_PSIZE_MASK;
				// m_flash_ctrl->CR |= FLASH_CR_PSIZE_1;   //Set it to 32bit so we can erase
				// quicker.
				m_flash_ctrl->CR &= BANK_MASK;
				// m_flash_ctrl->CR &= PAGE_MASK;
				m_flash_ctrl->CR |= sector_num << FLASH_CR_PNB_Pos;
				m_flash_ctrl->CR |= FLASH_CR_PER;
				m_flash_ctrl->CR |= FLASH_CR_STRT;
				status = Wait_For_Complete();
				/* if the erase operation is completed, disable the SER Bit */
				m_flash_ctrl->CR &= ( ~FLASH_CR_PER );
				m_flash_ctrl->CR &= PAGE_MASK;
				Lock_Flash();
			}
			else
			{
				/* if the previous operation is completed, proceed to erase the sector */
				// m_flash_ctrl->CR |= (~BANK_MASK);
				// m_flash_ctrl->CR &= PAGE_MASK;
				// sector_num = sector_num - 128;
				m_flash_ctrl->CR |= sector_num << FLASH_CR_PNB_Pos;
				m_flash_ctrl->CR |= FLASH_CR_PER;
				m_flash_ctrl->CR |= FLASH_CR_STRT;
				status = Wait_For_Complete();
				/* if the erase operation is completed, disable the SER Bit */
				m_flash_ctrl->CR &= ( ~FLASH_CR_PER );
				m_flash_ctrl->CR &= PAGE_MASK;
				Lock_Flash();
			}
#endif
		}
	} while ( ( status != uC_FLASH_COMPLETE ) && ( erase_retry < 2 ) );

	return ( status == uC_FLASH_COMPLETE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_FLASH_STATUS_EN uC_Flash::Wait_For_Complete( void )
{
	uC_FLASH_STATUS_EN status;

	Push_TGINT();
	__DSB();
	/* Wait for a Flash operation to complete or a TIMEOUT to occur */
	do
	{
		status = Get_Status();
		uC_Watchdog::Force_Feed_Dog();
	} while ( status == uC_FLASH_BUSY );
	Pop_TGINT();
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_FLASH_STATUS_EN uC_Flash::Get_Status( void )
{
	uC_FLASH_STATUS_EN status = uC_FLASH_COMPLETE;

	if ( ( m_flash_ctrl->SR & FLASH_SR_BSY ) == FLASH_SR_BSY )
	{
		status = uC_FLASH_BUSY;
	}
	else
	{
		volatile uint32_t SR;
		SR = m_flash_ctrl->SR;
		if ( ( SR & ( FLASH_SR_PGSERR
					  | FLASH_SR_PGAERR | FLASH_SR_WRPERR ) ) != 0 )
		{
			status = uC_FLASH_ERROR_PG;
			if ( ( SR & FLASH_SR_PGSERR ) != 0 )
			{
				m_flash_ctrl->SR |= FLASH_SR_PGSERR;
			}
			if ( ( SR & FLASH_SR_PGAERR ) != 0 )
			{
				m_flash_ctrl->SR |= FLASH_SR_PGAERR;
			}
			if ( ( SR & FLASH_SR_WRPERR ) != 0U )
			{
				status = uC_FLASH_ERROR_WRP;
				m_flash_ctrl->SR |= FLASH_SR_WRPERR;
			}
		}
		else
		{
			status = uC_FLASH_COMPLETE;
		}
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					uC_FLASH_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_Flash::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	BF_Lib::Copy_String( data, ( uint8_t* )address, length );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_Flash::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length )
{
	bool_t good_data = true;
	uint8_t* src_data;

	src_data = ( uint8_t* )address;

	while ( ( length-- ) && ( good_data == true ) )
	{
		if ( *check_data != *src_data )
		{
			good_data = false;
		}
		if ( check_data != &m_erase_val )
		{
			check_data++;
		}
		src_data++;
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	bool_t range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	end_address = m_chip_cfg->end_address;

	if ( ( address >= m_chip_cfg->start_address ) && ( ( address + length ) <= end_address ) )
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
void uC_Flash::Unlock_Flash( void )
{
	if ( ( m_flash_ctrl->CR & FLASH_CR_LOCK ) != 0U )
	{
		m_flash_ctrl->KEYR = FLASH_KEY1;
		m_flash_ctrl->KEYR = FLASH_KEY2;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Lock_Flash( void )
{
	m_flash_ctrl->CR |= FLASH_CR_LOCK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Get_Sector( uint32_t address ) const
{
	uint32_t page_index = 0U;
	uint32_t test_address;

	test_address = m_chip_cfg->page_index_start + m_chip_cfg->page_sizes[page_index];
	while ( ( address >= test_address ) && ( m_chip_cfg->page_sizes[page_index] != 0 ) )
	{
		page_index++;
		test_address += m_chip_cfg->page_sizes[page_index];
	}

	return ( page_index );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define MICROSECONDS_ADJUSTMENT         1000U
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							   NV_CTRL_LENGTH_TD length, bool_t include_erase_time )const
{
	uint32_t write_time;
	uint32_t byte_write_time;
	uint32_t page_index;
	uint32_t test_address;
	uint32_t erased_values;		// To count the number of values that are actually erased and don't need programming.

	write_time = 0U;
	page_index = 0U;
	test_address = m_chip_cfg->page_index_start + m_chip_cfg->page_sizes[page_index];
	while ( m_chip_cfg->page_sizes[page_index] != 0U )
	{
		if ( ( address <= test_address ) && ( ( address + length ) > test_address ) )
		{
			write_time += ( m_chip_cfg->page_erase_time *
							( m_chip_cfg->page_sizes[page_index] / m_chip_cfg->page_sizes[0] ) );
		}
		page_index++;
		test_address += m_chip_cfg->page_sizes[page_index];
	}

	// Check to see if we are calculating the erase time or a write time.
	if ( data != NULL )
	{
		erased_values = 0U;
		for ( uint32_t i = 0U; i < length; i++ )
		{
			if ( data[i] == FLASH_ERASED_VALUE )
			{
				erased_values++;
			}
		}

		byte_write_time = m_chip_cfg->byte_write_time * ( length - erased_values );
		if ( byte_write_time < MICROSECONDS_ADJUSTMENT )
		{
			byte_write_time = MICROSECONDS_ADJUSTMENT;
		}
		write_time += Round_Div( byte_write_time, MICROSECONDS_ADJUSTMENT );		// Because the byte write time is in
																					// microseconds we have to adjust it
																					// down.
	}
	else
	{
		// because we will erase the page we are on as well as the next page if we get there.
		write_time += ( m_chip_cfg->page_erase_time * ( Get_Page_Size( address ) / m_chip_cfg->page_sizes[0] ) );
	}

	return ( write_time );
}

}
