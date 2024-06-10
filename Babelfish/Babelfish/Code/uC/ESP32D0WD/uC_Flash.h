/**
 *****************************************************************************************
 * @file		uC_Flash.h
 *
 * @brief		This file provides the access to the flash memory for
 * @n			Read, Write and Erase operations.
 *
 * @copyright	2019 Eaton Corporation All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_FLASH_H
#define UC_FLASH_H

#include "esp_spi_flash.h"
#include "esp_flash_partitions.h"
#include "esp_ota_ops.h"
#include "esp32/rom/spi_flash.h"
#include "uC_Base.h"
#include "NV_Ctrl.h"
#include "CR_Tasker.h"

/**
 *****************************************************************************************
 * @brief									NV_Ctrls namespace.
 * @n										It is derived from the NV_Ctrls.
 * @n										This Provides the scope to the function like Read, Write Erase
 * @n										and variables inside it.
 *****************************************************************************************
 */
namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t uC_FLASH_DEFAULT_ERASE_VAL = 0xFFU;			///< Flash Erase Value.
static const uint8_t ENABLE_VAL = 1U;								///< Enable Value.

/**
 * @brief								Structure used to define the start and end address of flash.
 */
struct uC_FLASH_CHIP_CONFIG_TD
{
	uint32_t start_address;					///< Start address of flash.
	uint32_t mirror_start_address;			///< Mirror start address of flash details.
	uint32_t end_address;					///< End address of flash.
};

/**
 * @brief								Enum used to define Status of Flash.
 */
enum uC_FLASH_STATUS_EN
{
	uC_FLASH_BUSY,							///< Flash busy status.
	uC_FLASH_ERROR_PG,						///< Flash error status (PG).
	uC_FLASH_ERROR_WRP,						///< Flash error status (WRP).
	uC_FLASH_COMPLETE,						///< Flash complete status.
	uC_FLASH_TIMEOUT						///< Flash timeout status.
};

/**
 * @brief								Structure used to define the status of flash.
 * @n									(Not used in ESP32)
 */
struct FLASH_TypeDef
{};

/**
 ****************************************************************************************
 * @brief								The uC_Flash class contains implementations required for the
 * @n									access of the Flash memory.
 * @details								It provides functionalities to read, write, erase the Flash memory.
 ****************************************************************************************
 */
class uC_Flash : public NV_Ctrl
{
	public:
		/**
		 * @brief						Constructor to create instance of uC_Flash class.
		 * @param[in] chip_config:		Structure pointer for configure Flash.
		 * @param[in] flash_ctrl:		Structure pointer contains the flash register details.
		 * @param[in] synchronus_erase:	synchronous erase.
		 * @param[in] cback_func:		A pointer to the function to be called when the operation is
		 * @n							complete.
		 * @param[in] param:			A void pointer to an object passed as an argument to the cback
		 * @n							function.
		 * @param[in] erase_on_boundary:Flash erase on boundary.
		 * @return Void					None
		 */
		uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config = nullptr, FLASH_TypeDef* flash_ctrl = nullptr,
				  bool synchronus_erase = true, cback_func_t cback_func = NULL, cback_param_t param = NULL,
				  bool erase_on_boundary = false );

		/**
		 * @brief						Destructor to delete an instance of uC_Flash class
		 * @return Void					None.
		 */
		~uC_Flash( void );

		/**
		 * @brief						Reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data:				A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address:			The address of the first byte to be read.
		 * @param[in] length:			The number of bytes to be read.
		 * @param[in] cback:			A pointer to the function to be called when the operation is complete.
		 * @param[in] param:			A void pointer to an object passed as an argument to the cback function.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read( uint8_t* data, uint32_t address, uint32_t length, cback_func_t cback,
								   cback_param_t param );

		/**
		 * @brief						Writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data:				A pointer to the buffer used to hold the data for the write
		 * @n							operation.
		 * @param[in] address:			The address of the first byte to be written.
		 * @param[in] length:			The number of bytes to be written.
		 * @param[in] cback:			A pointer to the function to be called when the operation is
		 * @n							complete.
		 * @param[in] param:			A void pointer to an object passed as an argument to the cback
		 * @n							function.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Write( const uint8_t* data, uint32_t address, uint32_t length, cback_func_t cback,
									cback_param_t param );

		/**
		 * @brief						Erases a block of memory in the NV_Ctrl device. This may involve a range
		 * @n							larger than specified in the address and length arguments as some devices
		 * @n							have minimum block sizes that can be erased.
		 * @param[in] address:			The address of the first byte to be erased.
		 * @param[in] length:			The number of bytes to be erased.
		 * @param[in] cback:			A pointer to the function to be called when the operation is complete.
		 * @param[in] param:			A void pointer to an object passed as an argument to the cback function.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase( uint32_t address, uint32_t length, cback_func_t cback, cback_param_t param );

		/**
		 * @brief						Erases the entire NV_Ctrl device.
		 * @param[in] cback:			A pointer to the function to be called when the operation is complete.
		 * @param[in] param:			A void pointer to an object passed as an argument to the cback function.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase_All( cback_func_t cback, cback_param_t param );

		/**
		 * @brief						Reads a specified number of bytes from a given
		 * @n							address into a buffer.
		 * @param[in] data:				A pointer to the buffer used to receive the data from the read
		 * @n							operation.
		 * @param[in] address:			The address of the first byte to be read.
		 * @param[in] length:			The number of bytes to be read.
		 * @param[in] use_protection:	Use protection
		 * @n							is true use flash encryption
		 * @n							is false no encryption is required.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, bool
								   use_protection = false );

		/**
		 * @brief						Writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data:				A pointer to the buffer used to hold the data for the write
		 * @n							operation.
		 * @param[in] address:			The address of the first byte to be written.
		 * @param[in] length:			The number of bytes to be written.
		 * @param[in] use_protection:	Use protection
		 * @n							is true use flash encryption
		 * @n							is false no encryption is required.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									bool use_protection = false );

		/**
		 * @brief						Erases a block of memory in the NV_Ctrl device.
		 * @n							This may involve arrange
		 * @n							larger than specified in the address and length arguments as some
		 * @n							devices have minimum block sizes that can be erased.
		 * @param[in] address:			The address of the first byte to be erased.
		 * @param[in] length:			The number of bytes to be erased.
		 * @param[in] erase_data:		The default erase value.
		 * @param[in] protected_data:	Data protected details.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = uC_FLASH_DEFAULT_ERASE_VAL, bool protected_data = false );

		/**
		 * @brief						Erase the all the data in Flash.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase_All( void );

		/**
		 * @brief						Read checksum value
		 * @param[in] checksum_dest:		checksum pointer buffer used to receive the data from the read
		 * operation.
		 * @param[in] address:			The address of the first byte to be read.
		 * @param[in] length:			The number of bytes to be read.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length );

		/**
		 * @brief						Calculate the read time.
		 * @param[in] data:				A pointer to the buffer used to hold the data for the read
		 * @n							operation.
		 * @param[in] address:			The address of the first byte to be read.
		 * @param[in] length:			The number of bytes to be read.
		 * @return uint32_t				status of requested operation.
		 */
		uint32_t Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief						Calculate the write time .
		 * @param[in] data:				A pointer to the buffer used to hold the data for the write
		 * @n							operation.
		 * @param[in] address:			The address of the first byte to be written.
		 * @param[in] length:			The number of bytes to be written.
		 * @return uint32_t				status of requested operation.
		 */
		uint32_t Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
							 bool include_erase_time = true ) const;

		/**
		 * @brief						calculate the Erase time .
		 * @param[in] address:			The address of the first byte to be written.
		 * @param[in] length:			The number of bytes to be written.
		 * @return uint32_t				status of requested operation.
		 */
		uint32_t Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		/**
		 * @brief Get wait time which is introduced due to last operation
		 * @return wait time in mili-second.
		 */
		uint32_t Get_Wait_Time( void ) { return ( m_wait_time ); }

		/**
		 * @brief						Continue the operation of Flash.
		 * @return void					None
		 */
		virtual void Continue( void )
		{}

		/**
		 * @brief						Flash Async Task Static.
		 * @param[in] cr_task:			pointer to the function contains Cr_tasker value.
		 * @param[in] param:				passes the parameter for function.
		 * @return void.				None
		 */
		static void Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		/**
		 * @brief			Flash asynchronous task.
		 * @param[in] cr_task:			pointer to the function contains Cr_tasker value.
		 * @return void		None
		 */
		void Flash_Async_Task( CR_Tasker* cr_task );

		/**
		 * @brief						Attach the call back function.
		 * @param[in] func_callback:	A pointer to the function to be called when the operation is complete.
		 * @param[in] func_param:		A void pointer to an object passed as an argument to the cback function.
		 * @return void.				None
		 */
		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param );

		/**
		 * @brief						Get the status of Flash.
		 * @param[in] address:			Address of the Page.
		 * @param[in] length:			The number of bytes.
		 * @return uC_FLASH_STATUS_EN	status of requested operation.
		 */
		uC_FLASH_STATUS_EN Get_Status( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief						Erase sector
		 * @param[in] sec_no:			Sector number which sector to be erased.
		 * @return nv_status_t			status of requested operation.
		 */
		uC_FLASH_STATUS_EN Erase_Sector( uint32_t sec_no );

		/**
		 * @brief						Returns the memory range information
		 * @param[in] :					mem_range_info: range information will get updated her
		 * @return void.				None
		 */
		void Get_Memory_Info( mem_range_info_t* mem_range_info );

	private:
		/**
		 * @brief						Copy Constructor and Copy Assignment Operator.
		 * @details						Copy Constructor and Copy Assignment Operator
		 * definitions					made private to disallow usage.
		 * @n							The fact that we are disallowing these standard items should be Doxygen
		 * @n							documented.
		 * @n							Generally private members do not need to be Doxygen documented.
		 */
		uC_Flash( const uC_Flash& );
		uC_Flash & operator =( const uC_Flash& object );

		/**
		 * @brief						Function to get the page size information.
		 * @param[in] address			Address of the Page.
		 * @return  uint32_t			Page size value.
		 */
		inline uint32_t Get_Page_Size( uint32_t address ) const { return ( g_rom_flashchip.page_size );}

		/**
		 * @brief						Function to get the page mask information.
		 * @param[in] address			Address of the Page.
		 * @return  uint32_t			Page mask value.
		 */
		inline uint32_t Get_Page_Mask( uint32_t address ) { return ( ~( Get_Page_Size( address ) - ENABLE_VAL ) );}

		/**
		 * @brief						Unlock Flash.
		 * @return void					None
		 */
		void Unlock_Flash( void );

		/**
		 * @brief						Lock Flash.
		 * @return void					None
		 */
		void Lock_Flash( void );

		/**
		 * @brief						Check the address range valid or not.
		 * @param[in] address			The address of the Page to be erased.
		 * @return bool					status of requested operation.
		 */
		bool Erase_Page( uint32_t address );

		/**
		 * @brief						Check the address range valid or not.
		 * @param[in] address			The address of the first byte of the data.
		 * @param[in] length			The number of bytes of  to be checked.
		 * @return bool					status of requested operation.
		 */
		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief function to erase flash from the specified address location to given length.
		 * @param[in] address			Start address
		 * @param[in] length			length of data
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase_Now( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		/**
		 * @brief function to write specified data buffer into flash
		 * @param[in] data				data buffer to be written into flash
		 * @param[in] address			Start address
		 * @param[in] length			length of data
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		/**
		 * @brief function to read given length of data from the specified address location
		 * @param[out] data				read buffer to store the data read from the flash
		 * @param[in] address			Start address
		 * @param[in] length			length of data
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		/**
		 * @brief Asynchronous erase data handler
		 * @return void					None
		 */
		void asynch_erase_handler( void );

		/**
		 * @brief Asynchronous write data handler
		 * @return void					None
		 */
		void asynch_write_handler( void );

		/**
		 * @brief Asynchronous read data handler
		 * @return void					None
		 */
		void asynch_read_handler( void );

		/**
		 * @brief						Error conversion from ESP error type to NV_Ctrl status
		 * @param[in] spi_flash_error	ESP error type.
		 * @return nv_status_t			status of requested operation.
		 */
		NV_Ctrl::nv_status_t Interpret_SPI_Flash_Error( esp_err_t spi_flash_error );

		/**
		 * @brief						Private member variable of uc flash config structure.
		 *@n details					Chip configuration structure pointer.
		 */
		uC_FLASH_CHIP_CONFIG_TD const* m_chip_cfg;

		/**
		 * @brief						Private member variable Flash typedef structure.
		 * @n details					Flash control structure pointer.
		 */
		FLASH_TypeDef* m_flash_ctrl;

		/**
		 * @brief						Private member variable for a uint32_t.
		 *@n details					Used to get the wait time after read/write/erase operation.
		 */
		uint32_t m_wait_time;

		/**
		 * @brief						Private member variable for a uint8_t.
		 *@n details					Erase value details.
		 */
		uint8_t m_erase_val;

		/**
		 * @brief						Private member variable for a bool.
		 *@n							Synchronous or ASynchronous details.
		 *@n							Flash busy or not information
		 *@n							Erase boundary information.
		 */
		bool m_synchronus;
		bool m_flash_busy;
		bool m_erase_on_boundary;

		/**
		 * @brief						Private member variable of NV_Ctrl class.
		 *@n details					Protect from simultaneous read/write.
		 *@n							Used to maintain state for asynchronous operation.
		 *@n							Used to store the operation result status.
		 *@n							Used in asynchronous read/write/erase operation.
		 */
		static NV_Ctrl::semaphore_status_t m_semaphore;
		NV_Ctrl::nv_state_t m_state;
		NV_Ctrl::nv_status_t m_status;
		NV_Ctrl::asynch_service_struct_t m_asynch_data;

		/**
		 * @brief						Private member variable of CR_Tasker class.
		 *@n details					CR Tasker pointer.
		 */
		CR_Tasker* m_cr_task;
};
}

#endif	// #ifndef UC_FLASH_H

