/**
 *****************************************************************************************
 *	@file SST26VF.h
 *
 *	@brief The SST26VF is a serial interface Flash memory device.
 *
 *	@details It is designed for use in a wide variety of applications in which program
 *  code is shadowed from Flash memory into embedded or external RAM for execution.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SST26VF_H
#define SST26VF_H

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

struct SST26VF_CHIP_CONFIG_TD
{
	uint8_t address_length;
	uint32_t erase_page_size;
	uint32_t write_page_size;
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;
	uint32_t max_clock_freq;
};

// uncomment below line to use lookup table for SST26VF
// #define SST26VF_LOOKUP_TABLE
#ifdef SST26VF_LOOKUP_TABLE

typedef uint32_t sector_addr_t;
typedef uint32_t sector_num_t;
struct flash_page_t
{
	sector_addr_t sector_size;
	sector_num_t sector_no;
};

const flash_page_t EXTERNAL_FLASH_PAGE_SIZES[140] =
{
	{0x2000, 0}, {0x2000, 1}, {0x2000, 2}, {0x2000, 3}, {0x8000, 4},

	{0x10000, 5}, {0x10000, 6}, {0x10000, 7}, {0x10000, 8}, {0x10000, 9},
	{0x10000, 10}, {0x10000, 11}, {0x10000, 12}, {0x10000, 13}, {0x10000, 14},
	{0x10000, 15}, {0x10000, 16}, {0x10000, 17}, {0x10000, 18}, {0x10000, 19},
	{0x10000, 20}, {0x10000, 21}, {0x10000, 22}, {0x10000, 23}, {0x10000, 24},
	{0x10000, 25}, {0x10000, 26}, {0x10000, 27}, {0x10000, 28}, {0x10000, 29},

	{0x10000, 30}, {0x10000, 31}, {0x10000, 32}, {0x10000, 33}, {0x10000, 34},
	{0x10000, 35}, {0x10000, 36}, {0x10000, 37}, {0x10000, 38}, {0x10000, 39},
	{0x10000, 40}, {0x10000, 41}, {0x10000, 42}, {0x10000, 43}, {0x10000, 44},
	{0x10000, 45}, {0x10000, 46}, {0x10000, 47}, {0x10000, 48}, {0x10000, 49},

	{0x10000, 50}, {0x10000, 51}, {0x10000, 52}, {0x10000, 53}, {0x10000, 54},
	{0x10000, 55}, {0x10000, 56}, {0x10000, 57}, {0x10000, 58}, {0x10000, 59},
	{0x10000, 60}, {0x10000, 61}, {0x10000, 62}, {0x10000, 63}, {0x10000, 64},
	{0x10000, 65}, {0x10000, 66}, {0x10000, 67}, {0x10000, 68}, {0x10000, 69},

	{0x10000, 70}, {0x10000, 71}, {0x10000, 72}, {0x10000, 73}, {0x10000, 74},
	{0x10000, 75}, {0x10000, 76}, {0x10000, 77}, {0x10000, 78}, {0x10000, 79},
	{0x10000, 80}, {0x10000, 81}, {0x10000, 82}, {0x10000, 83}, {0x10000, 84},
	{0x10000, 85}, {0x10000, 86}, {0x10000, 87}, {0x10000, 88}, {0x10000, 89},

	{0x10000, 90}, {0x10000, 91}, {0x10000, 92}, {0x10000, 93}, {0x10000, 94},
	{0x10000, 95}, {0x10000, 96}, {0x10000, 97}, {0x10000, 98}, {0x10000, 99},
	{0x10000, 100}, {0x10000, 101}, {0x10000, 102}, {0x10000, 103}, {0x10000, 104},
	{0x10000, 105}, {0x10000, 106}, {0x10000, 107}, {0x10000, 108}, {0x10000, 109},

	{0x10000, 110}, {0x10000, 111}, {0x10000, 112}, {0x10000, 113}, {0x10000, 114},
	{0x10000, 115}, {0x10000, 116}, {0x10000, 117}, {0x10000, 118}, {0x10000, 119},
	{0x10000, 120}, {0x10000, 121}, {0x10000, 122}, {0x10000, 123}, {0x10000, 124},
	{0x10000, 125}, {0x10000, 126}, {0x10000, 127}, {0x10000, 128}, {0x10000, 129},

	{0x10000, 130}, {0x8000, 131}, {0x2000, 132}, {0x2000, 133}, {0x2000, 134}, {0x2000, 135},
	{0x00000, 0xFFFF}
};
#endif	/// SST26VF_LOOKUP_TABLE
/**
 ****************************************************************************************
 * @brief The SST26VF derived from the NV_Ctrl class contains implementations required for the access of the NV memory.
 *
 * @details It provides functionalities to read, write, erase the NV memory.
 *
 ****************************************************************************************
 */
class SST26VF : public NV_Ctrl
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members, importantly if does the spi control, chip select
		 * and chip
		 * configuration members initialized appropriately.
		 * @param[in] spi_ctrl: SPI control configuration.
		 * @param[in] chip_select: Chip select value.
		 * @param[in] chip_config: The configuration of SST26VF.
		 * @param[in] synchronus_erase: The Erase functionality can be configured synchronous or asynchronous.
		 * @param[in] cback_func: If asynchronous erase a callback function is required to be passed.
		 * @param[in] param: If asynchronous erase a callback parameter is required to be pass to know the status of
		 * erase.
		 * @param[in] fast_write: For quick write functionality here mirror address functionality will be disabled.
		 * @return None
		 */
		SST26VF( uC_SPI* spi_ctrl, uint8_t chip_select, SST26VF_CHIP_CONFIG_TD const* chip_config,
				 bool_t synchronus_erase = true, cback_func_t cback_func = NULL, cback_param_t param =
				 NULL, bool_t fast_write = false );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of SST26VF goes out of scope or deleted.
		 *  @return None
		 */
		~SST26VF( void );

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
		 * @details A appropriate erase value is written to erase the length of the SST26VF memory.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] erase_data: The erase value.
		 * @param[in] protected_data: Perform the operation with protection bit set or not.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = SST26VF_DEFAULT_ERASE_VAL, bool protected_data = true );

		/**
		 * @brief Performs the erase operation over the complete address range.
		 * @details A appropriate erase value is written to erase the length of the SST26VF memory.
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
		 * @brief This writes the SST26VFx device's status Register content.
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
		 * @param[in] include_erase_time: Flag to include erase time
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

	private:
		typedef struct BUFF_STRUCT_TD
		{
			BUFF_STRUCT_TD* next;
			uint8_t* data;
			NV_CTRL_LENGTH_TD length;
		} BUFF_STRUCT_TD;

		/**
		 * @brief function to write specified data buffer into flash
		 * @param[in] data : data buffer to be written into flash
		 * @param[in] recovery_address : recovery address
		 * @param[in] dest_address : destination address
		 * @param[in] length : length of data
		 * @param[in] use_protection : checksum included or not
		 * @return bool value true or false.
		 */
		bool Write_Prep( uint8_t* data, uint32_t recovery_address, uint32_t dest_address,
						 NV_CTRL_LENGTH_TD length, bool use_protection );

		/**
		 * @brief function to write specified data buffer into flash
		 * @param[in] buff_list: buffer list
		 * @param[in] dest_page: destination page address
		 * @param[in] source_page: source page address
		 * @param[in] use_protection: checksum included or not
		 * @return bool value true or false.
		 */
		bool Write_Buff( BUFF_STRUCT_TD* buff_list, uint32_t dest_page,
						 uint32_t source_page, bool use_protection );

		/**
		 * @brief function to erase data from particular start address
		 * @param[in] address: Start address
		 * @return bool value true or false.
		 */
		bool Erase_Page( uint32_t address );

		/**
		 * @brief function to varify data from particular address
		 * @param[in] data: data buffer to be varified
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @param[in] use_protection: checksum included or not
		 * @return bool value true or false.
		 */
		bool Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						 NV_CTRL_LENGTH_TD length, bool use_protection );

		/**
		 * @brief function to write specified data buffer into flash
		 * @param[in] data: data buffer to be written into flash
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return bool value true or false.
		 */
		bool Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		/**
		 * @brief function to read given length of data from the specifed address location
		 * @param[out] data: read buffer to store the data read from the flash
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return bool value true or false.
		 */
		bool Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
					   NV_CTRL_LENGTH_TD length, bool use_protection );

		/**
		 * @brief function to send the SPI commmand to read, write or erase the data
		 * @param[in] command: command to read or write or erase data
		 * @param[in] address: Start address
		 */
		void Send_Data_Command( uint8_t command, NV_CTRL_ADDRESS_TD address );

		/**
		 * @brief Check the flash address range
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return bool value true or false.
		 */
		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						  bool protection = true ) const;

		/**
		 * @brief Function to quickly read the specified length of data from the flash
		 * @param[out] data: data buffer for storing the data read from the flash
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return bool value as true or false.
		 */
		bool Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		/**
		 * @brief function to erase the block of data as per the command specified
		 * @param[in] address: Start address
		 * @param[in] erase_cmd: block erase command
		 * @return Operation time in milli second.
		 */
		bool Erase_Pages( uint32_t address, uint8_t erase_cmd );

		/**
		 * @brief function to erase the entire chip using chip erase command
		 * @param[in] address: Start address
		 * @param[in] erase_cmd: chip erase command
		 * @return bool value as true or false.
		 */
		bool Erase_Chip( uint32_t address, uint8_t erase_cmd );

		/**
		 * @brief function to get sector size of the particular address
		 * @param[in] address: Start address
		 * @return sector size.
		 */
		uint32_t Get_Block_Size( uint32_t address ) const;

		/**
		 * @brief Check weather the address is aligned or not
		 * @param[in] address: Start address
		 * @param[in] block_size: Size of the block
		 * @return bool value as true or false
		 */
		inline bool Is_Aligned( uint32_t address, uint32_t block_size )
		{
			return ( ( address == ( address & ( ~( block_size - 1 ) ) ) ) ? TRUE : FALSE );
		}

		/**
		 * @brief Perform the address alignement
		 * @param[in] address: Start address
		 * @param[in] block_size: Size of the block
		 * @return address
		 */
		inline uint32_t Aligne_Address( uint32_t address, uint32_t block_size )
		{
			return ( address & ( ~( block_size - 1 ) ) );
		}

		/**
		 * @brief Calculates time required for erase operation
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return Operation time in milli second.
		 */
		uint32_t Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		/**
		 * @brief Asynchronous erase data handler
		 * @return address
		 */
		void asynch_erase_handler( void );

		/**
		 * @brief Asynchronous write data handler
		 * @return address
		 */
		void asynch_write_handler( void );

		/**
		 * @brief Asynchronous read data handler
		 * @return address
		 */
		void asynch_read_handler( void );

		uC_SPI* m_spi_ctrl;			// used to store SPI control
		uint8_t m_chip_select;		// used to store chip select value
		SST26VF_CHIP_CONFIG_TD const* m_chip_cfg;	// chip configuration
		uint8_t* m_wpage_buff;		// used as buffer in read nad write operations
		uint32_t m_wait_time;		// used  to get the wait time after read or write operation

		uint8_t m_erase_val;		// used to define default erase value
		bool m_synchronous_erase;	// used to define syncronous or asynhronus data transfer

		BUFF_STRUCT_TD* m_start_buff;
		BUFF_STRUCT_TD m_src_buff;
		BUFF_STRUCT_TD m_checksum_buff;
		BUFF_STRUCT_TD m_begin_rec_buff;
		BUFF_STRUCT_TD m_end_rec_buff;
		uint16_t m_checksum;
		bool_t m_fast_write;
		bool_t m_serial_flash_busy;
		NV_Ctrl::cback_func_t m_call_back_ptr;
		NV_Ctrl::cback_param_t m_cback_param;

		uint8_t _block_protection[( 143 + 7 ) / 8];	// used to clear global block protection

		// Protect from simultaneous read/write
		static semaphore_status_t m_semaphore;
		nv_state_t m_state;			// used to maintain state for asynhronous operation
		nv_status_t m_status;		// used to store the operation result status
		NV_Ctrl::asynch_service_struct_t m_asynch_data;	// used in synchronous read/write operation

		// opcode and command
		static const uint8_t SST26VF_WRITE_STATUS_REGISTER_OPCODE = 0x01U;
		static const uint8_t SST26VF_WRITE_MEMORY_OPCODE = 0x02U;
		static const uint8_t SST26VF_READ_MEMORY_OPCODE = 0x03U;
		static const uint8_t SST26VF_WRITE_DISABLE_OPCODE = 0x04U;
		static const uint8_t SST26VF_READ_STATUS_REGISTER_OPCODE = 0x05U;
		static const uint8_t SST26VF_READ_CONFIG_REGISTER_OPCODE = 0x35U;
		static const uint8_t SST26VF_SET_WRITE_ENABLE_LATCH_OPCODE = 0x06U;
		static const uint8_t SST26VF_READ_DEVICE_ID_OPCODE = 0x9FU;
		static const uint8_t SST26VF_ERASE_4K_PAGE_OPCODE = 0x20U;
		static const uint8_t SST26VF_ERASE_BLOCK_OPCODE = 0xD8;
		static const uint8_t SST26VF_ERASE_CHIP_OPCODE = 0xC7U;

		static const uint8_t SST26VF_RBPR_OPCODE = 0x72;
		static const uint8_t SST26VF_ULBPR_OPCODE = 0x98;

		static const uint8_t SST26VF_STATUS_WRITE_PROTECT_ENABLE_BIT = 7U;
		static const uint8_t SST26VF_STATUS_BLOCK0_PROTECT_ENABLE_BIT = 3U;
		static const uint8_t SST26VF_STATUS_BLOCK1_PROTECT_ENABLE_BIT = 2U;
		static const uint8_t SST26VF_STATUS_WRITES_ENABLED_LATCH_BIT = 1U;

		static const uint8_t SST26VF_STATUS_BUSY_BIT = 0U;
		static const uint8_t SST26VF_STATUS_WRITE_PROTECT_ENABLE_BITMASK =
			( 1U << SST26VF_STATUS_WRITE_PROTECT_ENABLE_BIT );

		static const uint8_t SST26VF_STATUS_BLOCK0_PROTECT_ENABLE_BITMASK =
			( 1U << SST26VF_STATUS_BLOCK0_PROTECT_ENABLE_BIT );

		static const uint8_t SST26VF_STATUS_BLOCK1_PROTECT_ENABLE_BITMASK =
			( 1U << SST26VF_STATUS_BLOCK1_PROTECT_ENABLE_BIT );

		static const uint8_t SST26VF_STATUS_WRITES_ENABLED_LATCH_BITMASK =
			( 1U << SST26VF_STATUS_WRITES_ENABLED_LATCH_BIT );

		static const uint8_t UPPER_ADDRESS_BIT_SHIFT = 3U;
		static const uint8_t MAX_NUMBER_OF_WRITES = 2U;
		static const uint8_t MAX_NUMBER_OF_READS = 2U;
		static const uint8_t SST26VF_NV_CHECKSUM_LEN = 2U;

		static const uint32_t BLOCK_4K = 0x1000;
		static const uint32_t BLOCK_8K = 0x2000;
		static const uint32_t BLOCK_32K = 0x8000;
		static const uint32_t BLOCK_64K = 0x10000;

		static const uint8_t SST26VF_DEFAULT_ERASE_VAL = 0xFFU;
		static const uint32_t SST26VF_MAX_PROGRAM_BYTE = 256U;

		static const uint16_t SST26VF_ERASE_4K_MAX_TIME_MS = 25U;
		static const uint16_t SST26VF_ERASE_BLOCK_MAX_TIME_MS = 25U;
		static const uint16_t SST26VF_ERASE_CHIP_MAX_TIME_MS = 50U;

		static const uint16_t PAGE_PROGRAM_TIME_MS = 3U;
		static const uint32_t BYTE_WRITE_TIME_US = 7U;
		static const uint32_t BYTE_READ_TIME_US = 1U;

		static const uint32_t EXTERNAL_FLASH_START_ADDRESS = 0U;
		static const uint32_t SST26VF_SPI_SEMAPHORE_TIMEOUT = 2000U;

		static const uint32_t BLOCK_64K_UPPER_THRESHOLD = 0x007EFFFFU;
		static const uint32_t BLOCK_64K_LOWER_THRESHOLD = 0x00010000U;

		static const uint32_t BLOCK_32K_UPPER_ADDRESS_UPPER_THRESHOLD = 0x007F7FFFU;
		static const uint32_t BLOCK_32K_UPPER_ADDRESS_LOWER_THRESHOLD = 0x007F0000U;
		static const uint32_t BLOCK_32K_LOWER_ADDRESS_UPPER_THRESHOLD = 0x0000FFFFU;
		static const uint32_t BLOCK_32K_LOWER_ADDRESS_LOWER_THRESHOLD = 0x00008000U;

		static const uint32_t BLOCK_8K_UPPER_ADDRESS_UPPER_THRESHOLD = 0x007FFFFFU;
		static const uint32_t BLOCK_8K_UPPER_ADDRESS_LOWER_THRESHOLD = 0x007F8000U;
		static const uint32_t BLOCK_8K_LOWER_ADDRESS_UPPER_THRESHOLD = 0x00007FFFU;
		static const uint32_t BLOCK_8K_LOWER_ADDRESS_LOWER_THRESHOLD = 0x00000000U;

};

}	// end of namespace NV_Ctrls

#endif	// SST26VF_H
