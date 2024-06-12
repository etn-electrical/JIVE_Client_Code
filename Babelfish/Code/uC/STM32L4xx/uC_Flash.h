/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef UC_FLASH_H
#define UC_FLASH_H

#include "NV_Ctrl.h"
#include "uC_Watchdog.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{
// static const uint32_t FLASH_PGERR = ( FLASH_SR_ERSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR );
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

typedef struct uC_FLASH_CHIP_CONFIG_TD
{
	uint32_t const* page_sizes;
	uint32_t page_index_start;
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;

	uint32_t byte_write_time;		// (in microseconds)
	uint32_t page_erase_time;
} uC_FLASH_CHIP_CONFIG_TD;

#define uC_FLASH_DEFAULT_ERASE_VAL      0xFFU

typedef enum uC_FLASH_STATUS_EN
{
	uC_FLASH_BUSY,
	uC_FLASH_ERROR_PG,
	uC_FLASH_ERROR_WRP,
	uC_FLASH_COMPLETE,
	uC_FLASH_TIMEOUT
} uC_FLASH_STATUS_EN;

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Flash
{
	public:
		uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FLASH_TypeDef* flash_ctrl );
		~uC_Flash( void );

		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool_t use_protection = FALSE );

		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool_t use_protection = FALSE );

		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = uC_FLASH_DEFAULT_ERASE_VAL,
									bool_t protected_data = FALSE );

		NV_Ctrl::nv_status_t Erase_All( void );

		NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length );

		/**
		 * @brief Calculates time required for write operation.
		 * @param[in] data: Used to analyze number of bytes which has value FLASH_ERASED_VALUE. Does not require
		 * write operation for such bytes.
		 * @param[in] address: Address to write data to flash
		 * @param[in] length: length of data to write to flash
		 * @param[in] include_erase_time:  Flag to include erase time
		 * function.
		 * @return Operation time in milli second.
		 */
		uint32_t Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							 NV_CTRL_LENGTH_TD length, bool include_erase_time = true ) const;

		/**
		 * @brief Calculates time required for read operation. Not supported for this controller and need to define it
		 * properly. BF_ASSERT will be trigered by calling this API.
		 * @param[in] data: Not used inside read operation.
		 * @param[in] address: Address to read data from flash
		 * @param[in] length: length of data to read from flash
		 * function.
		 * @return Operation time in milli second.
		 */
		uint32_t Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							NV_CTRL_LENGTH_TD length ) const
		{
			BF_ASSERT( false );
			return ( 0 );
		}

		/**
		 * @brief Calculates time required for erase operation
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return Operation time in milli second.
		 */
		uint32_t Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
		{
			return ( Write_Time( NULL, address, length ) );
		}

		bool_t Erase_Page( uint32_t address );

	private:
		bool_t Write_Now( uint8_t* data, uint32_t dest_address,
						  NV_CTRL_LENGTH_TD length );

		bool_t Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
						 NV_CTRL_LENGTH_TD length );

		bool_t Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						   NV_CTRL_LENGTH_TD length );

		bool_t Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		uC_FLASH_STATUS_EN Wait_For_Complete( void );

		uC_FLASH_STATUS_EN Get_Status( void );

		void Unlock_Flash( void );

		void Lock_Flash( void );

		uint32_t Get_Sector( uint32_t address ) const;

		inline uint32_t Get_Page_Size( uint32_t address ) const
		{
			return ( m_chip_cfg->page_sizes[Get_Sector( address )] );
		}

		inline uint32_t Get_Page_Mask( uint32_t address )   { return ( ~( Get_Page_Size( address ) - 1U ) );}

		uC_FLASH_CHIP_CONFIG_TD const* m_chip_cfg;
		FLASH_TypeDef* m_flash_ctrl;

		uint8_t m_erase_val;
};

}
#endif
