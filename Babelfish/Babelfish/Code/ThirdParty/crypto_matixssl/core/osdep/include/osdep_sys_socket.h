/** osdep_sys_socket.h
 *
 * Wrapper for system header osdep_sys_socket.h
 */

/*****************************************************************************
* Copyright (c) 2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

/* This file just includes system header sys/socket.h.
   In case your system does not include all functions
   socket/connect/send via that file or
   does not have implementation of sys/socket.h, please
   customize this place holder header.
*/

#ifndef OSDEP_SYS_SOCKET_H_DEFINED
#define OSDEP_SYS_SOCKET_H_DEFINED 1


#include <sys/socket.h>

/* You may redefine the wrappers below in case your target system does not
   provide all of the functions below. The functions are from C standard
   ISO C99 and other common standards.
   The defines may be overrided from command line. */

/* Macro that provides Socket, which is macro wrapper for socket. */
#ifndef Socket
#define Socket socket
#endif /* Socket */

/* Macro that provides Connect, which is macro wrapper for connect. */
#ifndef Connect
#define Connect connect
#endif /* Connect */

/* Macro that provides Send, which is macro wrapper for send. */
#ifndef Send
#define Send send
#endif /* Send */



#endif /* OSDEP_SYS_SOCKET_H_DEFINED */
