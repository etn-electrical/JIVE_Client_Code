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

/* Added 1U so that scratch page address can point to start of the page 
	instead of pointing to end of previous page */
#define uC_FLASH_SCRATCH_PAGE_ADDRESS       ( m_chip_cfg->end_address - ( FLASH_PAGE_SIZE * 2U ) + 1U )

// #define uC_FLASH_WORD_ALIGNMENT_MASK		~(0x01U)

// #define uC_FLASH_CR_PER_RESET                 (static_cast<uint32_t>(0x00001FFDU))

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
	uint32_t recovery_address;		// Contains the data to be moved over to the dest.

	if ( Check_Range( address, length, use_protection ) )
	{
		write_count = 0U;

		if ( use_protection == true )
		{
			recovery_address = ( address - m_chip_cfg->start_address ) +
				m_chip_cfg->mirror_start_address;
		}
		else
		{
			recovery_address = ( address & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) +
								uC_FLASH_SCRATCH_PAGE_ADDRESS;
		}

		do
		{
			uC_Watchdog::Force_Feed_Dog();

			write_count++;

			good_data = Write_Now( data, recovery_address, address, length, use_protection );
			if ( good_data == true )
			{
				good_data = Check_Data( data, address, length, use_protection );
			}
		} while ( ( !good_data ) && ( write_count < MAX_NUMBER_OF_WRITES ) );

		if ( ( good_data == true ) && ( use_protection == true ) )
		{
			write_count = 0U;
			do
			{
				uC_Watchdog::Force_Feed_Dog();

				write_count++;

				good_data = Write_Now( data, address ,
									   recovery_address, length, use_protection );
				if ( good_data == true )
				{
					good_data = Check_Data( data, recovery_address, length, use_protection );
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
bool uC_Flash::Write_Now( uint8_t* data, uint32_t recovery_address, uint32_t dest_address,
						  NV_CTRL_LENGTH_TD length, bool use_protection )
{
    BUFF_STRUCT_TD* buff_walker;    ///< Pointer to new data to be written
    NV_CTRL_LENGTH_TD total_length;

	Push_TGINT();
	/* Add data to be written in flash */
	m_src_buff.data = data;
	m_src_buff.length = length;
	m_src_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
	m_start_buff = &m_src_buff;

	total_length = length;          ///< Length of data to be written

	if ( use_protection == true )   ///< Create checksum and add to the buffer
	{
		total_length += 2U;

		if ( data == &m_erase_val ) ///< If we are writing an erase value and we have to sum it up.
		{
			m_checksum = static_cast<uint16_t>( m_erase_val ) * static_cast<uint16_t>
				( m_src_buff.length );
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

    /* If dest_address (on which new data will be written) is in not start address of the page then 
        take the backup of data before dest_address in m_begin_rec_buff buffer 
        page = m_begin_rec_buff data + New data + m_end_rec_buff data
    */
    if ( ( dest_address & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) != 0U )
    { 
        /* Backup the data available before location at which we are writing new data */
        m_begin_rec_buff.data =
            reinterpret_cast<uint8_t*>( recovery_address & uC_FLASH_PAGE_ALIGNMENT_MASK );
        /* Length of begining data to be retain */
        m_begin_rec_buff.length = dest_address & ~uC_FLASH_PAGE_ALIGNMENT_MASK;
        m_begin_rec_buff.next = m_start_buff;
        m_start_buff = &m_begin_rec_buff;
    }

    if ( ( ( dest_address + total_length ) & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) != 0U )
    {  
		/* Backup the data available after location at which we are writing new data */
        m_end_rec_buff.data = reinterpret_cast<uint8_t*>( recovery_address + total_length );

        /* Length of end data to be retain */
        m_end_rec_buff.length = FLASH_PAGE_SIZE -
            ( ( dest_address + total_length ) & ~uC_FLASH_PAGE_ALIGNMENT_MASK );

        buff_walker = m_start_buff;
        m_end_rec_buff.next = reinterpret_cast<BUFF_STRUCT_TD*>( nullptr );
        while ( buff_walker->next != NULL )
        {
            buff_walker = buff_walker->next;
        }
        /* buff_walker next field will point to end data buffer which is to be retained */
        buff_walker->next = &m_end_rec_buff;
    }
	Write_Buff( m_start_buff, ( dest_address & uC_FLASH_PAGE_ALIGNMENT_MASK ), use_protection );

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
		/* If we are on the page boundary we erase the current page */
		if ( ( dest_address & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) == 0U )
		{
			/* If we are using the scratch space we need to move the page from scratch to the dest
			   area. We skip this operation if this is the first time in here.*/
			if ( use_protection == false )
			{
                /* We need to erase the page we took the data from so we can write it back. */
				Erase_Page( uC_FLASH_SCRATCH_PAGE_ADDRESS );
				Wait_For_Complete();

				/* Yes this seems incorrect but it shouldn't be.
				   We need to take the true destination and copy it to the scratch space.*/
				src_ptr = reinterpret_cast<uint16_t*>( dest_address );
				dest_ptr = reinterpret_cast<uint16_t*>( uC_FLASH_SCRATCH_PAGE_ADDRESS );
				for ( uint32_t i = 0U; i < FLASH_PAGE_SIZE; i += 2U )
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
            /* while loop termination condition will be set here after desried 
               completion of flash write */
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
		dest_address += 2U;		///< Because the flash is 16bit write size.
		Wait_For_Complete();
		m_flash_ctrl->CR &= ~FLASH_CR_PG;
	}
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
	m_flash_ctrl->SR |= ( FLASH_SR_PGERR );
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
bool uC_Flash::Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
						 bool use_protection ) const
{
	bool good_data = false;
	uint8_t* src_data;
	uint16_t checksum;

	BF_Lib::SPLIT_UINT16 read_checksum;

	src_data = reinterpret_cast<uint8_t*>( address );
	checksum = BF_Lib::Copy_String_Ret_Checksum( data, src_data, length );

	if ( use_protection == true )
	{
		read_checksum.u8[0] = data[length];
		read_checksum.u8[1] = data[length + 1U];
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
bool uC_Flash::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length,
						   bool use_protection ) const
{
	bool good_data = true;
	uint8_t* src_data;

	BF_Lib::SPLIT_UINT16 checksum;

	src_data = reinterpret_cast<uint8_t*>( address );

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
bool uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
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

#if 0
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Write_Buff( uint32_t dest_address, uint16_t* recovery_address,
						   BUFF_STRUCT_TD** buff_structs, NV_CTRL_LENGTH_TD* length )
{
	uint16_t* dest_ptr;

	BF_Lib::SPLIT_UINT16 temp_split;
	BUFF_STRUCT_TD* active_buff;

	dest_ptr = ( uint16_t* )( dest_address & uC_FLASH_WORD_ALIGNMENT_MASK );
	active_buff = *buff_structs;

	if ( ( dest_address & ~uC_FLASH_WORD_ALIGNMENT_MASK ) != 0U )
	{
		temp_split.u8[0] = ( *recovery_address & 0xFF00U ) >> 8U;
		temp_split.u8[1] = *active_buff->src_data;
		*dest_ptr = temp_split.u16;
		active_buff->length--;
		active_buff->src_data++;
		dest_ptr++;
		*length--;
		if ( ( active_buff->next != NULL ) &&
			 ( active_buff->length == 0U ) )
		{
			active_buff = active_buff->next;
		}
		Wait_For_Complete();
	}
	while ( ( *length > 1U ) && ( active_buff->length > 1U ) )
	{
		temp_split.u8[0] = *active_buff->src_data;
		active_buff->src_data++;
		temp_split.u8[1] = *active_buff->src_data;
		active_buff->src_data++;
		*dest_ptr = temp_split.u16;
		active_buff->length -= 2U;
		dest_ptr++;
		*length -= 2U;
		if ( ( active_buff->next != NULL ) &&
			 ( active_buff->length == 0U ) )
		{
			active_buff = active_buff->next;
		}
		Wait_For_Complete();
	}
	if ( ( *length == 1U ) && ( active_buff->length > 0U ) )
	{
		temp_split.u8[0] = *active_buff->src_data;
		temp_split.u8[1] = *recovery_address & 0x00FFU;
		*dest_ptr = temp_split.u16;
		active_buff->length--;
		active_buff->src_data++;
		dest_ptr++;
		*length--;
		Wait_For_Complete();
		if ( ( active_buff->next != NULL ) &&
			 ( active_buff->length == 0U ) )
		{
			active_buff = active_buff->next;
		}
	}
	if ( ( *length > 1U ) && ( active_buff->length == 1U ) )
	{
		temp_split.u8[0] = *active_buff->src_data;
		temp_split.u8[1] = *recovery_address & 0x00FFU;
		*dest_ptr = temp_split.u16;
		active_buff->length--;
		active_buff->src_data++;
		dest_ptr++;
		*length--;
		Wait_For_Complete();
		if ( ( active_buff->next != NULL ) &&
			 ( active_buff->length == 0U ) )
		{
			active_buff = active_buff->next;
		}
	}
	*buff_structs = active_buff;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Write_Page( AT25_WRITE_STRUCT* write_st )
{
	BUFF_STRUCT_TD begin_rec_buff;
	BUFF_STRUCT_TD end_rec_buff;
	BUFF_STRUCT_TD* buff_walker;
	uint32_t end_page_address;
	uint32_t start_page_address;
	uint32_t start_address;
	NV_CTRL_LENGTH_TD total_length;

	Push_TGINT();

	Erase_Page( write_st->dest_page );

	m_start_buff = m_src_buff;

	m_src_buff.data = data;
	m_src_buff.length = length;
	m_src_buff.next = NULL;
	m_data_buff = m_src_buff;
	total_length = length;

	if ( use_protection == true )
	{
		total_length += 2U;

		m_checksum = 0U;
		for ( NV_CTRL_LENGTH_TD i = 0U; i < m_src_buff.length; i++ )
		{
			m_checksum += m_src_buff.data[i];
		}
		m_checksum_buff.data = m_checksum;
		m_checksum_buff.length = uC_FLASH_NV_CHECKSUM_LEN;
		m_checksum_buff.next = NULL;
		m_src_buff.next = m_checksum_buff;
	}

	if ( start_address != start_page_address )
	{
		begin_rec_buff.data = start_page_address;		// write_st->current_dest_ptr;
		begin_rec_buff.length = start_address - start_page_address;
		begin_rec_buff.next = m_start_buff;
		m_start_buff = &begin_rec_buff;
	}

	end_page_address = ( start_page_address + total_length + FLASH_PAGE_SIZE ) &
		uC_FLASH_PAGE_ALIGNMENT_MASK;

	if ( ( start_address + total_length ) != end_page_address )
	{
		end_rec_buff.data = start_address + total_length;
		end_rec_buff.length = end_page_address - ( start_address + total_length );
		buff_walker = m_start_buff;
		end_rec_buff.next = NULL;
		while ( buff_walker->next != NULL )
		{
			buff_walker = buff_walker->next;
		}
		buff_walker->next = end_rec_buff;
	}

	Pop_TGINT();
}

uint32_t end_page_address;
uint32_t active_page_address;
uint16_t write_count;
NV_Ctrl::nv_status_t return_status;
uint8_t intermediate_status;

if ( use_protection == true )
{
	write_count = 0U;
	do
	{
		active_page_address = address & uC_FLASH_PAGE_ALIGNMENT_MASK;
		end_page_address = ( address + length + uC_FLASH_NV_CHECKSUM_LEN ) &
			uC_FLASH_PAGE_ALIGNMENT_MASK;

		m_write_st.src_page = ( active_page_address + m_chip_cfg->mirror_start_address );
		m_write_st.dest_page = active_page_address;
		m_write_st.current_dest_ptr = ( uint16_t* )address;
		m_write_st.src_ptr = data;
		m_write_st.src_length_remain = length;
		m_write_st.checksum.u16 = 0U;
		m_write_st.checksum_len_remain = uC_FLASH_NV_CHECKSUM_LEN;

		while ( active_page_address <= end_page_address )
		{
			Write_Page( &m_write_st );
			active_page_address += m_chip_cfg->erase_page_size;
		}
		intermediate_status = Check_Data( data, address, length, use_protection );
	} while ( ( write_count < MAX_NUMBER_OF_uC_FLASH_WRITES ) &&
			  ( intermediate_status == false ) );

	if ( intermediate_status == true )
	{
		write_count = 0U;
		do
		{
			active_page_address = address & uC_FLASH_EPAGE_ALIGNMENT_MASK;
			m_write_st.src_page = active_page_address;
			m_write_st.dest_page = ( active_page_address + m_chip_cfg->mirror_start_address );
			m_write_st.src_ptr = NULL;
			while ( active_page_address <= end_page_address )
			{
				Write_Page( &m_write_st );
				active_page_address += m_chip_cfg->erase_page_size;
			}
			intermediate_status = Check_Data( data,
											  ( address + m_chip_cfg->mirror_start_address ),
											  length, use_protection );
		} while ( ( write_count < MAX_NUMBER_OF_uC_FLASH_WRITES ) &&
				  ( intermediate_status == false ) );
	}
}
else
{
	write_count = 0U;
	do
	{
		active_page_address = address & uC_FLASH_EPAGE_ALIGNMENT_MASK;
		end_page_address = ( address + length ) & uC_FLASH_EPAGE_ALIGNMENT_MASK;

		m_write_st.current_dest_ptr = ( uint16_t* )address;
		m_write_st.src_length_remain = length;
		m_write_st.checksum.u16 = 0U;
		m_write_st.checksum_len_remain = 0U;

		while ( active_page_address <= end_page_address )
		{
			m_write_st.src_ptr = NULL;		// This forces a copy from one page to another.
			m_write_st.src_page = active_page_address;
			m_write_st.dest_page = uC_FLASH_SCRATCH_EPAGE_ADDRESS;
			Write_Page( &m_write_st );

			if ( data != &m_write_st.single_value )
			{
				m_write_st.src_ptr = &data[length - m_write_st.src_length_remain];
			}
			else
			{
				m_write_st.src_ptr = data;
			}
			m_write_st.src_page = uC_FLASH_SCRATCH_EPAGE_ADDRESS;
			m_write_st.dest_page = active_page_address;
			Write_Page( &m_write_st );

			active_page_address += m_chip_cfg->erase_page_size;
		}
		intermediate_status = Check_Data( data, address, length, use_protection );
	} while ( ( write_count < MAX_NUMBER_OF_uC_FLASH_WRITES ) &&
			  ( intermediate_status == false ) );
}

if ( intermediate_status == false )
{
	return_status = NV_Ctrl::DATA_ERROR;
}
else
{
	return_status = NV_Ctrl::SUCCESS;
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************

   BOOL uC_Flash_Ctrl::Write( uint8_t* data, UINT32 address, UINT32 length )
   {
    volatile uint16_t* recovery_ptr;
    uint16_t* dest_ptr;
    uint16_t temp_data;

    uint8_t* temp_u8_ptr;
    uint_fast8_t temp_data;
    uint_fast8_t align_ctr;
    BOOL short_write = false;

    if ( length > 0 )
    {
        recovery_ptr = (uint16_t*)( address & uC_FLASH_PAGE_ALIGNMENT_MASK );
        dest_ptr = (uint16_t*)( address & uC_FLASH_WORD_ALIGNMENT_MASK );

        while ( recovery_ptr < dest_ptr )
        {
 * recovery_ptr = *recovery_ptr;
            recovery_ptr++;
        }

        temp_data = *dest_ptr;
        temp_u8_ptr = (uint8_t*)&temp_data;
        temp_u8_ptr += ( address & ~uC_FLASH_WORD_ALIGNMENT_MASK );
        align_ctr = address & ~uC_FLASH_WORD_ALIGNMENT_MASK;

        if ( ( length < sizeof(uint_fast8_t) ) && ( ( (uint_fast8_t)dest_ptr &
 * uC_FLASH_PAGE_SIZE_MASK ) == 0 ) )
        {
            short_write = true;
        }

        while ( length > 0 )
        {
            if ( data == NULL )
            {
 * temp_u8_ptr = uC_FLASH_ERASE_VAL;
            }
            else
            {
 * temp_u8_ptr = *data;
                data++;
            }
            temp_u8_ptr++;
            length--;
            align_ctr++;

            if ( align_ctr >= sizeof(uint_fast8_t) )
            {
                align_ctr = 0;
 * dest_ptr = temp_data;
                dest_ptr++;
                if ( ( ( (uint_fast8_t)dest_ptr ) & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) == 0 )
                {
                    Write_Page_Now( Get_Page_Num( dest_ptr - sizeof(uint_fast8_t) ) );
                }
                temp_data = *dest_ptr;
                temp_u8_ptr = (uint8_t*)&temp_data;
            }
        }

        if ( ( ( (uint_fast8_t)dest_ptr & uC_FLASH_PAGE_SIZE_MASK ) != 0 ) || ( short_write ==
 * true ) )
        {
            if ( align_ctr != 0 )
            {
 * dest_ptr = temp_data;
                dest_ptr++;
            }
            recovery_ptr = dest_ptr;

            while ( ( ( (uint_fast8_t)recovery_ptr ) & ~uC_FLASH_PAGE_ALIGNMENT_MASK ) != 0)
            {
 * recovery_ptr = *recovery_ptr;
                recovery_ptr++;
            }

            Write_Page_Now( Get_Page_Num( recovery_ptr - sizeof(uint_fast8_t) ) );
        }
    }

    return ( true );
   }
 */

if ( ( write_st->src_length_remain > 0U ) &&
	 ( ( ( MBASE )word_dest_ptr < next_page ) ) &&
	 ( ( ( MBASE )write_st->current_dest_ptr & ~uC_FLASH_WORD_ALIGNMENT_MASK ) != 0U ) )
{
	m_flash_ctrl->CR |= FLASH_CR_PG;
	*word_dest_ptr = ( *word_src_ptr & 0x00FFU ) | ( ( *write_st->src_ptr ) << 8U );
	word_dest_ptr++;
	write_st->src_ptr++;
	write_st->src_length_remain--;
	Wait_For_Complete();
}

while ( ( write_st->src_length_remain > 0U ) &&
		( ( MBASE )word_dest_ptr < next_page ) )
{
	temp_write.u8[0] = *write_st->src_ptr;
	write_st->src_ptr++;
	temp_write.u8[1] = *write_st->src_ptr;
	write_st->src_ptr++;
	m_flash_ctrl->CR |= FLASH_CR_PG;
	*word_dest_ptr = temp_write.u16;
	word_dest_ptr++;
	write_st->src_length_remain--;
	Wait_For_Complete();
}

if ( ( write_st->src_length_remain > 0U ) &&
	 ( ( ( MBASE )word_dest_ptr < next_page ) ) &&
	 ( ( ( MBASE )write_st->src_ptr & ~uC_FLASH_WORD_ALIGNMENT_MASK ) != 0U ) )
{
	m_flash_ctrl->CR |= FLASH_CR_PG;
	*word_dest_ptr = ( *word_src_ptr & 0xFF00U ) | *write_st->src_ptr;
	word_dest_ptr++;
	write_st->src_ptr++;
	write_st->src_length_remain--;
	Wait_For_Complete();
}

if ( ( write_st->src_length_remain > 0U ) && ( write_st->checksum_len_remain > 0U ) )
{
	// If our alignment is on a 16bit boundary then we can just write the whole word.
	if ( ( MBASE )write_st->src_ptr & ~uC_FLASH_WORD_ALIGNMENT_MASK )
	{
		==
		0U )
		{
			if ( write_st->checksum_len_remain == uC_FLASH_NV_CHECKSUM_LEN )
			{}
			else
			{}

		}
		else
		{}
	}
	if ( write_st->checksum_len_remain > 0U )
		&&
		( ( ( MBASE )word_dest_ptr < next_page ) ) &&
		( ( ( MBASE )write_st->src_ptr & ~uC_FLASH_WORD_ALIGNMENT_MASK ) != 0U ) )
		{
			m_flash_ctrl->CR |= FLASH_CR_PG;
			*word_dest_ptr = ( *word_src_ptr & 0x00FFU ) | ( ( *write_st->src_ptr ) << 8U );
			word_dest_ptr++;
			write_st->src_ptr++;
			write_st->src_length_remain--;
			Wait_For_Complete();
		}
}
	/*
	 *****************************************************************************************
	 * See header file for function definition.
	 *****************************************************************************************
	 */
	bool uC_Flash::Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
	{
		uint8_t status = 0U;
		volatile uint32_t busy_ctr = 0U;

		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( uC_FLASH_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.
		Delay_Chip_Select_Change();

		m_spi_ctrl->Select_Chip( m_chip_select );

		Send_Data_Command( uC_FLASH_WRITE_MEMORY_ADD_DATA_OPCODE, address );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->Set_TX_DMA( data, length );
		m_spi_ctrl->Enable_TX_DMA();

		while ( m_spi_ctrl->TX_DMA_Busy() )
		{}									//! m_spi_ctrl->TX_Shift_Empty() ){};

		m_spi_ctrl->Disable_TX_DMA();

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		do
		{
			busy_ctr++;
			Read_Status( &status );
		} while ( Test_Bit( status, uC_FLASH_STATUS_BUSY_BIT ) );

		return ( true );
	}

#endif
}
