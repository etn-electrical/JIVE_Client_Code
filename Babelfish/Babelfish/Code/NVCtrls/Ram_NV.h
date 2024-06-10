/**
 *****************************************************************************************
 *	@file Ram_NV.h
 *
 *	@brief It creates a block of RAM that acts like Non volatile memory.
 *
 *	@details Handles the NVRam memory accessing.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAM_NV_H
#define RAM_NV_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "NV_Ctrl.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define RAM_NV_SIZE_DEFAULT         256

/**
 ****************************************************************************************
 * @brief Ram_NV class derived from the NV_Ctrl class creates virtual NVRAM space.
 *
 * @details It provides functionalities to read, write, erase the NVRAM memory.
 *
 ****************************************************************************************
 */
class Ram_NV : public NV_Ctrl
{
	public:
		/**
		 * @brief The constructor creates virtual ram nv space.
		 * @details It creates a block of RAM that acts like NV. The data will be lost on every reset.
		 * @param[in] nv_ram_block_size: the size of virtual nv space to allocate.
		 * @param[in] include_checksum: indicates to the module that the memory addresses include
		 * a checksum and are not compressed (i.e. lack of the checksum in the address calculation).
		 */
		Ram_NV( uint32_t nv_ram_block_size = RAM_NV_SIZE_DEFAULT, bool include_checksum =
				true );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Ram_NV goes out of scope or deleted.
		 *  @return None
		 */
		~Ram_NV( void );

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
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: Perform the operation with protection bit set or not.
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection );

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
									NV_CTRL_LENGTH_TD length, bool use_protection );

		/**
		 * @brief Performs the erase operation.
		 * @details A appropriate erase value is written to erase the length of the NVRAM memory.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] erase_data: The erase value.
		 * @param[in] protected_data: Perform the operation with protection bit set or not.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, uint8_t erase_data = 0,
									bool protected_data = true );

		/**
		 * @brief Performs the erase operation over the complete address range.
		 * @details A appropriate erase value is written to erase the complete length of the memory.
		 * @return Status of requested operation.
		 */
		NV_Ctrl::nv_status_t Erase_All( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual void Continue( void )
		{}

	private:
		bool Write_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
						   NV_CTRL_LENGTH_TD length, bool use_protection );

		bool Read_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
						  NV_CTRL_LENGTH_TD length, bool use_protection );

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						  bool protection = true ) const;

		uint8_t* m_nv_memory;
		uint32_t m_nv_mem_size;
		bool m_checksum_exists;	///< Indicates that the checksum is included in the address calculations.
};

}

#endif
