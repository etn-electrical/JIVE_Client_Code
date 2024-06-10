/**
 *****************************************************************************************
 *  @file               httpd.h header file  for http functionality.
 *  @brief              This file shall wrap all the functions required to initialize the HTTP server
 *  @copyright          2001-2003 Swedish Institute of Computer Science.
 *                      All rights reserved.
 *  @ note              Redistribution and use in source and binary forms, with or without modification,
 *                      are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * This version of the file has been modified by Luminary Micro to offer simple
 * server-side-include (SSI) and Common Gateway Interface (CGI) capability.
 *****************************************************************************************
 */

#ifndef __HTTPD_H__
#define __HTTPD_H__

#include "httpd_rest_dci.h"
#include "Modbus_Net.h"
#include "httpd_common.h"
/**
 * @brief:            This function is used as an external interface to initialize the HTTP server.
 * @param[in]REST_TARGET_INFO_ST_TD Structure containing the details of the various parameters to be input
 *
 * @note              This function first binds the connection to a local portnumber and IP address.
 *                    It helps in mapping all authentication based parameters.It finds all rest data structures
 *                    and thus initializes the HTTP server.
 */
void httpd_init( void );

static const uint16_t HTTP_DEFAULT_PORT = 80U;

#endif	/* __HTTPD_H__ */
