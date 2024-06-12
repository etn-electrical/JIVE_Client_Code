/**
 *****************************************************************************************
 *	@file Fram.h
 *
 *	@brief  FRAM is non-volatile random-access memory that offer the
 *	same functionality as flash memory.
 *
 *	@details
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FRAM_H
#define FRAM_H

#include "uC_SPI.h"
#include "NV_Ctrl.h"
#include "OS_Semaphore.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define FRAM_SEMAPHORE_TIMEOUT          2000U
#define FRAM_SPI_SEMAPHORE_TIMEOUT      2000U

typedef struct FRAM_CHIP_CONFIG_TD
{
	uint8_t address_length;
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;
	uint32_t max_clock_freq;
} FRAM_CHIP_CONFIG_TD;

/**
 ****************************************************************************************
 * @brief The FRAM class derived from the NV_Ctrl class contains implementations required for the access of the FRAM
 *memory.
 *
 * @details It provides functionalities to read, write, erase the FRAM memory.
 *
 ****************************************************************************************
 */
class FRAM : public NV_Ctrl
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members, importantly if does the spi control, chip select
		 *and chip
		 * configuration members initialized appropriately.
		 * @param[in] spi_ctrl: SPI control configuration.
		 * @param[in] chip_select: Chip select value.
		 * @param[in] chip_config: The chip configuration for FRAM.
		 * @return None
		 */
		FRAM( uC_SPI* spi_ctrl, uint8_t chip_select, FRAM_CHIP_CONFIG_TD const* chip_config );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of FRAM goes out of scope or deleted.
		 *  @return None
		 */
		~FRAM( void );

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
		virtual NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
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
		virtual NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length, bool use_protection = true );

		/**
		 * @brief Performs the erase operation.
		 * @details A appropriate erase value is written to erase the length of the AT25DF memory.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased. The address of the first byte to be read.
		 * @param[in] erase_data: The erase value.
		 * @param[in] protected_data: Perform the operation with protection bit set or not.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length, uint8_t erase_data = 0,
											bool protected_data = true );

		/**
		 * @brief Performs the erase operation over the complete address range.
		 * @details A appropriate erase value is written to erase the length of the AT25DF memory.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Erase_All( void );

		/**
		 * @brief Performs the checksum read operation.
		 * @param[in] checksum_dest: A pointer to the address used to write the checksum.
		 * @param[in] address: It contains the checksum.
		 * @param[in] length: The length to be read.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest,
													NV_CTRL_ADDRESS_TD address,
													NV_CTRL_LENGTH_TD length );

		// TODO
		virtual void Continue( void )
		{}

		/**
		 * @brief This writes the FRAM Status Register content.
		 * @param[in] status: The status register content to be written.
		 * @return Status of requested operation.
		 */
		bool Write_Status( uint8_t status );

		/**
		 * @brief This reads the Status Register content.
		 * @param[in] status: The status register address to be to be read.
		 * @return Status of requested operation.
		 */
		bool Read_Status( uint8_t* status );

		void Execute_Op_Code( uint8_t op_code, uint8_t* rx_data, uint16_t rx_data_len );

	private:
		bool Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
						NV_CTRL_LENGTH_TD length, bool use_protection );

		bool Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
					   NV_CTRL_LENGTH_TD length, bool use_protection );

		bool Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						 NV_CTRL_LENGTH_TD length );

		void Send_Data_Command( uint8_t fram_command, NV_CTRL_ADDRESS_TD address );

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						  bool protection = true ) const;

		uC_SPI* m_spi_ctrl;
		uint8_t m_chip_select;
		FRAM_CHIP_CONFIG_TD const* m_chip_cfg;

		#ifndef FRAM_MIRROR_SUPPORT_DISABLE
			#define FRAM_MIRROR_SUPPORT true
		#else
			#define FRAM_MIRROR_SUPPORT false
		#endif
		/* Enable or disable the MIRROR support for FRAM */
		static const bool MIRROR_SUPPORT = FRAM_MIRROR_SUPPORT;
};

}
#endif
