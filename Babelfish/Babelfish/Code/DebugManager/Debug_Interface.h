/**
 **********************************************************************************************
 * @file            Debug Manager interfaces base file for interface communication
 *
 * @brief           This file contains Debug Manager interface base class and pure virtual
 *					functions
 *
 * @details			Other debug interface class will be drived from this base class
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef DEBUG_INTERFACE_H
#define DEBUG_INTERFACE_H

/**
 ****************************************************************************************
 * @brief Debug_Interface (abstract base class).
 *
 * @details Functions used to send debug messages on interfaces will be derived from this
 * 			abstract class
 *
 ****************************************************************************************
 */

class Debug_Interface
{
	public:

		/**
		 * @brief Initialize the interface
		 * @details
		 */
		virtual void Init( void ) = 0;

		/**
		 * @brief Send data on interface
		 * @details This function will be implemented in derived classes and will be used to send
		 * 			data on interface
		 * @param[in] buffer : pointer to buffer from where data needs to be send
		 * @param[in] length : number of bytes to transmit
		 */
		virtual void Send( uint8_t* buffer, uint32_t length ) = 0;

		/**
		 * @brief Send data on interface
		 * @details This function will be implemented in derived classes and is used to send data directly on interface
		 * 			bypassing rest functionality of interface class. This should be	used when we have to transmit
		 * 			directly on interface without any further delay.
		 * 			This function bypass the interface send buffer management and task handler
		 */
		virtual void Send_Now( uint8_t* buffer, uint32_t length ) = 0;

};

#endif	// #ifndef DEBUG_INTERFACE_H
