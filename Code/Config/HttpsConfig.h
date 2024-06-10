/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef HTTPS_CONFIG_H
#define HTTPS_CONFIG_H

#define HTTPS_PERSISTENT_CONNECTION

/* Maximum SSL connection */
static const uint8_t MAXCONN = 5U;

/* Server will close the connection if no response received within SSL_TIMEOUT */
static const uint32_t SSL_TIMEOUT = 6000U;	/* In milliseconds */

/* Socket event monitoring time */
static const uint16_t SELECT_TIME = 500U;	/* In milliseconds */

/* Static resources like index.html and others are generally big in size.
   Server may take significant time to transfer such resources. Other https requests
   may get unattended within this time. To resolve this issue,
   Server will do time slicing for all static (resource) requests.
   Server will attend each connection for SERVING_TIME_PER_CONN time.
 */
static const uint32_t SERVING_TIME_PER_CONN = 2000U;/* In milliseconds */

#ifdef HTTPS_PERSISTENT_CONNECTION
/* Socket will remain open for KEEP_ALIVE_TIME seconds */
static const uint32_t KEEP_ALIVE_TIME = 2U;		/* In seconds */
#endif

#endif
