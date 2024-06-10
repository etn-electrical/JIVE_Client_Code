//Contents of CORE.H
//	©2000-2016, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------
// [1226] 22-Sep-2016 CLB added use of stdbool.h if it exists
// [1200] 07-Nov-2012 CLB Protocol revision 12 changes
// [400] 25-Aug-2009 CLB Protocol revision 4 changes
// [255] 01-Aug-2007
// [004] 08-Dec-2003
// [003] 30-Jul-2002
// [002] 27-Jun-2002
#define	FLOATLIB			1					//comment this out to exclude FP libs

#ifndef __CORE_H_INCLUDED
#define __CORE_H_INCLUDED

#include <stdbool.h>
#include "endian.h"

// To solve size_t issue during compilation for ESP32
#ifdef ESP32_SETUP
#include "Device_Defines.h"
#endif

#ifdef __BORLANDC__
#ifndef _WINDOWS
#ifdef _WIN32
#define _WINDOWS _WIN32
#endif
#endif
#endif

#if HAVE_STDBOOL_H								//	***1226 Begin
# include <stdbool.h>
#else
#if !HAVE__BOOL
#ifdef __cplusplus
typedef bool _Bool;
#else
typedef unsigned char _Bool;
#endif
#endif
#define bool _Bool
#define false 0
#define true 1
#define __bool_true_false_are_defined 1
#endif											//	***1226 End

#ifndef bpublic
#define bpublic 	extern
#endif

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef octet
typedef unsigned char octet;
#endif

#ifndef dword
typedef unsigned long dword;
#endif

#ifndef word
typedef unsigned short word;
#endif

//#ifndef false
//#define false		0
//#endif
//
//#ifndef true
//#define true		1
//#endif

#ifndef NULL
#define NULL		0
#endif

#ifndef far
#define far  
#endif

#ifndef _SIZE_T_DEFINED
//typedef unsigned long size_t;		//							***1205
#define _SIZE_T_DEFINED
#endif

enum BACnetDeviceStatus
	{	OPERATIONAL,           		//0
		OPERATIONAL_READ_ONLY, 		//1
		DOWNLOAD_REQUIRED,     		//2
		DOWNLOAD_IN_PROGRESS,  		//3
		NON_OPERATIONAL        		//4
	};

enum Charsets
	{	
		//chrsANSI,					//0
		chrsUTF8,					//0		//					***1200
		chrsDBCS,					//1
		chrsJIS,					//2
		chrsUCS4,					//3
		chrsUCS2,					//4
		chrs8859					//5
	};

typedef struct _BACnetAddress {			
	word network_number;				// Unsigned16,   -- A value of 0 indicates the local network
	octet mac_len;						// 0..6
	octet mac_address[6];				// OCTET STRING  -- A string of length 0 indicates a broadcast
} frBACnetAddress;

typedef struct _BACnetRecipient {
	octet choice;						//0=objid, 1=BACnetAddress	
	dword device;						//[0] BACnetObjectIdentifier
	frBACnetAddress address;			//[1] BACnetAddress
} frBACnetRecipient;					

typedef struct _Scale {					//												***603 Begin
	octet choice;						//0=float, 1=Integer
	union _scaleval {
		float f;						//[0] Real
		int i;							//[1] Integer
	} sv;
} frScale;								//												***603 End

typedef struct _frString{
	byte       len;					//string length in octets
	byte       maxlen;				//maximum length that this string can hold (<= 255), includes terminator
	bool       readonly;            //true if string is stored in flash, for example
	char       *text;				//pointer to the UTF string data
} frString;

typedef struct _frDevInfo {
	dword	deviceinstance;
	word	vendor_id;
	byte	system_status;				//see BACnetDeviceStatus
	//byte	object_types_supported[3];	//bit string, one bit per object type			***400
	//byte	object_types_supported[4];	//bit string, one bit per object type			***400
	byte	object_types_supported[7];	//bit string, one bit per object type			***1200
	byte	maxmaster;
	byte	mstpstation;
	byte	maxinfoframes;
	byte	whoisinterval;
	byte	apduretries;
	byte	apdu_timeout;			//in whole seconds
	frString *password;
	word	bipudp;					//little endian
	short	utcoffset;
	dword	bipipaddr;
	dword	bipipsubnet;
	dword	bbmdipaddr;				//IP address of the BBMD to register with		***255
	word    bbmdudp;                   //BBMD port number
	word	bbmdttl;				//the BBMD Time to Live								***255	***1225
	octet   lastrestartreason;		//													***1200 

	frString *device_name;
	frString *vendor_name;
	frString *model_name;
	frString *firmware_rev;
	frString *serial_num;
	frString *application_software_ver;
	frString *device_description;	//													***003
	frString *device_location;		//													***004
	frString *device_profile;		//													***004
} frDevInfo;

typedef struct _frTimeDate {
	byte	hour;					//0..23
	byte	minute;					//0..59
	byte	second;					//0..59
	byte	hundredths;				//0..99
	word	year;					//e.g. 1998
	byte	month;					//1..12
	byte	day;					//1..31
	byte	weekday;				//1..7 (1=Monday)
	byte	dst;					//0=no, 1=in effect
	int		utcoffset;				//-720..+720 time is offset from UTC by this amount of minutes
} frTimeDate;

typedef struct _frTime {
	byte	hour;					//0..23
	byte	minute;					//0..59
	byte	second;					//0..59
	byte	hundredths;				//0..99
} frTime;

typedef struct _frDate {
	byte	year;					//year-1900
	byte	month;					//1..12, 1=Jan
	byte	day;					//1..31
	byte	dow;					//1..7, 1=Monday, 7=Sunday
} frDate;

#define tsTimeType	0				//	***1200 Begin
#define tsSeqType	1
#define tsDTType	2				//	***1200 end


typedef struct _frTimeStamp {		//a time stamp
	byte	type;					//timestamp type (0=time, 1=sequenceNumber, 2=dateTime)
	byte	resv;					//reserved
	word	sequence;				//used only if type=1
	frTime	time;					//used only if type=0,2
	frDate	date;					//used only if type=2
} frTimeStamp;

typedef struct _frDateTimeStamp {	//a date/time stamp
	byte	year;					//year-1900
	byte	month;					//1..12, 1=Jan
	byte	day;					//1..31
	byte	dow;					//1..7, 1=Monday, 7=Sunday
	byte	hour;					//0..23
	byte	minute;					//0..59
	byte	second;					//0..59
	byte	hundredths;				//0..99
} frDateTimeStamp;

typedef struct _frObjectPropRef {
	dword	objid;
	dword	propid;
	dword	arrindex;
} frObjectPropRef;

typedef struct _frSmallVbag {
	byte	status;					//status see vbStates
	byte	pdtype;					//datatype see AppDatatypes
	byte	priority;				//priority for writing 1..16, always 0 for reading!
	byte	len;					//length of octet string in bytes or length of bit string in bits
	union _spd {
		dword	uval;				//unsigned value (boolean, Unsigned, enum, objid)
		long	sval;				//signed value (integer)
#ifdef FLOATLIB						//include float lib
		float	fval;				//real
		double	dval;				//double
#else								//exclude float lib
		dword	fval;
		dword	dval[2];
#endif
		frTime	tval;				//time
		frDate	dtval;				//date
		frTimeStamp ts;				//TimeStamp	//	***1200
		byte	psval[12];			//octet/bitstrings
	} pd;
} frSmallVbag;

typedef struct _frPropVal {			//a property value bag
	dword		propid;				//the property id
	dword		arrayindex;			//the array index (may be noindex)
	frSmallVbag	value;				//the value
// Note: The priority is contained inside the frSmallVbag.
} frPropVal;

void  bpublic fraLog(char *);											//***233

#endif //__CORE_H_INCLUDED
