/**
 *****************************************************************************************
 * @file      EtherNet_IP_Config.h
 * @details   This file contain macro used to enable different code sets with different
 *            functionality on ESP32 EtherNet IP Module.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef     EIP_CONFIG_H
#define     EIP_CONFIG_H



#define TCPIP_CAPABILITY_ACD_ENABLE         1
#define TCPIP_CAPABILITY_DHCP_ENABLE        1
#define TCPIP_CAPABILITY_IP_RESET_NEEDED_ENABLE     1
// #define TCPIP_CAPABILITY_HARDWARE_CONFIG_ENABLE     1

// #define EIP_ACD										// Enable the Address Conflict Detection.
#define EIP_PCCC_SUPPORT
// #define EIP_ALWAYS_RESET

/*This enables the ability to change the ethernet port behavior (ie: enable/disable and speed)*/

#define EIP_INTERFACE_CONTROL_SUPPORT
/* ori-Commented, uncommented on 10Dec14. This will enable the ability to set the admin state
   of the port.  Right now the switch does not support it.*/
#define EIP_ETHERNET_PORT_ADMIN_SUPPORT

/* Only need a string long enough to hold the IP Address
   Maximum size of the network path string*/
#define MAX_NETWORK_PATH_SIZE       20		// 256
/* Optional - not used */  /* Maximum size of the connection name string */
#define MAX_CONNECTION_NAME_SIZE    1		// 256
/* Maximum size of the tag string */
#define MAX_REQUEST_TAG_SIZE        1	// Unused	// 256
/* Maximum size of the module specific data */
#define MAX_MODULE_CONFIG_SIZE      1	// Unused	// 512
/* Maximum size of the error description string */
#define MAX_ERROR_DESC_SIZE         1	// Unused	//128 // 512

/* Maximum number of sessions that can be opened at the same time */
#define MAX_SESSIONS                4	// 10 // 128
/* Maximum number of connections that can be opened at the same time */
#define MAX_CONNECTIONS             8	// 16 // 128
/* Maximum number of outstanding requests */
#define MAX_REQUESTS                10	// 5 // 100
/* Maximum number of host adapters "IP Addresses" installed on a device */
#define MAX_HOST_ADDRESSES          1	// 16
/* Maximum number of classes the client application can register for its own processing */
#define MAX_CLIENT_APP_PROC_CLASS_NBR       20	// 16 // 64
/* Maximum number of services the client application can register for its own processing */
#define MAX_CLIENT_APP_PROC_SERVICE_NBR     20	// 16 // 64
/* Maximum number of object specific handlers the client application can register for its own processing */
#define MAX_CLIENT_APP_PROC_OBJECT_NBR      20	// 16 // 64
/* Maximum size of an assembly object in bytes */
#define MAX_ASSEMBLY_SIZE            80		// #k: New  Ori- 100 // 1502
/* Maximum assembly name length */
#define MAX_ASSEMBLY_NAME_SIZE       1		// 64
/* Maximum number of assembly instances */
#define MAX_ASSEMBLIES               20		// before-25June14=10 //#k: New  Ori- 12 // 128
/* members not supported - Maximum number of members per assembly instance */
#define MAX_MEMBERS                  1		// 16  //64
#define MAX_REQUEST_DATA_SIZE       ( MAX_ASSEMBLY_SIZE > 504 ? MAX_ASSEMBLY_SIZE : 504 )
/* Maximum data size that can be sent with a PCCC message. only used for parsing buffers*/
#define MAX_PCCC_DATA_SIZE          1		// Unused	//244
/* Number of supported connections per socket */
#define PLATFORM_MAX_CONNECTIONS_PER_SOCKET     64

#define ENETLINK_MAX_SPEED_DUPLEX_COUNT     4

// #define ENABLE_NON_CIP_MESSAGE_COUNTING

// #define EIP_NO_DYNAMIC_ASSEMBLY

#endif
