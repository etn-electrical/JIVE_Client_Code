/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EtIpApi.h
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** EtherNet/IP Protocol Stack API declaration. Should be included in the 
** client application project.
**
*****************************************************************************
*/
#include "EtherNet_IP_Config.h"

#ifndef ET_IP_API_H
#define ET_IP_API_H

#ifndef MAX_NETWORK_PATH_SIZE
#define MAX_NETWORK_PATH_SIZE			256			/* Maximum size of the network path string */
#endif
#ifndef MAX_CONNECTION_NAME_SIZE
#define MAX_CONNECTION_NAME_SIZE		256			/* Maximum size of the connection name string */
#endif
#ifndef MAX_REQUEST_TAG_SIZE
#define MAX_REQUEST_TAG_SIZE			256			/* Maximum size of the tag string */
#endif
#ifndef MAX_MODULE_CONFIG_SIZE
#define MAX_MODULE_CONFIG_SIZE			512			/* Maximum size of the module specific data */
#endif
#ifndef MAX_ERROR_DESC_SIZE
#define MAX_ERROR_DESC_SIZE				512			/* Maximum size of the error description string */
#endif
#ifndef MAX_SESSIONS
#define MAX_SESSIONS					128			/* Maximum number of sessions that can be opened at the same time */
#endif
#ifndef MAX_CONNECTIONS
#define MAX_CONNECTIONS					128			/* Maximum number of connections that can be opened at the same time */
#endif
#ifndef MAX_REQUESTS
#define MAX_REQUESTS					100			/* Maximum number of outstanding requests */
#endif

#ifndef MAX_HOST_ADDRESSES
#define MAX_HOST_ADDRESSES				16			/* Maximum number of host adapters "IP Addresses" installed on a device */
#endif

#ifndef MAX_CLIENT_APP_PROC_CLASS_NBR
#define MAX_CLIENT_APP_PROC_CLASS_NBR		64		/* Maximum number of classes the client application can register for its own processing */
#endif
#ifndef MAX_CLIENT_APP_PROC_SERVICE_NBR
#define MAX_CLIENT_APP_PROC_SERVICE_NBR		64		/* Maximum number of services the client application can register for its own processing */
#endif
#ifndef MAX_CLIENT_APP_PROC_OBJECT_NBR
#define MAX_CLIENT_APP_PROC_OBJECT_NBR		64		/* Maximum number of object specific handlers the client application can register for its own processing */
#endif

#ifndef MAX_ASSEMBLY_SIZE
#define MAX_ASSEMBLY_SIZE					1502	/* Maximum size of an assembly object in bytes */
#endif
#ifndef MAX_ASSEMBLY_NAME_SIZE
#define MAX_ASSEMBLY_NAME_SIZE				64		/* Maximum assembly name length */
#endif
#ifndef MAX_ASSEMBLIES
#define MAX_ASSEMBLIES						128		/* Maximum number of assembly instances */
#endif
#ifndef MAX_MEMBERS
#define MAX_MEMBERS							64		/* Maximum number of members per assembly instance */
#endif

/* Maximum data size that can be send with an expicit message, typically assembly data is the largest */
/* NOTE: UCMMs can only contain 504 bytes of data */
#define MAX_REQUEST_DATA_SIZE (MAX_ASSEMBLY_SIZE > 504 ? MAX_ASSEMBLY_SIZE : 504)
#ifndef MAX_PCCC_DATA_SIZE
#define MAX_PCCC_DATA_SIZE					244		/* Maximum data size that can be sent with a PCCC message */
#endif
#ifdef SUPPORT_CSP
#define MAX_PCCC_ADDRESS_SIZE				32		/* Maximum address data size that can be sent with a PCCC message */
#endif

#ifndef HEARTBEAT_CONN_POINT
#define HEARTBEAT_CONN_POINT				198		/* Connection point to indicate there is no data transferred, just the heartbeat (0 data length) message */
#endif
#ifndef LISTEN_ONLY_CONN_POINT
#define LISTEN_ONLY_CONN_POINT				199		/* Connection point to indicate  a Listen Only behaviour in the O->T direction. There is no data transferred in the O->T direction, just the listen only heartbeat (0 data length) message */
#endif
#ifdef EIP_CIPSAFETY
#define TIME_COORDINATION_CONN_POINT		0xfd	/* Connection point to receive Time Coordination data */
#endif

#ifndef MAX_FILE_OBJECT_INSTANCES
#define MAX_FILE_OBJECT_INSTANCES			10
#endif

#ifdef ET_IP_SCANNER

#if !defined NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES && !defined (NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES)
#if defined EIP_LARGE_MEMORY && !defined (EIP_NO_CCO)
#define NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES		10
#define NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES		10
#else
#define NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES		1
#define NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES		1
#endif
#endif

#define ASSEMBLY_SCANNER_OUTPUT_INSTANCE (HEARTBEAT_CONN_POINT-NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES)
#define ASSEMBLY_SCANNER_INPUT_INSTANCE (ASSEMBLY_SCANNER_OUTPUT_INSTANCE-NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES)

#ifndef INPUT_SCANNER_ASSEMBLY_SIZE
#define INPUT_SCANNER_ASSEMBLY_SIZE					65481
#endif
#ifndef OUTPUT_SCANNER_ASSEMBLY_SIZE
#define OUTPUT_SCANNER_ASSEMBLY_SIZE				65481
#endif
#endif

#define INVALID_SLOT_INDEX				0xff		/* Invalid slot index */
#define INVALID_CONN_POINT				0xffffffff	/* Invalid connection point */
#define INVALID_CLASS					0xffffffff	/* Class is not valid flag should be used when no class should be specified in the UCMM */
#define INVALID_SERVICE					0xff		/* Service is not valid flag should be used when no service should be specified in the UCMM */
#define INVALID_INSTANCE				0xffffffff	/* Instance is not valid flag should be used when no instance should be specified in the UCMM */
#define INVALID_ATTRIBUTE				0xffffffff	/* Attribute is not valid flag should be used when no attribute should be specified in the UCMM */
#define INVALID_MEMBER					0xffffffff	/* Member is not valid flag should be used when no member should be specified in the UCMM */
#define INVALID_OFFSET					0xffffffff	/* Data Table offset should be assigned automatically by the stack */
#define INVALID_CONNECTION_FLAG			0xffff		/* Connection flag should be built automatically by the stack */
#define INVALID_CONN_INSTANCE			0xffffffff	/* Invalid connection instance number */

#define REQUEST_INDEX_BASE				0x100		/* Request IDs will start with this number. Used to separate request IDs from the returned errors */
#define LIST_IDENTITY_INDEX_BASE		0x100		/* IDs will start with this number. Used to separate IDs from the returned errors */

/* Connection related notification messages. Second parameter in the callback function will be Connection Instance number */            
#define NM_CONNECTION_ESTABLISHED						1		/* New connection successfully established */
#define NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH		2		/* Connection establishment failed. Target is offline. */
#define NM_CONN_CONFIG_FAILED_NO_RESPONSE				3		/* Connection establishment failed. Target is online but did not respond to the connection request. */
#define NM_CONN_CONFIG_FAILED_ERROR_RESPONSE			4		/* Connection establishment failed. Target responded with an error response. Use EtIPGetConnectionErrorInfo() function to get error details. */
#define NM_CONNECTION_TIMED_OUT							5		/* Active connection timed out. iWatchdogTimeoutAction member of the connection configuration will determine the associated action */
#define NM_CONNECTION_CLOSED							6		/* Connection closed based either on the local or remote request */
#define NM_INCOMING_CONNECTION_RUN_IDLE_FLAG_CHANGED	7		/* Run Idle flag changed for the incoming Class1 connection. Param1 contains connection instance. Param2 contains the new flag (1 - Run, 0 - Idle). */
#define NM_ASSEMBLY_NEW_INSTANCE_DATA					8		/* New data received for the specified assembly instance. Param1 contains assembly instance. */
																/* Param2 contains the assembly size written.  Equal to assembly size except for varible sized I/O connections and when only sequence count changes for I/O connections. */
#define NM_ASSEMBLY_NEW_MEMBER_DATA						9		/* New data received for the specified assembly member. Param1 contains assembly instance. Param2 contains instance member. */
#define NM_CONNECTION_NEW_INPUT_SCANNER_DATA			10		/* New input data received for the scanner connection. Param1 contains connection instance. */
																/* Param2 contains the data size written.  Equal to connection data size except for varible sized connections and when only sequence count changes. */
#define NM_CONNECTION_VERIFICATION						11		/* New connection received.  Waiting for application to verify validity */
#define NM_CONNECTION_RECONFIGURED						12		/* Connection successfully reconfigured (successful NULL Forward Open) */
#define NM_INCOMING_CONNECTION_REDUNDANT_MASTER_CHANGED	13		/* Redundant connection's data now being used.  Param1 contains "master" connection instance. Param2 contains associated connection */
#define NM_INCOMING_CONNECTION_ROO_VALUE_CHANGED		14		/* Ready Ownership Output value changed for the incoming Class1 connection. Param1 contains connection instance. Param2 contains the new value. */
#define NM_INCOMING_CONNECTION_NO_REDUNDANT_MASTER		15		/* No redundant owner "master" for associated redundant connections */

/* Object Request related notification messages. Second parameter in the callback function will be the Request ID */            
#define NM_REQUEST_RESPONSE_RECEIVED					100		/* Response received on the previously sent request */
#define NM_REQUEST_FAILED_INVALID_NETWORK_PATH			101		/* Unable to connect to the path specified in the request */
#define NM_REQUEST_TIMED_OUT							102		/* Request timed out - response never received. */
#define NM_CLIENT_OBJECT_REQUEST_RECEIVED				103		/* Request received for the object registered for the client application processing */
#define NM_NEW_CLASS3_RESPONSE							104		/* Detected change in the response received on the previously established Class3 connection */
#ifdef EIP_PCCC_SUPPORT
#define NM_CLIENT_PCCC_REQUEST_RECEIVED					105		/* Request received for the PCCC object */
#endif
#define NM_NEW_LIST_IDENTITY_RESPONSE					106		/* Response received for previously send List Identity broadcast */
#define NM_ID_RESET										107		/* Identity Reset was received. Second parameter is Instance being reset. Third parameter is reset type */
#define NM_BACKPLANE_REQUEST_RECEIVED					108		/* Request received from a path targeted at the backplane */
#define NM_NETWORK_LED_CHANGE							109		/* Network LED status has changed.  First parameter is the new LED state. Second parameter is applicable IP address (0 for any/all). */
#define NM_CCO_CHANGES_COMMITTED						110		/* Connection Configuration Object changes completed. Second parameter is the current Edit Signature */


/* General error messages. Second parameter in the callback function is not used. */
#define NM_OUT_OF_MEMORY								200		/* Out of available memory. */
#define NM_UNABLE_INTIALIZE_WINSOCK						201		/* Unable to initialize Windows Sockets library. */
#define NM_UNABLE_START_THREAD							202		/* Unable to start a thread */
#define NM_ERROR_USING_WINSOCK							203		/* Received an error when using Windows Sockets library functions */
#define NM_ERROR_SETTING_SOCKET_TO_NONBLOCKING			204		/* Error encountered when setting the socket to non-blocking mode */
#define NM_ERROR_CREATING_SEMAPHORE						205		/* Error encountered when failing to create semaphore */
#define NM_SESSION_COUNT_LIMIT_REACHED					206		/* Number of network peers exceeded MAX_SESSIONS constant */
#define NM_CONNECTION_COUNT_LIMIT_REACHED				207		/* Number of connections exceeded MAX_CONNECTIONS limit */
#define NM_PENDING_REQUESTS_LIMIT_REACHED				208		/* Number of outstanding object requests exceeded MAX_REQUESTS limit */
#define NM_PENDING_REQUEST_GROUPS_LIMIT_REACHED			209		/* Number of outstanding multiple service requests exceeded MAX_REQUEST_GROUPS limit */
#define NM_ERROR_UNABLE_START_MODBUS					210		/* Cannot initialize Modbus stack */

/* Following error codes may be returned to one of the function calls. They will not be used when invoking the callback function */
#define ERR_OBJECT_REQUEST_UNKNOWN_INDEX				300		/* Unknown index specified when trying to get an object response */
#define ERR_NO_AVAILABLE_OBJECT_RESPONSE				301		/* The response on the request sent has not been received yet */
#define ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED		302		/* There is no active connection available for the specified instance */
#define ERR_NO_CLAIMED_HOST_ADDRESSES					303		/* No host addresses have been claimed yet */
#define ERR_INVALID_OR_NOT_CLAIMED_HOST_ADDRESS			304		/* Invalid or not yet claimed host address specified in the function call */
#define ERR_ASSEMBLY_INVALID_INSTANCE					305		/* Invalid assembly instance specified */
#define ERR_ASSEMBLY_INSTANCE_ALREADY_EXIST				306		/* Multiple claims of the assembly instance */
#define ERR_ASSEMBLY_OBJECTS_LIMIT_REACHED				307		/* Number of configured assemblies would exceed MAX_ASSEMBLIES limit */
#define ERR_ASSEMBLY_SPECIFIED_OFFSET_INVALID			308		/* Specified offset and size combination overlap with one of the already configured assemblies */
#define ERR_ASSEMBLY_NO_BUFFER_SPACE_LEFT				309		/* No unallocated buffer space left in the assembly buffer */
#define ERR_ASSEMBLY_SIZE_EXCEED_LIMIT					310		/* Specified assembly size exceed maximum allowed size of MAX_ASSEMBLY_SIZE */
#define ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID			311		/* Specified assembly instance has not been configured */
#define ERR_ASSEMBLY_INSTANCE_USED						312		/* Specified assembly instance is being used by one of the active or pending connections */
#define ERR_ASSEMBLY_MEMBER_SIZE_EXCEED_LIMIT			313		/* Specified member size would cause the total member list size exceed assembly instance size */
#define ERR_ASSEMBLY_MEMBER_LIMIT_REACHED				314		/* Number of configured members for the corresponding assembly would exceed MAX_MEMBERS limit */
#define ERR_ASSEMBLY_MEMBER_ALREADY_EXIST				315		/* Multiple claims of the assembly instance member */
#define ERR_IDENTITY_INVALID_INSTANCE					316		/* Invalid identity instance specified */
#define ERR_CONNECTION_ALREADY_BEING_CONFIGURED			317		/* NULL Forward Open request made while existing one is still being processed */
#define ERR_CONNECTION_CONFIGURATION_INVALID			318		/* Invalid value in EtIPConnectionConfig when calling EtIPOpenConnection()/scannerOpenConnection() */
#define ERR_CLASS3_TOO_MUCH_DATA						319		/* Message data size for class3 connection is too big */
#define ERR_REQUEST_GROUP_MISMATCH						320		/* New request for a currently populated request group specifies different target IP or host IP */
#define ERR_STRINGI_BAD_SIZE							321		/* Not enough buffer size was allocated for a STRINGI <-> buffer conversion */
#define ERR_INVALID_VARIABLE_CONNECTION_SIZE			322		/* Connection size specified is too large for the connection */
#define ERR_CONNECTION_NOT_VARIABLE_TYPE				323		/* Variable sized connection operation attempted on fixed sized connection */
#define ERR_OUTSTANDING_CLASS3_REQUEST					324		/* Attempting to send an explicit message on a class3 connection that already has an outstanding request */
#define ERR_SENDING_UCMMS_NOT_SUPPORTED					325		/* EIP_NO_ORIGINATE_UCMM is set and application is trying to send a UCMM */
#define ERR_FILE_OBJECT_NOT_INITIALIZED_PROPERLY		326		/* EtIPFileInstance is not in the proper state */
#define ERR_FILE_OBJECT_COUNT_LIMIT_REACHED				327		/* Number of File Object instances attempted to exceed MAX_FILE_INSTANCES constant */
#define ERR_FILE_OBJECT_INSTANCE_ALREADY_EXISTS			328		/* File Object instance already created.  Must be removed to be created again */


/* Assembly type will be set by combining a type from EtIPAssemblyAccessType with a type from
EtIPAssemblyDirectionType and, optionally, one of the types from tagEtIPAssemblyAdditionalFlag. */
typedef enum tagEtIPAssemblyAccessType
{
	AssemblyStatic  = 1,			/* Static Assembly. Outside explicit requests not allowed to modify member list. */
	AssemblyDynamic = 2				/* Dynamic Assembly. Outside explicit requests are allowed to modify member list. */
}
EtIPAssemblyAccessType;

#define ASSEMBLY_ACCESS_TYPE_FLAG	3

typedef enum tagEtIPAssemblyDirectionType
{
	AssemblyConsuming		= 4,	/* Assembly that consumes data from the network */
	AssemblyProducing		= 8,	/* Assembly that produces data on the network */
	AssemblyConfiguration	= 16	/* Assembly that will be used to store configuration data that comes with incoming Forward Open. Should be listed as Connection Configuration Instance in the incoming Forward Open. Not used for originating connections. */
}
EtIPAssemblyDirectionType;

#define ASSEMBLY_DIRECTION_TYPE_FLAG 0x1c

typedef enum tagEtIPAssemblyAdditionalFlag
{
	AssemblyRequireCCConfigurationForIncomingConnection		= 32,	/* Require that connection be configured through CC object before accepting incoming Forward Open */
	AssemblyNotAllowCCConfigurationForOutgoingConnection	= 64	/* Connections can be initiated only through an API call, not through CC object configuration */
}
EtIPAssemblyAdditionalFlag;

#define ASSEMBLY_ADDITIONAL_FLAG  0x60

typedef struct tagEtIPAssemblyMemberConfig
{
	UINT32 iMember;						/* Unique assembly member id. May be INVALID_MEMBER for the data that should be ignored. */
	UINT16 iOffset;						/* Offset in bits from the beginning of the assembly instance data buffer. Will be automatically assigned by the stack - not the client. */
	UINT16 iSize;						/* Member size in bits */
}
EtIPAssemblyMemberConfig;

typedef struct tagEtIPAssemblyConfig
{
	INT32 nInstance;								/* Unique assembly object instance. Examples are connection connection points and connection configuration instance. */
	INT32 nOffset;									/* Offset from the beginning of the assembly buffer in bytes. May be set to EtIPAssemblyConfig to be automatically assigned by the stack. */
	INT32 nSize;									/* Assembly size in bytes. */
	INT32 nType;									/* Combination of a type from EtIPAssemblyAccessType with a type from EtIPAssemblyDirectionType and, optionally, one of the types from tagEtIPAssemblyAdditionalFlag */
	EtIPAssemblyMemberConfig Members[MAX_MEMBERS];	/* Member list */
	INT32 nNumMembers;								/* Number of members in the member list */
	INT8  szTagName[MAX_ASSEMBLY_NAME_SIZE];		/* Optional 0 terminated ASCII name associated with the assembly instance */
	UINT32 lLastUpdateTimestamp;					/* Last time assembly instance was updated (if CONNECTION_STATS is defined */
}
EtIPAssemblyConfig;

/* Tag Types */
#define TAG_TYPE_CLASS						1
#define TAG_TYPE_INSTANCE					2
#define TAG_TYPE_ATTRIBUTE					3
#define TAG_TYPE_MEMBER						4
#define TAG_TYPE_PORT						5
#define TAG_TYPE_LINK_ADDRESS				6
#define TAG_TYPE_SLOT						7
#define TAG_TYPE_CONN_POINT					8
#define TAG_TYPE_KEY						9
#define TAG_TYPE_INHIBIT_INTERVAL			10
#define TAG_TYPE_SYMBOL						11
#define TAG_TYPE_EXT_SYMBOL					12
#define TAG_TYPE_DATA						13
#define TAG_TYPE_DATA2						14

#define MAX_TAGS							16	/* Max number of different tags that can be specified in the PDU */

typedef struct tagETAG
{
	ETAG_DATATYPE TagData;			/* Can be UINT8 or UINT16 or UINT8* depending on the type */
	UINT16 iTagDataSize;			/* Tag Data size in bytes */
	UINT16 iTagType;				/* See TAG_TYPE_* macros above for the type list */
}
ETAG;

typedef struct tagEPATH
{
	ETAG Tags[MAX_TAGS];
	UINT8 bNumTags;					/* Actual number of tags in EPATH */
	BOOL bPadded;					/* Either Padded or Packed format for EPATH */
}
EPATH;

typedef struct tagSTRINGI
{
	UINT8 aLang[3];								/* Common Language Codes from ISO 639-2/T */
	UINT8 bEPATH;								/* EPATH (limited to the values 0xD0, 0xD5, 0xD9, and0xDA) */
	UINT16 iCharacterSet;						/* Character set of string */
	UINT8 bLength;								/* Length of string */
	UINT8 stringData[MAX_REQUEST_TAG_SIZE];		/* String data */
} STRINGI;

typedef struct tagEtIPObjectRequest					/* Structure used to transfer UCMM message */
{
	INT8 networkPath[MAX_NETWORK_PATH_SIZE];		/* Null terminated peer network path string. If we are originating request this can be a complex routing path. If we are the target this is the IP address of the peer this request came from. */
	UINT8 bService;									/* Service to be performed */
	UINT32 iClass;									/* Target class */
	UINT32 iInstance;								/* Target instance. Can be 0 if the target is class */
	UINT32 iAttribute;								/* Target attribute. Must be INVALID_ATTRIBUTE if the attribute should not be a part of the path */
	UINT32 iMember;									/* Target member. Must be INVALID_MEMBER if the member should not be a part of the path */
	INT8 requestTag[MAX_REQUEST_TAG_SIZE];			/* Target extended symbol segment. If not used iTagSize must be 0. */
	UINT16 iTagSize;								/* The actual size of the requestTag */
	UINT8 requestData[MAX_REQUEST_DATA_SIZE];		/* Data that should be sent to the target. If not used iDataSize must be 0. */
	UINT16 iDataSize;								/* The actual size of the requestData */
	UINT32 lExplicitMessageTimeout;					/* Message Timeout in milliseconds. If 0, DEFAULT_EXPLICIT_REQUEST_TIMEOUT will be used instead. */
	UINT32 lHostIPAddr;								/* Host IPAddress in UINT32 format for the opened session */
}
EtIPObjectRequest;

typedef struct tagEtIPObjectRegisterType
{
	UINT8 bService;									/* Service to be performed */
	UINT32 iClass;									/* Target class */
	UINT32 iInstance;								/* Target instance. Can be 0 if the target is class */
	UINT32 iAttribute;								/* Target attribute. Must be INVALID_ATTRIBUTE if the attribute should not be a part of the path or 0 for any attribute in path */
	UINT32 iMember;									/* Target member. Must be INVALID_MEMBER if the member should not be a part of the path */
}
EtIPObjectRegisterType;

typedef struct tagEtIPEPATHObjectRequest			/* Structure used to transfer UCMM message using the ready to use EPATH */
{
	INT8 networkPath[MAX_NETWORK_PATH_SIZE];		/* Null terminated peer network path string. If we are originating request this can be a complex routing path. If we are the target this is the IP address of the peer this request came from. */
	UINT8 bService;									/* Service to be performed */
	EPATH ePATH;									/* Ready to use EPATH for cases more complex that the ones that can be serviced by EtIPObjectRequest */
	UINT8 requestData[MAX_REQUEST_DATA_SIZE];		/* Data that should be sent to the target. If not used iDataSize must be 0. */
	UINT16 iDataSize;								/* The actual size of the requestData */
	UINT32 lExplicitMessageTimeout;					/* Message Timeout in milliseconds. If 0, DEFAULT_EXPLICIT_REQUEST_TIMEOUT will be used instead. */
	UINT32 lHostIPAddr;								/* Host IPAddress in UINT32 format for the opened session */
}
EtIPEPATHObjectRequest;


typedef struct tagEtIPObjectResponse				/* Structure used to get the response to a UCMM or an Unconnected Send message */
{
	UINT8 bGeneralStatus;							/* 0 in case of successful response, or one of the CIP general codes. */
	UINT16 iExtendedStatus;							/* Together with General Status is used to provide more error information. */
	INT8 errorDescription[MAX_ERROR_DESC_SIZE];		/* Null terminated error description string. It is filled only if bGeneralStatus is other than 0. */
	UINT8 responseData[MAX_REQUEST_DATA_SIZE];		/* Response Data in case of a success. */
	UINT16 iDataSize;								/* The size of the responseData in bytes */
	UINT16 extendedStatusData[MAX_REQUEST_DATA_SIZE/2];	/* Extended Status Data in case of an error response. */
	UINT16 iExtendedStatusDataSize;					/* The size of the extendedStatusData in 16-bit words */
}
EtIPObjectResponse;

typedef struct tagEtIPPCCCRequest
{
	UINT32 lNetworkAddr;						/* Peer network IPAddress in UINT32 format. If we are the target this is the IP address of the peer this request came from. */
	UINT8 bCommand;								/* PCCC Command */
	UINT8 bExtCommand;							/* PCCC Extended Command (for commands 6, 7, 14 and 15) */
	UINT8 requestData[MAX_PCCC_DATA_SIZE];		/* Data that should be sent to the target. If not used iDataSize must be 0. */
	UINT16 iDataSize;							/* The actual size of the requestData */

	UINT8 bHasRequestorIDData;					/* Used to determine if iVendorID, lSerialNumber, and requesterIDData are valid data */
	UINT16 iVendorId;							/* Used with PCCC Object Execute PCCC service */
	UINT32 lSerialNumber;						/* Used with PCCC Object Execute PCCC service */
	UINT8 requesterIDData[MAX_PCCC_DATA_SIZE];	/* Used with PCCC Object Execute PCCC service */
	UINT8 iRequesterIDDataSize;					/* Used with PCCC Object Execute PCCC service */

#ifdef SUPPORT_CSP
	UINT8 addressData[MAX_PCCC_ADDRESS_SIZE];	/* Address data that should be sent to the target. If not used iAddressDataSize must be 0. */
	UINT16 iAddressDataSize;					/* The actual size of the addressData */
#endif

	UINT32 lExplicitMessageTimeout;				/* Message Timeout in milliseconds. If 0, DEFAULT_EXPLICIT_REQUEST_TIMEOUT will be used instead. */
	UINT32 lHostIPAddr;							/* Host IPAddress in UINT32 format for the opened session */

} EtIPPCCCRequest;

typedef struct tagEtIPPCCCResponse
{
	UINT8 bError;								/* PCCC Error */
	UINT8 bExtStatus;							/* Extended status for certain PCCC Commands */
	UINT8 responseData[MAX_PCCC_DATA_SIZE];		/* Response Data in case of a success. */
	UINT16 iDataSize;							/* The size of the responseData in bytes */
#ifdef SUPPORT_CSP
	UINT8 addressData[MAX_PCCC_ADDRESS_SIZE];	/* Address data that should be sent to the target. If not used iAddressDataSize must be 0. */
	UINT16 iAddressDataSize;					/* The actual size of the addressData */
#endif

} EtIPPCCCResponse;

typedef enum tagEtIPTransportClass				/* Class 0, 1 and 3 transports supported */
{
	Class0 = 0,
	Class1 = 1,
	Class3 = 3
}
EtIPConnectionClass;

typedef enum tagEtIPTransportType
{
	Cyclic					= 0,				/* Produce and consume cyclicly on the connection */
	ChangeOfState			= 0x10,				/* Produce and consume on the state change as well as cyclicly to keep the connection open */
	ApplicationTriggered	= 0x20				/* Treat the same as ChangeOfState */
}
EtIPTransportType;

typedef enum tagEtIPConnectionType
{
	PointToPoint			= 0x4000,			/* Should be always used for Originator to Target transports */
	Multicast				= 0x2000,			/* Could be used for Target to Originator transports */
	NullConnType			= 0					/* Null connection type */
}
EtIPConnectionType;

typedef enum tagEtIPConnectionPriority
{
	LowPriority				= 0x0,
	HighPriority			= 0x400,
	ScheduledPriority		= 0x800,
	UrgentPriority			= 0xC00
}
EtIPConnectionPriority;

typedef enum tagEtIPConnectionRunIdleFlag
{
	IdleFlag				= 0x0,
	RunFlag					= 0x1,
	UnknownRunIdleFlag		= 0x2
}
EtIPConnectionRunIdleFlag;

typedef enum tagEtIPConnectionState
{
	ConnectionNonExistent		= 0x0,	/* Connection is not instantiated */
	ConnectionConfiguring		= 0x1,	/* In the process of opening a new connection */
	ConnectionWaitingForID		= 0x2,	/* This status will never be assigned since Connection object services are not supported */
	ConnectionEstablished		= 0x3,	/* Connection is active */
	ConnectionTimedOut			= 0x4,	/* Connection has timed out; will stay in this state at least for some time if iWatchdogTimeoutAction is set to TimeoutManualReset or TimeoutDelayAutoReset.*/
	ConnectionDeferredDelete	= 0x5,	/* Connection is about to be deleted and waiting for child connections to be closed first. */
	ConnectionClosing			= 0x6	/* In the process of closing the connection */
}
EtIPConnectionState;

typedef enum tagEtIPWatchdogTimeoutAction
{
	TimeoutAutoDelete			= 0x0,	/* Immediately Delete connection on watchdog timeout */
	TimeoutDeferredDelete		= 0x1,	/* Delete connection after child connections are closed. */
	TimeoutAutoReset			= 0x2,	/* Immediately attempt to reconnect on watchdog timeout */
	TimeoutDelayAutoReset		= 0x3,	/* Attempt to reconnect after predetermined delay period */
	TimeoutManualReset			= 0x4	/* Wait in time out state until the user calls EtIPResetConnection */
}
EtIPWatchdogTimeoutAction;

typedef enum tagEtIPNetworkStatus
{
	NetworkStatusOff				= 0x0,		/* No power or no IP (claimed) address */
	NetworkStatusFlashingGreen		= 0x1,		/* No connections */
	NetworkStatusGreen				= 0x2,		/* At least 1 connection */
	NetworkStatusFlashingRed		= 0x3,		/* One or more connections have timed out */
	NetworkStatusRed				= 0x4,		/* Duplicate IP address */
	NetworkStatusFlashingGreenRed	= 0x5		/* Selftest */
}
EtIPNetworkStatus;

typedef struct tagEtIPDeviceID					/* Could be used for device keying */
{
	UINT16 iVendorID;
	UINT16 iProductType;
	UINT16 iProductCode;
	UINT8  bMajorRevision;
	UINT8  bMinorRevision;
}
EtIPDeviceID;

typedef struct tagEtIPErrorInfo
{
	UINT8 bGeneralStatus;								/* Together with Extended Status is used to provide more error information. 0 if connection is running fine, 0xD0 if connecting or disconnecting. */
	UINT16 iExtendedStatus;								/* Together with General Status is used to provide more error information. 2 if connecting, 3 if disconnecting, or any other extended error code. */
	INT8 errorDescription[MAX_ERROR_DESC_SIZE];			/* Null terminated error description string */
	UINT16 extendedStatusData[MAX_REQUEST_DATA_SIZE/2];	/* Extended Status Data if included in an error response. */
	UINT16 iExtendedStatusDataSize;						/* The size of the extendedStatusData in 16-bit words */
}
EtIPErrorInfo;

typedef struct tagEtIPConnectionConfig
{
	INT32 nInstance;									/* Instance of the Connection object. If 0, Instance will be assigned automatically by the stack and available on the function call return. */
	BOOL bOriginator;									/* Indicates whether whether this is outgoing or incoming connection */
	INT8 networkPath[MAX_NETWORK_PATH_SIZE];			/* Null terminated connection path string. If originator, it can be the complex routing path. If target, it is the IP address of the peer the connection request came from. */
	UINT16 iConnectionFlag;								/* Should be set to INVALID_CONN_POINT when calling EtIPOpenConnection. EtIPGetConnectionFlag() returns valid flag compatible with RSNetworx format */
	UINT32 iTargetConfigConnInstance;					/* Target Assembly configuration instance. */
	UINT32 iTargetProducingConnPoint;					/* Target Assembly Producing instance. */
	UINT32 iTargetConsumingConnPoint;					/* Target Assembly Consuming instance. */
	INT8 targetProducingConnTag[MAX_REQUEST_TAG_SIZE];	/* Optional target producing connection tag. Could be used instead of specifying the iTargetProducingConnPoint. */
	UINT16 iTargetProducingConnTagSize;					/* Connection target producing connection tag size. Must be set to 0 if target producing connection tag is not used */
	INT8 targetConsumingConnTag[MAX_REQUEST_TAG_SIZE];	/* Optional target consuming connection tag. Could be used instead of specifying the iTargetConsumingConnPoint. */
	UINT16 iTargetConsumingConnTagSize;					/* Connection target consuming connection tag size. Must be set to 0 if target consuming connection tag is not used */
	UINT16 iCurrentProducingSize;						/* Current size of producing data for variable sized class1 connections. Maximum size of producing data for class3 connection */
	UINT16 iCurrentConsumingSize;						/* Current size of consuming data for variable sized class1 connections. Maximum size of consuming data for class3 connection */
	UINT32 lProducingDataRate;							/* Producing by this stack I/O data rate in milliseconds */
	UINT32 lConsumingDataRate;							/* Consuming by this stack I/O data rate in milliseconds */
	UINT8 bProductionOTInhibitInterval;					/* The scanner (originator) will not produce with the higher rate than inhibit interval. Value of 0 disables it. */
	UINT8 bProductionTOInhibitInterval;					/* The adapter (target) will not produce with the higher rate than inhibit interval. Value of 0 disables it. */
	BOOL bOutputRunProgramHeader;						/* Set to TRUE if the first 4 bytes of the output data represent Run/Program header */
	BOOL bInputRunProgramHeader;						/* Set to TRUE if the first 4 bytes of the input data represent Run/Program header */
	UINT16 iProducingConnectionType;					/* Must be one of the EtIPConnectionType values */
	UINT16 iConsumingConnectionType;					/* Must be one of the EtIPConnectionType values */
	UINT16 iProducingPriority;							/* Must be one of the EtIPConnectionPriority values */
	UINT16 iConsumingPriority;							/* Must be one of the EtIPConnectionPriority values */
	UINT8 bTransportClass;								/* Indicates whether transport is Class 1 or Class 3. Must be one of the EtIPConnectionClass values. */
	UINT8 bTransportType;								/* One of the EtIPTransportType enumeration values */
	UINT8 bTimeoutMultiplier;							/* Timeout multiplier of 0 indicates that timeout should be 4 times larger than the consuming data rate, 1 - 8 times larger, 2 - 16 times, and so on */
	UINT16 iWatchdogTimeoutAction;						/* Watchdog Timeout Action from the EtIPWatchdogTimeoutAction enumeration */
	UINT32 lWatchdogTimeoutReconnectDelay;				/* If Watchdog Timeout Action is TimeoutDelayAutoReset, Watchdog Timeout Reconnect Delay specifies reconnect delay in milliseconds */
	EtIPDeviceID deviceId;								/* Optional target device identification. All members must be set to 0 to turn off identification. If set, it will be specified in the Forward Open Connection Path. */
	INT8 connectionName[MAX_CONNECTION_NAME_SIZE];		/* Optional user recognizable connection name. */
	UINT16 iConnectionNameSize;							/* Connection name size. Must be set to 0 if connection name is not used */
	UINT8 moduleConfig1[MAX_MODULE_CONFIG_SIZE];		/* Optional first part of the target device specific configuration */
	UINT16 iModuleConfig1Size;							/* The size of the first part of the target device specific configuration. Must be set to 0 if configuration is not passed to the target device */
	UINT8 moduleConfig2[MAX_MODULE_CONFIG_SIZE];		/* Optional second part of the target device specific configuration */
	UINT16 iModuleConfig2Size;							/* The size of the second part of the target device specific configuration. Must be set to 0 if configuration is not passed to the target device */
	UINT32 lHostIPAddr;									/* Host IPAddress in UINT32 format for the opened session */
	BOOL bRedundantOwner;								/* Set to TRUE if originating a redundant owner class 1 connection */
	BOOL bClass1VariableProducer;						/* Set to TRUE if I/O connection produced size is variable */
	BOOL bClass1VariableConsumer;						/* Set to TRUE if I/O connection consumed size is variable */
	UINT8 bReadyOwnershipOutputs;						/* Ready for Ownership of Outputs value for a class1 redundant connection */
	UINT8 bClaimOutputOwnership;						/* Claim Output Ownership value for a class1 redundant connection */

														/* Priority tick time and timeout values to be used by fwd_open */
	UINT8 bOpenPriorityTickTime;						/* Priority bit is bit 4. Low nibble is the number of milliseconds per tick */
	UINT8 bOpenTimeoutTicks;							/* Number of ticks to timeout */

#ifdef ET_IP_SCANNER
	INT32 nInputScannerOffset;							/* Offset in the Input Scanner assembly where the data begins for this connection */
	INT32 nInputScannerSize;							/* Size allocated for this connection in the Input Scanner assembly */
	INT32 nOutputScannerOffset;							/* Offset in the Output Scanner assembly where the data begins for this connection */
	INT32 nOutputScannerSize;							/* Size allocated for this connection in the Output Scanner assembly */
#ifdef EIP_LARGE_MEMORY
	INT32 nInputScannerTable;							/* Determines which Input Scanner assembly instance the data will go */
	INT32 nOutputScannerTable;							/* Determines which Output Scanner assembly instance the data will go */
#endif

#endif

/* #ifdef ET_IP_MODBUS */
	UINT16 iModbusConsumeSize;							/* Size in bytes of the Modbus data being consumed for routed connection */
	UINT16 iModbusProduceSize;							/* Size in bytes of the Modbus data being produced for routed connection */
	INT8 modbusNetworkPath[MAX_NETWORK_PATH_SIZE];		/* Null terminated connection path string for routed Modbus connection */
/* #endif */
}
EtIPConnectionConfig;

typedef struct tagEtIPEPATHConnectionConfig
{
	UINT32 lTargetIP;									/* Target IP address */
	EPATH ePATH;										/* Ready to use EPATH as a different representation than EtIPConnectionConfig */
	INT32 nInstance;									/* Instance of the Connection object. If 0, Instance will be assigned automatically by the stack and available on the function call return. */
	BOOL bOriginator;									/* Indicates whether whether this is outgoing or incoming connection */
	UINT16 iConnectionFlag;								/* Should be set to INVALID_CONN_POINT when calling EtIPOpenConnection. EtIPGetConnectionFlag() returns valid flag compatible with RSNetworx format */
	UINT16 iCurrentProducingSize;						/* Current size of producing data for variable sized class1 connections. Maximum size of producing data for class3 connection */
	UINT16 iCurrentConsumingSize;						/* Current size of consuming data for variable sized class1 connections. Maximum size of consuming data for class3 connection */
	UINT32 lProducingDataRate;							/* Producing by this stack I/O data rate in milliseconds */
	UINT32 lConsumingDataRate;							/* Consuming by this stack I/O data rate in milliseconds */
	BOOL bOutputRunProgramHeader;						/* Set to TRUE if the first 4 bytes of the output data represent Run/Program header */
	BOOL bInputRunProgramHeader;						/* Set to TRUE if the first 4 bytes of the input data represent Run/Program header */
	UINT16 iProducingConnectionType;					/* Must be one of the EtIPConnectionType values */
	UINT16 iConsumingConnectionType;					/* Must be one of the EtIPConnectionType values */
	UINT16 iProducingPriority;							/* Must be one of the EtIPConnectionPriority values */
	UINT16 iConsumingPriority;							/* Must be one of the EtIPConnectionPriority values */
	UINT8 bTransportClass;								/* Indicates whether transport is Class 1 or Class 3. Must be one of the EtIPConnectionClass values. */
	UINT8 bTransportType;								/* One of the EtIPTransportType enumeration values */
	UINT8 bTimeoutMultiplier;							/* Timeout multiplier of 0 indicates that timeout should be 4 times larger than the consuming data rate, 1 - 8 times larger, 2 - 16 times, and so on */
	UINT16 iWatchdogTimeoutAction;						/* Watchdog Timeout Action from the EtIPWatchdogTimeoutAction enumeration */
	UINT32 lWatchdogTimeoutReconnectDelay;				/* If Watchdog Timeout Action is TimeoutDelayAutoReset, Watchdog Timeout Reconnect Delay specifies reconnect delay in milliseconds */
	INT8 connectionName[MAX_CONNECTION_NAME_SIZE];		/* Optional user recognizable connection name. */
	UINT16 iConnectionNameSize;							/* Connection name size. Must be set to 0 if connection name is not used */
	UINT32 lHostIPAddr;									/* Host IPAddress in UINT32 format for the opened session */
	BOOL bRedundantOwner;								/* Set to TRUE if originating a redundant owner class 1 connection */
	BOOL bClass1VariableProducer;						/* Set to TRUE if I/O connection produced size is variable */
	BOOL bClass1VariableConsumer;						/* Set to TRUE if I/O connection consumed size is variable */
	UINT8 bReadyOwnershipOutputs;						/* Ready for Ownership of Outputs value for a class1 redundant connection */
	UINT8 bClaimOutputOwnership;						/* Claim Output Ownership value for a class1 redundant connection */

														/* Priority tick time and timeout values to be used by fwd_open */
	UINT8 bOpenPriorityTickTime;						/* Priority bit is bit 4. Low nibble is the number of milliseconds per tick */
	UINT8 bOpenTimeoutTicks;							/* Number of ticks to timeout */

#ifdef ET_IP_SCANNER
	INT32 nInputScannerOffset;							/* Offset in the Input Scanner assembly where the data begins for this connection */
	INT32 nInputScannerSize;							/* Size allocated for this connection in the Input Scanner assembly */
	INT32 nOutputScannerOffset;							/* Offset in the Output Scanner assembly where the data begins for this connection */
	INT32 nOutputScannerSize;							/* Size allocated for this connection in the Output Scanner assembly */

#ifdef EIP_LARGE_MEMORY
	INT32 nInputScannerTable;							/* Determines which Input Scanner assembly instance the data will go */
	INT32 nOutputScannerTable;							/* Determines which Output Scanner assembly instance the data will go */
#endif
#endif

/* #ifdef ET_IP_MODBUS */
	UINT16 iModbusConsumeSize;							/* Size in bytes of the Modbus data being consumed for routed connection */
	UINT16 iModbusProduceSize;							/* Size in bytes of the Modbus data being produced for routed connection */
	INT8 modbusNetworkPath[MAX_NETWORK_PATH_SIZE];		/* Null terminated connection path string for routed Modbus connection */
/* #endif */
}
EtIPEPATHConnectionConfig;

typedef struct tagEtIPInternalConnectionConfig
{
	UINT32 lProducingCID;				/* Producing Connection Id identifies data produced on this connection */
	UINT32 lConsumingCID;				/* Consuming Connection Id identifies data consumed on this connection */
	UINT32 lProducingIP;				/* IPAddress in UINT32 format connection is producing data to */
	UINT32 lConsumingIP;				/* IPAddress in UINT32 format connection is consuming data from */
	UINT16 lProducingPort;				/* TCP/UDP port connection is producing data to */
	UINT16 lConsumingPort;				/* TCP/UDP port connection is consuming data from */
	SOCKET lSocket;						/* Socket for producing/consuming data */
	UINT32 lProducingConnectionParams;	/* Raw producing connection parameters used in Forward Open */
	UINT32 lConsumingConnectionParams;	/* Raw consuming connection parameters used in Forward Open */
	UINT16 iConnectionSerialNbr;		/* Unique number identifying this connection */
	UINT16 iOrigVendorID;				/* Vendor id of connection originator */
	UINT32 lOrigDeviceSerialNbr;		/* Serial number of connection originator */

} EtIPInternalConnectionConfig;

typedef struct tagEtIPClass3Request				/* Structure used to set a Class3 request */
{
	UINT8 bService;								/* Service to be performed */
	UINT32 iClass;								/* Target class */
	UINT32 iInstance;							/* Target instance. Can be 0 if the target is class */
	UINT32 iAttribute;							/* Target attribute. Must be INVALID_ATTRIBUTE if the attribute should not be a part of the path */
	UINT32 iMember;								/* Target member. Must be INVALID_MEMBER if the member should not be a part of the path */
	INT8 requestTag[MAX_REQUEST_TAG_SIZE];		/* Target extended symbol segment. If not used iTagSize must be 0. */
	UINT16 iTagSize;							/* The actual size of the requestTag */
	UINT8 requestData[MAX_REQUEST_DATA_SIZE];	/* Data that should be sent to the target. If not used iDataSize must be 0. */
	UINT16 iDataSize;							/* The actual size of the requestData */
}
EtIPClass3Request;

typedef struct tagEtIPEPATHClass3Request		/* Structure used to set a Class3 request using the ready to use EPATH */
{
	UINT8 bService;								/* Service to be performed */
	EPATH ePATH;								/* Ready to use EPATH for cases more complex that the ones that can be serviced by EtIPObjectRequest */
	UINT8 requestData[MAX_REQUEST_DATA_SIZE];	/* Data that should be sent to the target. If not used iDataSize must be 0. */
	UINT16 iDataSize;							/* The actual size of the requestData */
}
EtIPEPATHClass3Request;

#define MAX_PRODUCT_NAME_SIZE				33		/* Maximum number of 32 characters and the ending 0 */

typedef struct tagEtIPIdentityInfo
{
	UINT16 iVendor;
	UINT16 iProductType;
	UINT16 iProductCode;
	UINT8 bMajorRevision;
	UINT8 bMinorRevision;
	UINT16 iStatus;
	UINT32 lSerialNumber;
	INT8 productName[MAX_PRODUCT_NAME_SIZE];
	UINT8 bUseApplicationStatus;
	STRINGI szUserName;
	STRINGI szUserDescription;
	STRINGI szUserLocation;
}
EtIPIdentityInfo;

#define MAX_IDENTITY_INSTANCE	1

/* Callback function to be called in case of any notification or error event */
typedef void ET_IP_CALLBACK LogEventCallbackType( INT32 nEvent, INT32 nParam1, INT32 nParam2 );
/* Callback function to be called if logging messages to the client app */
typedef void ET_IP_CALLBACK LogMessageCallbackType(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, void *pData, UINT16 iLen );


typedef struct tagEtIPConnectionStats					/* Structure used to transfer connection statistics */
{
	UINT32 lSendPackets;						/* Number of sent packets */
	UINT32 lRcvdPackets;						/* Number of received packets */
	float fSendAPI;								/* The actual packet interval for the sent packets */
	float fRcvdAPI;								/* The actual packet interval for the received packets */
	UINT32 lMaxSendDelay;						/* The maximum delay between any two consecutive send packets */
	UINT32 lMaxRcvdDelay;						/* The maximum delay between any two consecutive receive packets */
	UINT32 lLostPackets;						/* The number of lost packets */
	UINT16 iSendPacketError;					/* Number of I/O packets that failed to send */
	UINT16 iRcvdPacketError;					/* Number of received I/O packets that resulted in error */
	UINT16 iNumTimeouts;						/* Number of times connection has timed out.  More applicable to originated connections that retry */
	UINT16 iNumErrors;							/* Number of times connection has errored.  More applicable to originated connections that retry */
	UINT32 lLastRcvdTimestamp;					/* Timestamp of last received packet */
	UINT32 lLastSendTimestamp;					/* Timestamp of last sent packet */
	UINT32 lSendBytes;							/* Number of sent bytes */
	UINT32 lRcvdBytes;							/* Number of received bytes */
}
EtIPConnectionStats;

typedef struct tagEtIPSystemStats
{
	UINT32 iMaxActiveIOConnections;				/* The maximum number of class1 (I/O) connections open at one time */
	UINT32 iNumCurrentIOConnections;			/* Current number of class1 (I/O) connections */
	UINT32 iMaxActiveExplicitConnections;		/* The maximum number of class3 (explicit) connections open at one time */
	UINT32 iNumCurrentExplicitConnections;		/* Current number of class3 (explicit) connections */
	UINT16 iNumFailedConnections;				/* Total number of target connctions that responded with an error + number of initiated connections that responded with an error */
	UINT16 iNumTimedOutConnections;				/* Total number of connctions that have timed out */
	UINT16 iNumTCPConnectionsUsed;				/* Total number of "sessions" established */
	UINT16 iMaxTCPConnectionsUsed;				/* The maximum number of "sessions" established at one time */
	UINT16 iNumCurrentTCPConnections;			/* Current number of "sessions" established */
	UINT32 iNumSuccessfulSentIOMessages;		/* Total number of successfully send I/O messages */
	UINT32 iNumSuccessfulRecvIOMessages;		/* Total number of successfully received I/O messages */
	UINT16 iNumFailedSentIOMessages;			/* Total number of I/O packets that failed to send */
	UINT16 iNumFailedRecvIOMessages;			/* Total number of received I/O packets that resulted in error */
	UINT32 iNumSentClass3Messages;				/* Total number of sent class3 messages */
	UINT32 iNumRecvClass3Messages;				/* Total number of received class3 messages */
	UINT32 iNumSentUCMMMessages;				/* Total number of sent UCMM messages */
	UINT32 iNumRecvUCMMMessages;				/* Total number of received UCMM messages */

} EtIPSystemStats;

typedef struct tagEtIPListIdentityInfo
{
	INT32 lID;										/* Identitier for a device's response */
	UINT32 lIPAddress;								/* IP address of the device */
	UINT32 lHostAddress;							/* Host IP address that sent the ListIdentity broadcast */
	EtIPDeviceID deviceId;							/* Device information */
	UINT16 iStatus;									/* Device's status */
	UINT32 lSerialNumber;							/* Device's serial number */
	INT8 productName[MAX_PRODUCT_NAME_SIZE];		/* Device's product name */
	UINT8 bTargetState;								/* Device's state */

} EtIPListIdentityInfo;

#ifndef EIP_MAX_PSK_DATA_SIZE
#define EIP_MAX_PSK_DATA_SIZE	128
#endif

#ifndef EIP_MAX_FILE_PATH_SIZE
#define EIP_MAX_FILE_PATH_SIZE	260
#endif

typedef enum tagFileAccessType
{
	fileReadWrite  = 0,
	fileReadOnly  = 1
}
EtIPFileAccessType;

typedef enum tagFileInvocationType
{
	fileNoActionRequired  = 0,
	fileResetToIdentityObject  = 1,
	filePowerCycleDevice = 2,
	fileStartServiceRequestRequired = 3
} EtIPFileInvocationType;

typedef enum tagFileInstanceState
{
	fileNonexistent  = 0,
	fileEmpty  = 1,
	fileLoaded = 2,
	fileUploadInitiated = 3,
	fileDownloadInitiated = 4,
	fileUploadInProgress = 5,
	fileDownloadInProgress = 6,
	fileStoring = 7
}
EtIPFileInstanceState;

typedef struct tagAppFileInstance
{
	EtIPFileInstanceState State;			/* Object State */
	STRINGI szInstanceName;					/* Instance Name, e.g. "Configuration" */
	UINT16 iFormatRevision;					/* Instance Format revision unique for a particular file format convention */
	STRINGI szFileName;						/* File Name, e.g. "EDS.gz" */
	UINT8 bFileMajorRevision;				/* File data major revision */
	UINT8 bFileMinorRevision;				/* File data minor revision */
	UINT32 dwFileSize;						/* File size in bytes */
	INT16 iChecksum;						/* File data two's complement checksum */
	EtIPFileInvocationType InvocationType;	/* Defines what should happen after download of a new file is complete */
	UINT8 bSaveToNonVolatile;				/* If True, an explicit save to non-volatile storage is required after download is complete. */
	EtIPFileAccessType AccessType;			/* Defines whether the file is Read/Write or Read Only */
	UINT8 bEncoded;							/* True if file is encoded with ZLIB */
} EtIPFileInstance;

/* The following are CIP (Ethernet/IP) Generic error codes */
#define ROUTER_ERROR_SUCCESS					0x00	/* We done good... */
#define ROUTER_ERROR_FAILURE					0x01	/* Connection failure */
#define ROUTER_ERROR_NO_RESOURCE				0x02	/* Resource(s) unavailable */
#define ROUTER_ERROR_INVALID_PARAMETER_VALUE	0x03	/* Obj specific data bad */
#define ROUTER_ERROR_INVALID_SEG_TYPE			0x04	/* Invalid segment type in path */
#define ROUTER_ERROR_INVALID_DESTINATION		0x05	/* Invalid segment value in path */
#define ROUTER_ERROR_PARTIAL_DATA				0x06	/* Not all expected data sent */
#define ROUTER_ERROR_CONN_LOST					0x07	/* Messaging connection lost */
#define ROUTER_ERROR_BAD_SERVICE				0x08	/* Unimplemented service code */
#define ROUTER_ERROR_BAD_ATTR_DATA				0x09	/* Bad attribute data value */
#define ROUTER_ERROR_ATTR_LIST_ERROR			0x0A	/* Get/set attr list failed */
#define ROUTER_ERROR_ALREADY_IN_REQUESTED_MODE	0x0B	/* Obj already in requested mode */
#define ROUTER_ERROR_OBJECT_STATE_CONFLICT		0x0C	/* Obj not in proper mode */
#define ROUTER_ERROR_OBJ_ALREADY_EXISTS			0x0D	/* Object already created */
#define ROUTER_ERROR_ATTR_NOT_SETTABLE			0x0E	/* Set of get only attr tried */
#define ROUTER_ERROR_PERMISSION_DENIED			0x0F	/* Insufficient access permission*/
#define ROUTER_ERROR_DEV_IN_WRONG_STATE			0x10	/* Device not in proper mode */
#define ROUTER_ERROR_REPLY_DATA_TOO_LARGE		0x11	/* Response packet too large */
#define ROUTER_ERROR_FRAGMENT_PRIMITIVE			0x12	/* Primitive value will fragment */
#define ROUTER_ERROR_NOT_ENOUGH_DATA			0x13	/* Goldilocks complaint #1 */
#define ROUTER_ERROR_ATTR_NOT_SUPPORTED			0x14	/* Attribute is undefined */
#define ROUTER_ERROR_TOO_MUCH_DATA				0x15	/* Goldilocks complaint #2 */
#define ROUTER_ERROR_OBJ_DOES_NOT_EXIST			0x16	/* Non-existant object specified */
#define ROUTER_ERROR_NO_FRAGMENTATION			0x17	/* Fragmentation not active */
#define ROUTER_ERROR_DATA_NOT_SAVED				0x18	/* Attr data not previously saved*/
#define ROUTER_ERROR_DATA_WRITE_FAILURE			0x19	/* Attr data not saved this time */
#define ROUTER_ERROR_REQUEST_TOO_LARGE			0x1A	/* Routing failure on request */
#define ROUTER_ERROR_RESPONSE_TOO_LARGE			0x1B	/* Routing failure on response */
#define ROUTER_ERROR_MISSING_LIST_DATA			0x1C	/* Attr data not found in list */
#define ROUTER_ERROR_INVALID_LIST_STATUS		0x1D	/* Returned list of attr w/status*/
#define ROUTER_ERROR_SERVICE_ERROR				0x1E	/* Embedded service failed */
#define ROUTER_ERROR_VENDOR_SPECIFIC			0x1F	/* Vendor specific error */
#define ROUTER_ERROR_INVALID_PARAMETER			0x20	/* Invalid parameter */
#define ROUTER_ERROR_WRITE_ONCE_FAILURE			0x21	/* Write once previously done */
#define ROUTER_ERROR_INVALID_REPLY				0x22	/* Invalid reply received */
#define ROUTER_ERROR_BAD_KEY_IN_PATH			0x25	/* Electronic key in path failed */
#define ROUTER_ERROR_BAD_PATH_SIZE				0x26	/* Invalid path size */
#define ROUTER_ERROR_UNEXPECTED_ATTR			0x27	/* Cannot set attr at this time  */
#define ROUTER_ERROR_INVALID_MEMBER				0x28	/* Member ID in list nonexistant */
#define ROUTER_ERROR_MEMBER_NOT_SETTABLE		0x29	/* Cannot set value of member */
#define ROUTER_ERROR_UNKNOWN_MODBUS_ERROR		0x2B	/* Unhandled Modbus Error */
#define ROUTER_ERROR_SERVICE_NOT_SUPPORTED_FOR_PATH	0x2E	/* Service is supported, just not for this attribute */
#define ROUTER_ERROR_STILL_PROCESSING			0xFF	/* Special marker to indicate we haven't finished processing the request yet */

#define ROUTER_ERROR_ENCAP_PROTOCOL				0x6A	/* Error in encapsulation header */

/* The following are CIP (Ethernet/IP) Extended error codes */
#define ROUTER_EXT_ERR_NO_EXTENDED						0xFF
#define ROUTER_EXT_ERR_DUPLICATE_FWD_OPEN				0x100
#define ROUTER_EXT_ERR_CLASS_TRIGGER_INVALID			0x103
#define ROUTER_EXT_ERR_OWNERSHIP_CONFLICT				0x106
#define ROUTER_EXT_ERR_CONNECTION_NOT_FOUND				0x107
#define ROUTER_EXT_ERR_INVALID_CONN_TYPE				0x108
#define ROUTER_EXT_ERR_INVALID_CONN_SIZE				0x109
#define ROUTER_EXT_ERR_DEVICE_NOT_CONFIGURED			0x110
#define ROUTER_EXT_ERR_RPI_NOT_SUPPORTED				0x111
#define ROUTER_EXT_ERR_RPI_VALUE_NOT_ACCEPTABLE			0x112
#define ROUTER_EXT_ERR_CONNECTION_LIMIT_REACHED			0x113
#define ROUTER_EXT_ERR_VENDOR_PRODUCT_CODE_MISMATCH		0x114
#define ROUTER_EXT_ERR_PRODUCT_TYPE_MISMATCH			0x115
#define ROUTER_EXT_ERR_REVISION_MISMATCH				0x116
#define ROUTER_EXT_ERR_INVALID_CONN_POINT				0x117
#define ROUTER_EXT_ERR_INVALID_CONFIG_FORMAT			0x118
#define ROUTER_EXT_ERR_NO_CONTROLLING_CONNECTION		0x119
#define ROUTER_EXT_ERR_TARGET_CONN_LIMIT_REACHED		0x11A
#define ROUTER_EXT_ERR_RPI_SMALLER_THAN_INHIBIT			0x11B
#define ROUTER_EXT_ERR_TRANSPORT_CLASS_NOT_SUPPORTED	0x11C
#define ROUTER_EXT_ERR_PRODUCTION_TRIGGER_NOT_SUPPORTED	0x11D
#define ROUTER_EXT_ERR_DIRECTION_NOT_SUPPORTED			0x11E
#define ROUTER_EXT_ERR_INVALID_OT_FIXED_VARIABLE		0x11F
#define ROUTER_EXT_ERR_INVALID_TO_FIXED_VARIABLE		0x120
#define ROUTER_EXT_ERR_INVALID_OT_PRIORITY				0x121
#define ROUTER_EXT_ERR_INVALID_TO_PRIORITY				0x122
#define ROUTER_EXT_ERR_INVALID_OT_CONNECTION_TYPE		0x123
#define ROUTER_EXT_ERR_INVALID_TO_CONNECTION_TYPE		0x124
#define ROUTER_EXT_ERR_INVALID_OT_REDUNDANT_OWNER		0x125
#define ROUTER_EXT_ERR_INVALID_CONFIGURATION_SIZE		0x126
#define ROUTER_EXT_ERR_INVALID_OT_SIZE					0x127
#define ROUTER_EXT_ERR_INVALID_TO_SIZE					0x128
#define ROUTER_EXT_ERR_INVALID_CONFIGURATION_PATH		0x129
#define ROUTER_EXT_ERR_INVALID_CONSUMING_PATH			0x12A
#define ROUTER_EXT_ERR_INVALID_PRODUCING_PATH			0x12B
#define ROUTER_EXT_ERR_NO_CONFIGURATION_SYMBOL			0x12C
#define ROUTER_EXT_ERR_NO_CONSUME_SYMBOL				0x12D
#define ROUTER_EXT_ERR_NO_PRODUCE_SYMBOL				0x12E
#define ROUTER_EXT_ERR_INVALID_APP_PATH_COMBINATION		0x12F
#define ROUTER_EXT_ERR_INCONSISTENT_CONSUME_DATA_FORMAT	0x130
#define ROUTER_EXT_ERR_INCONSISTENT_PRODUCE_DATA_FORMAT	0x131
#define ROUTER_EXT_ERR_NULL_FORWARD_OPEN_NOT_SUPPORTED	0x132
#define ROUTER_EXT_ERR_BAD_CONN_TIMEOUT_MULTIPLIER		0x133
#define ROUTER_EXT_ERR_MISMATCH_TO_CONN_SIZE			0x134
#define ROUTER_EXT_ERR_MISMATCH_TO_FIXED_VAR			0x135
#define ROUTER_EXT_ERR_MISMATCH_TO_CONN_PRIORITY		0x136
#define ROUTER_EXT_ERR_MISMATCH_TRANSPORT_CLASS			0x137
#define ROUTER_EXT_ERR_MISMATCH_TO_PRODUCTION_TRIGGER	0x138
#define ROUTER_EXT_ERR_MISMATCH_TO_PRODUCTION_INHIBIT	0x139

#define ROUTER_EXT_ERR_CONNECTION_TIMED_OUT				0x203
#define ROUTER_EXT_ERR_UNCONNECTED_SEND_TIMED_OUT		0x204
#define ROUTER_EXT_ERR_PARAMETER_ERROR					0x205
#define ROUTER_EXT_ERR_MESSAGE_TOO_LARGE				0x206
#define ROUTER_EXT_ERR_UNCONN_ACK_WITHOUT_REPLY			0x207
#define ROUTER_EXT_ERR_NO_BUFFER_MEMORY_AVAILABLE		0x301
#define ROUTER_EXT_ERR_BANDWIDTH_NOT_AVAILABLE			0x302
#define ROUTER_EXT_ERR_TAG_FILTERS_NOT_AVAILABLE		0x303
#define ROUTER_EXT_ERR_REAL_TIME_DATA_NOT_CONFIG		0x304
#define ROUTER_EXT_ERR_PORT_NOT_AVAILABLE				0x311
#define ROUTER_EXT_ERR_LINK_ADDR_NOT_AVAILABLE			0x312
#define ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE		0x315
#define ROUTER_EXT_ERR_PATH_CONNECTION_MISMATCH			0x316
#define ROUTER_EXT_ERR_INVALID_NETWORK_SEGMENT			0x317
#define ROUTER_EXT_ERR_INVALID_LINK_ADDRESS				0x318
#define ROUTER_EXT_ERR_SECOND_RESOURCES_NOT_AVAILABLE	0x319
#define ROUTER_EXT_ERR_CONNECTION_ALREADY_ESTABLISHED	0x31A
#define ROUTER_EXT_ERR_DIRECT_CONN_ALREADY_ESTABLISHED	0x31B
#define ROUTER_EXT_ERR_MISC								0x31C
#define ROUTER_EXT_ERR_REDUNDANT_CONNECTION_MISMATCH	0x31D
#define ROUTER_EXT_ERR_NO_MORE_CONSUMER_RESOURCES		0x31E
#define ROUTER_EXT_ERR_NO_TARGET_PATH_RESOURCES			0x31F
#define ROUTER_EXT_ERR_VENDOR_SPECIFIC					0x320
#ifdef EIP_CIPSAFETY
#define ROUTER_EXT_ERR_INCOMPATIBLE_MULTICAST_RPI		0x801
#define ROUTER_EXT_ERR_INVALID_SAFETY_CONN_SIZE			0x802
#define ROUTER_EXT_ERR_INVALID_SAFETY_CONN_FORMAT		0x803
#define ROUTER_EXT_ERR_INVALID_TIME_CORRECTION_PARAM	0x804
#define ROUTER_EXT_ERR_INVALID_PING_EPI_MULTIPLIER		0x805
#define ROUTER_EXT_ERR_COORDINATION_MSG_MIN_MULTIPLIER	0x806
#define ROUTER_EXT_ERR_NET_TIME_EXPECTATION_MULTIPLIER	0x807
#define ROUTER_EXT_ERR_TIMEOUT_MULTIPLIER				0x808
#define ROUTER_EXT_ERR_INVALID_MAX_CONSUMER_NUMBER		0x809
#define ROUTER_EXT_ERR_INVALID_CPCRC					0x80A
#define ROUTER_EXT_ERR_TIME_CORRECTION_CONN_ID_INVALID	0x80B
#define ROUTER_EXT_ERR_SCID_MISMATCH					0x80C
#define ROUTER_EXT_ERR_TUNID_NOT_SET					0x80D
#define ROUTER_EXT_ERR_TUNID_MISMATCH					0x80E
#define ROUTER_EXT_ERR_CONFIG_OPERATION_NOT_ALLOWED		0x80F
#endif

/* Additional status codes that may go with extended errors */
#define CONNMGR_EXT_ERR_RPI_ACCEPTABLE						0
#define CONNMGR_EXT_ERR_RPI_UNSPECIFIED						1
#define CONNMGR_EXT_ERR_RPI_MIN_ACCEPTABLE					2
#define CONNMGR_EXT_ERR_RPI_MAX_ACCEPTABLE					3
#define CONNMGR_EXT_ERR_RPI_REQUIRED						4

/* The following are CIP (Ethernet/IP) service codes */
#define SVC_GET_ATTR_ALL			0x01
#define SVC_SET_ATTR_ALL			0x02
#define SVC_GET_ATTR_LIST			0x03
#define SVC_SET_ATTR_LIST			0x04
#define SVC_RESET					0x05
#define SVC_START					0x06
#define SVC_STOP					0x07
#define SVC_CREATE					0x08
#define SVC_DELETE					0x09
#define SVC_MULTIPLE_SVC_REQUEST	0x0A
#define SVC_APPLY_ATTRIBUTES		0x0D
#define SVC_GET_ATTR_SINGLE			0x0E
#define SVC_SET_ATTR_SINGLE			0x10
#define SVC_FIND_NEXT_OBJ_INST		0x11
#define SVC_RESTORE					0x15
#define SVC_SAVE					0x16
#define SVC_NO_OP					0x17
#define SVC_GET_MEMBER				0x18
#define SVC_SET_MEMBER				0x19
#define SVC_INSERT_MEMBER			0x1A
#define SVC_REMOVE_MEMBER			0x1B
#define SVC_CIP_DATA_TABLE_READ		0x4C
#define SVC_CIP_DATA_TABLE_WRITE	0x4D

/* Trace filters for clientSetDebuggingLevel() */
#define TRACE_LEVEL_ERROR			0x01	/* Displays major errors in the Ethernet/IP stack */
#define TRACE_LEVEL_WARNING			0x02	/* Displays possible errors/warnings in the Ethernet/IP stack */
#define TRACE_LEVEL_NOTICE			0x04	/* Displays notifications in the Ethernet/IP stack */
#define TRACE_LEVEL_DATA			0x08	/* Displays all Ethernet/IP traffic */
#define TRACE_LEVEL_USER			0x10	/* Used for application specific messages */
#define TRACE_LEVEL_ALL				0x1f	/* Display all messages */
#define TRACE_LEVEL_NONE			0x0		/* Disable filter */

#define TRACE_TYPE_TCP_IN			0x0100	/* Display incoming TCP data (further filter of TRACE_LEVEL_DATA) */
#define TRACE_TYPE_TCP_OUT			0x0200	/* Display outgoing TCP data (further filter of TRACE_LEVEL_DATA) */
#define TRACE_TYPE_UDP_IN			0x0400	/* Display incoming UDP (class1) data (further filter of TRACE_LEVEL_DATA) */
#define TRACE_TYPE_UDP_OUT			0x0800	/* Display outgoing UDP (class1) data (further filter of TRACE_LEVEL_DATA) */
#define TRACE_TYPE_BCAST_IN			0x1000	/* Display incoming broadcast data (further filter of TRACE_LEVEL_DATA) */
#define TRACE_TYPE_BCAST_OUT		0x2000	/* Display outgoing broadcast data (further filter of TRACE_LEVEL_DATA) */
#define TRACE_TYPE_DATA_FILTER		0xFF00	/* Display all TRACE_LEVEL_DATA */

#define TRACE_TYPE_MSG				0x0001	/* Display message information */
#define TRACE_TYPE_CONNECTION		0x0002	/* Display information related to connections */
#define TRACE_TYPE_SESSION			0x0004	/* Display information related to sessions */
#define TRACE_TYPE_EXPLICIT			0x0008	/* Display information related to explicit messages */
#define TRACE_TYPE_SECURITY			0x0010	/* Display information related to CIP Security */
#define TRACE_TYPE_SSL_LIB			0x0020	/* Display information from SSL library (if applicable) */
#define TRACE_TYPE_ALL				0xFFFF	/* Display all type messages */
#define TRACE_TYPE_NONE				0x0		/* Disable filter */




/* The following macros are required for certain types of processors (i.e. ARM).
   They replace word and double word addressing with individual byte handling */

/* Please note that we assume that over the wire the data always is transferred
   in little-endian format, even if target and/or originator are using big-endian */

/*
** Macros for ENCAP number representation conversion.  Multi-byte fields
** in ENCAP are all LITTLE ENDIAN (x86 order).  If the byte order of 
** the target machine is not specified then LITTLE ENDIAN is assumed.
**
** These macros are provided in three forms:
**      1.) In Place Conversion
**              Converts a variable and assigns the new value
**              back to the variable.
**
**              ENCAP_CVT_HL    - Convert into Host Long
**              ENCAP_CVT_PL    - Convert into Protocol Long
**              ENCAP_CVT_HS    - Convert into Host Short
**              ENCAP_CVT_PS    - Convert into Protocol Short
**
**      2.) Value Conversion
**              Simply convert variable value.  This form can
**              be used on the right-hand side of an assignment.
**
**              ENCAP_TO_HL(x)  - To Host Long
**              ENCAP_TO_PL(x)  - To Protocol Long
**              ENCAP_TO_HS(x)  - To Host Short
**              ENCAP_TO_PS(x)  - To Protocol Short
**/

#ifndef EIP_USER_UINT_MACROS
#ifndef HIBYTE
#define HIBYTE(w)			((UINT8)(((UINT16)(w) >> 8) & 0xFF))
#endif
#ifndef LOBYTE
#define LOBYTE(w)			((UINT8)(w))
#endif
#ifndef HIWORD
#define HIWORD(l)			((UINT16)(((UINT32)(l) >> 16) & 0xFFFF))
#endif
#ifndef LOWORD
#define LOWORD(l)			((UINT16)(l))
#endif
#ifndef MAKEWORD
#define MAKEWORD(a, b)		((UINT16)(((a) & 0xff) | ((UINT16)(((b) << 8) & 0xff00))))
#endif
#ifndef MAKELONG
#define MAKELONG(a, b)		((INT32)(((a) & 0xffff) | ((UINT32)(((b) << 16) & 0xffff0000))))
#endif

#define UINT32_SET( ptr, value )	{ *(UINT8*)(ptr) = LOBYTE(LOWORD((UINT32)value)); \
									*(UINT8*)(((UINT8*)(ptr))+1) = HIBYTE(LOWORD((UINT32)value)); \
									*(UINT8*)(((UINT8*)(ptr))+2) = LOBYTE(HIWORD((UINT32)value)); \
									*(UINT8*)(((UINT8*)(ptr))+3) = HIBYTE(HIWORD((UINT32)value)); }

#define UINT32_GET( ptr ) (UINT32) (MAKELONG(MAKEWORD((*(UINT8*)(ptr)),(*(UINT8*)(ptr+1))),MAKEWORD((*(UINT8*)(ptr+2)),(*(UINT8*)(ptr+3)))))

#define UINT16_SET( ptr, value )  { *(UINT8*)(ptr) = LOBYTE((UINT16)value);  \
							*(UINT8*)(((UINT8*)(ptr))+1) = HIBYTE((UINT16)value); }

#define UINT16_GET( ptr )	(UINT16) (MAKEWORD((*(UINT8*)(ptr)),(*(UINT8*)(ptr+1))))



#ifdef EIP_BIG_ENDIAN

#define _SWAPL_(a)			(((a) << 24) & 0xff000000) | (((a) << 8) & 0xff0000) | \
							(((a) >> 8) & 0xff00) | ((((UINT32) (a)) >> 24) & 0xff)

#define _SWAPS_(a)			(((a & 0xff) << 8) | ((((a) >> 8)) & 0xff))

#else /* #ifdef EIP_BIG_ENDIAN */

#define _SWAPL_(a)			(a)
#define _SWAPS_(a)			(a)

#endif /* #ifdef EIP_BIG_ENDIAN */

#endif /* #ifdef EIP_USER_UINT_MACROS */

#define ENCAP_CVT_HL(x)			(x) = _SWAPL_((x))
#define ENCAP_CVT_PL(x)			(x) = _SWAPL_((x))
#define ENCAP_CVT_HS(x)			(x) = _SWAPS_((x))
#define ENCAP_CVT_PS(x)			(x) = _SWAPS_((x))

#define ENCAP_TO_HL(x)			_SWAPL_((x))
#define ENCAP_TO_PL(x)			_SWAPL_((x))
#define ENCAP_TO_HS(x)			_SWAPS_((x))
#define ENCAP_TO_PS(x)			_SWAPS_((x))

#endif /* ET_IP_API_H */




