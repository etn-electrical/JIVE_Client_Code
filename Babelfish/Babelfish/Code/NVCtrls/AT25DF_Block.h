/**
 *****************************************************************************************
 *	@file AT25DF_Block.h
 *
 *	@brief The AT25DF is a serial interface Flash memory device.
 *
 *	@details It is designed for use in a wide variety of applications in which program
 *  code is shadowed from Flash memory into embedded or external RAM for execution.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef AT25DF_BLOCK_H
#define AT25DF_BLOCK_H

#include "uC_SPI.h"
#include "NV_Ctrl.h"
#include "Bit.hpp"
#include "CR_Tasker.h"
#include "Timers_Lib.h"
namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define SPI_SEMAPHORE_TIMEOUT   2000U

typedef struct AT25DF_BLOCK_CHIP_CONFIG_TD
{
	uint8_t address_length;
	uint32_t erase_page_size;
	uint32_t write_page_size;
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;
	uint32_t max_clock_freq;
} AT25DF_BLOCK_CHIP_CONFIG_TD;

#ifndef AT25DF_ERASE_4K_MAX_TIME
#define AT25DF_ERASE_4K_MAX_TIME    500		// ms
#endif
#ifndef AT25DF_ERASE_32K_MAX_TIME
#define AT25DF_ERASE_32K_MAX_TIME   800		// ms
#endif
#ifndef AT25DF_ERASE_64K_MAX_TIME
#define AT25DF_ERASE_64K_MAX_TIME   1200	// ms
#endif

typedef OS_TICK_TD TIMERS_TIME_TD;

/**
 ****************************************************************************************
 * @brief The AT25DF derived from the NV_Ctrl class contains implementations required for the access of the NV memory.
 *
 * @details It provides functionalities to read, write, erase the NV memory.
 *
 ****************************************************************************************
 */
class AT25DF_Block : public NV_Ctrl
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members, importantly if does the spi control, chip select
		 * and chip
		 * configuration members initialized appropriately.
		 * @param[in] spi_ctrl: SPI control configuration.
		 * @param[in] chip_select: Chip select value.
		 * @param[in] chip_config: The configuration of AT25DF.
		 * @param[in] synchronus_erase: The Erase functionality can be configured synchronous or asynchronous.
		 * @param[in] cback_func: If asynchronous erase a callback function is required to be passed.
		 * @param[in] param: If asynchronous erase a callback parameter is required to be pass to know the status of
		 * erase.
		 * @param[in] fast_write: For quick write functionality here mirror address functionality will be disabled.
		 * @return None
		 */
		AT25DF_Block( uC_SPI* spi_ctrl, uint8_t chip_select, AT25DF_BLOCK_CHIP_CONFIG_TD const* chip_config );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of AT25DF goes out of scope or deleted.
		 *  @return None
		 */
		~AT25DF_Block( void );

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
								   cback_func_t cback, cback_param_t param );

		/**
		 * @brief Erases the entire NV_Ctrl device.
		 * @param[in] cback: A pointer to the function to be called when the operation is complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( cback_func_t cback, cback_param_t param );

		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @details This does the read operation over the SPI. The data received is written into the address pointed by
		 * the
		 * variable data. In case of the protection being enabled, if there is a checksum match, then the data is
		 * considered good.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: Perform the operation with protection bit set or not.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = true );

		/**
		 * @brief Writes a specified number of bytes from a given address into a buffer.
		 * @details This initially checks the range of the address and the length to be basically between the start and
		 * end addresses;
		 * the end address being appropriately selected based upon the protection bit enabled or disabled.
		 * @param[in] data: A pointer to the buffer used to write the data.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] use_protection: Perform the operation with protection bit set or not.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool use_protection = true );

		/**
		 * @brief Performs the erase operation.
		 * @details A appropriate erase value is written to erase the length of the AT25DF memory.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] erase_data: The erase value.
		 * @param[in] protected_data: Perform the operation with protection bit set or not.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = AT25DF_DEFAULT_ERASE_VAL, bool protected_data = true );

		/**
		 * @brief Performs the erase operation over the complete address range.
		 * @details A appropriate erase value is written to erase the length of the AT25DF memory.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase_All( void );

		/**
		 * @brief Performs the checksum read operation.
		 * @param[in] checksum_dest: A pointer to the address used to write the checksum.
		 * @param[in] address: It contains the checksum.
		 * @param[in] length: The length to be read.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length );

		/**
		 * @brief This writes the AT25DFx device's status Register content.
		 * @details Commands send are write enable opcode and then write status register byte.
		 * @param[in] status: The status register content to be written.
		 * @return Status of requested operation.
		 */
		bool Write_Status( uint8_t status );

		/**
		 * @brief This reads the Status Register content.
		 * @details Commands send are read status register opcode and then the get the data into address.
		 * Particularly useful in cases where we need to read the status of the chip, e.g. its status if busy or free.
		 * @param[in] status: The status register address to be to be read.
		 * @return Status of requested operation.
		 */
		bool Read_Status( uint8_t* status );

		/**
		 * @brief This is to read the device id over the SPI and the written into the passed in address.
		 * @param[in] device_id_dest: The device id written into the passed in address.
		 * @return Status of requested operation.
		 */
		bool Read_Device_ID( uint8_t* device_id_dest );

		// TODO
		virtual void Continue( void )
		{}

		/**
		 * @brief Function to check the status of serial flash operation.
		 * @param[in] none.
		 * @return Status of requested operation.
		 */
		static void Serial_Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		void Serial_Flash_Async_Task( CR_Tasker* cr_task );

		/**
		 * @brief Function to attach callback function and callback parameter
		 * @param[in] func_callback : Attach the callback function to be called
		 * @param[in] func_param : Attach the callback parameter to be called
		 */
		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param );

		/**
		 * @brief Get memory range information
		 * @param[out] Information will be filled in here.
		 * @return None
		 */
		void Get_Memory_Info( mem_range_info_t* mem_range_info );

		/**
		 * @brief Calculates time required for read operation based on BYTE_READ_TIME_US
		 * @param[in] data: Not used inside read operation.
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return Operation time in milli second.
		 */
		uint32_t Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief Calculates time required for write operation based on BYTE_WRITE_TIME_US
		 * Write_Time() api is already exposed to Adopters which by default consider ERASE time
		 * We have to keep default value as true for "include_erase_time" to support backword compatibility
		 * @param[in] data: Used to analyze number of bytes which has value FLASH_ERASED_VALUE. Does not require
		 * write operation for such bytes.
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @param[in] include_erase_time: Flag to include erase time or not
		 * @return Operation time in milli second.
		 */
		uint32_t Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							 NV_CTRL_LENGTH_TD length, bool include_erase_time = true ) const;

		/**
		 * @brief Get wait time which is introduced due to last operation
		 * @return wait time in milli second.
		 */
		uint32_t Get_Wait_Time( void ) { return ( m_wait_time ); }

		/**
		 * @brief Get wait time for erase operation
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return wait time in milli second.
		 */
		uint32_t Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

	private:
		typedef struct BUFF_STRUCT_TD
		{
			BUFF_STRUCT_TD* next;
			uint8_t* data;
			NV_CTRL_LENGTH_TD length;
		} BUFF_STRUCT_TD;

		bool Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		bool Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
					   NV_CTRL_LENGTH_TD length, bool use_protection );

		bool Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						 NV_CTRL_LENGTH_TD length, bool use_protection );

		void Send_Data_Command( uint8_t command, NV_CTRL_ADDRESS_TD address );

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						  bool protection = true ) const;

		bool Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		bool Erase_Pages( uint32_t address, uint8_t erase_cmd );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Is_Aligned( uint32_t address, uint32_t block_size )
		{
			return ( ( address == ( address & ( ~( block_size - 1 ) ) ) ) ? TRUE : FALSE );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline uint32_t Aligne_Address( uint32_t address, uint32_t block_size )
		{
			return ( address & ( ~( block_size - 1 ) ) );
		}

		uC_SPI* m_spi_ctrl;
		uint8_t m_chip_select;
		AT25DF_BLOCK_CHIP_CONFIG_TD const* m_chip_cfg;
		uint8_t* m_wpage_buff;
		uint32_t m_wait_time;

		BUFF_STRUCT_TD* m_start_buff;
		BUFF_STRUCT_TD m_src_buff;
		BUFF_STRUCT_TD m_checksum_buff;
		BUFF_STRUCT_TD m_begin_rec_buff;
		BUFF_STRUCT_TD m_end_rec_buff;
		uint16_t m_checksum;
		uint8_t m_erase_val;
		bool m_synchronous_erase;

		// Protect from simultaneous read/write
		static semaphore_status_t m_semaphore;
		nv_state_t m_state;
		nv_status_t m_status;
		NV_Ctrl::asynch_service_struct_t m_asynch_data;

		void asynch_erase_handler( void );

		void asynch_write_handler( void );

		void asynch_read_handler( void );

		static const uint8_t AT25DF_WRITE_STATUS_REGISTER_OPCODE = 0x01U;
		static const uint8_t AT25DF_WRITE_MEMORY_ADD_DATA_OPCODE = 0x02U;
		static const uint8_t AT25DF_READ_MEMORY_ADD_DATA_OPCODE = 0x03U;
		static const uint8_t AT25DF_WRITE_DISABLE_OPCODE = 0x04U;
		static const uint8_t AT25DF_READ_STATUS_REGISTER_OPCODE = 0x05U;
		static const uint8_t AT25DF_SET_WRITE_ENABLE_LATCH_OPCODE = 0x06U;
		static const uint8_t AT25DF_READ_DEVICE_ID_OPCODE = 0x9FU;
		static const uint8_t AT25DF_ERASE_4K_PAGE_OPCODE = 0x20U;
		static const uint8_t AT25DF_ERASE_CHIP_OPCODE = 0x60U;
		static const uint8_t AT25DF_STATUS_WRITE_PROTECT_ENABLE_BIT = 7U;
		static const uint8_t AT25DF_STATUS_BLOCK0_PROTECT_ENABLE_BIT = 3U;
		static const uint8_t AT25DF_STATUS_BLOCK1_PROTECT_ENABLE_BIT = 2U;
		static const uint8_t AT25DF_STATUS_WRITES_ENABLED_LATCH_BIT = 1U;
		static const uint8_t AT25DF_STATUS_BUSY_BIT = 0U;
		static const uint8_t AT25DF_STATUS_ERROR_BIT = 5U;
		static const uint8_t AT25DF_STATUS_WRITE_PROTECT_ENABLE_BITMASK = ( 1U
																			<< AT25DF_STATUS_WRITE_PROTECT_ENABLE_BIT );
		static const uint8_t AT25DF_STATUS_BLOCK0_PROTECT_ENABLE_BITMASK = ( 1U
																			 << AT25DF_STATUS_BLOCK0_PROTECT_ENABLE_BIT );
		static const uint8_t AT25DF_STATUS_BLOCK1_PROTECT_ENABLE_BITMASK = ( 1U
																			 << AT25DF_STATUS_BLOCK1_PROTECT_ENABLE_BIT );
		static const uint8_t AT25DF_STATUS_WRITES_ENABLED_LATCH_BITMASK = ( 1U
																			<< AT25DF_STATUS_WRITES_ENABLED_LATCH_BIT );
		static const uint8_t UPPER_ADDRESS_BIT_SHIFT = 3U;
		static const uint8_t MAX_NUMBER_OF_WRITES = 2U;
		static const uint8_t MAX_NUMBER_OF_READS = 2U;
		static const uint8_t AT25DF_NV_CHECKSUM_LEN = 2U;
		static const uint8_t ERASE_BLOCK_4K = 0x20;			// erase cmd for 4K
		static const uint8_t ERASE_BLOCK_32K = 0x52;		// erase cmd for 32K
		static const uint8_t ERASE_BLOCK_64K = 0xD8;		// erase cmd for 64K
		static const uint32_t BLOCK_4K = 0x1000;
		static const uint32_t BLOCK_32K = 0x8000;
		static const uint32_t BLOCK_64K = 0x10000;
		/* Given constant defined from the shared link of serial flash datasheet
		 * visit https://www.adestotech.com/wp-content/uploads/doc3668.pdf
		 */
		static const uint8_t AT25DF_DEFAULT_ERASE_VAL = 0xFFU;
		static const uint32_t AT25DF_MAX_PROGRAM_BYTE = 256U;
		static const uint16_t BLOCK_64K_ERASE_TIME_MS = 900U;
		static const uint16_t BLOCK_32K_ERASE_TIME_MS = 650U;
		static const uint16_t BLOCK_4K_ERASE_TIME_MS = 200U;
		static const uint16_t PAGE_PROGRAM_TIME_MS = 3U;
		static const uint32_t BYTE_WRITE_TIME_US = 7U;
		static const uint32_t BYTE_READ_TIME_US = 1U;
};

}

#endif
