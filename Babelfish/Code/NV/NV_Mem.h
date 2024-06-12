/**
 *****************************************************************************************
 *	@file			NV_Mem.h Header file for Direct Memory Access RX and TX functionality implementation
 *					with USART as main peripheral.
 *
 *	@brief			This file shall wrap all the functions required for NV memory interface.
 *
 *	@details        The basic purpose of having Non volatile Memory is to keep the data stored into the memory,
 *	                e.g. some data related to project which is going to be constant.
 *
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here in.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */
#ifndef NV_MEM_H
   #define NV_MEM_H

/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
#include "NV_Ctrl.h"
#include "NV_Address.h"


namespace NV
{

/*
 *****************************************************************************************
 *		NV_MEM Flush Commands
 *****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief    The NV_Mem class declaration
 *
 * @details  It contains implementations required for performing operations with the NV memory.
             It provides functionalities to read, write, erase the NV memory.
 *
 ****************************************************************************************
 */
class NV_Mem
{
	public:

		/**
		 * @brief	The different status for NV_Mem operations.
		 */
		enum operation_t
		{
			/** @brief Initialization of the operation*/
			INITIALIZATION,
			/** @brief	Read operation*/
			READ_OP,
			/** @brief  Read checksum operation*/
			READ_CHECKSUM_OP,
			/** @brief  Write operation for the NV memory*/
			WRITE_OP,
			/** @brief  Erase operation for the NV memory*/
			ERASE_OP,
			/** @brief  Erase all operation for the NV memory*/
			ERASE_ALL_OP
		};

		/**
		 * @brief 	Datatype used for the call back parameter which is used as an argument in the callback function.
		 */
		typedef void* nv_error_cback_param_t;

		/**
		 * @brief 	Datatype used for the call back functions.
		 */
		typedef void (* nv_error_cback_t)( nv_error_cback_param_t param,
										   NV_Ctrl::nv_status_t nv_ctrl_status, operation_t operation, uint8_t mem_num,
										   uint32_t address );

		/**
		 * @brief 		This is the NV memory usage initialization.
		 * @details 	nv_flush_key_list is a list of uint8_t keys. This key is written to NV after a flush is
		 * complete.
		 * 				This value is then checked on each powerup. If the keys don't match then a flush is performed.
		 * 				The key value must be greater than zero and less than 0xFF.
		 * @param[in] 	nv_ctrl_list: Pointer to the non volatile control memory list.
		 * @param[in] 	nv_ctrl_list_size: The NV control list size consists of FRAM and RAM.
		 * @param[in] 	nv_flush_key_list: The address of the NV flush keys.
		 * @param[in] 	nv_error_cback: The NV memory error call back set to NULL.
		 * @param[in] 	nv_error_cback_param: The NV memory error call back parameter set to NULL.
		 */
		static void Init( NV_Ctrl** nv_ctrl_list, uint8_t nv_ctrl_list_size,
						  uint8_t const* nv_flush_key_list = NULL,
						  nv_error_cback_t nv_error_cback = NULL, nv_error_cback_param_t nv_error_cback_param =
						  NULL );

		/// Protection bit required when doing a NV read or write operations.
		static const bool USE_PROTECTION = true;

		/// This implies no protection when doing a NV read or write operations.
		static const bool NO_PROTECTION = false;

		/**
		 * @brief This is the NV memory read operation.
		 * @param[in] data: The pointer where the data read is to be written.
		 * @param[in] address: The NV memory address from where the data is to be read.
		 * @param[in] length: The Length to be read.
		 * @param[in] use_protection: The status of protection bit during the read and write operation.
		 * @return The status of NV read operation.
		 * @retval true The NV read operation was successful.
		 * @retval false The NV read operation was not successful.
		 */
		static bool Read( uint8_t* data, uint32_t address, uint32_t length,
						  bool use_protection = NV_Mem::USE_PROTECTION );

		/**
		 * @brief  This is the NV memory read checksum  data operation.
		 * @param[in] data: The pointer where the checksum data to be read is to be written.
		 * @param[in] address: The NV control memory address from where the checksum data is to be read.
		 * @param[in] length: The Length to be read.
		 * @return The status of NV checksum data read operation.
		 * @retval true The NV read operation was successful.
		 * @retval false The NV read operation was not successful.
		 */
		static bool Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length );

		/**
		 * @brief This is the NV memory write into operation.
		 * @param[in] data: The address from where the data is to be written into NV memory.
		 * @param[in] address: The NV memory address.
		 * @param[in] length: The Length to be written.
		 * @param[in] use_protection: The status of protection bit during the read and write operation.
		 * @return The status of NV write operation.
		 * @retval true The NV write operation was successful.
		 * @retval false The NV write operation was not successful.
		 */
		static bool Write( uint8_t* data, uint32_t address, uint32_t length,
						   bool use_protection = NV_Mem::USE_PROTECTION );

		/**
		 * @brief This is the NV memory erase operation from the passed in address.
		 * @param[in] address: The NV memory address.
		 * @param[in] length: The Length to be erased.
		 * @param[in] erase_data:Data to be erased
		 * @return status of requested operation.
		 * @retval true The erase operation was successful.
		 * @retval false The erase operation was not successful.
		 */
		static bool Erase( uint32_t address, uint32_t length, uint8_t erase_data = 0, bool protected_data = true );

		/**
		 * @brief Erases the entire NV_Mem.
		 * @return status of requested operation.
		 * @retval true The erase operation was successful.
		 * @retval false The erase operation was not successful.
		 */
		static bool Erase_All( void );

		/**
		 * @brief This loads the default data, address, length to NV Memory.
		 * @details Done during the erase operation for the id passed.
		 * @param[in] nv_ctrl_id: The corresponding control id for which the NV data is to be erased.
		 * @return None
		 */
		static void Load_Defaults( uint8_t nv_ctrl_id );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		NV_Mem( void );
		~NV_Mem( void );
		NV_Mem( const NV_Mem& rhs );
		NV_Mem & operator =( const NV_Mem& object );

		static bool Erase_All_Single_Id( uint8_t nv_ctrl_id );

		static const uint8_t NV_MEM_FLUSH_COMPLETE = 0xA5U;
		static NV_Ctrl** m_nv_ctrl_list;
		static uint_fast8_t m_nv_ctrl_list_size;
		static uint8_t const* m_nv_flush_key_list;
		static nv_error_cback_t m_nv_error_cback;
		static nv_error_cback_param_t m_nv_error_cback_param;
};

}
#endif
