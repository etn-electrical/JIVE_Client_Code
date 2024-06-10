/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *	NOTE: This file is not tested on STM32WB55 hardware
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Flash.h"
#include "Exception.h"
#include "StdLib_MV.h"
#include "uC_Interrupt.h"
#include "string.h"
#include "NV_Ctrl_Debug.h"
#include "Timers_Lib.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// #define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /*!< FLASH Busy flag */
// #define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /*!< FLASH End of Operation
// flag */
// #define FLASH_FLAG_PGERR               ((uint32_t)0x00000004)  /*!< FLASH Program error flag
// */
// #define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /*!< FLASH Write protected
// error flag */
// #define FLASH_FLAG_OPTERR              ((uint32_t)0x00000001)  /*!< FLASH Option Byte error
// flag */
//
// #define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCA) == 0x00000000) && ((FLAG)
//! = 0x00000000))
// #define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_EOP) ||
// \
//                                  ((FLAG) == FLASH_FLAG_PGERR) || ((FLAG) ==
// FLASH_FLAG_WRPRTERR) || \
// 	//								  ((FLAG) == FLASH_FLAG_BANK1_BSY) || ((FLAG) ==
// FLASH_FLAG_BANK1_EOP) || \
//                                  ((FLAG) == FLASH_FLAG_BANK1_PGERR) || ((FLAG) ==
// FLASH_FLAG_BANK1_WRPRTERR) || \
//                                  ((FLAG) == FLASH_FLAG_OPTERR))

static const uint32_t SECTOR_MASK = 0xFFFFFF07U;

#define uC_FLASH_PAGE_ALIGNMENT_MASK        ( ~( FLASH_PAGE_SIZE - 1U ) & 0xFFFFFF )
#define uC_FLASH_SCRATCH_PAGE_ADDRESS       ( m_chip_cfg->end_address - ( FLASH_PAGE_SIZE * 2U ) )
static const uint8_t FLASH_ERASED_VALUE = 0xFFU;
// #define uC_FLASH_WORD_ALIGNMENT_MASK		~(0x01U)

// #define uC_FLASH_CR_PER_RESET                 (static_cast<uint32_t>(0x00001FFDU))

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
#ifdef DEBUG
static bool interrupt_enabled = false;
#endif
NV_Ctrl::semaphore_status_t uC_Flash::m_semaphore = NV_Ctrl::RELEASE;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#ifdef DEBUG
void uC_Flash::uC_Flash_Int_Handler( void )
{
	/*	Stop execution to understand flash error event.
	    Interrupt can be triggered due to any of following event
	    1. Write protection error
	    2. Read protection error
	    3. Programming error
	 */
	BF_ASSERT( false );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Flash::uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FLASH_TypeDef* flash_ctrl,
					bool erase_on_boundary ) :
	NV_Ctrl(),
	m_chip_cfg( reinterpret_cast<uC_FLASH_CHIP_CONFIG_TD const*>( chip_config ) ),
	m_flash_ctrl( reinterpret_cast<FLASH_TypeDef*>( flash_ctrl ) ),
	m_erase_val( 0U ),
	m_erase_on_boundary( erase_on_boundary ),
	m_flash_async_task_handle( nullptr ),
	m_erase_start_time( 0U )
{

#ifdef DEBUG
	/*	Debug code for capturing flash error events	*/
	if ( interrupt_enabled == false )
	{
		interrupt_enabled = true;
		Unlock_Flash();
		Wait_For_Complete();
		m_flash_ctrl->CR |= FLASH_CR_ERRIE;
		Lock_Flash();

		uC_Interrupt::Set_Vector( uC_Flash_Int_Handler, FLASH_IRQn, uC_INT_HW_PRIORITY_4 );
		uC_Interrupt::Enable_Int( FLASH_IRQn );
	}
#endif

	m_state = IDLE;
	m_status = SUCCESS;
	memset( &m_asynch_data, 0, sizeof( m_asynch_data ) );

	m_asynch_erase_sector_head = 0;
	m_asynch_erase_sector_end = 0;

	m_wait_time = 0;

	m_flash_ctrl->SR |= FLASH_PGERR;


	m_flash_async_task_handle = new CR_Tasker( &Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
											   CR_TASKER_IDLE_PRIORITY, "Flash_Routine" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read( uint8_t* data, uint32_t address, uint32_t length,
									 cback_func_t cback, cback_param_t param )
{
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "R: %x	%d", address, length );
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_semaphore = NV_Ctrl::ACQUIRE;
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
NV_Ctrl::nv_status_t uC_Flash::Write( const uint8_t* data, uint32_t address, uint32_t length,
									  cback_func_t cback, cback_param_t param )
{
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "W: %x	%d", address, length );
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_semaphore = NV_Ctrl::ACQUIRE;

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
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, uint32_t length,
									  cback_func_t cback, cback_param_t param )
{
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "E: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = nullptr;
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;

			m_asynch_erase_sector_head = Get_Sector( address );
			m_asynch_erase_sector_end = Get_Sector( address + length );

			m_wait_time = Erase_Time( address, length );
			m_state = ERASE;
			m_status = NV_Ctrl::PENDING_CALL_BACK;

			// Erase operation will be held for ERASE_HOLDING_TIME_MILLI_SEC from current time
			m_erase_start_time = BF_Lib::Timers::Get_Time();
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
NV_Ctrl::nv_status_t uC_Flash::Erase_All( cback_func_t cback, cback_param_t param )
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
NV_Ctrl::nv_status_t uC_Flash::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									 bool use_protection )
{
	bool good_data;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length ) && ( length != 0U ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "r: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			good_data = Read_Now( data, address, length );
			m_semaphore = NV_Ctrl::RELEASE;
			if ( good_data == true )
			{
				m_status = NV_Ctrl::SUCCESS;
			}
			else
			{
				m_status = NV_Ctrl::DATA_ERROR;
			}
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

	if ( use_protection )
	{}
	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									  bool use_protection )
{
	bool good_data = false;
	uint8_t retry_count = 0U;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "w: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			while ( good_data == false && retry_count < WRITE_RETRY_COUNT )
			{
				if ( HAL_HSEM_Take( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
				{
					if ( !HAL_HSEM_IsSemTaken( HSEM_CPU1_FLASH_PROTECT ) )
					{
						if ( HAL_HSEM_Take( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
						{
							good_data = Write_Now( data, address, length );
							HAL_HSEM_Release( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 );
						}
						else
						{}
					}
					else
					{}
					HAL_HSEM_Release( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 );
				}
				else
				{}
				if ( good_data == false )
				{
					retry_count++;
					uC_Delay( SYNC_WRITE_RETRY_DELAY );
				}
			}
			m_semaphore = NV_Ctrl::RELEASE;
			if ( good_data == true )
			{
				good_data = Check_Data( data, address, length );
			}

			if ( good_data == true )
			{
				m_status = NV_Ctrl::SUCCESS;
			}
			else
			{
				m_status = NV_Ctrl::DATA_ERROR;
			}
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

	if ( use_protection )
	{}

	return ( m_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Write_Now( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length )
{
	bool return_val = true;
	uint32_t page_mask = 0U;
	bool erase_status = true;
	uint8_t padded_data[8] = {0U};
	uC_FLASH_STATUS_EN flash_status;

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
			m_flash_ctrl->CR |= FLASH_CR_PG;
			if ( length >= MIN_WRITE_CHUNK_LEN )
			{
				Write_u32( dest_address, *( reinterpret_cast<uint32_t*>( data ) ) );
				__ISB();	// As per reference cube code.
				Write_u32( dest_address + 4U, *( reinterpret_cast<uint32_t*>( data + 4U ) ) );
				dest_address += MIN_WRITE_CHUNK_LEN;
				length -= MIN_WRITE_CHUNK_LEN;
				data += MIN_WRITE_CHUNK_LEN;
			}
			else
			{
				for ( uint8_t i = 0U; i < MIN_WRITE_CHUNK_LEN; i++ )
				{
					if ( i < length )
					{
						padded_data[i] = *( data + i );
					}
					else
					{
						padded_data[i] = 0xFF;
					}
				}
				Write_u32( dest_address, *( reinterpret_cast<uint32_t*>( padded_data ) ) );
				__ISB();					// As per reference cube code.
				Write_u32( dest_address + 4U, *( reinterpret_cast<uint32_t*>( padded_data + 4U ) ) );
				length = 0U;
			}
			flash_status = Wait_For_Complete();

			if ( flash_status != uC_FLASH_COMPLETE )
			{
				return_val = false;
			}
			m_flash_ctrl->CR &= ( ~FLASH_CR_PG );
		}
	}
	Lock_Flash();

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
									  uint8_t erase_data, bool protected_data )
{
	bool erase_status = false;
	uint8_t retry_count = 0U;

	m_erase_val = erase_data;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		NV_CTRL_PRINT( DBG_MSG_INFO, "e: %x	%d", address, length );
		m_semaphore = NV_Ctrl::ACQUIRE;
		while ( erase_status == false && retry_count < WRITE_RETRY_COUNT )
		{
			if ( HAL_HSEM_Take( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
			{
				if ( !HAL_HSEM_IsSemTaken( HSEM_CPU1_FLASH_PROTECT ) )
				{
					if ( HAL_HSEM_Take( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
					{
						erase_status = Erase_Page( address );
						HAL_HSEM_Release( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 );
					}
					else
					{}
				}
				else
				{}
				HAL_HSEM_Release( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 );
			}
			else
			{}
			if ( erase_status == false )
			{
				retry_count++;
				uC_Delay( ERASE_RETRY_DELAY );
			}
		}
		m_semaphore = NV_Ctrl::RELEASE;
		if ( true == erase_status )
		{
			m_status = NV_Ctrl::SUCCESS;
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
bool uC_Flash::Erase_Page( uint32_t address )
{
	uC_FLASH_STATUS_EN status;
	uint32_t sector_num;

	status = Wait_For_Complete();

	if ( status == uC_FLASH_COMPLETE )
	{
		sector_num = Get_Sector( address );

#ifndef DO_NOT_CHANGE_MEM
		Unlock_Flash();
		/* if the previous operation is completed, proceed to erase the sector */
		m_flash_ctrl->CR &= SECTOR_MASK;
		m_flash_ctrl->CR &= ~FLASH_CR_PNB_Msk;
		m_flash_ctrl->CR |= FLASH_CR_PER | ( sector_num << FLASH_CR_PNB_Pos );
		m_flash_ctrl->CR |= FLASH_CR_STRT;

		status = Wait_For_Complete();
		/* if the erase operation is completed, disable the SER Bit */
		m_flash_ctrl->CR &= ( ~FLASH_CR_PER );
		m_flash_ctrl->CR &= SECTOR_MASK;
		Lock_Flash();
#endif
	}

	return ( status == uC_FLASH_COMPLETE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Erase_Page_Non_Blocking( uint32_t sector_num )
{
	Wait_For_Complete();
#ifndef DO_NOT_CHANGE_MEM
	Unlock_Flash();
	/* if the previous operation is completed, proceed to erase the sector */
	m_flash_ctrl->CR &= SECTOR_MASK;
	m_flash_ctrl->CR &= ~FLASH_CR_PNB_Msk;
	m_flash_ctrl->CR |= FLASH_CR_PER | ( sector_num << FLASH_CR_PNB_Pos );
	m_flash_ctrl->CR |= FLASH_CR_STRT;

	Wait_For_Complete();
	/* if the erase operation is completed, disable the SER Bit */
	m_flash_ctrl->CR &= ( ~FLASH_CR_PER );
	m_flash_ctrl->CR &= SECTOR_MASK;
	Lock_Flash();
#endif
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
bool uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool range_good = false;
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

	// programming.

	write_time = 0U;
	page_index = 0U;
	test_address = INTERNAL_FLASH_START_ADDRESS +
		INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;

	if ( include_erase_time == true )
	{
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
	}
	// Check to see if we are calculating the erase time or a write time.
	if ( data != NULL )
	{
		byte_write_time = BYTE_WRITE_TIME_US * ( length );
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
uint32_t uC_Flash::Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							  NV_CTRL_LENGTH_TD length ) const
{
	// Todo: Implementation pending
	uint32_t read_time = BYTE_READ_TIME_US * length;

	if ( read_time < MICROSECONDS_ADJUSTMENT )
	{
		read_time = MICROSECONDS_ADJUSTMENT;
	}
	// Because the byte read time is in microseconds we have to adjust it down.
	read_time = Round_Div( read_time, MICROSECONDS_ADJUSTMENT );
	return ( read_time );
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
	switch ( m_state )
	{
		case IDLE:
			break;

		case ERASE:
			if ( BF_Lib::Timers::Expired( m_erase_start_time, ERASE_HOLDING_TIME_MILLI_SEC ) )
			{
				asynch_erase_handler();
			}
			break;

		case READ:
			asynch_read_handler();
			break;

		case WRITE:
			asynch_write_handler();
			break;

		case COMPLETE:
			if ( m_asynch_data.cback != nullptr )
			{
				m_asynch_data.cback( m_asynch_data.param, m_status );
			}
			m_state = IDLE;
			break;

		case ERROR:
			if ( m_asynch_data.cback != nullptr )
			{
				m_asynch_data.cback( m_asynch_data.param, m_status );
			}
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
void uC_Flash::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_asynch_data.cback = func_callback;
	m_asynch_data.param = func_param;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Get_Memory_Info( mem_range_info_t* mem_range_info )
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
void uC_Flash::asynch_erase_handler( void )
{
	/* Acquired semaphore for complete erase operation */
	uC_FLASH_STATUS_EN status = Get_Status();

	if ( status == uC_FLASH_COMPLETE )
	{
		if ( m_asynch_erase_sector_head > m_asynch_erase_sector_end )
		{
			/* Release flash control after complete flash erase */
			m_semaphore = NV_Ctrl::RELEASE;
			m_state = COMPLETE;
			m_status = SUCCESS;
		}
		else
		{
			if ( HAL_HSEM_Take( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
			{
				if ( !HAL_HSEM_IsSemTaken( HSEM_CPU1_FLASH_PROTECT ) )
				{
					if ( HAL_HSEM_Take( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
					{
						NV_CTRL_PRINT( DBG_MSG_INFO, "Flash erase handle: increment" );
						Erase_Page_Non_Blocking( m_asynch_erase_sector_head++ );
						HAL_HSEM_Release( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 );
					}
					else
					{}
				}
				else
				{}
				HAL_HSEM_Release( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 );
			}
			else
			{}
		}
	}
	else if ( status != uC_FLASH_BUSY )
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR, "Flash erase handle: error" );
		m_state = ERROR;
		m_status = DATA_ERROR;
		m_semaphore = NV_Ctrl::RELEASE;
	}
	else
	{
		// NV_CTRL_PRINT( DBG_MSG_INFO, "Flash erase handle: wait");
		// Busy state, We should wait.
		// Todo: We might need to add some cr_tasker_delay here if we want to reduce polling freq when flash is BUSY
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::asynch_write_handler( void )
{
	NV_CTRL_LENGTH_TD length = 0U;
	bool good_data = false;
	uint8_t retry_count = 0U;


	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		/* Acquire semaphore only for WRITE_CHUCK_SIZE transfer */
		if ( m_asynch_data.length > WRITE_CHUCK_SIZE )
		{
			length = WRITE_CHUCK_SIZE;
		}
		else
		{
			length = m_asynch_data.length;
		}
		while ( good_data == false && retry_count < WRITE_RETRY_COUNT )
		{
			if ( HAL_HSEM_Take( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
			{
				if ( !HAL_HSEM_IsSemTaken( HSEM_CPU1_FLASH_PROTECT ) )
				{
					if ( HAL_HSEM_Take( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 ) == HAL_OK )
					{
						good_data = Write_Now( m_asynch_data.data,  m_asynch_data.address, length );
						HAL_HSEM_Release( HSEM_CPU2_FLASH_PROTECT, HSEM_PROCESS_1 );
					}
					else
					{}
				}
				else
				{}
				HAL_HSEM_Release( HSEM_FLASH_PROTECT, HSEM_PROCESS_1 );
			}
			else
			{}
			if ( good_data == false )
			{
				retry_count++;
				uC_Delay( ASYNC_WRITE_RETRY_DELAY );
			}
		}
		if ( good_data == true )
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
void uC_Flash::asynch_read_handler( void )
{
	NV_CTRL_LENGTH_TD length;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		/* Acquire semaphore only for READ_CHUCK_SIZE transfer */
		if ( m_asynch_data.length > READ_CHUCK_SIZE )
		{
			length = READ_CHUCK_SIZE;
		}
		else
		{
			length = m_asynch_data.length;
		}

		if ( true == Read_Now( m_asynch_data.data,  m_asynch_data.address, length ) )
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
	else
	{
		// misra
	}
}

}
