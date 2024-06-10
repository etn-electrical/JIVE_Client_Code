/**
 **********************************************************************************************
 * @file            Dtls_Green_Client.h Header File for interface class.
 *
 * @brief           Processor Independent Abstract Class for EMAC driver implementation
 *
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef DTLS_GREEN_CLIENT

#define DTLS_GREEN_CLIENT

#include "Includes.h"

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "app.h"
#include "OS_Tasker.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "Eth_if.h"
#include "ssl_connection.h"
#include "matrixssl/matrixsslApi.h"
#ifndef ESP32_SETUP
#include "Device_Cert_ECC.h"
#include "Device_Cert.h"
#endif

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
bool_t Dtls_Client_Init( enum load_key_type key_type );

#endif	// DTLS_GREEN_CLIENT
