/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPCNCT.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Connection collection header file
**
*****************************************************************************
*/

#ifndef EIPCNCT_H
#define EIPCNCT_H

#define INVALID_CONNECTION_SERIAL_NBR		0xffff			/* Invalid connection serial number */
#define INVALID_VENDOR_ID					0xffff			/* Invalid vendor ID */
#define INVALID_DEVICE_SERIAL_NBR			0xffffffff		/* Invalid device serial number */

#define FORWARD_CLOSE_TIMEOUT				100

#define CONNECTION_CONFIGURATION_DELAY		10
#define CONNECTION_FIRST_SEND_MAX_DELAY		50				/* When to send the first packet on a connection */
#define CONNECTION_FIRST_PACKET_TIMEOUT		10000			/* Connection timeout value for first packet received on connection (dictated by EIP spec) */

#define COLLECTING_STATS_DELAY 3000

#define DEFAULT_EXPLICIT_REQUEST_TIMEOUT			10000	/* Default explicit request timeout of 10 seconds */
#define DEFAULT_WATCHDOG_TIMEOUT_RECONNECT_DELAY	10000	/* Default automatic reconnect delay of 10 seconds if connection configuration iWatchdogTimeoutAction is set to TimeoutDelayAutoReset */

#define FORWARD_OPEN_ERROR_TIMEOUT					500		 /* Give the client half a sec to get the error info after unsuccessful forward open attempt */

#define TTL_SUBNET_LEVEL							1		/* The multicast packets will be propagated for up to TTL_SUBNET_LEVEL subnetworks */


typedef enum tagConfigurationState
{
	ConfigurationLogged							= 1,		/* Connection request has just arrived */
	ConfigurationWaitingForSession,							/* Waiting for the the session to be opened */
	ConfigurationWaitingForForwardOpenResponse,				/* Forward Open has just been sent; waiting for the response.*/
	ConfigurationFailedInvalidNetworkPath,					/* Unable to connect to the target */
	ConfigurationClosing,									/* Forward Close to be issued */
	ConfigurationWaitingForForwardCloseResponse,			/* Waiting for the response to the issued Forward Close or for a timeout for an incoming connection */
	ConfigurationClosingTimeOut,							/* Close connection, but give client time to get error information */
	ConfigurationAbortConnection							/* Close conenction immediately without sending a Fwd Close first (for interop test only) */
}
ConfigurationState;

#ifdef ET_IP_MODBUS
typedef enum tagModbusWriteType
{
	ModbusWriteTypeReadWrite = 1,						/* Use Read/Write Multiple registers command  (FC 23) for writing */
	ModbusWriteTypeWriteMultiple,						/* Use Write Multiple registers command  (FC 16) for writing */
	ModbusWriteTypeWriteSingle							/* Use Write Single register command  (FC 6) for writing */
}
ModbusWriteType;

typedef struct tagMODBUS_CONNECTION_CFG
{
	UINT16 iModbusProduceSize;							/* Size in bytes of the Modbus data being produced */
	UINT16 iModbusConsumeSize;							/* Size in bytes of the Modbus data being consumed */
	UINT8* pModbusAddress;								/* Modbus target address */
	UINT16 iModbusAddressSize;							/* Size of Modbus target address */
}
MODBUS_CONNECTION_CFG;

#endif

#ifdef ET_IP_SCANNER
typedef struct tagNULL_FORWARD_OPEN_CFG
{
	BOOL bConfiguring;									/* Whether a NULL forward open is in the process of being configured */
	UINT16 iModuleConfig1Size;							/* The size of the first part of the target device specific configuration. Must be set to 0 if configuration is not passed to the target device */
	UINT8 *moduleConfig1Data;							/* Optional first part of the target device specific configuration */
	UINT16 iModuleConfig2Size;							/* The size of the second part of the target device specific configuration. Must be set to 0 if configuration is not passed to the target device */
	UINT8 *moduleConfig2Data;							/* Optional first part of the target device specific configuration */
}
NULL_FORWARD_OPEN_CFG;
#endif

typedef struct tagCONNECTION_CFG
{
	INT32  nInstance;						/* Instance of the Connection object. Will be between 1 and MAX_CONNECTIONS */
	BOOL   bOriginator;						/* Indicates whether this stack is the originator of the connection */
	UINT16 iConnectionFlag;					/* Store connection flag set by RSNetworx */
	UINT32 lIPAddress;						/* Target IP address */
	EPATH  PDU;								/* EPATH describes the target object */
	UINT32 iTargetConfigConnInstance;		/* Connection object (usually Assembly) configuration instance. Could be INVALID_INSTANCE if connecting to a tag. Set to 1 when not sure if used. */
	UINT32 iTargetProducingConnPoint;		/* Connection object (usually Assembly) Producing by the Target (Out) connection point. Could be INVALID_CONN_POINT. */
	UINT32 iTargetConsumingConnPoint;		/* Connection object (usually Assembly) Consuming by the Target (In) connection point. Could be INVALID_CONN_POINT. */
	UINT8* targetProducingConnTag;			/* Target Producing Connection Tag that can be listed instead of Producing Connection Point. */
	UINT16 iTargetProducingConnTagSize;		/* Target Producing Connection Tag Size. Must be set to 0 if Target Producing Connection Tag is not used */
	UINT8* targetConsumingConnTag;			/* Target Consuming Connection Tag that can be listed instead of Consuming Connection Point. */
	UINT16 iTargetConsumingConnTagSize;		/* Target Consuming Connection Tag Size. Must be set to 0 if Target Consuming Connection Tag is not used */
	UINT16 iCurrentProducingSize;			/* Current size of producing data for variable sized class1 connections. Maximum size of producing data for class3 connection */
	UINT16 iCurrentConsumingSize;			/* Current size of consuming data for variable sized class1 connections. Maximum size of consuming data for class3 connection */
	UINT32 lProducingDataRate;				/* Producing I/O data rate in milliseconds */
	UINT32 lConsumingDataRate;				/* Consuming I/O data rate in milliseconds */
	UINT8  bProductionOTInhibitInterval;	/* The scanner (originator) will not produce with the higher rate than inhibit interval. Value of 0 disables it. */
	UINT8  bProductionTOInhibitInterval;	/* The adapter (target) will not produce with the higher rate than inhibit interval. Value of 0 disables it. */
	BOOL   bOutputRunProgramHeader;			/* Set to TRUE if the first 4 bytes of the output data represent Run/Program header */
	BOOL   bInputRunProgramHeader;			/* Set to TRUE if the first 4 bytes of the input data represent Run/Program header */
	UINT16 iProducingConnectionType;		/* Must be PointToPoint if Originator, or either PointToPoint or Multicast if target */
	UINT16 iConsumingConnectionType;		/* Must be PointToPoint if Target, or either PointToPoint or Multicast if Originator */
	UINT16 iProducingPriority;				/* Must be one of the EtIPConnectionPriority values */
	UINT16 iConsumingPriority;				/* Must be one of the EtIPConnectionPriority values */
	UINT8  bTransportClass;					/* Indicates whether transport is Class 1 or Class 3 */
	UINT8  bTransportType;					/* One of the EtIPTransportType enumeration values */
	UINT8  bTimeoutMultiplier;				/* Timeout multiplier of 0 indicates that timeout should be 4 times larger than producing data rate, 1 - 8 times larger, 2 - 16 times, and so on */
	UINT16 iWatchdogTimeoutAction;			/* Watchdog Timeout Action from the EtIPWatchdogTimeoutAction enumeration */
	UINT32 lWatchdogTimeoutReconnectDelay;	/* If Watchdog Timeout Action is TimeoutDelayReset, Watchdog Timeout Reconnect Delay specifies reconnect delay in milliseconds */
	BOOL   bRedundantOwner;					/* Set to TRUE if originating redundant owner class 1 */
	BOOL   bClass1VariableProducer;			/* Set to TRUE if connection produced size is variable */
	BOOL   bClass1VariableConsumer;			/* Set to TRUE if connection consumed size is variable */
	UINT8  bReadyOwnershipOutputs;			/* Ready for Ownership of Outputs value for a class1 redundant connection */
	UINT8  bClaimOutputOwnership;			/* Claim Output Ownership value for a class1 redundant connection */

	UINT8   bOpenPriorityTickTime;			/* Priority bit is bit 4. Low nibble is the number of milliseconds per tick */
	UINT8   bOpenTimeoutTicks;				/* Number of ticks to timeout */

	EtIPDeviceID deviceId;					/* Optional target device identification. It will be specified in the connection path when sending Forward Open. */
#ifdef ET_IP_SCANNER
	EtIPDeviceID proxyDeviceId;				/* Optional proxy device identification. Currently used only by RSNetworx. */
	EtIPDeviceID remoteDeviceId;			/* Optional remote target device identification. Currently used only by RSNetworx. */
#endif
	UINT8* pConnectionName;					/* Optional user recognizable connection name */
	UINT16 iConnectionNameSize;				/* Connection name size. Must be set to 0 if connection name is not used */
	UINT32 lHostIPAddr;						/* Host IPAddress in UINT32 format for the opened session */
#ifdef ET_IP_SCANNER
	INT32  nOutputScannerOffset;			/* I/O data offset used to store the data received by the scanner. */
	UINT16 iOutputScannerSize;				/* I/O size in bytes for the data recieved by the scanner (not including data sequence count and not including Run/Idle information) */
	INT32  nInputScannerOffset;				/* I/O data offset used to store the data produced by the scanner. */
	UINT16 iInputScannerSize;				/* I/O size in bytes for data produced by the scanner. (including the Run/Idle information, but not including data sequence count) */
#ifdef EIP_LARGE_MEMORY
	INT32  nInputScannerTable;				/* Determines which Input Scanner assembly instance the data will go */
	INT32  nOutputScannerTable;				/* Determines which Output Scanner assembly instance the data will go */
#endif

	UINT16 iPort;							/* Saved port stripped from pdu tags (when using CCO)*/
	UINT8  bPortSize;						/* Size of port tag (1 or 2 bytes) */

	NULL_FORWARD_OPEN_CFG nullForwardOpenCfg;	/* Data used to send a NULL forward open on an existing connection */

#endif

#ifdef ET_IP_MODBUS
	MODBUS_CONNECTION_CFG modbusCfg;			/* Connection parameters specific to Modbus */
#endif
}
CONNECTION_CFG;

#ifdef ET_IP_SCANNER
typedef struct tagCONFIG
{
	INT32 nAction;				/* Action scheduled for this configuration */
	CONNECTION_CFG cfg;			/* Configuration structure to fill in via UCMM */
	EPATH ExtraPDU;				/* PDU portion that is needed for CCO config, but is not part of the ForwardOpen */
}
CONFIG;
#endif

typedef struct  tagConnection 
{
	UINT32 lConnectionState;			/* Connection state from the EtIPConnectionState enumeration */
	UINT32 lConfigurationState;			/* The state of the connection configuration. Applicable only when lConnectionState is CONNECTION_CONFIGURING. */

#ifdef EIP_LARGE_CONFIG_OPT
	void *pDelayTimer;					/* config delay timer queue entry */
	void *pConfigTimer;					/* config timeout timer queue entry */
	void *pPollListEntry;				/* state poll service queue entry */
#else
	UINT32  lStartTick;					/* When the connection should start configuring. Used to delay connection open retry after a time out. */
	UINT32  lConfigurationTimeoutTick;	/* When the connection should time out if configuration status does not change for a prolonged period of time */
#endif

	UINT8  bGeneralStatus;				/* Together with Extended Status is used to provide more error information. 0 if connection is running fine, 0xD0 if connecting or disconnecting. */
	UINT16 iExtendedStatus;				/* Together with General Status is used to provide more error information. 2 if connecting, 3 if disconnecting, or any other extended error code. */
	UINT8* pExtendedErrorData;			/* Error related data.  Appended with Extended Error only if General Status is not equal to 0 */
	UINT16 iExtendedErrorDataSize;		/* Size of error related data in bytes. */
#ifdef ET_IP_SCANNER
	UINT8* pApplicationData;			/* Application data appended to ForwardOpen reply.  Appended only on successful ForwardOpens  (General Status = 0) */
	UINT16 iApplicationDataSize;		/* Size of application data in bytes. */
#endif
	BOOL bTransferImmediately;			/* Change Of State transfer flag */

	CONNECTION_CFG cfg;					/* Connection configuration */
	REQUEST_CFG requestCfg;				/* Class3 request configuration data */

	SOCKET lClass1Socket;				/* UDP socket handle */
	UINT32 lClass1SocketConnGroup;		/* Index into IP address's group of class1 UDP sockets */
	UINT8 bRunIdleFlag;					/* Stores the Run Idle flag for incoming I/O connections only */

	UINT8 bIsRedundantMaster;			/* Set TRUE for target connection that accept data for its set of redundant owner connections */
	UINT32 lRedundantMasterTimestamp;	/* Timestamp of when redundant connection transitioned to "master" */

	BOOL bCCConfigured;					/* TRUE if Forward Open was preceeded with the CC config request for an incoming connection, FALSE otherwise. Used only for incoming connections. */
	INT32 nCCProducingInstance;			/* Used only for incoming connections to store the CC instance corresponding to the Producing connection */

	struct sockaddr_in sReceiveAddr;	/* Socket address for receiving data */
	struct sockaddr_in sTransmitAddr;	/* Socket address for transmitting data */

	UINT32 lInAddrSeqNbr;				/* Packet Sequence number to received with I/O messages */
	UINT32 lOutAddrSeqNbr;				/* Packet Sequence number to send with I/O messages */

	UINT16 iInDataSeqNbr;				/* Data Sequence number received with the last connected message */
	UINT16 iOutDataSeqNbr;				/* Data Sequence number to send with the next connected message */
	UINT8 bReceivedIOPacket;			/* Set when first I/O packet is received (to ensure new data regardless of sequence count) */

	UINT32 lProducingCID;				/* Producing Connection Id identifies data produced on this connection */
	UINT32 lConsumingCID;				/* Consuming Connection Id identifies data consumed on this connection */

	UINT32 lInhibitInterval;			/* The data should not be transmitted on COS until inhibit interval has passed since the last transmission */
	UINT32 lTimeoutInterval;			/* Connection is timed out if there is no data from the device for this period */

	UINT16 iConnectionSerialNbr;		/* Unique number identifying this connection */
	UINT16 iVendorID;					/* Unique number identifying the originator vendor */
	UINT32 lDeviceSerialNbr;			/* Unique number identifying the originator device serial number */

#ifdef EIP_LARGE_CONFIG_OPT
	void *pProduceTimer;				/* production timer queue entry */
	void *pTimeoutTimer;				/* connection timeout timer queue entry */
	void *pInhibitTimer;				/* inhibit timer queue entry */
#else
	UINT32 lProduceTick;				/* When to produce next time on this connection */
	UINT32 lTimeoutTick;				/* When to timeout if no packets are received by then */
	UINT32 lInhibitExpireTick;			/* When to enable COS production */
#endif

	UINT8* pResponseData;				/* Class3 response data */
	UINT16 iResponseDataSize;			/* Class3 response data size in bytes */

	BOOL bOutstandingClass3Request;		/* Is set to TRUE if Class3 request had been sent, but the response has not been yet received */

	INT32 nSessionId;					/* Unique identifier for the session this connection */
	INT32 nRequestId;					/* Request ID of the Forward Open request */

	/* If set to KeepTCPOpenStateFalse - close TCP socket that was used for establishing I/O connection 
		if there was no other activity on teh TCP socket for more than 1 minute.
		This lets you save some resources in lueu of possible incompatibility with some hardware/software. */
	INT32 nKeepTCPOpenForActiveCorrespondingUDP;

#ifdef ET_IP_SCANNER
	CONFIG	CCOcfg;
#endif

#ifdef ET_IP_MODBUS
	INT32 nModbusReadRequestID;			/* Request ID used to track a sent Modbus read request */
	INT32 nModbusWriteRequestID;		/* Request ID used to track a sent Modbus write request */
	UINT16 iModbusWriteType;			/* Write command used for the connection */
	UINT8* pModbusConsumeData;			/* Store data to be written to Modbus device until the correct write Modbus function is determined */
	UINT16 iModbusConsumeDataSize;		/* Modbus data size in bytes */
#ifdef ET_IP_MODBUS_SERIAL
	UINT8 bModbusSerialAddress;			/* Serial address for ModbusSL translation */
#endif
#endif

#ifdef CONNECTION_STATS
	BOOL bCollectingStats;
#ifdef EIP_LARGE_CONFIG_OPT
	void *pStatsTimer;					/* stat collection timer queue entry */
#else
	UINT32 lStartCollectStatsTick;
#endif

	UINT32 lLostPackets;
	UINT32 lMaxRcvdDelay;
	UINT32 lMaxSendDelay;
	UINT32 lRcvdPPSCount;
	UINT32 lRcvdPPSCountPrev;
	UINT32 lSendPPSCount;
	UINT32 lSendPPSCountPrev;
	UINT32 lPPSResetTick;
	UINT32 lTotalRcvdPackets;
	UINT32 lTotalSendPackets;
	UINT32 lTotalRcvdBytes;
	UINT32 lTotalSendBytes;

	UINT32 lStartRcvdTick;
	UINT32 lStartSendTick;
	UINT32 lLastRcvdTick;
	UINT32 lLastSendTick;

	UINT16 iSendPacketError;
	UINT16 iRcvdPacketError;
	UINT16 iNumTimeouts;
	UINT16 iNumErrors;

#endif /* CONNECTION_STATS */

} CONNECTION;

typedef struct tagInternalStats
{
	UINT32 iNumSendUCMMMessagesPPS;		/* Number of UCMM messages sent this second */
	UINT32 iNumRecvUCMMMessagesPPS;		/* Number of UCMM messages received this second */
	UINT32 iNumSendUCMMMessagesPPSPrev;	/* Number of UCMM messages sent in previous second */
	UINT32 iNumRecvUCMMMessagesPPSPrev;	/* Number of UCMM messages received in previous second */

	EtIPSystemStats systemStats;
} INTERNAL_STATS;


#ifdef EIP_LARGE_BUF_ON_HEAP
extern CONNECTION* gConnections;
#else
extern CONNECTION gConnections[MAX_CONNECTIONS];		/* Connection parameters array */
#endif
extern CONNECTION* gpnConnections;						/* Pointer to next available connection */
#ifdef CONNECTION_STATS
extern INTERNAL_STATS gSystemStats;
#endif

typedef enum tagKeepTCPOpenForActiveCorrespondingUDPState
{
	KeepTCPOpenStateUnknown				= 1,	/* KeepTCPOpenForActiveCorrespondingUDPState not set */
	KeepTCPOpenStateTrue				= 2,	/* KeepTCPOpenForActiveCorrespondingUDPState set to TRUE */
	KeepTCPOpenStateFalse				= 3		/* KeepTCPOpenForActiveCorrespondingUDPState set to FALSE */
}
KeepTCPOpenForActiveCorrespondingUDPState;

extern INT32  gnKeepTCPOpenForActiveCorrespondingUDP;		/* A global variable that will be used if the same variable is not set for a particular connection */
extern UINT32 gHeartbeatConnPoint;
extern UINT32 gListenOnlyConnPoint;

#define NUM_CONNECTION_GROUPS    ((MAX_CONNECTIONS-1)/PLATFORM_MAX_CONNECTIONS_PER_SOCKET + 1)

extern void connectionInit(void);											/* Initialize connection array object */
extern CONNECTION* connectionNew(void);										/* Allocate a new connection in the array */
extern BOOL connectionAssignClass1Socket( CONNECTION* pConnection );		/* Assign UDP socket to read/write I/O */
extern void connectionReleaseClass1Socket( CONNECTION* pConnection );		/* Release UDP socket */
extern void connectionGoOffline( CONNECTION* pConnection, BOOL bTimeOut );	/* Take the connection offline */
extern void connectionRemove( CONNECTION* pConnection, BOOL bTimeOut );		/* Remove a particular connection from the array */
extern void connectionRemoveAll(void);										/* Clean up connection array */
extern BOOL connectionService(CONNECTION* pConnection, BOOL* pDataSent);	/* Service the specified connection */
extern UINT16 connectionGetConnectedCount(void);							/* Returns number of active connections */
extern UINT16 connectionGetIOConnectionCount(UINT32 lIPAddress);			/* Get number of active I/O connections on a HostIP address (0 for all) */
extern UINT16 connectionGetConnDeviceStatus(void);							/* Returns device connection status */
extern void connectionUpdateAllSeqCount(void);								/* Update the data sequence count to indicate new data for all connections */
extern BOOL connectionTimedOut(CONNECTION* pConnection);					/* Specified connection timed out */
extern void connectionRecalcTimeouts( CONNECTION* pConnection );			/* Reset timeout ticks */
extern void connectionResetAllTicks( CONNECTION* pConnection );				/* Reset all ticks */
extern void connectionResetProducingTicks( CONNECTION* pConnection );		/* Set the next tick to produce the I/O */
extern void connectionResetConsumingTicks( CONNECTION* pConnection );		/* I/O was recieved - reset the timeout tick */
extern CONNECTION* connectionGetFromInstance(INT32 nInstance);				/* Returns connection pointer based on the connection instance */
extern CONNECTION* connectionGetFromAssmInstance(UINT32 iInstance);			/* Returns connection that is using passed assembly instance */
extern INT32 connectionGetUnusedInstance(void);								/* Returns the first unused instance */
extern INT32 connectionGetNumOrigEstOrClsing(void);							/* Returns the number of connections with the status ConnectionEstablished or ConnectionClosing */
#ifdef CONNECTION_STATS
extern UINT32 connectionTotalLostIOPackets(void);							/* Sum up all lost packets from all I/O connections */
extern void connectionClearLostIOPackets(void);								/* Clear lost packet count from all I/O connections */
extern UINT32 connectionTotalIOPPS(void);									/* Sum up previous second's I/O packet count from all I/O connections */
extern UINT32 connectionTotalExplicitPPS(void);								/* Sum up previous second's message count from all explicit connections and UCMMs */
#endif
extern BOOL  connectionAnotherPending( CONNECTION* pConnection );			/* Return TRUE if another connection with the same IP address is being opened or closed */
extern void  connectionDelayPending( CONNECTION* pConnection );				/* Postpone the configuration of all other connections with the same IP address for CONNECTION_CONFIGURATION_DELAY period */
extern CONNECTION* connectionGetFromSerialNumber(UINT16 iConnectionSerialNbr,	/* Return an array index based on the connection Serial Number/vendor Id/device Serial Number combination */
						UINT16 iVendorID, UINT32 lDeviceSerialNbr);
extern CONNECTION* connectionGetFirstMultiProducer(CONNECTION* pConnection);	/*Returns the connection index for the first connection producing to the same multicast as nConnection does */
extern CONNECTION* connectionGetAnyMultiProducer(CONNECTION* pConnection);		/*Returns the connection index for any connection producing to the same multicast as nConnection does */
extern INT32 connectionGetMulticastProducers(CONNECTION* pConnection);			/* Returns the number of all other connections producing to the same multicast as nConnection does */
extern INT32 connectionNotListenOnlyCnctCnt(CONNECTION* pConnection);			/* Returns the number of all not listen only connections other than the passed one that are producing for the same connection point as the passed connection */
extern void connectionSetNewRedundantMaster(CONNECTION* pConnection);
extern BOOL connectionFindNewRedundantMaster(CONNECTION* pConnection);
extern void connectionConvertFromInternal(CONNECTION_CFG* pcfg, EtIPConnectionConfig* pConfig);
extern void connectionNotifyNetLEDChange(void);
extern EtIPNetworkStatus connectionGetCurrentNetLEDState(UINT32 lIPAddress);


extern void connectionCfgRelease(CONNECTION_CFG* pcfg);

extern void connectionParseConnectionPath( CONNECTION_CFG* pConfig );
extern void connectionGenerateConnPath(char* networkPath, CONNECTION_CFG* pConfig);

extern BOOL connectionIsDataConnPoint( UINT32 iInstance );
extern BOOL connectionIsIOConnection(CONNECTION* pConnection);

#ifdef ET_IP_SCANNER
extern BOOL connectionConvertToInternal(EtIPConnectionConfig* pcfg, CONNECTION_CFG* pConfig);
extern BOOL connectionConvertEPATHToInternal(UINT8* pByteArrayEPATH, INT32 iEPATHSize, EtIPEPATHConnectionConfig* pcfg, CONNECTION_CFG* pConfig, INT32* piError);
extern void connectionSetConnectionFlag(CONNECTION* pConnection);			/* Set connection flag member based on the configuration parameters */
#endif

#ifdef EIP_LARGE_CONFIG_OPT
extern void connectionServiceConnectionPolls();
extern int connectionServiceConnectionTimers();
#endif

#endif /* #ifndef EIPCNCT_H */
