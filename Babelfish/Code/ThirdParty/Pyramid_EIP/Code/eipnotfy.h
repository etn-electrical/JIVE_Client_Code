/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPNOTFY.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Sends notification events the calling application 
**
*****************************************************************************
*/

#ifndef EIPNOTFY_H
#define EIPNOTFY_H

/* CIP protocol specific warnings */
#define _CIPErrorMessages_    \
	ET_IP_MSG( WRN_UNKNOWN_ERROR,							0,	"Unknown CIP Error") \
	ET_IP_MSG( WRN_CONNECTION_IN_USE,						1,	"Connection in use or Duplicate Forward Open") \
	ET_IP_MSG( WRN_TRANSPORT_TRIGGER_COMBO_NOT_SUPPORTED,	2,	"Transport Class and Trigger combination not supported") \
	ET_IP_MSG( WRN_OWNERSHIP_CONFLICT,						3,	"Connection ownership conflict") \
	ET_IP_MSG( WRN_CONNECTION_NOT_FOUND,					4,	"Connection not found at target application") \
	ET_IP_MSG( WRN_INVALID_CONNECTION_TYPE,					5,	"Invalid connection type or connection priority") \
	ET_IP_MSG( WRN_INVALID_CONNECTION_SIZE,					6,	"Invalid connection size") \
	ET_IP_MSG( WRN_DEVICE_NOT_CONFIGURED,					7,	"Device not configured") \
	ET_IP_MSG( WRN_RPI_NOT_SUPPORTED,						8,	"RPI not supported or there is a problem with connection time-out multiplier or production inhibit time") \
	ET_IP_MSG( WRN_REACHED_MAX_CONNECTIONS,					9,	"Connection Manager cannot support any more connections") \
	ET_IP_MSG( WRN_NOT_MATCHED_VENDOR_OR_PRODUCT_CODE,		10,	"Either the Vendor Id or the Product Code in the key segment did not match the device") \
	ET_IP_MSG( WRN_NOT_MATCHED_PRODUCT_TYPE,				11,	"Product Type in the key segment did not match the device") \
	ET_IP_MSG( WRN_NOT_MATCHED_REVISIONS,					12,	"Major or Minor revision information in the key segment did not match the device") \
	ET_IP_MSG( WRN_INVALID_CONNECTION_POINT,				13,	"Invalid connection point specified in the connection path") \
	ET_IP_MSG( WRN_INVALID_CONFIGURATION_FORMAT,			14,	"Invalid configuration format") \
	ET_IP_MSG( WRN_NO_CONTROLLING_CONNECTION,				15,	"Connection request fails since there is no controlling connection currently open") \
	ET_IP_MSG( WRN_TARGET_NO_MORE_CONNECTIONS,				16,	"Target application cannot support any more connections") \
	ET_IP_MSG( WRN_RPI_TOO_SMALL,							17,	"RPI is smaller than the Production Inhibit time") \
	ET_IP_MSG( WRN_CONNECTION_TIMED_OUT,					18,	"Connection timed out") \
	ET_IP_MSG( WRN_UNCONNECTED_SEND_TIMED_OUT,				19,	"Unconnected Send timed out waiting for a response") \
	ET_IP_MSG( WRN_INVALID_UNCONNECTED_SEND_PARAMETER,		20,	"Parameter error in Unconnected Send service") \
	ET_IP_MSG( WRN_MESSAGE_TOO_LARGE,						21,	"Message too large for Unconnected message service") \
	ET_IP_MSG( WRN_UM_ACKNOWLEDGE_NO_REPLY,					22,	"Unconnected acknowledge without reply") \
	ET_IP_MSG( WRN_NO_BUFFER_MEMORY,						23,	"No buffer memory available") \
	ET_IP_MSG( WRN_NO_NETWORK_BANDWIDTH,					24,	"Network bandwidth not available for data") \
	ET_IP_MSG( WRN_NO_TAG_FILTERS,							25,	"No tag filters available") \
	ET_IP_MSG( WRN_NOT_CONFIGURED_TO_SEND_REAL_TIME_DATA,	26,	"Not configured to send real-time data") \
	ET_IP_MSG( WRN_PORT_NOT_AVAILABLE,						27,	"Port specified in port segment not available") \
	ET_IP_MSG( WRN_LINK_ADDR_NOT_AVAILABLE,					28,	"Link Address specified in port segment not available") \
	ET_IP_MSG( WRN_INVALID_SEGMENT_TAG,						29,	"Invalid Segment Type or Segment Value in Path") \
	ET_IP_MSG( WRN_NOT_MATCHED_PATH,						30,	"Path and Connection not equal in close") \
	ET_IP_MSG( WRN_SEGMENT_NOT_PRESENT,						31,	"Either segment not present or encoded value in Network segment is invalid") \
	ET_IP_MSG( WRN_INVALID_SELF_ADDRESS,					32,	"Link Address to Self invalid") \
	ET_IP_MSG( WRN_UNAVAILABLE_SECONDARY_RES,				33,	"Resources on Secondary unavailable") \
	ET_IP_MSG( WRN_CONNECTION_ALREADY_ESTABLISHED,			34,	"Connection already established") \
	ET_IP_MSG( WRN_DIRECT_CONNECTION_ALREADY_ESTABLISHED,	35,	"Direct connection already established") \
	ET_IP_MSG( WRN_MISCELLANEOUS,							36,	"Miscellaneous" ) \
	ET_IP_MSG( WRN_REDUNDANT_CONNECTION_MISMATCH,			37,	"Redundant connection mismatch" ) \
	ET_IP_MSG( WRN_NO_MORE_CONSUMER_RESOURCES,				38,	"No more consumer resources available in the producing object") \
	ET_IP_MSG( WRN_NO_TARGET_PATH_CONNECTION_RESOURCES,		39,	"No connection resources exist for target path" ) \
	ET_IP_MSG( WRN_VENDOR_SPECIFIC,							40,	"Vendor specific error" ) \
	ET_IP_MSG( WRN_NO_MORE_CONNECTION_MANAGER_RESOURCES,	41,	"Resources needed for the object to perform the requested service were unavailable") \
	ET_IP_MSG( WRN_INVALID_PARAMETER_VALUE,					42,	"Invalid Parameter value") \
	ET_IP_MSG( WRN_PATH_SEGMENT_ERROR,						43,	"The path segment identifier or the segment syntax was not understood by the processing node") \
	ET_IP_MSG( WRN_PATH_DESTINATION_UNKNOWN,				44,	"The path is referencing an object class, instance or structure element that is not known or is not contained in the processing node") \
	ET_IP_MSG( WRN_PARTIAL_TRANSFER,						45,	"Only part of the expected data was transferred") \
	ET_IP_MSG( WRN_LOST_CONNECTION,							46,	"The messaging connection was lost") \
	ET_IP_MSG( WRN_SERVICE_NOT_SUPPORTED,					47,	"The requested service was not implemented or was not defined for this object class/instance") \
	ET_IP_MSG( WRN_INVALID_ATTRIBUTE_VALUE,					48,	"Invalid attribute data detected" ) \
	ET_IP_MSG( WRN_ATTR_LIST_ERROR,							49,	"An attribute in the Get_Attribute_List or Set_Attribute_List response has a non-zero status" ) \
	ET_IP_MSG( WRN_ALREADY_IN_REQUESTED_MODE,				50,	"The object is already in the mode/state being requested by the service" ) \
	ET_IP_MSG( WRN_SERVICE_OBJECT_STATE_CONFLICT,			51,	"The object cannot perform the requested service in its current mode/state" ) \
	ET_IP_MSG( WRN_OBJ_ALREADY_EXISTS,						52,	"The requested instance of object to be created already exists" ) \
	ET_IP_MSG( WRN_ATTR_NOT_SETTABLE,						53,	"A request to modify a non-modifiable attribute was received" ) \
	ET_IP_MSG( WRN_PRIVILEGE_VIOLATION,						54,	"A permission/privilege check failed" ) \
	ET_IP_MSG( WRN_SERVICE_DEVICE_STATE_CONFLICT,			55,	"The device's current mode/state prohibits the execution of the requested service" ) \
	ET_IP_MSG( WRN_RESPONSE_DATA_TOO_LARGE,					56,	"The data to be transmitted in the response buffer is larger than the allocated response buffer" ) \
	ET_IP_MSG( WRN_FRAGMENT_PRIMITIVE,						57,	"The service specified an operation that is going to fragment a primitive data value, i.e. half a REAL data type" ) \
	ET_IP_MSG( WRN_NOT_ENOUGH_DATA_RECEIVED,				58,	"The service did not supply enough data to perform the specified operation" ) \
	ET_IP_MSG( WRN_ATTRIBUTE_NOT_SUPPORTED,					59,	"The attribute specified in the request is not supported" ) \
	ET_IP_MSG( WRN_TOO_MUCH_DATA_RECEIVED,					60,	"The service supplied more data than was expected") \
	ET_IP_MSG( WRN_OBJ_DOES_NOT_EXIST,						61,	"The object specified does not exist in the device") \
	ET_IP_MSG( WRN_NO_FRAGMENTATION,						62,	"The fragmentation sequence for this service is not currently active for this data") \
	ET_IP_MSG( WRN_DATA_NOT_SAVED,							63,	"The attribute data of this object was not saved prior to the requested service") \
	ET_IP_MSG( WRN_DATA_STORE_FAILURE,						64,	"The attribute data of this object was not saved due to a failure during the attempt") \
	ET_IP_MSG( WRN_REQUEST_TOO_LARGE,						65,	"The service request packet was too large for transmission on a network in the path to the destination") \
	ET_IP_MSG( WRN_RESPONSE_TOO_LARGE,						66,	"The service response packet was too large for transmission on a network in the path to the destination") \
	ET_IP_MSG( WRN_MISSING_LIST_DATA,						67,	"The service did not supply an attribute in a list of attributes that was needed by the service to perform the requested behaviour") \
	ET_IP_MSG( WRN_INVALID_LIST_STATUS,						68,	"The service is returning the list of attributes supplied with status information for those attributes that were invalid") \
	ET_IP_MSG( WRN_EMBEDDED_SERVICE_ERROR,					69,	"An embedded service resulted in an error") \
	ET_IP_MSG( WRN_INVALID_PARAMETER,						70,	"A parameter associated with the request was invalid") \
	ET_IP_MSG( WRN_WRITE_ONCE_FAILURE,						71,	"An attempt was made to write to a write-once medium that has already been written, or to modify a value that cannot be changed once established") \
	ET_IP_MSG( WRN_INVALID_REPLY,							72,	"An invalid reply is received (e.g. reply service code does not match the request service code, or reply message is shorter than the minimum expected reply size).") \
	ET_IP_MSG( WRN_KEY_FAILURE_IN_PATH,						73,	"The Key segment that was included as the first segment in the path does not match the destination module") \
	ET_IP_MSG( WRN_NOT_MATCHED_KEYSEG_VENDOR_PRODUCT_CODE,	74,	"Invalid Vendor Id or Product Code specified in the Key segment") \
	ET_IP_MSG( WRN_NOT_MATCHED_KEYSEG_PRODUCT_TYPE,			75,	"Invalid Product Type specified in the Key segment" ) \
	ET_IP_MSG( WRN_NOT_MATCHED_KEYSEG_REVISIONS,			76,	"Invalid Major or Minor revision specified in the Key segment" ) \
	ET_IP_MSG( WRN_INVALID_PATH_SIZE,						77,	"The size of the path which was sent with the Service Request is either not large enough to allow the request to be routed or too much routing data was included" ) \
	ET_IP_MSG( WRN_UNEXPECTED_ATTR,							78,	"An attempt was made to set an attribute that is not able to be set at this time" ) \
	ET_IP_MSG( WRN_INVALID_MEMBER,							79,	"The Member ID specified in the request does not exist in the specified Class/Instance/Attribute" ) \
	ET_IP_MSG( WRN_MEMBER_NOT_SETTABLE,						80,	"A request to modify a non-modifiable member was received" ) \
	ET_IP_MSG( WRN_CONNECTION_FAILURE,						81,	"A connection related service failed along the connection path" ) \
	ET_IP_MSG( WRN_ENCAP_ERROR,								82,	"An error in the encapsulation header" ) \
	ET_IP_MSG( WRN_UNKNOWN_MODBUS_ERROR,					83, "An unknown Modbus error occured") \
	ET_IP_MSG( WRN_RPI_VALUE_NOT_ACCEPTABLE,				84,	"RPI not acceptable.") \
	ET_IP_MSG( WRN_TRANSPORT_CLASS_NOT_SUPPORTED,			85,	"The transport class requested in the Transport Type/Trigger parameter is not supported") \
	ET_IP_MSG( WRN_PRODUCTION_TRIGGER_NOT_SUPPORTED,		86,	"The production trigger requested in the Transport Type/Trigger parameter is not supported") \
	ET_IP_MSG( WRN_DIRECTION_NOT_SUPPORTED,					87,	"The direction requested in the Transport Type/Trigger parameter is not supported") \
	ET_IP_MSG( WRN_INVALID_OT_FIXED_VARIABLE,				88,	"O->T fixed/variable flag is not supported") \
	ET_IP_MSG( WRN_INVALID_TO_FIXED_VARIABLE,				89,	"T->O fixed/variable flag is not supported") \
	ET_IP_MSG( WRN_INVALID_OT_PRIORITY,						90,	"O->T priority code is not supported") \
	ET_IP_MSG( WRN_INVALID_TO_PRIORITY,						91,	"T->O priority code is not supported") \
	ET_IP_MSG( WRN_INVALID_OT_CONNECTION_TYPE,				92,	"O->T connection type is not supported") \
	ET_IP_MSG( WRN_INVALID_TO_CONNECTION_TYPE,				93,	"T->O connection type is not supported") \
	ET_IP_MSG( WRN_INVALID_OT_REDUNDANT_OWNER,				94,	"O->T Redundant Owner flag is not supported") \
	ET_IP_MSG( WRN_INVALID_CONFIGURATION_SIZE,				95,	"The data segment provided in the Connection_Path parameter did not contain an acceptable number of 16-bit words for the the configuration application path requested") \
	ET_IP_MSG( WRN_INVALID_OT_SIZE,							96,	"Cannot support the size requested in the O->T Network Connection Parameter") \
	ET_IP_MSG( WRN_INVALID_TO_SIZE,							97,	"Cannot support the size requested in the T->O Network Connection Parameter") \
	ET_IP_MSG( WRN_INVALID_CONFIGURATION_PATH,				98,	"The configuration application path specified does not correspond to a valid configuration application path") \
	ET_IP_MSG( WRN_INVALID_CONSUMING_PATH,					99,	"The consumed application path specified does not correspond to a valid consumed application path") \
	ET_IP_MSG( WRN_INVALID_PRODUCING_PATH,					100,"The produced application path specified does not correspond to a valid produced application path") \
	ET_IP_MSG( WRN_NO_CONFIGURATION_SYMBOL,					101,"Configuration Symbol does not exist") \
	ET_IP_MSG( WRN_NO_CONSUME_SYMBOL,						102,"Consuming Symbol does not exist") \
	ET_IP_MSG( WRN_NO_PRODUCE_SYMBOL,						103,"Producing Symbol does not exist") \
	ET_IP_MSG( WRN_INVALID_APP_PATH_COMBINATION,			104,"The combination of configuration and/or consume and/or produce application paths specified are inconsistent with each other") \
	ET_IP_MSG( WRN_INCONSISTENT_CONSUME_DATA_FORMAT,		105,"Information in the data segment is not consistent with the format of the consumed data") \
	ET_IP_MSG( WRN_INCONSISTENT_PRODUCE_DATA_FORMAT,		106,"Information in the data segment is not consistent with the format of the produced data") \
	ET_IP_MSG( WRN_NULL_FORWARD_OPEN_NOT_SUPPORTED,			107,"The target does not support the function requested by the Null Forward Open") \
	ET_IP_MSG( WRN_BAD_CONN_TIMEOUT_MULTIPLIER,				108,"Connection timeout multiplier value is reserved or produces a timeout value that is too large to support in the device") \
	ET_IP_MSG( WRN_MISMATCH_TO_CONN_SIZE,					109,"Mismatched connection size for existing multicast connection") \
	ET_IP_MSG( WRN_MISMATCH_TO_FIXED_VAR,					110,"Mismatched fixed/variable connection type for existing multicast connection") \
	ET_IP_MSG( WRN_MISMATCH_TO_CONN_PRIORITY,				111,"Mismatched fixed/variable connection priority for existing multicast connection") \
	ET_IP_MSG( WRN_MISMATCH_TRANSPORT_CLASS,				112,"Mismatched transport class for existing multicast connection") \
	ET_IP_MSG( WRN_MISMATCH_TO_PRODUCTION_TRIGGER,			113,"Mismatched production trigger for existing multicast connection") \
	ET_IP_MSG( WRN_MISMATCH_TO_PRODUCTION_INHIBIT,			114,"Mismatched production inhibit timer for existing multicast connection") \

#define CIP_ERROR_CODES				114		/* Number of available CIP error codes */

#undef ET_IP_MSG
#define ET_IP_MSG( name, code, string )  name = code,

/* Build the enumeration from the specified CIP error codes */
typedef enum tagCIPErrorCode
{
	_CIPErrorMessages_
}
CIPErrorCode;

/* Associate event code with the event text */
typedef struct tagERROR_STRING
{
	char *pszError;
}
ERROR_STRING;

/* Structure used to pass around UCMM or Class3 information when parsing the request */
typedef struct tagEVENT
{
	INT32 nEvent;		/* Event type */
	INT32 nParam1;		/* Event parameter 1 */
	INT32 nParam2;		/* Event parameter 2 */
}
EVENT;

#define MAX_EVENTS						256		/* Maximum number of outstanding events */

extern LogEventCallbackType* gfnLogEvent;/* Client callback function */

EXTERN void notifyInit(void);
EXTERN void notifyEvent( INT32 nEvent, INT32 nParam1, INT32 nParam2 );
EXTERN void notifyService(void);

#ifndef EIP_NO_ORIGINATE_UCMM
extern void notifyGetCIPErrorDescription( UINT8 bGeneralStatus, UINT16 iExtendedStatus, UINT8* szDescription );
#endif
extern INT32 notifyGetCIPError( UINT8 bGenStatus, UINT16 iExtendedErrorCode );

#ifdef ET_IP_SCANNER
extern void notifyGetCIPErrorInfo( EtIPErrorInfo* pErrorInfo );
#endif


#endif /* #ifndef EIPNOTFY_H */
