/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef DTLS_CONFIG_H
   #define DTLS_CONFIG_H

/*Enable for LWM2M communication over DTLS*/


/*
 *****************************************************************************************
 *		Includes
 *****************************************************************************************
 */
// #if defined (uC_STM32F407_USAGE) || defined (uC_STM32F427_USAGE) || defined (uC_STM32F437_USAGE) || defined
// (uC_STM32F767_USAGE)
#ifdef uC_STM32F427_USAGE
/*Enable for LWM2M protocol*/
// #define LWM2M_SUPPORT
#endif

#ifdef LWM2M_SUPPORT

#define PX_GREEN_DTLS_SUPPORT
#define USE_ECC
#define USE_DH
#define USE_AES	/* Enable/Disable AES */	/*Enabled for LWM2M*/
#define USE_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256	/**< @security NIST_SHOULD */

#endif	// LWM2M_SUPPORT


/*
    Timeout and debug settings
 */
#  define MIN_WAIT_SECS   1	/* SHOULD be 1.  Also the default */
#  define MAX_WAIT_SECS   8	/* SHOULD be 2^x as time is doubled on timeout */


#endif	// DTLS_CONFIG_H


