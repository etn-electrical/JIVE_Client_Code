/**
 **********************************************************************************************
 * @file            Debug Terminal IO interface file
 *
 * @brief           This file contains Debug Manager Terminal IO interface implementation
 *
 * @details			This class will be used to send debug manager data on Terminal IO interface
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef DEBUG_INTERFACE_TERM_IO_H
#define DEBUG_INTERFACE_TERM_IO_H

#include "Includes.h"
#include "Debug_Interface.h"

/**
 ****************************************************************************************
 * @brief Debug_Manager_Term_IO (Derived class from Debug_Interface)
 *
 * @details This class will be used to initialize and send data on serial debug interface
 *
 ****************************************************************************************
 */
class Debug_Interface_Term_IO : public Debug_Interface
{
	public:
		/**
		 *  @brief Constructs an instance of class.
		 *  @details
		 */
		Debug_Interface_Term_IO();

		/**
		 * @brief Destroys an instance of the class.
		 * @details
		 */
		~Debug_Interface_Term_IO();

	private:

		/**
		 * @brief Initialize debug UART interface
		 * @details
		 */
		void Init( void );

		/**
		 * @brief Send data on UART interface
		 * @details This function will transmit data on serial interface
		 * @param[in] buffer : pointer to buffer from where data needs to be send
		 * @param[in] bytes_to_print : number of bytes to transmit
		 */
		void Send( uint8_t* buffer, uint32_t bytes_to_print );

		/**
		 * @brief Send data directly on UART interface
		 * @details This function will transmit data on serial interface bypassing other interface class functionality
		 */
		void Send_Now( uint8_t* buffer, uint32_t length );

};

#endif	// #ifndef DEBUG_INTERFACE_TERM_IO_H
