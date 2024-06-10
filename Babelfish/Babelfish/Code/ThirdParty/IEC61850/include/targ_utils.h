/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2013-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/* File: @linux/targ_utils.h
 *
 * Description: linux specific target layer macros and declarations.
 *
 */


#ifndef TARG_UTILS_H_
#define TARG_UTILS_H_

#define TARG_UTILS_H_CHANGE_FREERTOS

#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/*
 * Platform -specific definitions.
 * Note that vendor.h is included first, so macros can be overridden there.
 */

/*
 *  basic types
 */
#ifndef TMW_INT64
#define TMW_INT64 long long
#endif

#ifndef TMW_INT32
#define TMW_INT32 int 
#endif

/*
 *  Malloc And Free
 */
#ifndef TMW_TARG_MALLOC
#ifndef TARG_UTILS_H_CHANGE_FREERTOS
#include <malloc.h>
#endif
#define TMW_TARG_MALLOC(s) malloc(s)
#endif
#ifndef TMW_TARG_FREE
#define TMW_TARG_FREE(s)   free(s)
#endif

#define TMW_TP_USE_TASKS 1


/*
 * Sleep
 */
#define TMW_TARG_SLEEP( ms )          usleep( ms * 10 )
#define TMW_TARG_SLEEP_EX( ms, flag ) usleep( ms * 10 )


/*
 * String Compare
 */
#define TMW_TARG_STRCMPI(a,b)    strcasecmp(a,b)
#define TMW_TARG_STRNCMPI(a,b,c) strncasecmp(a,b,c)


#define TMW_TARG_MAX_CONN            66  /* MAX_LINUX_EVENTS_PER_TASK + 2 */

#if defined(TMW_USE_THREADS) || defined(SAV_ONLY)

#ifndef TARG_UTILS_H_CHANGE_FREERTOS
#include <pthread.h>
#define TMW_TARG_MAX_SEM 150
typedef pthread_mutex_t *         TmwTargSem_t;
TmwTargSem_t tmwTargSemCreate(void);
TmwTargRc_t tmwTargSemDelete(TmwTargSem_t sem);

#define TMW_TARG_SEM_CREATE( a ) {a = tmwTargSemCreate();}
#define TMW_TARG_SEM_DELETE( a ) tmwTargSemDelete( (TmwTargSem_t)a )
#define TMW_TARG_SEM_TAKE( a )   pthread_mutex_lock( (TmwTargSem_t)a )
#define TMW_TARG_SEM_GIVE( a )   pthread_mutex_unlock( (TmwTargSem_t)a )

/* Event handling */
typedef struct TmwTargEve_t {
    pthread_mutex_t mut;
    pthread_cond_t  cond;
    int             oldval;
    int             newval;
} TmwTargEve_t;

typedef TmwTargEve_t* TmwTargEve_p;
#endif

#ifdef TARG_UTILS_H_CHANGE_FREERTOS
typedef void* pthread_mutex_t;
typedef void* pthread_t;
typedef void* pthread_cond_t;

#define TMW_TARG_MAX_SEM 150
typedef pthread_mutex_t          TmwTargSem_t;
TmwTargSem_t tmwTargSemCreate(void);
TmwTargRc_t tmwTargSemDelete(TmwTargSem_t sem);

#define TMW_TARG_SEM_CREATE( a ) \
    if (sys_mutex_new(&a) != ERR_OK) {\
  LWIP_ASSERT("failed to create mem_mutex", 0);\
}\

#define TMW_TARG_SEM_DELETE( a ) (sys_mutex_free(&a))

#define TMW_TARG_SEM_TAKE( a )   (sys_mutex_lock(&a)) 
#define TMW_TARG_SEM_GIVE( a )   (sys_mutex_unlock(&a))

/* Event handling */
typedef struct TmwTargEve_t {
    pthread_mutex_t mut;
    pthread_cond_t  cond;
    int             oldval;
    int             newval;
} TmwTargEve_t;

typedef TmwTargEve_t* TmwTargEve_p;
#endif

#define TMW_TARG_MAX_EVE 50
#define TMW_TARG_EVENT               TmwTargEve_p
#define TMW_TARG_EVENT_CREATE( e )   //{ e = tmwTargEventCreate(); }
#define TMW_TARG_EVENT_DELETE( e )   //tmwTargEventDelete( (TMW_TARG_EVENT)e )
#define TMW_TARG_EVENT_SIGNAL( e )   //tmwTargEventSignal( (TMW_TARG_EVENT)e )
#define TMW_TARG_EVENT_RESET( e )
#define TMW_TARG_EVENT_WAIT( e, ms ) tmwTargEventWait( (TMW_TARG_EVENT)e, ms )

#define TMW_TARG_RFC1006_EVENT_PARAMS  \
            int self_pipe[2];          \
            int eventThreadRunning;    \
            int listenFd;

#define TMW_TARG_DL_EVENT_PARAMS       \
            int DlSocket;              \
            int self_pipe[2];          \
            int eventThreadRunning;

/* Thread support */
typedef pthread_t               TmwThreadId_t;
typedef void *                  TmwTargThreadDecl_t;
typedef void *                  TmwTargThreadReturn_t;
typedef void *                  TmwTargThreadArg_t;
typedef void *                  TmwTargThreadHandle_t;
typedef TmwTargThreadDecl_t (* TmwTargThreadFunction_t)( TmwTargThreadArg_t threadParams );

typedef void (* TmwTargTaskFunction_t )(void * args);

/* function to be passed to TMW_TARG_THREAD_CREATE - creates OS thread directly  */
typedef TmwTargThreadFunction_t TmwOSThreadFunction_t;

#define TMW_THREAD_GOOD_RETURN  NULL
#define TMW_THREAD_BAD_RETURN   NULL

#define TMW_DEFAULT_STACK_SIZE  32768
#define TMW_TARG_THREAD_CREATE( pId, pFunc, pFuncParams, stackSize ) \
            (tmwTargThreadCreate( pId, pFunc, \
                                 (TmwTargThreadArg_t) pFuncParams, stackSize ))

TMW_TARG_EVENT tmwTargEventCreate(void);
TmwTargRc_t tmwTargEventDelete(TMW_TARG_EVENT eve);
TmwTargRc_t tmwTargEventSignal(TMW_TARG_EVENT eve);
TmwTargRc_t tmwTargEventWait(TMW_TARG_EVENT eve, unsigned long ms );
#else

/* Semaphore handling */
#define TmwTargSem_t                 void *
#define TMW_TARG_SEM_CREATE( a )
#define TMW_TARG_SEM_DELETE( a )
#define TMW_TARG_SEM_TAKE( a )
#define TMW_TARG_SEM_GIVE( a )

/* Event handling */
#define TMW_TARG_EVENT               void *
#define TMW_TARG_EVENT_PTR           void *
#define TMW_TARG_EVENT_CREATE( e )
#define TMW_TARG_EVENT_DELETE( e )
#define TMW_TARG_EVENT_SIGNAL( e )
#define TMW_TARG_EVENT_RESET( e )
#define TMW_TARG_EVENT_WAIT( e, ms )

/* Thread support */
#define TmwThreadId_t                void *
typedef void (* TmwTargThreadFunction_t)( void *threadParams );

#define TMW_TARG_RFC1006_EVENT_PARAMS 
#define TMW_TARG_DL_EVENT_PARAMS
#endif


/*
 *   Console Support
 */

int kbhit( void );
#define GETCHAR( ch )   ch = fgetc(stdin)
#define ECHOCHAR( ch )
#define PUTCHAR( ch )   fputc( ch, stdout )
#define MOVEDOWN()      fputc( '\n', stdout);
#define CLREOL(b,pos)   \
    { \
        int i; \
        for (i = *(pos); i < 80; i++) \
            fputc( ' ', stdout ); \
        for (i = *(pos); i < 80; i++) \
            fputc( 0x08, stdout ); \
    }

#define UP_ARROW    0x41
#define DOWN_ARROW  0x42
#define RIGHT_ARROW 0x43
#define LEFT_ARROW  0x44
#define BACKSPACE   0x08
#define TTY_INSERT  0x4e
#define TTY_DELETE  0x7f

/* VT102 uses this: */
#define FCN_PREFIX  { 0x5b }
#define FCN_PREFIX_LEN 1




/*
 *  Socket handling
 */
typedef int                           TmwSocket_t;
#ifndef TARG_UTILS_H_CHANGE_FREERTOS
typedef struct sockaddr_in            TmwSockAddrIn_t;
typedef socklen_t                     TmwSockAddrLen_t;
#endif

#define TMW_SOCK_ADDR_LEN             sizeof(struct sockaddr_in)
#define TMW_SOCK_LP_ADDR              struct sockaddr *
#define TMW_SOCK_ERROR                -1
#define TMW_SOCK_INVALID              -1
#define TMW_SOCK_CLOSE( s )           close(s)
#define TMW_SOCK_IOCTL(fd, func, arg) ioctl(fd, func, arg)
#define TMW_SOCK_GET_LAST_ERROR( )    errno
#define TMW_SOCK_EWOULDBLOCK          EWOULDBLOCK
#define TMW_SOCK_EINPROGRESS          EINPROGRESS
#define TMW_SOCK_ENOBUFS              ENOBUFS
#define TMW_SOCK_SEND_FLAGS           MSG_NOSIGNAL

#define TMW_SOCK_STARTUP
#define TMW_SOCK_CLEANUP

#if !defined( RFC1006_KEEPALIVE_TIME )
    #define RFC1006_KEEPALIVE_TIME      5L   /* seconds idle until first keep-alive sent */
#endif

#if !defined( RFC1006_KEEPALIVE_INTERVAL )
    #define RFC1006_KEEPALIVE_INTERVAL   1L   /* seconds between retransmissions */
#endif

#if !defined( RFC1006_KEEPALIVE_COUNT )
    #define RFC1006_KEEPALIVE_COUNT   5L      /* interations before socket close */
#endif

#if !defined(RFC1006_USER_KEEPALIVE_INTERVAL )
    /* Linux does not timeout on the keepalive if there is a write pending
     * kernel 2.6.37 adds this option to workaround the issue */

    #define RFC1006_USER_KEEPALIVE_INTERVAL 10000
#endif

#define TMW_SOCK_NON_BLOCKING( s, retval ) \
    { \
        struct sigaction sa; \
        memset( &sa, 0, sizeof(sa) ); \
        sa.sa_handler = SIG_IGN; \
        sigaction( SIGPIPE, &sa, (struct sigaction *) NULL ); \
        retval = fcntl( s, F_SETFL, O_NONBLOCK ); \
    }

#define TMW_SOCK_REUSEADDR( s, ret ) \
    { \
        int cmd_arg = 1; \
        ret = setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &cmd_arg, sizeof(cmd_arg) ); \
   }

#define TMW_SOCK_KEEPALIVE( s, ret ) \
    { \
        int keepalive_enabled = 1; \
        int keepalive_time = RFC1006_KEEPALIVE_TIME; \
        int keepalive_count = RFC1006_KEEPALIVE_COUNT; \
        int keepalive_interval = RFC1006_KEEPALIVE_INTERVAL; \
        ret = setsockopt( s, SOL_SOCKET, SO_KEEPALIVE,  &keepalive_enabled, sizeof( keepalive_enabled ) ); \
        setsockopt(s, IPPROTO_TCP, TCP_KEEPIDLE, &keepalive_time, sizeof( keepalive_time ) ); \
        keepalive_time =  RFC1006_USER_KEEPALIVE_INTERVAL; \
        setsockopt(s, IPPROTO_TCP, TCP_USER_TIMEOUT, &keepalive_time, sizeof( keepalive_time ) ); \
        setsockopt(s, IPPROTO_TCP, TCP_KEEPCNT,  &keepalive_count, sizeof( keepalive_count ) ); \
        setsockopt(s, IPPROTO_TCP, TCP_KEEPINTVL, &keepalive_interval, sizeof( keepalive_interval ) ); \
    }

#define TMW_SOCK_NO_DELAY( s, ret ) \
    { \
        int cmd_arg = 1; \
        ret = setsockopt( s, SOL_TCP, TCP_NODELAY,  \
                (char *) &cmd_arg, sizeof(cmd_arg) ); \
    }

#define TMW_SOCK_WRITE_SELECTED( s, result ) \
    {   \
        struct timeval timeout;  \
        fd_set write_fds;  \
        timeout.tv_sec = 0;  \
        timeout.tv_usec = 0;  \
        FD_ZERO( &write_fds );  \
        FD_SET( s, &write_fds );  \
        result = select( s + 1, NULL, &write_fds, NULL, &timeout );  \
    }

#define TMW_SOCK_READ_SELECTED( s, result ) \
    {  \
        struct timeval timeout; \
        fd_set read_fds; \
        timeout.tv_sec = 0;  \
        timeout.tv_usec = 0;  \
        FD_ZERO( &read_fds ); \
        FD_SET( s, &read_fds ); \
        result = select( s + 1, &read_fds, NULL, NULL, &timeout ); \
    }

#define TMW_SOCK_CLOSE_SELECTED( s, result ) \
    {  \
        struct timeval timeout; \
        fd_set except_fds; \
        timeout.tv_sec = 0;  \
        timeout.tv_usec = 0;  \
        FD_ZERO( &except_fds ); \
        FD_SET( s, &except_fds ); \
        result = select( s, NULL, NULL, &except_fds, &timeout ); \
    }

#if defined(MMSD_USE_FILE_SERVICES)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/* File handling */
#define TMW_TARG_FILE_CHDIR(d)    chdir(d)
#define TMW_TARG_FILE_DELIMITER   '/'

#endif /* MMSD_USE_FILE_SERVICES */


#endif /* TARG_UTILS_H_ */
