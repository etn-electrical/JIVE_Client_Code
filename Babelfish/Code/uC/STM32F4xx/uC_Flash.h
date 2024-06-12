/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version C++ Style Guide Version 1.0
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

static const uint32_t FLASH_PGERR = ( FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR
									  | FLASH_SR_WRPERR );
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
				  bool erase_on_boundary = false );

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
								  cback_func_t cback, cback_param_t param );

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
								   cback_func_t cback, cback_param_t param );

		/**
		 * @brief Erases a block of memory in the NV_Ctrl device. This may involve a range larger than specified in the
		 * address and length arguments as some devices have minimum block sizes that can be erased.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase( uint32_t address, uint32_t length,
								   cback_func_t cback, cback_param_t param );

		/**
		 * @brief Erases the entire NV_Ctrl device.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( cback_func_t cback, cback_param_t param );

		/**
		 * @brief Synchronously reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = false );

		/**
		 * @brief Synchronously writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data: A pointer to the buffer used to hold the data for the write operation.
		 * @param[in] address: The address of the first byte to be written.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool use_protection = false );

		/**
		 * @brief Synchronously erases a block of memory in the NV_Ctrl device. This may involve a range larger than
		 * specified in the address and length arguments as some devices have minimum block sizes that can be erased.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = uC_FLASH_DEFAULT_ERASE_VAL,
									bool protected_data = false );

		/**
		 * @brief Synchronously erases the entire NV_Ctrl device.
		 * @return status
		 */
		NV_Ctrl::nv_status_t Erase_All( void );

		/**
		 * @brief Not supported.
		 */
		NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length );

		/**
		 * @brief Calculates time required for read operation based on BYTE_READ_TIME_US
		 * @param[in] data: Not used inside read operation.
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @param[in] include_erase_time: Flag to include erase time
		 * @return Operation time in milli second.
		 */
		uint32_t Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief Calculates time required for write operation based on BYTE_WRITE_TIME_US Write_Time() api is already
		 * exposed to Adopters which by default consider ERASE time We have to keep default value as true for
		 * "include_erase_time" to support backword compatibility
		 * @param[in] data: Used to analyze number of bytes which has value FLASH_ERASED_VALUE. Does not require write
		 * operation for such bytes.
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @param[in] include_erase_time: Flag to include erase time or not
		 * @return Operation time in milli second.
		 */
		uint32_t Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							 NV_CTRL_LENGTH_TD length, bool include_erase_time = true ) const;

		/**
		 * @brief Calculates time required for erase operation based on SECTOR_ERASE_TIME_BASELINE_MS
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return Operation time in milli second.
		 */
		uint32_t Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
		{
			return ( Write_Time( NULL, address, length ) );
		}

		/**
		 * @brief Get wait time which is introduced due to last operation
		 * @return wait time in milli second.
		 */
		uint32_t Get_Wait_Time( void ) { return ( m_wait_time ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual void Continue( void )
		{}

		/**
		 * @brief Static CR tasker for the flash internal operation
		 * @param[in] param: A  pointer to an object passed as an argument CR tasker handler .
		 * @param[in] handle: Parameters passed for task creation.
		 * @return : None
		 */
		static void Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		/**
		 * @brief CR tasker for the flash handler
		 * @return : None
		 */
		void Flash_Async_Task( void );

		/**
		 * @brief Used to attach callback for asyncronous operation
		 * @param[in] func_callback: function callback
		 * @param[in] func_param: parameter
		 * @return None
		 */
		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param );

		/**
		 * @brief Get flash status
		 * @return status
		 */
		uC_FLASH_STATUS_EN Get_Status( void ) const;

		/**
		 * @brief Get memory range information
		 * @param[out] Information will be filled in here.
		 * @return None
		 */
		void Get_Memory_Info( mem_range_info_t* mem_range_info );

	private:
		/* Previously function was inline but we made it normal function during LTK-10164 */
		uint32_t Get_Page_Size( uint32_t address ) const;

		inline uint32_t Get_Page_Mask( uint32_t address )   { return ( ~( Get_Page_Size( address ) - 1U ) );}

		inline void Write_u8( uint32_t adr, uint8_t dat )   { ( *( ( volatile uint8_t* ) ( adr ) ) ) = dat; }

		inline void Write_u16( uint32_t adr, uint16_t dat ) { ( *( ( volatile uint16_t* ) ( adr ) ) ) = dat; }

		inline void Write_u32( uint32_t adr, uint32_t dat ) { ( *( ( volatile uint32_t* ) ( adr ) ) ) = dat; }

		bool Write_Now( uint8_t* data, uint32_t dest_address,
						NV_CTRL_LENGTH_TD length );

		bool Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
					   NV_CTRL_LENGTH_TD length ) const;

		bool Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						 NV_CTRL_LENGTH_TD length ) const;

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		bool Erase_Page( uint32_t address );

		void Erase_Page_Non_Blocking( uint32_t sector_num );

		uC_FLASH_STATUS_EN Wait_For_Complete( void ) const;

		void Unlock_Flash( void );

		void Lock_Flash( void );

		uint32_t Get_Sector( uint32_t address ) const;

		uint32_t Get_Sector_Base_Address( uint32_t sector_id );

		/**
		 * @brief Interrupt handler for internal flash. Currenlty only used in DEBUG mode to capture flash error events
		 */
		static void uC_Flash_Int_Handler( void );

		void asynch_erase_handler( void );

		void asynch_write_handler( void );

		void asynch_read_handler( void );

		uint32_t m_wait_time;

		uC_FLASH_CHIP_CONFIG_TD const* m_chip_cfg;
		FLASH_TypeDef* m_flash_ctrl;

		uint8_t m_erase_val;
		bool m_erase_on_boundary;

		// Protect from simultaneous read/write
		static semaphore_status_t m_semaphore;
		nv_state_t m_state;
		nv_status_t m_status;

		// uint32_t m_asynch_erase_first_sector;
		uint32_t m_asynch_erase_sector_end;
		uint32_t m_asynch_erase_sector_head;

		NV_Ctrl::asynch_service_struct_t m_asynch_data;

		/* In one interation, We will read READ_CHUCK_SIZE bytes from flash and save into destination buffer.
		   Destination buffer length should always greater than READ_CHUCK_SIZE */
		const uint32_t READ_CHUCK_SIZE = 1024U;
		const uint32_t WRITE_CHUCK_SIZE = 64U;
};
}
#endif
