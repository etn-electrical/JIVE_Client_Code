/** @file platform_ltk.cpp Platform-specific intialization code for Eaton IoT Device SDK, and Azure. */

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Extended by Eaton Corp for platform specifics.

#include "Ssl_Context.h"
#include "Eth_if.h"
#include "IoT_Config.h"
#include "iot_device.h"
#include "azure_c_shared_utility/platform.h"
// #include "EthernetInterface.h"
// #include "NTPClient.h"
// #include "azure_c_shared_utility/optimize_size.h"
// #include "azure_c_shared_utility/xio.h"
#include "tlsio_matrixssl.h"
#include "azure_c_shared_utility/threadapi.h"
#include "platform_ltk_azure.h"
#include "azure_c_shared_utility/xlogging.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

#ifdef ESP32_SETUP
#include "esp_netif.h"
#include "tcpip_adapter_types.h"
#include "esp_interface.h"
#endif


extern "C" time_t get_time( time_t* currentTime );

/** Define the timestamp for a simple test for system clock
 * validity to January 1, 2020 12:00:01 AM.
 * The system clock has to be beyond this time to be valid. */
#define TEST_TIME_FOR_VALIDITY 1577836801

static Ssl_Context* bf_ssl_context_handle = nullptr;

/** Create a semaphore that we can use to prematurely wake the
 * messageThread from its normal sleep. */
static OS_Semaphore* msg_thrd_wake_semaphore = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Wait_Till_Ip_Configured( void )
{
#ifdef WIFI_STATION_MODE
	tcpip_adapter_ip_info_t ip;
	memset( &ip, 0, sizeof( tcpip_adapter_ip_info_t ) );
	// Checking netowrk link up status
	bool status = tcpip_adapter_is_netif_up( ( tcpip_adapter_if_t )ESP_IF_WIFI_STA );
	if ( status != true )
	{
		Log_Error( "Unable to Get IP Address" );
	}

#else
	Eth_if::Wait_Till_Ip_Configured();
#endif

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int setupRealTime( void )
{
	int result = 0;

#if 0
	if ( EthernetInterface::connect() )
	{
		result = __FAILURE__;
	}
	else
	{
		NTPClient ntp;
		if ( ntp.setTime( "0.pool.ntp.org" ) != 0 )
		{
			result = __FAILURE__;
		}
		else
		{
			result = 0;
		}
		EthernetInterface::disconnect();
	}
#endif
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int platform_init( void )
{
	int result = 0;

#if 0
	if ( EthernetInterface::init() )
	{
		result = __FAILURE__;
	}
	else if ( setupRealTime() != 0 )
	{
		result = __FAILURE__;
	}
	else if ( EthernetInterface::connect() )
	{
		result = __FAILURE__;
	}
	else
	{
		result = 0;
	}
#endif

	// wait for IP address allocation, must
	Wait_Till_Ip_Configured();

	if ( msg_thrd_wake_semaphore == nullptr )
	{
		// Allocate the semaphore for our messageThread's timing
		msg_thrd_wake_semaphore = new OS_Semaphore();
		if ( msg_thrd_wake_semaphore == nullptr )
		{
			LogError( "Could not allocate semaphore; can't proceed with IoT" );
			result = __FAILURE__;
		}
		/* Wait for ssl context */
		else if ( bf_ssl_context_handle != nullptr )
		{
			bf_ssl_context_handle->Wait_For_SSL_Context();
		}
		else
		{
			BF_ASSERT( false );
		}
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const IO_INTERFACE_DESCRIPTION* platform_get_default_tlsio( void )
{
	return ( tlsio_matrixssl_get_interface_description() );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
STRING_HANDLE platform_get_platform_info( PLATFORM_INFO_OPTION options )
{
	// No applicable options, so ignoring parameter
	( void )options;

	// Expected format: "(<runtime name>; <operating system name>; <platform>)"

	return ( STRING_construct( "(native; FreeRTOS; PX_Green)" ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platform_deinit( void )
{
#if 0
	EthernetInterface::disconnect();
#endif
	delete msg_thrd_wake_semaphore;
	msg_thrd_wake_semaphore = nullptr;
}

// Do a platform-specific sleep to pace the message thread.
extern "C" void platform_message_thread_sleep( void )
{
	if ( msg_thrd_wake_semaphore != nullptr )
	{
		// Wait on the semaphore, up to the time we have configured
		// If it is signaled, then we will return earlier.
		msg_thrd_wake_semaphore->Pend( IOT_MESSAGE_SLEEP_MS );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void platform_wake_message_thread( void )
{
	if ( msg_thrd_wake_semaphore != nullptr )
	{
		// Signal our semaphore, to wake the messageThread
		msg_thrd_wake_semaphore->Post();
	}
}

/*
   Function to attach SSL_Context to the iot code, by saving it locally
   for later retrieval (with Get_IOT_SSL_Context()).
   This is called during product initialization, eg from PROD_SPEC_CERT_RSA_SS_Init().
 */
void Attach_IOT_SSL_Context( Ssl_Context* bf_ssl_context )
{
	if ( bf_ssl_context != nullptr )
	{
		bf_ssl_context_handle = bf_ssl_context;
	}
}

/*
   Function to provide ssl_context to the other iot files
 */
extern "C" Ssl_Context* Get_IOT_SSL_Context( void )
{
	return ( bf_ssl_context_handle );
}

// Provides the trusted certificate to authenticate the Azure servers.
const char* get_azure_trusted_certificate()
{
	return ( certificates );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Is_System_Clock_Set( void )
{
	bool status = false;
	/* Check to make sure that the system time has been set.
	 * Can't connect to Azure unless the system clock (for TLS)
	 * is correct to within a few hours of the real time.
	 * So a very simple but revealing test here to see if
	 * we're at least past the date of code development! */
	time_t system_time_epoch_secs = get_time( nullptr );

	if ( system_time_epoch_secs < TEST_TIME_FOR_VALIDITY )
	{
		// LogInfo( "System clock not correctly set yet; can't proceed with IoT" );
	}
	else
	{
		status = true;
	}
	return ( status );
}

// Interceptor for the malloc() function for use in the IoT SDK.
void* iot_malloc( size_t size )
{
	return ( Ram::Allocate_Basic( size ) );
}

// Interceptor for the free() function for use in the IoT SDK.
void iot_free( void* ptr )
{
	Ram::De_Allocate( ptr );
}
