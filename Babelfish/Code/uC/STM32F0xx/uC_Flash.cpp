/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Flash.h"
#include "Exception.h"
#include "StdLib_MV.h"
#include "uC_Watchdog.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MAX_NUMBER_OF_WRITES        2U
#define MAX_NUMBER_OF_READS         2U

#define uC_FLASH_NV_CHECKSUM_LEN            2U

#define uC_FLASH_PAGE_ALIGNMENT_MASK        ~( m_chip_cfg->page_size - 1U )
#define uC_FLASH_SCRATCH_PAGE_ADDRESS       ( m_chip_cfg->end_address - \
											  ( m_chip_cfg->page_size * 2U ) )

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
	m_chip_cfg( chip_config ),
	m_flash_ctrl( flash_ctrl ),
	m_erase_val( 0U ),
	m_use_ram_buffer( false ),
	m_buffer_loc( nullptr ),
	m_start_buff( nullptr ),
	m_checksum( 0U ),
	m_src_buff{ nullptr, nullptr, 0U },
	m_checksum_buff{ nullptr, nullptr, 0U },
	m_begin_rec_buff{ nullptr, nullptr, 0U },
	m_end_rec_buff{ nullptr, nullptr, 0U }
{
	Unlock_Flash();
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
	uint_fast8_t read_count;

	if ( Check_Range( ( address + m_chip_cfg->start_address ), length,
					  use_protection ) && ( length != 0U ) )
	{
		read_count = 0U;
		do
		{
			read_count++;
			good_data = Read_Now( data, ( address + m_chip_cfg->start_address ), length,
								  use_protection );
		} while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_READS ) );

		// if the data didn't read right, go check the backup location to see if
		// our good data is there.
		if ( ( !good_data ) && ( use_protection == true ) )
		{
			good_data = true;

			read_count = 0U;
			do
			{
				read_count++;
				good_data = Read_Now( data, ( address + m_chip_cfg->mirror_start_address ),
									  length, use_protection );
			} while ( ( good_data == false ) && ( read_count < MAX_NUMBER_OF_READS ) );

			if ( good_data == true )
			{
				good_data = Write_Now( data, ( address + m_chip_cfg->mirror_start_address ),
									   ( address + m_chip_cfg->start_address ), length, use_protection );
			}
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
	uint_fast8_t write_count;
	uint32_t recovery_mem_address;	// Contains the data to be moved over to the dest.

	if ( Check_Range( ( address + m_chip_cfg->start_address ), length, use_protection ) )
	{
		write_count = 0U;

		if ( use_protection == true )
		{
			recovery_mem_address = address + m_chip_cfg->mirror_start_address;
		}
		else
		{
			recovery_mem_address =
				( ( address + m_chip_cfg->start_address ) & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) +
				uC_FLASH_SCRATCH_PAGE_ADDRESS;
		}

		do
		{
			uC_Watchdog::Force_Feed_Dog();

			write_count++;

			good_data = Write_Now( data, recovery_mem_address, ( address + m_chip_cfg->start_address ),
								   length, use_protection );
			if ( good_data == true )
			{
				good_data = Check_Data( data, ( address + m_chip_cfg->start_address ), length,
										use_protection );
			}
		} while ( ( !good_data ) && ( write_count < MAX_NUMBER_OF_WRITES ) );

		if ( ( good_data == true ) && ( use_protection == true ) )
		{
			write_count = 0U;
			do
			{
				uC_Watchdog::Force_Feed_Dog();

				write_count++;

				good_data = Write_Now( data, ( address + m_chip_cfg->start_address ),
									   recovery_mem_address, length, use_protection );
				if ( good_data == true )
				{
					good_data = Check_Data( data, recovery_mem_address, length, use_protection );
				}
			} while ( ( !good_data ) && ( write_count < MAX_NUMBER_OF_WRITES ) );
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

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Write_Now( uint8_t* data, uint32_t recovery_mem_address, uint32_t dest_mem_address,
						  NV_CTRL_LENGTH_TD length, bool use_protection )
{
	BUFF_STRUCT_TD* buff_walker;
	NV_CTRL_LENGTH_TD total_length;

	Push_TGINT();

	m_src_buff.data = data;
	m_src_buff.length = length;
	m_src_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
	m_start_buff = &m_src_buff;

	total_length = length;

	if ( use_protection == true )
	{
		total_length += 2U;

		if ( data == &m_erase_val )	// If we are writing an erase value and we have to sum it up.
		{
			m_checksum = static_cast<uint16_t>( m_erase_val ) *
				static_cast<uint16_t>( m_src_buff.length );
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
		m_checksum_buff.length = uC_FLASH_NV_CHECKSUM_LEN;
		m_checksum_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
		m_src_buff.next = &m_checksum_buff;
	}

	if ( ( dest_mem_address & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) != 0U )	// recovery_address !=
	// recovery_page_address )
	{
		m_begin_rec_buff.data =
			reinterpret_cast<uint8_t*>( recovery_mem_address & uC_FLASH_PAGE_ALIGNMENT_MASK );
		m_begin_rec_buff.length = dest_mem_address & ~uC_FLASH_PAGE_ALIGNMENT_MASK;
		m_begin_rec_buff.next = m_start_buff;
		m_start_buff = &m_begin_rec_buff;
	}

	if ( ( ( dest_mem_address + total_length ) & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) != 0U )// ( recovery_address
	// + total_length
	// ) !=
	// end_page_address
	// )
	{
		m_end_rec_buff.data = reinterpret_cast<uint8_t*>( recovery_mem_address + total_length );
		if ( use_protection == false )
		{
			m_end_rec_buff.data =
				reinterpret_cast<uint8_t*>( recovery_mem_address +
											( reinterpret_cast<uint32_t>( m_end_rec_buff.data ) &
											  ~uC_FLASH_PAGE_ALIGNMENT_MASK ) );
		}
		m_end_rec_buff.length = m_chip_cfg->page_size -
			( ( dest_mem_address + total_length ) & ~uC_FLASH_PAGE_ALIGNMENT_MASK );												//
																																	//
																																	// end_page_address
		// -
		// (
		// dest_address
		// +
		// total_length
		// );
		buff_walker = m_start_buff;
		m_end_rec_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
		while ( buff_walker->next != NULL )
		{
			buff_walker = buff_walker->next;
		}
		buff_walker->next = &m_end_rec_buff;
	}

	Write_Buff( m_start_buff, ( dest_mem_address & uC_FLASH_PAGE_ALIGNMENT_MASK ), use_protection );

	Pop_TGINT();

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Write_Buff( BUFF_STRUCT_TD* buff_list, uint32_t dest_page, bool use_protection )
{
	BUFF_STRUCT_TD* buff_walker;
	uint32_t dest_address;
	uint16_t* dest_ptr;
	uint16_t* src_ptr;

	BF_Lib::SPLIT_UINT16 temp_data;

	buff_walker = buff_list;
	dest_address = dest_page;

	while ( buff_walker != NULL )
	{
		// If we are on the page boundary we erase the current page.
		if ( ( dest_address & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) == 0U )
		{
			// If we are using the scratch space we need to move the page from scratch to the dest
			// area.
			// We skip this operation if this is the first time in here.
			if ( use_protection == false )
			{
				Erase_Page( uC_FLASH_SCRATCH_PAGE_ADDRESS );	// We need to erase the page we took
																// the data from so we can write it
																// back.
				Wait_For_Complete();

				// Yes this seems incorrect but it shouldn't be.
				// We need to take the true destination and copy it to the scratch space.
				src_ptr = reinterpret_cast<uint16_t*>( dest_address );
				dest_ptr = reinterpret_cast<uint16_t*>( uC_FLASH_SCRATCH_PAGE_ADDRESS );
				for ( uint32_t i = 0U; i < m_chip_cfg->page_size; i += 2U )
				{
					m_flash_ctrl->CR |= FLASH_CR_PG;
					*dest_ptr = *src_ptr;
					dest_ptr++;
					src_ptr++;
					Wait_For_Complete();
					m_flash_ctrl->CR &= ~FLASH_CR_PG;
				}
			}
			Erase_Page( dest_address );
			Wait_For_Complete();
		}

		temp_data.u8[0] = *buff_walker->data;
		if ( buff_walker->data != &m_erase_val )
		{
			buff_walker->data++;
		}
		buff_walker->length--;
		if ( buff_walker->length == 0U )
		{
			buff_walker = buff_walker->next;
		}
		temp_data.u8[1] = *buff_walker->data;
		if ( buff_walker->data != &m_erase_val )
		{
			buff_walker->data++;
		}
		buff_walker->length--;
		if ( buff_walker->length == 0U )
		{
			buff_walker = buff_walker->next;
		}
		dest_ptr = reinterpret_cast<uint16_t*>( dest_address );

		m_flash_ctrl->CR |= FLASH_CR_PG;
		*dest_ptr = temp_data.u16;
		dest_address += 2U;	// Because the flash is 16bit write size.
		Wait_For_Complete();
		m_flash_ctrl->CR &= ~FLASH_CR_PG;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											  NV_CTRL_LENGTH_TD length )
{
	return ( Read( reinterpret_cast<uint8_t*>( checksum_dest ), address + length,
				   uC_FLASH_NV_CHECKSUM_LEN, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, NV_CTRL_LENGTH_TD length,
									  uint8_t erase_data, bool protected_data )
{
	m_erase_val = erase_data;

	return ( Write( &m_erase_val, address, length, protected_data ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Erase_Page( uint32_t mem_address )
{
	uC_FLASH_STATUS_EN status;

	status = Wait_For_Complete();

	if ( status == uC_FLASH_COMPLETE )
	{
		/* if the previous operation is completed, proceed to erase the page */
		m_flash_ctrl->CR |= FLASH_CR_PER;
		m_flash_ctrl->AR = mem_address & uC_FLASH_PAGE_ALIGNMENT_MASK;
		m_flash_ctrl->CR |= FLASH_CR_STRT;

		/* Wait for last operation to be completed */
		status = Wait_For_Complete();
		m_flash_ctrl->CR &= ~FLASH_CR_PER;
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
		if ( ( m_flash_ctrl->SR & FLASH_SR_PGERR ) != 0U )
		{
			status = uC_FLASH_ERROR_PG;
		}
		else
		{
			if ( ( m_flash_ctrl->SR & FLASH_SR_WRPERR ) != 0U )
			{
				status = uC_FLASH_ERROR_WRP;
			}
			else
			{
				status = uC_FLASH_COMPLETE;
			}
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
	return ( Erase( 0U, ( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					uC_FLASH_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Read_Now( uint8_t* data, uint32_t mem_address, NV_CTRL_LENGTH_TD length,
						 bool use_protection ) const
{
	bool good_data = false;
	uint8_t* src_data;
	uint16_t checksum;

	BF_Lib::SPLIT_UINT16 read_checksum;

	src_data = reinterpret_cast<uint8_t*>( mem_address );
	checksum = BF_Lib::Copy_String_Ret_Checksum( data, src_data, length );

	if ( use_protection == true )
	{
		read_checksum.u8[0] = src_data[length];
		read_checksum.u8[1] = src_data[length + 1U];
		if ( read_checksum.u16 == checksum )
		{
			good_data = true;
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
bool uC_Flash::Check_Data( uint8_t* check_data, uint32_t mem_address, uint32_t length,
						   bool use_protection ) const
{
	bool good_data = true;
	uint8_t* src_data;

	BF_Lib::SPLIT_UINT16 checksum;

	src_data = reinterpret_cast<uint8_t*>( mem_address );

	checksum.u16 = 0U;
	while ( ( length > 0U ) && ( good_data == true ) )
	{
		length--;
		checksum.u16 = *src_data + checksum.u16;
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

	if ( ( use_protection == true ) && ( good_data == true ) )
	{
		if ( ( src_data[0] != checksum.u8[0] ) ||
			 ( src_data[1] != checksum.u8[1] ) )
		{
			good_data = false;
		}
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD mem_address, NV_CTRL_LENGTH_TD length,
							bool protection ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	if ( protection == true )
	{
		length += uC_FLASH_NV_CHECKSUM_LEN;
		end_address = m_chip_cfg->mirror_start_address;
	}
	else
	{
		end_address = m_chip_cfg->end_address;
	}

	if ( ( mem_address >= m_chip_cfg->start_address ) &&
		 ( ( mem_address + length ) <= end_address ) )
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

}
