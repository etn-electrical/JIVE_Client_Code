/**
 *****************************************************************************************
 *	@file   uC_Flash.h header file for flash driver
 *
 *	@brief  The file contains uC_Flash class required for reading,writing and erase on K6X internal
 * flash
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

#include <string.h>

#include "NV_Ctrl.h"
#include "uC_Watchdog.h"
#include "CR_Tasker.h"
#ifdef uC_KINETISK66x_USAGE
#include "MK66F18.h"
#endif
#ifdef uC_KINETISK60x_USAGE
#include "MK60F12.h"
#endif
#ifdef uC_KINETISK60DNx_USAGE
#include "uC_K60D10.h"
#endif
#include "uC_Code_Range.h"
#include "Babelfish_Assert.h"

/* Check if flash swap feature is available */
// #define FTFE_SWAP_SUPPORT \
// //(defined(PSP_HAS_FLASH_SWAP) && PSP_HAS_FLASH_SWAP && defined(BSPCFG_SWAP_INDICATOR_ADDR))

/// * Upper flash swap indicator address - symmetric to BSPCFG_SWAP_INDICATOR_ADDR */
// #define FTFE_SWAP_UPPER_INDICATOR_ADDR \
// //	( BSPCFG_SWAP_INDICATOR_ADDR + ( BSP_INTERNAL_FLASH_SIZE / 2 ) )

namespace NV_Ctrls
{
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
#define FTFE_FLASH_RAM_BUFFER           1024U
#define BYTE_ALIGNMENT                  16U

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Flash : public NV_Ctrl
{
	public:
#if defined ( uC_KINETISK60DNx_USAGE )
		uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FTFL_Type* flash_ctrl,
				  bool_t synchronus_erase = true, cback_func_t cback_func = NULL, cback_param_t param =
				  NULL );
#else
		uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FTFE_Type* flash_ctrl,
				  bool_t synchronus_erase = true, cback_func_t cback_func = NULL, cback_param_t param =
				  NULL );
#endif
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_Flash( void )
		{}

		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read
		 * operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
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
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
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
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
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
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Read the NV_Ctrl device.
		 * @param[in] data: A pointer to hold the read data from flash.
		 * @param[in] address: Address to read data from flash
		 * @param[in] length: length of data to read from flash
		 * @param[in] use_protection: to use protection during read
		 * function.
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = false );

		/**
		 * @brief write the NV_Ctrl device.
		 * @param[in] data: A pointer to hold the write data to flash.
		 * @param[in] address: Address to write data to flash
		 * @param[in] length: length of data to write to flash
		 * @param[in] use_protection: to use protection during write
		 * function.
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool use_protection = false );

		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = uC_FLASH_DEFAULT_ERASE_VAL,
									bool protected_data = false );

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

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual void Continue( void )
		{}

		static void Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		void Flash_Async_Task( void );

		/**
		 **********************************************************************************************
		 * @brief flash status
		 * @n                     0 = UC_FLASH_BUSY
		 * @n                     1 = UC_FLASH_ERROR
		 * @n                     2 = UC_FLASH_ERROR_WRP
		 * @n                     3 = UC_FLASH_COMPLETE
		 * @n                     4 = UC_FLASH_TIMEOUT
		 **********************************************************************************************
		 */
		enum uc_flash_status_enum_t
		{
			UC_FLASH_BUSY,
			UC_FLASH_ERROR,
			UC_FLASH_ERROR_WRP,
			UC_FLASH_COMPLETE,
			UC_FLASH_TIMEOUT
		};

	private:

		bool Write_Now( uint8_t* data, uint32_t dest_address,
						NV_CTRL_LENGTH_TD length );

		bool Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
					   NV_CTRL_LENGTH_TD length ) const;

		bool Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						 NV_CTRL_LENGTH_TD length ) const;

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		bool Erase_Page( uint32_t address );

		uc_flash_status_enum_t Wait_For_Complete( void ) const;

		uc_flash_status_enum_t Get_Status( void ) const;

		void Unlock_Flash( void );

		void Lock_Flash( void );

		uint32_t Get_Sector( uint32_t address ) const;

		void Fill_Ram_Buffer( const uint8_t* data_ram, NV_CTRL_LENGTH_TD length );

#if defined ( uC_KINETISK60DNx_USAGE )
		static void Ftfl_Ram_Function( volatile uint8_t*, volatile uint32_t*, uint8_t cmd );

		/* Returns pointer to byte following last byte of acceleration RAM. */
		const uint8_t* Ftfl_Flash_Ram_End( void );

		/* Flush acceleration RAM contents to flash to_ptr. end_ptr points to the byte following the
		   last written byte in acceleration RAM.
		 * Returns 0 on success; on error a code corresponding to FTFE_STAT error */
		int32_t Ftfl_Flash_Ram_Flush( uint8_t* to_ptr, uint8_t* end,
									  uint16_t len );

		/* On success, return pointer to beginning of acceleration ram; on failure return NULL */
		uint8_t* Ftfl_Flash_Ram_Ready( void );

		uint32_t Ftfl_Flash_Write_Phrase( const uint8_t* from_ptr,
										  uint8_t* to_ptr, unsigned size );

		uint32_t Ftfl_Flash_Erase_Sector( uint8_t* dest_ptr,
										  unsigned size );

		uint32_t Ftfl_Flash_Prog_Check( const uint8_t* from_ptr,
										uint32_t expected );

		static uint32_t Ftfl_Flash_Command_Sequence( uint8_t* command_array,
													 uint8_t count, void* affected_addr,
													 uint32_t affected_size );

#else
		static void Ftfe_Ram_Function( volatile uint8_t*, volatile uint32_t*, uint8_t cmd );

		/* Returns pointer to byte following last byte of acceleration RAM. */
		const uint8_t* Ftfe_Flash_Ram_End( void );

		/* Flush acceleration RAM contents to flash to_ptr. end_ptr points to the byte following the
		   last written byte in acceleration RAM.
		 * Returns 0 on success; on error a code corresponding to FTFE_STAT error */
		int32_t Ftfe_Flash_Ram_Flush( uint8_t* to_ptr, uint8_t* end,
									  uint16_t len );

		/* On success, return pointer to beginning of acceleration ram; on failure return NULL */
		uint8_t* Ftfe_Flash_Ram_Ready( void );

		uint32_t Ftfe_Flash_Write_Phrase( const uint8_t* from_ptr,
										  uint8_t* to_ptr, unsigned size );

		uint32_t Ftfe_Flash_Erase_Sector( uint8_t* dest_ptr,
										  unsigned size );

		uint32_t Ftfe_Flash_Prog_Check( const uint8_t* from_ptr,
										uint32_t expected );

		static uint32_t Ftfe_Flash_Command_Sequence( uint8_t* command_array,
													 uint8_t count, void* affected_addr,
													 uint32_t affected_size );

#endif


		static void Kinetis_Flash_Initialize( void );

		static void Kinetis_Flash_Initialize_End( void );

#if defined ( uC_KINETISK60DNx_USAGE )
		void Ftfl_Flash_Initialize_Banks( void );

#else
		void Ftfe_Flash_Initialize_Banks( void );

#endif

		uC_FLASH_CHIP_CONFIG_TD const* m_chip_cfg;
#if defined ( uC_KINETISK60DNx_USAGE )
		FTFL_Type* m_flash_ctrl_ptr;
#else
		FTFE_Type* m_flash_ctrl_ptr;
#endif

		uint8_t m_erase_val_u8;
		bool_t m_synchronous;
		bool_t m_flash_busy;
		NV_Ctrl::cback_func_t m_call_back_ptr;
		NV_Ctrl::cback_param_t m_cback_param;
		/*
		   Start address of FLASH_SWAP_INDICATOR valid range.
		   The start address cannot be placed in "flash configuration field" (0x400-0x40C)
		   so we use next 32B aligned value - 0x420.
		 */
#if defined ( uC_KINETISK60DNx_USAGE )
		static uint32_t const FTFL_SWAP_VALID_RANGE_START = 0x00000420;
		/* Write sector size. */
		static uint16_t const FTFL_PHRASE_SIZE = 0x0008;
#else
		static uint32_t const FTFE_SWAP_VALID_RANGE_START = 0x00000420;
		/* Write sector size. */
		static uint16_t const FTFE_PHRASE_SIZE = 0x0008;
#endif
		static uint16_t const PROG_RAM = 0x0400;
#if defined ( uC_KINETISK60DNx_USAGE )
		static uint32_t const FLASH_ERR =
			( FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_RDCOLERR_MASK | FTFL_FSTAT_FPVIOL_MASK |
			  FTFL_FSTAT_MGSTAT0_MASK );
#else
		static uint32_t const FLASH_ERR =
			( FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_FPVIOL_MASK |
			  FTFE_FSTAT_MGSTAT0_MASK );
#endif
};

}
#endif
