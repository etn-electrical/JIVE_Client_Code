// @File platform_ltk_azure.h provides missing Azure functionality
// for the PX Green Babelfish "adapter" files.

#ifndef PLATFORM_LTK_AZURE_H
#define PLATFORM_LTK_AZURE_H

// Inexplicably Missing from errno.h:
#define  ENOMEM          12  /* Out of memory */
#define  EINVAL          22  /* Invalid argument */

// Sometimes needs to be defined here, eg when '#define _Restrict       __restrict' isn't defined
#ifndef __restrict
#define __restrict      /* as nothing */
#endif
#ifndef _Restrict
#define _Restrict      /* as nothing */
#endif

// Declare this prototype
extern int socketio_get_fd();

// From azure_c_shared_utility/macro_utils.h:
#ifndef __FAILURE__
#define __FAILURE__ 1
#endif

/** Provides the trusted certificate to authenticate the Azure servers.
 * Currently this returns the "Baltimore" certificate provided by Microsoft.
 * This interface function was added to provide the trusted certificate early
 * in the Babelfish initialization cycle, which is when it is most needed
 * by Matrix SSL.
 * @return Text of the trusted certificate for Azure servers.
 */
extern const char * get_azure_trusted_certificate();

/** Simple utility that lets app code check that the tlsio_matrixssl layer is "open".
 * Ie, connected: tlsio_state == TLSIO_STATE_OPEN
 * @return True if the TLS IO State is "Open", else False if closed or in error state.
 */
extern bool is_tlsio_open();

/** Simple utility that lets app code check whether tlsio_matrixssl layer is retrying to open
 * the connection
 * @return True if TLS IO layer retry is in progress, else false if retry not started or
 * retries are over
 */
extern bool get_tlsio_retry_status( void );

/** Function that sets the tlsio retry status flag
 * @param retry_status true or false
 */
extern void set_tls_io_retry_status( bool retry_status );

#endif
