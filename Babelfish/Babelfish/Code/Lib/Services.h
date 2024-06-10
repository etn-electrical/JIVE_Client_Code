/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module provides a central place for services to be activated.
 *
 *	@details The intention of this module is to act like a redirector of common or even
 *	non-common services. An example service would be a factory reset.
 *	@n @b 1. Standard services reside in the 0->127 range.
 *	@n @b 2. Application specific services reside in the 128->255 range.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SERVICES_H
#define SERVICES_H

#include "Services_List.h"

namespace BF_Lib
{
/*
 **************************************************************************************************
 *  Constants
   --------------------------------------------------------------------------------------------------
 */

/// This will set the default state of the MFG access.  It would ideally be true (MFG access
// granted)
/// under debug conditions and default of false under normal release conditions.
#ifdef DEBUG
/// #define SERVICES_DEFAULT_MFG_ACCESS				true
#define SERVICES_DEFAULT_MFG_ACCESS             false
#else
#define SERVICES_DEFAULT_MFG_ACCESS             false
#endif

/**
 *****************************************************************************************
 * @brief This is a Services class which helps to run different services e.g.
 * run Bootloader.
 * @details It contains the functionality for using services.
 * @n @b Usage:
 * @n @b 1. To have the passed in service ID get executed and to attach a
 * callback to have the service called.
 * @n @b 2. A global unlock key to tell if we are in a MFG situation.
 *****************************************************************************************
 */
class Services
{
	public:

		/** @todo
		 * Callback function for service handling.  The function will get called when a service
		 * is requested.
		 * The service ID is the ID of the service being called out.  The list can be seen in
		 * Services_List.h
		 * The service data string contains the data passed into the service function.
		 * The service ID and a service data string are passed along to the function.
		 * The function will return whether the service was successful or not.
		 */
		typedef struct SERVICES_HANDLE_DATA_ST
		{
			uint8_t* rx_data;
			uint16_t rx_len;
			uint8_t* tx_data;
			uint16_t tx_len;
		} SERVICES_HANDLE_DATA_ST;

		/**
		 * Type alias for the type void*, used for the service handle callback.
		 */
		typedef void* SERVICES_HANDLE_CB;

		/**
		 *  @brief Function pointer.
		 *  @details Used for the service callback function.
		 *  @param[in] handle: The handle of the callback service function.
		 *  @param[in] service_id: The id of the service to execute.
		 *  @param[in] service_data: The data of the service.
		 *  @return The status of the service being asked to run, true implies successful.
		 */
		typedef bool SERVICES_HANDLER_CB ( SERVICES_HANDLE_CB handle, uint8_t service_id,
										   SERVICES_HANDLE_DATA_ST* service_data );

		/**
		 *  @brief Constructor
		 *  @details simply initializes all the service handlers to NULL.
		 *  @return None
		 */

		Services( void );

		/**
		 * @brief Destructor.
		 * @details Destructor does nothing.
		 */
		~Services( void )   {}

		/**
		 *  @brief
		 *  @details To have a service called, you must attach the callback with the id of the
		 * service
		 *  you expect to execute.  The list of services should be managed in an external header
		 * file.
		 *  @param[in] service_id: The id of the service to execute.
		 *  @param[in] cb_handler: The function for the service to be executed.
		 *  @param[in] cb_handle: The handle to the callback.
		 *  @return None
		 */
		static void Add_Service_Handler( uint8_t service_id, SERVICES_HANDLER_CB* cb_handler,
										 SERVICES_HANDLE_CB cb_handle );

		/**
		 *  @brief
		 *  @details This causes the passed in service ID to get executed.
		 *  @param[in] service_id: The id of the service to execute.
		 *  @param[in] service_data: Pointer to the service data.
		 *  @return The success or failure of the service.
		 *  @retval true will mean success.
		 *  @retval false will mean failure.
		 */
		static bool Execute_Service( uint8_t service_id,
									 SERVICES_HANDLE_DATA_ST* service_data =
									 reinterpret_cast<SERVICES_HANDLE_DATA_ST*>( nullptr ) );

		/**
		 *  @brief
		 *  @details A global unlock key is provided so functions can tell if we are in a MFG
		 * situation.
		 *  @n @b Usage: An example use is calibration time or HW rev updating.
		 *  @return The status whether we are in the manufacturing stage.
		 *  @retval true will mean not in MFG.
		 *  @retval false will default MFG access.
		 */
		static inline bool MFG_Access_Unlocked( void )
		{
			return ( m_mfg_access_unlocked );
		}

		/**
		 *  @brief
		 *  @details A global unlock key is provided so functions can write an unlock as false
		 *  if we are in a MFG situation.
		 *  @n @b Usage: An example use is calibration time or HW rev updating.
		 *  @param[in] unlock: status write bit as false.
		 *  @return None
		 */
		static inline void MFG_Access_Unlock( bool unlock = false )
		{
			m_mfg_access_unlocked = unlock;
		}

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Services( const Services& object );
		Services & operator =( const Services& object );

		static SERVICES_HANDLER_CB* m_app_spec_cb[SERVICES_MAX_APP_SPECIFIC_ARRAY];
		static SERVICES_HANDLE_CB m_app_spec_handle[SERVICES_MAX_APP_SPECIFIC_ARRAY];

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static bool Null_Func( SERVICES_HANDLE_CB handle, uint8_t service_id,
							   SERVICES_HANDLE_DATA_ST* service_data )
		{
			return ( false );
		}

		static bool m_mfg_access_unlocked;
};

}

#endif	/*SERVICES_H*/
