/** ACD according to RFC 5227 and
 *  The CIP Networks Library Volume 2: EtherNet/IP Adaptation of CIP
 *  Appendix F: Address Conflict Detection
 */

#include "Includes.h"

#include "OS_Tasker.h"

#include "lwip/opt.h"
#include "netif/etharp.h"
#include "LWIPService.h"
#include "DCI_Owner.h"

#include "acd.h"

#include "uC_Rand_Num.h"

#include "lwip/sys.h"
#include "lwip/ip.h"
#include "lwip/acd.h"
#include "lwip/dhcp.h"
#include "acd_Debug.h"
#include "Babelfish_Assert.h"
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

ACD_CONFLICTED_STATE m_acd_state = ACD_STATE_NOT_STARTED;
uint8_t m_ip_mode = static_cast<uint8_t>( IPADDR_USE_STATIC );
struct netif* m_p_netif = reinterpret_cast<struct netif*>( NULL );
uint8_t m_probe_index = 0U;
uint8_t m_announce_index = 0U;
struct ip4_addr m_ipaddr_to_probe = { 0U };
struct ip4_addr m_ipaddr_in_use = { 0U };
bool_t has_received_ongoing_conflict = FALSE;
OS_TICK_TD last_ongoing_conflict_tick = 0U;
uint8_t m_semiactive_probe_index = 0U;
ACD_CALLBACK m_p_acd_state_change_callback = reinterpret_cast<ACD_CALLBACK>( NULL );
ACD_CBACK_PARAM m_callback_param = reinterpret_cast<ACD_CBACK_PARAM>( NULL );
uint32_t m_timeout_cnt = 0U;

DCI_Owner* m_state_owner;
DCI_Owner* m_conflicted_state_owner;
DCI_Owner* m_conflicted_mac_owner;
DCI_Owner* m_conflicted_arp_pdu_owner;

bool_t m_acd_enabled = FALSE;
bool_t m_initial_probing_complete = FALSE;
bool_t m_request_to_switch_to_semi_active = false;

#ifdef __cplusplus
extern "C" {
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Init( acd_dcid_t const* dcids )
{
	m_state_owner = new DCI_Owner( dcids->state_owner );
	m_conflicted_state_owner = new DCI_Owner( dcids->conflicted_state_owner );
	m_conflicted_mac_owner = new DCI_Owner( dcids->conflicted_mac_owner );
	m_conflicted_arp_pdu_owner = new DCI_Owner( dcids->conflicted_arp_pdu_owner );

	DCI_Owner* enabled_owner = new DCI_Owner( dcids->acd_enabled_owner );
	enabled_owner->Check_Out( m_acd_enabled );
}

// assuming min_ms <= max_ms
// return a random number in [min_ms, max_ms]
// should be pretty uniformly distributed assuming
// 2^32 >> max_ms - min_ms
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t ACD_Get_Random_Delay_In_Msec( uint32_t min_ms, uint32_t max_ms )
{
	uint32_t range = 0U;
	uint32_t micro_random = 0U;

	BF_ASSERT( min_ms <= max_ms );

	if ( min_ms != max_ms )
	{
		range = ( max_ms - min_ms ) + 1U;
		uC_Rand_Num::Get( reinterpret_cast<uint8_t *>( &micro_random ),
			static_cast<uint16_t>( sizeof(uint32_t) ) );

		min_ms += ( micro_random % range );
	}
	else
	{
		// comment to avoid misra warning
	}
	return ( min_ms );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Start_Probing( struct ip4_addr* p_ip_probe )
{

	m_ipaddr_to_probe = *p_ip_probe;

	if ( Is_ACD_Valid() )
	{
		ACD_Switch_State( ACD_STATE_INITIAL_PROBING ); // no matter what the previous state was

		// start initial probing after random([180ms, 220ms])
		m_probe_index = 0U;
		ACD_Set_Timeout_Cnt(
			ACD_Get_Random_Delay_In_Msec( static_cast<uint32_t>( PROBE_WAIT_MIN_MSEC ),
				static_cast<uint32_t>( PROBE_WAIT_MAX_MSEC ) ) );
		ACD_DEBUG_PRINT( DBG_MSG_INFO, "ACD Probing Started" );
	}
	else
	{
		ACD_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid ACD" );
	}
}

// take action when m_timout_cnt == 1
// do nothing if m_timeout_cnt == 0, otherwise decrement m_timeout_cnt
// called from tcpip_thread every millisecond
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Timer()
{
	GINT_TDEF temp_gint;
	Push_GINT( temp_gint );

	if ( m_timeout_cnt > 0U )
	{
		m_timeout_cnt--;
		Pop_GINT( temp_gint );

		if ( m_timeout_cnt == 0U )
		{
			ACD_TimeOut( reinterpret_cast<void *>( NULL ) );
		}
		else
		{
			// comment to avoid misra warning
		}
	}
	else
	{
		Pop_GINT( temp_gint );
	}
}

// ACD_Timer() and ACD_Set_Timeout_Cnt() are thread-safe
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Set_Timeout_Cnt( uint32_t cnt )
{
	Push_TGINT();
	m_timeout_cnt = cnt;
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_TimeOut( void * arg )
{

	LWIP_UNUSED_ARG( arg );

	if ( Is_ACD_Valid() )
	{
		switch ( m_acd_state )
		{
			case ACD_STATE_INITIAL_PROBING:
				if ( m_probe_index >= static_cast<uint8_t>( PROBE_NUM ) )
				{
					// if we reach here it means that we have passed the initial probing phase with no conflicts
				  	ACD_DEBUG_PRINT( DBG_MSG_INFO, "Initial Probing Phase Passed" );
					m_announce_index = 1U;
					m_ipaddr_in_use.addr = m_ipaddr_to_probe.addr;
					etharp_acd_announce( m_p_netif, &m_ipaddr_in_use );

					ACD_Switch_State( ACD_STATE_ONGOING_DETECTION );
					has_received_ongoing_conflict = FALSE;
					// since previous actions' timings are randomized, probably fine with a fixed delay
					ACD_Set_Timeout_Cnt( static_cast<uint32_t>( ANNOUNCE_INTERVAL_SEC ) * 1000U );

					// we have passed initial probing successfully
					/* Disabling gatuitous request here. Else Announce frame is implicitly sent out in gratuitous
					   request(i.e Passed Initial Passing) */
					/* Sending additional announce request result in failure of ACD test as ACD test tool expects
					   two announce request with delta between them is 1800ms < delta < 2200ms.
					 */
					// m_p_netif->flags &= ~NETIF_FLAG_ETHARP;
					ACD_Passed_Initial_Probing();
					ACD_DEBUG_PRINT( DBG_MSG_INFO, "Initial Probing Successful" );
				}
				/*else if (m_probe_index == PROBE_NUM)
				 {
				 // we already sent PROBE_NUM probes, just wait before announcing
				 m_probe_index++;
				 // announcement delay [180ms, 220ms]
				 ACD_Set_Timeout_Cnt(
				 ACD_Get_Random_Delay_In_Msec(ANNOUNCE_WAIT_MIN_MSEC,
				 ANNOUNCE_WAIT_MAX_MSEC));
				 }*/
				else
				{ // m_probe_index < PROBE_NUM
				  // have more probe(s) to sent out
					m_probe_index++;
					etharp_acd_probe( m_p_netif, &m_ipaddr_to_probe );
					// interval in [180ms, 220ms]
					ACD_Set_Timeout_Cnt(
						ACD_Get_Random_Delay_In_Msec( static_cast<uint32_t>( PROBE_MIN_MSEC ),
							static_cast<uint32_t>( PROBE_MAX_MSEC ) ) );
				}
				break;
			case ACD_STATE_ONGOING_DETECTION:
				if ( m_announce_index < static_cast<uint8_t>( ANNOUNCE_NUM ) )
				{
					//m_p_netif->flags |= NETIF_FLAG_ETHARP;
                    m_announce_index++;
					etharp_acd_announce( m_p_netif, &m_ipaddr_in_use );
					if ( m_announce_index < static_cast<uint8_t>( ANNOUNCE_NUM ) )
					{
						ACD_Set_Timeout_Cnt( static_cast<uint32_t>( ANNOUNCE_INTERVAL_SEC ) * 1000U );
					}
					else
					{
						// delay [80 seconds, 165 seconds]
						ACD_Set_Timeout_Cnt(
							ACD_Get_Random_Delay_In_Msec(
								static_cast<uint32_t>( ONGOING_PROB_MIN_SEC ) * 1000U,
								static_cast<uint32_t>( ONGOING_PROB_MAX_SEC ) * 1000U ) );
					}
				}
				else
				{
					// announcement finished, periodic probing
					etharp_acd_probe( m_p_netif, &m_ipaddr_to_probe );
					// delay [80 seconds, 165 seconds]
					ACD_Set_Timeout_Cnt(
						ACD_Get_Random_Delay_In_Msec(
							static_cast<uint32_t>( ONGOING_PROB_MIN_SEC ) * 1000U,
							static_cast<uint32_t>( ONGOING_PROB_MAX_SEC ) * 1000U ) );
				}

				if ( ( m_announce_index >= static_cast<uint8_t>( ANNOUNCE_NUM ) ) &&
					 m_request_to_switch_to_semi_active )
				{
					ACD_Switch_State( ACD_STATE_SEMI_ACTIVE_PROBE );
					m_semiactive_probe_index = 0U;
					// TODO: revisit - do we need to randomize this too?
					// a fixed delay of 200ms
					ACD_Set_Timeout_Cnt( static_cast<uint32_t>( SEMI_ACTIVE_PROBE_DELAY_MS ) );
					m_request_to_switch_to_semi_active = false;
				}
				break;
			case ACD_STATE_SEMI_ACTIVE_PROBE:
				if ( m_semiactive_probe_index < static_cast<uint8_t>( SEMI_ACTIVE_PROBE_NUM ) )
				{
					m_semiactive_probe_index++;
					etharp_acd_probe( m_p_netif, &m_ipaddr_in_use );
					if ( m_semiactive_probe_index < static_cast<uint8_t>( SEMI_ACTIVE_PROBE_NUM ) )
					{
						// another fixed 200ms
						ACD_Set_Timeout_Cnt( static_cast<uint32_t>( SEMI_ACTIVE_PROBE_DELAY_MS ) );
					}
					else
					{
						// no conflict detected and SEMI_ACTIVE_PROBE_NUM probes sent
						// now we need to go back to ongoing detection

						// stop sending announcement even if less than ANNOUNCE_NUM is sent
						m_announce_index = static_cast<uint8_t>( ANNOUNCE_NUM );
						ACD_Switch_State( ACD_STATE_ONGOING_DETECTION );
						ACD_DEBUG_PRINT( DBG_MSG_INFO, "Switching State to Ongoing Detection" );
						// delay of [80ms, 165ms] for periodic probing
						ACD_Set_Timeout_Cnt(
							ACD_Get_Random_Delay_In_Msec(
								static_cast<uint32_t>( ONGOING_PROB_MIN_SEC ) * 1000U,
								static_cast<uint32_t>( ONGOING_PROB_MAX_SEC ) * 1000U ) );
					}
				}
				else
				{
					// should not reach here - defensive programming
					m_announce_index = static_cast<uint8_t>( ANNOUNCE_NUM );
					ACD_Switch_State( ACD_STATE_ONGOING_DETECTION );
					ACD_Set_Timeout_Cnt(
						ACD_Get_Random_Delay_In_Msec(
							static_cast<uint32_t>( ONGOING_PROB_MIN_SEC ) * 1000U,
							static_cast<uint32_t>( ONGOING_PROB_MAX_SEC ) * 1000U ) );
				}
				break;

			case ACD_STATE_NOT_STARTED:
				case ACD_STATE_CONFLICT_RETREAT:
				default:
				break;
		}
	}
	else
	{
		ACD_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid ACD" );
	}
	return;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t ACD_Check_Conflict( struct etharp_hdr* arp_pdu )
{
	struct ip4_addr sipaddr;
	struct ip4_addr dipaddr;
	struct eth_addr dhwaddr;
	bool_t directed_arp = true;
	uint8_t mac_index = 0;
	OS_TICK_TD current_tick;
	ACD_CONFLICT_STATE conflict_state = ACD_STATE_NO_CONFLICT_DETECTED;
	bool_t result;

	if ( Is_ACD_Enabled() )
	{
		if ( ACD_State() == ACD_STATE_RETREATED )
		{
			result = TRUE;
			ACD_DEBUG_PRINT( DBG_MSG_ERROR, "ACD Conflict Detected" );
		}
		else
		{
			// only deals with conflicts in these three states
			if ( !( ( m_acd_state != ACD_STATE_INITIAL_PROBING ) &&
					( m_acd_state != ACD_STATE_ONGOING_DETECTION ) &&
					( m_acd_state != ACD_STATE_SEMI_ACTIVE_PROBE ) ) )
			{
				memcpy( &dipaddr, &arp_pdu->dipaddr, sizeof ( dipaddr ) );
				memcpy( &sipaddr, &arp_pdu->sipaddr, sizeof ( sipaddr ) );
				memcpy( &dhwaddr, &arp_pdu->dhwaddr, sizeof ( dhwaddr ) );

				if ( m_acd_state == ACD_STATE_INITIAL_PROBING )
				{

					while( mac_index <ETHARP_HWADDR_LEN )
					{
						if( dhwaddr.addr[mac_index] != m_p_netif->hwaddr[mac_index] )
						{
							directed_arp = false;
							break;
						}
						mac_index++;
					}

					if( ( sipaddr.addr == 0U ) && (!directed_arp ) )
					{
						if ( static_cast<uint32_t>( ARP_REQUEST ) == htons( arp_pdu->opcode ) )
						{
							// an ARP probe has to be an ARP request
							if ( ip_addr_cmp( &dipaddr, &m_ipaddr_to_probe ) )
							{
								// now it's an ARP probe, check if it's probing the same address

								// it IS a conflict and we should retreat
								ACD_Record_LastConflictDetected( arp_pdu );

								if ( m_ip_mode == static_cast<uint8_t>( IPADDR_USE_STATIC ) )
								{
									LWIPServiceBringStaticNetifDown( m_p_netif );
								}
								else
								{
									dhcp_retreat( m_p_netif );
								}

								conflict_state = ACD_STATE_RETREATED;
								ACD_DEBUG_PRINT( DBG_MSG_ERROR, "Conflict detected and Retreating" );
							}
						}
					}
					else if ( ip_addr_cmp( &sipaddr, &m_ipaddr_to_probe ) )
					{
						ACD_Record_LastConflictDetected( arp_pdu );

						if ( m_ip_mode == static_cast<uint8_t>( IPADDR_USE_STATIC ) )
						{
							LWIPServiceBringStaticNetifDown( m_p_netif );
						}
						else
						{
							dhcp_retreat( m_p_netif );
						}
						conflict_state = ACD_STATE_RETREATED;
						ACD_DEBUG_PRINT( DBG_MSG_ERROR, "Conflict detected and Retreating" );
					}
					else
					{
						ACD_DEBUG_PRINT( DBG_MSG_INFO, "No Conflict Detected" );
					}
				}
				else
				{	// ACD_STATE_ONGOING_DETECTION or ACD_STATE_SEMI_ACTIVE_PROBE
					if ( ( sipaddr.addr != 0U ) &&
						 ( ip_addr_cmp( &sipaddr, &m_p_netif->ip_addr ) ) )
					{
						// it IS a conflict, record
						ACD_Record_LastConflictDetected( arp_pdu );

						if ( m_acd_state == ACD_STATE_SEMI_ACTIVE_PROBE )
						{
							// now we need to go back to ongoing detection

							// stop sending announcement even if less than ANNOUNCE_NUM is sent
							m_announce_index = static_cast<uint8_t>( ANNOUNCE_NUM );
							ACD_DEBUG_PRINT( DBG_MSG_INFO, "Back to Ongoing Detection" );
							ACD_Switch_State( ACD_STATE_ONGOING_DETECTION );
							// delay [80 sec, 165 sec]
							ACD_Set_Timeout_Cnt(
								ACD_Get_Random_Delay_In_Msec(
									static_cast<uint32_t>( ONGOING_PROB_MIN_SEC ) * 1000U,
									static_cast<uint32_t>( ONGOING_PROB_MAX_SEC ) * 1000U ) );

						}

						current_tick = OS_Tasker::Get_Tick_Count();

						if ( has_received_ongoing_conflict &&
							 ( ( static_cast<OS_TICK_TD>( DEFEND_INTERVAL_SEC )
								 * static_cast<OS_TICK_TD>( OS_TICK_FREQUENCY_HERTZ ) ) >=
							   ( current_tick - last_ongoing_conflict_tick ) ) )
						{
							// according to policy b of RFC 5227, we need to retreat
							if ( m_ip_mode == static_cast<uint8_t>( IPADDR_USE_STATIC ) )
							{
								LWIPServiceBringStaticNetifDown( m_p_netif );
							}
							else
							{
								dhcp_retreat( m_p_netif );
							}
							conflict_state = ACD_STATE_RETREATED;
							ACD_DEBUG_PRINT( DBG_MSG_ERROR, "Conflict detected and Retreating" );
						}
						else
						{
							// no previous conflict, or previous conflict was more than DEFEND_INTERVAL_SEC ago
							// according to policy b of RFC 5227, we need to defend by
							// sending out an announcement
							conflict_state = ACD_STATE_CONFLICT_DETECTED;
							ACD_DEBUG_PRINT( DBG_MSG_INFO,"No previous Conflict Detected" );
							etharp_acd_announce( m_p_netif, &m_ipaddr_in_use );
						}

						ACD_Record_LastConflictTick( current_tick );
					}
				}
			}

			m_state_owner->Check_In( reinterpret_cast<uint8_t*>( &conflict_state ) );

			if ( conflict_state != ACD_STATE_NO_CONFLICT_DETECTED )
			{
				if ( NULL != m_p_acd_state_change_callback )
				{
					m_p_acd_state_change_callback( m_callback_param );
				}
				else
				{
					// comment to avoid misra warning
				}
			}
			else
			{
				// comment to avoid misra warning
			}

			result = conflict_state != ACD_STATE_NO_CONFLICT_DETECTED;
		}
	}
	else
	{
		ACD_DEBUG_PRINT( DBG_MSG_INFO, "No ACD Conflict" );
		result = false;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Switch_State( ACD_CONFLICTED_STATE new_state )
{
	m_acd_state = new_state;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Record_LastConflictDetected( struct etharp_hdr* arp_pdu )
{
	// uint8_t i = 0;
	// uint8_t conflict_status;

	// only 1, 2, 3 in CIP Vol 2, ver 1.11 Table 5-3.10 will be used
	m_conflicted_state_owner->Check_In( reinterpret_cast<uint8_t*>( &m_acd_state ) );
	m_conflicted_state_owner->Load_Current_To_Init();

	// MAC byte order, according to ODVA CIP Vol 2, version 1.11
	// 5-4.3.2.3
	// The recommended display format is "XX-XX-XX-XX-XX-XX",
	// starting with the first octet.
	m_conflicted_mac_owner->Check_In( reinterpret_cast<uint8_t*>( arp_pdu->shwaddr.addr ) );
	m_conflicted_mac_owner->Load_Current_To_Init();

	m_conflicted_arp_pdu_owner->Check_In( reinterpret_cast<uint8_t*>( arp_pdu ) );
	m_conflicted_arp_pdu_owner->Load_Current_To_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Record_LastConflictTick( OS_TICK_TD tick )
	{
	has_received_ongoing_conflict = TRUE;
	last_ongoing_conflict_tick = tick;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Semi_Active_Indicated( void )
{
	if ( Is_ACD_Valid() )
	{
		if ( ACD_STATE_ONGOING_DETECTION == m_acd_state )
		{
			m_request_to_switch_to_semi_active = true;
		}
		else
		{
			// comment to avoid misra warning
		}
	}
	else
	{
		ACD_DEBUG_PRINT( DBG_MSG_ERROR, "Invalid ACD" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
// callback for STATIC/DHCP to bind to address since initial
// probing does not see conflicts
void ACD_Passed_Initial_Probing()
{
	if ( Is_ACD_Enabled() )
	{
		if (m_ip_mode == IPADDR_USE_DHCP)
		{
			dhcp_passed_initial_probing(m_p_netif);
		} 
		else 
		{
			// static IP
			LWIPServiceBringStaticNetifUp(m_p_netif);
		}
	}

	// Notify EIP task that probing is completed and ready to go for restart, on 16 Jul 2015 to fix ACD Conformance
	// error
	m_initial_probing_complete = TRUE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Is_ACD_Valid( void )
{
	bool_t status = false;
	
	if ( Is_ACD_Enabled() )
	{
		if ( ACD_State() != ACD_STATE_RETREATED )
		{
			status = true;
		}
		else
		{
		  ACD_DEBUG_PRINT( DBG_MSG_ERROR, "ACD Conflict State: ACD_STATE_RETREATED" );
		}
	}
	else
	{
		ACD_DEBUG_PRINT( DBG_MSG_ERROR, "ACD not enabled" );
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ACD_CONFLICT_STATE ACD_State( void )
{
	ACD_CONFLICT_STATE state;
	m_state_owner->Check_Out( (uint8_t*)&state );
	return ( state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Set_State_Change_Callback( ACD_CALLBACK pCallback,
			ACD_CBACK_PARAM callback_param )
{
	m_p_acd_state_change_callback = pCallback;
	m_callback_param = callback_param;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Is_Initial_Probing_Complete( void )
{
	return ( m_initial_probing_complete );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Config_ACD( uint8_t ip_mode, struct netif * p_netif )
{
	if ( ( ip_mode != IPADDR_USE_STATIC ) && ( ip_mode != IPADDR_USE_DHCP ) )
	{
		BF_ASSERT(false);
	}
	m_ip_mode = ip_mode;
	m_p_netif = p_netif;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ACD_Restart( struct ip4_addr* p_ip_probe )
{
	ACD_CONFLICT_STATE conflict_state = ACD_STATE_NO_CONFLICT_DETECTED;

	// Change the state to No conflict as we have new link-up which may not have conflict
	m_state_owner->Check_In( reinterpret_cast<uint8_t*>( &conflict_state ) );

	ACD_Start_Probing( p_ip_probe );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Is_ACD_Enabled( void )
{
	return ( m_acd_enabled );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Is_ACD_Capable( void )
{
	return ( TRUE );
}

#ifdef __cplusplus
}
#endif
