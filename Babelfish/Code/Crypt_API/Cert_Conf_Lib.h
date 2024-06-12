/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the base class and APIs for the Device Certificate operations.
 *
 *	@details Cert_Lib contains the necessary structure and methods for device certificate
 *	configuration.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CERT_CONF_LIB_H
#define CERT_CONF_LIB_H

#include "Includes.h"
#include "PKI_Common.h"

class Cert_Conf_Lib
{
	public:

		/**
		 * @brief Structure used to define the attributes of device certificate.
		 */
		struct cert_config_data_t
		{
			uint32_t generate_time_epoch;
			uint16_t valid_days;
			char_t* serial_num;		// Mandatory
			char_t* country;
			char_t* state_or_province;
			char_t* locality;
			char_t* org_unit;
			char_t* organization;
			char_t* email_addr;
			char_t* common_name;	// Mandatory
			char_t* san_dns_name;
			char_t* san_ip_address;
		};

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Cert_Lib goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Cert_Conf_Lib()
		{}

		/**
		 * @brief Validates the device certificate.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Validate( void )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Call the Generate function for the device certificate generation and return pointer.
		 * @param[out] cert_conf: pointer to the device certificate configuration.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Cert_Config( void** cert_conf )
		{
			return ( PKI_Common::OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Cert_Conf_Lib()
		{}

};

#endif	// CERT_CONF_LIB_H_
