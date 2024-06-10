/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
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
#include "uC_OTP.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t FLASH_KEY1 = 0x45670123U;
static const uint32_t FLASH_KEY2 = 0xCDEF89ABU;
static const uint32_t CR_PSIZE_MASK = 0xFFFFFCFFU;
#ifdef OTP_RAM_SIMULATION
static const uint8_t LOCK_BLOCK_SIZE = 16U;
uint8_t* uC_OTP::m_otp_lock_block = nullptr;
#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_OTP::uC_OTP( uc_otp_config_t const* otp_config, FLASH_TypeDef* flash_ctrl ) :
	NV_Ctrl(),
	m_flash_ctrl( reinterpret_cast<FLASH_TypeDef*>( flash_ctrl ) )
{
#ifndef OTP_RAM_SIMULATION
	BF_ASSERT( ( otp_config->otp_start_address >= OTP_START_ADDRESS ) &&
			   ( otp_config->otp_start_address < OTP_LOCK_BLOCK_ADDRESS ) &&
			   ( ( otp_config->size + ( otp_config->otp_start_address - OTP_START_ADDRESS ) ) <
				 OTP_SIZE ) );
	m_otp_cfg = otp_config;
#else
	if ( m_otp_lock_block == nullptr )
	{
		uint8_t i;
		m_otp_cfg = otp_config;
		m_otp_lock_block = new uint8_t[LOCK_BLOCK_SIZE];
		for ( i = 0; i < LOCK_BLOCK_SIZE; i++ )
		{
			m_otp_lock_block[i] = OTP_UNLOCK_VALUE;
		}
	}
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_OTP::Is_Locked( void )
{
	bool_t return_status = false;
	uint8_t total_block_number = Get_Num_Blocks();
	uint8_t n = 0U;
	uint32_t lock_block_otp_ptr = 0U;

#ifdef OTP_RAM_SIMULATION
	lock_block_otp_ptr = reinterpret_cast<uint32_t>( m_otp_lock_block );
#else
	lock_block_otp_ptr = Get_Lock_Start_Index_Address();
#endif
	for (; n < total_block_number; n++ )
	{
		if ( *( reinterpret_cast<uint8_t*>( lock_block_otp_ptr + n ) ) ==
			 OTP_LOCK_VALUE )
		{
			return_status = true;
		}
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_OTP::Lock_Block( uint8_t block_number )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	uC_OTP_STATUS_EN flash_status;

#ifdef OTP_RAM_SIMULATION
	*( m_otp_lock_block + block_number ) = uC_OTP::OTP_LOCK_VALUE;

	return_status = NV_Ctrl::SUCCESS;
#else
	uint32_t otp_block_index = Get_Lock_Start_Index_Address();
	flash_status = Wait_For_Complete();

	if ( flash_status == uC_OTP_COMPLETE )
	{
		m_flash_ctrl->CR |= FLASH_CR_PG;
		Write_u8( ( otp_block_index + block_number ),
				  uC_OTP::OTP_LOCK_VALUE );
	}
	flash_status = Wait_For_Complete();

	if ( flash_status == uC_OTP_COMPLETE )
	{
		return_status = NV_Ctrl::SUCCESS;
	}
#endif
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_OTP::Get_Num_Blocks( void )
{
	uint8_t block_number = 0;

	block_number = ( m_otp_cfg->size + ( OTP_MIN_BLK_SIZE - 1 ) ) / OTP_MIN_BLK_SIZE;
	return ( block_number );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_OTP::Lock( void )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	bool_t lock_status = false;
	uint8_t n = 0U;

	lock_status = Is_Locked();
	if ( lock_status == false )
	{
		uint8_t total_block_number = Get_Num_Blocks();
		for (; n < total_block_number; n++ )
		{
			return_status = Lock_Block( n );
			if ( return_status == NV_Ctrl::DATA_ERROR )
			{
				break;
			}
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_OTP::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool_t status = false;

#ifdef OTP_RAM_SIMULATION
	status = true;
#else

	if ( ( address >= OTP_START_ADDRESS ) &&
		 ( ( address + length ) < OTP_LOCK_BLOCK_ADDRESS ) && ( length <= m_otp_cfg->size ) )
	{
		status = true;
	}
#endif
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_OTP::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length ) const
{
	bool_t status = true;

	BF_Lib::Copy_Const_String( data,
							   reinterpret_cast<uint8_t const*>( address ),
							   length );
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_OTP::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
								   bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	bool_t status = false;

	if ( ( status = Check_Range( address, length ) ) == true )
	{
		status = Read_Now( data, address, length );
		if ( status == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_OTP_STATUS_EN uC_OTP::Wait_For_Complete( void ) const
{
	uC_OTP_STATUS_EN status;

	Push_TGINT();
	/* Wait for a Flash operation to complete or a TIMEOUT to occur */
	do
	{
		status = Get_Status();
		uC_Watchdog::Force_Feed_Dog();
	} while ( status == uC_OTP_BUSY );
	Pop_TGINT();
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_OTP_STATUS_EN uC_OTP::Get_Status( void ) const
{
	uC_OTP_STATUS_EN status = uC_OTP_COMPLETE;
	uint32_t status_reg;

	status_reg = m_flash_ctrl->SR;
	/* Check for memory operation */
	if ( ( status_reg & FLASH_SR_BSY ) == FLASH_SR_BSY )
	{
		status = uC_OTP_BUSY;
	}
	else
	{
		/* Check for errors */
		if ( status_reg & OTP_PGERR )
		{
			status = uC_OTP_ERROR_PG;
		}
		else
		{
			if ( ( status_reg & FLASH_SR_WRPERR ) != 0U )
			{
				status = uC_OTP_ERROR_WRP;
			}
			else
			{
				status = uC_OTP_COMPLETE;
			}
		}
		/* Clear the Error Flags by writing 1 to it*/
		m_flash_ctrl->SR |= OTP_PGERR;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_OTP::Unlock_Flash( void )
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
void uC_OTP::Lock_Flash( void )
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
bool_t uC_OTP::Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	bool_t status = true;
	bool_t lock_status = false;
	uC_OTP_STATUS_EN flash_status;

	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;

	/*Check whether the data region is locked or not */
	lock_status = Is_Locked();
	/* IF false means block is not lock and we can write */
	if ( lock_status == false )
	{
#ifdef OTP_RAM_SIMULATION
		BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( address ), data, length );

		/* Lock the block */
		return_status = Lock();
#else
		/* unlock flash to perform program operation */
		Unlock_Flash();
		/* Check whether flash memory is in operation */
		flash_status = Wait_For_Complete();
		/*We are selecting x64 parallelism */
		while ( ( length > 0U ) && ( flash_status == uC_OTP_COMPLETE ) )
		{
			m_flash_ctrl->CR &= CR_PSIZE_MASK;
			// flash_status = uC_OTP_COMPLETE;
			if ( ( ( length / 4U ) != 0U ) && ( ( address % 4U ) == 0U ) )
			{
				if ( *( reinterpret_cast<uint32_t*>( address ) ) !=
					 *( reinterpret_cast<uint32_t*>( data ) ) )
				{
					/* Write 4 bytes */
					m_flash_ctrl->CR |= ( FLASH_CR_PG | FLASH_CR_PSIZE_1 );
					Write_u32( address, *( ( uint32_t* )data ) );
					flash_status = Wait_For_Complete();
				}
				data += 4U;
				address += 4U;
				length -= 4U;
			}
			else if ( ( length & 0x02U ) && ( ( address % 2U ) == 0U ) )
			{
				if ( *( reinterpret_cast<uint16_t*>( address ) ) !=
					 *( reinterpret_cast<uint16_t*>( data ) ) )
				{
					/* Write 2 bytes */
					m_flash_ctrl->CR |= ( FLASH_CR_PSIZE_0 | FLASH_CR_PG );
					Write_u16( address, *( ( uint16_t* )data ) );
					flash_status = Wait_For_Complete();
				}
				data += 2U;
				address += 2U;
				length -= 2U;
			}
			else if ( length != 0U )
			{
				if ( *( reinterpret_cast<uint8_t*>( address ) ) !=
					 *( reinterpret_cast<uint8_t*>( data ) ) )
				{
					/* Write 1 byte */
					m_flash_ctrl->CR |= FLASH_CR_PG;
					Write_u8( address, *( ( uint8_t* )data ) );
					flash_status = Wait_For_Complete();
				}
				data += 1U;
				address++;
				length--;
			}
			else
			{
				/*Misra Compliance*/
			}

			if ( flash_status != uC_OTP_COMPLETE )
			{
				status = false;
			}
			m_flash_ctrl->CR &= ( ~FLASH_CR_PG );

		}

		/* Lock the block */
		return_status = Lock();
		Lock_Flash();
#endif
		if ( return_status == NV_Ctrl::SUCCESS )
		{
			status = true;
		}
		else
		{
			status = false;
		}
	}
	else
	{
		/* data block is lock so no write can be done */
		status = false;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_OTP::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	bool_t status = false;

	if ( ( status = Check_Range( address, length ) ) == true )
	{
		status = Write_Now( data, address, length );
		if ( status == true )
		{
			status = Check_Data( data, address, length );
			if ( status == true )
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
			return_status = NV_Ctrl::DATA_ERROR;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t uC_OTP::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length ) const
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
		src_data++;
		check_data++;
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_OTP::Get_Lock_Start_Index_Address( void )
{
	uint8_t otp_block_index = ( m_otp_cfg->otp_start_address - OTP_START_ADDRESS ) /
		OTP_MIN_BLK_SIZE;

	return ( OTP_LOCK_BLOCK_ADDRESS + otp_block_index );
}

}
