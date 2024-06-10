/**
 **********************************************************************************************
 * @file           	uC_IPCC.h
 *
 * @brief           This files wraps up esp32 IPC calls
 *
 * @details			This file is used for Inter-Processor Communication Control
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef uC_IPCC_H_
#define uC_IPCC_H_

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef void (* ipcc_func_t)( void* arg );

/*
 *****************************************************************************************
 *		Global variable
 *****************************************************************************************
 */

/**
 **********************************************************************************************************
 * @brief                   uC_IPCC class
 * @details                 Contains functions to register callback function and send data to another core
 * @n						Parameters are core_id, ipcc_func_t
 **********************************************************************************************************
 */
class uC_IPCC
{
	public:
		/**
		 * @brief Constructor
		 * @return this
		 */
		uC_IPCC();

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~uC_IPCC();

		/**
		 * @brief                           Function to Register call back function
		 * @param[in] core_id:				Core id on which function should be executed
		 * @param[in] fun	:				Function name which should be executed
		 * @return                          void
		 * @note							Function definition must be available in derived class
		 */
		void Register_Callback_Fun( uint32_t core_id, ipcc_func_t fun );

		/**
		 * @brief                           Function to Send function to another core for execution
		 * @param[in] core_id:				Core id on which function should be executed
		 * @param[in] fun	:				Function name which should be executed
		 * @param[in] data	:				Arbitrary argument of type void*
		 * @param[in] wait	:				0 - non blocking call. Calling task will remain in blocked state until
		 * 										function start execution
		 * 									1 - blocking call. Calling task will remain in blocked state until
		 * 										function completes execution
		 * @param[in] device:				device structure pointer, null for ESP32
		 * @return                          Status of send operation
		 * @retval                          0x00  - ESP_OK
		 * @retval							0x102 - ESP_ERR_INVALID_ARG
		 * @retval							0x103 - ESP_ERR_INVALID_STATE
		 */
		uint32_t Send( uint32_t core_id, ipcc_func_t fun,
					   void* data, uint32_t wait, void* device );

		/**
		 * @brief                           Function to get current core id
		 * @return                          returns current core id
		 */
		uint32_t Get_Core_ID( void );

};

#endif	/* uC_IPCC_H_ */
