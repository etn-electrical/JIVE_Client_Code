// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef TLSIO_TEMPLATE_H
#define TLSIO_TEMPLATE_H

/* This is a template file used for porting 
 * This is the Eaton version used for Babelfish's Matrix SSL/TLS layer. */

/* Make sure that you replace tlsio_template everywhere in this file with your own TLS library name (like tlsio_mytls) */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "azure_c_shared_utility/xio.h"
/* Eaton fix: comment out the next mocking header: */
//#include "azure_c_shared_utility/umock_c_prod.h"

MOCKABLE_FUNCTION(, const IO_INTERFACE_DESCRIPTION*, tlsio_matrixssl_get_interface_description);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TLSIO_TEMPLATE_H */
