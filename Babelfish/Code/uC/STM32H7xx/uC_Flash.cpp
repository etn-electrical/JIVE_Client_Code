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
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */
#include "Includes.h"
#include "uC_Flash.h"
#include "StdLib_MV.h"
#include "uC_Interrupt.h"
#include "string.h"
#include "NV_Ctrl_Debug.h"
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


static const uint8_t FLASH_ERASED_VALUE = 0xFFU;
static const uint32_t BANK1_START_ADDR = 0x08000000U;
static const uint32_t BANK1_END_ADDR = 0x080FFFFFU;
static const uint32_t BANK2_START_ADDR = 0x08100000U;
static const uint32_t BANK2_END_ADDR = 0x081FFFFFU;
// static const uint32_t uC_BASE_OPT_KEY_1 = 0x08192A3BU;     //keys to unlock the option bytes.
// static const uint32_t uC_BASE_OPT_KEY_2 = 0x4C5D6E7FU;     //keys to unlock the option bytes.

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
	/*	Stop execution to understand flash error event. Interrupt can be triggered due to any of following event 1.
	   Write protection error 2. Read protection error 3. Programming error
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
	m_erase_on_boundary( erase_on_boundary )
{

#ifdef DEBUG
	/*	Debug code for capturing flash error events	*/
	if ( interrupt_enabled == false )
	{
		interrupt_enabled = true;
		Unlock_Flash();
		Wait_For_Complete();
		/* Bank 1 error events */
		m_flash_ctrl->CR1 |= ( FLASH_CR_WRPERRIE | FLASH_CR_PGSERRIE | \
							   FLASH_CR_STRBERRIE | FLASH_CR_INCERRIE | \
							   FLASH_CR_OPERRIE | FLASH_CR_RDPERRIE | \
							   FLASH_CR_RDSERRIE );
		/* Bank 2 error events */
		m_flash_ctrl->CR2 |= ( FLASH_CR_WRPERRIE | FLASH_CR_PGSERRIE | \
							   FLASH_CR_STRBERRIE | FLASH_CR_INCERRIE | \
							   FLASH_CR_OPERRIE | FLASH_CR_RDPERRIE | \
							   FLASH_CR_RDSERRIE );
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

	m_flash_ctrl->CCR1 |= FLASH_PGERR;
	m_flash_ctrl->CCR2 |= FLASH_PGERR;

	new CR_Tasker( &Flash_Async_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
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
			m_asynch_erase_sector_end = Get_Sector( address + length - 1U );

			m_wait_time = Erase_Time( address, length );
			m_state = ERASE;
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
	bool good_data;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "w: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			good_data = Write_Now( data, address, length );
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
	bool return_val = false;

	if ( ( dest_address >= BANK1_START_ADDR ) && ( dest_address <= BANK1_END_ADDR ) )
	{
		return_val = Write_Now_Bank1( data, dest_address, length );
	}
	else if ( ( dest_address >= BANK2_START_ADDR ) && ( dest_address <= BANK2_END_ADDR ) )
	{
		return_val = Write_Now_Bank2( data, dest_address, length );
	}
	else
	{
		// misra
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Write_Now_Bank1( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length )
{
	bool return_val = true;
	uint32_t page_mask = 0U;
	bool erased_area = true;
	bool erase_status = true;
	uC_FLASH_STATUS_EN flash_status;
	uint32_t start_address = dest_address;
	uint32_t full_size = length;

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
				m_flash_ctrl->CR1 &= ( ~FLASH_CR_PSIZE );
				flash_status = uC_FLASH_COMPLETE;

				if ( ( ( length / 4U ) != 0U ) && ( ( dest_address % 4U ) == 0U ) )
				{
#ifndef DO_NOT_CHANGE_MEM
					/* Write 4 bytes */
					m_flash_ctrl->CR1 |= ( FLASH_CR_PG | FLASH_CR_PSIZE_1 );

					__ISB();
					__DSB();

					/* Flash memory programming by 256 bits thus row_index range 1 to 8 */
					uint8_t row_index = ( length < 32U ) ? ( length / 4U ):( 8U );
					uint8_t write_count = 32U, index = 0;
					do
					{
						( *( ( volatile uint32_t* ) ( dest_address ) ) ) = *( ( uint32_t* )data );
						dest_address += 4U;
						data += 4U;
						length -= 4U;
						write_count -= 4U;
						row_index--;
					} while ( row_index != 0U );

					while ( index < write_count )
					{
						*( ( volatile uint8_t* ) ( dest_address ) +
						   index ) = *( ( volatile uint8_t* ) ( dest_address ) + index );
						index++;
					}

					__ISB();
					__DSB();

					/* Forces a write operation even if the write buffer is not full */
					if ( ( m_flash_ctrl->SR1 & FLASH_SR_WBNE ) == FLASH_SR_WBNE )
					{
						m_flash_ctrl->CR1 |= FLASH_CR_FW;
					}
					flash_status = Wait_For_Complete();
#endif
				}
				else if ( ( length & 0x02U ) && ( ( dest_address % 2U ) == 0U ) )
				{
#ifndef DO_NOT_CHANGE_MEM
					/* Write 2 bytes */
					m_flash_ctrl->CR1 |= ( FLASH_CR_PG | FLASH_CR_PSIZE_0 );

					__ISB();
					__DSB();

					/* Flash memory programming by 256 bits thus row_index range 1 to 8 */
					uint8_t row_index = ( length < 32U ) ? ( length / 2U ):( 16U );
					uint8_t write_count = 32U, index = 0;

					do
					{
						( *( ( volatile uint16_t* ) ( dest_address ) ) ) = *( ( uint16_t* )data );
						dest_address += 2U;
						data += 2U;
						length -= 2U;
						write_count -= 2U;
						row_index--;
					} while ( row_index != 0U );

					while ( index < write_count )
					{
						*( ( volatile uint8_t* ) ( dest_address ) +
						   index ) = *( ( volatile uint8_t* ) ( dest_address ) + index );
						index++;
					}

					__ISB();
					__DSB();

					/* Forces a write operation even if the write buffer is not full */
					if ( ( m_flash_ctrl->SR1 & FLASH_SR_WBNE ) == FLASH_SR_WBNE )
					{
						m_flash_ctrl->CR1 |= FLASH_CR_FW;
					}
					flash_status = Wait_For_Complete();
#endif
				}
				else if ( length != 0U )
				{
#ifndef DO_NOT_CHANGE_MEM
					/* Write 1 byte */
					m_flash_ctrl->CR1 |= FLASH_CR_PG;

					__ISB();
					__DSB();

					/* Flash memory programming by 256 bits thus row_index range 1 to 8 */
					uint8_t row_index = ( length < 32U ) ? ( length ):( 32U );
					uint8_t write_count = 32U, index = 0;

					do
					{
						( *( ( volatile uint8_t* ) ( dest_address ) ) ) = *( ( uint8_t* )data );
						dest_address += 1U;
						data += 1U;
						length -= 1U;
						write_count -= 1U;
						row_index--;
					} while ( row_index != 0U );

					while ( index < write_count )
					{
						*( ( volatile uint8_t* ) ( dest_address ) +
						   index ) = *( ( volatile uint8_t* ) ( dest_address ) + index );
						index++;
					}

					__ISB();
					__DSB();

					/* Forces a write operation even if the write buffer is not full */
					if ( ( m_flash_ctrl->SR1 & FLASH_SR_WBNE ) == FLASH_SR_WBNE )
					{
						m_flash_ctrl->CR1 |= FLASH_CR_FW;
					}
					flash_status = Wait_For_Complete();
#endif
				}
				else
				{
					/*Misra Compliance*/
				}

				if ( flash_status != uC_FLASH_COMPLETE )
				{
					return_val = false;
				}
				m_flash_ctrl->CR1 &= ( ~FLASH_CR_PG );
			}
		}
		SCB_CleanInvalidateDCache_by_Addr( ( uint32_t* )start_address, full_size );
		SCB_InvalidateICache();
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
bool uC_Flash::Write_Now_Bank2( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length )
{
	bool return_val = true;
	uint32_t page_mask = 0U;
	bool erased_area = true;
	bool erase_status = true;
	uC_FLASH_STATUS_EN flash_status;
	uint32_t start_address = dest_address;
	uint32_t full_size = length;

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
				m_flash_ctrl->CR2 &= ( ~FLASH_CR_PSIZE );
				flash_status = uC_FLASH_COMPLETE;

				if ( ( ( length / 4U ) != 0U ) && ( ( dest_address % 4U ) == 0U ) )
				{
#ifndef DO_NOT_CHANGE_MEM
					/* Write 4 bytes */
					m_flash_ctrl->CR2 |= ( FLASH_CR_PG | FLASH_CR_PSIZE_1 );

					__ISB();
					__DSB();

					/* Flash memory programming by 256 bits thus row_index range 1 to 8 */
					uint8_t row_index = ( length < 32U ) ? ( length / 4U ):( 8U );
					uint8_t write_count = 32U, index = 0;
					do
					{
						( *( ( volatile uint32_t* ) ( dest_address ) ) ) = *( ( uint32_t* )data );
						dest_address += 4U;
						data += 4U;
						length -= 4U;
						write_count -= 4U;
						row_index--;
					} while ( row_index != 0U );

					while ( index < write_count )
					{
						*( ( volatile uint8_t* ) ( dest_address ) +
						   index ) = *( ( volatile uint8_t* ) ( dest_address ) + index );
						index++;
						// write_count--;
					}

					__ISB();
					__DSB();

					/* Forces a write operation even if the write buffer is not full */
					if ( ( m_flash_ctrl->SR2 & FLASH_SR_WBNE ) == FLASH_SR_WBNE )
					{
						m_flash_ctrl->CR2 |= FLASH_CR_FW;
					}
					flash_status = Wait_For_Complete();
#endif
				}
				else if ( ( length & 0x02U ) && ( ( dest_address % 2U ) == 0U ) )
				{
#ifndef DO_NOT_CHANGE_MEM
					/* Write 2 bytes */
					m_flash_ctrl->CR2 |= ( FLASH_CR_PG | FLASH_CR_PSIZE_0 );

					__ISB();
					__DSB();

					/* Flash memory programming by 256 bits thus row_index range 1 to 8 */
					uint8_t row_index = ( length < 32U ) ? ( length / 2U ):( 16U );
					uint8_t write_count = 32U, index = 0;

					do
					{
						( *( ( volatile uint16_t* ) ( dest_address ) ) ) = *( ( uint16_t* )data );
						dest_address += 2U;
						data += 2U;
						length -= 2U;
						write_count -= 2U;
						row_index--;
					} while ( row_index != 0U );

					while ( index < write_count )
					{
						*( ( volatile uint8_t* ) ( dest_address ) +
						   index ) = *( ( volatile uint8_t* ) ( dest_address ) + index );
						index++;
					}

					__ISB();
					__DSB();

					/* Forces a write operation even if the write buffer is not full */
					if ( ( m_flash_ctrl->SR2 & FLASH_SR_WBNE ) == FLASH_SR_WBNE )
					{
						m_flash_ctrl->CR2 |= FLASH_CR_FW;
					}
					flash_status = Wait_For_Complete();
#endif
				}
				else if ( length != 0U )
				{
#ifndef DO_NOT_CHANGE_MEM
					/* Write 1 byte */
					m_flash_ctrl->CR2 |= FLASH_CR_PG;

					__ISB();
					__DSB();

					/* Flash memory programming by 256 bits thus row_index range 1 to 8 */
					uint8_t row_index = ( length < 32U ) ? ( length ):( 32U );
					uint8_t write_count = 32U, index = 0;

					do
					{
						( *( ( volatile uint8_t* ) ( dest_address ) ) ) = *( ( uint8_t* )data );
						dest_address += 1U;
						data += 1U;
						length -= 1U;
						write_count -= 1U;
						row_index--;
					} while ( row_index != 0U );

					while ( index < write_count )
					{
						*( ( volatile uint8_t* ) ( dest_address ) +
						   index ) = *( ( volatile uint8_t* ) ( dest_address ) + index );
						index++;
					}

					__ISB();
					__DSB();

					/* Forces a write operation even if the write buffer is not full */
					if ( ( m_flash_ctrl->SR2 & FLASH_SR_WBNE ) == FLASH_SR_WBNE )
					{
						m_flash_ctrl->CR2 |= FLASH_CR_FW;
					}
					flash_status = Wait_For_Complete();
#endif
				}
				else
				{
					/*Misra Compliance*/
				}

				if ( flash_status != uC_FLASH_COMPLETE )
				{
					return_val = false;
				}
				m_flash_ctrl->CR2 &= ( ~FLASH_CR_PG );
			}
		}
		SCB_CleanInvalidateDCache_by_Addr( ( uint32_t* )start_address, full_size );
		SCB_InvalidateICache();
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
									  uint8_t erase_data, bool protected_data )
{
	bool erase_status = false;
	uint32_t sector_size = 0U;
	uint32_t sector_num = 0U;
	uint32_t base_address = 0U;

	m_erase_val = erase_data;
	NV_Ctrl::nv_status_t m_status = NV_Ctrl::BUSY_ERROR;

	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		if ( Check_Range( address, length ) )
		{
			NV_CTRL_PRINT( DBG_MSG_INFO, "e: %x	%d", address, length );
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_status = NV_Ctrl::SUCCESS;
			sector_num = Get_Sector( address );
			while ( ( m_status == NV_Ctrl::SUCCESS ) && ( length != 0 ) )
			{
				uC_Watchdog::Force_Feed_Dog();
				base_address = Get_Sector_Base_Address( sector_num );
				sector_size = Get_Page_Size( base_address );
				/* Even if the length value is less than the sector size it will erase the entire sector */
				erase_status = Erase_Page( base_address );
				if ( erase_status != true )
				{
					m_status = NV_Ctrl::BUSY_ERROR;
				}
				else
				{
					sector_num++;
					/* Decrement the length as per the erased sector size. */
					if ( length >= sector_size )
					{
						length -= sector_size;
					}
					else
					{
						length = 0U;
					}
				}
			}	// end of while loop()

			m_semaphore = NV_Ctrl::RELEASE;
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
bool uC_Flash::Erase_Page( uint32_t address )
{
	uC_FLASH_STATUS_EN status;
	uint32_t sector_num;

	status = Wait_For_Complete();

	if ( status == uC_FLASH_COMPLETE )
	{
#ifndef DO_NOT_CHANGE_MEM
		if ( ( address >= BANK1_START_ADDR ) && ( address <= BANK1_END_ADDR ) )
		{
			sector_num = Get_Sector( address );
			sector_num %= 8U;	/// > Bank 1 sector_start = 0 and sector_end = 7
			Unlock_Flash();

			/* if the previous operation is completed, proceed to erase the sector */
			m_flash_ctrl->CR1 &= ( ~FLASH_CR_PSIZE );
			m_flash_ctrl->CR1 |= FLASH_CR_PSIZE_1;	// Set it to 32bit so we can erase quicker.
			m_flash_ctrl->CR1 &= ( ~FLASH_CR_SNB );
			m_flash_ctrl->CR1 |= ( FLASH_CR_SER | ( sector_num * FLASH_CR_SNB_1 ) );

			/*
			    As per datasheet the erase can take 3 seconds so increasing the watchdog time to 4 Sec.
			 */
			uC_Watchdog::Init( WATCHDOG_TIMEOUT_ERASE_SECTOR );

			/*
			    Added 2 DSB instructions here as per note from STM32H743 manual. Note: When setting the STRT bit in the
			       FLASH_CR register and before polling the BSY bit to be cleared, the software can issue a DSB
			       instruction to guarantee the completion of a previous access to FLASH_CR register.
			 */
			__ISB();
			__DSB();
			m_flash_ctrl->CR1 |= FLASH_CR_START;
			__ISB();
			__DSB();

			status = Wait_For_Complete();

			/* if the erase operation is completed, disable the SER Bit */
			m_flash_ctrl->CR1 &= ( ~FLASH_CR_SER );
			m_flash_ctrl->CR1 &= ( ~FLASH_CR_SNB );
			Lock_Flash();

			uC_Watchdog::Init( WATCHDOG_TIMEOUT );
		}
		else if ( ( address >= BANK2_START_ADDR ) && ( address <= BANK2_END_ADDR ) )
		{
			sector_num = Get_Sector( address );
			sector_num %= 8U;	/// > Bank 2 sector_start = 0 and sector_end = 7
			Unlock_Flash();

			/* if the previous operation is completed, proceed to erase the sector */
			m_flash_ctrl->CR2 &= ( ~FLASH_CR_PSIZE );
			m_flash_ctrl->CR2 |= FLASH_CR_PSIZE_1;	// Set it to 32bit so we can erase quicker.
			m_flash_ctrl->CR2 &= ( ~FLASH_CR_SNB );
			m_flash_ctrl->CR2 |= ( FLASH_CR_SER | ( sector_num * FLASH_CR_SNB_1 ) );

			/*
			    As per datasheet the erase can take 3 seconds so increasing the watchdog time to 4 Sec.
			 */
			uC_Watchdog::Init( WATCHDOG_TIMEOUT_ERASE_SECTOR );

			/*
			    Added 2 DSB instructions here as per note from STM32H743 manual. Note: When setting the STRT bit in the
			       FLASH_CR register and before polling the BSY bit to be cleared, the software can issue a DSB
			       instruction to guarantee the completion of a previous access to FLASH_CR register.
			 */
			__ISB();
			__DSB();
			m_flash_ctrl->CR2 |= FLASH_CR_START;
			__ISB();
			__DSB();

			status = Wait_For_Complete();

			/* if the erase operation is completed, disable the SER Bit */
			m_flash_ctrl->CR2 &= ( ~FLASH_CR_SER );
			m_flash_ctrl->CR2 &= ( ~FLASH_CR_SNB );
			SCB_CleanInvalidateDCache_by_Addr( ( uint32_t* )Get_Sector_Base_Address( sector_num ),
											   INTERNAL_FLASH_PAGE_SIZES[sector_num].sector_size );
			SCB_InvalidateICache();
			Lock_Flash();

			uC_Watchdog::Init( WATCHDOG_TIMEOUT );
		}
		else
		{
			// Misra
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
void uC_Flash::Erase_Page_Non_Blocking( uint32_t sector_num )
{
#ifndef DO_NOT_CHANGE_MEM
	Unlock_Flash();
	if ( sector_num < 8 )
	{
		/* Erase the sector */
		m_flash_ctrl->CR1 &= ( ~FLASH_CR_PSIZE );
		m_flash_ctrl->CR1 |= FLASH_CR_PSIZE_1;		// Set it to 32bit so we can erase quicker.
		m_flash_ctrl->CR1 &= ( ~FLASH_CR_SNB );
		m_flash_ctrl->CR1 |= ( FLASH_CR_SER | ( sector_num * FLASH_CR_SNB_1 ) );

		__ISB();
		__DSB();
		m_flash_ctrl->CR1 |= FLASH_CR_START;
		__ISB();
		__DSB();
	}
	else if ( sector_num < 16 )
	{
		sector_num %= 8U;		/// > Bank 2 has sector_start = 0 and sector_end = 7
		/* Erase the sector */
		m_flash_ctrl->CR2 &= ( ~FLASH_CR_PSIZE );
		m_flash_ctrl->CR2 |= FLASH_CR_PSIZE_1;		// Set it to 32bit so we can erase quicker.
		m_flash_ctrl->CR2 &= ( ~FLASH_CR_SNB );
		m_flash_ctrl->CR2 |= ( FLASH_CR_SER | ( sector_num * FLASH_CR_SNB_1 ) );

		__ISB();
		__DSB();
		m_flash_ctrl->CR2 |= FLASH_CR_START;
		__ISB();
		__DSB();
	}
	else
	{
		BF_ASSERT( false );
	}

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
	/*The Data Synchronization Barrier (DSB) acts as a special kind of memory barrier. The DSB operation will complete
	   when all explicit memory accesses before this instruction have completed. No instructions after the DSB will be
	   executed until the DSB instruction has completed, that is, when all of the pending accesses have completed*/
	__ISB();
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
uC_FLASH_STATUS_EN uC_Flash::Get_Status( void ) const
{
	uC_FLASH_STATUS_EN status = uC_FLASH_COMPLETE;
	uint32_t status_reg_bk1, status_reg_bk2;

	/* H743 has dedicated status registers for each bank */
	status_reg_bk1 = m_flash_ctrl->SR1;
	status_reg_bk2 = m_flash_ctrl->SR2;

	/* Check for memory operation */
	if ( ( ( status_reg_bk1 & FLASH_SR_BSY ) == FLASH_SR_BSY ) ||
		 ( ( status_reg_bk2 & FLASH_SR_BSY ) == FLASH_SR_BSY ) ||
		 ( ( status_reg_bk1 & FLASH_SR_QW ) == FLASH_SR_QW ) ||
		 ( ( status_reg_bk2 & FLASH_SR_QW ) == FLASH_SR_QW ) )
	{
		status = uC_FLASH_BUSY;
	}
	else
	{
		/* Check for errors */
		if ( ( status_reg_bk1 & FLASH_PGERR ) ||
			 ( status_reg_bk2 & FLASH_PGERR ) )
		{
			status = uC_FLASH_ERROR_PG;
		}
		else
		{
			if ( ( status_reg_bk1 & FLASH_SR_WRPERR ) ||
				 ( status_reg_bk2 & FLASH_SR_WRPERR ) )
			{
				status = uC_FLASH_ERROR_WRP;
			}
			else
			{
				status = uC_FLASH_COMPLETE;
			}
		}
		/* Clear the Error Flags by writing 1 to it*/
		m_flash_ctrl->CCR1 |= FLASH_PGERR;
		m_flash_ctrl->CCR2 |= FLASH_PGERR;
		m_flash_ctrl->CCR1 |= FLASH_CCR_CLR_EOP;
		m_flash_ctrl->CCR2 |= FLASH_CCR_CLR_EOP;
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
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	if ( ( m_chip_cfg->start_address >= BANK1_START_ADDR ) &&
		 ( m_chip_cfg->start_address <= BANK2_END_ADDR ) &&
		 ( m_chip_cfg->end_address >= BANK1_START_ADDR ) &&
		 ( m_chip_cfg->end_address <= BANK2_END_ADDR ) )
	{
		/* Erase Length should be equal to the sector size */
		return_status = ( Erase( m_chip_cfg->start_address,
								 ( m_chip_cfg->end_address - m_chip_cfg->start_address + 1U ),
								 uC_FLASH_DEFAULT_ERASE_VAL, false ) );
	}
	else
	{
		BF_ASSERT( false );
	}
	return ( return_status );
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

	if ( ( address >= BANK1_START_ADDR ) && ( address <= BANK2_END_ADDR ) )
	{
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
	 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 2-13-3, Unsigned octal and hexadecimal literals
	 * require a 'U' suffix
	 * @n Justification: The ST supplied includes where this #define resides shall not be changed.
	 */
	if ( ( m_flash_ctrl->CR1 & FLASH_CR_LOCK ) != 0U )
	{
		/* Unlock Bank 1 */
		m_flash_ctrl->KEYR1 = FLASH_KEY1;
		m_flash_ctrl->KEYR1 = FLASH_KEY2;
	}
	if ( ( m_flash_ctrl->CR2 & FLASH_CR_LOCK ) != 0U )
	{
		/* Unlock Bank 2 */
		m_flash_ctrl->KEYR2 = FLASH_KEY1;
		m_flash_ctrl->KEYR2 = FLASH_KEY2;
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
	 * @n PCLint: Note 1960: Violates MISRA C++ 2008 Required Rule 2-13-3, Unsigned octal and hexadecimal literals
	 * require a 'U' suffix
	 * @n Justification: The ST supplied includes where this #define resides shall not be changed.
	 */
	/*lint -e{1960} */
	m_flash_ctrl->CR1 |= FLASH_CR_LOCK;
	m_flash_ctrl->CR2 |= FLASH_CR_LOCK;
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

	/* Check whether the address is within range */
	if ( ( address >= BANK1_START_ADDR ) && ( address <= BANK2_END_ADDR ) )
	{
		test_address = INTERNAL_FLASH_START_ADDRESS +
			INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
		while ( ( address >= test_address ) &&
				( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size != 0U ) )
		{
			page_index++;
			test_address += INTERNAL_FLASH_PAGE_SIZES[page_index].sector_size;
		}
	}
	else
	{
		BF_ASSERT( false );
	}
	return ( INTERNAL_FLASH_PAGE_SIZES[page_index].sector_no );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Get_Sector_Base_Address( uint32_t sector_id )
{
	uint32_t i = 0U;
	uint32_t sector_start_address = FLASH_BANK1_BASE;

	for ( i = 0; INTERNAL_FLASH_PAGE_SIZES[i].sector_no < sector_id; i++ )
	{
		sector_start_address += INTERNAL_FLASH_PAGE_SIZES[i].sector_size;
	}
	return ( sector_start_address );
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
	uint32_t erased_values;			// To count the number of values that are actually erased and

	// don't need programming.

	write_time = 0U;
	page_index = 0U;
	if ( ( address >= BANK1_START_ADDR ) && ( ( address + length ) <= ( BANK2_END_ADDR + 1U ) ) )
	{
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
			/*Because the byte write time is in microseconds we have to adjust it down.*/
			write_time += Round_Div( byte_write_time, MICROSECONDS_ADJUSTMENT );

		}
		else
		{
			// because we will erase the page we are on as well as the next page if we get
			write_time += ( SECTOR_ERASE_TIME_BASELINE_MS
							* ( Get_Page_Size( address ) /
								INTERNAL_FLASH_PAGE_SIZES[0].sector_size ) );
		}
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
	// uint32_t read_time = BYTE_READ_TIME_US * length;
	uint32_t read_time = 10 * length;

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
			NV_CTRL_PRINT( DBG_MSG_INFO, "Flash erase handle: increment" );
			Erase_Page_Non_Blocking( m_asynch_erase_sector_head++ );
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
		// NV_CTRL_PRINT( DBG_MSG_INFO, "Flash erase hadle: wait");
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
	NV_CTRL_LENGTH_TD length;

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

		if ( true == Write_Now( m_asynch_data.data,  m_asynch_data.address, length ) )
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

/*lint +e1924 */
