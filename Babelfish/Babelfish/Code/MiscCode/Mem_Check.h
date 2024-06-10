/**
 *****************************************************************************************
 *	@file
 *
 *	@brief The memory to be used shall be checked here.
 *
 *	@details The status of the memory good or not, is calculated on the basis of the
 *	calculated and the passed in CRC considering the bytes per check, the start address,
 *	length and the CRC address.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MEM_CHECK_H
	#define MEM_CHECK_H

#include "CR_Tasker.h"

namespace BF_Misc
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define SINGLE_PASS_CHECK_SIZE          16U

/*
 **************************************************************************************************
 *  @brief The Mem_Check class contains the Memory range check functionality.
 *  @details The memory check shall be done on the basis of the calculated and the passed in CRC.
 *  @n @b Usage:
 *  @n @b 1. The status of the memory range checked, if its good to use or not.
 **************************************************************************************************
 */
class Mem_Check
{
	public:

		/**
		 *  @brief Constructor.
		 *  @details This basically initializes the CR tasker, apart for initializing its own data members.
		 *  @param[in] start_address: The start address of the memory range to be checked.
		 *  @param[in] length: The length of the memory range to be checked.
		 *  @param[in] crc_address: The CRC of the memory.
		 *  @param[in] bytes_per_pass: The number of bytes that will be checked in one pass.
		 */
		Mem_Check( uint8_t const* start_address, uint32_t length, uint16_t const* crc_address,
				   uint16_t bytes_per_pass = SINGLE_PASS_CHECK_SIZE );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Mem_Check goes out of scope or deleted.
		 *  @return None
		 */
		~Mem_Check( void );

		/**
		 *  @brief Does a CRC check
		 *  @details CRC check of the passed in CRC with the calculated one and decide if the memory range is fine or
		 * not.
		 *  @return True if memory range is good, returns false if memory range is bad.
		 */
		static bool Immediate_Check( uint8_t const* start_address, uint32_t length,
									 uint16_t const* crc_address );

		/**
		 *  @brief Does a Blank check
		 *  @details Verify that given memory range is blank based on blank_check_value.
		 *  @return True if memory range is blank, returns false if memory range is not blank.
		 */
		static bool Blank_Check( uint8_t const* start_addr, uint8_t const* end_addr,
								 uint8_t blank_check_value );

	private:
		Mem_Check( const Mem_Check& );	///< Private copy constructor
		Mem_Check & operator =( const Mem_Check & );///< Private copy assignment operator

		void Checker_Task( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Checker_Task_Static( CR_TASKER_PARAM param )
		{ ( ( Mem_Check* )param )->Checker_Task(); }

		static const uint16_t INIT_CRC_VALUE_FOR_FLASH = 0U;
		static const uint16_t MEM_CHECK_CRC_INIT = 0U;
		uint8_t const* m_start_address;
		uint8_t const* m_current_address;
		uint32_t m_length;
		uint16_t const* m_crc_address;
		uint16_t m_current_crc;
		uint16_t m_bytes_per_pass;
};

}
#endif
