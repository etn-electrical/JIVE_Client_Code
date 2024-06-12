//[004] 01-Feb-2001
//Contents of NAPDU.H
//	©2000-2010, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------
#ifndef __NAPDU_H_INCLUDED
#define __NAPDU_H_INCLUDED

#include "core.h"

#ifdef _WINDOWS
#ifndef octet
typedef unsigned char octet;
#endif

#ifndef word
typedef unsigned short word;
#endif
#endif

#define	BroadcastDNET			0xFFFF
#define	NSDUisNLmessage			0x80			//1==NL message, 0=APDU
//								0x40			//reserved
#define NSDUhasDNET				0x20			//1==DNET, DLEN, HopCount present
//								0x10			//reserved
#define NSDUhasSNET				0x08			//1==SNET, SLEN present
#define NSDUexpectsreply		0x04
#define NSDUpriority			0x03			//mask for priority field
#define NSDUnormalreply			0x00

enum NALSpriority
	{	NORMAL_MESSAGE,							//0
		URGENT_MESSAGE,							//1
		CRITICAL_EQUIPMENT_MESSAGE,				//2
		LIFE_SAFETY_MESSAGE						//3
	};

enum BACnetNLMessageType
	{	Who_Is_Router_To_Network,				//0
		I_Am_Router_To_Network,					//1
		I_Could_Be_Router_To_Network,			//2
		Reject_Message_To_Network,				//3                                                           
		Router_Busy_To_Network,					//4
		Router_Available_To_Network,			//5
		Initialize_Routing_Table,				//6
		Initialize_Routing_Table_Ack,			//7
		Establish_Connection_To_Network,		//8
		Disconnect_Connection_To_Network,		//9
		NumNLmessageTypes						//10
	};

enum BACnetNLReject
	{	NLREJECT_Other,							//0
		NLREJECT_UnknownDNET,					//1
		NLREJECT_Busy,							//2
		NLREJECT_UnknownNLmessage,				//3                                                           
		NLREJECT_TooLongToRoute					//4
	};

typedef struct _NPCI {							//NPCI
	octet	npciVersion;
	octet	npciControl;
	word	npciDNET;
	octet	npciDLEN;
	octet	npciDADR[8];
	} NPCI;

typedef struct _NSRC {							//NSRC
	word	SNET;
	octet	SLEN;
	octet	SADR[8];
} NSRC;
	
typedef struct _IRTDATA {						//IRTDATA
	word	DNET;
	octet	PortID;
	octet	PortInfoLength;
	} IRTDATA;
	
enum BACnetPDUTypes
	{	CONF_REQ_PDU,							//0
		UNCONF_REQ_PDU,							//1
		SIMPLE_ACK_PDU,							//2
		COMPLEX_ACK_PDU,						//3
		SEGMENT_ACK_PDU,						//4
		ERROR_PDU,								//5
		REJECT_PDU,								//6
		ABORT_PDU								//7
	} ;

#define APDUSIZE50		0x00					//B'0000'         Up to MinimumMessageSize (50 octets)				***405 Begin
#define APDUSIZE128		0x01					//B'0001'         Up to 128 octets
#define APDUSIZE206		0x02					//B'0010'         Up to 206 octets (fits in a LonTalk frame)
#define APDUSIZE480		0x03					//B'0011'         Up to 480 octets (fits in an ARCNET frame)
#define APDUSIZE1024	0x04					//B'0100'         Up to 1024 octets
#define APDUSIZE1476	0x05					//B'0101'         Up to 1476 octets (fits in an ISO 8802-3 frame)
												//B'0110'         reserved by ASHRAE
												//B'0111'         reserved by ASHRAE
												//B'1000'         reserved by ASHRAE
												//B'1001'         reserved by ASHRAE
												//B'1010'         reserved by ASHRAE
												//B'1011'         reserved by ASHRAE
												//B'1100'         reserved by ASHRAE
												//B'1101'         reserved by ASHRAE
												//B'1110'         reserved by ASHRAE
												//B'1111'         reserved by ASHRAE								***405 End



//-------------------------------------------------------------------------------------
//Note:	these ASDU structs assume that segmented PDUs have been filtered out already!
typedef struct _ASDU {	//ASDU
	octet	asduPDUheader;
	octet	asduInvokeID;
	octet	asduServiceAckChoice; 				//ComplexACK
	} ASDU;
	
typedef struct _ASDUREQ {	//ASDU for Confirmed Requests
	octet	asdureqPDUheader;
	octet	asdureqMaxResponse;
	octet	asdureqInvokeID;
	octet	asdureqServiceChoice;
	} ASDUREQ;
//-------------------------------------------------------------------------------------

#define	ASDUpdutype				0xF0			//mask for pdu type field
#define	ASDUpdutypeshr			4				//shift right this many times to get it
#define	ASDUissegmented			0x08			//all segmented types have this bit set
#define ASDUmoresegments		0x04			//1=more segments
#define ASDUsegmentrespaccepted 0x02			//1=segmented responses accepted
#define ASDUnak					0x02			//1=segmented NAK, 0=segmented ACK
#define ASDUserver				0x01			//1=from a server, 0=from a client
#define ASDUMaxResp				0x0F			//mask for max response

enum BACnetAbortReason
	{	ABORT_Other,							//0
		ABORT_BufferOverflow,					//1
		ABORT_InvalidAPDUInThisState,			//2
		ABORT_PreemptedByHigherPriorityTask,	//3
		ABORT_SegmentationNotSupported,			//4
		ABORT_OutOfResources=9					//9
	};
	
enum BACnetRejectReason
	{	REJECT_Other,							//0
		REJECT_BufferOverflow,					//1
		REJECT_InconsistentParameters,			//2
		REJECT_InvalidParameterDataType,		//3
		REJECT_InvalidTag,						//4
		REJECT_MissingRequiredParameter,		//5
		REJECT_ParameterOutOfRange,				//6
		REJECT_TooManyArguments,				//7
		REJECT_UndefinedEnumeration,			//8
		REJECT_UnrecognizedService				//9
	};

enum BACnetConfirmedService
	{
	//Alarm and Event Services
		acknowledgeAlarm,						//0
		confirmedCOVNotification,				//1
		confirmedEventNotification,				//2
		getAlarmSummary,						//3
		getEnrollmentSummary,					//4
		subscribeCOV,							//5
	//File Access Services
		atomicFileRead,							//6
		atomicFileWrite,						//7
	//Object Access Services
		addListElement,							//8
		removeListElement,						//9
		createObject,							//10
		deleteObject,							//11
		readProperty,							//12
		readPropertyConditional,				//13
		readPropertyMultiple,					//14
		writeProperty,							//15
		writePropertyMultiple,					//16
	//Remote Device Management Services
		deviceCommunicationControl,				//17
		confirmedPrivateTransfer,				//18
		confirmedTextMessage,					//19
		reinitializeDevice,						//20
	//Virtual Terminal Services
		vtOpen,									//21
		vtClose,								//22
		vtData,									//23
	//Security Services
		authenticate,							//24
		requestKey,								//25
		readRange,								//26							***210 Begin
		lifeSafetyOperation,					//27
		subscribeCOVProperty,					//28
		getEventInformation						//29							***210 End
	};

enum BACnetUnconfirmedService
	{
		IAm,									//0
		IHave,									//1
		unconfirmedCOVNotification,				//2
		unconfirmedEventNotification,			//3
		unconfirmedPrivateTransfer,				//4
		unconfirmedTextMessage,					//5
		timeSynchronization,					//6
		whoHas,									//7
		whoIs,									//8
		UTCtimeSynchronization					//9								***004
	};
#endif //__NAPDU_H_INCLUDED
