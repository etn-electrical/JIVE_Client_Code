/**
 *****************************************************************************************
 *	@file        Certs.h responsible for providing certs interfaces.
 *
 *	@brief       This file shall provide the public APIs to access certs module.
 *	             This module provides REST URL path to access certificate/s
 *
 *	@details     The LTK REST services are designed primarily for use from the adopting  product
 *	             web front end for browser based data access. However, they provide an open and
 *	             standard method for data access from any supporting REST client
 *

 *  @copyright   2019 Eaton Corporation. All Rights Reserved.
 *
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed
 *               here in.  This technical information should not be reproduced or used
 *               without direct written permission from Eaton Corporation.*
 *
 *
 *
 *
 *
 *
 *****************************************************************************************
 */
#ifndef __REST_CERTS_H__
#define __REST_CERTS_H__

#include "Cert_Store.h"

void Attach_Cert_Store_Interface( Cert_Store* handle );

#endif
