/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Mem_Check.h"
#include "CRC16.h"
#include "Babelfish_Assert.h"
namespace BF_Misc
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Mem_Check::Mem_Check( uint8_t const* start_address, uint32_t length,
					  uint16_t const* crc_address, uint16_t bytes_per_pass ) :
	m_start_address( start_address ),
	m_current_address( start_address ),
	m_length( length ),
	m_crc_address( crc_address ),
	m_current_crc( INIT_CRC_VALUE_FOR_FLASH ),
	m_bytes_per_pass( bytes_per_pass )
{
	new CR_Tasker( &Checker_Task_Static,
				   reinterpret_cast<CR_TASKER_PARAM>( this ), "Mem Check" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Mem_Check::~Mem_Check( void )
{
	m_start_address = reinterpret_cast<uint8_t*>( nullptr );
	m_current_address = reinterpret_cast<uint8_t*>( nullptr );
	m_crc_address = reinterpret_cast<uint16_t*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Mem_Check::Immediate_Check( uint8_t const* start_address, uint32_t length,
								 uint16_t const* crc_address )
{
	return ( BF_Lib::CRC16::Calc_On_String( start_address, length,
											MEM_CHECK_CRC_INIT ) == *crc_address );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Mem_Check::Checker_Task( void )
{
	uint16_t pass_counter = 0U;
	uint8_t const* end_address = m_start_address + m_length;

	while ( ( pass_counter < SINGLE_PASS_CHECK_SIZE ) && ( m_current_address < end_address ) )
	{
		m_current_crc =
			BF_Lib::CRC16::Calc_On_Byte( *m_current_address, m_current_crc );
		pass_counter++;
		m_current_address++;
	}

	if ( m_current_address == end_address )
	{
		BF_ASSERT( m_current_crc == *m_crc_address );
		m_current_crc = INIT_CRC_VALUE_FOR_FLASH;
		m_current_address = m_start_address;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Mem_Check::Blank_Check( uint8_t const* start_addr, uint8_t const* end_addr,
							   uint8_t blank_check_value )
{
	bool_t ret_val = true;
	uint8_t const* start_address = start_addr;

	if ( end_addr > start_addr )
	{
		do
		{
			if ( ( uint8_t )*start_address++ != blank_check_value )
			{
				ret_val = false;
				start_address = end_addr;
			}
		} while ( start_address < end_addr );
	}
	else
	{
		ret_val = false;
	}
	return ( ret_val );
}

}
