/**
 **********************************************************************************************
 * @file            acd.h Header File for interface class.
 *
 * @brief           The file contains ACD, a class required
 *                  to handle Address conflict detection.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef __ACD_H__
#define __ACD_H__

#include <string.h>
#include <stdbool.h>
#include <intrinsics.h>
#include "uC_OS_Device_Defines.h"

/* ODVA CIP Volume 2. Appendix F: Address Conflict Detection specifies:
 * (according to version 1.11)
 * PROBE_WAIT (180..220) ms (initial random delay)
 * PROBE_NUM 4 (number of probe packets)
 * PROBE_MIN 180 ms  (minimum delay until repeated probe)
 * PROBE_MAX 220 ms (maximum delay until repeated probe)
 * ANNOUNCE_WAIT (180..220) ms  (delay before announcing)
 *
 * ONGOING_PROBE_MIN 80  seconds (minimum interval for ongoing probes)
 * ONGOING_PROBE_MAX 165  seconds (maximum interval for ongoing probes)
 *
 * The SemiActiveProbe activity is a modified probing activity in which
 * only two ARP probes are sent using probe delays of 200 ms.
 *
 * According to the state diagram Figure F-1.1, the two 200ms delays
 * are fixed values without randomization
 *
 */

/* Other parameters should use what's specified in RFC 5227
 *
 * ANNOUNCE_NUM         2          (number of Announcement packets)
 * ANNOUNCE_INTERVAL    2 seconds  (time between Announcement packets)
 * MAX_CONFLICTS       10          (max conflicts before rate-limiting)
 * RATE_LIMIT_INTERVAL 60 seconds  (delay between successive attempts)
 * DEFEND_INTERVAL     10 seconds  (minimum interval between defensive
 *                                  ARPs)
 */

/**
 **********************************************************************************************
 * @brief        ACD class declaration
 * @n            This class checks the address conflict.
 **********************************************************************************************
 */
/**
 * @brief ACD timer interval
 */
#define ACD_TMR_INTERVAL    1U

/**
 * @brief Various states of address conflict detection
 */
typedef enum ACD_CONFLICTED_STATE
{
	ACD_STATE_NOT_STARTED = 0U,
	ACD_STATE_INITIAL_PROBING,
	ACD_STATE_ONGOING_DETECTION,
	ACD_STATE_SEMI_ACTIVE_PROBE,
	ACD_STATE_CONFLICT_RETREAT
} ACD_CONFLICTED_STATE;

/**
 * @brief address conflict detection states after process
 */
typedef enum ACD_CONFLICT_STATE
{
	ACD_STATE_NO_CONFLICT_DETECTED,
	ACD_STATE_CONFLICT_DETECTED,
	ACD_STATE_RETREATED
} ACD_CONFLICT_STATE;

/**
 * @brief parameter type for ACD call back
 */
typedef void* ACD_CBACK_PARAM;

/**
 * @brief ACD call back function pointer
 */
typedef void (* ACD_CALLBACK)( ACD_CBACK_PARAM );

typedef uint16_t DCI_ID_TD;

/**
 * @brief Structure to hold ACD DCIDs
 */
struct acd_dcid_t
{
	DCI_ID_TD acd_enabled_owner;
	DCI_ID_TD state_owner;
	DCI_ID_TD conflicted_state_owner;
	DCI_ID_TD conflicted_mac_owner;
	DCI_ID_TD conflicted_arp_pdu_owner;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                   Constructor to create instance of ACD
 * @param[in] dcids         Pointer to struct of type acd_dcid_t,
 *                          DCIDs shall be passed from prod specific.
 */
void ACD_Init( struct acd_dcid_t const* dcids );

/**
 * @brief                   Function to check if ACD is enabled
 * @return bool           returns true if enabled else return false
 */
bool Is_ACD_Enabled( void );

/**
 * @brief                   Function to handle previous retreat condition on link-up
 */
void ACD_Restart( struct ip4_addr* p_ip_probe );

/**
 * @brief                   Function to configure mode and network interface data structure
 * @param[in] ip_mode       IP address Mode, possible value for ip_mode are -
 * @n                       IPADDR_USE_STATIC  0
 * @n                       IPADDR_USE_DHCP    1
 * @n                       IPADDR_USE_AUTOIP  2
 * @param[in] p_netif       Pointer to Generic Data structure for network interface
 */
void Config_ACD( uint8_t ip_mode, struct netif* p_netif );

/**
 * @brief                   Function to start probing ACD
 * @param[in] p_ip_probe    pointer to IP address structure
 */
void ACD_Start_Probing( struct ip4_addr* p_ip_probe );

/**
 * @brief                   Function to start probing ACD
 * @return bool           returns true if enabled else return false
 */
// New function added on 16 Jul 2015 to fix ACD Conformance error
bool Is_Initial_Probing_Complete( void );

/**
 * @brief                   Function to check Address conflict called from Check_Conflict function
 * @param[in] p_ip_probe    pointer to etharp_hdr structure
 * @return bool           Returns true if there is conflict and further ARP processing should STOP
 * @n                       Return false if it is not conflict and further ARP should CONTINUE processing
 */
bool ACD_Check_Conflict( struct etharp_hdr* arp_pdu );

/**
 * @brief                   This Function is called when we have more than one ports,
 *                          if at least one port was already up, and the other port is just brought up
 */
void ACD_Semi_Active_Indicated( void );

/**
 * @brief                     Callback function if any specific task to be process on ACD state change
 * @param[in] pCallback       Call back function handle
 * @param[in] callback_param  Parameters of callback function
 */
void ACD_Set_State_Change_Callback( ACD_CALLBACK pCallback,
									ACD_CBACK_PARAM callback_param );

/**
 * @brief                     This function is called from tcpip_thread every millisecond.
 * @n                         It take action when ACD Counter equal to 1, does nothing if ACD counter equal to 0,
 *otherwise decrement.
 */
void ACD_Timer( void );

/**
 * @brief                   Function to check if device is ACD capable
 * @return bool           returns true if device is ACD capable else returns false
 */
bool Is_ACD_Capable( void );

/**
 * @brief                   Function to return current ACD state, read from DCID
 * @return bool           returns ACD state
 */
ACD_CONFLICT_STATE ACD_State( void );

void ACD_Record_LastConflictDetected( struct etharp_hdr* arp_pdu );

void ACD_Record_LastConflictTick( OS_TICK_TD tick );

uint32_t ACD_Get_Random_Delay_In_Msec( uint32_t min_ms, uint32_t max_ms );

void ACD_TimeOut( void* arg );

void ACD_Switch_State( ACD_CONFLICTED_STATE new_state );

void ACD_Passed_Initial_Probing();

void ACD_Set_Timeout_Cnt( uint32_t cnt );

bool Is_ACD_Valid( void );

#ifdef __cplusplus
}
#endif

// this should take the NV value of SelectACD during initialization
// and should not change until the next reset
// CIP Vol 2, ver 1.11
// The default value of SelectAcd shall be 1 indicating that ACD is enabled.
// When the value of SelectAcd is changed by a Set_Attribute service, the new value of
// SelectAcd shall not be applied until the device executes a restart.

// ACD_CONFLICTED_STATE m_acd_state;
// uint8_t m_ip_mode;
// struct netif* m_p_netif;
//
// struct ip4_addr m_ipaddr_to_probe;
// struct ip4_addr m_ipaddr_in_use;
//
// uint8_t m_probe_index;
// uint8_t m_announce_index;
// uint8_t m_semiactive_probe_index;
//
// bool has_received_ongoing_conflict;
// OS_TICK_TD last_ongoing_conflict_tick;
//
// ACD_CBACK_PARAM m_callback_param;
// ACD_CALLBACK m_p_acd_state_change_callback;
//
// DCI_Owner* m_state_owner;
// DCI_Owner* m_conflicted_state_owner;
// DCI_Owner* m_conflicted_mac_owner;
// DCI_Owner* m_conflicted_arp_pdu_owner;
////New member added on 16 Jul 2015 to fix ACD Conformance error
// bool m_initial_probing_complete;
//
// uint32_t m_timeout_cnt;
// bool m_acd_enabled;
// bool m_request_to_switch_to_semi_active;

static const uint32_t PROBE_WAIT_MIN_MSEC = 180U;				// 200 -10% according to ESE-590-002
static const uint32_t PROBE_WAIT_MAX_MSEC = 220U;				// 200 +10% according to ESE-590-002
static const uint32_t PROBE_NUM = 4U;
static const uint32_t PROBE_MIN_MSEC = 200U;			// changed from 180 according to ESE-590-002
static const uint32_t PROBE_MAX_MSEC = 200U;			// changed from 220 according to ESE-590-002

static const uint32_t ANNOUNCE_NUM = 2U;
static const uint32_t ANNOUNCE_WAIT_MIN_MSEC = 180U;			// 200 -10% according to ESE-590-002
static const uint32_t ANNOUNCE_WAIT_MAX_MSEC = 220U;			// 200 +10% according to ESE-590-002
static const uint32_t ANNOUNCE_INTERVAL_SEC = 2U;

static const uint32_t ONGOING_PROB_MIN_SEC = 80U;
static const uint32_t ONGOING_PROB_MAX_SEC = 165U;

static const uint32_t DEFEND_INTERVAL_SEC = 2U;			// changed back to 2 based on communication with Qi Zeng from
														// ODVA

static const uint32_t SEMI_ACTIVE_PROBE_DELAY_MS = 200U;
static const uint32_t SEMI_ACTIVE_PROBE_NUM = 2U;

#endif	/*__ACD_H__*/
