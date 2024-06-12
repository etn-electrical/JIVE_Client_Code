/**
 **********************************************************************************************
 *	@file            MDNS_Client.cpp C++ Implementation File for mDNS common functions.
 *
 *	@brief           The file shall include the definitions of all the functions required for
 *                   mDNS initilization and Services.
 *	@details
 *	@copyright       2019 Eaton Corporation. All Rights Reserved.
 *      @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#include "MDNS_Client.h"
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#include "Babelfish_Assert.h"

MDNS_Client::mdns_service_t MDNS_Client::m_total_services = -1;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
MDNS_Client::MDNS_Client( mdns_service_request_t* mdns_service_conf ) :
	m_service_conf( mdns_service_conf ),
	m_service_id( 0U )
{
	if ( m_total_services == -1 )
	{
		mdns_resp_init();

		// Enable IGMP for mDNS
		netif_list->flags |= NETIF_FLAG_IGMP;

		mdns_resp_add_netif( netif_list, MDNS_DOMAIN_NAME, MDNS_TTL );
	}

	m_total_services++;

	/* Stop here if the total number of configured services exceed the MDNS_MAX_SERVICES limit */
	BF_ASSERT( m_total_services < MDNS_MAX_SERVICES );

	m_service_id = m_total_services;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t MDNS_Client::Start_Service()
{
	bool_t status = true;

	if ( ERR_OK != mdns_search_service( m_service_conf->name, m_service_conf->type,
										m_service_conf->serv_proto, netif_list,
										m_service_conf->serv_cb, m_service_conf->arg, &m_service_id ) )
	{
		status = false;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t MDNS_Client::Restart_Service()
{
	bool_t status = true;

	mdns_search_stop( m_service_id );

	if ( ERR_OK != mdns_search_service( m_service_conf->name, m_service_conf->type,
										m_service_conf->serv_proto, netif_list,
										m_service_conf->serv_cb, m_service_conf->arg, &m_service_id ) )
	{
		status = false;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void MDNS_Client::Stop_Service()
{
	mdns_search_stop( m_service_id );
}
