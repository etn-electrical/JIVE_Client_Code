/*
 ******************************* C++ Header File *******************************
 *
 *  Copyright (c) 2012 Eaton Corporation, All Rights Reserved.
 *
 *
 *	$Id: AT24C08.h 15341 2012-06-28 09:27:22Z Jingfang Yang $
 *
 *******************************************************************************
 */
#ifndef CAT25128_H
   #define CAT25128_H

#include "uC_SPI.h"
#include "NV_Ctrl.h"
#include "CR_Tasker.h"
#include "CR_Queue.h"
#include "Ram.h"
#include "StdLib_MV.h"
#include "Timers_Lib.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

typedef struct EEPROM_CHIP_CONFIG_TD
{
	uint8_t address_length;
	uint8_t write_page_size;
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;
	uint32_t max_clock_freq;
} EEPROM_CHIP_CONFIG_TD;

typedef enum
{
	EEPROM_START_TO_WRITE,
	EEPROM_WRITTING_PAGE,
	EEPROM_WRITE_PAGE_FINISH,
	EEPROM_CHECK_DATA,
	EEPROM_WRITE_FINISH
} EEPROM_WRITE_STATUS_EN;


typedef struct
{
	bool_t result;
	EEPROM_WRITE_STATUS_EN status;
} RESULT_STATUS_ST;

typedef OS_TICK_TD TIMERS_TIME_TD;

#define CAT25128_NV_CHECKSUM_LEN                    2U
#define CAT25128_DEFAULT_ERASE_VAL                  0x00U
#define CONVERT_MS_TO_COUNT( millisecond )          ( millisecond * ( ( MASTER_CLOCK_FREQUENCY / 1000 ) / 15 ) )

/**
 ****************************************************************************************
 * @brief The CAT25128 derived from the NV_Ctrl class contains implementations required for the access of the NV memory.
 *
 * @details It provides functionalities to read, write, erase the NV memory.
 *
 ****************************************************************************************
 */
class CAT25128 : public NV_Ctrl
{
	public:
		CAT25128( EEPROM_CHIP_CONFIG_TD const* chip_config, uint16_t buff_size, uC_SPI* spi_ctrl, uint8_t chip_select );
		~CAT25128( void );

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

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual void Continue( void )
		{}

		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @details This does the read operation over the SPI. The data received is written into the address pointed by
		 *the
		 * variable data. In case of the protection being enabled, if there is a checksum match, then the data is
		 *considered good.
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
		 *end addresses;
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
									uint8_t erase_data, bool protected_data = true );

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

		void Set_Fault( void );

		void Clr_Fault( void );

		bool_t Is_Ready( void );

		void Use_OS( bool_t value ){m_use_os = value;}

		static void Set_Write_Lock( bool_t write_lock );

		static bool_t Get_Write_Lock();

	private:
		void Eeprom_Handler( CR_Tasker* cr_task );

		static void Eeprom_Handler_Static( CR_Tasker* cr_task,
										   CR_TASKER_PARAM handle ){ ( ( CAT25128* )handle )->Eeprom_Handler( cr_task );
		}

		/**
		 * This function shouldn't get the operate right
		 */
		bool_t Internal_Write( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, bool_t use_checksum,
							   bool_t use_protection );

		RESULT_STATUS_ST Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		/**
		 * Get the length will be passed to Write_Now method according to address and length
		 */
		void Write_Prep( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, NV_CTRL_LENGTH_TD* output_length );

		bool_t Writting( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		bool_t Read_Now( uint8_t* data, uint32_t address, uint32_t length, bool_t use_checksum );

		bool_t Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						   bool_t use_checksum );

		bool_t Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,  bool_t protection = TRUE );

		bool_t Check_CheckSum( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		void Send_Data_Command( uint8_t spi_command, NV_CTRL_ADDRESS_TD address );

		bool_t Read_Status( uint8_t* status );

		bool_t Is_Writting( void );

		CR_Queue* m_cr_queue;
		EEPROM_CHIP_CONFIG_TD const* m_chip_cfg;
		uint8_t m_max_buff_size;
		uint8_t* m_data_buffer;
		uint8_t* m_wpage_buffer;
		uint8_t* m_data_ptr;
		EEPROM_WRITE_STATUS_EN m_status;
		TIMERS_TIME_TD m_start_time;

		// use in write coroutine
		NV_CTRL_ADDRESS_TD m_address;
		NV_CTRL_LENGTH_TD m_length;
		NV_CTRL_LENGTH_TD m_done_length;
		NV_CTRL_LENGTH_TD m_deal_length;
		bool_t m_use_checksum;
		bool_t m_use_protection;
		uint8_t m_retry_times;
		uint8_t m_erase_val;
		uC_SPI* m_spi_ctrl;
		uint8_t m_chip_select;
		bool_t m_use_os;
		static bool_t m_write_lock;
};

}

#endif


