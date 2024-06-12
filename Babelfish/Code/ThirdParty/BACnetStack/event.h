//Contents of EVENT.H
//	©2004-2010, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------

#ifndef __EVENT_H_INCLUDED
#define __EVENT_H_INCLUDED


#include "core.h"
#include "vsbhp.h"

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
		}npu;
	} frEventNotification;


#endif //__EVENT_H_INCLUDED
