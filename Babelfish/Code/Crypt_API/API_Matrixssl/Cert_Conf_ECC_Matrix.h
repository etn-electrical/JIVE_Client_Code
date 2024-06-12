/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the ECC class and APIs for the Device Certificate using ECC.
 *
 *	@details Cert_ECC_Matrix contains the necessary methods for device certificate generation using
 *	the Elliptic Curve Cryptography.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CERT_CONF_ECC_MATRIX_H
#define CERT_CONF_ECC_MATRIX_H

#include "Cert_Conf_Lib.h"

class Cert_Conf_ECC_Matrix : public Cert_Conf_Lib
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Cert_ECC_Matrix class.
		 * @param[in] cert_conf: Device certificate configuration.
		 * @param[in] pki_conf_t: Certificate generation algorithm attributes.
		 * @return None
		 */
		Cert_Conf_ECC_Matrix( Cert_Conf_Lib::cert_config_data_t* cert_conf,
							  const PKI_Common::pki_conf_t* pki_conf );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Cert_ECC_Matrix goes out of scope or deleted.
		 *  @return None
		 */
		~Cert_Conf_ECC_Matrix( void );

		/**
		 * @brief Validates the device certificate.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Validate( void );

		/**
		 * @brief Call the Generate function for the device certificate generation and return
		 * pointer.
		 * @param[out] cert_conf: pointer to the device certificate configuration.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get_Cert_Config( void** cert_conf );

	private:

		Cert_Conf_Lib::cert_config_data_t* m_cert_conf;
		psCertConfig_t* volatile m_pscert_conf;
		const PKI_Common::pki_conf_t* m_pki_conf;

		/**
		 * @brief Generate device certificate.
		 * @return status of requested operation.
		 */
		PKI_Common::pki_status_t Generate( void );

		int32_t Write_DN_Attribute( psPool_t* pool, psCertConfig_t* conf, char_t* device_cn,
									uint8_t device_cn_len );

};

#endif	// CERT_CONF_ECC_MATRIX_H
