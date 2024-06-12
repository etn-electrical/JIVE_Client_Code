/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_FLASH_H
   #define UC_FLASH_H

#include "NV_Ctrl.h"
#include "uC_Watchdog.h"
#include "CR_Tasker.h"
namespace NV_Ctrls
{

static const uint32_t FLASH_PGERR = ( FLASH_SR_PGERR | FLASH_SR_WRPRTERR );
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

typedef struct uC_FLASH_CHIP_CONFIG_TD
{
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;

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
class uC_Flash : public NV_Ctrl
{
	public:
		uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FLASH_TypeDef* flash_ctrl,
				  bool_t isSynchronusErase = true, cback_func_t cback_func = NULL, cback_param_t param =
				  NULL );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Flash( void )
		{}

		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Read( uint8_t* data, uint32_t address, uint32_t length,
								  cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data: A pointer to the buffer used to hold the data for the write operation.
		 * @param[in] address: The address of the first byte to be written.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Write( const uint8_t* data, uint32_t address, uint32_t length,
								   cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Erases a block of memory in the NV_Ctrl device. This may involve a range
		 * larger than specified in the address and length arguments as some devices have
		 * minimum block sizes that can be erased.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase( uint32_t address, uint32_t length,
								   cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Erases the entire NV_Ctrl device.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = false );

		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool use_protection = false );

		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = uC_FLASH_DEFAULT_ERASE_VAL, bool protected_data = false );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		NV_Ctrl::nv_status_t Erase_All( void )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length );

		virtual void Continue( void ){}

		static void Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		void Flash_Async_Task( void );

	private:
		bool Write_Now( uint8_t* data, uint32_t dest_address,
						NV_CTRL_LENGTH_TD length );

		bool Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
					   NV_CTRL_LENGTH_TD length ) const;

		bool Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						 NV_CTRL_LENGTH_TD length ) const;

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		bool Erase_Page( uint32_t address );

		uC_FLASH_STATUS_EN Wait_For_Complete( void ) const;

		uC_FLASH_STATUS_EN Get_Status( void ) const;

		void Unlock_Flash( void );

		void Lock_Flash( void );

		uC_FLASH_CHIP_CONFIG_TD const* m_chip_cfg;
		FLASH_TypeDef* m_flash_ctrl;

		uint8_t m_erase_val;
		bool_t m_synchronous;
		bool_t m_flash_busy;
		NV_Ctrl::cback_func_t m_call_back_ptr;
		NV_Ctrl::cback_param_t m_cback_param;
};
}
#endif
