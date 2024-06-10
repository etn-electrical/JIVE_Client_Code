/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides APIs that calls code sigining and certificate generation APIs
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef PROD_SPEC_PKI_H_
#define PROD_SPEC_PKI_H_

#include "Includes.h"
#include "PKI_Common.h"
#include "CR_Tasker.h"

/**
 * @brief enum for code signing status
 */
enum pki_state_enum_t
{
#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
	PKI_CERT_INIT,
	PKI_CERT_PROGRESS,
	PKI_CERT_FAILURE,
#endif
	PKI_CS_INIT,
	PKI_CS_PROGRESS,
	PKI_CS_SUCCESS,
	PKI_CS_FAILURE = -1
};

static const uint16_t PKI_TIMER_PERIOD_250_MS = 250U;

/**
 * @brief code signing and communication certfificate initialization
 * @details initiate certificate store, create a task to handle code signing and certificate
 * generation
 * @param None.
 * @return None.
 */
void PROD_SPEC_PKI_Init( void );


/**
 * @brief This function calls code signing API to verify fvk and firmware signature when firmware
 * is located in internal flash memory
 * @details Enable flag to start firmware signature verification
 * @param[in] nv_handle : NV_Ctrl handle on which verification needs to be performed
 * @param[in] key_upgrade_status : Key status
 * @param[out] key_upgrade_status : Message for bootloader, if set then bootloader should upgrade
 * code signing keys in certificate store.
 * @return Status of requested operation.
 */
PKI_Common::pki_status_t Prod_Spec_Code_Sign( NV_Ctrl* nv_handle, bool* key_upgrade_status );

/**
 * @brief Update Keys/Certs in cert store
 * @details This function is called when key upgrade message is received after APP Fw upgrade
 * @param None
 * @return Status of requested operation.
 */
bool Update_Code_Sign_Cert( void );

#endif	// PROD_SPEC_PKI_H_
