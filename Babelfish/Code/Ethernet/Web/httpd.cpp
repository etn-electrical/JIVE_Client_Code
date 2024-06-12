/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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
 */

#include <string.h>

#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "Eth_if.h"
#include "httpd.h"
#include "http_auth.h"
#ifdef __DIGEST_ACCESS_AUTH__
#include "httpd_digest_authentication.h"
#endif
#include "Mem_Check.h"


#ifdef HAMS_DEBUG
#include <assert.h>
#include <stdio.h>
#define DERR( ... ) fprintf( stderr, __VA_ARGS__ )
#define _ASSERT( x ) assert( x )
#else
#define DERR( ... )
#define _ASSERT( x ) do{ \
		if ( !( x ) ){ \
			while ( 1 ){}; \
		} \
} while ( 0 )
#endif

#include "server.h"
#include "EthConfig.h"
#include "Server_IO.h"
#include "Babelfish_Assert.h"

void s_update_host_from_ip( uint32_t ip );

void close_conn( struct tcp_pcb* pcb );

static uint32_t s_send_cb( hams_parser* parser, const uint8_t* data, uint32_t length,
						   uint32_t copy );

static void http_cb_err( void* arg, err_t err );

static err_t http_cb_poll( void* arg, struct tcp_pcb* pcb );

static err_t http_accept( void* arg, struct tcp_pcb* pcb, err_t err );

static err_t drop_recv( void* arg, struct tcp_pcb* pcb, struct pbuf* p,
						err_t err );

static err_t echo_cb_recv( void* arg, struct tcp_pcb* pcb, struct pbuf* p,
						   err_t err );

static void echo_cb_err( void* arg, err_t err );

static err_t echo_cb_sent( void* arg, struct tcp_pcb* pcb, u16_t len );

static bool_t InvalidRamPtr( void* memPtr );

#ifdef DEBUG
static uint32_t s_count_connections = 0U;
static uint32_t s_conn_errs = 0U;
static uint32_t s_closes = 0U;
#endif


extern "C"
{
extern uint8_t __ICFEDIT_region_RAM_start__;
extern uint8_t __ICFEDIT_region_RAM_end__;
}

// static const uint32_t ram_start = ( uint32_t ) ( &__ICFEDIT_region_RAM_start__ );
// static const uint32_t ram_end = ( uint32_t ) ( &__ICFEDIT_region_RAM_end__ );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t s_send_cb( hams_parser* parser, const uint8_t* data, uint32_t length,
					uint32_t copy )
{
	uint8_t return_val = 0U;
	http_server_req_state* rs = Http_Server_Get_State( parser );
	tcp_pcb* pcb = reinterpret_cast<tcp_pcb*>( rs->stack_data );

	BF_ASSERT( !InvalidRamPtr( pcb ) );

	if ( 0U == data )
	{
		if ( copy == 3 )// close conn and unhook callbacks
		{
			close_conn( pcb );

		}
		else
		{
			tcp_shutdown( pcb, 0, 1 );
		}
		return_val = 0U;
	}
	else
	{
		uint8_t flags = copy ? ( static_cast<uint8_t>( TCP_WRITE_FLAG_COPY ) ) : 0U;
		err_t err = tcp_write( pcb, data, length, flags );
		if ( ERR_OK == err )
		{
			return_val = 0U;
		}
		else if ( ERR_MEM == err )
		{
			return_val = 1U;
		}
		else
		{
			BF_ASSERT( false );
			// return_val = 2U;
		}
	}

	return ( return_val );
}

/* LWIP interface functions. */

/******************************************************************************/
void close_conn( struct tcp_pcb* pcb )
{
	// give up and release all resources
#ifdef DEBUG
	++s_closes;
#endif
	tcp_arg( pcb, reinterpret_cast<void*>( NULL ) );
	tcp_sent( pcb, NULL );
	tcp_recv( pcb, NULL );

	tcp_shutdown( pcb, 0, 1 );
}

/* ****************************************************************************
 *****************************************************************************/
// call back function for recv - handling request messages
// note that a request message may be broken into pieces
// so attention must be paid for this function to work properly
// at re-entry
err_t http_cb_recv( void* arg, struct tcp_pcb* pcb, struct pbuf* p,
					err_t err )
{
	bool_t loop_break = false;

	BF_ASSERT( !InvalidRamPtr( arg ) );

	hams_parser* parser = ( reinterpret_cast<hams_parser*>( arg ) );
	http_server_req_state* rs = Http_Server_Get_State( parser );

#ifdef ESP32_SETUP
	parser->general_use_reg = pcb->remote_ip.u_addr.ip4.addr;
#else
	parser->general_use_reg = pcb->remote_ip.addr;
#endif
	parser->remote_port = pcb->remote_port;

	if ( ( err == ERR_OK ) && ( p != NULL ) )
	{
		pbuf* base = p;
		// uint32_t tot_len = base->tot_len;
		uint32_t acc = 0U;
		while ( ( p != NULL ) && ( false == loop_break ) )
		{
			if ( p->len &&
				 ( !( Http_Server_Input( parser,
										 ( reinterpret_cast<const uint8_t*>( p->payload ) ), p->len )
					  & HAMS_EXPECT_RX ) ) )
			{

				/* On error, just drop further input. */
				if ( ( parser->flags & HAMS_FLAG_ERROR ) ||
					 ( parser->flags & HAMS_FLAG_REDIRECT ) )
				{
					tcp_recved( pcb, base->tot_len );
					pbuf_free( base );
					close_conn( pcb );
					tcp_recv( pcb, drop_recv );
					loop_break = true;
				}
				/* Check if going into echo mode. */
				else if ( parser->flags & HAMS_FLAG_ECHO_CONTENT )
				{
					acc += p->len;
					p = p->next;

					/* Setup HTTP callbacks. */
					tcp_recv( pcb, echo_cb_recv );
					tcp_sent( pcb, echo_cb_sent );
					tcp_err( pcb, echo_cb_err );
					tcp_poll( pcb, NULL, 0U );

					/* Write out as many pcbs as we can given space limitation.s */
					uint32_t ss = tcp_sndbuf( pcb );

					while ( p && ( ss > p->len ) )
					{
						( void )tcp_write( pcb, p->payload, p->len, TCP_WRITE_FLAG_COPY );
						acc += p->len;
						parser->content_length -= p->len;
						ss -= p->len;
						p = p->next;
					}
					tcp_recved( pcb, acc );
					pbuf_free( base );

					tcp_arg( pcb, reinterpret_cast<void*>( parser->content_length ) );

					loop_break = true;

				}
				else
				{
					tcp_recved( pcb, acc );
					pbuf_free( base );
					loop_break = true;
				}

			}
			else
			{
				acc += p->len;
				p = p->next;
			}
		}
		if ( false == loop_break )
		{
			tcp_recved( pcb, acc );
			pbuf_free( base );
		}
	}
	else
	{
		/* If not planning to send any data, just close it now.
		 * Otherwise close will happen in http_cb_sent */
		if ( !( HAMS_EXPECT_TX & Http_Server_Input( parser, NULL, 0 ) ) )
		{
			close_conn( pcb );
		}
	}
	return ( ERR_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static err_t echo_cb_recv( void* arg, struct tcp_pcb* pcb, struct pbuf* p,
						   err_t err )
{
	if ( ( err == ERR_OK ) && p )
	{
		pbuf* base = p;
		uint32_t ss = tcp_sndbuf( pcb );
		uint32_t acc = 0U;

		while ( ( p != NULL ) && ( ss > p->len ) )
		// while ((( p && ss )> (p->len ))
		{
			( void ) tcp_write( pcb, p->payload, p->len, TCP_WRITE_FLAG_COPY );
			acc += p->len;
			ss -= p->len;
			p = p->next;
		}
		ULBASE x = reinterpret_cast<ULBASE>( arg );
		if ( acc >= x )
		{
			close_conn( pcb );
		}
		else
		{
			x -= acc;
			tcp_arg( pcb, reinterpret_cast<void*>( x ) );
			tcp_recved( pcb, acc );
			pbuf_free( base );
		}
	}
	else
	{
		close_conn( pcb );
	}
	return ( ERR_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static err_t echo_cb_sent( void* arg, struct tcp_pcb* pcb, u16_t len )
{
	return ( ERR_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void echo_cb_err( void* arg, err_t err )
{
#ifdef DEBUG
	++s_conn_errs;
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static err_t drop_recv( void* arg, struct tcp_pcb* pcb, struct pbuf* p,
						err_t err )
{
	if ( ( err == ERR_OK ) && ( p != NULL ) )
	{
		tcp_recved( pcb, p->tot_len );
		pbuf_free( p );
	}
	else
	{
		close_conn( pcb );
	}
	return ( ERR_OK );
}

/******************************************************************************/
static err_t http_cb_sent( void* arg, struct tcp_pcb* pcb, u16_t len )
{

	if ( arg )
	{

		BF_ASSERT( !InvalidRamPtr( arg ) );

		hams_parser* parser = ( hams_parser* )arg;
		http_server_req_state* rs = Http_Server_Get_State( parser );

		/* TODO check if what HAMS sent has been fully sent. */
		if ( 0U == Http_Server_Output_Sent( parser, 1U ) )
		{
			close_conn( pcb );
		}
	}
	return ( ERR_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static err_t http_cb_poll( void* arg, struct tcp_pcb* pcb )
{
	if ( arg )
	{

		BF_ASSERT( !InvalidRamPtr( arg ) );

		hams_parser* parser = reinterpret_cast<hams_parser*>( arg );
		http_server_req_state* rs = Http_Server_Get_State( parser );

		if ( rs->flags & REQ_FLAG_RETRY )
		{
			rs->flags &= ~REQ_FLAG_RETRY;
			if ( 0U == Http_Server_Output_Sent( parser, 1U ) )
			{
				close_conn( pcb );
			}
		}
	}
	if ( pcb->state == FIN_WAIT_2 )
	{
		tcp_close( pcb );
	}
	return ( ERR_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static void http_cb_err( void* arg, err_t err )
{
#ifdef DEBUG
	++s_conn_errs;
#endif
	LWIP_UNUSED_ARG( err );

	if ( arg )
	{
		hams_parser* parser = ( hams_parser* )arg;
		http_server_req_state* rs = Http_Server_Get_State( parser );

		Http_Server_Output_Sent( parser, 0 );
	}
}

/******************************************************************************/

/******************************************************************************/
// call back function for accept()
static err_t http_accept( void* arg, struct tcp_pcb* pcb, err_t err )
{
	int return_val = 0;
	bool_t https_conn = false;

	if ( ERR_OK != err )
	{
		return_val = err;
	}
	else
	{
		hams_parser* parser = Http_Server_Alloc_Parser( ( void* )pcb, https_conn );
		/* TODO return 429 here. */
		if ( !parser )
		{
			return_val = ERR_MEM;
		}
		else
		{

#ifdef DEBUG
			++s_count_connections;
#endif
			tcp_nagle_disable( pcb );
			/* Tell TCP that this is the structure we wish to be passed for our
			   callbacks. */
			tcp_arg( pcb, reinterpret_cast<void*>( parser ) );

			/* Setup HTTP callbacks. */
			tcp_recv( pcb, http_cb_recv );
			tcp_sent( pcb, http_cb_sent );
			tcp_err( pcb, http_cb_err );

			tcp_poll( pcb, http_cb_poll, 2 );
			return_val = ERR_OK;
		}
	}
	return ( return_val );
}

/*----------------------------------------------------------------------------*/
// external interface to initialize the HTTP server
void httpd_init()
{
	// opening up the listening pcb at well-known port 80
	struct tcp_pcb* pcb;

	pcb = tcp_new();
	tcp_bind( pcb, IP_ADDR_ANY, HTTP_DEFAULT_PORT );
	pcb = tcp_listen( pcb );
	tcp_arg( pcb, pcb );// get listen pcb as argument in http accept, to decrement accept pending there.
	tcp_accept( pcb, http_accept );
	// coverity[leaked_storage]
	Register_Media_On_Server( HTTP_MEDIA, new DCI_Owner( DCI_HTTP_ENABLE_DCID ), s_send_cb );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static bool_t InvalidRamPtr( void* memPtr )
{
	/* As the lwIP files are moved to DTCMRAM region for F767 target.
	   Ram ptr check is removed temporarily, further improvements can be done with better approach*/
	return ( false );

}
