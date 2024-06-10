/**
 *****************************************************************************************
 * @file		Prod_Spec_Cert_ECC_CA.h
 *
 * @brief		Product Specific initialization
 * @details		This file contains declaration for generating CA signed certificate.
 *
 *
 * @copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_CERT_ECC_CA_H
#define PROD_SPEC_CERT_ECC_CA_H

#include "Cert_Store.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t CERT_GEN_EPOCH_TIME = 1577860200U;// GMT: January 1, 2020 12:00:00 AM
static const uint16_t CA_CERT_VALID_DAYS = ( 20 * 365U );
static const uint16_t SSL_CERT_VALID_DAYS = ( 20 * 365U );
static const char_t COUNTRY[] = "US";
static const char_t STATE_OR_PROVINCE[] = "PA";
static const char_t LOCALITY[] = "Pittsburgh";
static const char_t ORG_UNIT[] = "CCIS";
static const char_t ORGANIZATION[] = "Eaton Corporation";
static const char_t EMAIL_ADDRESS[] = "testmail@eaton.com";
static const char CA_COMMON_NAME[] = "PXGCA-";

static const uint8_t MAC_ADDR_LEN = 6U;
static const uint16_t MAX_PRIV_KEY_LEN = 1300U;
static const uint16_t MAX_CERT_LEN = 1024U;

static const uint16_t MAC_ADDR_MAX_STR = 20U;
static const uint8_t SERIAL_NUM_LEN = 6U;
static const uint8_t IP_ADDR_STR_LEN = 16U;
static const uint16_t SOFT_RESET_RESPONSE_TIME_uS = 5000U;
static const uint32_t FINGERPRINT = 0x12345678;
static const uint16_t SERIAL_NUM_STR_MAX_LEN = 20U;

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */

/**
 * @brief		product specific Device certificate initialization and address conflict
 *  detection.
 * @details		This function will create a new thread which will eventually generate a
 *				CA-signed ECC certificate. After completion of task, this thread will
 *				signal other waiting modules and then delete itself.
 * @param[in]	void
 * @return      none
 */
void PROD_SPEC_CERT_ECC_CA_Init( void );


/**
 * @brief Handle CA and device certificate generation.
 * @details Generate CA_private_key, CA_certificate and Device_Key if not available in cert store
 * block and update the same in cert store block this function generates device certificate on power
 * up.
 * @param[in] cert_store_if : Pointer to cert store block
 * @return      Status of requested operation
 */
PKI_Common::pki_status_t Prod_Spec_Certificate_Handler( Cert_Store* cert_store_if );

#endif
