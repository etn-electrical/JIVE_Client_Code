/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Ssl_Context.h"
#include "mtxssl_if.h"
#include "IoT_Config.h"
#include "platform_ltk_azure.h"
// #include "Ethernet_Debug.h"

/*
 *****************************************************************************************
 *		Static Variable
 *****************************************************************************************
 */
bool Ssl_Context::m_library_init = false;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ssl_Context::Ssl_Context( void ) :
	m_sslContext( mtxssl_if_SSL_CTX_new( nullptr ) ),
	m_sslcontext_semph( new OS_Semaphore() ),
	m_conn_timeout( 0U ),
	m_conn_idle_state_timeout( 0U )
{
	if ( m_library_init == false )
	{
		m_library_init = Library_Init();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ssl_Context::~Ssl_Context( void )
{
	delete m_sslcontext_semph;
	mtxssl_if_SSL_CTX_free( m_sslContext );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ssl_Context::Load_Keys( enum load_key_type key_type, const unsigned char* cert_buf,
							 int32_t cert_len, const unsigned char* priv_buf, int32_t priv_len,
							 const unsigned char* CA_buf, int32_t CA_len )
{
	bool return_val = true;
	int32_t status = PS_FAILURE_T;

	// if ( CA_buf != nullptr )
	// {
	// ETH_DEBUG_PRINT( DBG_MSG_DEBUG,  "Given CA: %.60s", (const char*) CA_buf );
	// }

	if ( key_type == KEY_RSA_LOAD )
	{
		status = mtxssl_if_SSL_CTX_load_key_material_RSA( m_sslContext, cert_buf, cert_len,
														  priv_buf, priv_len, CA_buf, CA_len );
	}
	else if ( key_type == KEY_ECC_LOAD )
	{
		status = mtxssl_if_SSL_CTX_load_key_material_ECC( m_sslContext, cert_buf, cert_len,
														  priv_buf, priv_len, CA_buf, CA_len );
	}
	else
	{}

	if ( status != PS_SUCCESS_T )
	{
		return_val = false;
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ssl_Context::Wait_For_SSL_Context( void )
{
	if ( m_sslcontext_semph != nullptr )
	{
		/* Wait for ssl_context */
		( void ) m_sslcontext_semph->Pend( OS_QUEUE_INDEFINITE_TIMEOUT );
		/* Release the semaphore */
		( void ) m_sslcontext_semph->Post();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Ssl_Context::Signal( void )
{
	m_sslcontext_semph->Post();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ssl_Context::Library_Init( void )
{
	bool return_val = false;

	return_val = mtxssl_if_SSL_library_init();

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ssl_Context::Initialize_Timeout( int32_t conn_timeout, int32_t idle_state_timeout )
{
	m_conn_timeout = conn_timeout;
	m_conn_idle_state_timeout = idle_state_timeout;
	return ( true );
}
