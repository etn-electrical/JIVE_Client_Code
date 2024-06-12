/**
 *****************************************************************************************
 *  @file	Ssl_Context.h
 *	@brief	SSL Context management class.
 *
 *	@details This file should provide methods to create and manage SSL context.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef SSL_CONTEXT_H
#define SSL_CONTEXT_H

#include "Includes.h"
#include "OS_Semaphore.h"

enum load_key_type
{
	KEY_RSA_LOAD = 0,
	KEY_ECC_LOAD = 1
};

/**
 ****************************************************************************************
 * @brief This is Ssl Context class
 * @details It provides method to create new context and to load keys.
 * @li It provides public methods to
 * @li 1. Create new context.
 * @li 2. Load keys.
 * @li 3. Return context.
 ****************************************************************************************
 */
class Ssl_Context
{
	public:

		uint32_t m_conn_timeout;
		uint32_t m_conn_idle_state_timeout;

		/**
		 * @brief Constructor.
		 * @details This constructor creates a new ssl_context and initialize the ssl
		 * library.
		 * @param[in] void.
		 * @return this.
		 */
		Ssl_Context( void );

		/**
		 * @brief Destructor.
		 * @details This destructor deallocates the memory allocated to constructor.
		 * @param[in] void.
		 * @return none.
		 */
		~Ssl_Context( void );


		/**
		 * @brief Method to load keys.
		 * @details This method load certificate and private keys based on the algorithm used.
		 * @param[in] key_type: enum value to define type of the key.
		 * @param[in] cert_buf: Pointer to the certificate buffer.
		 * 	From Matrix SSL: The X.509 ASN.1 identity certificate for this SSL peer.
		 *  This parameter is always relevant to servers.
		 *  Clients will want to supply an identity certificate and private key
		 * 		if supporting mutual authentication. NULL otherwise.
		 * @param[in] cert_len: Length of the certificate buffer.
		 * @param[in] priv_buf: Pointer to the private key buffer.
		 * 	The PKCS#8 or “SEC1: Elliptical Curve Cryptography” private EC key that was used to sign the certBuf.
		 *  This parameter is always relevant to servers.
		 *  Clients will want to supply an identity certificate and private key
		 * 		if supporting mutual authentication. NULL otherwise.
		 * @param[in] priv_len: Length of the private key buffer.
		 * @param[in] CA_buf: Pointer to the trusted CA certificate buffer.
		 * 	The X.509 ASN.1 stream of the trusted root certificates (Certificate Authorities) for this SSL peer.
		 *  This parameter is always relevant to clients.
		 *  Servers will want to supply a CA if requesting mutual authentication. NULL otherwise.
		 * @param[in] CA_len: Length of the trusted CA certificate buffer.
		 * @return true if success, false if failed.
		 */
		bool Load_Keys( enum load_key_type key_type, const unsigned char* cert_buf, int32_t cert_len,
						const unsigned char* priv_buf, int32_t priv_len,
						const unsigned char* CA_buf, int32_t CA_len );

		/**
		 * @brief Method to return our SSL context for working with Matrix SSL.
		 * @details This method returns the context that we created
		 * 			when mtxssl_if_SSL_CTX_new() was called. Mostly useful for getting its keys structure.
		 * @param[in] void.
		 * @return Opaque (ie, void) pointer to the Matrix SSL context (actually of type SSL_CTX*).
		 */
		inline void* Get_Context( void )
		{
			return ( m_sslContext );
		}

		/**
		 * @brief Initialize library.
		 * @details Initialize SSL library.
		 * @param[in] void.
		 * @return True if initialization succeeds, else False.
		 */
		static bool Library_Init( void );

		/**
		 * @brief Initialize library.
		 * @param[in] conn_timeout: ssl connection timeout.
		 * @param[in] idle_state_timeout: timeout for keep alive feature.
		 * @return true if success, false if unsuccessful.
		 */
		bool Initialize_Timeout( int32_t conn_timeout, int32_t idle_state_timeout = 0 );

		/**
		 * @brief Semaphore Handling - waits until our semaphore is signaled.
		 * @details This function handles semaphore to sync with other threads.
		 * @param[in] void.
		 * @return NULL.
		 */
		void Wait_For_SSL_Context( void );

		/**
		 * @brief Function to release (post) our semaphore.
		 * @details
		 * @param[in] void.
		 * @return NULL.
		 */
		void Signal( void );

	private:
		/** Holds an opaque pointer to our SSL_CTX context (defined in opensslApi.h). */
		void* m_sslContext = nullptr;

		/* Semaphore to halt other threads which need ssl_context */
		OS_Semaphore* m_sslcontext_semph;

		/** Static flag that indicates whether we have initialized the Matrix SSL library yet.
		 * Because this is static, a second instance of the Ssl_Context will know not to try
		 * to re-initialize that library. */
		static bool m_library_init;
};

#endif
