/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPNOTFY.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Sends notification events to the calling application
**
*****************************************************************************
*/

#include "eipinc.h"

LogEventCallbackType* gfnLogEvent = NULL;	/* Client application callback function */

#ifdef EIP_LARGE_BUF_ON_HEAP
static EVENT* gEvents;						/* Events array */
#else
static EVENT gEvents[MAX_EVENTS];			/* Events array */
#endif
static INT32 gnEvents;						/* Number of outstanding events */

/*
** Now declare and initialize the translation table from the
** common and user specified status codes.
** Add a final entry to mark the end of the table.
*/

#undef ET_IP_MSG
#define ET_IP_MSG( name, code, string )  { string },

/* Allocate CIP error description array */
ERROR_STRING const gCIPErrorMessages[] = { _CIPErrorMessages_ };

#ifndef EIP_CUSTOM_NOTIFY
/*---------------------------------------------------------------------------
** notifyInit( )
**
** Initialize event array
**---------------------------------------------------------------------------
*/

void notifyInit(void)
{
	#ifdef EIP_LARGE_BUF_ON_HEAP
	gEvents = (EVENT *)EtIP_mallocHeap(MAX_EVENTS * sizeof(EVENT));
	if(gEvents == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gEvents to %d entries\n", MAX_EVENTS);
		return;
	}
#endif

	gnEvents = 0;
}

/*---------------------------------------------------------------------------
** notifyEvent( )
**
** Notify the client about an event received
**---------------------------------------------------------------------------
*/

void notifyEvent( INT32 nEvent, INT32 nParam1, INT32 nParam2 )
{
	EVENT* pEvent;

#ifdef TRACE_OUTPUT

#ifdef ET_IP_SCANNER
	EtIPErrorInfo errorInfo;
#endif

	CONNECTION*  pConnection;
	REQUEST* pRequest;

	switch( nEvent )
	{
	case NM_CONNECTION_ESTABLISHED:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"New connection opened with instance %d", nParam1);
		else
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, 0, 0,
				"New connection opened with instance %d", nParam1);
		break;
#ifdef ET_IP_SCANNER
	case NM_CONN_CONFIG_FAILED_INVALID_NETWORK_PATH:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"Connection with an instance %d could not be established. Target is offline.", nParam1);
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0,
				"Connection with an instance %d could not be established. Target is offline.", nParam1);
		break;
	case NM_CONN_CONFIG_FAILED_NO_RESPONSE:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"Connection with an instance %d could not be established. Target is online, but not responding.", nParam1);
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0,
				"Connection with an instance %d could not be established. Target is online, but not responding.", nParam1);
		break;
	case NM_CONN_CONFIG_FAILED_ERROR_RESPONSE:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
		{
			errorInfo.bGeneralStatus = pConnection->bGeneralStatus;
			errorInfo.iExtendedStatus = pConnection->iExtendedStatus;
			notifyGetCIPErrorInfo(&errorInfo);

			DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"Connection with an instance %d could not be established: %s", nParam1, errorInfo.errorDescription);
		}
		else
			DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0,
				"Connection with an instance %d could not be established: %s", nParam1, errorInfo.errorDescription);
		break;
	case NM_CONNECTION_RECONFIGURED:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"Connection with an instance %d has been reconfigured.", nParam1);
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0,
				"Connection with an instance %d has been reconfigured", nParam1);
		break;
	case NM_CONNECTION_NEW_INPUT_SCANNER_DATA:
			pConnection = connectionGetFromInstance(nParam1);
			if (pConnection != NULL)
				DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
					"New input data receieved for the scanner connection with instance %d", nParam1);
			else
				DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, 0, 0,
					"New input data receieved for the scanner connection with instance %d", nParam1);
		break;

	case NM_NEW_CLASS3_RESPONSE:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"New class3 data received for connection %d", pConnection->cfg.nInstance);
		else
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, 0, 0,
				"New class3 data received for connection %d", pConnection->cfg.nInstance);
		break;
#endif
	case NM_CONNECTION_TIMED_OUT:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"Connection with instance %d timed out", nParam1);
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, 0, 0,
				"Connection with instance %d timed out", nParam1);
		break;
	case NM_CONNECTION_CLOSED:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"Connection with instance %d closed", nParam1);
		else
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, 0, 0,
				"Connection with instance %d closed", nParam1);
		break;
	case NM_INCOMING_CONNECTION_RUN_IDLE_FLAG_CHANGED:
		pConnection = connectionGetFromInstance(nParam1);
		if (pConnection != NULL)
			DumpStr2(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"New Run/Idle flag %d received for connection with instance %d", nParam2, nParam1);
		else
			DumpStr2(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, 0, 0,
				"New Run/Idle flag %d received for connection with instance %d", nParam2, nParam1);
		break;
	case NM_ASSEMBLY_NEW_INSTANCE_DATA:
		pConnection = connectionGetFromAssmInstance(nParam1);
		if (pConnection != NULL)
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"New input data received for connection %d", pConnection->cfg.nInstance);
		else
			DumpStr1(TRACE_LEVEL_WARNING, TRACE_TYPE_EXPLICIT, 0, 0,
				"New data received for assembly with instance %d", nParam1);
		break;
	case NM_ASSEMBLY_NEW_MEMBER_DATA:
		pConnection = connectionGetFromAssmInstance(nParam1);
		/* If there is a connection, NM_ASSEMBLY_NEW_INSTANCE_DATA will cover it */
		if (pConnection == NULL)
			DumpStr2(TRACE_LEVEL_WARNING, TRACE_TYPE_EXPLICIT, 0, 0,
				"New input data received for member %d of the assembly instance %d", nParam2, nParam1);
		break;
#ifndef EIP_NO_ORIGINATE_UCMM
	case NM_REQUEST_RESPONSE_RECEIVED:
		pRequest = requestGetByRequestId( nParam1 );
		if (pRequest != NULL)
			DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
				"Response received to request %d with GRC = 0x%x", nParam1, pRequest->bGeneralError);
		else
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, 0, 0,
				"Response received for request %d", nParam1);
		break;
	case NM_REQUEST_FAILED_INVALID_NETWORK_PATH:
		pRequest = requestGetByRequestId( nParam1 );
		if (pRequest != NULL)
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
				"Request %d failed - invalid network path", nParam1);
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, 0, 0,
				"Request %d failed - invalid network path", nParam1);
		break;
	case NM_REQUEST_TIMED_OUT:
		pRequest = requestGetByRequestId( nParam1 );
		if (pRequest != NULL)
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
				"Request %d failed - no response", nParam1);
		else
			DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_EXPLICIT, 0, 0,
				"Request %d failed - no response", nParam1);
		break;
#endif
	case NM_CLIENT_OBJECT_REQUEST_RECEIVED:
		pRequest = requestGetByRequestId( nParam1 );
		if (pRequest != NULL)
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
				"New client request received with Id %d", nParam1);
		else
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, 0, 0,
				"New client request received with Id %d", nParam1);
		break;
#ifndef EIP_NO_CIP_ROUTING
	case NM_BACKPLANE_REQUEST_RECEIVED:
		pRequest = requestGetByRequestId( nParam1 );
		if (pRequest != NULL)
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
				"New backplane request received with Id %d", nParam1);
		else
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, 0, 0,
				"New backplane request received with Id %d", nParam1);
		break;
#endif
#ifdef EIP_PCCC_SUPPORT
	case NM_CLIENT_PCCC_REQUEST_RECEIVED:
		pRequest = requestGetByRequestId( nParam1 );
		if (pRequest != NULL)
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress,
				"New PCCC request received with Id %d", nParam1);
		else
			DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, 0, 0,
				"New PCCC request received with Id %d", nParam1);
		break;
#endif
	case NM_NETWORK_LED_CHANGE:
		DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, 0, 0, "New Network LED value = %d", nParam1);
		break;
	case NM_OUT_OF_MEMORY:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Out of memory");
		break;
	case NM_UNABLE_INTIALIZE_WINSOCK:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Unable to initialize Winsock");
		break;
	case NM_UNABLE_START_THREAD:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Unable to start thread");
		break;
	case NM_ERROR_USING_WINSOCK:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error using Sockets");
		break;
	case NM_ERROR_SETTING_SOCKET_TO_NONBLOCKING:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error setting socket to non-blocking");
		break;
	case NM_ERROR_CREATING_SEMAPHORE:
		DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error creating semaphore");
		break;
	case NM_SESSION_COUNT_LIMIT_REACHED:
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_SESSION, 0, 0, "Session count limit reached");
		break;
	case NM_CONNECTION_COUNT_LIMIT_REACHED:
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, 0, 0, "Connection count limit reached");
		break;
	case NM_PENDING_REQUESTS_LIMIT_REACHED:
		DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_EXPLICIT, 0, 0, "Pending requests limit reached");
		break;
	case NM_ID_RESET:
		DumpStr2(TRACE_LEVEL_WARNING, TRACE_TYPE_EXPLICIT | TRACE_TYPE_MSG, 0, 0, "Identity Instance %d, Reset Type %d", nParam1, nParam2);
		break;
	default:
		break;
	}

#endif /* #ifdef TRACE_OUTPUT */

	if ( gnEvents < MAX_EVENTS )
	{
		pEvent = &gEvents[gnEvents];
		pEvent->nEvent = nEvent;
		pEvent->nParam1 = nParam1;
		pEvent->nParam2 = nParam2;
		gnEvents++;
	}
}

/*---------------------------------------------------------------------------
** notifyService( )
**
** Sends all events from the event array
**---------------------------------------------------------------------------
*/
void notifyService(void)
{
	INT32 i;
	EVENT* pEvent;

	if ( gfnLogEvent )
	{
		for (i = 0, pEvent = gEvents; 
			 i < gnEvents; 
			 i++, pEvent++)
		{
			DumpStr3(TRACE_LEVEL_NOTICE, TRACE_TYPE_MSG, 0, 0, "notifyService Event %d, Param1 %d, Param2 %d",
				pEvent->nEvent, pEvent->nParam1, pEvent->nParam2);

			gfnLogEvent(pEvent->nEvent, pEvent->nParam1, pEvent->nParam2);
		}
	}

	gnEvents = 0;
}

#endif /* EIP_CUSTOM_NOTIFY */


#ifdef ET_IP_SCANNER
/*---------------------------------------------------------------------------
** notifyGetCIPErrorInfo( )
**
** Find a CIP error description based on the error codes and place it in 
** EtIPErrorInfo structure
**---------------------------------------------------------------------------
*/
void notifyGetCIPErrorInfo( EtIPErrorInfo* pErrorInfo )
{
	INT32 nErrMsgId;

	nErrMsgId = notifyGetCIPError( pErrorInfo->bGeneralStatus, pErrorInfo->iExtendedStatus );

	strcpy( (char*)pErrorInfo->errorDescription, gCIPErrorMessages[nErrMsgId].pszError);
}
#endif

#ifndef EIP_NO_ORIGINATE_UCMM
/*---------------------------------------------------------------------------
** notifyGetCIPErrorDescription( )
**
** Find a CIP error description based on the error codes
**---------------------------------------------------------------------------
*/
void notifyGetCIPErrorDescription( UINT8 bGeneralStatus, UINT16 iExtendedStatus, UINT8* szDescription )
{
	INT32 nErrMsgId;

	nErrMsgId = notifyGetCIPError( bGeneralStatus, iExtendedStatus );

	strcpy( (char*)szDescription, (const char*)gCIPErrorMessages[nErrMsgId].pszError);
}

/*---------------------------------------------------------------------------
** notifyGetCIPError( )
**
** Return an error message identifier based on the general and extended
** error codes.
**---------------------------------------------------------------------------
*/
INT32 notifyGetCIPError( UINT8 bGenStatus, UINT16 iExtendedErrorCode )
{
	switch( bGenStatus )
	{
	case ROUTER_ERROR_FAILURE:
		switch( iExtendedErrorCode )
		{
			case ROUTER_EXT_ERR_DUPLICATE_FWD_OPEN:
				return WRN_CONNECTION_IN_USE;

			case ROUTER_EXT_ERR_CLASS_TRIGGER_INVALID:
				return WRN_TRANSPORT_TRIGGER_COMBO_NOT_SUPPORTED;

			case ROUTER_EXT_ERR_OWNERSHIP_CONFLICT:
				return WRN_OWNERSHIP_CONFLICT;

			case ROUTER_EXT_ERR_CONNECTION_NOT_FOUND:
				return WRN_CONNECTION_NOT_FOUND;

			case ROUTER_EXT_ERR_INVALID_CONN_TYPE:
				return WRN_INVALID_CONNECTION_TYPE;

			case ROUTER_EXT_ERR_INVALID_CONN_SIZE:
				return WRN_INVALID_CONNECTION_SIZE;

			case ROUTER_EXT_ERR_DEVICE_NOT_CONFIGURED:
				return WRN_DEVICE_NOT_CONFIGURED;

			case ROUTER_EXT_ERR_RPI_NOT_SUPPORTED:
				return WRN_RPI_NOT_SUPPORTED;

			case ROUTER_EXT_ERR_RPI_VALUE_NOT_ACCEPTABLE:
				return WRN_RPI_VALUE_NOT_ACCEPTABLE;

			case ROUTER_EXT_ERR_CONNECTION_LIMIT_REACHED:
				return WRN_REACHED_MAX_CONNECTIONS;

			case ROUTER_EXT_ERR_VENDOR_PRODUCT_CODE_MISMATCH:
				return WRN_NOT_MATCHED_VENDOR_OR_PRODUCT_CODE;

			case ROUTER_EXT_ERR_PRODUCT_TYPE_MISMATCH:
				return WRN_NOT_MATCHED_PRODUCT_TYPE;

			case ROUTER_EXT_ERR_REVISION_MISMATCH:
				return WRN_NOT_MATCHED_REVISIONS;

			case ROUTER_EXT_ERR_INVALID_CONN_POINT:
				return WRN_INVALID_CONNECTION_POINT;

			case ROUTER_EXT_ERR_INVALID_CONFIG_FORMAT:
				return WRN_INVALID_CONFIGURATION_FORMAT;

			case ROUTER_EXT_ERR_NO_CONTROLLING_CONNECTION:
				return WRN_NO_CONTROLLING_CONNECTION;

			case ROUTER_EXT_ERR_TARGET_CONN_LIMIT_REACHED:
				return WRN_TARGET_NO_MORE_CONNECTIONS;

			case ROUTER_EXT_ERR_RPI_SMALLER_THAN_INHIBIT:
				return WRN_RPI_TOO_SMALL;

			case ROUTER_EXT_ERR_CONNECTION_TIMED_OUT:
				return WRN_CONNECTION_TIMED_OUT;

			case ROUTER_EXT_ERR_UNCONNECTED_SEND_TIMED_OUT:
				return WRN_UNCONNECTED_SEND_TIMED_OUT;

			case ROUTER_EXT_ERR_PARAMETER_ERROR:
				return WRN_INVALID_UNCONNECTED_SEND_PARAMETER;

			case ROUTER_EXT_ERR_MESSAGE_TOO_LARGE:
				return WRN_MESSAGE_TOO_LARGE;

			case ROUTER_EXT_ERR_UNCONN_ACK_WITHOUT_REPLY:
				return WRN_UM_ACKNOWLEDGE_NO_REPLY;

			case ROUTER_EXT_ERR_NO_BUFFER_MEMORY_AVAILABLE:
				return WRN_NO_BUFFER_MEMORY;

			case ROUTER_EXT_ERR_BANDWIDTH_NOT_AVAILABLE:
				return WRN_NO_NETWORK_BANDWIDTH;

			case ROUTER_EXT_ERR_TAG_FILTERS_NOT_AVAILABLE:
				return WRN_NO_TAG_FILTERS;

			case ROUTER_EXT_ERR_REAL_TIME_DATA_NOT_CONFIG:
				return WRN_NOT_CONFIGURED_TO_SEND_REAL_TIME_DATA;

			case ROUTER_EXT_ERR_PORT_NOT_AVAILABLE:
				return WRN_PORT_NOT_AVAILABLE;

			case ROUTER_EXT_ERR_LINK_ADDR_NOT_AVAILABLE:
				return WRN_LINK_ADDR_NOT_AVAILABLE;

			case ROUTER_EXT_ERR_INVALID_SEGMENT_TYPE_VALUE:
				return WRN_INVALID_SEGMENT_TAG;

			case ROUTER_EXT_ERR_PATH_CONNECTION_MISMATCH:
				return WRN_NOT_MATCHED_PATH;

			case ROUTER_EXT_ERR_INVALID_NETWORK_SEGMENT:
				return WRN_SEGMENT_NOT_PRESENT;

			case ROUTER_EXT_ERR_INVALID_LINK_ADDRESS:
				return WRN_INVALID_SELF_ADDRESS;

			case ROUTER_EXT_ERR_SECOND_RESOURCES_NOT_AVAILABLE:
				return WRN_UNAVAILABLE_SECONDARY_RES;

			case ROUTER_EXT_ERR_CONNECTION_ALREADY_ESTABLISHED:
				return WRN_CONNECTION_ALREADY_ESTABLISHED;

			case ROUTER_EXT_ERR_DIRECT_CONN_ALREADY_ESTABLISHED:
				return WRN_DIRECT_CONNECTION_ALREADY_ESTABLISHED;

			case ROUTER_EXT_ERR_MISC:
				return WRN_MISCELLANEOUS;

			case ROUTER_EXT_ERR_REDUNDANT_CONNECTION_MISMATCH:
				return WRN_REDUNDANT_CONNECTION_MISMATCH;

			case ROUTER_EXT_ERR_NO_MORE_CONSUMER_RESOURCES:
				return WRN_NO_MORE_CONSUMER_RESOURCES;

			case ROUTER_EXT_ERR_NO_TARGET_PATH_RESOURCES:
				return WRN_NO_TARGET_PATH_CONNECTION_RESOURCES;

			case ROUTER_EXT_ERR_VENDOR_SPECIFIC:
				return WRN_VENDOR_SPECIFIC;

			case ROUTER_EXT_ERR_TRANSPORT_CLASS_NOT_SUPPORTED:
				return WRN_TRANSPORT_CLASS_NOT_SUPPORTED;

			case ROUTER_EXT_ERR_PRODUCTION_TRIGGER_NOT_SUPPORTED:
				return WRN_PRODUCTION_TRIGGER_NOT_SUPPORTED;

			case ROUTER_EXT_ERR_DIRECTION_NOT_SUPPORTED:
				return WRN_DIRECTION_NOT_SUPPORTED;

			case ROUTER_EXT_ERR_INVALID_OT_FIXED_VARIABLE:
				return WRN_INVALID_OT_FIXED_VARIABLE;

			case ROUTER_EXT_ERR_INVALID_TO_FIXED_VARIABLE:
				return WRN_INVALID_TO_FIXED_VARIABLE;

			case ROUTER_EXT_ERR_INVALID_OT_PRIORITY:
				return WRN_INVALID_OT_PRIORITY;

			case ROUTER_EXT_ERR_INVALID_TO_PRIORITY:
				return WRN_INVALID_TO_PRIORITY;

			case ROUTER_EXT_ERR_INVALID_OT_CONNECTION_TYPE:
				return WRN_INVALID_OT_CONNECTION_TYPE;

			case ROUTER_EXT_ERR_INVALID_TO_CONNECTION_TYPE:
				return WRN_INVALID_TO_CONNECTION_TYPE;

			case ROUTER_EXT_ERR_INVALID_OT_REDUNDANT_OWNER:
				return WRN_INVALID_OT_REDUNDANT_OWNER;

			case ROUTER_EXT_ERR_INVALID_CONFIGURATION_SIZE:
				return WRN_INVALID_CONFIGURATION_SIZE;

			case ROUTER_EXT_ERR_INVALID_OT_SIZE:
				return WRN_INVALID_OT_SIZE;

			case ROUTER_EXT_ERR_INVALID_TO_SIZE:
				return WRN_INVALID_TO_SIZE;

			case ROUTER_EXT_ERR_INVALID_CONFIGURATION_PATH:
				return WRN_INVALID_CONFIGURATION_PATH;

			case ROUTER_EXT_ERR_INVALID_CONSUMING_PATH:
				return WRN_INVALID_CONSUMING_PATH;

			case ROUTER_EXT_ERR_INVALID_PRODUCING_PATH:
				return WRN_INVALID_PRODUCING_PATH;

			case ROUTER_EXT_ERR_NO_CONFIGURATION_SYMBOL:
				return WRN_NO_CONFIGURATION_SYMBOL;

			case ROUTER_EXT_ERR_NO_CONSUME_SYMBOL:
				return WRN_NO_CONSUME_SYMBOL;

			case ROUTER_EXT_ERR_NO_PRODUCE_SYMBOL:
				return WRN_NO_PRODUCE_SYMBOL;

			case ROUTER_EXT_ERR_INVALID_APP_PATH_COMBINATION:
				return WRN_INVALID_APP_PATH_COMBINATION;

			case ROUTER_EXT_ERR_INCONSISTENT_CONSUME_DATA_FORMAT:
				return WRN_INCONSISTENT_CONSUME_DATA_FORMAT;

			case ROUTER_EXT_ERR_INCONSISTENT_PRODUCE_DATA_FORMAT:
				return WRN_INCONSISTENT_PRODUCE_DATA_FORMAT;

			case ROUTER_EXT_ERR_NULL_FORWARD_OPEN_NOT_SUPPORTED:
				return WRN_NULL_FORWARD_OPEN_NOT_SUPPORTED;

			case ROUTER_EXT_ERR_BAD_CONN_TIMEOUT_MULTIPLIER:
				return WRN_BAD_CONN_TIMEOUT_MULTIPLIER;

			case ROUTER_EXT_ERR_MISMATCH_TO_CONN_SIZE:
				return WRN_MISMATCH_TO_CONN_SIZE;

			case ROUTER_EXT_ERR_MISMATCH_TO_FIXED_VAR:
				return WRN_MISMATCH_TO_FIXED_VAR;

			case ROUTER_EXT_ERR_MISMATCH_TO_CONN_PRIORITY:
				return WRN_MISMATCH_TO_CONN_PRIORITY;

			case ROUTER_EXT_ERR_MISMATCH_TRANSPORT_CLASS:
				return WRN_MISMATCH_TRANSPORT_CLASS;

			case ROUTER_EXT_ERR_MISMATCH_TO_PRODUCTION_TRIGGER:
				return WRN_MISMATCH_TO_PRODUCTION_TRIGGER;

			case ROUTER_EXT_ERR_MISMATCH_TO_PRODUCTION_INHIBIT:
				return WRN_MISMATCH_TO_PRODUCTION_INHIBIT;

			}
			return WRN_CONNECTION_FAILURE;

	case ROUTER_ERROR_NO_RESOURCE:
		return WRN_NO_MORE_CONNECTION_MANAGER_RESOURCES;

	case ROUTER_ERROR_INVALID_PARAMETER_VALUE:
		return WRN_INVALID_PARAMETER_VALUE;

	case ROUTER_ERROR_INVALID_SEG_TYPE:
		return WRN_PATH_SEGMENT_ERROR;

	case ROUTER_ERROR_INVALID_DESTINATION:
		return WRN_PATH_DESTINATION_UNKNOWN;

	case ROUTER_ERROR_PARTIAL_DATA:
		return WRN_PARTIAL_TRANSFER;

	case ROUTER_ERROR_CONN_LOST:
		return WRN_LOST_CONNECTION;

	case ROUTER_ERROR_BAD_SERVICE:
		return WRN_SERVICE_NOT_SUPPORTED;

	case ROUTER_ERROR_BAD_ATTR_DATA:
		return WRN_INVALID_ATTRIBUTE_VALUE;

	case ROUTER_ERROR_ATTR_LIST_ERROR:
		return WRN_ATTR_LIST_ERROR;

	case ROUTER_ERROR_ALREADY_IN_REQUESTED_MODE:
		return WRN_ALREADY_IN_REQUESTED_MODE;

	case ROUTER_ERROR_OBJECT_STATE_CONFLICT:
		return WRN_SERVICE_OBJECT_STATE_CONFLICT;

	case ROUTER_ERROR_OBJ_ALREADY_EXISTS:
		return WRN_OBJ_ALREADY_EXISTS;

	case ROUTER_ERROR_ATTR_NOT_SETTABLE:
		return WRN_ATTR_NOT_SETTABLE;

	case ROUTER_ERROR_PERMISSION_DENIED:
		return WRN_PRIVILEGE_VIOLATION;

	case ROUTER_ERROR_DEV_IN_WRONG_STATE:
		return WRN_SERVICE_DEVICE_STATE_CONFLICT;

	case ROUTER_ERROR_REPLY_DATA_TOO_LARGE:
		return WRN_RESPONSE_DATA_TOO_LARGE;

	case ROUTER_ERROR_FRAGMENT_PRIMITIVE:
		return WRN_FRAGMENT_PRIMITIVE;

	case ROUTER_ERROR_NOT_ENOUGH_DATA:
		return WRN_NOT_ENOUGH_DATA_RECEIVED;

	case ROUTER_ERROR_ATTR_NOT_SUPPORTED:
		return WRN_ATTRIBUTE_NOT_SUPPORTED;

	case ROUTER_ERROR_TOO_MUCH_DATA:
		return WRN_TOO_MUCH_DATA_RECEIVED;

	case ROUTER_ERROR_OBJ_DOES_NOT_EXIST:
		return WRN_OBJ_DOES_NOT_EXIST;

	case ROUTER_ERROR_NO_FRAGMENTATION:
		return WRN_NO_FRAGMENTATION;

	case ROUTER_ERROR_DATA_NOT_SAVED:
		return WRN_DATA_NOT_SAVED;

	case ROUTER_ERROR_DATA_WRITE_FAILURE:
		return WRN_DATA_STORE_FAILURE;

	case ROUTER_ERROR_REQUEST_TOO_LARGE:
		return WRN_REQUEST_TOO_LARGE;

	case ROUTER_ERROR_RESPONSE_TOO_LARGE:
		return WRN_RESPONSE_TOO_LARGE;

	case ROUTER_ERROR_MISSING_LIST_DATA:
		return WRN_MISSING_LIST_DATA;

	case ROUTER_ERROR_INVALID_LIST_STATUS:
		return WRN_INVALID_LIST_STATUS;

	case ROUTER_ERROR_SERVICE_ERROR:
		return WRN_EMBEDDED_SERVICE_ERROR;

	case ROUTER_ERROR_VENDOR_SPECIFIC:
		return WRN_VENDOR_SPECIFIC;

	case ROUTER_ERROR_INVALID_PARAMETER:
		return WRN_INVALID_PARAMETER;

	case ROUTER_ERROR_WRITE_ONCE_FAILURE:
		return WRN_WRITE_ONCE_FAILURE;

	case ROUTER_ERROR_INVALID_REPLY:
		return WRN_INVALID_REPLY;

	case ROUTER_ERROR_BAD_KEY_IN_PATH:
		switch( iExtendedErrorCode )
		{
		case ROUTER_EXT_ERR_VENDOR_PRODUCT_CODE_MISMATCH:
			return WRN_NOT_MATCHED_KEYSEG_VENDOR_PRODUCT_CODE;

		case ROUTER_EXT_ERR_PRODUCT_TYPE_MISMATCH:
			return WRN_NOT_MATCHED_KEYSEG_PRODUCT_TYPE;

		case ROUTER_EXT_ERR_REVISION_MISMATCH:
			return WRN_NOT_MATCHED_KEYSEG_REVISIONS;
		}
		return WRN_KEY_FAILURE_IN_PATH;

	case ROUTER_ERROR_BAD_PATH_SIZE:
		return WRN_INVALID_PATH_SIZE;

	case ROUTER_ERROR_UNEXPECTED_ATTR:
		return WRN_UNEXPECTED_ATTR;

	case ROUTER_ERROR_INVALID_MEMBER:
		return WRN_INVALID_MEMBER;

	case ROUTER_ERROR_MEMBER_NOT_SETTABLE:
		return WRN_MEMBER_NOT_SETTABLE;

	case ROUTER_ERROR_UNKNOWN_MODBUS_ERROR:
		return WRN_UNKNOWN_MODBUS_ERROR;

	case ROUTER_ERROR_ENCAP_PROTOCOL:
		return WRN_ENCAP_ERROR;

	default:
		break;
	}

	return WRN_VENDOR_SPECIFIC;
}
#endif
