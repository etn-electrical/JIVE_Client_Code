/**
 *****************************************************************************************
 *	@file        SNTP header file for the webservices.
 *
 *	@brief       This file shall wrap the functions required for webservices using SNTP protocol.
 *
 *	@details     The LTK SNTP services are designed primarily for use of time syncronisation.
 *              It is also used for logging, device certificate validation, cloud connectivity
 *              and possibly for future when alarms are supported in toolkit.
 *

 *  @copyright   2017 Eaton Corporation. All Rights Reserved.
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
#ifndef __SNTP_IF_H__
#define __SNTP_IF_H__

#include "Includes.h"
#include "DCI_Owner.h"
#include "StdLib_MV.h"
#include <string.h>
#include <stdlib.h>
#ifdef ESP32_SETUP
#include "Sntp_Config_ESP32.h"
#endif

class SNTP_If
{
	public:

		/**
		 * @brief Defines the events which can be returned by the SNTP stack.  These can also be requests.
		 */
		enum cback_event_t
		{
			NEW_TIME_RECEIVED,		//!< A new time was received and is being passed in epoch time variable.
			RETRIEVE_TIME,			//!< The sntp client code is requesting from the application the current time.
			SERVERS_NOT_RESPONDING	//!< All three sntp servers are not responding.
		};

		/**
		 * @brief Callback parameter typically used for the "this" pointer.
		 */
		typedef void* cback_param_t;

		/**
		 * @brief Callback to handle events triggered from the SNTP client.
		 * @param handle:  Typically the this pointer.
		 * @param event:  What type of event it is.  See cback_event_t enumeration.
		 * @param epoch_sec:  A pointer to a 32bit time in seconds.  Direction depends on the event.
		 * @param epoch_sec:  A pointer to a 32bit time in micro seconds.  Direction depends on the event.
		 */
		typedef void cback_t ( cback_param_t handle, cback_event_t event, uint32_t* epoch_sec, uint32_t* epoch_usec );


		/**
		 * @brief                  Constructor to create the SNTP_If class.
		 * @param                   None
		 * @return                  None
		 */
		SNTP_If( cback_t* cback = Unused_Event_Cback, cback_param_t cback_param = nullptr );

		/**@brief Destructor to delete the instance of the SNTP_If class when it goes out of
		   scope.*/
		~SNTP_If( void );

		/** @brief  update will monitor the variables which are updated by sntp dcid callback*/
		static void Update_Static( void* handle )
		{ reinterpret_cast<SNTP_If*>( handle )->Update(); }

		/** @brief  Update Active NTP server status into DCID*/
		void Set_Active_Server_Status( uint8_t connection_status,
									   uint8_t server_index = DISCONNECTED );

		/** @brief  Set DCID: DCI_UNIX_EPOCH_TIME ram value*/
		void Set_Epoch_Time( uint32_t& sec, uint32_t& us );

		/** @brief  Get DCID: DCI_UNIX_EPOCH_TIME ram value*/
		void Get_Epoch_Time( uint32_t& sec, uint32_t& us );

		/** @brief Lower layer sntp.cpp will use following interfaces to get DCID time*/
		uint32_t Get_Sntp_Update_Time( void );

		/** @brief Lower layer sntp.cpp will use following interfaces to retry DCID time*/
		uint16_t Get_Sntp_Retry_Time( void );

		/**
		 *  @brief Set the sntp server based on the time server - IP address/Domain name
		 *  @param[in] time_server: time_Server string placeholder
		 *	@param[in] dci_owner_sntp_server: sntp time server DCI owner
		 *	@param[in] server_index: Index of the time server in the server list
		 *  @return	DCI callback return type.
		 */
		DCI_CB_RET_TD Set_Sntp_Server_List( uint8_t* time_server, DCI_Owner* dci_owner_sntp_server,
											uint8_t server_index, DCI_ACCESS_ST_TD* access_struct );

#ifdef SNTP_DOMAIN_NAME_SUPPORT
		/**
		 *  @brief Sanitize the sntp string using the lookup table as per the https://en.wikipedia.org/wiki/Hostname
		 *  @param[in] access_struct: DCI access structure for the Time server DCID
		 *  @return	DCI callback return type.
		 */
		DCI_CB_RET_TD Sntp_Dns_Check_Whitelist_Sanitize( DCI_ACCESS_ST_TD* access_struct );

#endif

		/** @brief  Used to calculate offset time*/
		void Calculate_Offset_Time( uint32_t* t1, uint32_t* t2, uint32_t* t3, uint32_t* t4,
									uint32_t* offset );

		/** @brief  Values for SNTP operation types */
		enum sntp_operation_t
		{
			SNTP_SERVICE_DISABLE = 0,			///< User has disabled sntp service
			SNTP_SERVICE_ENABLE_SERVER_ENTRY_VIA_USER,	///< sntp service with manual server entry
			SNTP_SERVICE_ENABLE_SERVER_ENTRY_VIA_DHCP	///< sntp service with auto server entry
		};

		/** @brief  Values for SNTP status */
		enum sntp_status_t
		{
			DISCONNECTED = 0,		///< No server connected
			SERVER_1_CONNECTED,		///< SNTP server 1 is connected
			SERVER_2_CONNECTED,		///< SNTP server 2 is connected
			SERVER_3_CONNECTED		///< SNTP server 3 is connected
		};

		/** @brief  Values for sntp server index */
		enum sntp_server_index_t
		{
			SNTP_SERVER_1_INDEX = 0,	///< SNTP server 1 index
			SNTP_SERVER_2_INDEX,		///< SNTP server 2 index
			SNTP_SERVER_3_INDEX			///< SNTP server 3 index
		};

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 * @n The fact that we are disallowing these standard items should be Doxygen
		 * documented.  Generally private members do not need to be Doxygen documented.
		 */
		SNTP_If( const SNTP_If& object );
		SNTP_If & operator =( const SNTP_If& object );

		/**
		 * @brief                               Link DCID and SNTP service using this callback
		 * @param                               cback_param Parameters for Callback function
		 * @return                              data for Callback function
		 */
		static DCI_CB_RET_TD Sntp_Service_Callback_Static(
			DCI_CBACK_PARAM_TD cback_param, DCI_ACCESS_ST_TD* access_struct )
		{
			return ( reinterpret_cast<SNTP_If*>( cback_param )->Sntp_Service_Callback(
						 access_struct ) );
		}

		/**
		 * @brief The instance based DCI callback handler.
		 * @param access_struct
		 * @return
		 */
		DCI_CB_RET_TD Sntp_Service_Callback( DCI_ACCESS_ST_TD* access_struct );

		/** @brief  Sntp_Update_Static() will call this function*/
		void Update( void );

		/**
		 * @brief Function which provides essentially nothing to the
		 * @param handle
		 * @param event
		 * @param epoch
		 */
		static void Unused_Event_Cback( cback_param_t handle, cback_event_t event,
										uint32_t* epoch_sec, uint32_t* epoch_usec ) {}

		/**
		 * @brief Check if string contains only IP address
		 * @param[in] ip_add: Input String which needs to be validate
		 * @param[in] raw_data: Output raw data will get stored here
		 * @return  0xFF        : Validation failed
		 *			received octets : Number of bytes parsed in successful validation.
		 */
		uint8_t SNTP_Validate_And_Parse_IPv4_Address( char_t* ip_add, void* raw_data );

		/** @brief Flag for sntp trigger event*/
		bool_t m_sntp_update_required;

		/** @brief Sntp dcid pointers*/
		DCI_Owner* m_owner_sntp_service_enable;
		DCI_Owner* m_owner_sntp_server_1;
		DCI_Owner* m_owner_sntp_server_2;
		DCI_Owner* m_owner_sntp_server_3;
		DCI_Owner* m_owner_sntp_update_time;
		DCI_Owner* m_owner_sntp_retry_time;
		DCI_Owner* m_owner_sntp_active_server_status;
		DCI_Owner* m_owner_sntp_time_offset;

		/** @brief Storage to hold the server domain strings read from the DCI*/
		uint8_t* m_sntp_server_1 = nullptr;
		uint8_t* m_sntp_server_2 = nullptr;
		uint8_t* m_sntp_server_3 = nullptr;

#ifdef SNTP_OFFSET_DEBUG
		/** @brief SNTP time offset debug variables */
		static const uint8_t MAX_SNTP_OFFSET_SAMPLES = 11U;
		static const uint8_t VALID_SNTP_OFFSET_INDEX = 1U;
		int64_t m_sntp_time_offset_samples[MAX_SNTP_OFFSET_SAMPLES] = {0U};
		uint8_t m_sntp_time_offset_index = 0U;
#endif

#ifdef ESP32_SETUP
		/** @brief  Define a minimum valid string length to validate the
		 *			SNTP server domain name.Using 12 as a reasonable min
		 *			length value for the SNTP server domain name
		 *			"pool.ntp.org" - Considered the all NTP servers global
		 *			12 characters is minimum length of the domain name.
		 */
		static const uint8_t MIN_SERVER_LEN_FOR_TEST = 12U;
#else
		/** @brief  Define a minimum valid string length to validate the
		 *			SNTP server domain name.Using 13 as a reasonable min
		 *			length value for the SNTP server domain name
		 *			".pool.ntp.org" - Considered the all NTP servers global
		 *			13 characters is minimum length of the domain name.
		 */
		static const uint8_t MIN_SERVER_LEN_FOR_TEST = 13U;
#endif

		/** @brief Constant required to check whether IP Address has all the octets*/
		static const uint8_t MAX_IP_OCTETS = 4U;

		/** @brief  Important Note: Max string length allowed on SNTP Domain name string
		 *			Considered the all NTP global servers Maximum it
		 *			can go upto 35 characters, Safer side kept 40 as max length
		 *			If user updating the Array Cnt of DCI_SNTP_SERVER_1,DCI_SNTP_SERVER_2
		 *			DCI_SNTP_SERVER_3 more than 40 in the DCI sheet, update this const
		 *			to the same value in this constant.
		 */
		static const uint8_t MAX_SNTP_SERVER_NAME_LENGTH = 40U;

		/** @brief Initialize with application space callback function*/
		cback_t* m_rtc_update_cb;
		cback_param_t m_rtc_update_cb_param;

		/** @brief Scaling constants required for second to microsecond conversion*/
		static const uint32_t SEC_TO_MICRO_SEC_SCALE = 1000000U;

		/** @brief Sntp subsecond (32bit) can represent time with  2^32 = 4,294,967,295
		 *              with a precision of about 232 picoseconds. (4295 = 2^32 / 10^6)
		 *              Converting sntp pico second to micro second.
		 */
		static const uint16_t SNTP_PICO_TO_MICRO_SEC_SCALE = 4295U;

		/** @brief This constant indicates the server string received is invalid*/
		static const uint8_t INVALID_SERVER = 0xFF;

};

#endif	// __SNTP_IF_H__
