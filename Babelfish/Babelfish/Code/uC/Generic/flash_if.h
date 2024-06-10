/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __FLASH_IF_H__
#define __FLASH_IF_H__

#include <stdint.h>
#include "Includes.h"
#include "uC_Code_Range.h"
#include "Mem_Check.h"

const uint32_t TOTAL_SECTORS =
	( sizeof ( INTERNAL_FLASH_PAGE_SIZES ) / ( sizeof( uint32_t ) * 2 ) );

/**
 * @brief              This function shall check if the flash area is erasable or not.
 *                     If the flash area is erasable it shall indicate how many sectors
 *                     that we can erase and will update the erase address buffer with the start
 *                     addresses of those buffers.
 * @param[in] s_addr   Address of flash memory from which you want to start erasing memory.
 * @param[in] e_addr   Address of flash memory untill which you want to erase memory.
 * @param[in] e_add_buff   Pointer to the buffer. If the input flash memory area is erasable this
 *                         buffer will get updated with the start addresses of those sectors
 * @param[in] no_sectors_erase  If flash memory region is erasable, this variable will get updated with
 *                              number which will indicate how many sectors in that region need to erased.
 *                              This count menas thes many sectors are not blank.
 * @param[out] bool_t type  true - flash area is erasable
 *                          false - flash area is not erasable.
 */
bool_t Is_Flash_Area_Erasable( uint32_t s_addr, uint32_t e_addr, uint32_t* e_add_buff, uint8_t* no_sectors_erase );

#endif

