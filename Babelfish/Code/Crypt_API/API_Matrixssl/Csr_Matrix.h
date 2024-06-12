/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides the RSA class and APIs for the Device Certificate using RSA.
 *
 *	@details Csr_Matrix contains the necessary methods for device certificate generation using
 *	the RSA algorithm.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef CSR_MATRIX_H
#define CSR_MATRIX_H

#include "Csr_Lib.h"
#include "Cert_Conf_Lib.h"
#include "Key_Lib.h"

class Csr_Matrix : public Csr_Lib
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Csr_Matrix class.
		 * @param[in] cert_conf: Device certificate configuration.
		 * @param[in] pki_conf_t: Certificate generation algorithm attributes.
		 * @return None
		 */
		Csr_Matrix( Key_Lib* key, Cert_Conf_Lib* cert_conf );

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Csr_Matrix class.
		 * @param[in] cert_conf: Device certificate configuration.
		 * @param[in] pki_conf_t: Certificate generation algorithm attributes.
		 * @return None
		 */
		Csr_Matrix( uint8_t* csr_mem, uint16_t csr_mem_len );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Csr_Matrix goes out of scope or deleted.
		 *  @return None
		 */
		~Csr_Matrix( void );

		/**
		 * @brief Call the Generate function for the device certificate generation and return
		 * pointer.
		 * @param[out] cert_conf: pointer to the device certificate configuration.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Get( void** dn, void** pub_key, void** req_ext );



		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		PKI_Common::pki_status_t Get_Key_Handle( Key_Lib** key_handle )
		{
			PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

			if ( m_key != nullptr )
			{
				*key_handle = m_key;
				ret_status = PKI_Common::SUCCESS;
			}
			return ( ret_status );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		PKI_Common::pki_status_t Get_Cert_Conf_Handle( Cert_Conf_Lib** cert_conf_handle )
		{
			PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;

			if ( m_cert_conf != nullptr )
			{
				*cert_conf_handle = m_cert_conf;
				ret_status = PKI_Common::SUCCESS;
			}
			return ( ret_status );
		}

	private:

		/**
		 * @brief Validates the device certificate.
		 * @return status of requested operation.
		 */
		virtual PKI_Common::pki_status_t Generate( void );

		Key_Lib* volatile m_key;
		Cert_Conf_Lib* volatile m_cert_conf;
		uint8_t* m_csr_mem;
		uint16_t m_csr_mem_len;
		volatile bool m_del_csr_mem = false;

};

#endif	// CSR_MATRIX_H
