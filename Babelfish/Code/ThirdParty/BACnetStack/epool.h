//Contents of EPOOL.H
//	©2004, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------

#ifndef __EPOOL_H_INCLUDED
#define __EPOOL_H_INCLUDED

#define epmax		100				//number of alarm buffers

#include "core.h"


typedef struct	_frEventNotification{	//to transfer an event notification to VSB
	struct _frEventNotification *next;	//link to next one
	int			invokeid; 			//noinvokeid=waiting to be sent
	long		ntries;				//number of times we've tried to send it (-1=sent)
	long		timer;				//a timer
	byte		notifytype;			//0=alarm, 1=event, 2=ack-notification
	byte		confirmed;			//0=unconfirmed, 1=confirmed notification
	byte		priority;
	byte		alarmackrequired;	//0=no, 1=yes
	byte		statusflags;
	byte		resv;
	dword		destdevice;			//destination device (0xFFFFFFFF=broadcast)
	dword		processid;			//process id
	dword		initdeviceinst;		//initiating device instance
	dword		objid;				//object id
	frTimeStamp	timestamp;			//time of the event notification
	dword		notificationclass;
	word		obx;				//object index
	word		eventtype;			//the event type that has occurred: entypeCOS or entypeOOR
	word		fromstate;			//the event state prior to the alarm/event
									// 0=normal, 1=fault, 2=offnormal, 
									// 3=high-limit, 4=low-limit
	word		tostate;			//the event state after the alarm/event
	frString 	msgh;				//header of message text for the event
	char		msg[128];			//place to put message for the event
	union	{
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

typedef struct {
	frEventNotification	*front;
	frEventNotification	*rear;
	word		count;
	} frEfifo;

//------------------------------------------------------------------------
//Functions provided by EPOOL.C:

void bpublic				epoolInit(void);
bpublic frEventNotification	*epoolGet(void);
void bpublic				epoolFree(frEventNotification *);

#endif //__EPOOL_H_INCLUDED
