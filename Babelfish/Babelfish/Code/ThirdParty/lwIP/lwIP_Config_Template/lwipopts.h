//*****************************************************************************
//
// lwipopts.h - Configuration file for lwIP
//
// Copyright (c) 2008 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
//
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
//
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 3618 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************
//
// NOTE:  This file has been derived from the lwIP/src/include/lwip/opt.h
// header file.  It has been reformated to Luminary coding standards, and
// most comments have been removed.
//
// For additional details, refer to the original "opt.h" file, and lwIP
// documentation.
//
//*****************************************************************************

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "Includes.h"
#include "Ram.h"

//*****************************************************************************
//
// ---------- Platform specific locking ----------
//
//*****************************************************************************
#define SYS_LIGHTWEIGHT_PROT            1           // default is 0
#define NO_SYS                          0           // default is 0
//#define MEMCPY(dst,src,len)             memcpy(dst,src,len)
//#define SMEMCPY(dst,src,len)            memcpy(dst,src,len)

//*****************************************************************************
//
// ---------- Memory options ----------
//
//*****************************************************************************
#define MEM_LIBC_MALLOC                 1
#define MEM_ALIGNMENT                   4           // default is 1
#define MEM_SIZE                        (16 * 1024)  // default is 1600, was 16K
#define MEMP_OVERFLOW_CHECK             0	 // 0, 1, or 2
#define MEMP_SANITY_CHECK               0
//#define MEM_USE_POOLS                   0
//#define MEMP_USE_CUSTOM_POOLS           0

//*****************************************************************************
//
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
#define MEMP_NUM_PBUF                     16    // Default 16, was 16
#define MEMP_NUM_RAW_PCB                1
/* UDP listeners are DHCP and SNMP  */
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                  5    // Default 5, was 12
/* DDB: Changing the following value to 3 as there are only 2 listening TCP ports:
 * -HTTP
 * -Modbus TCP
 *  */
#define MEMP_NUM_TCP_PCB_LISTEN         2
#define MEMP_NUM_TCP_SEG                16
//#define MEMP_NUM_REASSDATA              5
//#define MEMP_NUM_ARP_QUEUE              30
//#define MEMP_NUM_IGMP_GROUP             8
#define MEMP_NUM_SYS_TIMEOUT              10
#define MEMP_NUM_MB_APP_STRUCT            6
#define MEMP_NUM_HTTPD_RECV_BUF           8
#define MEMP_HTTPD_RECV_BUF_SIZE          2048
//#define MEMP_NUM_NETBUF                 2
//#define MEMP_NUM_NETCONN                4
//#define MEMP_NUM_TCPIP_MSG_API          8
//#define MEMP_NUM_TCPIP_MSG_INPKT        8
#define PBUF_POOL_SIZE                    24    // Default 16, was 36

//*****************************************************************************
//
// ---------- ARP options ----------
//
//*****************************************************************************
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  10
#define ARP_QUEUEING                    1
#define ETHARP_TRUST_IP_MAC             1

//*****************************************************************************
//
// ---------- IP options ----------
//
//*****************************************************************************
#define IP_FORWARD                      0
#define IP_OPTIONS_ALLOWED              1
#define IP_REASSEMBLY                   1           // default is 1
#define IP_FRAG                         1           // default is 1
//#define IP_REASS_MAXAGE                 3
//#define IP_REASS_MAX_PBUFS              10
//#define IP_FRAG_USES_STATIC_BUF         1
//#define IP_FRAG_MAX_MTU                 1500
#define IP_DEFAULT_TTL                  255

//*****************************************************************************
//
// ---------- ICMP options ----------
//
//*****************************************************************************
#define LWIP_ICMP                       1
#define ICMP_TTL                       (IP_DEFAULT_TTL)

//*****************************************************************************
//
// ---------- RAW options ----------
//
//*****************************************************************************
//#define LWIP_RAW                        1
//#define RAW_TTL                        (IP_DEFAULT_TTL)

//*****************************************************************************
//
// ---------- DHCP options ----------
//
//*****************************************************************************
#define LWIP_DHCP                       1           // default is 0
//#define DHCP_DOES_ARP_CHECK             ((LWIP_DHCP) && (LWIP_ARP))

//*****************************************************************************
//
// ---------- UPNP options ----------
//
//*****************************************************************************
//#define LWIP_UPNP                       0

//*****************************************************************************
//
// ---------- PTPD options ----------
//
//*****************************************************************************
//#define LWIP_PTPD                       0

//*****************************************************************************
//
// ---------- AUTOIP options ----------
//
//*****************************************************************************
#define LWIP_AUTOIP                     0           // default is 0
#define LWIP_DHCP_AUTOIP_COOP           ((LWIP_DHCP) && (LWIP_AUTOIP))
                                                    // default is 0

//*****************************************************************************
//
// ---------- SNMP options ----------
//
//*****************************************************************************
#define LWIP_SNMP                       1
#define SNMP_CONCURRENT_REQUESTS        1
#define SNMP_TRAP_DESTINATIONS          1
#define SNMP_PRIVATE_MIB                0
#define SNMP_SAFE_REQUESTS              1

//*****************************************************************************
//
// ---------- IGMP options ----------
//
//*****************************************************************************
#define LWIP_IGMP                       1

//*****************************************************************************
//
// ---------- DNS options -----------
//
//*****************************************************************************
//#define LWIP_DNS                        1
//#define DNS_TABLE_SIZE                  4
//#define DNS_MAX_NAME_LENGTH             256
//#define DNS_MAX_SERVERS                 2
//#define DNS_DOES_NAME_CHECK             1
//#define DNS_USES_STATIC_BUF             1
//#define DNS_MSG_SIZE                    512

//*****************************************************************************
//
// ---------- UDP options ----------
//
//*****************************************************************************
#define LWIP_UDP                        1
//#define LWIP_UDPLITE                    0
#define UDP_TTL                         (IP_DEFAULT_TTL)

//*****************************************************************************
//
// ---------- TCP options ----------
//
//*****************************************************************************
#define LWIP_TCP                        1
#define TCP_TTL                         (IP_DEFAULT_TTL)
//#define TCP_WND                         4096   // default is 2048
//#define TCP_MAXRTX                      12
//#define TCP_SYNMAXRTX                   6
#define TCP_QUEUE_OOSEQ                 1
#define TCP_MSS                        536        // default is 128
//#define TCP_CALCULATE_EFF_SEND_MSS      1
#define TCP_SND_BUF                     (4 * TCP_MSS)
#define TCP_WND                         (4 * TCP_MSS)
                                                    // default is 256, was 6 *
#define TCP_SND_QUEUELEN                (4 * (TCP_SND_BUF/TCP_MSS))
//#define TCP_SNDLOWAT                    (TCP_SND_BUF/2)
//#define TCP_LISTEN_BACKLOG              0
//#define TCP_DEFAULT_LISTEN_BACKLOG      0xff

//*****************************************************************************
//
// ---------- API options ----------
//
//*****************************************************************************
//#define LWIP_EVENT_API                  0
//#define LWIP_CALLBACK_API               1

//*****************************************************************************
//
// ---------- Pbuf options ----------
//
//*****************************************************************************
#define PBUF_LINK_HLEN                  16          // default is 14
#define PBUF_POOL_BUFSIZE               256
// #define PBUF_POOL_BUFSIZE       LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)
                                                    // default is LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)
#define ETH_PAD_SIZE                    0           // default is 0

//*****************************************************************************
//
// ---------- Network Interfaces options ----------
//
//*****************************************************************************
//#define LWIP_NETIF_HOSTNAME             0
//#define LWIP_NETIF_API                  0
//#define LWIP_NETIF_STATUS_CALLBACK      0
#define LWIP_NETIF_LINK_CALLBACK          1
//#define LWIP_NETIF_HWADDRHINT           0

//*****************************************************************************
//
// ---------- LOOPIF options ----------
//
//*****************************************************************************
//#define LWIP_HAVE_LOOPIF                0
//#define LWIP_LOOPIF_MULTITHREADING      1

//*****************************************************************************
//
// ---------- Thread options ----------
//
//*****************************************************************************
#define TCPIP_THREAD_NAME              "tcpip_thread"
#define TCPIP_THREAD_STACKSIZE          800   //Changed to 800 from 400 due to error -CT10 FORWARD OPEN Request
#define TCPIP_THREAD_PRIO               OS_TASK_PRIORITY_3
//#define TCPIP_MBOX_SIZE                 0
//#define SLIPIF_THREAD_NAME             "slipif_loop"
//#define SLIPIF_THREAD_STACKSIZE         0
//#define SLIPIF_THREAD_PRIO              1
//#define PPP_THREAD_NAME                "pppMain"
//#define PPP_THREAD_STACKSIZE            0
//#define PPP_THREAD_PRIO                 1
#define DEFAULT_THREAD_NAME            "lwIP"
#define DEFAULT_THREAD_STACKSIZE        128
#define DEFAULT_THREAD_PRIO             3
//#define DEFAULT_RAW_RECVMBOX_SIZE       0
//#define DEFAULT_UDP_RECVMBOX_SIZE       0
//#define DEFAULT_TCP_RECVMBOX_SIZE       0
//#define DEFAULT_ACCEPTMBOX_SIZE         0

//*****************************************************************************
//
// ---------- Sequential layer options ----------
//
//*****************************************************************************
//#define LWIP_TCPIP_CORE_LOCKING         0
#define LWIP_NETCONN                    1           // default is 1

//*****************************************************************************
//
// ---------- Socket Options ----------
//
//*****************************************************************************
#define LWIP_SOCKET                     1           // default is 1
#define LWIP_COMPAT_SOCKETS             1  // ejo - was 0
//#define LWIP_POSIX_SOCKETS_IO_NAMES     1
//#define LWIP_TCP_KEEPALIVE              0
//#define LWIP_SO_RCVTIMEO                0
//#define LWIP_SO_RCVBUF                  0
//#define SO_REUSE                        0
#define ERRNO           1

//*****************************************************************************
//
// ---------- Statistics options ----------
//
//*****************************************************************************
#define LWIP_STATS                      1
#define LWIP_STATS_LARGE                1
#define LWIP_STATS_DISPLAY              0
#define LINK_STATS                      1
#define ETHARP_STATS                    0
#define IP_STATS                        1
//#define IPFRAG_STATS                    (IP_REASSEMBLY || IP_FRAG)
#define ICMP_STATS                      0
//#define IGMP_STATS                      (LWIP_IGMP)
#define UDP_STATS                       (LWIP_UDP)
#define TCP_STATS                       (LWIP_TCP)
#define MEM_STATS                       0
#define MEMP_STATS                      0
#define SYS_STATS                       1

#define LWIP_PROVIDE_ERRNO 1


//*****************************************************************************
//
// ---------- PPP options ----------
//
//*****************************************************************************
#define PPP_SUPPORT                     0
#define PPPOE_SUPPORT                   0
//#define PPPOS_SUPPORT                   PPP_SUPPORT

#if PPP_SUPPORT
//#define NUM_PPP                         1
//#define PAP_SUPPORT                     0
//#define CHAP_SUPPORT                    0
//#define MSCHAP_SUPPORT                  0
//#define CBCP_SUPPORT                    0
//#define CCP_SUPPORT                     0
//#define VJ_SUPPORT                      0
//#define MD5_SUPPORT                     0
//#define FSM_DEFTIMEOUT                  6
//#define FSM_DEFMAXTERMREQS              2
//#define FSM_DEFMAXCONFREQS              10
//#define FSM_DEFMAXNAKLOOPS              5
//#define UPAP_DEFTIMEOUT                 6
//#define UPAP_DEFREQTIME                 30
//#define CHAP_DEFTIMEOUT                 6
//#define CHAP_DEFTRANSMITS               10
//#define LCP_ECHOINTERVAL                0
//#define LCP_MAXECHOFAILS                3
//#define PPP_MAXIDLEFLAG                 100

//#define PPP_MAXMTU                      1500
//#define PPP_DEFMRU                      296
#endif

//*****************************************************************************
//
// ---------- checksum options ----------
//
//*****************************************************************************
//#define CHECKSUM_GEN_IP                 1
//#define CHECKSUM_GEN_UDP                1
//#define CHECKSUM_GEN_TCP                1
//#define CHECKSUM_CHECK_IP               1
//#define CHECKSUM_CHECK_UDP              1
//#define CHECKSUM_CHECK_TCP              1
#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
#endif




//*****************************************************************************
//
// ---------- Debugging options ----------
//
//*****************************************************************************
#define U8_F "c"
#define S8_F "c"
#define X8_F "x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
//extern void LWIPDebug(const char *pcString, ...);
//#define LWIP_PLATFORM_DIAG(x) {LWIPDebug x;}


extern void LWIPDebug(const char *pcString, ...);
// extern void UART1printf(const char *pcString, ...);
// #define LWIP_PLATFORM_DIAG(x) {UART1printf x;}
#define LWIP_PLATFORM_DIAG(x)

#define LWIP_DEBUG						0


//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SERIOUS
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SEVERE

#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON
//#define LWIP_DBG_TYPES_ON               (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)

#define ETHARP_DEBUG                    LWIP_DBG_ON     // default is OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF     // default is OFF
#define PBUF_DEBUG                     LWIP_DBG_ON
#define API_LIB_DEBUG                  LWIP_DBG_ON
#define API_MSG_DEBUG                  LWIP_DBG_ON
#define SOCKETS_DEBUG                  LWIP_DBG_ON
#define ICMP_DEBUG                     LWIP_DBG_ON
#define IGMP_DEBUG                     LWIP_DBG_ON
#define INET_DEBUG                     LWIP_DBG_ON
#define IP_DEBUG                        LWIP_DBG_ON     // default is OFF
#define IP_REASS_DEBUG                 LWIP_DBG_ON
#define RAW_DEBUG                      LWIP_DBG_ON
#define MEM_DEBUG                      LWIP_DBG_ON
#define MEMP_DEBUG                     LWIP_DBG_ON
#define SYS_DEBUG                      LWIP_DBG_ON
#define TCP_DEBUG                      LWIP_DBG_ON
#define TCP_INPUT_DEBUG                LWIP_DBG_ON
#define TCP_FR_DEBUG                   LWIP_DBG_ON
#define TCP_RTO_DEBUG                  LWIP_DBG_ON
#define TCP_CWND_DEBUG                 LWIP_DBG_ON
#define TCP_WND_DEBUG                  LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG               LWIP_DBG_ON
#define TCP_RST_DEBUG                  LWIP_DBG_ON
#define TCP_QLEN_DEBUG                 LWIP_DBG_ON
#define UDP_DEBUG                       LWIP_DBG_ON     // default is OFF
#define TCPIP_DEBUG                    LWIP_DBG_ON
#define PPP_DEBUG                      LWIP_DBG_ON
#define SLIP_DEBUG                     LWIP_DBG_ON
#define DHCP_DEBUG                      LWIP_DBG_ON     // default is OFF
#define AUTOIP_DEBUG                   LWIP_DBG_ON
#define SNMP_MSG_DEBUG                 LWIP_DBG_ON
#define SNMP_MIB_DEBUG                 LWIP_DBG_ON
#define DNS_DEBUG                      LWIP_DBG_ON

#endif /* __LWIPOPTS_H__ */
