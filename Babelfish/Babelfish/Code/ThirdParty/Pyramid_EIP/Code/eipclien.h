/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPCLIEN.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Library main API
**
*****************************************************************************
*/

#ifndef EIPCLIEN_H
#define EIPCLIEN_H

#define ERROR_STATUS								(-1)	/* Return in case of a failure */

#ifdef EIP_LOAD_SHARING
#define EIP_CHECK_BROADCAST_RATE				20	/* Check for broadcasted EtherNet/IP messages every X "ticks" of clientMainTask() */
#define EIP_CHECK_INCOMING_SESSION_RATE			100	/* Check for incoming TCP sessions every X "ticks" of clientMainTask() */
#define EIP_LIMIT_NUM_CONNECTIONS_PROCESSED		10	/* The maximum number of I/O connections that will send out a packet in a single "tick" of clientMainTask() */
#define EIP_LIMIT_NUM_IO_PACKETS_RECEIVED		10	/* The maximum number of I/O packets received that will be received in a single "tick" of clientMainTask() */
#define EIP_LIMIT_NUM_SESSIONS_PROCESSED		5	/* The maximum number of TCP sessions checked in a single "tick" of clientMainTask() */
#define EIP_REQUEST_SEND_RATE					1	/* Send a ready request every X "ticks" of clientMainTask() */
#endif

#define CONN_PROCESSED_LIMIT					3	/* Increase to bump up TCP priority; decrease to raise UDP priority */
#define TCP_MINIMUM_RATE						10	/* Minimum rate at which TCP packets will be processed, even if there is plenty of UDP traffic. If UDP traffic allows, TCP will be processed more frequently. */

extern PLATFORM_MUTEX_TYPE ghClientMutex;			/* Used to protect integrity of the client calls */

#ifndef EIP_NO_ORIGINATE_UCMM
extern INT32 gnCurrentRequestGroupIndex;			/* Currently populated request group Id or (-1) if no group is currently being populated */
#endif

extern UINT32 gpClientAppProcClasses[MAX_CLIENT_APP_PROC_CLASS_NBR];		/* Class number array that will be processed by the client application */
extern UINT32 glClientAppProcClassNbr;										/* Number of classes registered for client application processing */
extern UINT32 gpClientAppProcServices[MAX_CLIENT_APP_PROC_SERVICE_NBR];		/* Service number array that will be processed by the client application */
extern UINT32 glClientAppProcServiceNbr;									/* Number of services registered for client application processing */
extern EtIPObjectRegisterType  gpClientAppProcObjects[MAX_CLIENT_APP_PROC_OBJECT_NBR];	/* Object handler array that will be processed by the client application */
extern UINT32 glClientAppProcObjNbr;										/* Number of specific object handlers registered for client application processing */

extern UINT32  gdwTickCount;			/* Stored system tick count */

EXTERN BOOL gbTerminated;				/* Flag indicating whether the client is stopped */
EXTERN BOOL gbReallyTerminated;			/* Flag indicating Ethernet/IP thread is done */

extern BOOL gbRunMode;					/* Indicate whether Run or Idle state will be sent when producing connection data */

/*---------------------------------------------------------------------------
** clientAdapterStart( )
**
** Start executing the adapter.
**---------------------------------------------------------------------------
*/
EXTERN BOOL clientStart(void);

/*---------------------------------------------------------------------------
** clientAdapterStop( )
**
** Stop running the adapter. Should always be called before unloading the DLL.
**---------------------------------------------------------------------------
*/
EXTERN void clientStop(void);

/*---------------------------------------------------------------------------
** clientRegisterEventCallBack( )
**
** Register client application callback function
**---------------------------------------------------------------------------
*/
EXTERN void clientRegisterEventCallBack( LogEventCallbackType *pfnLogEvent );

/*---------------------------------------------------------------------------
** clientRegisterMessageCallBack( )
**
** Register client application log message function
** This is only utilized with TRACE_OUTPUT defined
**---------------------------------------------------------------------------
*/
EXTERN void clientRegisterMessageCallBack( LogMessageCallbackType *pfnLogMessage );

/*---------------------------------------------------------------------------
** clientSetDebuggingLevel( )
**
** Sets the level of debug statements written to a file/debug window if
** tracing is turned on.  The two filters are ANDed to determine if message
** should be logged.  See EtIPApi.h for filter macros (TRACE_LEVEL_* and TRACE_TYPE_*)
**---------------------------------------------------------------------------
*/
EXTERN void clientSetDebuggingLevel(UINT8 iDebugLevel, UINT16 iDebugType);

/*---------------------------------------------------------------------------
** clientAddAssembly( )
**
** Add a new assembly object. iAssemblyInstance must be unique among assembly
** object collection. Connection points and configuration connection instance
** for the new connection - all fall in the category of the assembly instance.
** iType must be a combination of a type from EtIPAssemblyAccessType with a type 
** from EtIPAssemblyDirectionType and, optionally, one of the types from 
** tagEtIPAssemblyAdditionalFlag defined in EtIPApi.h.
** nOffset will determine offset of this particular assembly in the assembly
** buffer. nOffset may be passed as INVALID_OFFSET - in this case it will be 
** automatically assigned by the stack from the unalocated assembly space. 
** iLength is the assembly object size in bytes.
** szAssemblyName is the optional tag name associated with the assembly
** instance. It may be specified by the incoming Forward Open instead of the 
** assembly instance. Can be passed as NULL if there is no associated tag name.
** Returns 0 in case of success or one of the ERR_ASSEMBLY_... error codes.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientAddAssemblyInstance(UINT32 iAssemblyInstance, UINT16 iType, INT32 nOffset, UINT16 iLength, char* szAssemblyName);

/*---------------------------------------------------------------------------
** clientRemoveAssemblyInstance( )
**
** Removes previosly configured assembly object. iAssemblyInstance is the 
** unique instance of the assembly object.
** Returns 0 in case of success or ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID
** the instance specified is invalid, or ERR_ASSEMBLY_INSTANCE_USED if 
** there is an active or pending connection that is configured to use 
** this instance.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientRemoveAssemblyInstance(UINT32 iAssemblyInstance);    

/*---------------------------------------------------------------------------
** clientRemoveAllAssmInstances( )
**
** Removes all previosly configured assembly objects.
** Returns 0 in case of success or ERR_ASSEMBLY_INSTANCE_USED if 
** there is an active or pending connection that is configured to use 
** this instance.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientRemoveAllAssmInstances(void);

/*---------------------------------------------------------------------------
** clientAddAssemblyMember( )
**
** Add a new member for the assembly object. If members are used, this function 
** will be called multiple times after calling EtIPAddAssemblyInstance. 
** The first call will be for offset 0. All sebsequent ones will be for the 
** offset starting where the previous EtIPAddAssemblyMember ended. 
** EtIPAddAssemblyMember should cover all data size allocated for this
** assembly instance. If a few bits should be skipped EtIPAddAssemblyMember
** will be called with nMember set to INVALID_MEMBER. There is no byte or word 
** padding.
** iAssemblyInstance must be a valid instance previously allocated with a 
** EtIPAddAssemblyInstance successful call. iMember is a member id associated
** with this particular member. iBitSize is the member size in bits.  
** Returns 0 in case of success or one of the ERR_ASSEMBLY_... error codes.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientAddAssemblyMember(UINT32 iAssemblyInstance, UINT32 iMember, UINT16 iBitSize);

/*---------------------------------------------------------------------------
** clientRemoveAllAssemblyMembers( )
**
** Removes previosly configured assembly object members. Gives the client
** application a chance to repopulate member array if needed. There should 
** be no active or pending connections using this assembly instance for the 
** call to succeed.
** iAssemblyInstance is previously configured instance of the assembly object.
** Returns 0 in case of success or ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID
** the instance specified is invalid, or ERR_ASSEMBLY_INSTANCE_USED if 
** there is an active or pending connection that is configured to use 
** this instance.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientRemoveAllAssemblyMembers(UINT32 iAssemblyInstance);

/*---------------------------------------------------------------------------
** clientGetAssemblyInstances( )
**
** Returns total number of configured assemblies.
** pnAssemblyInstanceArray should point to the array of MAX_ASSEMBLIES integers.
** It will receive the assembly instances.
** Example:
**
** USHORT AssemblyInstanceArray[MAX_ASSEMBLIES];
** INT32 nNumAssemblies = clientGetAssemblyInstances(AssemblyInstanceArray);
**
** After return AssemblyInstanceArray[0] will have the first assembly instance,
** AssemblyInstanceArray[1] will have the second assembly instance ...
** AssemblyInstanceArray[nNumAssemblies-1] will have the last assembly 
** instance.
**---------------------------------------------------------------------------
*/

EXTERN INT32 clientGetAssemblyInstances(INT32 *pnAssemblyInstanceArray);

/*---------------------------------------------------------------------------
** clientGetAssemblyConfig( )
**
** Return configuration for the particular assembly instance.
** nAssemblyInstance is the assembly instance obtained by calling 
** EtIPGetAssemblyInstances() or EtIPAddAssemblyInstance().
** pAssemblyConfig is the pointer to the EtIPAssemblyConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_ASSEMBLY_INVALID_INSTANCE
** if assembly with this instance does not exist.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetAssemblyConfig(INT32 nAssemblyInstance, EtIPAssemblyConfig* pAssemblyConfig);    

/*-------------------------------------------------------------------------------
** clientGetAssemblyMemberData( )
**
** Fills the provided buffer with the assembly data for the particular 
** member of an assembly instance. pData is the pointer to the buffer to
** be fiiled in. iSize is the allocated buffer length in bytes.
** iAssemblyInstance and iMember identify the target member of an assembly object.
** Returns the number of bytes populated in pData, which will be the least of 
** the member size and the passed buffer length. 
**--------------------------------------------------------------------------------
*/
EXTERN UINT16 clientGetAssemblyMemberData( UINT32 iAssemblyInstance, UINT32 iMember, UINT8* pData, UINT16 iSize );

/*---------------------------------------------------------------------------
** clientSetAssemblyMemberData( )
**
** Fills the assembly member with the data from the passed data buffer.
** iAssemblyInstance and iMember identify the target member.
** pData is the pointer to the buffer to copy data from. 
** iSize is the buffer length in bytes. If member size is less than 
** iSize only member size bits will be copied from the pData. 
** Returns the actual size of copied data in bytes, which will be the 
** least of the member size and the passed buffer length.
**---------------------------------------------------------------------------
*/
EXTERN UINT16 clientSetAssemblyMemberData( UINT32 iAssemblyInstance, UINT32 iMember, UINT8* pData, UINT16 iSize );

/*---------------------------------------------------------------------------
** clientGetAssemblyInstanceData( )
**
** Fills the provided buffer with the assembly data for the particular 
** assembly instance. pData is the pointer to the buffer. iSize is the 
** buffer length. Returns the actual size of copied data which will be 
** the least of assembly object size and the passed buffer length.
** Returns 0 if iAssemblyInstance has not been configured.
**---------------------------------------------------------------------------
*/
EXTERN UINT16 clientGetAssemblyInstanceData( UINT32 iAssemblyInstance, UINT8* pData, UINT16 iSize );

/*---------------------------------------------------------------------------
** clientSetAssemblyInstanceData( )
**
** Fills the assembly object with the passed buffer data. pData is the 
** pointer to the buffer. iSize is the buffer length. 
** Returns the actual size of copied data, which may be less than iSize
** if the assembly object size is less than iSize.
** Returns 0 if iAssemblyInstance has not been configured.
**---------------------------------------------------------------------------
*/
EXTERN UINT16 clientSetAssemblyInstanceData( UINT32 iAssemblyInstance, UINT8* pData, UINT16 iSize );
/*EATON_EIP_Changes:MAV*/
EXTERN UINT16 clientSetAssemblyInstanceDataOffset( UINT32 iAssemblyInstance,
										UINT8* pData, UINT16 iOffset, UINT16 iSize );

/*-------------------------------------------------------------------------------
** clientGetAssemblyCombinedData( )
**
** Fills the provided buffer with the assembly data for one or more 
** assembly instances. pData is the pointer to the buffer. iSize is the 
** number of bytes to copy. nOffset is the start copy offset in the big assembly 
** buffer - buffer where data for all instances is stored.
** Return the number of bytes copied in the provided buffer, which will be iSize
** if successful or 0 if nOffset + iSize exceed the assembly buffer size.
**-------------------------------------------------------------------------------
*/
EXTERN UINT16 clientGetAssemblyCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize );

/*---------------------------------------------------------------------------
** clientSetAssemblyCombinedData( )
**
** Fills one or more assembly instances with the passed buffer data. pData is 
** the pointer to the buffer. iSize is the number of bytes to copy. 
** nOffset is the start copy offset in the assembly buffer  - buffer 
** of ASSEMBLY_SIZE bytes where data for all instances is stored.
** Return the number of bytes copied from the provided buffer, which will be 
** iSize if successful or 0 if nOffset + iSize exceed the assembly buffer size.
**---------------------------------------------------------------------------
*/
EXTERN UINT16 clientSetAssemblyCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize );

/*---------------------------------------------------------------------------
** clientAutomaticRunIdleEnforcement( )
**
** Enforces the presense of the 32-bit header in the O->T direction and enforces
** the lack of a 32-bit header in the T->O direction for ExclusiveOwner I/O 
** connections.  Enforces no 32-bit header for either direction on InputOnly or
** ListenOnly connections.
**---------------------------------------------------------------------------
*/
EXTERN void clientAutomaticRunIdleEnforcement(BOOL bEnforce);

/*---------------------------------------------------------------------------
** clientGetNumConnections( )
**
** Returns total number of connections in the state other than 
** ConnectionNonExistent.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetNumConnections(void);

/*---------------------------------------------------------------------------
** clientGetConnectionInstances( )
**
** Returns total number of connections in the state other than 
** ConnectionNonExistent.
** pnConnectionIDArray should point to the array of MAX_CONNECTIONS integers.
** It will receive the connection instances.
** Example:
**
** INT32 ConnectionInstanceArray[MAX_CONNECTIONS];
** INT32 nNumConnections = clientGetConnectionInstances(ConnectionInstanceArray);
**
** After return ConnectionInstanceArray[0] will have the first connection instance,
** ConnectionInstanceArray[1] will have the second connection instance ...
** ConnectionInstanceArray[nNumConnections-1] will have the last connection 
** instance.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetConnectionInstances(INT32 *pnConnectionInstanceArray);

/*---------------------------------------------------------------------------
** clientGetConnectionState( )
**
** Return the state of the particular connection from the EtIPConnectionState
** enumeration.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances() and should be between 1 and MAX_CONNECTIONS. 
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetConnectionState(INT32 nConnectionInstance);

/*---------------------------------------------------------------------------
** clientGetConnectionRunIdleFlag( )
**
** Should be called for incoming I/O connections only to get the Run/Idle 
** flag that came with the run/idle header in the I/O message.
** Returns a value from the EtIPConnectionRunIdleFlag enumeration.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetConnectionRunIdleFlag(INT32 nConnectionInstance);                

/*---------------------------------------------------------------------------
** clientGetConnectionConfig( )
**
** Return configuration for the particular connection.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances().
** pConfig is the pointer to the EtIPConnectionConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetConnectionConfig(INT32 nConnectionInstance, EtIPConnectionConfig* pConfig);

/*---------------------------------------------------------------------------
** clientGetConnectionInternalCfg( )
**
** Return internal configuration for the particular connection.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances().
** pConfig is the pointer to the EtIPInternalConnectionConfig structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetConnectionInternalCfg(INT32 nConnectionInstance, EtIPInternalConnectionConfig* pConfig);

/*---------------------------------------------------------------------------
** clientGetConnectionStats( )
**
** Return connection stats for the particular connection.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances().
** pStats is the pointer to the EtIPConnectionStats structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** bClear will reset stats to 0 if TRUE
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetConnectionStats(INT32 nConnectionInstance, EtIPConnectionStats* pStats, BOOL bClear);

/*---------------------------------------------------------------------------
** clientSetConnectionProducedSize( )
**
** Set the producing size for a variable sized class1 connection.
** nConnectionInstance is the connection instance obtained by calling 
** clientGetConnectionInstances().
** iSize is the new size sent on the I/O connection. The size must be equal to or 
** lesser than the original connection size
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nConnectionInstance is out of the connection instance range: from 1 to 
** MAX_CONNECTIONS, or is in ConnectionNonExistent state. Or 
** ERR_INVALID_VARIABLE_CONNECTION_SIZE if the size specified is too large.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSetConnectionProducedSize(INT32 nConnectionInstance, UINT16 iSize);

/*---------------------------------------------------------------------------
** clientSetNewConnectionInstance( )
**
** Set a new connection ID for a particular connection.  This is aimed at applications
** organizing their target connections because the EIP stack initially assigned it an ID.
** nOldConnectionInstance is the existing connection instance
** nNewConnectionInstance is the new connection instance
** Returns 0 in case of a success, or ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED
** if nOldConnectionInstance is out of the connection instance range or doesn't exist.
** Also returns ERR_INVALID_CONNECTION_INSTANCE_SPECIFIED if nNewConnectionInstance is 
** already in use.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSetNewConnectionInstance(INT32 nOldConnectionInstance, INT32 nNewConnectionInstance);

/*---------------------------------------------------------------------------
** clientSendConnectionResponse( )
**
** Send a response to a potential connection.
** nConnectionInstance is the connection instance passed by NM_CONNECTION_VERIFICATION notification
** pErrorInfo is the response from the application.  If bGeneralStatus = 0, connection is approved/successful
** bAppSize is the size of Application Data (in words) appended to the ForwardOpen reply (typically 0)
** pAppData is the Application Data (in words) appended to the ForwardOpen reply
** See CIP specification for details on error codes.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSendConnectionResponse(INT32 nConnectionInstance, EtIPErrorInfo* pErrorInfo, UINT8 bAppSize, UINT8* pAppData);

/*---------------------------------------------------------------------------
** clientAbortAllConnections( )
**
** Remove all connections.  This will close all originated and abort
** all target connections regardless of class.
**---------------------------------------------------------------------------
*/
EXTERN void clientAbortAllConnections();

/*---------------------------------------------------------------------------
** clientSetInputAndListenOnlyConnPts( )
**
** Set the values to be used for InputOnly and ListenOnly connections points.
** This is intended for devices/applications that can't (or don't want to) change 
** the HEARTBEAT_CONN_POINT and LISTEN_ONLY_CONN_POINT macros and provides backwards
** compatibility since HEARTBEAT_CONN_POINT and LISTEN_ONLY_CONN_POINT 
** have been changed
** 
**---------------------------------------------------------------------------
*/
EXTERN void clientSetInputAndListenOnlyConnPts(UINT32 lInputOnlyConnPt, UINT32 lListenOnlyConnPt);

/*---------------------------------------------------------------------------
** clientGetRunMode()
**
** Returns Run mode setting. Initially set to FALSE. May be modified by 
** calling clientSetRunMode() function.
** The Run mode is being sent as the first bit of the 32-bit I/O header, 
** which (if configured) is sent with every producing I/O packet.
**---------------------------------------------------------------------------
*/
EXTERN BOOL  clientGetRunMode(void);

/*---------------------------------------------------------------------------
** clientSetRunMode()
**
** Set Run mode by passing TRUE, or Idle mode by pasing FALSE as a parameter.
** The state is being sent as the first bit of the 32-bit I/O header, 
** which (if configured) is sent with every producing I/O packet.
**---------------------------------------------------------------------------
*/
EXTERN void  clientSetRunMode( BOOL bRunMode );

/*---------------------------------------------------------------------------
** clientStartRequestGroup( )
**
** Starts the unconnected request group used to send multiple service 
** message. All subsequent clientSendUnconnectedRequest and 
** clientSendEPATHUnconnectedRequest calls issued before next clientStopRequestGroup
** is called are grouped into a single multiple service request. It will be sent
** after clientStopRequestGroup is called.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientStartRequestGroup(void);

/*---------------------------------------------------------------------------
** clientStopRequestGroup( )
**
** Stops forming the unconnected request group used to send multiple service 
** message. All clientSendUnconnectedRequest and clientSendEPATHUnconnectedRequest 
** calls issued between clientStartRequestGroup and clientStopRequestGroup
** are grouped into a single multiple service request. It will be sent
** after clientStopRequestGroup is called.
**---------------------------------------------------------------------------
*/
EXTERN void clientStopRequestGroup(void);

/*---------------------------------------------------------------------------
** clientSendUnconnectedRequest( )
**
** Send either UCMM or Unconnected Send depending on whether szNetworkPath 
** specifies local or remote target.
** clientGetUnconnectedResponse will be used to get the response.
** pRequest points to a structure containing the request parameters.
** Check the return value. If it's greater than or equal to REQUEST_INDEX_BASE,
** then it is a request Id to be used when calling clientGetUnconnectedResponse.
** If it is less than REQUEST_INDEX_BASE then it's one of the 
** errors listed above.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSendUnconnectedRequest( EtIPObjectRequest* pRequest );

/*---------------------------------------------------------------------------
** clientSendEPATHUnconnectedRqst( )
**
** Same as EtIPSendUnconnectedRequest above, but EPATH is populated by the 
** user. May be used in cases where more complicated EPATH should be used.
** Any available segment type mey be include in EPATH or the user may
** specify word size for parameters like class, instance, or member.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSendEPATHUnconnectedRqst( EtIPEPATHObjectRequest* pEPATHRequest );

/*---------------------------------------------------------------------------
** clientGetUnconnectedResponse( )
**
** Get a response for the previously transferred UCMM or an
** Unconnected Send message.
** nRequestId is a request Id returned from the previous call to the  
** clientSendUnconnectedRequest function. 
** pResponse contain response data in case of a success and
** extended error indormation in case of an error.
** Returns 0 if able to get the response or one of the error constants 
** listed above. If return value is 0 it does not guarantee the successful
** CIP response. You need to check bGeneralStatus member of pResponse to 
** determine whether the target returned success or an error. bGeneralStatus
** of 0 indicates a success. If bGeneralStatus is other than 0, iExtendedStatus
** will contain an optional extended error code and errorDescription will
** contain the error text corresponding to the general and extended error codes.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetUnconnectedResponse(INT32 nRequestId, EtIPObjectResponse* pResponse);

/*---------------------------------------------------------------------------
** clientRegObjectsForClientProc( )
**
** Provides the list of the objects that will be processed by the client 
** application. When the request is received for one of these objects
** the NM_CLIENT_OBJECT_REQUEST_RECEIVED notification message will be 
** sent to the client app. The client application can use clientGetClientRequest()
** and clientSendClientResponse() after that to get the request and 
** send the response.
** pClassNumberList is the pointer to the integer array. Each integer represent
** the class number of the object that will be proccessed by the client 
** application.
** nNumberOfClasses is the number of classes in the list.
** pServiceNumberList is the pointer to the integer array. Each integer represent
** the service that will be proccessed by the client application.
** nNumberOfServices is the number of the services in the list.
**---------------------------------------------------------------------------
*/
EXTERN void clientRegObjectsForClientProc(UINT32* pClassNumberList, UINT32 nNumberOfClasses, 
										  UINT32* pServiceNumberList, UINT32 nNumberOfServices);


/*---------------------------------------------------------------------------
** clientRegRequestsForClientProc( )
**
** Provides the list of specific explicit requests that will be processed by the client 
** application.  The intent is to override or extend existing functionality of the 
** objects already in the EtherNet/IP library without modifying the source directly.
** When the request is received for one of these objects the 
** NM_CLIENT_OBJECT_REQUEST_RECEIVED notification message will be 
** sent to the client app. The client application can use clientGetClientRequest()
** and clientSendClientResponse() after that to get the request and 
** send the response.
** pSpecificRequests is the pointer to an array of specific services within objects
** already included in the EtherNet/IP stack. Each "specific object" represents
** the service/class/instance/attribute combination that will be proccessed 
** by the client application.
** nNumberOfSpecificObjects is the number of the specific object requests in the list.
**---------------------------------------------------------------------------
*/
EXTERN void clientRegRequestsForClientProc(EtIPObjectRegisterType* pSpecificRequests, UINT32 nNumberOfSpecificObjects);

/*---------------------------------------------------------------------------
** clientGetClientRequest( )
**
** Gets the pending request that should be processed by the client
** application. The objects for the client application processing must
** be registered in advance by calling clientRegObjectsForClientProc()
** or clientRegRequestsForClientProc() function.
** The client application will get NM_CLIENT_OBJECT_REQUEST_RECEIVED callback
** with a new request Id. The same request Id should be passed when calling
** clientGetClientRequest() function.
** pRequest is the pointer to the EtIPObjectRequest structure that will have 
** the request parameters on return.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetClientRequest( INT32 nRequestId, EtIPObjectRequest* pObjectRequest );

/*---------------------------------------------------------------------------
** clientGetEPATHClientRequest( )
**
** Same as EtIPGetClientRequest above, but EPATH may be used in cases where 
** more complicated EPATH may be used.
** Any available segment type may be include in EPATH including duplicate
** occurences of the same type.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetEPATHClientRequest( INT32 nRequestId, EtIPEPATHObjectRequest* pEPATHRequest );

#ifdef EIP_PCCC_SUPPORT
/*---------------------------------------------------------------------------
** clientGetPCCCRequest( )
**
** Gets the pending PCCC request that should be processed by the client
** application. 
** The client application will get NM_CLIENT_PCCC_REQUEST_RECEIVED callback
** with a new request Id. The same request Id should be passed when calling
** clientGetPCCCRequest() function.
** pRequest is the pointer to the EtIPPCCCRequest structure that will have 
** the request parameters on return.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/

EXTERN INT32 clientGetPCCCRequest(INT32 nRequestId, EtIPPCCCRequest* pPCCCRequest);
#endif

/*---------------------------------------------------------------------------
** clientSendClientResponse( )
**
** Send a response to the previously received request registered to be
** processed by the client application.
** The client application previously polled the request by calling
** clientGetClientRequest() function and now sends the response to that
** request.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSendClientResponse( INT32 nRequestId, EtIPObjectResponse* pResponse ); 

#ifdef EIP_PCCC_SUPPORT
/*---------------------------------------------------------------------------
** clientSendPCCCResponse( )
**
** Send a response to the previously received PCCC request.
** The client application previously polled the request by calling
** clientGetPCCCRequest() function and now sends the response to that
** request.
** Returns 0 if successful or ERR_OBJECT_REQUEST_UNKNOWN_INDEX if there is no 
** pending request with this Id.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSendPCCCResponse(int nRequestId, EtIPPCCCResponse* pResponse);
#endif

/*---------------------------------------------------------------------------
** clientGetIdentityInfo( )
**
** Returns identity information.
** The client application is responsible for allocating EtIPIdentityInfo
** structure and passing its pointer with the clientGetIdentityInfo function
** call. On return the structure fill be filled with identity information.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetIdentityInfo(UINT32 iInstance, EtIPIdentityInfo* pInfo );

/*---------------------------------------------------------------------------
** clientSetIdentityInfo( )
**
** Updates identity information.
** iInstance is the Identity Instance number to set the data to
** pInfo is the pointer to the EtIPIdentityInfo structure with the new
** identity information.
** To change only some of the identity parameters the client application
** can call clientGetIdentityInfo, modify only appropriate structure members
** and then call clientSetIdentityInfo.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSetIdentityInfo(UINT32 iInstance, EtIPIdentityInfo* pInfo );

/*---------------------------------------------------------------------------
** clientSetIdentityStatus( )
**
** Explicitly sets the Identity Status so it is not determined by the stack.
** iInstance is the Identity Instance number to set the status
** iStatus is the current status.
** To change only some of the identity parameters the client application
** can call clientGetIdentityInfo, modify only appropriate structure members
** and then call clientSetIdentityInfo.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSetIdentityStatus(UINT32 iInstance, UINT16 iStatus);

/*---------------------------------------------------------------------------
** clientClearIdentityStatus( )
**
** Clears application's setting of Identity status so it can be determined 
** by the stack.
** iInstance is the Identity Instance number to clear the status
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientClearIdentityStatus(UINT32 iInstance);

/*---------------------------------------------------------------------------
** clientGetHostIPAddress( )
**
** Returns an array of the host IP addresses as Internet unsigned long numbers
** alHostIPAddresses should point to the preallocated unsigned long array 
** where IP address should be stored.
** iNumHostIPAddressesAllowed indicates the number of unsigned long numbers
** preallocated in alHostIPAddresses.
** Returns the number of the obtained IP addresses or iNumHostIPAddressesAllowed
** (whichever is smaller).
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetHostIPAddress( UINT32* alHostIPAddresses, UINT32 iNumHostIPAddressesAllowed);

/*---------------------------------------------------------------------------
** clientClaimHostIPAddress( )
**
** Claim one of the host IP addresses returned by clientGetHostIPAddress().
** It can be called repeatedly for each claimed IP address.  A TCP port can also
** be specified with the IP address.  If 0 is specified, the default TCP port
** 44818 (ENCAP_SERVER_PORT) will be used.  Note that each IP address can only
** use one port.
** Returned TRUE if claimed successfully, FALSE if the IP address is not in 
** the host IP address list.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientClaimHostIPAddress( UINT32 lHostIPAddress, UINT16 iPort );

/*---------------------------------------------------------------------------
** clientGetClaimedHostIPAddress( )
**
** Returns an array with the claimed host IP addresses.
** alClaimedHostIPAddresses should point to the preallocated unsigned long array 
** where the claimed IP address will be stored.
** iNumHostIPAddressesAllowed indicates the number of unsigned long numbers
** preallocated in alHostIPAddresses.
** Returns the number of the claimed IP addresses or iNumHostIPAddressesAllowed 
** (whichever is smaller).
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetClaimedHostIPAddress(UINT32* alClaimedHostIPAddresses, UINT32 iNumHostIPAddressesAllowed);

/*---------------------------------------------------------------------------
** clientClaimAllHostIPAddress( )
**
** Claims the ownership of all available host IP addresses.
**---------------------------------------------------------------------------
*/
EXTERN void clientClaimAllHostIPAddress(void);

/*---------------------------------------------------------------------------
** clientUnclaimHostIPAddress( )
**
** Unclaim one of the host IP addresses returned by clientGetHostIPAddress().
** Will be called repeatedly for each claimed IP address.
** Returned TRUE if claimed successfully, FALSE if the IP address is not in 
** the host IP address list.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientUnclaimHostIPAddress( UINT32 lHostIPAddress );

/*---------------------------------------------------------------------------
** clientUnclaimAllHostIPAddress( )
**
** Removes the ownership of all available claimed host IP addresses.
**---------------------------------------------------------------------------
*/
EXTERN void clientUnclaimAllHostIPAddress(void);

/*---------------------------------------------------------------------------
** clientModbusGetCOMPorts( )
**
** Returns an array of the COM port numbers
** Only used if ET_IP_MODBUS_SERIAL is defined
** alCOMPorts should point to the preallocated unsigned array 
** where COM ports should be stored.
** Returns the number of the obtained COM ports
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientModbusGetCOMPorts(UINT32* alCOMPorts);

/*---------------------------------------------------------------------------
** clientModbusClaimCOMPort( )
**
** Claim one of the COM ports returned by clientModbusGetCOMPorts().
** Will be called repeatedly for each claimed COM port.
** Only used if ET_IP_MODBUS_SERIAL is defined
** Returned TRUE if claimed successfully, FALSE if the COM port is not in 
** the host list.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientModbusClaimCOMPort(UINT32 lCOMPort);

/*---------------------------------------------------------------------------
** clientModbusGetClaimedCOMPorts( )
**
** Returns an array with the claimed COM ports.
** Only used if ET_IP_MODBUS_SERIAL is defined
** alCOMPorts should point to the preallocated unsigned array where the 
** claimed COM ports will be stored.
** Returns the number of the claimed COM ports
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientModbusGetClaimedCOMPorts(UINT32* alCOMPorts);

/*---------------------------------------------------------------------------
** clientModbusClaimAllCOMPorts( )
**
** Claims the ownership of all available COM ports.
** Only used if ET_IP_MODBUS_SERIAL is defined
**---------------------------------------------------------------------------
*/
EXTERN void clientModbusClaimAllCOMPorts();

/*---------------------------------------------------------------------------
** clientMainTask( )
**
** Executes one iteration of the EtherNet/IP Protocol stack.
** Function should be called at repeated intervals by the Client application
** to provide the EtherNet/IP Protocol stack with a time slice in which to 
** execute.
**---------------------------------------------------------------------------
*/
EXTERN void clientMainTask(void);

/*--------------------------------------------------------------------------------
** clientRelease()
**
** Clean up client resources on termination. 
**---------------------------------------------------------------------------------
*/                
EXTERN void clientRelease(void);

/*--------------------------------------------------------------------------------
** clientGetTickCount()
**
** Returns the system tick count. 
**---------------------------------------------------------------------------------
*/                
EXTERN UINT32 clientGetTickCount(void);

/*-------------------------------------------------------------------------------
** clientSetKeepTCPOpenForUDP()
**
** This flag is set to TRUE by default. Use this call to change 
** KeepTCPOpenForActiveCorrespondingUDP flag. 
** When KeepTCPOpenForActiveCorrespondingUDP flag is set to TRUE the TCP 
** socket that was used to establish a I/O connection is kept open
** for the duration of the I/O connection.
** If this flag is set to False - TCP socket that was used to establish an
** I/O connection is closed if there is no other activity on this TCP 
** socket for more than 1 minute after connection is opened. Setting this flag
** to FALSE lets you save some resources in lueu of possible incompatibility 
** with some hardware/software. Incompatible hardware includes Rockwell devices. 
** When communicating with Rockwell hardware this flag should be kept as TRUE.
** Even when this flag is set to TRUE the other side of the I/O connection
** may close the TCP socket. This will be handled and I/O connection will be 
** kept open regardless what the bKeepTCPOpenForActiveCorrespondingUDP flag is.
** nConnectionInstance may be set to a particular connection instance and in this 
** case the flag will only apply to that connection. It may also be set to 
** INVALID_INSTANCE and it will apply to all connections already opened or will 
** be opened in the future that did not receive the same call for their instance 
** number.
**--------------------------------------------------------------------------------
*/
EXTERN int clientSetKeepTCPOpenForUDP( BOOL bKeepTCPOpenForActiveCorrespondingUDP, INT32 nConnectionInstance );


/*---------------------------------------------------------------------------
** clientSetSTRINGI( )
**
** Convenient utility function to set a STRINGI to a data buffer
**---------------------------------------------------------------------------
*/
EXTERN void clientSetSTRINGI(UINT8* pDataBuffer, STRINGI* pString);

/*---------------------------------------------------------------------------
** clientGetSTRINGI( )
**
** Convenient utility function to get a STRINGI from a data buffer
** Returns 0 for success, ERR_STRINGI_BAD_SIZE if iBufferSize is too small
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetSTRINGI(STRINGI* pString, UINT8* pDataBuffer, UINT32 iBufferSize);

/*---------------------------------------------------------------------------
** clientLogMessage( )
**
** Logs the provided NULL terminated message in the stack's log file with a TRACE_LEVEL_USER filter bit
**---------------------------------------------------------------------------
*/

EXTERN void clientLogMessage(char *szMsg);

/*---------------------------------------------------------------------------
** clientGetNetworkStatus( )
**
** Returns the state of the network indicator (LED).  If multiple IP addresses
** are used and EIP_NSLED_PER_IP is defined, lHostIP is the "claimed" IP address
** to retrieve the NS LED status.  Otherwise lHostIP is ignored.
**---------------------------------------------------------------------------
*/
EXTERN EtIPNetworkStatus clientGetNetworkStatus(UINT32 lHostIP);

/*---------------------------------------------------------------------------
** clientSendBroadcastListIdentity( )
**
** Sends out a List Identity broadcast message
** clientGetListIdentityResponse will be used to get the response(s)
** lIPAddress is the host address used to send the message
** iMaxDelay is the maximum delay allowed by targets to respond
** Returns TRUE if messages was successfully sent
**---------------------------------------------------------------------------
*/
EXTERN BOOL clientSendBroadcastListIdentity(UINT32 lIPAddress, UINT16 iMaxDelay);

/*---------------------------------------------------------------------------
** clientGetListIdentityResponse( )
**
** Get a response for the previously sent List Identity broadcast.
** nId is a Id returned from the NM_NEW_LIST_IDENTITY_RESPONSE notification 
** Returns 0 in case of success or one of the error codes.
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetListIdentityResponse(INT32 nId, EtIPListIdentityInfo* pInfo);

/*---------------------------------------------------------------------------
** clientSetAppVerifyConnection( )
**
** Determines whether the application will verify connections before they
** are accepted.  If the application verifies connections it will recieve a
** NM_CONNECTION_VERIFICATION notification
**---------------------------------------------------------------------------
*/
EXTERN void clientSetAppVerifyConnection(BOOL bVerify);

/*---------------------------------------------------------------------------
** clientSetBackplaneSupport( )
**
** Determines whether the application support requests to the "backplane"
** port.  Requests targeted at the "device" slot will be handled by the 
** EtherNet/IP stack.  Requests for slots outside of the "device" will 
** receive a NM_BACKPLANE_REQUEST_RECEIVED notification.  The client 
** application can use clientGetClientRequest() and clientSendClientResponse() 
** after that to get the request and send the response.
** bEnable enables/disables support for responding to backplane requests
** iSlot is the slot for the EtherNet/IP stack "device"
**---------------------------------------------------------------------------
*/
EXTERN void clientSetBackplaneSupport(BOOL bEnable, UINT16 iSlot);

/*---------------------------------------------------------------------------
** clientGetSystemStats( )
**
** Return system connection stats.
** pStats is the pointer to the EtIPSystemStats structure allocated
** by the calling application. This structure is filled by this stack
** in case of a successful response.
** bClear will reset stats to 0 if TRUE
** Returns 0 in case of a success, or -1 if CONNECTION_STATS isn't defined
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientGetSystemStats(EtIPSystemStats* pStats, BOOL bClear);

/*---------------------------------------------------------------------------
** clientSetSystemStats( )
**
** Set system connection stats.  The intent is to reset specific members
** of the EtIPSystemStats structure to 0.
** pStats is the pointer to the EtIPSystemStats structure allocated
** by the calling application. 
** Returns 0 in case of a success, or -1 if CONNECTION_STATS isn't defined
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientSetSystemStats(EtIPSystemStats* pStats);

/*---------------------------------------------------------------------------
** clientFileObjectAddInstance( )
**
** Add an instance to the File Object
**---------------------------------------------------------------------------
*/
EXTERN INT32 clientFileObjectAddInstance(INT32 iInstance, EtIPFileInstance* pInstance);

/*---------------------------------------------------------------------------
** clientFileObjectRemoveInstance( )
**
** Remove an instance to the File Object
**---------------------------------------------------------------------------
*/
EXTERN BOOL clientFileObjectRemoveInstance(INT32 iInstance);

/*---------------------------------------------------------------------------
** clientFileObjectGetInstanceProperties( )
**
** Get information about a File object instance
**---------------------------------------------------------------------------
*/
EXTERN BOOL clientFileObjectGetInstanceProperties(INT32 iInstance, EtIPFileInstance* pInstance);

/*---------------------------------------------------------------------------
** clientFileObjectSetBaseDirectory( )
**
** Set the directory used by the File object for its files
**---------------------------------------------------------------------------
*/
EXTERN BOOL clientFileObjectSetBaseDirectory(char* strDirectory);

/*---------------------------------------------------------------------------
** clientComputeFileSizeAndChecksum( )
**
** Compute the file size and checksum for a File Object file
**---------------------------------------------------------------------------
*/
EXTERN BOOL clientComputeFileSizeAndChecksum(char* pstrFilename, UINT32* pFileSize, INT16* piChecksum);

#endif /* #ifndef EIPCLIEN_H */
