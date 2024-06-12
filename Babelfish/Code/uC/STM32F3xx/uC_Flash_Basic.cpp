/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Flash_Basic.h"
#include "Exception.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// #define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /*!< FLASH Busy flag */
// #define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /*!< FLASH End of Operation flag */
// #define FLASH_FLAG_PGERR               ((uint32_t)0x00000004)  /*!< FLASH Program error flag */
// #define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /*!< FLASH Write protected error flag */
// #define FLASH_FLAG_OPTERR              ((uint32_t)0x00000001)  /*!< FLASH Option Byte error flag */
//
// #define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCA) == 0x00000000) && ((FLAG) != 0x00000000))
// #define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_EOP) || \
//                                  ((FLAG) == FLASH_FLAG_PGERR) || ((FLAG) == FLASH_FLAG_WRPRTERR) || \
// //								  ((FLAG) == FLASH_FLAG_BANK1_BSY) || ((FLAG) == FLASH_FLAG_BANK1_EOP) || \
//                                  ((FLAG) == FLASH_FLAG_BANK1_PGERR) || ((FLAG) == FLASH_FLAG_BANK1_WRPRTERR) || \
//                                  ((FLAG) == FLASH_FLAG_OPTERR))

#ifndef FLASH_KEY1
#define FLASH_KEY1               ( static_cast<uint32_t>( 0x45670123U ) )
#endif
#ifndef FLASH_KEY2
#define FLASH_KEY2               ( static_cast<uint32_t>( 0xCDEF89ABU ) )
#endif

#define MAX_NUMBER_OF_WRITES        2U
#define MAX_NUMBER_OF_READS         2U

#define uC_FLASH_NV_CHECKSUM_LEN            2U

#define uC_FLASH_PAGE_ALIGNMENT_MASK        ~( FLASH_PAGE_SIZE - 1U )
#define uC_FLASH_SCRATCH_PAGE_ADDRESS       ( m_chip_cfg->end_address - ( FLASH_PAGE_SIZE * 2U ) )
// #define uC_FLASH_WORD_ALIGNMENT_MASK		~(0x01U)

// #define uC_FLASH_CR_PER_RESET             	(static_cast<uint32_t>(0x00001FFDU))

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
					bool_t synchronus_erase, cback_func_t cback_func, cback_param_t param ) :
	NV_Ctrl(),
	m_chip_cfg( reinterpret_cast<uC_FLASH_CHIP_CONFIG_TD const*>( chip_config ) ),
	m_flash_ctrl( reinterpret_cast<FLASH_TypeDef*>( flash_ctrl ) ),
	m_erase_val( 0U ),
	m_synchronous( synchronus_erase ),
	m_call_back_ptr( cback_func ),
	m_cback_param( param ),
	m_flash_busy( false )
{
	m_flash_ctrl->SR |= FLASH_SR_PGERR;

	Unlock_Flash();
	if ( false == m_synchronous )
	{
		BF_ASSERT( NULL != cback_func );

		new CR_Tasker( &Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
					   CR_TASKER_IDLE_PRIORITY, "Flash_Routine" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									 bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;

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
									  bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;

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
bool uC_Flash::Write_Now( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length )
{
	bool_t erased_area = true;

	if ( !( length & 0x01 ) )
	{
		uint32_t counter = 0U;
		uint32_t address = dest_address;

		Push_TGINT();

		for ( counter = 0; ( ( counter < length ) && ( true == erased_area ) ); counter++ )
		{
			if ( ( *( uint8_t* )address != uC_FLASH_DEFAULT_ERASE_VAL ) )
			{
				erased_area = false;
			}
		}

		if ( true == erased_area )
		{
			// 16bit write word
			uint16_t* dest_ptr = ( uint16_t* )dest_address;
			uint16_t* src_ptr = ( uint16_t* )data;

			while ( length > 0U )
			{
				m_flash_ctrl->CR |= FLASH_CR_PG;
				*dest_ptr = *src_ptr;
				dest_ptr++;
				src_ptr++;
				length -= 2;
				Wait_For_Complete();
				m_flash_ctrl->CR &= ~FLASH_CR_PG;
			}
		}

		Pop_TGINT();
	}
	else
	{
		erased_area = false;
	}
	return ( erased_area );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, NV_CTRL_LENGTH_TD length, uint8_t erase_data,
									  bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool_t erase_status = false;

	m_erase_val = erase_data;
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
bool uC_Flash::Erase_Page( uint32_t address )
{
	uC_FLASH_STATUS_EN status;

	status = Wait_For_Complete();

	if ( status == uC_FLASH_COMPLETE )
	{
		/* if the previous operation is completed, proceed to erase the page */
		m_flash_ctrl->CR |= FLASH_CR_PER;
		m_flash_ctrl->AR = address & uC_FLASH_PAGE_ALIGNMENT_MASK;
		m_flash_ctrl->CR |= FLASH_CR_STRT;

		/* Wait for last operation to be completed */
		if ( m_synchronous == true )
		{
			status = Wait_For_Complete();
			m_flash_ctrl->CR &= ~FLASH_CR_PER;
		}
		else
		{
			m_flash_busy = true;
			status = uC_FLASH_BUSY;	/* Asynchronous transfer so we will set status as Flash busy */
		}
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

	/* Wait for a Flash operation to complete or a TIMEOUT to occur */
	do
	{
		status = Get_Status();
		uC_Watchdog::Force_Feed_Dog();
	} while ( status == uC_FLASH_BUSY );

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

	if ( ( m_flash_ctrl->SR & FLASH_SR_BSY ) == FLASH_SR_BSY )
	{
		status = uC_FLASH_BUSY;
	}
	else
	{
		volatile uint32_t SR;
		SR = m_flash_ctrl->SR;
		if ( SR & FLASH_SR_PGERR )
		{
			status = uC_FLASH_ERROR_PG;
		}
		else
		{
			if ( ( SR & FLASH_SR_WRPERR ) != 0U )
			{
				status = uC_FLASH_ERROR_WRP;
			}
			else
			{
				status = uC_FLASH_COMPLETE;
			}
		}
	}

	m_flash_ctrl->SR |= ( FLASH_SR_PGERR | FLASH_SR_WRPERR );
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length ) const
{
	BF_Lib::Copy_String( data, ( uint8_t* )address, length );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length ) const
{
	bool good_data = true;
	uint8_t* src_data;

	src_data = reinterpret_cast<uint8_t*>( address );

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
bool uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool range_good = false;
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
			m_flash_ctrl->CR &= ( ~FLASH_CR_PER );
			// m_flash_ctrl->CR &= SECTOR_MASK;
			m_flash_busy = false;
			// Check for Error
			if ( m_flash_ctrl->SR & FLASH_PGERR )
			{
				// Reset Error Flags
				m_flash_ctrl->SR |= FLASH_PGERR;
			}

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

}
