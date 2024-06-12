//Contents of VSBHP.H
//	©2000-2014, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------
//[1208] 07-Oct-2014 DMF

#ifndef __VSB_H_INCLUDED
#define __VSB_H_INCLUDED

#include "core.h"
#include "mac.h"
#include "semaphore.h"
#include "npool.h"
#include "enums.h"

#define	whoismoratorium 15						//how often we can ranged Who-Is the same guy in secs
#define noobject		0xFFFFFFFFL
#define noindex			0xFFFFFFFFL
#define nodevice		0xFFFFFFFFL
#define noproperty		0xFFFFFFFFL				//								***1206
#define noincrement		0xFFFFFFFFL				//								***1206
#define norange			0xFF					//								***400
#define notheardfrom	0xFF
#define nullvalue		0xFFFFFFFFL
#define unknowninvokeid	0xFFFF
#define broadcast		0xFFFFFFFFL
#define noinvokeid		-1
#define wildcardinst	4194303					//								***400
#define globalnetwork	0xFFFF					//								***1208


#define maxobjectlist	93						//this is limited by the APDU size for mstp with no segmentation
#define maxdevices		32						//								***002
#define maxcopropstring	66						//max size for CreateObject string properties (must be less than 253!) ***1205
#define maxmsvstates	64						//max states for an MSV object	***1205
#define maxcptblocklen	((maxobjectlist-2)*4)	//max length of conf private transfer block	***1205
#define stringarraydelimiter	'|'				//								***1205
#define charstringarraydelimiter ','			//								***1218

#define MAXLOGRECORDS	20						//								***400 Begin
#define MAXDOPR			23
#define MAXDOR			46
#define MAXDESTS		5
#define MAXCALENTRY		20
#define MAXTIMEVAL		24	
#define MAXSPECIALEVNTS 5						//								***400 End
#define MAXSPECEVNTTV	10						//								***407
#define MAXCOVSUBS		10						//								***409
#define MAXRECIPS		23						//								***1200
#define MAXEVENTLOGRECORDS	3					//								***1206


//#ifndef false
//#define false		0
//#endif
//
//#ifndef true
//#define true		1
//#endif

#ifndef FALSE
#define FALSE		false
#endif

#ifndef TRUE
#define TRUE		true
#endif

#ifndef NULL
#define NULL		0
#endif

typedef struct	_frBACnetdevice {
	dword	devinst;				//the device instance
	word	maxlen;					//max APDU len accepted
	word	vendorid;				//the vendor id
	word	timer;					//Who-Is timer								***240
	frSource src;					//the source address info
} frBACnetdevice;

enum AppDatatypes								//note: these correspond directly to BACnet app datatypes!
	{	adtNull,           						//0
		adtBoolean, 							//1
		adtUnsigned,     						//2
		adtInteger,  							//3
		adtReal,								//4
		adtDouble,								//5
		adtOctetString,							//6
		adtCharString,							//7
		adtBitString,							//8
		adtEnumerated,							//9
		adtDate,								//10
		adtTime,								//11
		adtObjectID,							//12
		adtObjectList=16,						//16
		adtEmptyList=17,						//17
		adtEventTimeStamp=18,					//18										***400
		adtRecipientAddr=19,					//19										***1200
		adtPropRef=20,							//20										***224
		adtOptPropRef=21,						//21										***224
		adtLogBuffer=22,						//22										***400
		adtDateTimeStamp=23,					//23										***400
		adtDeviceObjectPropertyReference=24,	//24										***400
		//adtUnsignedArray=25,					//25		added for clarity				***400
		adtRecipientList=26,					//26										***400
		adtDateList=27,							//27										***400
		adtEffectivePeriod=28,					//28										***401
		adtTimeValue=29,						//29										***401		
		adtSpecialEvent=30,						//31										***401
		adtTimeStamp=32,						//32										***1200
		adtCOVSubscription=37,					//37										***409
		adtDeviceObjectReference=38,			//38										***600
		adtScale=39,							//39										***603
		adtObjPropRef=40,						//40										***1205 Begin
		adtStringArray=41,						//41
		adtUnsignedList=42,						//42
		adtPrivateTransfer=43,					//43										***1205 End
		adtEventLogBuffer=44,					//44										***1206
		adtDateTime=45,							//45										***1206
		adtSACK=254,							//254										***220
		adtError=255							//255
	};

typedef struct _BACnetDestination {		
	octet validDays;					//BACnetDaysOfWeek,	
	frTime fromTime;					//Time,
	frTime toTime;						//Time,
	frBACnetRecipient recipient;		//BACnetRecipient,
	dword processIdentifier;			//Unsigned32,
	bool issueConfirmedNotifications;	//BOOLEAN,
	octet transitions;					//BACnetEventTransitionBits
} frBACnetDestination;

typedef struct _frLogRecord {			//	***400 Begin	
	frDateTimeStamp timestamp;			//the timestamp
	octet datatype;						//0..9 (correspond to the tags, too), no ANY in this stack
										//0=BACnetLogStatus (bitstring)
										//1=boolean
										//2=real
										//3=enumerated
										//4=unsigned
										//5=signed
										//6=bitstring
										//7=null
										//8=error
										//9=time change (real)
	octet sigbits;      				//how many significant bits in the bitstring value
	union _logDatum {		
		octet logstatus;				//bitstring BACnetLogStatus, 3 significant bits
		bool bval;						//boolean value
#ifdef FLOATLIB							//include float lib
		float	fval;					//real value
#else
		dword	fval;
#endif
		dword eval;						//32 bit enumeration
		dword uval;						//32 bit unsigned
		long sval;						//32 bit signed value
		octet bitval[4];				//32 bit bitstring
		bool nullval;					//null value, set true for null 
		dword failure[2];				//error
#ifdef FLOATLIB							//include float lib
		float	timechange;				//real value
#else
		dword	timechange;
#endif
	} logDatum;
} frBACnetLogRecord;

typedef struct _frDeviceObjPropRef { 
	dword		objid;					//the object id
	dword		propid;					//the property id
	dword		arrayindex;				//the array index (may be noindex)
	dword		deviceid;				//the device object id (may be noobject)
} frDeviceObjPropRef;	

typedef struct _frEventLogRecord {		//													***1206 Begin
	byte			elrType;			//0=logstatus, 1=notification, 2=timechange
	byte			elrStatus;			//log status when elrtype==0
	byte			nc;					//notification class when elrType==1
	byte			priority;			//when elrType==1
	byte			eventType;			//when elrType==1
	byte			ackReqd;			//when elrType==1
	byte			fromState;			//when elrType==1
	byte			toState;			//when elrType==1
	frDateTimeStamp timestamp;			//the timestamp
#ifdef FLOATLIB							//include float lib
	float			timechange;			//when elrType==2
#else
	dword			timechange;
#endif
	dword			processID;
	dword			eventObjectID;
	frString		msgh;				//string header for alarm message
	char			msg[72];			//space for alarm message
//Note that this union is much SMALLER than the general case in frEventNotification!
	union	_npel {						//note: 16 bytes max!
		struct	{
			byte	usedbits;
			byte	bitstring[9];
			byte	StatusFlags;		//BACnet bit-order
										// 0=in-alarm
										// 1=fault
										// 2=overridden
										// 3=out-of-service
			} COB;						//change-of-bitstring
		struct	{
			byte	newstate;			//enumeration value
			byte	StatusFlags;
			word	propertystates;		//which enumeration is it?
			} COS;						//change-of-state
		struct	{
			byte	usedbits;
			byte	changedbitstring[9];
			byte	valueorbits;	//0=bits, 1=value
			byte	StatusFlags;
#ifdef FLOATLIB
			float	changedvalue;
#else
			dword	changedvalue;
#endif
			} COV;					//change-of-value
		struct	{
			dword	commandvalue;
			dword	feedback;
			byte	StatusFlags;
			} CF;					//command-failure
		struct	{
#ifdef FLOATLIB
			float	refvalue;
			float	setpointvalue;
			float	errorlimit;
#else
			dword	refvalue;
			dword	setpointvalue;
			dword	errorlimit;
#endif
			byte	StatusFlags;
			} FL;					//floating-limit
		struct	{
#ifdef FLOATLIB
			float	exceedingvalue;
			float	deadband;
			float	exceededlimit;
#else
			dword	exceedingvalue;
			dword	deadband;
			dword	exceededlimit;
#endif
			byte	StatusFlags;
			} OOR;					//out-of-range
#ifdef ESP32_SETUP
		struct {						//													***1207 Begin
			frDeviceObjPropRef bufferProperty;
			dword	previousNotification;
			dword	currentNotification;
			} THPBR;						//													***1207 End
			/*Changing struct name from BR to THPBR cos ESP32 is having special register name as 'BR'*/
#else
        struct {						//													***1207 Begin
			frDeviceObjPropRef bufferProperty;
			dword	previousNotification;
			dword	currentNotification;
			} BR;						//													***1207 End
#endif   		// ESP32_SETUP	
		}npel;
} frBACnetEventLogRecord;				//													***1206 End

typedef struct _frDeviceObjRef {													//		***600 Begin
	dword		objid;					//the object id
	dword		deviceid;				//the device object id (may be noobject)
} frDeviceObjRef;																	//		***600 End

typedef struct _frRange {
	byte			rangetype;			//0=byPosition, 
										//1=byTime (old)
										//2=timerange 
										//3=bySequence
										//4=byTime (Addendum b)
										//0xFF no range	
	byte			pad[3];
	dword			refindex;			//only needed if rangetype=0,3
	long			count;				//only needed if rangetype=0,1,4
	dword			firstseq;			//only if 3 and 4 (0xFFFFFFFF=none)
	frDateTimeStamp	stime;				//needed for rangetype=1,2,4
	frDateTimeStamp	etime;				//needed for rangetype=2;
} frRange;								

typedef struct _frBACnetDateRange {
	frDate	start;
	frDate	end;
} frBACnetDateRange;

typedef struct _DateRangeDaysOfWeek {	//													***1206 Begin
	frDate	StartDate;
	frDate	EndDate;
	octet	DaysOfWeek;  
} DateRangeDaysOfWeek;					//													***1206 End

typedef struct _frBACnetCalendarEntry {
	byte				type;			//0=date
										//1=BACnetDateRange
										//2=BACnetWeekNDay
										//14= DateRangeDaysOfWeek							***1206
	frDate				dt;
	frBACnetDateRange	dr;
	octet				wnd[3];
	DateRangeDaysOfWeek  drdow;			//													***1206
} frBACnetCalendarEntry;				//	***400 End		

typedef struct _frScheduleAny {			//	***401 Begin
	byte	pdtype;						//datatype
										//any of adtUnsigned,adtReal,adtBoolean,adtEnumeration,
										//or     adtInteger,adtDouble,adtNull,adtBitString
	byte sigbits;						//number of significant bits for a bitstring
	union _schedpd {
		octet	psval[4];				//bitstring value (up to 32 bits)
		dword	uval;					//unsigned value (boolean, Unsigned, enum
		long	sval;					//signed value (integer)
#ifdef FLOATLIB							//include float lib
		float	fval;					//real
		double	dval;					//double
#else									//exclude float lib
		dword	fval;
		dword	dval[2];
#endif
		frTime	tval;					//time
		frDate	dtval;					//date	
	}	schedpd;
} frScheduleAny;

typedef struct _frBACnetTV_val
{
	byte pdtype;						//datatype
										//any of adtUnsigned,adtReal,adtBoolean,adtEnumeration,
										//or     adtInteger,adtDouble,adtNull,adtBitString
	byte sigbits;						//number of significant bits for a bitstring
	union _tvpd {
		octet	psval[4];				//bitstring value
		dword	uval;					//unsigned value (boolean, Unsigned, enum)										
		long	sval;					//signed value (integer)
#ifdef FLOATLIB							//include float lib
		float	fval;					//real
		double	dval;					//double
#else									//exclude float lib
		dword	fval;
		dword	dval[2];
#endif		
	}	tvpd;
} frBACnetTV_val;

typedef struct _frBACnetTV {
	frTime time;
	frBACnetTV_val value;	
} frBACnetTV;

typedef struct _frBACnetTimeValue {	
	octet day;							//the day it's associated with, 0xFF is empty
	frBACnetTV tv;
} frBACnetTimeValue;					

typedef struct _frBACnetSpecialEvent {
	octet caltype;						//0=BACnetCalendarEntry, 1=calendarrefernce
	frBACnetCalendarEntry calendarEntry;
	dword calendarreference;
	octet numvalues;					//number of time value pairs
	frBACnetTV tv[MAXSPECEVNTTV];		//up to 10						//	***407
	dword eventPriority;				//1..16
} frBACnetSpecialEvent;													//	***401 End

typedef struct _frObjPropRef {		//an object property reference				***409 Begin
	dword		objid;				//the objectid
	dword		propid;				//the property id
	dword		arrayindex;			//the array index (may be noindex)
	dword		listelement;		//which list element this is
} frObjPropRef;

typedef struct _frAddress {			//to transfer BACnetAddressBinding to VSB
	word		network;			//the network number (always big-endian)
	word		noctets;			//the actual number of octets to follow
	octet		address[8];			//the address
} frAddress;

typedef struct _frRecipientAddr {	//to transfer BACnetAddressBinding/BACnetRecipient to VSB
	byte	type;					//0=device,1=address
	byte	spare[3];
	union	{
		dword		device;			//the device objectid
		frAddress	addr;			//the address
		} recipient;				//the recipient address
} frRecipientAddr;

typedef struct _frCOVSubscription {
	frRecipientAddr		recip;		//recipient process address	16 bytes
	dword				procid;		//recipient process id		4 bytes
	frObjPropRef		propref;	//monitored prop ref		16 bytes
	byte				issue;		//issue confirmed notifications (0=no, 1=yes)
	byte				hasincrement;	//0=no,1=yes	
	byte				pad[2];
	dword				timeremaining;
#if (FLOATLIB)
	float				increment;	//cov increment (opt)
#else
	dword				increment;
#endif
}frCOVSubscription;															//	***409 End

typedef struct _frCOprop {													//	***1205 Begin
	byte		pdtype;					//datatype see AppDatatypes
	byte		propid;					//only handle propids<256
	union	{
		byte	pb;						//bool, transbits, notifytype, limenable, numstates
		dword	pd;
#ifdef FLOATLIB							//include float lib
		float	pf;						//real
#else									//exclude float lib
		dword	pf;
#endif
		byte	plou[maxcopropstring];
		char	ps[maxcopropstring];
frObjPropRef	popr;
			} iv;						//initial value by datatype
}frCOprop;								//										***1205 End

typedef struct _frConfPrivateTransfer {	//										***1205 Begin
	word	vendorid;					//the vendor ID
	word	blocklen;					//length of used result block
	dword	service;					//private service code
	byte	block[maxcptblocklen];		//block of extra results
}frConfPrivateTransfer;					//										***1205 End

typedef struct _frVbag {
	dword	objid;						//object identifier						***1205
	dword	propid;						//property ID							***1205
	dword	arrindex;					//array index							***1205
	byte	status;						//status see vbStates
	byte	pdtype;						//datatype see AppDatatypes
	byte	priority;					//priority for writing 1..16, always 0 for reading!
	byte	narray;						//size of array (0=not an array)
	union _pd {
		dword	uval;					//unsigned value (boolean, Unsigned, enum, objid)
		//or length if octet (in bytes)/bit (bits) string
		long	sval;					//signed value (integer)
#ifdef FLOATLIB							//include float lib
		float	fval;					//real
		double	dval;					//double
#else									//exclude float lib
		dword	fval;
		dword	dval[2];
#endif
		frTime	tval;					//time
		frDate	dtval;					//date
		frString stval;					//character string
		int		errval;					//error value
	}	pd;
	union	_ps {
		byte	bits;
		byte	psval[maxobjectlist*4];	//string bytes (also csv string arrays)
		dword	psdval[maxobjectlist];	//dword array (also object list objectids)
#ifdef FLOATLIB							//include float lib
		float	psfval[maxobjectlist];	//real array
#else
		dword	psfval[maxobjectlist];
#endif
		frConfPrivateTransfer	pscpt;					//														***1205
		frScale					psscale;				//														***603
		frObjectPropRef			pspropref;				//														***224
		frDateTimeStamp			psdts[3];				//event_time_stamps
		frTimeStamp				ts;						//timestamp												***1200
		frBACnetLogRecord		pslr[MAXLOGRECORDS];	//((maxobjectlist*4)/sizeof(frBACnetLogRecord))=20		***400
		frBACnetEventLogRecord	pselr[MAXEVENTLOGRECORDS];	//((maxobjectlist*4)/sizeof(frBACnetEventLogRecord))=3  ***1206
		frDeviceObjPropRef		psdopr[MAXDOPR];		//((maxobjectlist*4)/sizeof(frDeviceObjPropRef))=23		***400
		frDeviceObjRef			psdor[MAXDOR];			//
		frBACnetDestination		psdest[MAXDESTS];		//recipient list --> we can only hold 5					***400
		frBACnetCalendarEntry	pscal[MAXCALENTRY];		//((maxobjectlist*4)/sizeof(frBACnetCalendarEntry))=23	***400		
		frBACnetTimeValue		tv[MAXTIMEVAL];			//time values											***400
		frBACnetDateRange		ep;						//an effective period									***400
		frBACnetSpecialEvent	se[MAXSPECIALEVNTS];	//special events										***400
		frCOVSubscription		cs[MAXCOVSUBS];			//((maxobjectlist*4)/sizeof(frCOVSubscription))=7		***409
		frBACnetRecipient		ra[MAXRECIPS];			//((maxobjectlist*4)/sizeof(frBACnetRecipient))=23		***1200
	}	ps;
	// Note: In the ps union:
	//		1. if pdtype==adtCharString and narray==0 then pd.stval and ps.psval has a string
	//		2. if pdtype==adtCharString and narray!=0 then pd.stval and ps.psval has a string array with narray values separated by commas
	//		3. if pdtype==adtEnumerated or adtReal and narray==0 then ps.psval is not used and the value is in pd 
	//		4. if pdtype==adtEnumerated or adtReal and narray!=0 then ps.psdval and ps.psfval has narray values
	//		   NULL values are represented by the value=nullvalue in the array
	
} frVbag;

typedef struct _frAlarmSummary {		//to transfer AlarmSummaries to VSB
	dword		objid;					//object id
	word		eventstate;				//enumerated event state
	byte		transitionsacked;		//bitmap in BACnet order (3 bits)
} frAlarmSummary;

typedef struct _frEventSummary {		//to transfer Addendum C EventSummaries		***004
	dword		objid;					//object id
	word		eventstate;				//enumerated event state
	byte		transitionsacked;		//bitmap in BACnet order (3 bits)
	byte		transitionbits;			//bitmap in BACnet order (3 bits)
	word		notifytype;				//the notify type (0=alarm,1=event,2=ack-notification)
	frTimeStamp	timestamp[3];			//sequence of time stamps
	dword		eventpriorities[3];
} frEventSummary;

typedef struct	_frAckAlarm {			//to transfer AckAlarmRequests from VSB
	dword		objid;					//object id
	dword		processid;				//process id
	dword		eventstate;				//event state being ack'd (an enumeration)
	frTimeStamp	timestamp;				//time from the event notification
	frTimeStamp	acktimestamp;			//time of acknowledgement
	frString	whoackedstring;
	char		whoacked[254];			//	***1200		
} frAckAlarm;

typedef struct _frCOV {					//to transfer Unsubscribed COVs to/from VSB
	dword		processid;				//this is for a specific process			***1206
	dword		deviceinst;				//the initiating device instance
	dword		objid;					//the monitored object id
	word		numberpvs;				//the actual number of propvals to follow
	frPropVal	propval[8];				//the property values (limited to 8)
} frCOV;

typedef struct _frSubscribeCOV {		//to transfer SubscribeCOVs to/from VSB
	octet		choice;					//0=device id, 1=MAC (more common)			***1200
	dword		devid;					//destination device id						***1200
	frSource	src;					//the source address of the message
	dword		processid;				//the subscriber's processid
	dword		deviceid;				//the initiating device object id
	dword		objid;					//the monitored object id
	dword		time;					//from VSB=lifetime, to VSB=time remaining in seconds
	bool		cancel;					//T/F
	bool		confirmed;				//T/F
	word		numberpvs;				//the actual number of propvals to follow
	frPropVal	propval[8];				//the property values (limited to 8)
} frSubscribeCOV;

typedef struct frSubscribeCOVProperty {	//to transfer SubscribeCOVs to/from VSB		***1223 Begin
	octet		choice;					//0=device id, 1=MAC (more common)
	dword		devid;					//destination device id
	frSource	src;					//the source address of the message
	dword		processid;				//the subscriber's processid
	dword		deviceid;				//the initiating device object id
	dword		objid;					//the monitored object id
	dword		propid;					//the monitored property id
	dword		time;					//from VSB=lifetime, to VSB=time remaining in seconds
#ifdef FLOATLIB							//include float lib
	float		cov_increment;			//real	
#else									//exclude float lib
	dword		cov_increment;	
#endif
	bool		hasincrement;			//T/F
	bool		cancel;					//T/F
	bool		confirmed;				//T/F
	word		numberpvs;				//the actual number of propvals to follow
	frPropVal	propval[8];				//the property values (limited to 8)
} frSubscribeCOVProperty;				//											***1223 End

typedef struct	_frEventNotification{	//to transfer an event notification to VSB
	byte		notifytype;			//0=alarm, 1=event, 2=ack-notification
	byte		confirmed;			//0=unconfirmed, 1=confirmed notification
	byte		priority;
	byte		alarmackrequired;	//0=no, 1=yes
	//dword		destdevice;			//destination device (0xFFFFFFFF=broadcast)
	frBACnetRecipient dest;
	dword		processid;			//process id
	dword		initdeviceinst;		//initiating device instance
	dword		objid;				//object id
	frTimeStamp	timestamp;			//time of the event notification
	dword		notificationclass;
	word		eventtype;			//the event type that has occurred, see eventnotificationtypes
	word		fromstate;			//the event state prior to the alarm/event
									// 0=normal, 1=fault, 2=offnormal, 
									// 3=high-limit, 4=low-limit
	word		tostate;			//the event state after the alarm/event
	frString 	msgh;				//header of message text for the event
	char		msg[128];			//place to put message for the event
	union	_npu {
		struct	{
			word	usedbits;
			byte	bitstring[14];
			byte	StatusFlags;	//BACnet bit-order
									// 0=in-alarm
									// 1=fault
									// 2=overridden
									// 3=out-of-service
			} COB;					//change-of-bitstring
		struct	{
			dword	newstate;		//enumeration value
			word	propertystates;	//which enumeration is it?
			byte	StatusFlags;
			} COS;					//change-of-state
		struct	{
			word	usedbits;
			byte	changedbitstring[14];
#ifdef FLOATLIB
			float	changedvalue;
#else
			dword	changedvalue;
#endif
			byte	valueorbits;	//0=bits, 1=value
			byte	StatusFlags;
			} COV;					//change-of-value
		struct	{
			dword	commandvalue;
			dword	feedback;
			byte	StatusFlags;
			} CF;					//command-failure
		struct	{
#ifdef FLOATLIB
			float	refvalue;
			float	setpointvalue;
			float	errorlimit;
#else
			dword	refvalue;
			dword	setpointvalue;
			dword	errorlimit;
#endif
			byte	StatusFlags;
			} FL;					//floating-limit
		struct	{
#ifdef FLOATLIB
			float	exceedingvalue;
			float	deadband;
			float	exceededlimit;
#else
			dword	exceedingvalue;
			dword	deadband;
			dword	exceededlimit;
#endif
			byte	StatusFlags;
			} OOR;					//out-of-range
		struct	{
			word		numberpvs;
			frPropVal	propval[8];
			} CET;
#ifdef ESP32_SETUP
		struct {					//									***1207 Begin
			frDeviceObjPropRef bufferProperty;
			dword	previousNotification;
			dword	currentNotification;
			} THPBR;					//buffer ready						***1207 End
			/*Changing struct name from BR to THPBR cos ESP32 is having special register name as 'BR'*/
#else
		struct {					//									***1207 Begin
			frDeviceObjPropRef bufferProperty;
			dword	previousNotification;
			dword	currentNotification;
			} BR;					//buffer ready						***1207 End
#endif
		}npu;
	} frEventNotification;

#define status_flags_in_alarm			0x80				//					***603 Begin
#define status_flags_fault				0x40
#define status_flags_overridden			0x20
#define status_flags_out_of_service		0x10
#define status_flags_normal				0x00				//					***603 End

#define	NO_UNITS						vsbeu_no_units					//units enumeration for none

//Error codes
//note:	all are 16 bit values, MSB=error-class, LSB=error-code
//		the special case of (device,other) is disallowed in this implementation
//		so that a value of zero means "no error"

#define	deviceerror							0x0000
#define objecterror							0x0100
#define propertyerror						0x0200
#define resourceserror						0x0300
#define securityerror						0x0400
#define serviceserror						0x0500
#define vterror								0x0600
#define clienterror							0x7D00
#define rejectreason						0x7E00
#define abortreason							0x7F00

#define vsbe_configurationinprogress		deviceerror+2		//						***005
#define vsbe_inconsistentparameters			serviceserror+7
#define vsbe_invaliddatatype				propertyerror+9
#define vsbe_propertyisnotanarray			propertyerror+50
#define vsbe_propertyisnotalist				propertyerror+22
#define vsbe_duplicatename					propertyerror+48
#define vsbe_nospacetoaddlistelement        resourceserror+19          //              ***1230
#define	vsbe_nospacetowriteproperty			resourceserror+20
#define vsbe_resourcesother					resourceserror+0
#define vsbe_svcsinvalidtag					serviceserror+57		//					***1221
#define vsbe_invalidaccessmethod			serviceserror+10
#define vsbe_invalidfilestart				serviceserror+11
#define vsbe_servicerequestdenied			serviceserror+29
#define vsbe_optionalfunctionalitynotsupported	serviceserror+45	//				***1228
#define vsbe_servicecommunicationdisabled	serviceserror+83	//					***1200
#define vsbe_unknownobject					objecterror+31
#define vsbe_unknownproperty				propertyerror+32 
#define vsbe_unsupportedobjecttype			objecterror+36	
#define vsbe_valueoutofrange				propertyerror+37
#define vsbe_writeaccessdenied				propertyerror+40
#define vsbe_invalidarrayindex				propertyerror+42
#define	vsbe_readaccessdenied				propertyerror+27	//					***600
#define vsbe_notcovproperty					propertyerror+44	//					***1223
#define vsbe_initiationdisabled				clienterror+32
#define vsbe_unknowndevice					clienterror+31
#define vsbe_timeout						clienterror+30
#define vsbe_slavemodeenabled				clienterror+33		//					***1200
#define vsbe_commdisabled					clienterror+34		//					***1200
#define vsbe_objecterrorother				objecterror+0;
#define vsbe_passwordfailure				securityerror+26	//					***1200
#define vsbe_resourcesother					resourceserror+0	//					***216
#ifdef ESP32_SETUP
#define vsbe_segmentationnotsupported		( abortreason + 4 )		//				***031  (For this compiler is giving error like(suggest parentheses around '+' in operand of '&' [-Werror=parentheses]))
#else
#define vsbe_segmentationnotsupported		abortreason+4		//					***031
#endif
#define vsbe_outofresources					abortreason+9		//					***1205
#define vsbe_charactersetnotsupported		propertyerror+41
#define vsbe_svccharsetnotsupported			serviceserror+41	//					***1200
#define vsbe_objectnamenotunique			objecterror+24		//must change this error eventually!
#define vsbe_invalidparametertype           rejectreason+3      //                  ***1202
#define vsbe_invalidtag						rejectreason+4		//					***400
#define vsbe_parameteroutofrange			rejectreason+6		//					***1205
#define vsbe_noerror						0xFFFF

//standard object types we use								//						***250 Begin

#define ANALOG_INPUT_OBJTYPE					0U
#define ANALOG_OUTPUT_OBJTYPE					1U
#define ANALOG_VALUE_OBJTYPE					2U
#define BINARY_INPUT_OBJTYPE					3U
#define BINARY_OUTPUT_OBJTYPE					4U
#define BINARY_VALUE_OBJTYPE					5U
#define	CALENDAR_OBJTYPE						6U
#define COMMAND_OBJTYPE							7U
#define DEVICE_OBJTYPE							8U
#define EVENT_ENROLLMENT_OBJTYPE				9U
#define FILE_OBJTYPE							10U
#define	GROUP_OBJTYPE							11U
#define	LOOP_OBJTYPE							12U
#define	MULTI_STATE_INPUT_OBJTYPE				13U
#define	MULTI_STATE_OUTPUT_OBJTYPE				14U
#define	NOTIFICATION_CLASS_OBJTYPE				15U
#define	PROGRAM_OBJTYPE							16U
#define	SCHEDULE_OBJTYPE						17U
#define	AVERAGING_OBJTYPE						18U
#define	MULTI_STATE_VALUE_OBJTYPE				19U
#define	TREND_LOG_OBJTYPE						20U  				//				***250 End
#define	LIFE_SAFETY_POINT_OBJTYPE				21U
#define	LIFE_SAFETY_ZONE_OBJTYPE				22U
#define	ACCUMULATOR_OBJTYPE						23U 				//				***400
#define PULSE_CONVERTER_OBJTYPE					24U					//				***400
#define	EVENT_LOG								25U					//				***1200
#define	GLOBAL_GROUP							26U					//				***1200
#define	TREND_LOG_MULTIPLE						27U					//				***1200
#define LOAD_CONTROL_OBJTYPE					28U					//				***600
#define STRUCTURED_VIEW_OBJTYPE					29U					//				***600
#define ACCESS_DOOR_OBJTYPE						30U					//				***600
// -- value 31 is unassigned										//				***1200 Begin
#define ACCESS_CREDENTIAL_OBJTYPE				32U
#define ACCESS_POINT_OBJTYPE					33U
#define ACCESS_RIGHTS_OBJTYPE					34U
#define ACCESS_USER_OBJTYPE						35U
#define ACCESS_ZONE_OBJTYPE						36U
#define CREDENTIAL_DATA_INPUT_OBJTYPE			37U
#define NETWORK_SECURITY_OBJTYPE				38U
#define BITSTRING_VALUE_OBJTYPE					39U
#define CHARACTERSTRING_VALUE_OBJTYPE			40U
#define DATE_PATTERN_VALUE_OBJTYPE				41U
#define DATE_VALUE_OBJTYPE						42U
#define DATETIME_PATTERN_VALUE_OBJTYPE			43U
#define DATETIME_VALUE_OBJTYPE					44U
#define INTEGER_VALUE_OBJTYPE					45U
#define LARGE_ANALOG_VALUE_OBJTYPE				46U
#define OCTETSTRING_VALUE_OBJTYPE				47U
#define POSITIVE_INTEGER_VALUE_OBJTYPE			48U
#define TIME_PATTERN_VALUE_OBJTYPE				49U
#define TIME_VALUE_OBJTYPE						50U					//				***1200 End

//standard properties we use
#define ACKED_TRANSITIONS						0U
#define ACK_REQUIRED							1U
#define ACTION									2U					//				***400
#define ACTION_TEXT								3U					//				***400
#define ACTIVE_COV_SUBSCRIPTIONS				152U					//				***409
#define ACTIVE_TEXT								4U
#define ALARM_VALUE								6U
#define ALARM_VALUES							7U					//				***410
#define ALLX									8U
#define APDU_SEGMENT_TIMEOUT					10U
#define APDU_TIMEOUT							11U
#define APPLICATION_SOFTWARE_VERSION			12U
#define ARCHIVE									13U					//				***250
#define AUTO_SLAVE_DISCOVERY					169U					//				***400
#define BUFFER_SIZE								126U					//				***400
#define CONFIGURATION_FILES						154U					//				***400
#define COV_INCREMENT							22U					//				***416
#define DATABASE_REVISION						155U					//				***002
#define DATE_LIST								23U					//				***400
#define DAYLIGHT_SAVINGS_STATUS					24U
#define DEADBAND								25U
#define DESCRIPTION								28U
#define DEVICE_ADDRESS_BINDING					30U
#define EFFECTIVE_PERIOD						32U					//				***400
#ifdef ESP32_SETUP
#define ENABLE_BACNET                           133U					// ***400 enable for Bacnet (Changing name from ENABLE to ENABLE_BACNET because ESP32 is having enum'ENABLE' in Uc_base.h)
#else
#define ENABLE                                  133U					// ***400 enable for Bacnet
#endif
#define EVENT_ENABLE							35U
#define EVENT_STATE								36U
#define EVENT_TIME_STAMPS						130U
#define EXCEPTION_SCHEDULE						38U					//				***400
#define FAULT_VALUES							39U					//				***410
#define FILE_ACCESS_METHOD						41U					//				***250
#define FILE_SIZE								42U					//				***250
#define FILE_TYPE								43U					//				***250
#define FIRMWARE_REVISION						44U
#define HIGH_LIMIT								45U
#define INACTIVE_TEXT							46U
#define ISSUE_CONFIRMED_NOTIFICATIONS			51U
#define	LAST_RESTORE_TIME						157U
#define LAST_RESTART_REASON						196U					//				***1200
#define LIMIT_ENABLE							52U
#define LIST_OF_OBJECT_PROPERTY_REFERENCE		54U
#define LOCAL_DATE								56U
#define LOCAL_TIME								57U
#define LOCATION								58U
#define LOG_BUFFER								131U					//				***400
#define LOG_DEVICE_OBJECT_PROPERTY				132U				//				***400
#define LOG_INTERVAL							134U					//				***400
#define LOGGING_TYPE							197U					//				***400
#define LOW_LIMIT								59U
#define MAX_APDU_LENGTH_ACCEPTED				62U
#define MAX_INFO_FRAMES							63U
#define MAX_MASTER								64U
#define MAX_PRES_VALUE							65U					//				***603
#define MODEL_NAME								70U
#define MODIFICATION_DATE						71U					//				***250
#define NOTIFICATION_CLASS						17U					//				***400
#define NOTIFY_TYPE								72U
#define NUMBER_OF_APDU_RETRIES					73U
#define NUMBER_OF_STATES						74U
#define OBJECT_IDENTIFIER						75U
#define OBJECT_LIST								76U
#define OBJECT_NAME								77U
#define OBJECT_TYPE								79U
#define OPTIONALX								80U
#define OUT_OF_SERVICE							81U
#define POLARITY								84U
#define PRESENT_VALUE							85U
#define PRIORITY								86U
#define PRIORITY_ARRAY							87U
#define PRIORITY_FOR_WRITING					88U
#define PROCESS_IDENTIFIER						89U
#define PROFILE_NAME							168U
#define PROTOCOL_CONFORMANCE_CLASS				95U
#define PROTOCOL_OBJECT_TYPES_SUPPORTED			96U
#define PROTOCOL_SERVICES_SUPPORTED				97U
#define PROTOCOL_VERSION						98U
#define READ_ONLY_VSBHP							99U					//				***016
#define RECIPIENT								101U
#define RECIPIENT_LIST							102U				//				***400
#define RECORD_COUNT							141U					//				***250
#define RELIABILITY								103U					//				***032
#define RELINQUISH_DEFAULT						104U
#define REQUIRED								105U
#define RESTART_NOTIFICATION_RECIPIENTS			202U					//				***1200
#define SCALE									187U					//				***603
#define SCHEDULE_DEFAULT						174U					//				***400
#define SEGMENTATION_SUPPORTED					107U
#define SLAVE_PROXY_ENABLE						172U					//				***400
#define START_TIME								142U					//				***400
#define STATE_TEXT								110U
#define STATUS_FLAGS							111U
#define STOP_TIME								143U
#define STOP_WHEN_FULL							144U					//				***400
#define SYSTEM_STATUS							112U
#define TIME_DELAY								113U
#define TIME_OF_DEVICE_RESTART					203U					//				***1200
#define TOTAL_RECORD_COUNT						145U					//				***400
#define	UNITSX									117U
#define UTC_OFFSET								119U
#define VENDOR_IDENTIFIER						120U
#define VENDOR_NAME								121U
#define PROTOCOL_REVISION						139U
#define WEEKLY_SCHEDULE							123U				//				***400
#define NODE_SUBTYPE							207U				//				***600
#define NODE_TYPE								208U				//				***600
#define STRUCTURED_OBJECT_LIST					209U				//				***600
#define SUBORDINATE_ANNOTATIONS					210U				//				***600
#define SUBORDINATE_LIST						211U					//				***600
#define PROPERTY_LIST							371U					//				***1206

//Eaton Specific properties
#define EATON_DEFAULT_PASSWORD          600
#define SERIAL_NUMBER 					601   // Fixed DH1-218



//Object types supported															***400 Begin
//BACnet object types support bits, segmented into bytes
//for ease in setting up frDevInfo objecttypessupported member

//these are for object_types_supported[0]
#define vsbots_AI				1<<7	//Analog Input
#define vsbots_AO				1<<6	//Analog Output
#define vsbots_AV				1<<5	//Analog Value
#define vsbots_BI				1<<4	//Binary Input
#define vsbots_BO				1<<3	//Binary Output
#define vsbots_BV				1<<2	//Binary Value
#define vsbots_CAL				1<<1	//Calendar
#define vsbots_COM				1<<0	//Command

//these are for object_types_supported[1]
#define vsbots_DEV				1<<7	//Device
#define vsbots_EE				1<<6	//Event Enrollment
#define vsbots_FILE				1<<5	//File
#define vsbots_GRP				1<<4	//Group
#define vsbots_LOOP				1<<3	//Loop
#define vsbots_MSI				1<<2	//Multistate Input
#define vsbots_MSO				1<<1	//Multistate Output
#define vsbots_NC				1<<0	//Notfication class

//these are for object_types_supported[2]
#define vsbots_PGM				1<<7	//Program
#define vsbots_SCH				1<<6	//Schedule
#define vsbots_AVG				1<<5	//Averaging
#define vsbots_MSV				1<<4	//Multi State Value
#define vsbots_TL				1<<3	//Trend Log
#define vsbots_LSP				1<<2	//Life Safety Point
#define vsbots_LSV				1<<1	//Life Safety Value
#define vsbots_ACC				1<<0	//Accumulator

//these are for object_types_supported[3]
#define vsbots_PC				1<<7	//Pulse Converter
#define vsbots_EL				1<<6	//Event Log					//				***1200
#define vsbots_GG				1<<5	//Global Group				//				***1200
#define vsbots_TLM				1<<4	//Trend Log Multiple		//				***1200
#define vsbots_LC				1<<3	//Load Control				//				***600 Begin
#define vsbots_SV				1<<2	//Structured View	
#define vsbots_AD				1<<1	//Access Door				//				***600 End		
//--value 31 is unassigned											//				***1200 Begin

//these are for object_types_supported[4]
#define vsbots_AC				1<<7	//Access Credential		
#define vsbots_AP				1<<6	//Access Point
#define vsbots_AR				1<<5	//Access Rights           
#define vsbots_AU				1<<4	//Access User             
#define vsbots_AZ				1<<3	//Access Zone             
#define vsbots_CDI				1<<2	//Credential Data Input	  
#define vsbots_NS				1<<1	//Network Security        
#define	vsbots_BSV				1<<0	//Bitstring Value         

//these are for object_types_supported[5]
#define	vsbots_CSV				1<<7	//Characterstring Value	  
#define	vsbots_DPV				1<<6	//Date Pattern Value	  
#define	vsbots_DV				1<<5	//Date Value			  
#define	vsbots_DTPV				1<<4	//Datetime Pattern Value  
#define	vsbots_DTV				1<<3	//Datetime Value          
#define	vsbots_IV				1<<2	//Integer Value           
#define	vsbots_LAV				1<<1	//Large Analog Value      
#define	vsbots_OSV				1<<0	//Octetstring Value       

//these are for object_types_supported[6]
#define	vsbots_PIV				1<<7	//Positive Integer Value  
#define	vsbots_TPV				1<<6	//Time Pattern Value      
#define	vsbots_TV				1<<5	//Time Value				//				***1200 End

//for readRange ResultFlags
#define vsbrr_firstitem			0x80
#define vsbrr_lastitem			0x40
#define vsbrr_moreitems			0x20

//for log buffer log-status
#define vsblb_logdisabled		0x80
#define vsblb_bufferpurged		0x40
#define vsblb_loginterrupted	0x20											//	***1200

//for reinitialize device
#define vsb_coldstart			0x00											//	***1224 Begin
#define vsb_warmstart			0x01
#define vsb_startbackup			0x02
#define vsb_endbackup			0x03
#define vsb_startrestore		0x04
#define vsb_endrestore			0x05
#define vsb_abortrestore		0x06											

//BACnetNotifyType
#define vsb_notifytype_alarm	0x00
#define vsb_notifytype_event	0x01
#define vsb_notifytype_ack		0x02											//	***1224 End

//																					***400 End

////////////////////////////////////////////
// Stack Routines

void  bpublic frMain(void);
void  bpublic frStartup(byte);
void  bpublic frWork(byte);
int   bpublic frTransmitIAm(word);												//	***1205
int   bpublic frTransmitIHave(word, dword, frString *);		
int   bpublic frTransmitWhoHas(word, dword, dword, dword, frString *);			//	***1202
#ifdef useMSTP
void  bpublic frReplyPostponed(void);
#endif
void bpublic maxMaster(byte); // To update Device Max master value when changed using Keypad // DH1-131
void   bpublic Track_COV_registration();

////////////////////////////////////////////
// Server Routines

int   bpublic fraAcknowledgeAlarm(frAckAlarm *);									
int   bpublic fraAtomicReadFile(dword, dword, word *, byte *, bool *);				
int   bpublic fraAtomicWriteFile(dword, dword *, word, byte *);						
bool  bpublic fraCanDoAlarms(void);												//	***232 
bool  bpublic fraCanDoAlarmAck(void);											//	***404
bool  bpublic fraCanDoAlarmSummary(void);										//	***404
bool  bpublic fraCanDoCOV(void);												//	***232 
bool  bpublic fraCanDoCOVNotifications(bool cnf);								//	***1224
bool  bpublic fraCanDoCOVProperty(void);										//	***1223
bool  bpublic fraCanDoEventInfo(void);											//	***404
bool  bpublic fraCanDoFiles(void);												//	***232 
bool  bpublic fraCanDoOCD(void);												//	***1205
bool  bpublic fraCanDoPvtTransfer(bool);										//	***1205
bool  bpublic fraCanDoReadRange(void);											//	***400
bool  bpublic fraCanDoReinit(void);												//	***404
bool  bpublic fraCanDoRestartNotify(void);										//	***1200
bool  bpublic fraCanDoRPM(void);												//	***418
bool  bpublic fraCanDoWPM(void);												//	***418
bool  bpublic fraCanDoStructuredView(void);										//	***600
bool  bpublic fraCanDoTimeSync(void);											//	***261
bool  bpublic fraCanDoUTCTimeSync(void);										//	***404
void  bpublic fraConfirmedPrivateTransfer(frSource *,word,byte *,word,frVbag *); //	***1205
int   bpublic fraCOV(frCOV *);													//	***1223
int   bpublic fraCreateObject(dword,byte,frCOprop *);							//	***1205
int   bpublic fraDeleteObject(dword);											//	***1205
void  bpublic fraGetAlarms(byte,word *, void *,bool *);								
int	  bpublic fraGetCOVSubscriptions(frVbag *);									//	***410
dword bpublic fraGetDatabaseRevision(void);											
void  bpublic fraGetDeviceInfo(frDevInfo *);
uint32_t  bpublic fraGetNumberOfCOVSubscriptions(void);								//	***410
dword bpublic fraGetNumberOfObjects(void);											
byte  bpublic fraGetNumberofRestartRecipients(void);							//	***1200
dword bpublic fraGetNumberOfStructuredViewObjects(void);						//	***600
dword bpublic fraGetObjectAt(dword);											//	***263
byte  bpublic fraGetPropertyDatatype(dword,byte);								//  ***1205
int   bpublic fraGetRestartNotifyRecipients(frVbag *);							//	***1200
dword bpublic fraGetStructuredViewObjectAt(dword);								//	***600
bool  bpublic fraGetTimeDate(frTimeDate *);
bool  bpublic fraGetLastRestartTime(frTimeDate *);								//	***1200
void  bpublic fraIHave(dword,dword,frString *);									//	***1202
void  bpublic fraIncrementDatabaseRevision(void);										
void  bpublic fraLog(char *);													//	***233
bool  bpublic fraMSTPTxComplete(void);												
void  bpublic fraPrivateTransfer(frSource *,word,byte *,word);					//	***233
int   bpublic fraReadProperty(dword, dword, dword , frVbag *, dword *);				
int	  bpublic fraReadRange(dword,dword,dword, frVbag *, frRange *, byte *);		//	***400
int   bpublic fraReinitDevice(int, frString *);										
void  bpublic fraResponse(frVbag *);											//	***240
int	  bpublic fraSaveDeviceVars(frDevInfo *);									//	***404
bool  bpublic fraSetTimeDate(frTimeDate *);
int   bpublic fraSubscribeCOV(frSubscribeCOV *);								//	***210 
int   bpublic fraSubscribeCOVProperty(frSubscribeCOVProperty *);				//	***1223
void  bpublic fraUnsubscribedCOV(frCOV *);										
void  bpublic fraWhoHas(word, bool, dword, frString *);							//	***210 
int   bpublic fraWriteProperty(dword, dword, dword , frVbag *);						

////////////////////////////////////////////
// Client Routines

//int   bpublic frcConfirmedPrivateTransfer(dword,frVbag *);							//	***1205
int   bpublic frcConfirmedPrivateTransfer(dword, frSource *, frVbag *);					//	***1224
int	  bpublic frcCOVNotify(frSubscribeCOV *, frVbag *);									//	***240
int	  bpublic frcCOVNotifyDevice(dword,frSubscribeCOV*, frVbag *);						//	***1200
bool  bpublic frcDefineDevice(frBACnetdevice *);										//	***002
int   bpublic frcEventNotify(frEventNotification *, frSource *, frVbag *);				//	***402
void  bpublic frcForceWhoIs(void);												
int   bpublic frcPrivateTransfer(dword,frSource *,dword,octet *,word);					//	***233
int   bpublic frcUnconfirmedPrivateTransfer(dword, frSource *, frVbag *);				//	***1224
int   bpublic frcReadProperty(dword, dword, dword, dword , frVbag *);					//	***240
int   bpublic frcReadPropertyEx(dword, frVbag *);										//	***1205
int	  bpublic frcSearchForDevice(dword);									
int   bpublic frcSubscribeCOV(frVbag *);												//	***1206
int   bpublic frcTimeSynchronization(bool);												//	***216
int   bpublic frcUnsolicitedCOV(frCOV *);												//	***006
int   bpublic frcWriteProperty(dword, dword, dword, dword , frVbag *);					//	***240
int   bpublic frcWritePropertyEx(dword, frVbag *);										//	***1205

#endif //__VSB_H_INCLUDED
