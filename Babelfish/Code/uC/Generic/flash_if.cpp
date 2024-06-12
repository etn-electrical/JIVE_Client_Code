/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "flash_if.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Is_Flash_Area_Erasable( uint32_t s_addr, uint32_t e_addr, uint32_t* e_add_buff, uint8_t* no_sectors_erase )
{
	uint32_t page_addr = s_addr;
	uint32_t start_addr = INTERNAL_FLASH_START_ADDRESS;
	bool_t ret_val = false;
	uint8_t erase_sector_count = 0;
	uint8_t sector_index = 0U;	/* number of sectors to be erased */
	uint32_t erase_addr_array[TOTAL_SECTORS + 1U];

	if ( ( e_addr > s_addr ) && ( s_addr > ( uint32_t ) uC_App_Fw_End_Address() ) )
	{
		uint32_t index;
		for ( index = 0; ( index < TOTAL_SECTORS ) && ( start_addr <= e_addr ); index++ )
		{
			if ( page_addr < ( start_addr + INTERNAL_FLASH_PAGE_SIZES[index].sector_size ) )
			{
				erase_addr_array[sector_index++] = start_addr;
				page_addr += INTERNAL_FLASH_PAGE_SIZES[index].sector_size;
			}
			start_addr += INTERNAL_FLASH_PAGE_SIZES[index].sector_size;
		}

		if ( !( index >= TOTAL_SECTORS ) )
		{
			ret_val = true;
			// required to blank check till last address
			erase_addr_array[sector_index] = e_addr + 1U;
			/* Also Calculate the number of sectors which need to be erased */
			for ( uint8_t i = 0; i < sector_index; i++ )
			{
				if ( !BF_Misc::Mem_Check::Blank_Check( ( uint8_t* )erase_addr_array[i],
													   ( uint8_t* )( erase_addr_array[i + 1] - 1U ), 0xFF ) )
				{
					*e_add_buff++ = erase_addr_array[i];
					erase_sector_count++;
				}
			}

			*no_sectors_erase = erase_sector_count;
		}
	}

	return ( ret_val );
}