/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
 * casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts
 * are required for all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */
#include "Includes.h"
#include "uC_Flash.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// #define DO_NOT_CHANGE_MEM
static const uint32_t FLASH_KEY1 = 0x45670123U;
static const uint32_t FLASH_KEY2 = 0xCDEF89ABU;

static const uint32_t CR_PSIZE_MASK = 0xFFFFFCFFU;
static const uint32_t SECTOR_MASK = 0xFFFFFF07U;

static const uint8_t FLASH_ERASED_VALUE = 0xFFU;


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
uC_Flash::uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FLASH_TypeDef* flash_ctrl,
					bool_t synchronus_erase, cback_func_t cback_func, cback_param_t param,
					bool_t erase_on_boundary ) :
	NV_Ctrl(),
	m_chip_cfg( reinterpret_cast<uC_FLASH_CHIP_CONFIG_TD const*>( chip_config ) ),
	m_flash_ctrl( reinterpret_cast<FLASH_TypeDef*>( flash_ctrl ) ),
	m_erase_val( 0U ),
	m_synchronous( synchronus_erase ),
	m_call_back_ptr( cback_func ),
	m_cback_param( param ),
	m_flash_busy( false ),
	m_erase_on_boundary( erase_on_boundary )
{
	m_flash_ctrl->SR |= FLASH_PGERR;

	if ( false == m_synchronous )
	{
		BF_ASSERT( NULL != cback_func );

		new CR_Tasker( &Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
					   CR_TASKER_IDLE_PRIORITY, "Flash_Routine" );

	}
	// coverity[leaked_storage]
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
	uint32_t page_mask = 0U;
	bool_t erased_area = true;
	bool_t erase_status = true;
	uC_FLASH_STATUS_EN flash_status;

	if ( ( true == erased_area ) || ( m_erase_on_boundary == true ) )
	{
		Unlock_Flash();
		Wait_For_Complete();
		while ( ( length > 0U ) && ( return_val == true ) )
		{
			if ( m_erase_on_boundary == true )
			{
				page_mask = Get_Page_Mask( dest_address );
				// If we are on the page boundary we erase the current page.
				if ( ( ( dest_address & ~page_mask ) == 0U ) ||
					 ( *( uint8_t* )dest_address != FLASH_ERASED_VALUE ) )
				{
					// We need to erase the page we took the data from so we can write it back.
					erase_status = Erase_Page( dest_address );

					if ( erase_status == false )
					{
						/* Erase failure */
						return_val = false;
						length = 0U;
					}
					else
					{
						Unlock_Flash();
					}
				}
			}
			if ( erase_status == true )
			{
				/* We are going to program a byte. */
				m_flash_ctrl->CR &= CR_PSIZE_MASK;
				flash_status = uC_FLASH_COMPLETE;

				if ( ( ( length / 4U ) != 0U ) && ( ( dest_address % 4U ) == 0U ) )
				{
					if ( *( uint32_t* )dest_address != *( ( uint32_t* )data ) )
					{
#ifndef DO_NOT_CHANGE_MEM
						/* Write 4 bytes */
						m_flash_ctrl->CR |= ( FLASH_CR_PG | FLASH_CR_PSIZE_1 );
						Write_u32( dest_address, *( ( uint32_t* )data ) );
						flash_status = Wait_For_Complete();
#endif
					}
					if ( data != &m_erase_val )
					{
						data += 4U;
					}
					dest_address += 4U;
					length -= 4U;
				}
				else if ( ( length & 0x02U ) && ( ( dest_address % 2U ) == 0U ) )
				{
					if ( *( uint16_t* )dest_address != *( ( uint16_t* )data ) )
					{
#ifndef DO_NOT_CHANGE_MEM
						/* Write 2 bytes */
						m_flash_ctrl->CR |= ( FLASH_CR_PSIZE_0 | FLASH_CR_PG );
						Write_u16( dest_address, *( ( uint16_t* )data ) );
						flash_status = Wait_For_Complete();
#endif
					}
					if ( data != &m_erase_val )
					{
						data += 2U;
					}
					dest_address += 2U;
					length -= 2U;
				}
				else if ( length != 0U )
				{
					if ( *( uint8_t* )dest_address != *( ( uint8_t* )data ) )
					{
#ifndef DO_NOT_CHANGE_MEM
						/* Write 1 byte */
						m_flash_ctrl->CR |= FLASH_CR_PG;
						Write_u8( dest_address, *( ( uint8_t* )data ) );
						flash_status = Wait_For_Complete();
#endif
					}
					if ( data != &m_erase_val )
					{
						data += 1U;
					}
					dest_address++;
					length--;
				}
				else
				{
					/*Misra Compliance*/
				}

				if ( flash_status != uC_FLASH_COMPLETE )
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
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, NV_CTRL_LENGTH_TD length,
									  uint8_t erase_data, bool_t protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool_t erase_status = false;

	m_erase_val = erase_data;

	// return ( Write( &m_erase_val, address, length, protected_data ) );
	erase_status = Erase_Page( address );
	if ( true == erase_status )
	{
		return_status = NV_Ctrl::SUCCESS;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_Flash::Erase_Page( uint32_t address )
{
	uC_FLASH_STATUS_EN status;
	uint32_t sector_num;

	m_flash_busy = false;
	status = Wait_For_Complete();

	if ( status == uC_FLASH_COMPLETE )
	{
		sector_num = Get_Sector( address );

#ifndef DO_NOT_CHANGE_MEM
		Unlock_Flash();
		/* if the previous operation is completed, proceed to erase the sector */
		m_flash_ctrl->CR &= CR_PSIZE_MASK;
		m_flash_ctrl->CR |= FLASH_CR_PSIZE_1;		// Set it to 32bit so we can erase quicker.
		m_flash_ctrl->CR &= SECTOR_MASK;
		m_flash_ctrl->CR |= FLASH_CR_SER | ( sector_num * FLASH_CR_SNB_0 );
		m_flash_ctrl->CR |= FLASH_CR_STRT;
		if ( true == m_synchronous )
		{
			status = Wait_For_Complete();
			/* if the erase operation is completed, disable the SER Bit */
			m_flash_ctrl->CR &= ( ~FLASH_CR_SER );
			m_flash_ctrl->CR &= SECTOR_MASK;
			Lock_Flash();
		}
		else
		{
			m_flash_busy = true;
			status = uC_FLASH_BUSY;		/* Asynchronous transfer so we will set status as Flash busy
										 */
		}
#endif
	}

	return ( status == uC_FLASH_COMPLETE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_FLASH_STATUS_EN uC_Flash::Wait_For_Complete( void ) const
{
	uC_FLASH_STATUS_EN status;

	Push_TGINT();
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
uC_FLASH_STATUS_EN uC_Flash::Get_Status( void ) const
{
	uC_FLASH_STATUS_EN status = uC_FLASH_COMPLETE;
	uint32_t status_reg;

	status_reg = m_flash_ctrl->SR;
	/* Check for memory operation */
	if ( ( status_reg & FLASH_SR_BSY ) == FLASH_SR_BSY )
	{
		status = uC_FLASH_BUSY;
	}
	else
	{
		/* Check for errors */
		if ( status_reg & FLASH_PGERR )
		{
			status = uC_FLASH_ERROR_PG;
		}
		else
		{
			if ( ( status_reg & FLASH_SR_WRPERR ) != 0U )
			{
				status = uC_FLASH_ERROR_WRP;
			}
			else
			{
				status = uC_FLASH_COMPLETE;
			}
		}
		/* Clear the Error Flags by writing 1 to it*/
		m_flash_ctrl->SR |= FLASH_PGERR;
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
bool_t uC_Flash::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length ) const
{
	BF_Lib::Copy_String( data, ( uint8_t* )address, length );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_Flash::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length ) const
{
	bool_t good_data = true;
	uint8_t* src_data;

	src_data = ( uint8_t* )address;

	while ( ( length > 0U ) && ( good_data == true ) )
	{
		length--;
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
bool_t uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool_t range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	end_address = m_chip_cfg->end_address;

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
void uC_Flash::Unlock_Flash( void )
{
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 2-13-3
	 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 2-13-3, Unsigned octal and
	 * hexadecimal literals require a 'U' suffix
	 * @n Justification: The ST supplied includes where this #define resides shall not be
	 * changed.
	 */
	/*lint -e{1960} */
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
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 2-13-3
	 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 2-13-3, Unsigned octal and
	 * hexadecimal literals require a 'U' suffix
	 * @n Justification: The ST supplied includes where this #define resides shall not be
	 * changed.
	 */
	/*lint -e{1960} */
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

	test_address = INTERNAL_FLASH_START_ADDRESS +
		INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	while ( ( address >= test_address ) &&
			( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size != 0U ) )
	{
		page_index++;
		test_address += INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	}

	return ( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_no );
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
							   NV_CTRL_LENGTH_TD length, bool include_erase_time ) const
{
	uint32_t write_time;
	uint32_t byte_write_time;
	uint32_t page_index;
	uint32_t test_address;
	uint32_t erased_values;	// To count the number of values that are actually erased and don't need

	// programming.

	write_time = 0U;
	page_index = 0U;
	test_address = INTERNAL_FLASH_START_ADDRESS +
		INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
	while ( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size != 0U )
	{
		if ( ( address <= test_address ) && ( ( address + length ) > ( test_address + 1U ) ) )
		{
			write_time += ( SECTOR_ERASE_TIME_BASELINE_MS *
							( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size /
							  INTERNAL_FLASH_PAGE_SIZES[0].sector_size ) );
		}
		page_index++;
		test_address += INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
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

		byte_write_time = BYTE_WRITE_TIME_US * ( length - erased_values );
		if ( byte_write_time < MICROSECONDS_ADJUSTMENT )
		{
			byte_write_time = MICROSECONDS_ADJUSTMENT;
		}
		write_time += Round_Div( byte_write_time, MICROSECONDS_ADJUSTMENT );	// Because the byte
																				// write
																				// time is in
																				// microseconds we
																				// have
																				// to adjust it
																				// down.
	}
	else
	{
		// because we will erase the page we are on as well as the next page if we get there.
		write_time += ( SECTOR_ERASE_TIME_BASELINE_MS
						* ( Get_Page_Size( address ) /
							INTERNAL_FLASH_PAGE_SIZES[0].sector_size ) );
	}

	return ( write_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{

	( ( uC_Flash* )param )->Flash_Async_Task();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Flash_Async_Task( void )
{
	nv_status_t result;

	if ( true == m_flash_busy )
	{
		uC_FLASH_STATUS_EN status = Get_Status();
		if ( status != uC_FLASH_BUSY )
		{
			m_flash_ctrl->CR &= ( ~FLASH_CR_SER );
			m_flash_ctrl->CR &= SECTOR_MASK;
			m_flash_busy = false;
			// Check for Error
			if ( FLASH->SR & FLASH_PGERR )
			{
				// Reset Error Flags
				FLASH->SR |= FLASH_PGERR;
			}
			Lock_Flash();

			if ( status == uC_FLASH_COMPLETE )
			{
				result = SUCCESS;
			}
			else
			{
				result = DATA_ERROR;
			}
			m_call_back_ptr( m_cback_param, result );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_cback_param = func_param;
	m_call_back_ptr = func_callback;
}

}

/*lint +e1924 */
