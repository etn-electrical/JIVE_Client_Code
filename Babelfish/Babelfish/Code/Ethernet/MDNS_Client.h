/**
 *****************************************************************************************
 *	@file        MDNS header file for the webservices.
 *
 *	@brief       This file shall wrap the functions required for MDNS protocol.
 *
 *	@details     The MDNS Client services are designed primarily for lwIP mDNS discovery feature.
 *
 *  @copyright   2019 Eaton Corporation. All Rights Reserved.
 *
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed
 *               here in.  This technical information should not be reproduced or used
 *               without direct written permission from Eaton Corporation.
 *
 *
 *
 *
 *
 *
 *****************************************************************************************
 */
#ifndef __MDNS_CLIENT_H__
#define __MDNS_CLIENT_H__

#include "Includes.h"
#include "DCI_Owner.h"
#include "lwip/apps/mdns.h"

class MDNS_Client
{
	public:

		/**
		 * @brief mDNS specific typedef for int8_t - used for service ID and total number of services.
		 */
		typedef int8_t mdns_service_t;

		/**
		 * @brief Callback parameter typically used for the "this" pointer.
		 */
		typedef void* cback_param_t;

		/**
		 * @brief 			Callback function for mDNS service response
		 * @param answer:	Domain name buffer pointer
		 * @param varpart:	Retrieved information from response packet
		 * @param varlen:	Retrieved information length
		 * @param flags:`	Response flags
		 * @param arg:		Argument for requested service id
		 */
		typedef void cback_t( mdns_answer* answer, const char* varpart, int8_t varlen, int8_t flags, void* arg );

		/**
		 * @brief Structure to hold all the service information.
		 */
		struct mdns_service_request_t
		{
			const char* name;	///< Name of service, like 'myweb'.
			const char* type;	///< Type of service, like '_http'.
			mdns_sd_proto serv_proto;		///< Protocol, TCP or UDP.
			search_result_fn_t serv_cb;		///< Callback function called for each response.
			void* arg;				///< Arguments to the callback, if any.
		};

		/**
		 * @brief                  Constructor to create the MDNS_Client class.
		 * @param                   mdns_service_conf
		 * @return                  None
		 */
		MDNS_Client( mdns_service_request_t* mdns_service_conf );

		/**@brief Destructor to delete the instance of the MDNS_Client class when it goes out of
		   scope.*/
		~MDNS_Client( void ) {}

		/**
		 * @brief Function to start an mDNS service discovery.
		 * @details Start_Service API should be used in sync with the Stop_Service API.
		 * We should not be calling Start_Service API without Stop_Service called first.
		 * This is to be taken care explicitely because the same service should not be started
		 * again if it is already started.
		 * To overcome this situation, adopter may want to use Restart_Service API which will
		 * take care of stopping the service automatically insite the same API.
		 * @return success				true = success | false = failure
		 */
		bool_t Start_Service( void );

		/**
		 * @brief Function to restart an mDNS service discovery
		 * @details Restart_Service API will take care of stopping the service which needs to be
		 * started again.
		 * @return success				true = success | false = failure
		 */
		bool_t Restart_Service( void );

		/**
		 * @brief Function to stop an mDNS service discovery
		 */
		void Stop_Service( void );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 * @n The fact that we are disallowing these standard items should be Doxygen
		 * documented.  Generally private members do not need to be Doxygen documented.
		 */
		MDNS_Client( const MDNS_Client& object );
		MDNS_Client & operator =( const MDNS_Client& object );

		/**
		 * @brief Service id that will be assigned by the stack after slot allocation for service
		 */
		mdns_service_t m_service_id;

		/**
		 * @brief Service request structure for storing the service information
		 */
		mdns_service_request_t* m_service_conf;

		/**
		 * @brief Total number of configured services
		 */
		static mdns_service_t m_total_services;
};

#endif	// __MDNS_CLIENT_H__
