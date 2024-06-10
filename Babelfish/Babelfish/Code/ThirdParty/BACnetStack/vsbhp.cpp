//#define	DEBUGVSBHP	1						//uncomment for debug mode
#define		vstring		"v12.29"
#ifdef __BORLANDC__	
#pragma warn -aus
#pragma warn -par
#pragma warn -eff
#endif
/*---------------------------------------------------------------------------
	©2000-2016, PolarSoft® Inc., All Rights Reserved
	
module:		VSBHP.C
desc:		FreeRange VSB High Performance BACnet Client/Server
authors:	David M. Fisher, Jack R. Neyer, Coleman L. Brumley
last edit:	05-Dec-2016 [1229] CLB  > correct termination issue in OBJECT_NAME write
								    > various code cleanups
			07-Nov-2016 [1228] CLB  > correctly handle indefinite times in HandleDCC
									> correct goes_to_hccrej in HandleDCC, HandleReinitDevice, and HandleConfirmedPvtTransfer
									> added vsbe_optionalfunctionalitynotsupported
			01-Nov-2016 [1227] CLB  > correct issues in reinitDevice handling
									> created HandleReinitDevice, HandleDCC, and HandleConfirmedPvtTransfer
									> refactoring/reodering in HandleConfirmedReq
									> correct password issue in HandleDCC
			21-Sep-2016 [1226] CLB  > Handled uses of BACnetDateTime in MODIFICATION_DATE, PRESENT_VALUE, and RELINQUISH_DEFAULT
			07-Jul-2016 [1225] CLB  > Change placement of fraCanDoCOVNotifications(false) in PROTOCOL_SERVICES_SUPPORTED
									> indicate execute of I-Have in PROTOCOL_SERVICES_SUPPORTED
									> frDevInfo bbmdttl is now word so it's not zero on BigEndian architecture
			24-Mar-2016 [1224] CLB  > Change frcConfirmedPrivateTransfer to take either device or frSource
								    > Added frcUnconfirmedPrivateTransfer
									> Modified frcPrivateTransfer to call frcUnconfirmedPrivateTransfer 
									>    (only for backwards compatbility)
									> Removed unused variable in HandleCOVNotify
									> Corrected ap increment in HandleCOVNotify
									> added error handling for reinitializeDevice
									> add handler for confirmedCOVNotification
									> clean up unused parameter in dUnsignedList
									> correct endian problem in HandleAtomicFileRead
									> correct length check for adtTimeValue encoding
									> added fraCanDoCOVNotifications
									> added support for the priority property 
									> corrected MINEVENTSIZE et al
			23-Feb-2016 [1223] CLB  > protect against invalid Bit String and Character String lengths
									> address frSource issues in frcPrivateTransfer
									> also check portid in di.maxmaster==0 checks for slave nodes
									> use Abort/OUT_OF_RESOURCES when call to npoolGetPacket fails
								    > allow NULL in EncodeTimeValue
									> various changes based on BTL test feedback
									> fraCOV returns error or success now (API change)
									> Support SubscribeCOVProperty (API change)
			23-Jan-2016 [1222] CLB  > additional validations in SkipNLHeader
									> use nxprop in PROPERTY_LIST handler
									> don't send I-Am in response to Who-Is with 4194303 in range
			22-Jun-2015 [1221] CLB  > Modified HandleAcknowledgeAlarm to deal with non UTF-8 charset
									> Modified HandleGetEventInformation to return a small number
									>	of events with more==TRUE if all we have can't fit in one, unsegmented APDU
									> Split HandleReadWritePropertyMultiple into 
									>	HandleReadPropertyMultiple & HandleReadWritePropertyMultiple			
									> Modified HandleWritePropertyMultiple to return complex Result(-)
			03-Jun-2015 [1220] CLB  > version change for mstp.c and bip.c changes
			22-May-2015 [1219] CLB	> correct goto in HandleReadPropertyMultiple to reset error code
			13-May-2015 [1218] CLB  > Correct DNET endian issues in RSVP and frcPrivateTransfer
									> Don't explicitly support execution of ConfirmedCOVNotification
									> fixes to adtCharString in the array case
									> added object type comparison to WhoHas device instance handling									
			12-Feb-2015 [1217] CLB  > don't include Property-List in list of results for ALL/REQUIRED RPM
									> corrected embedded error handling in RPM 
			11-Dec-2014 [1216] CLB  > correct tag issue in EncodeEventLogRecord
			20-Nov-2014 [1215] CLB  > version change for mstp.c changes
			18-Nov-2014 [1214] CLB  > version change for mstp.c changes
			13-Nov-2014 [1213] CLB  > ensure the frNpacket's frSource is copied correctly in 
										ConfirmedRequest, UnconfirmedRequestMAC, ConfirmedRequestMAC\
									> Remove duplicate parsing code in HandleConfirmedReq
			11-Nov-2014 [1212] CLB  > Remove calls to npoolbzero, as that is now handled in npoolGetPacket
			07-Nov-2014 [1211] DMF  > Fix UnconfirmedSend, ConfirmedSend to test for snet/slen/drlen
									> Fix frcEventNotify to use UnconfirmedSend
									> Fix test for small buffer in HandleAtomicFileRead
			06-Nov-2014 [1210] CLB  > Addition of new wrapper function, transmit
									> Replace calls to macTransmit with calls to transmit
									> correct routing information in ConfirmedRequestMAC
									> correct routing information in ConfirmedRequest
									> correct routing information in UnconfirmedRequestMAC
									> use frSource verbatim in HandleSubscribeCOV
									> correct routing information in RSVP
			01-Nov-2014 [1209] DMF	> Rewrite macReceiveIndicate and SkipNLHeader for alignment-agnostic behavior
									> Fix sadr initialization in SkipNLHeader
									> Fix HandleAtomicFileRead double release scenario
									> Remove volatile keyword usage
			21-Oct-2014 [1208] DMF  > Fix UnconfirmedSend sending to direct device
									> Fix UnconfirmedRequestMAC globalnetwork and local network handling
									> Fix subscriber destination in HandleSubscribeCOV
									> Add volatile keyword to prevent optimizer issues
			23-Sep-2014 [1207] DMF  > Fix Buffer_Ready
									> Fix EncodeEventLogRecord handling of pv->eventType for BR case
									> Fix frcEventNotify unconfirmed|device case to lookup known peer
									> Fix frcEventNotify unconfirmed|address case to do local broadcast
									> Fix EncodeEventParameters handling of ep->eventtype for BR case
									> Fix dRecipient handling to not return invalid tag improperly
									> Fix HandleReadPropertyMultiple handling of all properties walk
			02-Jun-2014 [1206] DMF  > Add support for BUFFER_READY notifications
									> Add PROPERTY_LIST property to device object
									> Fix decoding of objectID in HandleGetEventInformation
									> Fix broadcast test in ConfirmedSend
									> Fix HandleAtomicFileRead
									  >get word size in proper endian form from union
									  >fix test for client max response
								    > Fix encoding of DateList
									> Support all Backup/Restore types in reinitializeDevice
									> Support client side COV
									> Support adtStringArray in HandleCreateObject
									> Fix encoders for EncodeDestination, EncodeCalendarEntry
									> Support for encoding EventLogRecords in ReadRange handling
									> Support for "special" calendar entries
									> Fix end of list detection in dRecipientList
									> Add adtDateTime encoding and decoding
									> Add adtObjPropRef support
			10-Mar-2014 [1205] DMF add frhReceiveIndicate, various fixes and unused variables/code removed
			05-Feb-2014 [1204] CLB Free timeout buffers in EverySecond regardless of vb->status
			07-Nov-2013 [1203] CLB correct broadcast issue in ConfirmedSend
			18-Oct-2013 [1202] CLB correct some calls to dUINTEx et al. 	
								   Used vsbe_invalidtag instead of (rejectreason+4)                                   
                                   Correct processid decode in HandleSubscribeCOV
                                   Remove use of calloc in HandleGetEventInformation
			05-Aug-2013 [1201] CLB Correct RPM issues with STRUCTURED_VIEW_OBJECT_LIST and ACTIVE_COV_SUBSCRIPTIONS
                                    when those services/features are not supported by the application
            27-May-2013 [1200] CLB Support for protocol revision 12
								   Protocol services supported is 51 bits
								   chrsANSI is now chrsUTF8
								   log-status in frBACnetLogRecord is 3 bits
								   return services/communication-disabled for reinitializeDevice
										only if communication is DISABLED via DCC
								   use frGetErrorCode/frGetErrorClass in all calls to SendError
								   implement new frString paradigm
								   frAckAlarm is static in HandleAcknowledgeAlarm								   
								   Added Restart Notifications
								    > Added fraCanDoRestartNotify()
									> Added vsbrn_restartreasons enum
									> Added LAST_RESTART_REASON
									> Added TIME_OF_DEVICE_RESTART
									> Added RESTART_NOTIFICATION_RECIPIENTS
								   Added slave mode for MS/TP:
									> PROTOCOL_SERVICES_SUPPORTED changes accordingly when in slave mode
									> Who-Is, Who-Has are dropped in HandledUnconfirmedReq
									> new error codes, vsbe_slavemodeenabled & vsbe_commdisabled
									> return vsbe_slavemodeenabled for any initiated request
									>    when maxmaster==0 (indicates slave mode)
									> frTransmitIAm now returns an error upon failure
									> max_master and max_info_frames aren't in a slave's Device Object
							   JRN Fixes:
								    > fix frGetErrorClass and frGetErrorCode to operate 16 bit error codes rather than 32 bit error codes
									> return fre_writeaccessdenied for ACTIVE_COV_SUBSCRIPTIONS, DEVICE_ADDRESS_BINDING props rather than
									  invalid tag
									> return vsbe_unknownproperty for ALL, OPTIONAL, REQUIRED readProperty or writeProperty
									> add defense for values > 32 bits and remove endianess from decoders
			(see OLDREVISIONS.TXT for previous edits)
----------------------------------------------------------------------------*/ 
//NOTE: This is a very simple BACnet Server/Client Stack for small devices.
//
// Data-Sharing
//		DS-COV-A  (Initiate SubscribeCOV)
//		DS-COV-B  (Initiate ConfirmedCOVNotification and UnconfirmedCOVNotification)
//		DS-COVP-A (Initiate SubscribeCOVProperty)
//		DS-COVP-B (Initiate ConfirmedCOVNotification and UnconfirmedCOVNotification)
//		DS-RP-A   (Initiate ReadProperty)
//		DS-RP-B   (Execute ReadProperty)
//		DS-RPM-B  (Execute ReadPropertyMultiple)
//		DS-WP-A   (Initiate WriteProperty)
//		DS-WP-B   (Execute WriteProperty)
//		DS-WPM-B  (Execute WritePropertyMultiple)
//	
// Alarm & Event Management
//		AE-N-I-B (Initiate ConfirmedEventNotification and UnconfirmedEventNotification)
//		AE-ACK-B (Execute AcknowledgeAlarm)
//		AE-INFO-B (Execute GetEventInformation)
//		AE-ESUM-B (Execute GetEnrollmentSummary)
//
// Scheduling
//		SCHED-I-B (Provide scheduling of internal values based on date and time)
//
// Trending
//		T-VMT-I-B (Execute ReadRange)
//		T-ATR-B   (Initiate ConfirmedEventNotification and UnconfirmedEventNotification, Execute ReadRange)
//
// Device & Network Management
//		DM-DDB-A (Initiate Who-Is, Execute I-Am)
//		DM-DDB-B (Execute Who-Is, Initiate I-Am)
//		DM-DOB-A (Initiate Who-Has, Execute I-Have)
//		DM-DOB-B (Execute Who-Has, Initiate I-Have)
//		DM-DCC-B (Execute DeviceCommunicationControl)
//		DM-TS-B  (Execute TimeSynchronization)
//		DM-UTC-B (Execute UTC TimeSynchonrization)
//		DM-RD-B  (Execute ReinitializeDevice)
//		DM-BR-B  (Execute ReinitializeDevice, AtomicReadFile, AtomicWriteFile)
//
// It can function as any of the following mutually exclusive BACnet Data Link Layers types:
//		MS/TP (default)
//		Standard BACnet Ethernet
//      BACnet/IP (non-BBMD)
//
// Segmentation is NOT supported.

/*
#include <stdio.h>								//only for debugging
#ifdef _WINDOWS
#include <dos.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#include <memory.h>
#include <string.h>								//										***010
*/

#include "util.h"
#include "mac.h"
#include "napdu.h"
#include "npool.h"								//										***243
#include "vsbhp.h"
#include "endian.h"
#ifdef useMSTP
#include "mstp.h"
#define MIMICSLOWMSTP 1
#endif
//SRP: Added to get the stack compiled
#include "string.h"


#include "Prod_Specific_BACnet.h"
#include "EthConfig.h"
#include "Trusted_IP_Filter.h"

///////////////////////////////////////////////////////////////////////
// module variables

#define		maxalarmtries			10			//max times we try to send alarm		***007
#define		maxcoprops				16			//max props we accept in CreateObject	***1205


static	byte		portid;						//the port type (one of portMax)
frDevInfo	di;									//place to save device info				

static	bool		commEnabled;
static	long		commEnableTimer;
static	bool		initiationDisabled=false;	//										***400
static	long		secondtimer=1000;			//in msec

//SRP:#ifndef _WINDOWS													//					**1208 Begin
//SRP:static	frfifo alq={NULL,NULL,0,{PTHREAD_MUTEX_INITIALIZER,0}};			//appl layer queue (queue of frNpacket's)
//SRP:static	frfifo confirmq={NULL,NULL,0,{PTHREAD_MUTEX_INITIALIZER,0}};	//client request (queue of frNpacket's)
//SRP:#else
static	frfifo alq;//={NULL,NULL,0,{0,0}};						//					***256
static	frfifo confirmq;//={NULL,NULL,0,{0,0}};				//					***256
//SRP:#endif														//					***1208 End

static	octet		nextInvokeID=0;				
												//	***401	***409

static	frBACnetdevice	devlist[maxdevices];

#ifdef DEBUGVSBHP
static	char dbg[256];
#endif

///////////////////////////////////////////////////////////////////////
//	prototypes

static  void		buildvbString(frVbag *vb);								//						***1205
static	word		calcClientMaxAPDU(octet);									//					***405
static	frNpacket	*ConfirmedRequest(dword,octet,bool,int *,frVbag *);		//						***240
static	frNpacket	*ConfirmedRequestMAC(frSource *,octet,bool,int *,frVbag *); //					***240
static	void		ConfirmedSend(frNpacket *);								//						***240
static	word		dBITSTRING(octet **,byte *,word);
static	bool		dBOOL(octet **);
static	word		dCHARSTRING(octet **,frString *);						//						***1200
static	dword		dCONTEXT(octet **,bool);
static	void		dDATESTRING(octet **,word *,octet *,octet *,octet *);
static	dword		dDWORD(octet **,bool);
static  int			dDWORDEx(octet **,bool,dword *);							//					***1200x
static	octet		*DecodeCalendarEntry(octet *,frBACnetCalendarEntry *,int *);	//				***401
static	dword		dENUM(octet **);
static	int			dENUMEx(octet **,dword *);									//					***1200x
static	long		dINT(octet **);
static	int			dINTEx(octet **,long *);									//					***1200x
static	octet		dNULL(octet **);
static	dword		dOBJECTID(octet **,word *);
static	word		dOCTETSTRING(octet **,octet *,word);
static	int			dPRIMITIVE(octet **,frVbag *);							//						***1205
static	octet		*dRecipientList(octet *, frVbag *,int *);				//						***401
static	octet		*dSpecialEvent(octet *, frVbag *,dword,int *);			//						***401
static	void		dTIMESTRING(octet **,octet *,octet *,octet *,octet *);
static	int			dTimeValue(octet **,frBACnetTV *);						//						***401
static	int			dTVval(octet **,frBACnetTV *);							//						***401
static	dword		dUINT(octet **);
static	int			dUINTEx(octet **, dword *);									//					***1200x
static	octet		*dWeeklySchedule(octet *, frVbag *,dword,int *);		//						***401

#ifdef FLOATLIB
static	void		dDOUBLE(octet **,double *);
static	float		dREAL(octet **);
#else
static	void		dDOUBLE(octet **,dword *);
static	dword		dREAL(octet **);
#endif

static	octet 		*eBITSTRING(octet *,byte *,octet,word);
static	octet  		*eBOOL(octet *, bool);
static	octet  		*eCHARSTRING(octet *,frString *);
static	octet  		*eDATESTRING(octet *, word, octet, octet, octet);
//octet	*eDOUBLE(octet *, double);
static	octet  		*eDWORD(octet *,dword,octet,bool);
static	octet  		*eENUM(octet *,dword);
static	octet  		*eINT(octet *, long);
static	octet		*EncodeCalendarEntry(octet* tp,frBACnetCalendarEntry* pv);	//					***1206
static	octet		*EncodeCOVNotifyParameters(octet *,dword,dword,dword,dword,frPropVal *,word); //***210
static	octet		*EncodeDestination(octet* tp,frBACnetDestination* pv);		//					***1206
static	octet 		*EncodeEventParameters(octet *,frEventNotification *);
static	octet		*EncodeLogRecord(octet*,frBACnetLogRecord*);								//	***1206
static	bool 		EncodePropertyValue(octet **,frVbag *,word);								//	***243
static	octet		*EncodeTimeValue(octet* tp,frBACnetTV* pv);							//			***400
static	octet 		*eNULL(octet *);
static	octet 		*eOCTETSTRING(octet *,octet *,word);
#ifdef FLOATLIB
static	octet 		*eDOUBLE(octet *, double *);
static	octet 		*eREAL(octet *,octet,float);
#else
static	octet 		*eDOUBLE(octet *, dword *);
static	octet 		*eREAL(octet *,octet, dword);
#endif
static	octet 		*etagOBJECTID(octet *,octet,dword);
static	octet 		*eTIMESTRING(octet *op, octet, octet, octet, octet);
static	octet 		*eUINT(octet *,dword);
static	void		EverySecond(void);												//				***240
static	void		HandleAcknowledgeAlarm(frNpacket *,octet *,octet);			//					***1205
static	void		HandleAtomicFileRead(frNpacket *,octet *,octet);			//					***1205
static	void		HandleAtomicFileWrite(frNpacket *,octet *,octet);			//					***1205
static	void		HandleCACK(ASDU *,word,frVbag *);							//					***002
static	bool		HandledConfirmedPvtTransfer(frNpacket* am, octet** rap, octet* rr, octet id, bool* goes_to_hcrrej);	//	***1227
static	frNpacket	*HandleConfirmedReq(frNpacket *);							//					***256
static	void		HandleCOVNotify(frNpacket *,octet *,word,octet);			//					***1206
static  void		HandleCreateObject(frNpacket *,octet *,octet);				//					***1205
static  void		HandleDeleteObject(frNpacket *,octet *,octet);				//					***1205
static	bool		HandleDCC(frNpacket* am, octet** rap, octet* rr, octet id, bool* goes_to_hcrrej);		//		***1227
static	void		HandleGetAlarmSummary(frNpacket *,octet);					//					***1205
static	void		HandleGetEventInformation(frNpacket *,octet *,octet);		//					***1205
static	void		HandleReadRange(frNpacket *,octet *,dword,octet,frVbag *);	//					***400
static	void		HandleReadWriteProperty(frNpacket *,octet *,word,octet,frVbag *);
static	void		HandleReadPropertyMultiple(frNpacket *,octet *,word,octet,frVbag *);	//		***1221
static  bool		HandleReinitDevice(frNpacket* am, octet** rap, octet* rr, octet id, bool* goes_to_hcrrej);	//	***1227
static	void		HandleSubscribeCOV(frNpacket *,octet *,word,octet);			//					***240
static	void		HandleSubscribeCOVProperty(frNpacket *am,octet *op,word dsize,octet id);	//	***1223
static	void		HandleUnconfirmedReq(frNpacket *);
static	void		HandleWritePropertyMultiple(frNpacket *am,octet *op,word dsize,octet id,frVbag *vb);	//	***1221
static	void		LocalizeUTCTime(frTimeDate *);
static	void		MakeLocalTimeUTC(frTimeDate *);								//					***210
static	int			ReadWriteOneProperty(frNpacket *,dword,dword,dword,octet,frVbag *,dword *);
static	int			ReadWritePropertyReq(dword,frVbag *,bool);					//					***1205
static	octet 	 	*RSVP(frNpacket *,octet);
static	word		SearchForDevice(dword);										//					***210
static	void		SendAbort(frNpacket *,octet,octet);
static	void		SendError(frNpacket *,octet,octet,word,word);
static	void		SendCACK(frNpacket *,octet,dword,dword,dword,frVbag *);
static  int			SendPrivateTransfer(dword device, frSource *xsrc, frVbag *vb, bool cnf);	//	***1224
static	void		SendSACK(frNpacket *,octet,octet);
static	void		SendReject(frNpacket *,octet,octet);
static	bool		SendWhoIs(dword);											//					***240
static	bool		SkipNLheader(frNpacket *, octet *, octet);					//					***1209
static void			transmit(frNpacket *pkt,word datalen,void* data, byte reqflag, bool isBcast);// ***1210
static	frNpacket	*UnconfirmedRequest(word,octet, bool);						//					***1205
static	frNpacket	*UnconfirmedRequestMAC(frSource *, octet, bool);			//					***403
static	void		UnconfirmedSend(frNpacket *);								//					***240
static	bool		WouldOverflow(frSource *,octet *,octet *,word);				//					***604

// PxGreen added api to check whether requect came from trusted client. 
// BACnet whitlisting LTK-3135
// PX Green Modified - LTK-3872
static bool Is_Request_Trusted (frNpacket *am );

///////////////////////////////////////////////////////////////////////
//	Initialization

void bpublic frStartup(byte port)
{	int		i;

	if (port>=portMax)
		portid=portDefault;									//default
	else
		portid=port;
#ifdef DEBUGVSBHP
	if (portid==portMSTP)
		fraLog("\r\nfrStartup(portMSTP)");
	else if (portid==portEth)
		fraLog("\r\nfrStartup(portEth)");
	else
		fraLog("\r\nfrStartup(portBIP)");
#endif
	commEnabled=true;
	commEnableTimer=0;

	for (i=0;i<maxdevices;i++)
	{	devlist[i].devinst=nodevice;
		devlist[i].src.slen=notheardfrom;
	}
	
	alq.front=alq.rear=NULL;								//empty AL queue
	alq.count=0;
	//semaClear(&alq.mutex);

	confirmq.front=confirmq.rear=NULL;						//	***256 Begin
	confirmq.count=0;
	//semaClear(&confirmq.mutex);								//	***256 End

	fraGetDeviceInfo(&di);									//ask app module for device info
 	npoolInit();
	macInit(portid,&di);

#ifdef DEBUGVSBHP
	fraLog("\r\nDone frStartup");
#endif
}

///////////////////////////////////////////////////////////////////////
//	The main timer loop
//
// in:	actualelapsed	actual time elapsed in ms

void bpublic frWork(byte actualelapsed)
{	
	macWork(portid,actualelapsed);							//let MAC layers have some time
	if(((commEnabled==false) ||								//	***406
		(initiationDisabled))	&&							//	***406
		(commEnableTimer>0))
	{	commEnableTimer-=(long)actualelapsed;
		if(commEnableTimer<=0)
		{	commEnableTimer=0;
			commEnabled=true;
			initiationDisabled=false;						//	***405
		}
	}
	secondtimer-=actualelapsed;								//update second timer
	if(secondtimer<=0)
	{	secondtimer=1000;									//restart timer
		EverySecond();										//do once per second stuff
		Track_COV_registration();
	}
}

///////////////////////////////////////////////////////////////////////						***240 Begin
//	Come here every second

static	void EverySecond(void)
{	word		i;
	frNpacket	*np;
	frVbag		*vb;
#ifdef DEBUGVSBHP
	char		t[128];		//DEBUG
#endif

//admin confirmed request timers and retries
	for(i=0;i<confirmq.count;i++)
	{ 	if(confirmq.count==0) break;						//it's empty now, we're done
		np=GetQ(&confirmq);									//								***256
		if(np->timer>0) np->timer--;						//count down his timer
		if(np->timer==0)									//this one's timed out
		{	if(np->ntries++>=di.apduretries)				//we can't try this one again	***407 post increment
			{	vb=(frVbag *)np->vb;						//point to its Vbag
				if (vb->status==vbsPending)					//App messed with this before we timed out so leave it alone
				{	vb->pdtype=adtError;
					vb->pd.errval=vsbe_timeout;
					vb->status=vbsComplete;					//leave it out of the queue
					fraResponse(vb);
				}
				npoolFreePacket(np);						//								***1204
			}
			else											//we can try this one again
			{	np->timer=di.apdu_timeout;					//refresh timer
				ConfirmedSend(np);							//this will put it back in the queue
			}
		}
		else												//just return it to the queue
		{
#ifdef DEBUGVSBHP
			sprintf(t,"%s %d PutQ %08lX\n",__FILE__,__LINE__,(long unsigned int)&np);
			fraLog(t);
#endif			
			PutQ(&confirmq,np);								//	***256
		}
	}

//admin who-is timers
	for(i=0;i<maxdevices;i++)
	{	if(devlist[i].timer)
			devlist[i].timer--;								//count down non-zero timers
		if(devlist[i].timer==0&&							//if no moratorium and
		   devlist[i].devinst!=nodevice&&					//this slot is used and
		   devlist[i].src.slen==notheardfrom)				//device is still not heard from
		{	if(SendWhoIs(devlist[i].devinst))				//send a ranged Who-Is
				devlist[i].timer=whoismoratorium;			//if successful, don't ask again for a while
		}
	}
}															//								***240 End

///////////////////////////////////////////////////////////////////////
//	Extract the Error Class from a 32-bit encoded error
//
//in:	error	is encoded error
//out:			the error class

word bpublic frGetErrorClass (int error)
{
	return (word)((error>>8)&0xFF);							//								***1200x
}

///////////////////////////////////////////////////////////////////////
//	Extract the Error Code from a 32-bit encoded error
//
//in:	error	is encoded error
//out:			the error code

word bpublic frGetErrorCode (int error)
{
	return (word)(error&0xFF);								//								***1200x
}

///////////////////////////////////////////////////////////////////////						***1205
//	Transmit an Iam packet
//
//in:	dnet		(big endian) 0=local broadcast, FFFF=global, else specific network		***1205
//out:	0 if success
//		else error

int  bpublic frTransmitIAm(word dnet)						//								***1205
{	
octet		*tp;											//								***250 Begin
frNpacket	*np;											//								***1208
	
	if(commEnabled==false) return vsbe_commdisabled;		//comm is disabled				***1200
	if(initiationDisabled) return vsbe_initiationdisabled;	//initiation is disabled		***1200
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0))  return vsbe_slavemodeenabled;		//we're in slave mode			***1223
#endif
	if((np=UnconfirmedRequest(dnet,IAm,false))==NULL)		//								***1205
	{
		fraLog((char*)"UnconfirmedRequest failure");
		return vsbe_resourcesother;							//								***1200
	}
	tp=np->apdu;
	tp=etagOBJECTID(tp,0xC0,di.deviceinstance+((dword)(DEVICE_OBJTYPE)<<22L));		//		***1206
	tp=eUINT(tp,macMaxAPDU(portid));	//max APDU
	tp=eENUM(tp,0x03);					//No segmentation
	tp=eUINT(tp,di.vendor_id);			//Same Vendor ID for everyone
	np->nlen+=(word)(tp-np->apdu);			//add in stuff after header
	UnconfirmedSend(np); 				//broadcast it and release it
	return 0;								
}															//								***250 End

///////////////////////////////////////////////////////////////////////						***1205
//	Transmit an IHave packet
//
//in:	dnet		(big endian) 0=local broadcast, FFFF=global, else specific network		***1205
//		objid		the object identifier
//		oname		pointer to the object name
// out:	0			if success
//					else error 

int  bpublic frTransmitIHave(word dnet, dword objid, frString *oname)
{	
octet		*tp;											//								***250 Begin
frNpacket	*np;
	
	if(commEnabled==false) return vsbe_commdisabled;		//comm is disabled				***1200
	if(initiationDisabled) return vsbe_initiationdisabled;	//initiation is disabled		***1200
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0))  return vsbe_slavemodeenabled;		//we're in slave mode			***1223
#endif
	if((np=UnconfirmedRequest(dnet,IHave,false))==NULL)		//								***1205
	{
		fraLog((char*)"UnconfirmedRequest failure");
		return vsbe_resourcesother;							//								***418
	}
	tp=np->apdu;
	tp=etagOBJECTID(tp,0xC0,di.deviceinstance+((dword)(DEVICE_OBJTYPE)<<22L));	//			***1206
	tp=etagOBJECTID(tp,0xC0,objid);
	tp=eCHARSTRING(tp,oname);
	np->nlen+=(word)(tp-np->apdu);							//add in stuff after header
	UnconfirmedSend(np); 									//broadcast it and release it
	return 0;
}

///////////////////////////////////////////////////////////////////////					***1205 Begin
//	Transmit a WhoHas packet
//
//in:	dnet		(big endian) 0=local broadcast, FFFF=global, else specific network		***1205
//		dlo			nodevice or dev inst low
//		dhi			nodevice or dev inst high
//		objid		the object identifier		(noobject if use name form)
//		oname		pointer to the object name 	(Null if use objid form)
// out:	0			if success
//					else error 

int  bpublic frTransmitWhoHas(word dnet, dword dlo, dword dhi, dword objid, frString *oname)
{	
octet		*tp,*sp;
frNpacket	*np;
	
	if(commEnabled==false) return vsbe_commdisabled;		//comm is disabled
	if(initiationDisabled) return vsbe_initiationdisabled;	//initiation is disabled
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0))  return vsbe_slavemodeenabled;		//we're in slave mode	***1223
#endif
	if((np=UnconfirmedRequest(dnet,whoHas,false))==NULL)
	{
		fraLog((char*)"UnconfirmedRequest failure");
		return vsbe_resourcesother;
	}
	tp=np->apdu;
	if(dlo!=nodevice&&dhi!=nodevice)						//encode range
	{
		tp=eDWORD(tp,dlo,0x08,false);
		tp=eDWORD(tp,dhi,0x18,false);

	}
	if(objid!=noobject)										//use objid form
		tp=etagOBJECTID(tp,0x28,objid);
	else													//use string form
	{
		sp=tp;
		tp=eCHARSTRING(tp,oname);
		*sp=(*sp&0x07)+0x38;								//fix up as context tag 3
	}
	np->nlen+=(word)(tp-np->apdu);							//add in stuff after header
	UnconfirmedSend(np); 									//broadcast it and release it
	return 0;
}															//							***1205 End

///////////////////////////////////////////////////////////////////////					***002 Begin
//	Define a BACnet device so we can do client stuff
//
// in:	dp		a pointer to an frBACnetdevice struct
// out:	true	if success

bool bpublic frcDefineDevice(frBACnetdevice *dp)
{	word	i,j=maxdevices;
	dword	device=dp->devinst&0x3FFFFFL;			//									***003

	for (i=0;i<maxdevices;i++)						//search for the device in our list
	{	if (devlist[i].devinst==device) return true;
		if ((j==maxdevices)&&(devlist[i].devinst==nodevice)) j=i;	//first empty
	}
	if (j==maxdevices) return false;				//couldn't find an empty
	devlist[j]=*dp;
	devlist[j].devinst&=0x3FFFFFL;					//make sure							***003
	return true;
}

///////////////////////////////////////////////////////////////////////					***1224 Begin
//	Transmit an UnconfirmedPrivateTransfer request on our vendor id
//
//  in:	device		the device instance to transfer to (if nodevice then src has frSource)
//		xsrc		the alternative pointer to an frSource (may be NULL if device !=nodevice)
//		svc			the service code
//		params		pointer to the service parameters (or NULL if nbytes==0)
//		nbytes		the number of octets in params
// out:	0			ok
//		else		reason we failed
//				NOTE: vsbe_resourcesother		indicates tx buffer is not free
//				      vsbe_unknowndevice		indicates device has not yet sent an Iam
//					  vsbe_parameteroutofrange	indicate parameter block is too long
//NOTE: if device is nodevice AND xsrc is NULL, then this will result a globally broadcast
//			UnconfirmedPrivateTransfer

int bpublic frcUnconfirmedPrivateTransfer(dword device, frSource *xsrc, frVbag *vb)
{
	return SendPrivateTransfer(device, xsrc, vb, false);
}													//									***1224 End

///////////////////////////////////////////////////////////////////////					***1224 Begin
//	Transmit a ConfirmedPrivateTransfer packet
//
//in:	device	the device instance to transfer to (if nodevice then src has frSource)
//		xsrc	the alternative pointer to an frSource (may be NULL if device !=nodevice)
//		vb		points to a frVbag with transfer parameters and to receive result
//out:	0		if success
//		else	error 
//				NOTE: vsbe_resourcesother		indicates tx buffer is not free
//				      vsbe_unknowndevice		indicates device has not yet sent an Iam
//					  vsbe_parameteroutofrange	indicate parameter block is too long
//NOTE: Either device must be nodevice and xsrc contains the populated frSource
//      or device must be a valid device instance

int	bpublic frcConfirmedPrivateTransfer(dword device, frSource *xsrc, frVbag *vb)
{
	return SendPrivateTransfer(device, xsrc, vb, true);
}													//									***1224 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Transmit a ReadProperty packet
//
// in:	device	the device instance containing the object to be read
//		objid	the object identifier
//		propid	the property identifier
//		aindex	the array index or the special value noindex
//		vb		points to a frVbag to receive the property value/or to write the property value

int	bpublic frcReadProperty(dword device, dword objid, dword propid, dword aindex, frVbag *vb)
{
	vb->objid=objid;								//copy objid,propid,arrindex		***1205 Begin
	vb->propid=propid;
	vb->arrindex=aindex;
	return ReadWritePropertyReq(device,vb,true);	//									***1205 End
}

int  bpublic frcReadPropertyEx(dword device, frVbag *vb)	//							***1205 Begin
{	
	return ReadWritePropertyReq(device,vb,true);
}													//									***1205 End

///////////////////////////////////////////////////////////////////////
//	Transmit a WriteProperty packet
//
// in:	device	the device instance containing the object to be written
//		objid	the object identifier
//		propid	the property identifier
//		aindex	the array index or the special value noindex
//		vb		points to a frVbag to receive the property value/or to write the property value

int  bpublic frcWriteProperty(dword device, dword objid, dword propid, dword aindex, frVbag *vb)
{	
	vb->objid=objid;								//copy objid,propid,arrindex		***1205 Begin
	vb->propid=propid;
	vb->arrindex=aindex;
	return ReadWritePropertyReq(device,vb,false);	//									***1205 End
}

int  bpublic frcWritePropertyEx(dword device, frVbag *vb)	//							***1205 Begin
{	
	return ReadWritePropertyReq(device,vb,false);
}													//									***1205 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Common Processor to transmit a ReadProperty/WriteProperty packet
//
// in:	device			the device instance containing the object to be read/written
//		vb->objid		the object identifier
//		vb->propid		the property identifier
//		vb->arrindex	the array index or the special value noindex
//		vb				points to a frVbag to receive the property value/or to write the property value
//		readflag		true if read/else write
//	out:	0			if success														***216 Begin
//			else		error 
//					NOTE: vsbe_resourcesother indicates tx buffer is not free
//						  vsbe_unknowndevice indicates device has not yet sent an Iam	***216 End

static	int	ReadWritePropertyReq(dword device, frVbag *vb, bool readflag)	//			***1205
{
frNpacket	*np;
word	i;													//							***200
dword	*dw;
octet	*tp;												//							***200
int		r;

	if(commEnabled==false)
		return vsbe_servicerequestdenied;					//can't transmit right now
	if(initiationDisabled) return vsbe_initiationdisabled;	//							***400
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;		//we're in slave mode		***1223
#endif
	if((np=ConfirmedRequest(device,(octet)((readflag)?readProperty:writeProperty),true,&r,vb))==NULL) //try for a packet
		return r;											//unknown device or can't get one right now
	tp=np->apdu;
	tp=etagOBJECTID(tp,0x08,vb->objid);						//							***1205
	tp=eDWORD(tp,vb->propid,0x18,false);					//							***1205
	if (vb->arrindex!=noindex)								//							***1205
		tp=eDWORD(tp,vb->arrindex,0x28,false);				//							***1205
	if (!readflag)
	{	// encode value
		*tp++=0x3E;											//context open tag 3
		switch(vb->pdtype)
		{	case adtUnsigned:
				tp=eUINT(tp,vb->pd.uval);
				break;
			case adtInteger:
				tp=eINT(tp,vb->pd.sval);
				break;
			case adtReal:
				tp=eREAL(tp,0x44,vb->pd.fval);
				break;
			case adtDouble:
				#if FLOATLIB								//							***002 Begin
				tp=eDOUBLE(tp,&vb->pd.dval);
				#else
				tp=eDOUBLE(tp,&vb->pd.dval[0]);
				#endif										//							***002 End
				break;
			case adtBoolean:
				tp=eBOOL(tp,(bool)vb->pd.uval);
				break;
			case adtObjectID:
				tp=etagOBJECTID(tp,0xC0,vb->pd.uval);
				break;
			case adtEnumerated:
				tp=eENUM(tp,vb->pd.uval);
				break;
			case adtCharString:
				tp=eCHARSTRING(tp,&vb->pd.stval);
				break;
			case adtBitString:
				tp=eBITSTRING(tp,vb->ps.psval,0x80,(word)vb->pd.uval);	//				***005
				break;
			case adtOctetString:
				tp=eOCTETSTRING(tp,vb->ps.psval,(word)vb->pd.uval);		//				***005
				break;
			case adtDate:
				tp=eDATESTRING(tp,(word)vb->pd.dtval.year,vb->pd.dtval.month,vb->pd.dtval.day,vb->pd.dtval.dow);
				break;
			case adtTime:
				tp=eTIMESTRING(tp,vb->pd.tval.hour,vb->pd.tval.minute,vb->pd.tval.second,vb->pd.tval.hundredths);
				break;
 			case adtNull:
 				tp=eNULL(tp);
 				break;
			case adtObjectList:
				dw=vb->ps.psdval;							//							***005
				for(i=0;i<vb->pd.uval;i++)
				{	tp=etagOBJECTID(tp,0xC0,*dw);
					dw++;
				}
				break;
			case adtEmptyList:
				break;
 		}
		*tp++=0x3F;											//context close tag 3
		if (vb->priority)									//							***211
			tp=eDWORD(tp,(dword)vb->priority,0x48,false);	//							***211
	}
	vb->status=vbsPending;
	np->nlen+=(word)(tp-np->apdu);								//add in stuff after header
	ConfirmedSend(np); 										//send it
	return 0;												//success
}															//							***240 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Look through our device list for a specific device instance.
//	If the device is not found in the list, then the least recently used
//	list slot (maxdevices-1) is overwritten with the new device
//	instance and the slot is marked as notyetheardfrom. 
//  If the device is found in the list, its slot is promoted to the
//	front of the list.
//
// in:	device			the device instance
// out:	0				we know this device and have heard from it
//		notheardfrom	not heard from yet

static	word SearchForDevice(dword device)
{	word			i,j;
	static frBACnetdevice	d;

	device&=0x3FFFFFL;										//make sure it's a reasonable device instance
	for (i=0;i<maxdevices;i++)								//search for the device in our list
	{	if (devlist[i].devinst==device)
		{	d=devlist[i];									//we found our man 
			for(j=i;j>0;j--)								//push other slots down
				devlist[j]=devlist[j-1];
			devlist[0]=d;									//promote him to the front
			goto sfd1;
		}
	}
	for(j=maxdevices-1;j>0;j--)								//push other slots down
		devlist[j]=devlist[j-1];
	devlist[0].devinst=device;								//mark slot used
	devlist[0].src.slen=notheardfrom;						//we need to do WhoIs to him
	devlist[0].timer=0;										//he will Who-Is immediately
sfd1:
	if(devlist[0].src.slen==notheardfrom)
		return notheardfrom;
	return 0;												//							***240 End
}

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Transmit an Unsolicited/UnconfirmedCOV request
//
// in:	cp		points to an frCOV describing the COV to be transmitted
// out:	0							accepted
//		vsbe_resourcesother			indicates tx buffer is not free						***216
//		vsbe_invaliddatatype		if notify parameters contains an invalid datatype
//		vsbe_servicerequestdenied	comm is disabled due to DCC

int bpublic frcUnsolicitedCOV(frCOV	*cp)
{	
static frSubscribeCOV	sp;									//							***414
word					i;

	if(commEnabled==false)									//							***400 Begin
		return vsbe_servicerequestdenied;					//can't transmit right now
	if(initiationDisabled) return vsbe_initiationdisabled;	//							***400 End
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;		//we're in slave mode		***1223
#endif
	byteset((byte*)&sp,0x00,sizeof(frSubscribeCOV));		//make sure it's 0x00		***1200
	sp.confirmed=false;
	sp.processid=0;											//unsubscribed
	sp.objid=cp->objid;
	sp.numberpvs=cp->numberpvs;
	for(i=0;i<cp->numberpvs;i++)
		sp.propval[i]=cp->propval[i];
	return frcCOVNotify(&sp,NULL);							//							***240 End
}

///////////////////////////////////////////////////////////////////////					***1206 Begin
//	Transmit a SubscribeCOV request
//
// in:	vb		points to a frVbag structure
//		vb->ps.cs[0]							is the COV request we will use
//		vb->ps.cs[0].procid						is your subscribing process ID
//		vb->ps.cs[0].recip.recipient.device		the device to subscribe to (ONLY DEVICE FORM SUPPORTED!)
//		vb->ps.cs[0].propref.objid				the objectID to subscribe to
//		vb->ps.cs[0].propref.propid				the property ID for a SubscribeCOVProperty or
//												noproperty means do SubscribeCOV
//		vb->ps.cs[0].timeremaining				0 means cancel any outstanding subscription
//												else lifetime in seconds of subscription
//		vb->ps.cs[0].increment					noincrement means use default, else use this one
//		vb->ps.cs[0].issue						1 if confirmed notifications needed, 0=unconf
//
// out:	0								accepted
//		vsbe_resourcesother				indicates tx buffer is not free
//		vsbe_unknowndevice				indicates device has not yet sent an Iam
//		vsbe_initiationdisabled			indicates we are DCC disabled
//		vsbe_slavemodeenabled			indicates we are a slave and can't initiate anything

int bpublic frcSubscribeCOV(frVbag *vb)
{
frNpacket	*np;
octet		*tp,svc;
int			r;


	if(commEnabled==false)
		return vsbe_servicerequestdenied;					//can't transmit right now
	if(initiationDisabled) return vsbe_initiationdisabled;
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;		//we're in slave mode	***1223
#endif
	svc=(vb->ps.cs[0].propref.propid==noproperty)?subscribeCOV:subscribeCOVProperty;
	if((np=ConfirmedRequest(vb->ps.cs[0].recip.recipient.device,svc,true,&r,vb))==NULL) //try for a packet
		return r;											//unknown device or can't get one right now
	tp=np->apdu;
	tp=eDWORD(tp,vb->ps.cs[0].procid,0x08,false);			//processID
	tp=etagOBJECTID(tp,0x18,vb->ps.cs[0].propref.objid);	//objectID
	if(vb->ps.cs[0].timeremaining!=0)						//it's a subscription, not a 0==cancel
	{
		tp=eDWORD(tp,vb->ps.cs[0].issue,0x28,false);		//confirmed yes or no
		tp=eDWORD(tp,vb->ps.cs[0].timeremaining,0x38,false); //lifetime
		if(vb->ps.cs[0].propref.propid!=noproperty)			//it's a COVproperty
		{
			*tp++=0x4E;										//property reference
			tp=eDWORD(tp,vb->ps.cs[0].propref.propid,0x08,false);	//propertyID
			//			tp=eDWORD(tp,vb->ps.cs[0].propref.arrayindex,0x18,false); //we don't support array indices in these subscriptions
			*tp++=0x4F;
			if(vb->ps.cs[0].increment!=noincrement)			//also send an increment
				tp=eREAL(tp,0x5C,vb->ps.cs[0].increment);	//encode real tagged item
		}
	}
	else 
	{
		if(vb->ps.cs[0].propref.propid!=noproperty)			//it's a COVproperty
		{
			*tp++=0x4E;										//property reference
			tp=eDWORD(tp,vb->ps.cs[0].propref.propid,0x08,false);	//propertyID
			*tp++=0x4F;
		}
	} 
	vb->status=vbsPending;
	np->nlen+=(word)(tp-np->apdu);							//add in stuff after header
	ConfirmedSend(np); 										//send it
	return 0;												//success
}															//							***1206 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Transmit an Confirmed/UnconfirmedCOV request
//
// in:	cp		points to an frSubscribeCOV describing the COV to be transmitted
//			cp->src		is the subscriber's MAC address info
//		vb		points to a frVbag to receive the response data
// out:	0							accepted
//		vsbe_resourcesother			indicates tx buffer is not free						***216
//		vsbe_invaliddatatype		if notify parameters contains an invalid datatype
//		vsbe_servicerequestdenied	comm is disabled due to DCC

int bpublic frcCOVNotify(frSubscribeCOV *cp, frVbag *vb)
{	frNpacket	*np;
	octet		*tp;
	int			r;

	if(commEnabled==false)
		return vsbe_servicerequestdenied;						//can't transmit right now
	if(initiationDisabled) return vsbe_initiationdisabled;							//	***400
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;			//we're in slave mode				***1223
#endif
	cp->deviceid=((dword)DEVICE_OBJTYPE<<22L)+di.deviceinstance;	//setup our device objectID		***1206
	if(cp->confirmed)
	{		
		if((np=ConfirmedRequestMAC(&cp->src,confirmedCOVNotification,true,&r,vb))==NULL) //try for a packet
		{
			return r;											//unknown device or can't get one right now
		}

		tp=np->apdu;
		if((tp=EncodeCOVNotifyParameters(tp,cp->processid,cp->deviceid,cp->objid,cp->time,cp->propval,cp->numberpvs))==NULL)
		{	
#ifdef DEBUGVSBHP														//	***256 Begin
			sprintf(dbg,"%s calling npoolFreePacket\n",__FUNCTION__);
			fraLog(dbg);												//	***256 End
#endif
			npoolFreePacket(np);										//done with it
			return vsbe_invaliddatatype;
		}
		vb->status=vbsPending;
		np->nlen+=(word)(tp-np->apdu);								//add in stuff after header
		ConfirmedSend(np); 										//send it
	}
	else
	{			
		if((np=UnconfirmedRequestMAC(&cp->src,unconfirmedCOVNotification,false))==NULL) //try for a packet	***416		
			return vsbe_resourcesother;							//can't get one right now
		tp=np->apdu;
		if((tp=EncodeCOVNotifyParameters(tp,cp->processid,cp->deviceid,cp->objid,cp->time,cp->propval,cp->numberpvs))==NULL)
		{
			npoolFreePacket(np);										//done with it	***416
			return vsbe_invaliddatatype;
		}
		if(vb!=NULL) vb->status=vbsComplete;					//say it finished already
		np->nlen+=(word)(tp-np->apdu);								//add in stuff after header
		UnconfirmedSend(np); 									//send it
	}
	return 0;													//success
}																//					***240 End

///////////////////////////////////////////////////////////////////////			//	***1200 Begin
//	Transmit an Confirmed/UnconfirmedCOV request
//
// in: 	device		the device to send the COV to
//		cp			points to an frSubscribeCOV describing the COV to be transmitted
//		vb		points to a frVbag to receive the response data
//
// out:	0				accepted 
//	vsbe_resourcesother		indicates no tx buffer is available
//	vsbe_invaliddatatype		if notify parameters contains an invalid datatype
//	vsbe_servicerequestdenied	comm is disabled due to DCC

int bpublic frcCOVNotifyDevice(dword device,frSubscribeCOV *cp, frVbag* vb)
{	word	dvx;

	if(commEnabled==false)
		return vsbe_servicerequestdenied;						//can't transmit right now
	if(initiationDisabled) return vsbe_initiationdisabled;							
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;			//we're in slave mode	***1223
#endif
	if ((dvx=SearchForDevice(device&0x3FFFFF))>=maxdevices)		//remember address info
		return vsbe_unknowndevice;								//failed
	cp->src=devlist[dvx].src;
	return frcCOVNotify(cp,vb);
}																				//	***1200 End

///////////////////////////////////////////////////////////////////////
//	Encode the body of a COV Notify request
//
// in:	tp			points to the buffer to encode into (assumes there's enough space)
//		processid	the process identifier
//		deviceid	the device id
//		objid		the object id
//		timeremaining	the time remaining
//		pv			pointer to an array of frPropVals
//		n			the count of propvals in pv
// out:	tp			advanced or NULL if failed

static	octet *EncodeCOVNotifyParameters(octet *tp,dword processid,dword deviceid,dword objid,dword timeremaining,frPropVal *pv,word n)
{	word	i;
	octet *bpp;
	tp=eDWORD(tp,processid,0x08,false);				//[0]process identifier
	tp=eDWORD(tp,deviceid,0x18,false);				//[1]initiating device instance
	tp=etagOBJECTID(tp,0x28,objid);					//[2]monitored object id
	tp=eDWORD(tp,timeremaining,0x38,false);			//[3]time remaining
	*tp++=0x4E;										//[4]opening tag list of values
	for (i=0;i<n;i++)
	{	tp=eDWORD(tp,pv->propid,0x08,false);		//[0]property id
		if (pv->arrayindex!=noindex)	
			tp=eDWORD(tp,pv->arrayindex,0x18,false);//[1]arrayindex
		*tp++=0x2E;									//[2]opening tag value
		switch(pv->value.pdtype)
		{	case adtUnsigned:
				tp=eUINT(tp,pv->value.pd.uval);
				break;
			case adtInteger:
				tp=eINT(tp,pv->value.pd.sval);
				break;
			case adtReal:
				tp=eREAL(tp,0x44,pv->value.pd.fval);
				break;
			case adtDouble:
				#if FLOATLIB
				tp=eDOUBLE(tp,&pv->value.pd.dval);
				#else
				tp=eDOUBLE(tp,&pv->value.pd.dval[0]);
				#endif
				break;
			case adtBoolean:
				tp=eBOOL(tp,(bool)pv->value.pd.uval);
				break;
			case adtObjectID:
				tp=etagOBJECTID(tp,0xC0,pv->value.pd.uval);
				break;
			case adtEnumerated:
				tp=eENUM(tp,pv->value.pd.uval);
				break;
			case adtBitString:
				//tp=eBITSTRING(tp,pv->value.pd.psval,0x80,(word)pv->value.pd.uval);
				tp=eBITSTRING(tp,pv->value.pd.psval,0x80,(word)pv->value.len);				//	***407
				break;
			case adtOctetString:
				//tp=eOCTETSTRING(tp,pv->value.pd.psval,(word)pv->value.pd.uval);
				tp=eOCTETSTRING(tp,pv->value.pd.psval,(word)pv->value.len);					//	***407
				break;
			case adtDate:
				tp=eDATESTRING(tp,(word)pv->value.pd.dtval.year,pv->value.pd.dtval.month,pv->value.pd.dtval.day,pv->value.pd.dtval.dow);
				break;
			case adtTime:
				tp=eTIMESTRING(tp,pv->value.pd.tval.hour,pv->value.pd.tval.minute,pv->value.pd.tval.second,pv->value.pd.tval.hundredths);
				break;
			case adtTimeStamp:						//										***1200 Begin				
				switch(pv->value.pd.ts.type)
				{
				case 0:													// [0]time
					bpp=tp;
					tp=eTIMESTRING(tp,pv->value.pd.ts.time.hour,pv->value.pd.ts.time.minute,pv->value.pd.ts.time.second,pv->value.pd.ts.time.hundredths);
					*bpp=(*bpp&0x07)+0x08;								//tag 0
					break;
				case 1:													// [1]sequence
					tp=eDWORD(tp,pv->value.pd.ts.sequence,0x18,false);
					break;
				case 2:													// [2]bacnetdatetime
					*tp++=0x2E;											// open tag
					tp=eDATESTRING(tp,pv->value.pd.ts.date.year,pv->value.pd.ts.date.month,pv->value.pd.ts.date.day,pv->value.pd.ts.date.dow);
					tp=eTIMESTRING(tp,pv->value.pd.ts.time.hour,pv->value.pd.ts.time.minute,pv->value.pd.ts.time.second,pv->value.pd.ts.time.hundredths);
					*tp++=0x2F;											// close tag
					break;
				default:
					return NULL;
				}								
				break;								//										***1200 End
 			case adtNull:
 				tp=eNULL(tp);
 				break;
			case adtEmptyList:
				break;
			default:
				return NULL;
 		}
		*tp++=0x2F;									//[2]closing tag value
		pv++;
	}
	*tp++=0x4F;										//[4]closing tag list of values
	return tp;
}

///////////////////////////////////////////////////////////////////////					***1206 Begin
//	Encode the LogRecord for a trend log buffer
//
// in:	tp			points to the buffer to encode into (assumes there's enough space)
//		pv			pointer to an array of frBACnetLogRecord
// out:	tp			advanced or NULL if failed

static	octet *EncodeLogRecord(octet* tp,frBACnetLogRecord* pv)
{
	*tp++=0x0E;
	tp=eDATESTRING(tp,(word)pv->timestamp.year, pv->timestamp.month, pv->timestamp.day, pv->timestamp.dow);
	tp=eTIMESTRING(tp,pv->timestamp.hour,pv->timestamp.minute,pv->timestamp.second,pv->timestamp.hundredths);
	*tp++=0x0F;
	*tp++=0x1E;
	switch(pv->datatype)
	{
	case 0:			
		tp=eBITSTRING(tp,&pv->logDatum.logstatus,0x08,3);	//[0] BACnetLogStatus	//	***1200
		break;
	case 1:	
		*tp++=0x19;											//[1] Boolean
		*tp++=(pv->logDatum.bval)?0x01:0x00;		
		break;
	case 2:		
		tp=eREAL(tp,0x2C,pv->logDatum.fval);				//[2] Real			-- 32 bits
		break;
	case 3:
		tp=eDWORD(tp,pv->logDatum.eval,0x38,false);			//[3] Enumerated	--32 bits
		break;
	case 4:
		tp=eDWORD(tp,pv->logDatum.uval,0x48,false);			//[4] unsigned		--32 bits
		break;
	case 5:
		tp=eDWORD(tp,pv->logDatum.sval,0x58,true);			//[5] signed		--32 bits
		break;
	case 6:
		tp=eBITSTRING(tp,pv->logDatum.bitval,0x68,pv->sigbits);	//[6] bitstring --32 bits
		break;
	case 7:
		*tp++=0x78;											//[7] NULL, no contents
		break;
	case 8:
		*tp++=0x8E;											//[8] failure		
		tp=eUINT(tp,pv->logDatum.failure[0]);				//error class
		tp=eUINT(tp,pv->logDatum.failure[1]);				//error code		
		*tp++=0x8F;											//End [8]
		break;
	case 9:
		tp=eREAL(tp,0x9C,pv->logDatum.timechange);			//[9] time change (real)
		break;
	default:
		break;
	}
	*tp++=0x1F;
	pv++;													//point to next log record
	return tp;
}															//							***1206 End

///////////////////////////////////////////////////////////////////////					***1206 Begin
//	Encode the EventLogRecord for an event log buffer
//
// in:	tp			points to the buffer to encode into (assumes there's enough space)
//		pv			pointer to an array of frBACnetEventLogRecord
// out:	tp			advanced or NULL if failed

static	octet *EncodeEventLogRecord(octet* tp,frBACnetEventLogRecord* pv)
{
dword	ourdevid = di.deviceinstance+((dword)(DEVICE_OBJTYPE)<<22L);
octet	*sp;																						//	***1216

	*tp++=0x0E;
	tp=eDATESTRING(tp,(word)pv->timestamp.year, pv->timestamp.month, pv->timestamp.day, pv->timestamp.dow);
	tp=eTIMESTRING(tp,pv->timestamp.hour,pv->timestamp.minute,pv->timestamp.second,pv->timestamp.hundredths);
	*tp++=0x0F;
	*tp++=0x1E;
	switch(pv->elrType)
	{
	case 0:			
		tp=eBITSTRING(tp,&pv->elrStatus,0x08,3);			//[0] BACnetLogStatus
		break;
	case 1:	
		*tp++=0x1E;											//[1] ConfirmedEventNotification-Request
		tp=eDWORD(tp,pv->processID,0x08,false);				//   [0] unsigned processID
		tp=eDWORD(tp,ourdevid,0x18,false);					//   [1] BACnetObjectIdentifier initiatingDeviceObjectID
		tp=eDWORD(tp,pv->eventObjectID,0x28,false);			//   [2] BACnetObjectIdentifier eventObjectID
		*tp++=0x3E;											//   [3] time stamp always datetime type
		*tp++=0x2E;											//	    [2] date time
		tp=eDATESTRING(tp,(word)(pv->timestamp.year+1900),pv->timestamp.month,pv->timestamp.day,pv->timestamp.dow);
		tp=eTIMESTRING(tp,pv->timestamp.hour,pv->timestamp.minute,pv->timestamp.second,pv->timestamp.hundredths);
		*tp++=0x2F;
		*tp++=0x3F;
		tp=eDWORD(tp,pv->nc,0x48,false);					//   [4] unsigned notification class
		tp=eDWORD(tp,pv->priority,0x58,false);				//   [5] unsigned priority
		tp=eDWORD(tp,pv->eventType,0x68,false);				//   [6] BACnetEventType
		if(pv->msgh.len>0)									//   [7] CharacterString message		//	***1216 Begin
		{
			sp=tp;			
			tp=eCHARSTRING(tp,&pv->msgh);
			*sp=(*sp&0x7)+0x78;								//	 make it context tagged
		}																							//	***1216 End
		*tp++=0x89;											//   [8] notify type
		*tp++=vsb_notifytype_alarm;							//			alarm		
		if(pv->ackReqd)
		{
			*tp++=0x99;										//   [9] boolean Ack Reqd
			*tp++=1;
		}
		tp=eDWORD(tp,pv->fromState,0xA8,false);				//   [10] unsigned fromState
		tp=eDWORD(tp,pv->toState,0xB8,false);				//   [11] unsigned toState
		*tp++=0xCE;											//   [12] event parameters
		switch(pv->eventType)
		{
			case entypeCOB:
				*tp++=0x0E;									//       [0] COB
				tp=eBITSTRING(tp,pv->npel.COB.bitstring,0x08,pv->npel.COB.usedbits);	//[0]bit string
				tp=eBITSTRING(tp,&pv->npel.COB.StatusFlags,0x18,4);					//[1]status flags
				*tp++=0x0F;
				break;
			case entypeCOS:
				*tp++=0x1E;									//       [1] COS
				*tp++=0x0E;									//          [0] new state
				tp=eDWORD(tp,pv->npel.COS.newstate,(byte)(((pv->npel.COS.propertystates<<4)&0xF0)+8),false);	//[x] new state
				*tp++=0x0F;
				tp=eBITSTRING(tp,&pv->npel.COS.StatusFlags,0x18,4);	//  [1]status flags
				*tp++=0x1F;
				break;
			case entypeCOV:
				*tp++=0x2E;									//        [2] COV
				*tp++=0x0E;									//           [0] value or bits
				if (pv->npel.COV.valueorbits==1)
					tp=eREAL(tp,0x18,pv->npel.COV.changedvalue); //          [1] changed value
				else										//              [0] changed bit string
					tp=eBITSTRING(tp,pv->npel.COV.changedbitstring,0x08,pv->npel.COV.usedbits);
				*tp++=0x0F;
				tp=eBITSTRING(tp,&pv->npel.COV.StatusFlags,0x18,4); //    [1] status flags
				*tp++=0x2F;
				break;
			case entypeCF:									//		  [3] CF
				*tp++=0x3E;	
				tp=eDWORD(tp,pv->npel.CF.commandvalue,0x08,false);		//[0]command value
				tp=eBITSTRING(tp,&pv->npel.CF.StatusFlags,0x18,4);		//[1]status flags
				tp=eDWORD(tp,pv->npel.CF.feedback,0x28,false);			//[2]command feedback
				*tp++=0x3F;
				break;
			case entypeFL:
				*tp++=0x4E;									//        [4] FL
				tp=eREAL(tp,0x08,pv->npel.FL.refvalue);					//[0]reference value
				tp=eBITSTRING(tp,&pv->npel.FL.StatusFlags,0x18,4);		//[1]status flags
				tp=eREAL(tp,0x28,pv->npel.FL.setpointvalue);				//[2]setpoint value
				tp=eREAL(tp,0x38,pv->npel.FL.errorlimit);				//[3]errorlimit value
				*tp++=0x4F;
				break;
			case entypeOOR:
				*tp++=0x5E;									//        [5] OOR
				tp=eREAL(tp,0x08,pv->npel.OOR.exceedingvalue);			//[0]exceeding limit
				tp=eBITSTRING(tp,&pv->npel.OOR.StatusFlags,0x18,4);		//[1]status flags
				tp=eREAL(tp,0x28,pv->npel.OOR.deadband);					//[2]deadband
				tp=eREAL(tp,0x38,pv->npel.OOR.exceededlimit);			//[3]exceeded limit
				*tp++=0x5F;
				break;
			case entypeBR:
				*tp++=0xAE;									//        [10] BR		***1207 Begin
				*tp++=0x0E;														//[0]buffer-property
#ifdef ESP32_SETUP
				tp=etagOBJECTID(tp,0x08,pv->npel.THPBR.bufferProperty.objid);
				tp=eDWORD(tp,pv->npel.THPBR.bufferProperty.propid,0x18,false);
				if(pv->npel.THPBR.bufferProperty.arrayindex!=noindex)
					tp=eDWORD(tp,pv->npel.THPBR.bufferProperty.arrayindex,0x28,false);
				if(pv->npel.THPBR.bufferProperty.deviceid!=nodevice)
					tp=eDWORD(tp,pv->npel.THPBR.bufferProperty.deviceid,0x38,false);
				*tp++=0x0F;
				tp=eDWORD(tp,pv->npel.THPBR.previousNotification,0x18,false);		//[1]previous-notification
				tp=eDWORD(tp,pv->npel.THPBR.currentNotification,0x28,false);		//[2]current-notification
				*tp++=0xAF;									//						***1207 End
				/*Changing struct name from BR to THPBR cos ESP32 is having special register name as 'BR'*/
#else
				tp=etagOBJECTID(tp,0x08,pv->npel.BR.bufferProperty.objid);
				tp=eDWORD(tp,pv->npel.BR.bufferProperty.propid,0x18,false);
				if(pv->npel.BR.bufferProperty.arrayindex!=noindex)
					tp=eDWORD(tp,pv->npel.BR.bufferProperty.arrayindex,0x28,false);
				if(pv->npel.BR.bufferProperty.deviceid!=nodevice)
					tp=eDWORD(tp,pv->npel.BR.bufferProperty.deviceid,0x38,false);
				*tp++=0x0F;
				tp=eDWORD(tp,pv->npel.BR.previousNotification,0x18,false);		//[1]previous-notification
				tp=eDWORD(tp,pv->npel.BR.currentNotification,0x28,false);		//[2]current-notification
				*tp++=0xAF;									//						***1207 End
#endif
				break;
		}
		*tp++=0xCF;
		*tp++=0x1F;
		break;
	case 2:		
		tp=eREAL(tp,0x2C,pv->timechange);					//[2] Real			-- 32 bits
		break;
	default:
		break;
	}
	*tp++=0x1F;
	pv++;													//point to next event log record
	return tp;
}															//							***1206 End

///////////////////////////////////////////////////////////////////////					***400 Begin
//	Encode the body of a time value pair
//
// in:	tp			points to the buffer to encode into (assumes there's enough space)
//		pv			pointer to a frBACnetTimeValue
// out:	tp			advanced or NULL if failed

static	octet *EncodeTimeValue(octet* tp,frBACnetTV *pv)
{
	octet *pp=tp;	
	
	pp=eTIMESTRING(pp,pv->time.hour,pv->time.minute,
		pv->time.second,pv->time.hundredths);
	switch(pv->value.pdtype)
	{
	case adtEnumerated:
		pp=eENUM(pp,pv->value.tvpd.uval);	
		break;	
	case adtUnsigned:			
		pp=eUINT(pp,pv->value.tvpd.uval);
		break;
	case adtInteger:				
		pp=eINT(pp,pv->value.tvpd.sval);	
		break;
	case adtBoolean:				
		pp=eBOOL(pp,(bool)pv->value.tvpd.uval);
		break;
	case adtDouble:
#ifdef FLOATLIB							
		pp=eDOUBLE(pp,&pv->value.tvpd.dval);
#else
		pp=eDOUBLE(pp,&pv->value.tvpd.dval[0]);
#endif									
		break;
	case adtReal:
		pp=eREAL(pp,0x44,pv->value.tvpd.fval);	
		break;
	case adtBitString:
		pp=eBITSTRING(pp,pv->value.tvpd.psval,0x80,pv->value.sigbits);
		break;
	case adtOctetString:
		pp=eOCTETSTRING(pp,pv->value.tvpd.psval,pv->value.sigbits);
		break;
   	case adtNull:																//		***1223 Begin
       pp = eNULL(pp);
       break;																	//		***1223 End
	default:
		break;
	}	
	return pp;
}

///////////////////////////////////////////////////////////////////////					***1206 Begin
//	Encode a BACnetCalendarEntry for a date list
//
// in:	tp			points to the buffer to encode into (assumes there's enough space)
//		pv			pointer to an array of frCalendarEntry
// out:	tp			advanced or NULL if failed

static	octet *EncodeCalendarEntry(octet* tp,frBACnetCalendarEntry* pv)
{
octet *sp;

	if(pv->type==0)
	{
		sp=tp;									//save the starting pointer
		tp=eDATESTRING(tp,pv->dt.year,pv->dt.month,pv->dt.day,pv->dt.dow);	//[0] Date
		*sp&=7;									//get the encoded length
		*sp+=0x08;								//make it context tag 0
	}
	else if (pv->type==1)
	{
		*tp++=0x1E;															//[1] DateRange
		tp=eDATESTRING(tp,pv->dr.start.year,pv->dr.start.month,pv->dr.start.day,pv->dr.start.dow);
		tp=eDATESTRING(tp,pv->dr.end.year,pv->dr.end.month,pv->dr.end.day,pv->dr.end.dow);
		*tp++=0x1F;
	}
	else if(pv->type==2)													//[2] BACnetWeekNDay
	{
		sp=tp;									//save the starting pointer
		tp=eOCTETSTRING(tp,pv->wnd,3);			//let the encoder do it's work
		*sp&=7;									//get the encoded length
		*sp+=0x28;								//make it context tag 2
	}
    else																			//	***1223 Begin
    {
	   //do nothing
    }																				//	***1223 End
	pv++;										//point to next cal entry
	return tp;
}												//										***1206 End

///////////////////////////////////////////////////////////////////////					***1206 Begin	
//	Encode a BACnetDestination for a recipient list
//
// in:	tp			points to the buffer to encode into (assumes there's enough space)
//		pv			pointer to an array of frBACnetDestination
// out:	tp			advanced or NULL if failed

static	octet *EncodeDestination(octet* tp,frBACnetDestination* pv)
{			
	tp=eBITSTRING(tp,&pv->validDays,0x80,7);	//always 7 significant bits
	tp=eTIMESTRING(tp,pv->fromTime.hour,pv->fromTime.minute,pv->fromTime.second,pv->fromTime.hundredths);
	tp=eTIMESTRING(tp,pv->toTime.hour,pv->toTime.minute,pv->toTime.second,pv->toTime.hundredths);		//	***404
	if(pv->recipient.choice==0)					//device identifier form
		tp=etagOBJECTID(tp,0x08,((pv->recipient.device)&0x3FFFFF)+((dword)(DEVICE_OBJTYPE)<<22));		//	***1223
	else										//address form
	{
		*tp++=0x1E;
		tp=eUINT(tp,pv->recipient.address.network_number);
		tp=eOCTETSTRING(tp,pv->recipient.address.mac_address,pv->recipient.address.mac_len);
		*tp++=0x1F;
	}
	tp=eUINT(tp,pv->processIdentifier);
	tp=eBOOL(tp,pv->issueConfirmedNotifications);
	tp=eBITSTRING(tp,&pv->transitions,0x80,3);	//always 3 significant bits	
	pv++;										//point to next destination
	return tp;
}												//										***1206 End


///////////////////////////////////////////////////////////////////////				***240 Begin
//	Transmit a UTCTimeSynchronization/TimeSynchronization request
//
// in:	utc		true if UTCTimeSynchronization else TimeSynchronization
// out:	0	if success																***216 Begin
//		else error 
//			NOTE: vsbe_resourcesother indicates tx buffer is not free				***216 End

int bpublic frcTimeSynchronization(bool utc)
{	frNpacket	*np;
	octet		*tp;
	frTimeDate	td;

	if(commEnabled==false)
		return vsbe_servicerequestdenied;				//can't transmit right now
	if(initiationDisabled) return vsbe_initiationdisabled;						//	***400
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;			//we're in slave mode				***1223
#endif
	fraGetTimeDate(&td);
	if (utc)
	{	if((np=UnconfirmedRequest(BroadcastDNET,UTCtimeSynchronization,false))==NULL) //try for a packet	***1205
			return vsbe_resourcesother;					//can't get one right now
		MakeLocalTimeUTC(&td);
	}
	else
	{	if((np=UnconfirmedRequest(BroadcastDNET,timeSynchronization,false))==NULL) //try for a packet		***1205
			return vsbe_resourcesother;					//can't get one right now
	}
	tp=np->apdu;
	tp=eDATESTRING(tp,(word)(td.year-1900),td.month,td.day,td.weekday);
	tp=eTIMESTRING(tp,td.hour,td.minute,td.second,td.hundredths);
	np->nlen+=(word)(tp-np->apdu);							//add in stuff after header
	UnconfirmedSend(np); 								//broadcast it and release it
	return 0;											//							***240 End
}

///////////////////////////////////////////////////////////////////////					***1224 Begin
//	Transmit an UnconfirmedPrivateTransfer request on our vendor id
//
//  in:	device		the device instance to transfer to (if nodevice then src has frSource)
//		xsrc		the alternative pointer to an frSource (may be NULL if device !=nodevice)
//		svc			the service code
//		params		pointer to the service parameters (or NULL if nbytes==0)
//		nbytes		the number of octets in params
// out:	0			ok
//		else		reason we failed

int bpublic frcPrivateTransfer(dword device, frSource *xsrc, dword svc, octet *params, word nbytes)
{
	static frVbag vb;
	vb.ps.pscpt.blocklen = nbytes;
	if (vb.ps.pscpt.blocklen > maxcptblocklen)
		return vsbe_parameteroutofrange;			//can't fit this parameter block
	vb.ps.pscpt.vendorid = di.vendor_id;	
	vb.ps.pscpt.service = svc;
	frcpy(&vb.ps.pscpt.block[0], params, nbytes);
	return frcUnconfirmedPrivateTransfer(device, xsrc, &vb);
}																				//		***1224 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Transmit a ConfirmedEventNotification packet
//
// in:	ep		points to the structure for the event notification
//		vb		points to frVbag for confirmation of delivery
//
// out:	0							accepted
//		vsbe_resourcesother			indicates tx buffer is not free						***216
//		vsbe_invaliddatatype		if notify parameters contains an invalid datatype
//		vsbe_servicerequestdenied	comm is disabled due to DCC
//		vsbe_unknowndevice			device form's device is not known					***1207

int bpublic frcEventNotify(frEventNotification *ep, frSource *src, frVbag *vb)
{	frNpacket	*np;		
	octet		*tp;
	int			r,i;
	word		dvx;											//index of peer device	***1207

	if(commEnabled==false)
		return vsbe_servicerequestdenied;						//can't transmit right now
	if(initiationDisabled) return vsbe_initiationdisabled;						//		***400
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;			//we're in slave mode	***1223
#endif
	if(ep->confirmed)											//this is a directed, confirmed request
	{
		if(ep->dest.choice==0)														//	***403 Begin
		{
			if((np=ConfirmedRequest(ep->dest.device,confirmedEventNotification,true,&r,vb))==NULL) //try for a packet
				return r;										//unknown device or can't get one right now			
		}
		else													
		{
			src->snet.w=flipw(ep->dest.address.network_number);                 //  ***1209 Begin
			src->slen=ep->dest.address.mac_len;
			for(i=0;i<ep->dest.address.mac_len;i++)
				src->sadr[i]=ep->dest.address.mac_address[i];
			//use the drlen and dradr as provided by the caller
			//if drlen is 0x00 in this routed case, then we always
			//use the local broadcast address                                   //  ***1209 End
			if((np=ConfirmedRequestMAC(src,confirmedEventNotification,true,&r,vb))==NULL)
				return r;										//can't get one right now			
		}																														
		tp=np->apdu;
		tp=EncodeEventParameters(tp,ep);
		vb->status=vbsPending;
		np->nlen+=(word)(tp-np->apdu);							//add in stuff after header
		ConfirmedSend(np); 										//send it
	}
	else														//unconfirmed
	{	
		if(ep->dest.choice==0)									//send to device
		{
			if((dvx=SearchForDevice(ep->dest.device))==notheardfrom)	//											***1207 Begin
				return vsbe_unknowndevice;
			bytecpy((byte *)src,(byte *)&devlist[dvx].src,sizeof(frSource));	//copy destination info				***1209

			if((np=UnconfirmedRequestMAC(src,unconfirmedEventNotification,false))==NULL) //try for a packet			***1207 End
				return vsbe_resourcesother;						//can't get one right now			
			tp=np->apdu;
			tp=EncodeEventParameters(tp,ep);
			if(vb!=NULL) vb->status=vbsComplete;				//say it finished already
			np->nlen+=(word)(tp-np->apdu);						//add in stuff after header
			UnconfirmedSend(np); 								//send it
		}
		else													//send to address
		{
			src->snet.w=flipw(ep->dest.address.network_number);	//this is the destination network
			src->slen=ep->dest.address.mac_len;				   
//			src->drlen=0;										//no router				***1211 Delete
			for(i=0;i<ep->dest.address.mac_len;i++)				//this becomes the destination address in the frame
				src->sadr[i]=ep->dest.address.mac_address[i];
			if((np=UnconfirmedRequestMAC(src,unconfirmedEventNotification,false))==NULL) //try for a packet
				return vsbe_resourcesother;						//can't get one right now				
			tp=np->apdu;
			tp=EncodeEventParameters(tp,ep);
			if(vb!=NULL) vb->status=vbsComplete;				//say it finished already
			np->nlen+=(word)(tp-np->apdu);						//add in stuff after header						
//			transmit(np,np->nlen,np->npdu,macisUnconfirmed,false); //don't know router	***1211 Delete
//			npoolFreePacket(np);								//done with it			***1211 Delete
			UnconfirmedSend(np); 								//send it				***1211
		}																										//	***403 End
	}
	return 0;												//success
}

///////////////////////////////////////////////////////////////////////
//	Encode the parameters for an event notification into a transmission buffer
//
// in:	tp		points to the transmission buffer
//		ep		points to the structure for the event notification		
//
// out:	pointer to the updated transmission buffer, NULL if something was wrong

static	octet *EncodeEventParameters(octet *tp,frEventNotification *ep)
{	octet	*sp;
	word	i;

	tp=eDWORD(tp,ep->processid,0x08,false);						//[0]processid
	ep->initdeviceinst=((dword)DEVICE_OBJTYPE<<22)+di.deviceinstance;
	tp=etagOBJECTID(tp,0x18,ep->initdeviceinst);				//[1]initiating device instance
	tp=etagOBJECTID(tp,0x28,ep->objid);							//[2]event object id
	*tp++=0x3E;													//context open tag 3 [3]time stamp
	switch (ep->timestamp.type)
	{	
	case 0:
		sp=tp;
		tp=eTIMESTRING(tp,ep->timestamp.time.hour,ep->timestamp.time.minute,
						ep->timestamp.time.second,ep->timestamp.time.hundredths);
		*sp=(*sp&0x7)+0x8;										//modify tag
		break;

	case 1:
		tp=eDWORD(tp,ep->timestamp.sequence,0x18,false);
		break;

	case 2:
		*tp++=0x2E;													//	context open tag 2 [2] date time
		tp=eDATESTRING(tp,(word)(ep->timestamp.date.year+1900),ep->timestamp.date.month,
						ep->timestamp.date.day,ep->timestamp.date.dow);
		tp=eTIMESTRING(tp,ep->timestamp.time.hour,ep->timestamp.time.minute,
						ep->timestamp.time.second,ep->timestamp.time.hundredths);
		*tp++=0x2F;													//	context close tag 2
		break;
	}
	*tp++=0x3F;													//context close tag 3
	tp=eDWORD(tp,ep->notificationclass,0x48,false);				//[4]notification class
	tp=eDWORD(tp,(dword)ep->priority,0x58,false);				//[5]priority
	tp=eDWORD(tp,(dword)ep->eventtype,0x68,false);				//[6]event type (COV, etc.)
	if ((ep->msgh.len>0)&&(ep->msgh.len<=(word)((portid==portMSTP)?256:1024)))		//[7]message (OPTIONAL)
	{	sp=tp;
		tp=eCHARSTRING(tp,&ep->msgh);
		*sp=(*sp&0x7)+0x78;										//modify tag
	}
	tp=eDWORD(tp,(dword)ep->notifytype,0x88,false);				//[8]notify type (ALARM, EVENT, ACK_NOTIFICATION)
	if ((ep->notifytype==ntAlarm)||(ep->notifytype==ntEvent))	//ALARM or EVENT                        ***606 ***1202
	{	tp=eDWORD(tp,(dword)ep->alarmackrequired,0x98,false);	//[9]ack required (iif ALARM or EVENT)
		tp=eDWORD(tp,(dword)ep->fromstate,0xA8,false);			//[10]from state (iif ALARM or EVENT)
		tp=eDWORD(tp,(dword)ep->tostate,0xB8,false);			//[11]to state (iif ALARM or EVENT)
		*tp++=0xCE;												// open context tag 12 [12]event values (iif ALARM or EVENT)
		switch(ep->eventtype)
		{
			case entypeCOB:
				*tp++=0x0E;										//open context tag 0
				tp=eBITSTRING(tp,ep->npu.COB.bitstring,0x08,ep->npu.COB.usedbits);	//[0]bit string
				tp=eBITSTRING(tp,&ep->npu.COB.StatusFlags,0x18,4);		//[1]status flags
				*tp++=0x0F;										//close context tag 0
				break;
			case entypeCOS:
				*tp++=0x1E;										//open context tag 1
				*tp++=0x0E;											//open context tag 0
				tp=eDWORD(tp,ep->npu.COS.newstate,(byte)(((ep->npu.COS.propertystates<<4)&0xF0)+8),false);	//[x]new state
				*tp++=0x0F;											//close context tag 0
				tp=eBITSTRING(tp,&ep->npu.COS.StatusFlags,0x18,4);		//[1]status flags
				*tp++=0x1F;										//close context tag 1
				break;
			case entypeCOV:
				*tp++=0x2E;										//open context tag 2
				*tp++=0x0E;											//open context tag 0
				if (ep->npu.COV.valueorbits==1)
					tp=eREAL(tp,0x18,ep->npu.COV.changedvalue);		//[1]changed value
				else
					tp=eBITSTRING(tp,ep->npu.COV.changedbitstring,0x08,ep->npu.COV.usedbits);	//[0]changed bit string
				*tp++=0x0F;											//close context tag 0
				tp=eBITSTRING(tp,&ep->npu.COV.StatusFlags,0x18,4);		//[1]status flags
				*tp++=0x2F;										//close context tag 2
				break;
			case entypeCF:
				*tp++=0x3E;										//open context tag 3
				tp=eDWORD(tp,ep->npu.CF.commandvalue,0x08,false);		//[0]command value
				tp=eBITSTRING(tp,&ep->npu.CF.StatusFlags,0x18,4);		//[1]status flags
				tp=eDWORD(tp,ep->npu.CF.feedback,0x28,false);			//[2]command feedback
				*tp++=0x3F;										//close context tag 3
				break;
			case entypeFL:
				*tp++=0x4E;										//open context tag 4
				tp=eREAL(tp,0x08,ep->npu.FL.refvalue);				//[0]reference value
				tp=eBITSTRING(tp,&ep->npu.FL.StatusFlags,0x18,4);	//[1]status flags
				tp=eREAL(tp,0x28,ep->npu.FL.setpointvalue);			//[2]setpoint value
				tp=eREAL(tp,0x38,ep->npu.FL.errorlimit);			//[3]errorlimit value
				*tp++=0x4F;										//close context tag 4
				break;
			case entypeOOR:
				*tp++=0x5E;										//open context tag 5
				tp=eREAL(tp,0x08,ep->npu.OOR.exceedingvalue);			//[0]exceeding limit
				tp=eBITSTRING(tp,&ep->npu.OOR.StatusFlags,0x18,4);		//[1]status flags
				tp=eREAL(tp,0x28,ep->npu.OOR.deadband);					//[2]deadband
				tp=eREAL(tp,0x38,ep->npu.OOR.exceededlimit);			//[3]exceeded limit
				*tp++=0x5F;										//close context tag 5
				break;
			case entypeCET:										//							***233 Begin
				*tp++=0x6E;										//open context tag 6
				for (i=0;i<ep->npu.CET.numberpvs;i++)
				{	tp=eDWORD(tp,ep->npu.CET.propval[i].propid,0x08,false);		//[0]property id
					if (ep->npu.CET.propval[i].arrayindex!=noindex)
						tp=eDWORD(tp,ep->npu.CET.propval[i].arrayindex,0x18,false);	//[1]arrayindex
					*tp++=0x2E;										//open context tag 2
					switch(ep->npu.CET.propval[i].value.pdtype)
					{	case adtUnsigned:
							tp=eUINT(tp,ep->npu.CET.propval[i].value.pd.uval);
							break;
						case adtInteger:
							tp=eINT(tp,ep->npu.CET.propval[i].value.pd.sval);
							break;
						case adtReal:
							tp=eREAL(tp,0x44,ep->npu.CET.propval[i].value.pd.fval);
							break;
						case adtDouble:
							#if FLOATLIB
							tp=eDOUBLE(tp,&ep->npu.CET.propval[i].value.pd.dval);
							#else
							tp=eDOUBLE(tp,&ep->npu.CET.propval[i].value.pd.dval[0]);
							#endif
							break;
						case adtBoolean:
							tp=eBOOL(tp,(bool)ep->npu.CET.propval[i].value.pd.uval);
							break;
						case adtObjectID:
							tp=etagOBJECTID(tp,0xC0,ep->npu.CET.propval[i].value.pd.uval);
							break;
						case adtEnumerated:
							tp=eENUM(tp,ep->npu.CET.propval[i].value.pd.uval);
							break;
//						case adtCharString:
//							tp=eCHARSTRING(tp,ep->npu.CET.propval[i].value.pd.sp);
//							break;
						case adtBitString:
							tp=eBITSTRING(tp,ep->npu.CET.propval[i].value.pd.psval,0x80,(word)ep->npu.CET.propval[i].value.len);
							break;
						case adtOctetString:
							tp=eOCTETSTRING(tp,ep->npu.CET.propval[i].value.pd.psval,(word)ep->npu.CET.propval[i].value.len);
							break;
						case adtDate:
							tp=eDATESTRING(tp,(word)ep->npu.CET.propval[i].value.pd.dtval.year,ep->npu.CET.propval[i].value.pd.dtval.month,ep->npu.CET.propval[i].value.pd.dtval.day,ep->npu.CET.propval[i].value.pd.dtval.dow);
							break;
						case adtTime:
							tp=eTIMESTRING(tp,ep->npu.CET.propval[i].value.pd.tval.hour,ep->npu.CET.propval[i].value.pd.tval.minute,ep->npu.CET.propval[i].value.pd.tval.second,ep->npu.CET.propval[i].value.pd.tval.hundredths);
							break;
 						case adtNull:
 							tp=eNULL(tp);
 							break;
						case adtEmptyList:
							break;
						}
					*tp++=0x2F;										//close context tag 2
				}
				*tp++=0x6F;										//close context tag 6		***233 End
			case entypeBR:										//							***1207 Begin
				*tp++=0xAE;
				*tp++=0x0E;														//[0]buffer-property
#ifdef ESP32_SETUP
				tp=etagOBJECTID(tp,0x08,ep->npu.THPBR.bufferProperty.objid);
				tp=eDWORD(tp,ep->npu.THPBR.bufferProperty.propid,0x18,false);
				if(ep->npu.THPBR.bufferProperty.arrayindex!=noindex)
					tp=eDWORD(tp,ep->npu.THPBR.bufferProperty.arrayindex,0x28,false);
				if(ep->npu.THPBR.bufferProperty.deviceid!=nodevice)
					tp=eDWORD(tp,ep->npu.THPBR.bufferProperty.deviceid,0x38,false);
				*tp++=0x0F;
				tp=eDWORD(tp,ep->npu.THPBR.previousNotification,0x18,false);		//[1]previous-notification
				tp=eDWORD(tp,ep->npu.THPBR.currentNotification,0x28,false);		//[2]current-notification
				*tp++=0xAF;
				/*Changing struct name from BR to THPBR cos ESP32 is having special register name as 'BR'*/
#else
				tp=etagOBJECTID(tp,0x08,ep->npu.BR.bufferProperty.objid);
				tp=eDWORD(tp,ep->npu.BR.bufferProperty.propid,0x18,false);
				if(ep->npu.BR.bufferProperty.arrayindex!=noindex)
					tp=eDWORD(tp,ep->npu.BR.bufferProperty.arrayindex,0x28,false);
				if(ep->npu.BR.bufferProperty.deviceid!=nodevice)
					tp=eDWORD(tp,ep->npu.BR.bufferProperty.deviceid,0x38,false);
				*tp++=0x0F;
				tp=eDWORD(tp,ep->npu.BR.previousNotification,0x18,false);		//[1]previous-notification
				tp=eDWORD(tp,ep->npu.BR.currentNotification,0x28,false);		//[2]current-notification
				*tp++=0xAF;
#endif
				break;											//							***1207 End
			default:
				return NULL;
		}
		*tp++=0xCF;
	}
   else if (ep->notifytype==ntAck_notification)			//	***1223 Begin
   {
      tp=eDWORD(tp,(dword)ep->tostate,0xB8,false);       //[11]to state (iif ALARM or EVENT)
   }      												//	***1223 End	
	return tp;
}

#ifdef useMSTP									//	***404 Begin
void bpublic frReplyPostponed(void)
{	
	mstpDoReplyPostponed();		//tell MS/TP to issue a reply postponed
								//to let the network get on with life
}
#endif											//	***404 End

///////////////////////////////////////////////////////////////////////
//This is called when a message is received from a MAC layer that is 
//broadcast or addressed to this station
//in:	rpe		0=no reply expected, 1=reply expected
//		port	the port this is coming from
//		salen	the source MAC address length
//		sa		source MAC address
//		rlen	received length (always >0)
//		rp		points to received NPDU octets
//out:	true if packet was processed												***004

bool bpublic macReceiveIndicate(octet rpe, byte port, byte salen, octet *sa, int rlen, octet *rp)	//	***004
{
frNpacket	*q=NULL;								//						***1208
int			i=0;											//						***1209
int			pdutype;										//						***1209

#ifdef DEBUGVSBHP
	char		b[256]={0};
	char		t[1024];

	sprintf(b,"\r\nmacReceiveIndicate(%u) (salen=%d) (rlen=%d)\r\n",port,salen,rlen);
	fraLog(b);
	t[0]=(char)0;
	for(i=0;i<rlen;i++)
	{	sprintf(b,"%2.2X ",rp[i]);
		strcat(t,b);
	}
	fraLog(t);
#endif

/*	if(frhReceiveIndicate(rpe,port,salen,sa,rlen,rp)) 		//call app hook			***1205
		return true;*/										//hook handled it		***1205
		
    if(rlen<2) return true;                         		//reject, too small		***1209 Begin
    if(rlen>macMaxNPDU(port)) return true;          		//reject, too big
    if(salen>8) return true;                        		//reject, src addr is too large
    if((q=npoolGetPacket((word)rlen))==NULL)
    {
#ifdef DEBUGVSBHP
		fraLog("NULL Q!!!");
#endif
		return false;                             			//can't get packet space ***004
    }
	q->src.port=port;                               //copy source port
	q->nlen=(word)rlen;
	for(i=0;i<rlen;i++) q->npdu[i]=rp[i];           //copy packet
	if(!SkipNLheader(q,sa,salen))                   //quietly drop it                   //    ***410
	{
		npoolFreePacket(q);                       //free the packet
		return true;
	}
#ifdef DEBUGVSBHP
	sprintf(b,"%s %d PutQ port=%d %08X\n",__FILE__,__LINE__,q->src.port,(unsigned int)q);
	fraLog(b);
#endif
	q->nflag&=~Nreplyexpected;          //                                         ***410 Begin
	pdutype=((q->apdu[0]&ASDUpdutype)>>ASDUpdutypeshr); //get the pdu type field
	if(pdutype!=UNCONF_REQ_PDU)   
	{
		q->nflag|=Nreplyexpected;                       //it’s a confirmed request ***410 End
		q->src.maxresponse=(q->apdu[1]&0x0F);           //store the incoming max-apdu       ***405
		q->invokeid=q->apdu[2];
		q->svc=q->apdu[3];
	}
	else if(pdutype==UNCONF_REQ_PDU)
	{
		q->svc=q->apdu[2];
	}
	PutQ(&alq,q);                                   //put directly into AL queue	***256
	return true;                                    //processed						***1209 End
}

/////////////////////////////////////////////////////////////////////// 			***1209 Begin
//	Point the apdu field of an frNpacket to the apdu, skipping the NL header
//
//in:	nm		points to an frNpacket
//		sa		points to source MAC address
//		slen	length of source MAC address
//out:	false	failed
//		true	OK

static	bool SkipNLheader(frNpacket *nm, octet *sa, octet salen)
{
octet	*sp;
NPCI	npci;
byte	len=0;
int		i;

	if(salen==0) return false;								//this can't be right
	if(salen>8) return false;								//this can't be right	***1222
	nm->src.snet.w=0;										//assume it's local
	nm->src.slen=salen;
	nm->src.drlen=0;										//not routed
	for(i=0;i<salen;i++) nm->src.sadr[i]=sa[i];				//copy source address
	sp=nm->npdu;											//point to npdu

	npci.npciVersion=*sp++;
	if(npci.npciVersion!=1) return false;					//we only know about BACnet v1!
	npci.npciControl=*sp++;
	if(npci.npciControl&NSDUisNLmessage) return false;		//this is a network layer message or
	if((npci.npciControl&0x40) || (npci.npciControl&0x10))	//reserved bits are set
		return false;										//drop it, we don't deal with any of these
	if(npci.npciControl&NSDUhasDNET)						//parse DNET/DLEN?DADR
	{
		npci.npciDNET=(*sp++)<<8;
		npci.npciDNET+=*sp++;
		if(npci.npciDNET!=BroadcastDNET) return false;		//this is a specific network (not broadcast or local)
		npci.npciDLEN=*sp++;
		if(npci.npciDLEN>8) return false;					//no way this is valid	***1222
		for(i=0;i<npci.npciDLEN;i++)
			  npci.npciDADR[i]=*sp++;
	}
	if(npci.npciControl&NSDUhasSNET)						//parse SNET/SLEN/SADR
	{
		nm->src.drlen=salen;
		for(i=0;i<salen;i++) nm->src.dradr[i]=sa[i];		//remember router's address
		nm->src.snet.w=(*sp++)<<8;
		nm->src.snet.w+=*sp++;
		nm->src.slen=*sp++;
		if(nm->src.slen>8) return false;					//no way this is valid	***1222
		for(i=0;i<nm->src.slen;i++) nm->src.sadr[i]=*sp++;	//remember source address
	}
	if(npci.npciControl&NSDUhasDNET) sp++;					//hopcount
	len=(byte)(sp-nm->npdu);
	nm->apdu=sp;											//save pointer to APDU
	nm->nlen-=len;											//this is the length of our APDU
	if(!nm->nlen) return false;								//pitch this one
#ifdef DEBUGVSBHP
	sprintf(t,"SkipNLheader portid=%d",nm->src.port);
	fraLog(t);
#endif
	return true;
}															//						***1209 End

///////////////////////////////////////////////////////////////////////				***1210  Begin
//	try to transmit data
//
//in:	pkt             the destination port
//		datalen			the length of data np points to
//		data			points to dlen bytes of data to send (copied by lower layers)
//		reqflg			macisRequest, macisResponse, or macisUnconfirmed
//      isBcast         true for broadcast, false otherwise

static void transmit(frNpacket *pkt,word datalen,void* data, byte reqflag, bool isBcast)
{
    if(isBcast)
        macTransmit(portid, NULL, datalen, (octet*) data, reqflag);
    else
    {
        if(pkt->src.snet.w)
        {
            if(pkt->src.drlen==0)   //the router address wasn't provide, use local broadcast
                macTransmit(portid, NULL, datalen, (octet*) data, reqflag);  //local braodcast
            else        //this is a routed packet, dradr contains the local router MAC address
                macTransmit(portid, pkt->src.dradr, datalen, (octet*) data, reqflag);
        }
        else
        {
            //Not routed, sadr contains the local address
            macTransmit(portid, pkt->src.sadr, datalen, (octet*) data, reqflag);
        }
        
    }
}                                                           //						***1210 End

///////////////////////////////////////////////////////////////////////
//This is called periodically by the main application to allow
//VSBHP to process AL messages

void bpublic frMain(void)
{
frNpacket	*am=NULL,*np;							//								***1209 Begin
frVbag		*vb;
ASDUREQ 	ap;               						//for requests from clients
ASDU 		asp;                 					//for responses from servers
int			pdutype;
octet		*op;
dword		e1,e2;
word		i;
#ifdef DEBUGVSBHP
	char t[128];
#endif
	if((am=GetQ(&alq))==NULL)									//	***256
	{
		return;						//no messages
	}
	if(!am->apdu)												//	***410 Begin
	{
		npoolFreePacket(am);
		return;
	}															//	***410 End
#ifdef DEBUGVSBHP
		fraLog("Non-Empty Q\n");
#endif

	asp.asduPDUheader=am->apdu[0];
	ap.asdureqPDUheader=am->apdu[0];
	pdutype=((ap.asdureqPDUheader&ASDUpdutype)>>ASDUpdutypeshr); //get the pdu type field
	if(pdutype<=UNCONF_REQ_PDU)         						//these PDU types use ASDUREQ
	{
	    ap.asdureqMaxResponse=am->apdu[1];
	    ap.asdureqInvokeID=am->apdu[2];
	    ap.asdureqServiceChoice=am->apdu[3];
	}
	else                                      					//all others use ASDU
	{
	    asp.asduInvokeID=am->apdu[1];
	    asp.asduServiceAckChoice=am->apdu[2];
	}															//					***1209 End
#ifdef DEBUGVSBHP
	sprintf(t,"Portid %08X portid=%d",(unsigned int)am,am->src.port);
	fraLog(t);
	sprintf(t,"PDU-Type = %d",pdutype);
	fraLog(t);
#endif
	switch(pdutype)
	{
	case UNCONF_REQ_PDU:                           
		HandleUnconfirmedReq(am);
		break;
	case CONF_REQ_PDU:
#ifdef ESP32_SETUP
		ap.asdureqInvokeID = am->apdu[2];  //To solve the comlier error for uninitialize asdureqInvokeID in thid function.
#endif
		if(ap.asdureqPDUheader&ASDUissegmented)			//we don't support segmentation
			SendAbort(am,ap.asdureqInvokeID,ABORT_SegmentationNotSupported);
		else
		{										
			am=HandleConfirmedReq(am);						//							***256
		}
		break;
	case SEGMENT_ACK_PDU:
		if((asp.asduPDUheader&ASDUserver)==0)			//abort if came from client
			SendAbort(am,asp.asduInvokeID,ABORT_InvalidAPDUInThisState);
		break;
	case COMPLEX_ACK_PDU:									//							***240 Begin
#ifdef ESP32_SETUP
		ap.asdureqInvokeID = am->apdu[2];  //To solve the comlier error for uninitialize asdureqInvokeID in thid function.
#endif
		if (asp.asduPDUheader&ASDUissegmented)
		{	SendAbort(am,ap.asdureqInvokeID,ABORT_SegmentationNotSupported);
			break;
		}
		//fall into SIMPLE_ACK intentionally
	case SIMPLE_ACK_PDU:
		for (i=0;i<confirmq.count;i++)						//see if we know about this guy
		{	np=GetQ(&confirmq);								//	***256
			if (np->invokeid==asp.asduInvokeID&&np->svc==asp.asduServiceAckChoice)	//invoke id and service must match ***1209
			{	vb=(frVbag *)np->vb;						//point to its Vbag
				if (pdutype==COMPLEX_ACK_PDU)
					HandleCACK(&asp,(word)am->nlen,vb);
				else //if(pdutype==SIMPLE_ACK_PDU)
					vb->pdtype=adtSACK;
				vb->status=vbsComplete;
				fraResponse(vb);
#ifdef DEBUGVSBHP											//	***256 Begin
				sprintf(dbg,"%s (SIMPLE_ACK_PDU) calling npoolFreePacket\n",__FUNCTION__);
				fraLog(dbg);
#endif														//	***256 End
				npoolFreePacket(np);						//don't need the confirm packet anymore
				break;										//don't return it to the queue
			}
			else
			{
#ifdef DEBUGVSBHP
				sprintf(t,"%s %d PutQ %08X\n",__FILE__,__LINE__,(unsigned int)&np);
				fraLog(t);
#endif				
				PutQ(&confirmq,np);							//just return it to the queue ***256
			}
		}
		break;

	case ERROR_PDU:
	case REJECT_PDU:
	case ABORT_PDU:
		for (i=0;i<confirmq.count;i++)						//see if we know about this guy
		{	np=GetQ(&confirmq);								//	***256
			if (np->invokeid==asp.asduInvokeID)			//invoke id must match
			{	vb=(frVbag *)np->vb;						//point to its Vbag
				op=(octet *)&asp;							//					***1223
				if (pdutype==ERROR_PDU)
				{	op+=3;									//advance to error class
					e1=dENUM(&op)<<8;						//error class
					e2=dENUM(&op);							//error code
				}
				else if (pdutype==ABORT_PDU)
				{	op+=2;									//advance to abort reason
					e1=abortreason;
					e2=(dword)*op;							//abort reason		***229
				}
				else	//(pdutype==REJECT_PDU)
				{	op+=2;									//advance to reject reason
					e1=rejectreason;
					e2=(dword)*op;							//reject reason		***229
				}
				vb->pd.errval=(int)(e1+e2);
				vb->pdtype=adtError;
				vb->status=vbsComplete;
				fraResponse(vb);
#ifdef DEBUGVSBHP											//	***256 Begin
				sprintf(dbg,"%s (ABORT_PDU) calling npoolFreePacket\n",__FUNCTION__);
				fraLog(dbg);
#endif														//	***256 End
				npoolFreePacket(np);						//don't need the confirm packet anymore
				break;										//don't return it to the queue
			}
			else
			{
#ifdef DEBUGVSBHP		
				sprintf(t,"%s %d PutQ %08X\n",__FILE__,__LINE__,(unsigned int)&np);
				fraLog(t);
#endif				
				PutQ(&confirmq,np);							//just return it to the queue ***256
			}
		}
		break;												//							***240 End
	default:
		break;												//toss packet
	}
	if(am!=NULL)
	{
#ifdef DEBUGVSBHP											//	***256 Begin
		sprintf(dbg,"%s (at exit) calling npoolFreePacket\n",__FUNCTION__);
		fraLog(dbg);										//	***256 End
#endif
		npoolFreePacket(am);
	}
}

///////////////////////////////////////////////////////////////////////					***1205 Begin
//	Handle an incoming CACK (for a readProperty)
//
//in:	*ap			points to an APDU request
//		alen		length of request
//		vb			place to put value, same as provided in request

static	void HandleCACK(ASDU *ap,word alen,frVbag *vb)
{
octet		svc,*op,*sop;

	op=(octet *)ap;
	svc=op[2];												//get service
	op+=3;													//point past service choice
	alen-=3;												//adjust remaining size
	sop=op;
	switch(svc)
	{
	case readProperty:
		if((*op&0xF0)!=0) return;							//[0]BACnetObjectID
		vb->objid=dDWORD(&op,false);						//decode object id
		if((*op&0xF0)!=0x10) return;						//[1]BACnetPropertyID
		vb->propid=dDWORD(&op,false);						//decode property id
		if(((word)(op-sop)<alen)&&((*op&0xF0)==0x20))		//[2]Unsigned
			vb->arrindex=dDWORD(&op,false);					//decode array index
		else
			vb->arrindex=noindex;

		if((word)(op-sop)<alen)
		{
			if(*op++==0x3E)									//we found Value Open Tag
				dPRIMITIVE(&op,vb);							//try to decode primitive
		}
		break;
	case confirmedPrivateTransfer:
		//ConfirmedPrivateTransfer-ACK ::= SEQUENCE {
		//	vendorID [0] Unsigned16,
		//	serviceNumber [1] Unsigned,
		//	resultBlock [2] ABSTRACT-SYNTAX.&Type OPTIONAL }
		if((*op&0xF0)!=0) return;							//[0]vendorID
		if(vb->ps.pscpt.vendorid!=dDWORD(&op,false)) return;
		if((*op&0xF0)!=0x10) return;						//[1]servicenumber
		vb->ps.pscpt.service=dDWORD(&op,false);
		if(*op++==0x2E)										//[2]resultblock
		{
			if(sop[alen]!=0x2F)
			{
				vb->pdtype=adtError;
				vb->pd.errval=vsbe_invalidtag;
				break;
			}
			vb->ps.pscpt.blocklen=(word)(alen-(op-sop));
			if(vb->ps.pscpt.blocklen>sizeof(vb->ps.pscpt.block))	//can't accept this much reply
			{
				vb->pdtype=adtError;
				vb->pd.errval=vsbe_parameteroutofrange;
			}
			else											//copy result block
			{
				vb->pdtype=adtPrivateTransfer;
				bytecpy(vb->ps.pscpt.block,(byte *)op,vb->ps.pscpt.blocklen);
			}
		}
		else
			vb->ps.pscpt.blocklen=0;						//no result block
		break;
	}
}															//							***1205 End

/////////////////////////////////////////////////////////////////////// 
//	Determine if the ADPU is an unconfirmed request that we handle
//
//in:	am		incoming packet

static	void HandleUnconfirmedReq(frNpacket *am)
{
dword		lowlim,highlim,devinst,oid;
frTimeDate	td;
word		svcnum,vendorid,objtype,n=0,i,j=0xFFFF;			//							***263 init n
octet		*sp,*ap=am->apdu;
octet		oname[128];										//							***1200
frString	oname_str = { };										//							***1200
octet		scv=ap[1];
//static frCOV		cov;									//							***1206 Delete
//frSmallVbag	*vb;										//							***1206 Delete
//bool		oktocov=false;									//							***1206 Delete	
bool		olddisableinitiation=initiationDisabled;		//							***404

#ifdef DEBUGVSBHP
	char		b[128];											//						***605
	byte		b_len=0;										//						***605

	sprintf(b,"\r\nHandleUnconfirmedReq: nlen=%u, svc=%u %2.2X %2.2X %2.2X %2.2X",am->nlen,ap[1],ap[2],ap[3],ap[4],ap[5]);
	fraLog(b);
#endif

// LTK 3135:Px_Green added one more check for BACnet ip whitlisting support	
#ifdef TRUSTED_IP_FILTER
	if( Is_Request_Trusted(am) == false )
	{	
		//Error: Client ip address is not trusted.
		return;
	}
#endif

	if(commEnabled==false) return;						//								***010	
	

	switch(scv)											//service choice
	{	
//--------------------------------------------------------------------------------------------------
	case whoIs:
#if useMSTP
		if((portid==portMSTP) && (di.maxmaster==0)) return;							//slave mode, just stop			***1223
#endif
#ifdef DEBUGVSBHP
		fraLog("\r\nHandleUnconfirmedReq: Recv WhoIs");
#endif
		if(am->nlen>2)									//we must check our instance with the limits
		{
			ap+=2;
			if((*ap&0xf8)!=0x08)						//it's not context tag 0
				break;
			lowlim=dCONTEXT(&ap,false);
			if((*ap&0xf8)!=0x18)						//it's not context tag 1
				break;
			highlim=dCONTEXT(&ap,false);
			if((di.deviceinstance<lowlim)||(di.deviceinstance>highlim))	
				break;
		} 
#ifdef DEBUGVSBHP
		fraLog("\r\nHandleUnconfirmedReq: Transmit IAm");
#endif		
		//temporarily set initiationDisabled to false
		//just so we can initiate an IAm in response to this Who-Is
		//periodic I-Ams are disabled when initiationDisabled is true!!!
		if(initiationDisabled)							//								***404 Begin
			initiationDisabled=false;					//								***400 End

		frTransmitIAm(am->src.snet.w);					//transmit I-Am to source net	***1205
		initiationDisabled=olddisableinitiation;		//								***404 End		
		break;
//--------------------------------------------------------------------------------------------------
	case timeSynchronization:
		if(!fraCanDoTimeSync()) return;			//								***261  ***404 moved here
//--------------------------------------------------------------------------------------------------
	case UTCtimeSynchronization:
		if((scv==UTCtimeSynchronization) &&		//	***404
		   (!fraCanDoUTCTimeSync()))			//	***404
			return;
		ap+=2;
		if (fraGetTimeDate(&td))						//to get dst and utc
		{
			dDATESTRING(&ap,&td.year,&td.month,&td.day,&td.weekday);
			dTIMESTRING(&ap,&td.hour,&td.minute,&td.second,&td.hundredths);
			if (scv==UTCtimeSynchronization) LocalizeUTCTime(&td);		//the time must be adjusted
			fraSetTimeDate(&td);
		}
		break;
//--------------------------------------------------------------------------------------------------
	case IHave:											//						***1205 Begin
		ap+=2;
		devinst=dOBJECTID(&ap,&objtype);				//get device inst
		oid=dCONTEXT(&ap,false);						//get objid
		byteset(oname,0x00,128);						//blank name
		buildfrString(&oname_str,oname,false,128,128);			
		if(dCHARSTRING(&ap,&oname_str)!=vsbe_noerror)
			break;										//just quietly drop it
		fraIHave(devinst,oid,&oname_str);				//tell app about it
		break;											//						***1205 End
//--------------------------------------------------------------------------------------------------
	case IAm:
		ap+=2;
		devinst=dOBJECTID(&ap,&objtype);
		for	(i=0;i<maxdevices;i++)
		{	
			if (devlist[i].devinst==devinst)
			{	
				j=i;
				break;									//we found a match
			}
		}
#ifdef DEBUGVSBHP
		sprintf(b,"Received IAm %lu slot=%u",devinst,j);
		fraLog(b);
#endif
		//always update
		if (j==0xFFFF) return;							//only update slots we've been configured for
		devlist[j].devinst=devinst;
		devlist[j].src.snet.w=am->src.snet.w;
		devlist[j].src.port=am->src.port;
		devlist[j].src.slen=am->src.slen;
		for(i=0;i<am->src.slen;i++)
			devlist[j].src.sadr[i]=am->src.sadr[i];
		devlist[j].src.drlen=am->src.drlen;
		for(i=0;i<am->src.drlen;i++)
			devlist[j].src.dradr[i]=am->src.dradr[i];
		devlist[j].maxlen=(word)dUINT(&ap);
		lowlim=dENUM(&ap);								//don't really care
		devlist[j].vendorid=(word)dUINT(&ap);
#ifdef DEBUGVSBHP
		b_len=0;
		b_len+=sprintf(b+b_len,"...vid %u,port %u,snet %u, sadr=",devlist[j].vendorid,am->src.port,am->src.snet.w);	//	***605 Begin		
		for(i=0;i<am->src.slen;i++)
			b_len+=sprintf(b+b_len,"%.2X ",am->src.sadr[i]);
		if((!am->src.drlen)&&(b_len<sizeof(b-2)))	//is there no DLEN, then end the string with a CRLF if there's room
			strcat(b,"\r\n");
		fraLog(b);
		if(am->src.drlen)
		{
			b_len=0;
			b_len+=sprintf(b+b_len,"...dradr (dlen=%d) =",am->src.drlen);			
			for(i=0;i<am->src.drlen;i++)
				b_len+=sprintf(b+b_len,"%.2X ",am->src.dradr[i]);
			fraLog(b);							
		}																											//	***605 End
#endif
		break;
//--------------------------------------------------------------------------------------------------
	case whoHas:
#ifdef useMSTP
		if((portid==portMSTP) && (di.maxmaster==0)) return;						//slave mode, just stop		***1223
#endif
		ap+=2;
		if((*ap&0xf8)==0x08)							//it's a context tag 0
		{	lowlim=dCONTEXT(&ap,false);
			if((*ap&0xf8)!=0x18)						//it's not context tag 1
				break;
			highlim=dCONTEXT(&ap,false);
			if((di.deviceinstance<lowlim)||(di.deviceinstance>highlim))
				break;									//we're not in deviceInstanceRangeLow/HighLimit
		}
		if((*ap&0xf8)==0x28)							//it's a context tag 2
		{	oid=dOBJECTID(&ap,&objtype);				//decode object id
#ifdef DEBUGVSBHP
			sprintf(b,"\r\ncall fraWhoHas objtype=%u oid=%8.8lX",objtype,oid);
			fraLog(b);
#endif		
			if((oid==di.deviceinstance) &&			//								***404 Begin
			   (objtype==DEVICE_OBJTYPE))			//								***1218 
				frTransmitIHave(am->src.snet.w,oid+(((dword)DEVICE_OBJTYPE)<<22),di.device_name);	
			else	
			{
				oid+= (((dword)objtype)<<22);
				fraWhoHas(am->src.snet.w,false,oid,NULL);
			}										//								***404 End
		}
		else if((*ap&0xf8)==0x38)					//it's a context tag 3
		{	
			byteset(oname,0x00,128);				//								***404
			buildfrString(&oname_str,oname,false,128,128);	//						***1200 Begin			
			if(dCHARSTRING(&ap,&oname_str)!=vsbe_noerror)
			{				
				break;								//just quietly drop it
			}										//								***1200 End
			//dCHARSTRING(&ap,(frString *)oname,120);		//decode objectname
#ifdef DEBUGVSBHP
			sprintf(b,"\r\ncall fraWhoHas name=%s",oname_str.text);
			fraLog(b);
#endif			
			if(frcmp(oname,di.device_name->text,di.device_name->len)==0)	//		***404 Begin
			{
				frTransmitIHave(am->src.snet.w,di.deviceinstance+(((dword)DEVICE_OBJTYPE)<<22),di.device_name);	
			}																			
			else									//								***404 End
				fraWhoHas(am->src.snet.w,true,0,&oname_str);
				//fraWhoHas(am->src.snet.w,true,0,(frString *)oname);
		}
		break;
//--------------------------------------------------------------------------------------------------
	case unconfirmedCOVNotification:
		HandleCOVNotify(am,ap+2,am->nlen-2,0);		//use common handler			***1206
		break;										//								***210 End
//--------------------------------------------------------------------------------------------------
	case unconfirmedPrivateTransfer:				//								***233 Begin
		if(!fraCanDoPvtTransfer(false)) return;		//								***1205
		sp=ap;
		ap+=2;
		if ((*ap&0xF0)!=0) break;	
		vendorid=(word)dCONTEXT(&ap,false);			//[0]vendorID
		if (vendorid!=di.vendor_id) break;
		if ((*ap&0xF0)!=0x10) break;				//better be for us	
		svcnum=(word)dCONTEXT(&ap,false);			//[1]serviceNumber
		if (*ap==0x2E)								//[2]optional serviceParameters
		{	ap++;
			if ((dword)(ap-sp)>=am->nlen) break;
			if (sp[am->nlen-1]!=0x2F) break;
			n=(word)(am->nlen-(ap-sp)-1);
		}
		fraPrivateTransfer(&am->src,svcnum,ap,n);
		break;										//								***233 End
	}		
}

///////////////////////////////////////////////////////////////////////				***1206 Begin 
//	Handle COVNotifications
//
//in:	am			incoming packet
//		am->src		points to source info structure
//		ap			points to the APDU
//		alen		the length of the apdu
//		cnf			0=unconfirmed, 1=confirmed

void HandleCOVNotify(frNpacket *am,octet *ap,word alen,octet cnf)
{
octet		svc,id;									//	***1224
word		i;
dword		lifetime;
frSmallVbag	*vb;
int			e;
bool		oktocov=false;
frCOV		cov;

	if(cnf)											//confirmed
	{
		svc=ap[3];									//remember service and invokeID
		id=ap[2];
		//sap=ap;									//	***1224
		ap+=4;
		alen-=4;
	}
	else
	{
		svc=ap[1];									//remember service
		//ap+=2;									//	***1224
		alen-=2;
	}
	e=vsbe_noerror;
	if((*ap&0xF8)!=0x08) goto hcnitag;				//it's not context tag 0 (processID)
	cov.processid=dCONTEXT(&ap,false);				//[0]process identifier
	if((*ap&0xF8)!=0x18) goto hcnitag;				//it's not context tag 1 (deviceID)
	cov.deviceinst=dCONTEXT(&ap,false);				//[1]initiating device instance
	if((*ap&0xF8)!=0x28) goto hcnitag;				//it's not context tag 2 (monitored objectid)
	cov.objid=dCONTEXT(&ap,false);					//[2]monitored object id
	if((*ap&0xF8)!=0x38) goto hcnitag;				//it's not context tag 3 (time remaining)
	lifetime=dCONTEXT(&ap,false);					//[3]time remaining
	if(*ap++!=0x4E) goto hcnitag;					//not [4]opening tag list of values

	cov.numberpvs=0;
	while (cov.numberpvs<sizeof(cov.propval))
	{
		if (*ap==0x4F)								//[4]closing tag list of values
		{
			oktocov=true;							//we have everything
			break;
		}
		if((*ap&0xF0)!=0)							//	[0]BACnetPropertyID
			goto hcnitag;
		cov.propval[cov.numberpvs].propid=dDWORD(&ap,false); //	decode property id
		if((*ap!=0x1F)&&((*ap&0xF0)==0x10))					//	[1]Unsigned
			cov.propval[cov.numberpvs].arrayindex=dDWORD(&ap,false); //	decode array index
		else
			cov.propval[cov.numberpvs].arrayindex=noindex;
		if(*ap++==0x2E)								//we found Value Open Tag
		{
			vb=&cov.propval[cov.numberpvs].value;
			vb->pdtype=((*ap)>>4)&0x0F;				//datatype
			if ((*ap&0x7)>5)						//this better be (0-5)
			{
				e=vsbe_invaliddatatype;
				goto hcnerr;
			}
			switch(vb->pdtype)
			{	
			case adtNull:
				if (*ap!=0x00) goto hcnitag;
				dNULL(&ap);							//we don't need the value...
				break;
			case adtBoolean:
				if (*ap!=0x10&&*ap!=0x11) goto hcnitag;
				vb->pd.uval=dBOOL(&ap);
				break;
			case adtUnsigned:
				vb->pd.uval=dUINT(&ap);
				break;
			case adtInteger:
				vb->pd.sval=dINT(&ap);
				break;
			case adtReal:
				if (*ap!=0x44) goto hcnitag;
				vb->pd.fval=dREAL(&ap);
				break;
			case adtEnumerated:
				vb->pd.uval=dENUM(&ap);
				break;
			case adtObjectID:
				vb->pd.uval=dDWORD(&ap,false);
				break;
			case adtDouble:
#if FLOATLIB
				dDOUBLE(&ap,&vb->pd.dval);
#else
				dDOUBLE(&ap,&vb->pd.dval[0]);
#endif
				break;
			case adtDate:
				if (*ap!=0xA4) goto hcnitag;
				dDATESTRING(&ap,&i,&vb->pd.dtval.month,&vb->pd.dtval.day,&vb->pd.dtval.dow);
				vb->pd.dtval.year=(byte)(i-1900);
				break;
			case adtTime:
				if (*ap!=0xB4) goto hcnitag;
				dTIMESTRING(&ap,&vb->pd.tval.hour,&vb->pd.tval.minute,&vb->pd.tval.second,&vb->pd.tval.hundredths);
				break;
			case adtOctetString:
				vb->pd.uval=dOCTETSTRING(&ap,vb->pd.psval,sizeof(vb->pd.psval));
				break;
			case adtBitString:
				vb->pd.uval=dBITSTRING(&ap,vb->pd.psval,sizeof(vb->pd.psval));
				break;
			default:
				goto hcnitag;
			}
			if(*ap++!=0x2F) goto hcnitag;
		}
		else
			goto hcnitag;
		cov.numberpvs++;
	}
	if(oktocov)										//it's ok to pass it on
	{
		if((cov.processid==0)&&(lifetime==0))		//it's unsubscribed, unconfirmed by nature		***1223 Begin
			fraUnsubscribedCOV(&cov);				//let app know we got one
		else
		{
			e = fraCOV(&cov);						//let app know we got one
			if((e!=vsbe_noerror)) 
				goto hcnerr;		
			if(cnf)
			{
				SendSACK(am,id,svc);				//send SACK for it				
			}								
		}																						//	***1223 End
	}
	else
	{
hcnitag: e=vsbe_invalidtag;							//must common error
hcnerr:	if(cnf)										//must provide an error return
			SendError(am,id,svc,(word)(e>>8),(word)(e&0xFF));
	}
}													//									***1206 End

/////////////////////////////////////////////////////////////////////// 
//	Convert a UTC time/data to a local time date
//in:	*td			points to an frTimeDate structure with the UTC time/date
//out:	*td			has the local adjusted time

static	void LocalizeUTCTime(frTimeDate *td)
{	short	daymins;								//								***240
	
//	assume secs and hundredths won't change!!!
	daymins=(short)td->hour*60+(short)td->minute;
	daymins-=(short)((td->dst)?(td->utcoffset-60):td->utcoffset);
	if (daymins<0)					//GMT day rolled over and we didn't? 
	{	daymins+=(1440);
		td->weekday=(td->weekday!=1)?(td->weekday-1):7;
		if (td->day!=1)				//it's easy
			td->day--;
		else						//we have to roll back our month
		{	switch (--td->month)
			{
			case 0:					//we rolled back into the previous year
				td->year--;
				td->month=12;
				td->day=31;
				break;
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
				td->day=31;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				td->day=30;
				break;
			case 2:
				td->day=((td->year&0x3)==0)?29:28;
				break;
			}
		}
	}
	else if(daymins>=1440)			//We have to roll forward a day					***228 Begin
	{	daymins-=(1440);
		td->weekday=(td->weekday!=7)?(td->weekday+1):1;
		td->day++;
		switch (td->month)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if (td->day==32)		//we rolled over to the next month
			{	td->day=1;
				if (++td->month>12)
				{	td->month=1;	//we rolled into the next year
					td->year++;
				}
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if (td->day==31)		//we rolled over to the next month
			{	td->day=1;
				td->month++;
			}
		case 2:
			if ((td->day==30&&((td->year&0x3)==0))||(td->day==29&&((td->year&0x3)!=0)))
			{	td->day=1;
				td->month++;
			}
			break;
		}	
	}								//													***228 End
	td->hour=(byte)(daymins/60);
	td->minute=(byte)(daymins%60);
}

///////////////////////////////////////////////////////////////////////					***210 Begin 
//	Convert a local time date to a UTC time/date 
//in:	*td			points to an frTimeDate structure with the local time/date
//out:	*td			has the UTC adjusted time

static	void MakeLocalTimeUTC(frTimeDate *td)
{	
	short	daymins=0,daysinmonth=0;		//	***263 initialize to shut up compiler
	
//	assume secs and hundredths won't change!!!
	daymins=(short)td->hour*60+(short)td->minute;
	daymins+=(short)((td->dst)?(td->utcoffset-60):td->utcoffset);
	if (daymins>=1440)					//GMT day rolled over and we didn't?			***228 
	{	daymins-=(1440);
		td->weekday=(td->weekday!=7)?(td->weekday+1):1;				//					***228
		td->day++;
		switch (td->month)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			daysinmonth=31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			daysinmonth=30;
			break;
		case 2:
			daysinmonth=((td->year&0x3)==0)?29:28;
			break;
		}
		if (td->day>daysinmonth)
		{	td->day=1;
			if (++td->month>12)
			{	td->month=1;
				td->year++;
			}
		}
	}
	else if (daymins<0)									//								***228 Begin
	{	daymins+=(1440);
		td->weekday=(td->weekday!=1)?(td->weekday-1):7;
		if (td->day!=1)				//it's easy
			td->day--;
		else						//we have to roll back our month
		{	switch (--td->month)
			{
			case 0:					//we rolled back into the previous year
				td->year--;
				td->month=12;
				td->day=31;
				break;
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
				td->day=31;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				td->day=30;
				break;
			case 2:
				td->day=((td->year&0x3)==0)?29:28;
				break;
			}
		}
	}													//								***228 End
	td->hour=(byte)(daymins/60);
	td->minute=(byte)(daymins%60);
}														//								***210 End

/////////////////////////////////////////////////////////////////////// 
//	Determine if the ADPU is a confirmed request that we handle
//in:	am		incoming packet

static	frNpacket	*HandleConfirmedReq(frNpacket *am)	//								***256
{
octet *rap,rr,id;
static frVbag vb;
bool goes_to_hcrrej=false;								//								***1227
#ifdef DEBUGVSBHP
char		b[64];
#endif

// LTK 3135:Px_Green added one more check for BACnet ip whitlisting support	
#ifdef TRUSTED_IP_FILTER
	if( Is_Request_Trusted(am) == false )
	{
		//Error: Client ip address is not trusted.
		return am;
	}
#endif
	
	if((commEnabled==false)&&
		(am->svc!=deviceCommunicationControl&&			//								***1213
		am->svc!=reinitializeDevice))					//								***1213
			return am;									//								***256
	rap=(octet *)am->apdu;								//								***1213
	id=am->invokeid;								//save invoke ID					***1205
#ifdef DEBUGVSBHP
	sprintf(b,"\r\nHandleConfirmedReq: nlen=%u, svc=%u[iid=0x%2.2X] ",am->nlen,rap[3],id);	// ***1213
	fraLog(b);
#endif
	switch(am->svc)										//								***1213
	{
//-----------------------------------------------------------
//IMPORTANT: HandleReadWriteProperty, HandleReadPropertyMultiple and HandleWritePropertyMultiple free am, 
//				so, we need to return NULL in all of those cases!
	case readProperty:
#ifdef DEBUGVSBHP
		sprintf(b,"\r\nReadProp(%u)",id);				//								***1213
		fraLog(b);
#endif
		vb.priority=0;									//we will be reading
		goto hcrrw;
	case writeProperty:
		vb.priority=255;								//we will be writing
hcrrw:
		HandleReadWriteProperty(am,&rap[4],(word)(am->nlen-4),id,&vb);			//		***1213
		return NULL;									//								***256
//-----------------------------------------------------------
	case readPropertyMultiple:
		if(!fraCanDoRPM()) goto hcunks;						//	***418
		vb.priority=0;										//we will be reading
		HandleReadPropertyMultiple(am,&rap[4],(word)(am->nlen-4),id,&vb);	//	***1221
		return NULL;														//	***1221
//-----------------------------------------------------------
	case writePropertyMultiple:
		if(!fraCanDoWPM()) goto hcunks;						//	***418
		vb.priority=255;									//we will be writing
		HandleWritePropertyMultiple(am,&rap[4],(word)(am->nlen-4),id,&vb);	//	***1221
		return NULL;										//	***256
//-----------------------------------------------------------
	case acknowledgeAlarm:
		if(!fraCanDoAlarmAck()) goto hcunks;				//							***261
		HandleAcknowledgeAlarm(am,&rap[4],id);			//							***1205
		break;
//-----------------------------------------------------------
	case atomicFileRead:
		if(!fraCanDoFiles()) goto hcunks;					//							***261	
		HandleAtomicFileRead(am,&rap[4],id);				//							***1205
		break;
//-----------------------------------------------------------
	case atomicFileWrite:
		if(!fraCanDoFiles()) goto hcunks;					//							***261	
		HandleAtomicFileWrite(am,&rap[4],id);				//							***1205
		break;
//-----------------------------------------------------------
	case getAlarmSummary:
		if(!fraCanDoAlarmSummary()) goto hcunks;			//	***261 ***404			***1205
		HandleGetAlarmSummary(am,id);						//							***1205
		break;
//-----------------------------------------------------------
	case getEventInformation:
		if(!fraCanDoEventInfo()) goto hcunks;				//	***261	***404			***1205
		HandleGetEventInformation(am,&rap[4],id);		//							***1205
		break;
//-----------------------------------------------------------
	case subscribeCOV:
		if(!fraCanDoCOV()) goto hcunks;						//	***261	***404
		HandleSubscribeCOV(am,&rap[4],(word)(am->nlen-4),id);
		break;												//							***210 End
//-----------------------------------------------------------
	case subscribeCOVProperty:											//				***1223 Begin
		if(!fraCanDoCOVProperty()) goto hcunks;
		HandleSubscribeCOVProperty(am,&rap[4],(word)(am->nlen-4),id);
		break;															//				***1223 End
//-----------------------------------------------------------
	case confirmedCOVNotification:											//			***1224 Begin
		goto hcunks; // DH1-188
//		HandleCOVNotify(am, &rap[0], (word)(am->nlen - 4), 1);		//use common handler			
		break;																//			***1224 End
//-----------------------------------------------------------
//IMPORTANT: HandledConfirmedPvtTransfer frees am, 
//				so, we need to return NULL!!!
	case confirmedPrivateTransfer:						//								***1205 Begin
		if(!fraCanDoPvtTransfer(true)) goto hcunks;
		if(HandledConfirmedPvtTransfer(am, &rap, &rr, id, &goes_to_hcrrej)) return NULL;
		if (goes_to_hcrrej) goto hcrrej;				//								***1205 End
//-----------------------------------------------------------
	case deviceCommunicationControl:						
		if (HandleDCC(am, &rap, &rr, id, &goes_to_hcrrej)) break;
		if (goes_to_hcrrej) goto hcrrej;
		break;
//-----------------------------------------------------------
	case reinitializeDevice:
		if (!fraCanDoReinit()) goto hcunks;			
		if (HandleReinitDevice(am, &rap, &rr, id, &goes_to_hcrrej)) break;		//		***1227
		if (goes_to_hcrrej) goto hcrrej;										//		***1227
		break;
//-----------------------------------------------------------							***400 Begin
	case readRange:
		if(!fraCanDoReadRange()) goto hcunks;	
		HandleReadRange(am,&rap[4],(dword)am->nlen-4,id,&vb);
		break;
//-----------------------------------------------------------							***1205 Begin
	case createObject:
		if(!fraCanDoOCD()) goto hcunks;	
		HandleCreateObject(am,&rap[4],id);
		break;
//-----------------------------------------------------------
	case deleteObject:
		if(!fraCanDoOCD()) goto hcunks;	
		HandleDeleteObject(am,&rap[4],id);
		break;
//-----------------------------------------------------------							***1205 End
	default:
hcunks:	rr=REJECT_UnrecognizedService;						//							***261
hcrrej:	SendReject(am,id,rr);
	}
	return am;												//							***256	
}

///////////////////////////////////////////////////////////////////////					***400 Begin
//	Handler for ReadRange service
//
//  in:	am		incoming packet
//		op		points to the ReadRange PDU
//		dsize	is the number of octets to decode
//		id		is the invoke id
//		vb		points to value bag we can use

static	void HandleReadRange(frNpacket *am,octet *op,dword dsize,octet id,frVbag *vb)
{
	octet		*sop,*pp,flags;
	dword		objid,propid,arrayindex=noindex;
	static frRange		range;			//	***414
frNpacket *np=NULL;								//							***1208
	word overhead;
	frNpacket *nm=am;
	int r;
	word nc;

	sop=op;
	if((*op&0xF0)==0)										//[0]BACnetObjectID
		objid=dDWORD(&op,false);							//decode object id
	else
hrrinvtag:	
	{	
		SendReject(am,id,REJECT_InvalidTag);
		return;
	}
	if((*op&0xF0)==0x10)									//[1]BACnetPropertyID
		propid=dDWORD(&op,false);							//decode property id
	else 
		goto hrrinvtag;
	if(propid==ALLX||propid==REQUIRED||propid==OPTIONALX)
	{	
		r=vsbe_inconsistentparameters;						//***418
		goto hrrerr;
	}
	if(((word)(op-sop)<dsize)&&((*op&0xF0)==0x20))			//[2]Unsigned
		arrayindex=dDWORD(&op,false);						//decode array index
	range.rangetype=norange;
	if(((word)(op-sop)<dsize)&&(*op==0x3E))					//[3]byPosition
	{	
		op++;
		range.rangetype=0;
		range.refindex=dUINT(&op);
		range.count=dINT(&op);
		if (*op++!=0x3F)
			goto hrrinvtag;
	}
	else if(((word)(op-sop)<dsize)&&(*op==0x4E))			//[4]byTime, deprecated
	{	
		r=vsbe_inconsistentparameters;
		goto hrrerr;
	}
	else if(((word)(op-sop)<dsize)&&(*op==0x5E))			//[5]timeRange, deprecated
	{	
		r=vsbe_inconsistentparameters;
		goto hrrerr;
	}
	else if(((word)(op-sop)<dsize)&&(*op==0x6E))			//[6]bySequenceNumber	
	{	
		op++;
		range.rangetype=3;
		range.refindex=dUINT(&op);
		range.count=dINT(&op);
		if (*op++!=0x6F)
			goto hrrinvtag;
	}
	else if(((word)(op-sop)<dsize)&&(*op==0x7E))			//[7]byTime (protocol rev 4)
	{	
		op++;
		range.rangetype=4;
		if (*op!=0xA4) goto hrrinvtag;
		dDATESTRING(&op,&nc,&range.stime.month,&range.stime.day,&range.stime.dow);
		range.stime.year=(byte)(nc-1900);
		if (*op!=0xB4) goto hrrinvtag;
		dTIMESTRING(&op,&range.stime.hour,&range.stime.minute,&range.stime.second,&range.stime.hundredths);
		range.count=dINT(&op);
		if (*op++!=0x7F)
			goto hrrinvtag;
	}														
	else if ((word)(op-sop)<dsize)
		goto hrrinvtag;

	r=fraReadRange(objid,propid,arrayindex,vb,&range,&flags);	
	if (r==vsbe_noerror)
	{
		np=npoolGetPacket(maxnl);								//we need a big packet for this
		if (np==NULL)
		{
			SendAbort(nm,id,ABORT_OutOfResources);						//	***1223			
			goto rrfree;													
		}
		frcpy(&np->src,&am->src,sizeof(am->src));
		nm=np;
		pp=RSVP(np,0);													//point to buffer start
		*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;			//PDU header
		*pp++=id;														//invoke id
		*pp++=readRange;												//service
		pp=etagOBJECTID(pp,0x08,objid);									//[0]objectid
		pp=eDWORD(pp,propid,0x18,false);								//[1]propertyid
		if(arrayindex!=noindex) pp=eDWORD(pp,arrayindex,0x28,false);	//[2]arrayindex
		pp=eBITSTRING(pp,&flags,0x38,3);								//[3]resultFlags
		pp=eDWORD(pp,vb->narray,0x48,false);							//[4]itemCount
		*pp++=0x5E;														//context open tag 5	
		overhead=(word)((pp-nm->npdu)+1);
		if (vb->narray!=0)												//not empty	
		{
			if(EncodePropertyValue(&pp,vb,(word)(npoolNPDUsize(nm)-overhead)))
			{
				SendReject(nm,id,REJECT_Other);							//no other appropriate error yet
				goto rrfree;
			}
		}		
		*pp++=0x5F;														//context close tag 5		
		if (range.firstseq!=0xFFFFFFFF)									
		{				
			pp=eDWORD(pp,range.firstseq,0x68,false);					//[6]firstSequenceNumber			
		}																
		//macTransmit(nm->src.port,nm->src.sadr,(word)(pp-nm->npdu),nm->npdu,macisResponse);
		transmit(nm,(word)(pp-nm->npdu),nm->npdu,macisResponse,false);          //  ***1210
	}
	else
hrrerr:
		SendError(am,id,readRange,frGetErrorClass(r),frGetErrorCode(r));
rrfree:														
	//DO NOT FREE am here -- am is freed from the pool by the caller of this function!!!	
	if(np!=NULL)														
	{
		npoolFreePacket(np);								
	}						
	return;
}																					

///////////////////////////////////////////////////////////////////////				***401 Start
//	Decode a recipient list
//in:	op		is the address of the pointer to the tag in an octet buffer
//		vp		pointer to the vbag where the decoded list will go
//out:	return	vsbe_noerror for success, error code for failure
//		np		advanced

static	octet *dRecipientList(octet *op, frVbag *vp,int *r)
{
	word i;	
	octet *np=op;	
	if((*op!=0x3E) && (*op!=0x2E))  //	***410
	{
		*r=vsbe_invalidtag;
		return np;
	}
	*op++;
	vp->pdtype=adtRecipientList;	
	i=0;
	if((*op==0x3F) || (*op==0x2F))	//	***410 Begin
	{
		vp->pdtype=adtEmptyList;
		*r=vsbe_noerror;		
		np=op;
		return np;
	}								//	***410 End
nxtdest:
	if((((*op)>>4)&0x0F)!=0x08) 
	{
		*r=vsbe_invaliddatatype;
		np=op;
		return np;
	}
	dBITSTRING(&op,&vp->ps.psdest[i].validDays,sizeof(octet));	
	if (*op!=0xB4) 
	{
		*r=vsbe_invaliddatatype;
		np=op;
		return np;
	}
	dTIMESTRING(&op,&vp->ps.psdest[i].fromTime.hour,
				  &vp->ps.psdest[i].fromTime.minute,&vp->ps.psdest[i].fromTime.second,
				  &vp->ps.psdest[i].fromTime.hundredths);
	if (*op!=0xB4) 
	{
		*r=vsbe_invaliddatatype;
		np=op;
		return np;
	}
	dTIMESTRING(&op,&vp->ps.psdest[i].toTime.hour,
				  &vp->ps.psdest[i].toTime.minute,&vp->ps.psdest[i].toTime.second,
				  &vp->ps.psdest[i].toTime.hundredths);
	if(*op==0x1E)
	{
		*op++;
		vp->ps.psdest[i].recipient.choice=1;
		if((((*op)>>4)&0x0F)!=0x02) 
		{
			*r=vsbe_invaliddatatype;
			np=op;
			return np;
		}
		vp->ps.psdest[i].recipient.address.network_number=(word)dUINT(&op);
		if((((*op)>>4)&0x0F)!=0x06) 
		{
			*r=vsbe_invaliddatatype;
			np=op;
			return np;
		}
		vp->ps.psdest[i].recipient.address.mac_len=(octet)dOCTETSTRING(&op,&vp->ps.psdest[i].recipient.address.mac_address[0],6);		
		if(*op++!=0x1F) 
		{
			*r=vsbe_invalidtag;
			np=op;
			return np;
		}
	}
	else
	{
		vp->ps.psdest[i].recipient.choice=0;		
		if((((*op)>>4)&0x0F)!=0x00) 							//	***1223
		{
			*r=vsbe_invalidtag;
			np=op;
			return np;
		}			
		vp->ps.psdest[i].recipient.device=dDWORD(&op,false);	
	}
	if((((*op)>>4)&0x0F)!=0x02) 
	{
		*r=vsbe_invaliddatatype;
		np=op;
		return np;
	}
	vp->ps.psdest[i].processIdentifier=dUINT(&op);
	if((((*op)>>4)&0x0F)!=0x01) 
	{
		*r=vsbe_invaliddatatype;
		np=op;
		return np;
	}
	vp->ps.psdest[i].issueConfirmedNotifications=dBOOL(&op);
	if((((*op)>>4)&0x0F)!=0x08)
	{
		*r=vsbe_invaliddatatype;
		np=op;
		return np;
	}
	dBITSTRING(&op,&vp->ps.psdest[i].transitions,sizeof(octet));	
	if((*op!=0x3F) && (*op!=0x2F))
	{
//		*op++;													//	***1206 Delete
		i++;
		goto nxtdest;
	}
	*op++;
	np=op;
	vp->narray=(byte)i+1;
	*r=vsbe_noerror;
	return np;
}																						

/////////////////////////////////////////////////////////////////////// 
//	Decode a time value value element
//in:	op		is the address of the pointer to the tag in an octet buffer
//		pv		pointer to the frBACnetTV where the value will go
//out:	return	vsbe_noerror for success, error code for failure
//		op		advanced

static	int dTVval(octet **op,frBACnetTV *pv)
{	int	r=vsbe_noerror;									//							***1200x

	switch(pv->value.pdtype)
	{
	case adtUnsigned:
//		pv->value.tvpd.uval=dUINT(op);					//							***1200x
		r=dUINTEx(op,&pv->value.tvpd.uval);				//							***1200x				
		break;
	case adtReal:
		pv->value.tvpd.fval=dREAL(op);
		break;
	case adtBoolean:
		pv->value.tvpd.uval=dBOOL(op);
		break;
	case adtEnumerated:
//		pv->value.tvpd.uval=dUINT(op);					//							***1200x
		r=dUINTEx(op,&pv->value.tvpd.uval);				//							***1200x				
		break;		
	case adtInteger:
//		pv->value.tvpd.sval=dINT(op);					//							***1200x
		//r=pv->value.tvpd.sval=dINTEx(op,&pv->value.tvpd.sval);	//					***1200x
		r=dINTEx(op,&pv->value.tvpd.sval);							//				***1202
		break;
	case adtDouble:
#ifdef FLOATLIB														//				***1202 Begin
		dDOUBLE(op,&pv->value.tvpd.dval);
#else
		dDOUBLE(op,&pv->value.tvpd.dval[0]);
#endif																//				***1202 End
		break;
	case adtNull:
		pv->value.tvpd.uval=dNULL(op);
		break;
	case adtBitString:	
		pv->value.sigbits=(byte)dBITSTRING(op,pv->value.tvpd.psval,sizeof(pv->value.tvpd.psval));
		break;
	default:
		return vsbe_invaliddatatype;
	}
	return r;											//							***1200x
}

/////////////////////////////////////////////////////////////////////// 
//	Decode a time value pair
//in:	op		is the address of the pointer to the tag in an octet buffer
//		pv		pointer to the frBACnetTV where the value will go
//out:	return	vsbe_noerror for success, error code for failure
//		op		advanced

static	int dTimeValue(octet **op,frBACnetTV *tv)
{
	int r;
	octet *np=*op;

	if(*np!=0xB4) return vsbe_invalidtag;
	dTIMESTRING(&np,&tv->time.hour,&tv->time.minute,
					&tv->time.second,&tv->time.hundredths);	
	tv->value.pdtype=((*np)>>4)&0x0F;	//get the data type for the TimeVal
	r=dTVval(&np,tv);					//decode the value for the TimeVal
	*op=np;
	if(r!=vsbe_noerror) return r;		
	return vsbe_noerror;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode a sequence of daily schedules 
//in:	op		is the address of the pointer to the tag in an octet buffer
//		vp		pointer to the vbag where the decoded list will go
//		aindex  the array index, can be no index
//out:	r		vsbe_noerror for success, error code for failure
//		op		advanced

static	octet *dWeeklySchedule(octet *op, frVbag *vp,dword aindex,int *r)
{	
	word i=0xFFFF,day=0xFFFF;							//							***1205
	octet *np=op;
	if((*op!=0x3E) && (*op!=0x2E)) 
	{
		*r=vsbe_invalidtag;
		return np;
	}
	*op++;
	if((*op==0x3F) || (*op==0x2F))	//	***410 Begin
	{
		vp->pdtype=adtEmptyList;
		*r=vsbe_noerror;
		np=op;
		return np;
	}								//	***410 End
	vp->pdtype=adtTimeValue;		
	if(aindex==0) 
	{
		*r=vsbe_invaliddatatype;
		return op;
	}
	if(aindex>7&&aindex!=noindex) 
	{
		*r=vsbe_invalidarrayindex;	
		return op;
	}
	//each day is delimited by 0x0E...0x0F
	//and can contain multiple timevalue pairs for each day
nextday:
	day++;
	if(*op++!=0x0E) 
	{
		*r=vsbe_invalidtag;	//next tag is either 0x0F or 0xB4
		return op;
	}
	if(*op==0x0F) 
	{				
		*op++;
		//if it's a valid array index
		//then there can only be one set of 0x0E...0x0F tags
		if((*op==0x0E)&&(aindex!=noindex)) 
		{
			*r=vsbe_invalidtag;
			return op;
		}
		else if(*op!=0x0E)
			goto exit;
		goto nextday;
	}
	if (*op!=0xB4) 
	{
		*r=vsbe_invaliddatatype;	
		return np;
	}
nexttv:
	i++;											//next value element
	if(i>=MAXTIMEVAL)
	{
		*r=vsbe_resourcesother;					//can't hold anymore
		return op;
	}
	*r=dTimeValue(&op,&vp->ps.tv[i].tv);
	//dTIMESTRING(&op,&vp->ps.tv[i].tv.time.hour,
	//			  &vp->ps.tv[i].tv.time.minute,&vp->ps.tv[i].tv.time.second,
	//			  &vp->ps.tv[i].tv.time.hundredths);	
	//vp->ps.tv[i].tv.value.pdtype=((*op)>>4)&0x0F;	//get the data type for the TimeVal
	//r=dTVval(&op,&vp->ps.tv[i].tv);					//decode the value for the TimeVal
	if(*r!=vsbe_noerror) 
	{
		return op;	
	}
	if((aindex<=7)&&(aindex!=noindex))					//which day's slot does it go into	//	***408
	{
		vp->ps.tv[i].day=(octet)aindex;
	}		
	else if(aindex==noindex)
	{
		vp->ps.tv[i].day=(octet)(day+1);				//										***408
	}	
	if(*op==0xB4)
	{
		goto nexttv;
	}
	if(*op++!=0x0F) 
	{
		*r=vsbe_invalidtag;
		return op;
	}
	if(*op==0x0E) goto nextday;	
exit:
	if((*op!=0x3F) && (*op!=0x2F)) 
	{
		*r=vsbe_invalidtag;	
		return op;
	}
	*op++;
	vp->narray=(byte)i;
	np=op;
	*r=vsbe_noerror;
	return np;		
}	

///////////////////////////////////////////////////////////////////////						***108  Begin
// Decode a CalendarEntryValue
//
// in:	op		pointer to the octet string to decode
//		cal		pointer to frCalendarEntry struct
//		r		pointer to return error status
// out:	pointer to advanced octet string

static	octet	*DecodeCalendarEntry(octet *op,frBACnetCalendarEntry *cal,int	*r)
{	word	year;

	*r=vsbe_noerror;								//assume success
	if (*op==0x0C)									//date
	{	
		cal->type=0;
		dDATESTRING(&op,&year,&cal->dt.month,&cal->dt.day,&cal->dt.dow);
		cal->dt.year=(year==0xFFFF)?0xFF:(byte)(year-1900);
	}
	else if (*op==0x1E)								//dateRange
	{	
		op++;
		cal->type=1;
		if (*op!=0xA4)
		{	
			*r=vsbe_invaliddatatype;
			return op;
		}
		dDATESTRING(&op,&year,&cal->dr.start.month,&cal->dr.start.day,&cal->dr.start.dow);
		cal->dr.start.year=(year==0xFFFF)?0xFF:(byte)(year-1900);
		if (*op!=0xA4)
		{	
			*r=vsbe_invaliddatatype;
			return op;
		}
		dDATESTRING(&op,&year,&cal->dr.end.month,&cal->dr.end.day,&cal->dr.end.dow);
		cal->dr.end.year=(year==0xFFFF)?0xFF:(byte)(year-1900);
		if(*op++!=0x1F)
			*r=vsbe_invaliddatatype;
	}	
	else if (*op==0x2B)								//weekNDay
	{	
		cal->type=2;
		dOCTETSTRING(&op,&cal->wnd[0],sizeof(cal->wnd));
	}	
	else
		*r=vsbe_invaliddatatype;
	return op;
}

///////////////////////////////////////////////////////////////////////						***108  Begin
// Decode a SpecialEvent
//
// in:	op		pointer to the octet string to decode
//		cal		pointer to frVbag struct
//		aindex	the arrayindex to modify, or noindex
// out:	r		vsbe_noerror or error
//				advanced octet string

static	octet *dSpecialEvent(octet *op, frVbag *vp,dword aindex,int *r)
{
	word i=0xFFFF,x=0xFFFF;						//											***1205
	if((*op!=0x3E) && (*op!=0x2E))
	{
		*r=vsbe_invalidtag;
		return op;
	}
	*op++;
	if((*op==0x3F) || (*op==0x2F))				//											***410 Begin
	{
		vp->pdtype=adtEmptyList;
		*r=vsbe_noerror;		
		return op;
	}											//											***410 End
	vp->pdtype=adtSpecialEvent;		
	if(aindex==0) 
	{
		*r= vsbe_writeaccessdenied;				//											***1223
		return op;	
	}
	if((aindex>=MAXSPECIALEVNTS)&&(aindex!=noindex))
	{
		*r=vsbe_resourcesother;	
		return op;
	}
	if(aindex!=noindex)
		i=(word)aindex;
	else
	{
nextevent:
		i++;
	}
	if(i>=MAXSPECIALEVNTS) 
	{
		*r=vsbe_resourcesother;
		return op;
	}
	if(*op==0x0E)								//[0] calendar entry
	{
		vp->ps.se[i].caltype=0;			
		*op++;
		op=DecodeCalendarEntry(op,&vp->ps.se[i].calendarEntry,r);
		if (*op++!=0x0F) 
		{
			*r=vsbe_invalidtag;
			return op;
		}
	}
	else if(*op==0x1C)							//[1] calendarReference (objid)
	{
		vp->ps.se[i].caltype=1;			
		vp->ps.se[i].calendarreference=dDWORD(&op,false);
	}
	else if(*op==0x2F)							// No Data- Closing tag						***408 Begin
	{
		*op++;
		goto exit;
	}											//											***408 End
	else
	{
		*r=vsbe_invalidtag;
		return op;
	}
	if(*op++!=0x2E)								//[2] listOfTimeValue
	{
		*r=vsbe_invalidtag;		
		return op;
	}
nexttimeval:
	x++;
	if(x>=MAXSPECEVNTTV)						//we only hold this many, reject upon overflow
	{
		*r=vsbe_resourcesother;
		return op;
	}
	*r=dTimeValue(&op,&vp->ps.se[i].tv[x]);
	if(*r!=vsbe_noerror)
	{
		return op;
	}
	if(*op==0xB4) 
		goto nexttimeval;
	vp->ps.se[i].numvalues=(octet)x+1;													//	***407
	if(*op++!=0x2F) 
	{
		*r=vsbe_invalidtag;
		return op;
	}
	if(*op!=0x39)								//[3] event priority
	{
		*r=vsbe_invalidtag;		
		return op;
	}
	vp->ps.se[i].eventPriority=dUINT(&op);		
	//if we're working on a particular element
	//then there better only be one thing (delimted by an end tag)	
	if((*op!=0x3F)&&(aindex!=noindex)&&(*op!=0x2F)) 
	{
		*r=vsbe_invalidtag;
		return op;
	}
	if((*op!=0x3F) && (*op!=0x2F))
	{		
		x=0xFFFF;								//reset x for next event					***1205
		goto nextevent;
	}
	*op++;
	if(aindex==noindex)		//calculated												//	***407 Begin
		vp->narray=(octet)i+1;						//how many elements are here?			
	else					//provided in aindex, won't be off by one
		vp->narray=(octet)i;															//	***407 End
exit:																					//	***408
	*r=vsbe_noerror;
	return op;
}																						//	***401 End

///////////////////////////////////////////////////////////////////////						***410  Begin
// Decode a list of unsigned values
//
// in:	op		pointer to the octet string to decode
//		vb		pointer to frVbag struct
//		r       pointer to return error status
//					vsbe_noerror or error
// out:			advanced octet string

static	octet *dUnsignedList(octet *op,frVbag *vb,int *r)
{	
	octet	t;
	
	if((*op!=0x3E)&&(*op!=0x2E))							
	{
		*r=vsbe_invalidtag;		
	}                    
	else
	{
		*op++;
		vb->narray=0;
		if((*op==0x3F)||(*op==0x2F))                        //handle the empty list                               
		{
			vb->pdtype=adtEmptyList;                        //tell the app it's empty                       
			*r=vsbe_noerror;								//									***414			
		}                          
		else
		{			
			vb->pdtype=adtUnsigned;			                 //we have an unsigned array		***415
nextdwval:                        
			t=((*op)>>4)&0x0F;                              //datatype
			if(t!=adtUnsigned) 
			{
				*r=vsbe_invaliddatatype;
				return op;
			}
			*r=dPRIMITIVE(&op,vb);							//decode the unsigned value			***1205
			if(vb->narray==maxobjectlist)
			{
				*r=vsbe_resourcesother;
				return op;
			}
			vb->ps.psdval[vb->narray++]=vb->pd.uval;        //put it in the array			
			if (*r!=vsbe_noerror)
			{      				
				return op;
			}
			if((*op!=0x3F)&&(*op!=0x2F))                    //have we reached the end tags?
				goto nextdwval;
		}
	}														
	return op;
}																		//						***410 End

///////////////////////////////////////////////////////////////////////
//	Handler for ReadProperty and WriteProperty services
//
//  in:	am		incoming packet
//		op		points to the ReadProperty or WriteProperty PDU
//		dsize	is the number of octets to decode
//		id		is the invoke id
//		vb		points to value bag we can use
//		  vb.priority	=0 if reading, =255 if writing

static	void HandleReadWriteProperty(frNpacket *am,octet *op,word dsize,octet id,frVbag *vb)
{	octet	*sop,iswrite,service;
	dword	objid,objinst,propid,arrayindex=noindex;			
	word objtype;
	int	r;
	word	year;
	word	nc;																						//	***1225
	

#ifdef DEBUGVSBHP
	char	t[128];
#endif


	sop=op;
	iswrite=(vb->priority==255)?1:0;
	service=(iswrite)?writeProperty:readProperty;
	if((*op&0xF0)==0)										//[0]BACnetObjectID
		objid=dDWORD(&op,false);							//decode object id
	else
		goto drwinvtag;

	objinst=objid&0x3FFFFF;										//get instance number	***417 Begin
	objtype=objid>>22L;											//get object type
	if((objinst==wildcardinst)&&(objtype==DEVICE_OBJTYPE))		//is it the wildcard?
	{
		objid=(di.deviceinstance + ((dword)(DEVICE_OBJTYPE)<<22)); //coerce to our dev instance
	}												
	else if(objinst==wildcardinst)								//not the device object, NAK it
	{
		r=vsbe_unknownobject;									//otherwise, unknown object
		goto drwerror;	
	}															//						***417 End

	if((*op&0xF0)==0x10)										//[1]BACnetPropertyID
	{
		propid=dDWORD(&op,false);								//decode property id
		if(propid==ALLX||propid==REQUIRED||propid==OPTIONALX)	//						***418 Begin
		{		
			if (iswrite)										//						***1200x Begin
			{
				r=vsbe_writeaccessdenied;						
			}
			else												//						***1200x End
			{
				r=vsbe_inconsistentparameters;						
			}
			goto drwerror;
		}														//						***418 End
	}
	else 
		goto drwinvtag;
	if(((word)(op-sop)<dsize)&&((*op&0xF0)==0x20))			//[2]Unsigned
	{
		arrayindex=dDWORD(&op,false);						//decode array index
	}
	else if(((word)(op-sop)<dsize)&&						//							***417 Begin
		    ((*op&0xF0)!=0x20)    &&
			(!iswrite))										//unexpected RP data, NAK it
	{
		goto drwinvtag;										//							***417 End
	}

	if(iswrite&&((word)(op-sop)<dsize))						//it's a writeproperty
	{	
		objtype=objid>>22L;
		
		if (propid==ALLX||propid==OPTIONALX||propid==REQUIRED)
		{
			r=vsbe_unknownproperty;
			goto drwerror;
		}
		if (propid==ACTIVE_COV_SUBSCRIPTIONS)	//										***1200x Begin
		{
			if(objtype==DEVICE_OBJTYPE)
			{
				if (fraCanDoCOV())
					r=vsbe_writeaccessdenied;
				else
					r=vsbe_unknownproperty;
			}
			else
				r=vsbe_unknownproperty;
			goto drwerror;
		}
		else if (propid==DEVICE_ADDRESS_BINDING||propid==OBJECT_LIST)
		{
			if(objtype==DEVICE_OBJTYPE)
				r=vsbe_writeaccessdenied;
			else
				r=vsbe_unknownproperty;
			goto drwerror;
		}
		else if(propid==RECIPIENT_LIST)							//							***401 Begin	***1200x End
		{			
			if(objtype!=NOTIFICATION_CLASS_OBJTYPE)				//							***410 Begin
			{
				r=vsbe_unknownproperty;
				goto drwerror;
			}													//							***410 End
			op=dRecipientList(op,vb,&r);
			if(r==vsbe_invalidtag) 
				goto drwinvtag;
			else if (r!=vsbe_noerror)
			{
				goto drwerror;									//							***410
			}
		}
		else if(propid==WEEKLY_SCHEDULE)
		{
			if(objtype!=SCHEDULE_OBJTYPE)						//							***410 Begin
			{
				r=vsbe_unknownproperty;
				goto drwerror;
			}													//							***410 End
			op=dWeeklySchedule(op,vb,arrayindex,&r);
			if(r==vsbe_invalidtag) 
				goto drwinvtag;
			else if (r!=vsbe_noerror)
			{
				goto drwerror;														//		***410
			}
		}
		else if(propid==EXCEPTION_SCHEDULE)
		{
			if(objtype!=SCHEDULE_OBJTYPE)						//							***410 Begin
			{
				r=vsbe_unknownproperty;
				goto drwerror;
			}													//							***410 End
			op=dSpecialEvent(op,vb,arrayindex,&r);
			if(r==vsbe_invalidtag) 
				goto drwinvtag;
			else if (r!=vsbe_noerror)
			{
				goto drwerror;									//							***410
			}
		}
		else if(propid==EFFECTIVE_PERIOD)
		{
			if(objtype!=SCHEDULE_OBJTYPE)						//							***410 Begin
			{
				r=vsbe_unknownproperty;
				goto drwerror;
			}													
			if((*op!=0x3E)&&(*op!=0x2E)) goto drwinvtag;		
			*op++;												//							***410 End
			if((*op==0x3F)||(*op==0x2F))
			{
				vb->pdtype=adtEmptyList;
			}
			else
			{
				vb->pdtype=adtEffectivePeriod;
				dDATESTRING(&op,&year,&vb->ps.ep.start.month,&vb->ps.ep.start.day,&vb->ps.ep.start.dow);
				vb->ps.ep.start.year=(year==0xFFFF)?0xFF:(byte)(year-1900);
				dDATESTRING(&op,&year,&vb->ps.ep.end.month,&vb->ps.ep.end.day,&vb->ps.ep.end.dow);
				vb->ps.ep.end.year=(year==0xFFFF)?0xFF:(byte)(year-1900);
				if((*op!=0x3F)&&(*op!=0x2F)) goto drwinvtag;
				*op++;
			}
		}
		else if(propid==LIST_OF_OBJECT_PROPERTY_REFERENCE)
		{	
			if(objtype!=SCHEDULE_OBJTYPE)						//							***410 Begin
			{
				r=vsbe_unknownproperty;
				goto drwerror;
			}													
			if((*op!=0x3E)&&(*op!=0x2E)) goto drwinvtag;
			*op++;
			vb->narray=0;
			if((*op==0x3F)||(*op==0x2F))
			{
				vb->pdtype=adtEmptyList;
			}													//							***410 End
			else
			{
				vb->pdtype=adtDeviceObjectPropertyReference;
nextdopr:
				vb->ps.psdopr[vb->narray].objid=dDWORD(&op,false);	//[0]      BACnetObjectIdentifier
				vb->ps.psdopr[vb->narray].propid=dUINT(&op);		//[1]      BACnetPropertyIdentifier
				if((*op&0xF0)==0x20)								//[2]      Unsigned OPTIONAL
					vb->ps.psdopr[vb->narray].arrayindex=dUINT(&op);
				else
					vb->ps.psdopr[vb->narray].arrayindex=noindex;
				if(((*op&0xF0)==0x30)&&(*op!=0x3F))					//[3]      BACnetObjectIdentifier OPTIONAL
					vb->ps.psdopr[vb->narray].deviceid=dDWORD(&op,false);
				else
					vb->ps.psdopr[vb->narray].deviceid=nodevice;
				if((*op!=0x3F)&&(*op!=0x2F))			//either an end tag		***410
				{
					if(*op==0x0C)		//or this begins another element
					{
						vb->narray++;
						if(vb->narray>=MAXDOPR) 
						{
							r=vsbe_resourcesother;
							goto drwerror;										//	***410
						}
						goto nextdopr;
					}
					else	
						goto drwinvtag;
				}
				else
					vb->narray++;			//the count is zero based			
			}
		}
		else if(propid==DATE_LIST)
		{
			if(objtype!=CALENDAR_OBJTYPE)						//							***410 Begin
			{
				r=vsbe_unknownproperty;
				goto drwerror;								
			}													
			if((*op!=0x3E)&&(*op!=0x2E)) goto drwinvtag;
			*op++;
			vb->narray=0;
			if((*op==0x3F)||(*op==0x2F))
			{
				vb->pdtype=adtEmptyList;		
			}													//							***410 End			
			else
			{
				vb->pdtype=adtDateList;
nextcal:
				op=DecodeCalendarEntry(op,&vb->ps.pscal[vb->narray],&r);
				if(r==vsbe_invalidtag) 
					goto drwinvtag;
				else if (r!=vsbe_noerror)
				{
calerr:
					goto drwerror;								//	***410
				}
				if((*op!=0x3F)&&(*op!=0x2F))							//either an end tag	***410
				{
					if((*op==0x0C) || (*op==0x1E) || (*op==0x2B))		//or this begins another element
					{
						vb->narray++;
						if(vb->narray>=MAXCALENTRY) 
						{
							r=vsbe_resourcesother;
							goto calerr;
						}
						goto nextcal;
					}
					else	
						goto drwinvtag;				
				}
				else
					vb->narray++;			//the count is zero based	
			}
		}
		else if((propid==ALARM_VALUES) || (propid==FAULT_VALUES))		//	***410 Begin
		{
			if((objtype!=MULTI_STATE_INPUT_OBJTYPE)&&					
			   (objtype!=MULTI_STATE_VALUE_OBJTYPE)&&
			   (objtype!=LIFE_SAFETY_POINT_OBJTYPE)&&
			   (objtype!=LIFE_SAFETY_ZONE_OBJTYPE))
			{
				r=vsbe_unknownproperty;
				goto drwerror;								
			}		
			op=dUnsignedList(op,vb,&r);
			if(r==vsbe_invalidtag) 
				goto drwinvtag;
			else if (r!=vsbe_noerror)
			{
				goto drwerror;											
			}
		}																//	***410 End
		else if (propid==PRIORITY)									//	***1224 Begin
		{
			if(objtype!=NOTIFICATION_CLASS_OBJTYPE)
			{
				r=vsbe_unknownproperty;
				goto drwerror;
			}
			op=dUnsignedList(op,vb,&r);			//treats lists and arrays identically
			if (r==vsbe_invalidtag)
				goto drwinvtag;
			else if (r!=vsbe_noerror)
			{
				goto drwerror;
			}
		}																//	***1224 End
		else if (propid==LOG_DEVICE_OBJECT_PROPERTY )					//	***1223 Begin
		{
			if(iswrite)     
			{
				r = vsbe_writeaccessdenied;
				goto drwerror;
			}   
		}
		else if(propid==EVENT_TIME_STAMPS)
		{
			if(iswrite)     
			{
				r = vsbe_writeaccessdenied;
				goto drwerror;
			}   
		}
		else if (propid==LOG_BUFFER)
		{
			if(iswrite)     
			{
				r = vsbe_writeaccessdenied;
				goto drwerror;
			}   
		}

		else if (propid==PRIORITY_ARRAY)
		{
			if(iswrite)     
			{
				r = vsbe_writeaccessdenied;
				goto drwerror;
			}  
		}
		else if (propid==STATE_TEXT)
		{
			if(iswrite)     
			{
				r = vsbe_writeaccessdenied;
				goto drwerror;
			}  
		}																							//	***1223 End		
		else if((propid==MODIFICATION_DATE)||														//	***1226 Begin
			    (propid==PRESENT_VALUE)||
				(propid==RELINQUISH_DEFAULT))					
		{
			if((propid==MODIFICATION_DATE)&&(objtype!=FILE_OBJTYPE))
			{
				r=vsbe_unknownproperty;
				goto drwerror;
			}
			if((propid==PRESENT_VALUE)||(propid==RELINQUISH_DEFAULT))
			{
				if((objtype!=DATETIME_VALUE_OBJTYPE)&&(objtype!=DATETIME_PATTERN_VALUE_OBJTYPE))
				{
					if(*op++==0x3E)
						goto drwprim;                                                     
					else
						goto drwinvtag;                                                     
				}				
			}
			if((*op!=0x3E)&&(*op!=0x2E)) goto drwinvtag;
			*op++;
			vb->pdtype=adtDateTime;
			vb->ps.ts.type=2;			//datetime
			if(*op==0xA4)
			{
				dDATESTRING(&op,&nc,&vb->ps.ts.date.month,&vb->ps.ts.date.day,&vb->ps.ts.date.dow);
				vb->ps.ts.date.year = (byte)(nc-1900);
			}
			else
				goto drwinvtag;
			if(*op==0xB4)
				dTIMESTRING(&op,&vb->ps.ts.time.hour,&vb->ps.ts.time.minute,&vb->ps.ts.time.second,&vb->ps.ts.time.hundredths);			
			else 
				goto drwinvtag;
			if((*op!=0x3F)&&(*op!=0x2F)) goto drwinvtag;
			if(iswrite&&((word)(op-sop)<dsize)&&((*op&0xF0)==0x40)) //it's a writeproperty with priority	***008
				vb->priority=(byte)dDWORD(&op,false);				//decode priority
			else
				vb->priority=0;										//no priority
		}																							//	***1226 End
		else if(*op++==0x3E)								//we found Value Open Tag	***401 End
		{  
drwprim:	vb->pdtype=((*op)>>4)&0x0F;								//datatype						//	***1226
			r=dPRIMITIVE(&op,vb);							//							***1205
			if (r==(vsbe_invalidtag))                                           //      ***1202
				goto drwinvtag;
			else if (r!=vsbe_noerror)
			{	
drwerror:
				SendError(am,id,service,(word)(r>>8),(word)(r&0xFF));
				goto drwfree;													//		***409
				//return;														//		***409			
			}
			if(*op++!=0x3F) goto drwinvtag;
			if(iswrite&&((word)(op-sop)<dsize)&&((*op&0xF0)==0x40)) //it's a writeproperty with priority	***008
				vb->priority=(byte)dDWORD(&op,false);		//decode priority
			else
				vb->priority=0;								//no priority
		}						
		else												//invalid tag 
		{	
drwinvtag:	SendReject(am,id,REJECT_InvalidTag);
			goto drwfree;									//	***256
			//return;
		}
	}						
//if we get here, it's a read or write and everything is set up in
//objid, propid, arrayindex and vb...
	r=ReadWriteOneProperty(am,objid,propid,arrayindex,iswrite,vb,NULL);	//made into proc
	if (r==vsbe_noerror)
	{	if (iswrite)
			SendSACK(am,id,service);
		else
		{
#ifdef DEBUGVSBHP
			sprintf(t,"call to SendCACK %08X portid=%d",(unsigned int)am,am->src.port);
			fraLog(t);
#endif
			SendCACK(am,id,objid,propid,arrayindex,vb);
		}
	}
	else if((r&0xFF00)==abortreason)						//							***242
		SendAbort(am,id,(octet)(r&0xFF));					//							***242
	else
		SendError(am,id,service,(word)(r>>8),(word)(r&0xFF));
drwfree:													//							***256 Begin
#ifdef DEBUGVSBHP
	sprintf(dbg,"%s calling npoolFreePacket\n",__FUNCTION__);
	fraLog(dbg);											
#endif
	npoolFreePacket(am);									//							***256 End
}
///////////////////////////////////////////////////////////////////////
//	Handler for one ReadProperty or WriteProperty request
//
//  in:	am			incoming packet	
//		objid		the object id
//		propid		the property id
//		arrayindex	the arrayindex
//		iswrite		TRUE if write, FALSE if read
//		vb			pointer to a valuebag
//		nextpid		NULL or pointer to next property id for ALL, REQ, OPT
// out:	r			error code or success=vsbe_noerror

static	int	ReadWriteOneProperty(frNpacket *am,dword objid,dword propid,dword arrayindex,octet iswrite,frVbag *vb,dword *nextpid)
{	
dword			*dw,i,n,nxobjid=noobject,pid=propid;		//	***1222
//dword			nxprop=noproperty;
	const dword			*lp;    // GSB
	word			objtype=objid>>22L;								//	***406
	static frTimeDate		ts;					//	***414
	int				r;
	bool			mustsave=false;
	bool			mustincrement=false;
	octet			b;
	bool			isarray=false;									//	***400
	frString returnPass = {5,5,0,(char*)"*****"};
    //frString emptyPass = {0,0,""};
	int ret_val = vsbe_noerror;

	i=0;															//	***400 Begin
	while(listofarrays[i]!=0xFFFFFFFF)
	{
		if(propid==listofarrays[i]) 
		{
			isarray=true;
			break;
		}
		i++;
	}	
	//An array index is provided but the property is not an array.
	if((false==isarray)&&(noindex!=arrayindex))
	{
		return vsbe_propertyisnotanarray;							
	}		
	i=0;															//	***400 End

#ifdef DEBUGVSBHP
	char			t[1024];
	sprintf(t,"ReadProp: %08X=objid -- %lu = propid -- %ld = arrayindex\r\n",(unsigned int)objid,propid,arrayindex);
	fraLog(t);
#endif
	if((objid>>22L)!=DEVICE_OBJTYPE)							//it's not our device object
	{	
		if((propid==PRIORITY_ARRAY) &&							//	***400 Begin
		   (arrayindex>16)          &&
		   (arrayindex!=noindex))			
		{
invidx:
			r=vsbe_invalidarrayindex;
			return r;
		}
		else if((propid==WEEKLY_SCHEDULE) &&					
				(arrayindex>7)            &&
				(arrayindex!=noindex))							
		{
			goto invidx;
		}														//	***400 End
		else if((propid==EVENT_TIME_STAMPS) &&					//	***406 Begin
				(arrayindex>3)				&&
				(arrayindex!=noindex))
		{
			goto invidx;
		}
		if((propid==PRIORITY) && (objtype==NOTIFICATION_CLASS_OBJTYPE) &&
				(arrayindex>3) && (arrayindex!=noindex))
		{
			goto invidx;
		}														//	***406 End

		if(iswrite)
		{														//	***406 Begin
/*			if ((propid==PRESENT_VALUE) &&
				((objtype==ANALOG_OUTPUT_OBJTYPE)		||
				 (objtype==ANALOG_VALUE_OBJTYPE)		||
				 (objtype==BINARY_OUTPUT_OBJTYPE)		||
				 (objtype==BINARY_VALUE_OBJTYPE)		||
				 (objtype==MULTI_STATE_OUTPUT_OBJTYPE)	||
				 (objtype==MULTI_STATE_VALUE_OBJTYPE)) &&
				(vb->priority==6))
			{
				return vsbe_inconsistentparameters;
			}
			else*/
				r=fraWriteProperty(objid,propid,arrayindex,vb);	 //tell the app we have a write request
		}														//	***406 End
		else
			r=fraReadProperty(objid,propid,arrayindex,vb,nextpid);	//tell the app we have a read request
		if(r==0) r=vsbe_noerror;
		return r;											//return the error
	}
	n=objid&0x3FFFFF;										//get instance number
	if(n!=di.deviceinstance&&n!=wildcardinst)				//not our instance or the wild instance
		return vsbe_unknownobject;
//it's our device object
nextprop:
	vb->narray=0;											//so we don't return an array unless we need to		***002
	switch(pid)
	{
	case ALLX:
	case REQUIRED:
	case OPTIONALX:
		if(!nextpid) return vsbe_inconsistentparameters;		//												***418
		if(iswrite)	return vsbe_writeaccessdenied;
		if(pid==ALLX)
			lp=DEalllist;
		else if(pid==REQUIRED)
			lp=DEreqlist;
		else //if(pid==OPTIONALX)
			lp=DEoptlist;
		pid=*nextpid;										//search list for this one
		while (*lp!=0xFFFFFFFF)
		{	if (pid==(dword)(*lp++))
			{	*nextpid=(dword)(*lp);						//get next one
				if (*nextpid==0xFFFF) break;				//we're all done
				pid=*nextpid;
				goto nextprop;
			}
		}
		return vsbe_unknownproperty;						//we're all done
//----------------------------------------------------------------------------------------------------
//Device properties (alphabetically sorted):
	case ACTIVE_COV_SUBSCRIPTIONS:							//	***410 Begin
	  if(!fraCanDoCOV())
      {                                                     //  ***1201 Begin
          if (propid==ALLX||propid==OPTIONALX)
          {	pid=propid;
              goto nextprop;
          }
          return vsbe_unknownproperty;
      }
      if(iswrite)	return vsbe_writeaccessdenied;          //  ***1201 End
      vb->pdtype=adtEmptyList;
      vb->narray=0;
      n=fraGetNumberOfCOVSubscriptions();
	  if(n>MAXCOVSUBS) n=MAXCOVSUBS;
      if(n)
      {
         r=fraGetCOVSubscriptions(vb); 
         if(r==vsbe_noerror) // okay
         {
            vb->pdtype=adtCOVSubscription;
            vb->narray=(byte)n;
         }
         else // failure
            return r;
      }
      break;												//	***410 End

	case APDU_TIMEOUT:
		if(iswrite)
		{
	           return vsbe_writeaccessdenied;
	          /*if(vb->pdtype!=adtUnsigned) return vsbe_invaliddatatype;
				if(vb->pd.uval>255000) return vsbe_valueoutofrange;
				di.apdu_timeout=(byte)(vb->pd.uval/1000);		//convert to whole seconds
				mustsave=true;
	          */
		}
		else
		{	vb->pdtype=adtUnsigned;
			vb->pd.uval=di.apdu_timeout*1000;				//convert to milliseconds
		}
		break;

	case APPLICATION_SOFTWARE_VERSION:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtCharString;			
		vb->narray=0;
		buildvbString(vb);									//	***1205
		if(frStringCpy(&vb->pd.stval,di.application_software_ver,di.application_software_ver->len)!=di.application_software_ver->len)	//	***1200
			return vsbe_resourcesother;
		break;
			
	case DATABASE_REVISION:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtUnsigned;
		vb->pd.uval=fraGetDatabaseRevision();				//ask application for it
		break;

	case DAYLIGHT_SAVINGS_STATUS:
		if (!fraCanDoUTCTimeSync())							//	***404 Begin
		{	
			if (propid==ALLX||propid==OPTIONALX)	
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 													//	***404 End
		if(iswrite)	return vsbe_writeaccessdenied;
		if (fraGetTimeDate(&ts)==false) return vsbe_servicerequestdenied;	//	***1223
		vb->pdtype=adtBoolean;
		vb->pd.uval=(dword)ts.dst;
		break;
	
	case DESCRIPTION:
		if(iswrite)
		{	
            return vsbe_writeaccessdenied;
/*

			if(vb->pdtype!=adtCharString) return vsbe_invaliddatatype;
         if(vb->pd.stval.len>di.device_description->maxlen-1) return vsbe_valueoutofrange;        // ***400
			if(frStringCpy(di.device_description,&vb->pd.stval,vb->pd.stval.len)!=vb->pd.stval.len)	//	***1200
				return vsbe_resourcesother;															//	***1200
			mustsave=true;
*/
		}
		else
		{	
			vb->pdtype=adtCharString;			
			vb->narray=0;	
			buildvbString(vb);								//	***1205
			if(frStringCpy(&vb->pd.stval,di.device_description,di.device_description->len)!=di.device_description->len)	//	***1200
				return vsbe_resourcesother;
		}
		break;

	case DEVICE_ADDRESS_BINDING:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtEmptyList;
		break;

    case FIRMWARE_REVISION:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtCharString;			
		vb->narray=0;	
		buildvbString(vb);									//	***1205
		if(frStringCpy(&vb->pd.stval,di.firmware_rev,di.firmware_rev->len)!=di.firmware_rev->len)	//	***1200
				return vsbe_resourcesother;
		break;

	case LAST_RESTART_REASON:								//	***1200 Begin
		if(!fraCanDoRestartNotify())
		{
			if (propid==ALLX||propid==OPTIONALX)	
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		}
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtEnumerated;
		vb->narray=0;
		vb->pd.uval=di.lastrestartreason;
		break;										//	***1200 End

	case LOCAL_DATE:
		if (!fraCanDoTimeSync() &&					//	***404 Begin
			!fraCanDoUTCTimeSync())					
		{	
			if (propid==ALLX||propid==OPTIONALX)	
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 											//	***404 End
		if(iswrite)	return vsbe_writeaccessdenied;
		if (fraGetTimeDate(&ts)==false) return vsbe_unknownproperty;
		vb->pdtype=adtDate;
		vb->pd.dtval.year=(byte)(ts.year-1900);
		vb->pd.dtval.month=ts.month;
		vb->pd.dtval.day=ts.day;
		vb->pd.dtval.dow=ts.weekday;
		break;
					
	case LOCAL_TIME:
		if (!fraCanDoTimeSync() &&					//	***404 Begin
			!fraCanDoUTCTimeSync())					
		{	
			if (propid==ALLX||propid==OPTIONALX)	
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 											//	***404 End
		if(iswrite)	return vsbe_writeaccessdenied;
		if (fraGetTimeDate(&ts)==false) return vsbe_unknownproperty;
		vb->pdtype=adtTime;
		vb->pd.tval.hour=ts.hour;
		vb->pd.tval.minute=ts.minute;
		vb->pd.tval.second=ts.second;
		vb->pd.tval.hundredths=ts.hundredths;
		break;
			
	case LOCATION:
/*		if(iswrite)
		{	
			if(vb->pdtype!=adtCharString) return vsbe_invaliddatatype;
         if(vb->pd.stval.len>di.device_location->maxlen-1) return vsbe_valueoutofrange;        // ***400
			if(frStringCpy(di.device_location,&vb->pd.stval,vb->pd.stval.len)!=vb->pd.stval.len)	//	***1200
				return vsbe_resourcesother;															//	***1200
			mustsave=true;
		}
		else
		{	
			vb->pdtype=adtCharString;			
			vb->narray=0;		
			buildvbString(vb);						//	***1205
			if(frStringCpy(&vb->pd.stval,di.device_location,di.device_location->len)!=di.device_location->len)	//	***1200
				return vsbe_resourcesother;
		}*/

		return vsbe_unknownproperty;
		break;
			
	case MAX_APDU_LENGTH_ACCEPTED:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtUnsigned;
		vb->pd.uval=macMaxAPDU(am->src.port);		//get max apdu length
		break;

	case MAX_MASTER:
		if (portid!=portMSTP)
		{				
			if (propid==ALLX||propid==OPTIONALX)	// ***404
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 
		if(di.maxmaster==0)								//	***1200 Begin
		{
			if (propid==ALLX||propid==OPTIONALX)
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		}												//	***1200 End
		if(iswrite)                                     // DH1-131 made Writable
		{
			if(vb->pdtype!=adtUnsigned) return vsbe_invaliddatatype;
			if(vb->pd.uval>127) return vsbe_valueoutofrange;
			di.maxmaster=(byte)vb->pd.uval;
			mstpMaster(di.maxmaster);
			mustsave=true;
		}
		else
		{	vb->pdtype=adtUnsigned;
			vb->pd.uval=(dword)di.maxmaster;
		}
		break;

	case MAX_INFO_FRAMES:
		if (portid!=portMSTP) 
		{	
			if (propid==ALLX||propid==OPTIONALX)	// ***404		 
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 
		if(di.maxmaster==0)								//	***1200 Begin
		{
			if (propid==ALLX||propid==OPTIONALX)
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		}												//	***1200 End
		if(iswrite)
		{	/*if(vb->pdtype!=adtUnsigned) return vsbe_invaliddatatype;
			if(vb->pd.uval>255) return vsbe_valueoutofrange;
			di.maxinfoframes=(byte)vb->pd.uval;
			mustsave=true;*/
	          return vsbe_writeaccessdenied;

		}
		else
		{	vb->pdtype=adtUnsigned;
			vb->pd.uval=(dword)di.maxinfoframes;
		}
		break;
	
	case MODEL_NAME:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtCharString;
		vb->narray=0;
		buildvbString(vb);								//	***1205
		if(frStringCpy(&vb->pd.stval,di.model_name,di.model_name->len)!=di.model_name->len)	//	***1200
				return vsbe_resourcesother;													//	***1200		
		break;

	case NUMBER_OF_APDU_RETRIES:
		if(iswrite)
		{	/*if(vb->pdtype!=adtUnsigned) return vsbe_invaliddatatype;
			if(vb->pd.uval>10) return vsbe_valueoutofrange;
			di.apduretries=(byte)vb->pd.uval;
			mustsave=true;*/
	           return vsbe_writeaccessdenied;
		}
		else
		{	vb->pdtype=adtUnsigned;
			vb->pd.uval=(dword)di.apduretries;
		}
		break;

	case OBJECT_IDENTIFIER:
		if(iswrite)
		{
	           return vsbe_writeaccessdenied;
/*

			if(vb->pdtype!=adtObjectID) return vsbe_invaliddatatype;
			n=vb->pd.uval&0x3FFFFF;
			if((vb->pd.uval>>22L)!=DEVICE_OBJTYPE||
			   n==0x3FFFFF) return vsbe_valueoutofrange;
			di.deviceinstance=n;
			mustincrement=true;		
			mustsave=true;
*/
		}
		else
		{				
			vb->pdtype=adtObjectID;
			//vb->pd.uval=objid;												//	***403
			vb->pd.uval=((DEVICE_OBJTYPE<<22L) + di.deviceinstance);			//	***403
		}
		break;
	
	case OBJECT_LIST:
		if(iswrite)	return vsbe_writeaccessdenied;
		n=fraGetNumberOfObjects();
		if(arrayindex==0)								//return the number of objects
		{	vb->pdtype=adtUnsigned;
			vb->pd.uval=n;
		}
		else if(arrayindex!=noindex)
		{	if(arrayindex>n)
				return vsbe_invalidarrayindex;
			nxobjid=fraGetObjectAt(arrayindex);								//	***263 Begin
			if(nxobjid!=noobject)
			{
				vb->pdtype=adtObjectID;
				vb->pd.uval=nxobjid;
			}																//	***263 End
		}
		else
		{
			//vsbhp policy is to not return a partial list if we can't fit it
			if(n>maxobjectlist)
			{	
				return vsbe_segmentationnotsupported;				
			}	
			vb->pdtype=adtObjectList;
			vb->pd.uval=n;
			dw=vb->ps.psdval;
			for(i=1;i<=n;i++)
			{	
				nxobjid=fraGetObjectAt(i);									//	***263
				*dw++=nxobjid;
			}
		}
		break;
		
	case OBJECT_TYPE:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtEnumerated;
		vb->pd.uval=DEVICE_OBJTYPE;
		break;

	case OBJECT_NAME:  // Fixed DH1-217
		if(iswrite)
		{
#ifdef OBJECT_NAME_WRITE_ENABLE
			if(vb->pdtype!=adtCharString) return vsbe_invaliddatatype;
			if(vb->pd.stval.len>di.device_name->maxlen-1) return vsbe_valueoutofrange;				//	***400	***1229
			if(vb->pd.stval.len<1) return vsbe_valueoutofrange;										//	***1200
			if(frStringCpy(di.device_name,&vb->pd.stval,vb->pd.stval.len)!=vb->pd.stval.len)		//	***1200
				return vsbe_resourcesother;			
			mustsave=true;
			mustincrement=true;
#else
			return vsbe_writeaccessdenied;
#endif
		}
		else
		{	
			vb->pdtype=adtCharString;			
			vb->narray=0;
			buildvbString(vb);							//	***1205
			if(frStringCpy(&vb->pd.stval,di.device_name,di.device_name->len)!=di.device_name->len)	//	***1200
				return vsbe_resourcesother;			
		}
		break;

	case PROFILE_NAME:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtCharString;
		vb->narray=0;
		buildvbString(vb);								//	***1205
		if(frStringCpy(&vb->pd.stval,di.device_profile,di.device_profile->len)!=di.device_profile->len)	//	***1200
				return vsbe_resourcesother;													//	***1200		
		break;

      case PROPERTY_LIST:								//								***1206 Begin
/*
            if(iswrite) return vsbe_writeaccessdenied;
			if (propid==ALLX||propid==REQUIRED)			// if RPM, skip this one		***1217 Begin
			{												
				pid=propid;
				goto nextprop;
			}											//								***1217 End
            n=sizeof(DEalllist)-1;
            if(arrayindex==0)							//return the number of properties
            {
                  vb->pdtype=adtUnsigned;
                  vb->pd.uval=n;
            }
            else if(arrayindex!=noindex)
            {
                  if(arrayindex>n)
                        return vsbe_invalidarrayindex;
                  nxprop=DEalllist[arrayindex-1];									//	***1222 Begin
                  if(nxprop!=noproperty)						
                  {
                        vb->pdtype=adtEnumerated;
                        vb->pd.uval=nxprop;
                  }
            }
            else
            {
                  if(n>maxobjectlist)
                        return vsbe_segmentationnotsupported;                       
                  vb->pdtype=adtEnumerated;
                  vb->pd.uval=n;
                  vb->narray=(byte)n;
                  dw=vb->ps.psdval;
                  for(i=0;i<n;i++)
                  {     
                        nxprop=DEalllist[i];
                        *dw++=nxprop;
                  }																	//	***1222 End
            }
*/
            return vsbe_unknownproperty;
            break;										//								***1206 End

	case PROTOCOL_OBJECT_TYPES_SUPPORTED:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtBitString;
		//vb->pd.uval=17;									//17 bits of significance	***400 Begin		
		//vb->pd.uval=25;									//25 bits of significance	***404
		//vb->pd.uval=31;									//31 bits of significance	***600 ***601
		vb->pd.uval=51;										//51 bits of significance	***1200
		vb->ps.psval[0]=di.object_types_supported[0];
		vb->ps.psval[1]=di.object_types_supported[1];
		vb->ps.psval[2]=di.object_types_supported[2];
		vb->ps.psval[3]=di.object_types_supported[3];		//							***400 End ***404
		vb->ps.psval[4]=di.object_types_supported[4];		//							***1200 Begin
		vb->ps.psval[5]=di.object_types_supported[5];
		vb->ps.psval[6]=di.object_types_supported[6];		//							***1200 End
		break;

	case PROTOCOL_REVISION:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtUnsigned;
		vb->pd.uval=12;									//								***1200
		break;	

	case PROTOCOL_SERVICES_SUPPORTED:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtBitString;
		vb->pd.uval=40;									//40 bits of significance
//----------------------------------
//	acknowledgeAlarm			0x80
//	confirmedCOVNotification	0x40
//	confirmedEventNotification	0x20
//	getAlarmSummary				0x10
//	getEnrollmentSummary		0x08
//	subscribeCOV				0x04
//	atomicReadFile				0x02
//	atomicWriteFile				0x01
		b=0x00;											//											***1218											
		if(fraCanDoCOVNotifications(false)) b |= 0x40;	//app can accept confirmedCOVNotification	***1224
		if(fraCanDoFiles()) b|=0x03;					//app has files
		if(fraCanDoCOV()) b|=0x04;						//app accepts COV subscriptions		
		if(fraCanDoAlarmAck()) b|=0x80;					//app accepts Alarm Acknowledge				***404
		if(fraCanDoAlarmSummary()) b|=0x10;				//app accpets Alarm Summary					***404
		vb->ps.psval[0]=b;
//----------------------------------
//	addListElement				0x80
//	removeListElement			0x40
//	createObject				0x20
//	deleteObject				0x10
//	readProperty				0x08
//	readPropertyConditional		0x04
//	readPropertyMultiple		0x02
//	writeProperty				0x01
		b=(fraCanDoRPM())?0x0B:0x09;						//					***1205
		if(fraCanDoOCD()) b|=0x30;							//					***1205
		vb->ps.psval[1]=b;									//					***1205
//----------------------------------
//	writePropertyMultiple		0x80
//	deviceCommunicationControl	0x40
//	confirmedPrivateTransfer	0x20
//	confirmedTextMessage		0x10
//	reinitializeDevice			0x08
//	vtOpen						0x04
//	vtClose						0x02
//	vtData						0x01		
		b=0x40;												//can always do DCC		
		if(fraCanDoWPM())	 b|=0x80;						//					***418
		if(fraCanDoReinit()) b|=0x08;
		if(fraCanDoPvtTransfer(true)) b|=0x20;				//					***1205
		vb->ps.psval[2]=b;									//					***404 End		
//----------------------------------
//	authenticate				0x80
//	requestKey					0x40
//Unconfirmed Services
//	i-Am						0x20
//	i-Have						0x10
//	unconfirmedCOVNotification	0x08
//	unconfirmedEventNotification 0x04
//	unconfirmedPrivateTransfer	0x02
//	unconfirmedTextMessage		0x01
		if(di.maxmaster!=0)									//can't do these if we're a slave			***1200 Begin
		{
            // Disabled I-Am as we dont support it DH1-181
			vb->ps.psval[3]=//0x20|							//can always do IAm (0x20)	***261
//							0x10|									//can execute I-Have (0x10)			***1225
							((fraCanDoCOVNotifications(false)) ? 0x08 : 0x00)|						//	***1225
						    ((fraCanDoPvtTransfer(false))?0x02:0x00);	//		***1205
		}
		else
		{
			//fraCanDoPvtTransfer must differentiate between unconfirmed
			//and confirmed pvt transfer now. 
			vb->ps.psval[3]=0x00|((fraCanDoPvtTransfer(false))?0x02:0x00);	//	***1205
		}
//----------------------------------
//	timeSynchronization			0x80
//	who-Has						0x40
//	who-Is						0x20
//Services added after 1995
//	readRange					0x10
//	utcTimeSynchronization		0x08
//	lifeSafetyOperation			0x04
//	subscribeCOVProperty		0x02
//	getEventInformation			0x01		
		if(di.maxmaster!=0)						//can't do these if we're a slave			***1200 Begin
		{
			vb->ps.psval[4]=0x20|0x40;				//always do WhoIs and WhoHas	***404
			vb->ps.psval[4]|=((fraCanDoTimeSync())?0x80:0x00);					
			vb->ps.psval[4]|=((fraCanDoUTCTimeSync())?0x08:0x00);				//	***404
			vb->ps.psval[4]|=((fraCanDoCOVProperty())?0x02:0x00);				//	***1223
		}
		else
		{
			vb->ps.psval[4]=0x00;
		}
		vb->ps.psval[4]|=((fraCanDoReadRange())?0x10:0x00);					//	***404
		vb->ps.psval[4]|=((fraCanDoEventInfo())?0x01:0x00);					//	***404		***1200 End
		break;

	case PROTOCOL_VERSION:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtUnsigned;
		vb->pd.uval=1;
		break;
	
	case RESTART_NOTIFICATION_RECIPIENTS:									//	***1200 Begin
		if (!fraCanDoRestartNotify())					
		{	
			if (propid==ALLX||propid==OPTIONALX)	
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 										
		n=fraGetNumberofRestartRecipients();
		if(n==0)
		{
			vb->pdtype=adtEmptyList;
		}
		else
		{
			if(n>MAXRECIPS) n=MAXRECIPS;
			r=fraGetRestartNotifyRecipients(vb); 
			if(r==vsbe_noerror) // okay
			{
				vb->narray=(byte)n;
				vb->pdtype=adtRecipientAddr;
			}
			else 
				return r;
		}
		break;																//	***1200 End

	case SEGMENTATION_SUPPORTED:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtEnumerated;
		vb->pd.uval=3;									//no segmentation
		break;

	case STRUCTURED_OBJECT_LIST:												//	***600 Begin
		if(!fraCanDoStructuredView())
        {                                                                       //  ***1201 Begin
			if (propid==ALLX||propid==OPTIONALX)
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		}                                                                       //  ***1201 End
		if(iswrite)	return vsbe_writeaccessdenied;
		n=fraGetNumberOfStructuredViewObjects();
		if(arrayindex==0)
		{
			vb->pdtype=adtUnsigned;
			vb->pd.uval=n;
		}
		else if(arrayindex!=noindex)
		{
			if(arrayindex>n)
				return vsbe_invalidarrayindex;
			nxobjid=fraGetStructuredViewObjectAt(arrayindex);				
			if(nxobjid!=noobject)
			{
				vb->pdtype=adtObjectID;
				vb->pd.uval=nxobjid;
			}																
		}
		else
		{
			//vsbhp policy is to not return a partial list if we can't fit it
			if(n>maxobjectlist)
			{	
				return vsbe_segmentationnotsupported;				
			}	
			vb->pdtype=adtObjectList;
			vb->pd.uval=n;
			dw=vb->ps.psdval;
			for(i=1;i<=n;i++)
			{	
				nxobjid=fraGetStructuredViewObjectAt(i);
				*dw++=nxobjid;
			}
		}					
		break;																	//	***600 End

	case SYSTEM_STATUS:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtEnumerated;
		vb->pd.uval=(dword)di.system_status;			//is this dynamic?
		break;

	case TIME_OF_DEVICE_RESTART:													//	***1200 Begin
		if (!fraCanDoRestartNotify())					
		{	
skipTDR:
			if (propid==ALLX||propid==OPTIONALX)	
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 											
		if(iswrite)	return vsbe_writeaccessdenied;
		if (fraGetLastRestartTime(&ts)==false) 
		{
			goto skipTDR;
		}		
		vb->pdtype=adtTimeStamp;
		vb->ps.ts.type=tsDTType;
		vb->ps.ts.date.day=ts.day;
		vb->ps.ts.date.dow=ts.weekday;
		vb->ps.ts.date.month=ts.month;
		vb->ps.ts.date.year=(byte)(ts.year-1900);		
		vb->ps.ts.time.hour=ts.hour;
		vb->ps.ts.time.hundredths=ts.hundredths;
		vb->ps.ts.time.minute=ts.minute;		
		vb->ps.ts.time.second=ts.second;		
		break;																		//	***1200 End
			
	case UTC_OFFSET:
		if (!fraCanDoUTCTimeSync())					//	***404 Begin
		{	
			if (propid==ALLX||propid==OPTIONALX)	
			{	pid=propid;
				goto nextprop;
			}
			return vsbe_unknownproperty;
		} 											//	***404 End
		if(iswrite)
		{	if(vb->pdtype!=adtInteger) return vsbe_invaliddatatype;
			if(vb->pd.sval<-720||vb->pd.sval>720) return vsbe_valueoutofrange;
			di.utcoffset=(short)vb->pd.sval;
			//Think we need to call fraSetTimeDate here?
			mustsave=true;
		}
		else
		{	vb->pdtype=adtInteger;
			vb->pd.sval=di.utcoffset;
		}
		break;
		
	case VENDOR_IDENTIFIER:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtUnsigned;
		vb->pd.uval=(dword)di.vendor_id;
		break;

	case VENDOR_NAME:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtCharString;			
		vb->narray=0;
		buildvbString(vb);							//	***1205
		if(frStringCpy(&vb->pd.stval,di.vendor_name,di.vendor_name->len)!=di.vendor_name->len)	//	***1200
			return vsbe_resourcesother;	
		break;
	case EATON_DEFAULT_PASSWORD:
        if(iswrite) //return vsbe_writeaccessdenied;
        {
          if(vb->pdtype!=adtCharString) return vsbe_invaliddatatype;
            if(vb->pd.stval.len>20) return vsbe_valueoutofrange;
            *di.password = vb->pd.stval;
        }
        else
        {
            vb->pdtype=adtCharString;
            vb->pd.stval.len = returnPass.len;
            vb->narray=0;
            vb->pd.stval.text = returnPass.text;
        }
	break;

	//Nishchal D: Added SERIAL_NUMBER
	case SERIAL_NUMBER:
		if(iswrite)	return vsbe_writeaccessdenied;
		vb->pdtype=adtCharString;
		vb->narray=0;
		buildvbString(vb);
		if(frStringCpy(&vb->pd.stval,di.serial_num,di.serial_num->len)!=di.serial_num->len)
				return vsbe_resourcesother;

		break;


	default:
		ret_val = Check_Prod_Spec_Property(pid,vb,iswrite,&mustsave);
	}
	if(mustsave)														//	***404 Begin
	{
		r=fraSaveDeviceVars(&di);				//device object was written to
		if(mustincrement&&r==vsbe_noerror)		//need to increment the database revision
			fraIncrementDatabaseRevision();		
		fraGetDeviceInfo(&di);					//update the global guy (revert or new stuff)
		return r;
	}																	//	***404 End
	return ( ret_val );														//	***404 End
	//return vsbe_noerror;
}

///////////////////////////////////////////////////////////////////////				***1221 Begin
//	Handler for ReadPropertyMultiple service
//
//  in:	am		incoming packet	
//  	op		points to the PDU
//		dsize	is the number of octets to decode
//		id		is the invoke id
//		vb		points to value bag we can use

static	void HandleReadPropertyMultiple(frNpacket *am,octet *op,word dsize,octet id,frVbag *vb)
{	
octet		*sop,*stp,*ttp,*tp,firstprop,allprops;
octet		*txb;									
frNpacket	*txPacket;
frNpacket *bm;
octet		bmnflag;										
dword	objid,objinst,propid,arrayindex,nextpid;
word		objtype;											
word		n;												
int			r=vsbe_noerror;										
int		ec;

	n=macMaxRPDU(am->src.port);
	txPacket=npoolGetPacket(maxnl);
	if(!txPacket) 
	{
		SendAbort(am,id,vsbe_segmentationnotsupported&0x00FF);							//	***1223
		npoolFreePacket(am);
		return;
	}
	txb=txPacket->npdu;																	//just use the NPDU buffer, we don't care about the rest
	stp=ttp=tp=txb;
	sop=op;
	*ttp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;		//PDU header
	*ttp++=id;													//invoke id
	*ttp++=readPropertyMultiple;														//service
	while ((word)(op-sop)<dsize)
	{	
		firstprop=true;
		if((*op&0xF0)!=0)										//[0]BACnetObjectID
			goto rpminvtag;
		objid=dDWORD(&op,false);								//decode object id
		objinst=objid&0x3FFFFF;															//get instance number	
		objtype=objid>>22L;											//get object type
		if((objinst==wildcardinst)&&(objtype==DEVICE_OBJTYPE))		//is it the wildcard?
		{
			objid=(di.deviceinstance + ((dword)(DEVICE_OBJTYPE)<<22));	//coerce to our dev instance
			r=vsbe_noerror;																//we must reset it here					
		}												
		else if(objinst==wildcardinst)								//not the device object, NAK it
		{
			r=vsbe_unknownobject;									//otherwise, unknown object
			//we need to continue decoding
		}																				
		if(*op++!=0x1E)											//[1]opening tag BACnetPropertyRef
			goto rpminvtag;
		if((*op&0xF0)!=0)										//	[0]BACnetPropertyID
			goto rpminvtag;
nextprop:	
		propid=dDWORD(&op,false);								//	decode property id
		if((*op!=0x1F)&&((*op&0xF0)==0x10))						//	[1]Unsigned
			arrayindex=dDWORD(&op,false);						//	decode array index
		else
			arrayindex=noindex;
		allprops=false;
		nextpid=0xFFFFFFFF;									//don't really care as long as it's not ALL, REQUIRED or OPTIONAL
		vb->priority=0;										//no priority
		if(r!=vsbe_noerror)																//issue decoding object or property					
		{
			ec=frGetErrorClass(r);
			if((ec==frGetErrorClass(objecterror))||			//these are object classes that can be embedded in RPM CACK
				(ec==frGetErrorClass(propertyerror)))
			{
				goto skip1;									//so skip it, but include the error in the CACK
			}
		}	
		if ((propid==ALLX		|| 
			propid==REQUIRED	|| 
			propid==OPTIONALX))
		{	
			allprops=true;
			nextpid=propid;
			if(arrayindex!=noindex)														//these special props cannot be arrays		
			{	
				r=vsbe_propertyisnotanarray;					
				goto skip1;
			}
		}
nextval:
		r=ReadWriteOneProperty(am,objid,propid,arrayindex,false,vb,&nextpid);
		if(r==vsbe_segmentationnotsupported) 
		{
			SendAbort(am,id,vsbe_segmentationnotsupported&0x00FF);
			goto rpmfree;        
		} 
skip1:		
		if (firstprop)																	//Encode:
		{	
			if (WouldOverflow(&am->src,stp,ttp,7)) 
				goto oflow;																//make sure we have room for the objectid and open/close 1E/1F
			ttp=etagOBJECTID(ttp,0x08,objid);											//[0]objectid
			*ttp++=0x1E;																//[1] opening tag listofresults
		}
		if ((allprops==false) || (allprops && (r!=vsbe_unknownproperty)))
		{	
			if (WouldOverflow(&am->src,stp,ttp,(word)((arrayindex!=noindex)?7:12)))		//make sure we have room for the max propid and arrayinde and tags	***226
			{					
				SendAbort(am,id,vsbe_segmentationnotsupported&0x00FF);
				goto rpmfree;
			}							
			ttp=eDWORD(ttp,(allprops)?nextpid:propid,0x28,false);						//[2]propertyid
			if(arrayindex!=noindex)
				ttp=eDWORD(ttp,arrayindex,0x38,false);									//[3]arrayindex
			firstprop=false;
			if (r==vsbe_noerror)
			{	
				if (WouldOverflow(&am->src,stp,ttp,2)) 
					goto oflow;															//make sure we have room for the open/close 4E/4F
				*ttp++=0x4E;															//[4] opening tag property value
				if(EncodePropertyValue(&ttp,vb,(word)(n-(ttp-stp))))					//can't fit it
				{						
					SendAbort(am,id,vsbe_segmentationnotsupported&0x00FF);
					goto rpmfree;
				}
				*ttp++=0x4F;															//[4] closing tag property value
				if (allprops) goto nextval;
			}
			else
			{	
				if ((allprops==false)||(allprops && (r!=vsbe_unknownproperty)))	
				{	
					if (WouldOverflow(&am->src,stp,ttp,12)) 
						goto oflow;														//make sure we have room for the 5E class...code 5F
					*ttp++=0x5E;														//[5] opening tag property access error						
					ttp=eENUM(ttp,(dword)((r>>8)&0xFF));								// error class
					ttp=eENUM(ttp,(dword)(r&0xFF));										// error code
					*ttp++=0x5F;														//[5] closing tag property access error
					r=vsbe_noerror;														//must reset the error code 
					if (allprops) goto nextval;
				}													//else all done with all, opt or req
			}
		}
		if((*op&0xF0)==0)																//multiple properties for this objid
		{
			if ((word)(op-sop)>=dsize)
				goto rpminvtag;																
			r=vsbe_noerror;																//must reset the error code
			goto nextprop;
		}
		*ttp++=0x1F;																	//Encode [1] closing tag listofresults					
		if(*op++!=0x1F)																	//[1]closing tag BACnetPropertyRef
		{			
rpminvtag:	
			SendReject(am,id,REJECT_InvalidTag);
			goto rpmfree;
		}
	}
	tp=RSVP(am,0);																		//set up reply buffer
	n=(word)(am->nlen+(ttp-stp));														//total length of this blivit
	if(n>npoolNPDUsize(am))																//ouch, can't fit this much
	{																					//here, n should always be <= maxmac 
		//but we may not have a big buffer am points to.
		if ((am->nflag&Nisbig)==0)														//it's a small buffer
		{	
			if((bm=npoolGetPacket(maxmac))!=NULL)										//try to get a big packet
			{
				bmnflag=bm->nflag;														//preserve the flag so it gets freed correctly
				frcpy(bm,am,sizeof(frNpacket));
				bm->nflag=bmnflag;														//preserve the flag so it gets freed correctly					
				npoolFreePacket(am);													//done with small guy					
				am=bm;					
			}
			else																		//can't get a big buffer and we need one
			{
oflow:		
				SendAbort(am,id,vsbe_segmentationnotsupported&0x00FF);	
				goto rpmfree;													
			}
		}
		tp=RSVP(am,0);
	}		
	bytecpy(tp,stp,(word)(ttp-stp));
	transmit(am,n,am->npdu,macisResponse,false);  
rpmfree:																
	npoolFreePacket(am);	
	npoolFreePacket(txPacket);
}																

///////////////////////////////////////////////////////////////////////
//	Handler for WritePropertyMultiple service
//
//  in:	am		incoming packet	
//  	op		points to the PDU
//		dsize	is the number of octets to decode
//		id		is the invoke id
//		vb		points to value bag we can use

static	void HandleWritePropertyMultiple(frNpacket *am,octet *op,word dsize,octet id,frVbag *vb)
{
	octet		*sop, *stp, *ttp, *tp;// , firstprop, allprops;
dword		objid,propid,arrayindex,nextpid;
//dword		objinst;
word		objtype;											
word		n,year;
int			r=vsbe_noerror;										
octet		appliedatleastone=0x00;

	n=macMaxRPDU(am->src.port);	
	sop=op;		
	while ((word)(op-sop)<dsize)
	{
		//firstprop=true;
		if((*op&0xF0)!=0)																//[0]BACnetObjectID
			goto wpminvtag;
		objid=dDWORD(&op,false);														//decode object id
//        objinst=objid&0x3FFFFF;															//get instance number
		objtype=objid>>22L;		
		if(*op++!=0x1E)																	//[1]opening tag BACnetPropertyRef
			goto wpminvtag;
		if((*op&0xF0)!=0)																//	[0]BACnetPropertyID
			goto wpminvtag;
nextprop:	
		propid=dDWORD(&op,false);														//	decode property id
		if((*op!=0x1F)&&((*op&0xF0)==0x10))												//	[1]Unsigned
			arrayindex=dDWORD(&op,false);												//	decode array index
		else
			arrayindex=noindex;
		//allprops=false;
		nextpid=0xFFFFFFFF;																//don't really care as long as it's not ALL, REQUIRED or OPTIONAL
		vb->priority=0;																	//no priority
		if(r!=vsbe_noerror)																//issue decoding object or property					
		{
			goto firstfailed;
		}	
		objtype=objid>>22L;			
		if(propid==RECIPIENT_LIST)
		{
			op=dRecipientList(op,vb,&r);				
			if(r==vsbe_invalidtag) 
				goto wpminvtag;
			else if (r!=vsbe_noerror)
			{
				SendError(am,id,writePropertyMultiple,(word)(r>>8),(word)(r&0xFF));
				goto wpmfree; 
			}
			if(*op==0x2F) *op++;							//increment to the x'1F' 
		}
		else if(propid==WEEKLY_SCHEDULE)
		{
			op=dWeeklySchedule(op,vb,arrayindex,&r);
			if(r==vsbe_invalidtag) 
				goto wpminvtag;
			else if (r!=vsbe_noerror)
			{
				SendError(am,id,writePropertyMultiple,(word)(r>>8),(word)(r&0xFF));
				goto wpmfree;
			}
			if(*op==0x2F) *op++;							//increment to the x'1F'
		}
		else if(propid==EXCEPTION_SCHEDULE)
		{
			op=dSpecialEvent(op,vb,arrayindex,&r);
			if(r==vsbe_invalidtag) 
				goto wpminvtag;
			else if (r!=vsbe_noerror)
			{
				SendError(am,id,writePropertyMultiple,(word)(r>>8),(word)(r&0xFF));
				goto wpmfree;
			}
			if(*op==0x2F) *op++;							//increment to the x'1F'
		}
		else if(propid==EFFECTIVE_PERIOD)
		{
			if(*op++!=0x2E) goto wpminvtag;
			vb->pdtype=adtEffectivePeriod;
			dDATESTRING(&op,&year,&vb->ps.ep.start.month,&vb->ps.ep.start.day,&vb->ps.ep.start.dow);
			vb->ps.ep.start.year=(year==0xFFFF)?0xFF:(byte)(year-1900);
			dDATESTRING(&op,&year,&vb->ps.ep.end.month,&vb->ps.ep.end.day,&vb->ps.ep.end.dow);
			vb->ps.ep.end.year=(year==0xFFFF)?0xFF:(byte)(year-1900);
			if(*op++!=0x2F) goto wpminvtag;
		}
		else if(propid==LIST_OF_OBJECT_PROPERTY_REFERENCE)
		{			
			if(*op++!=0x2E) goto wpminvtag;	
			vb->pdtype=adtDeviceObjectPropertyReference;
			vb->narray=0;
nextdopr:
			vb->ps.psdopr[vb->narray].objid=dDWORD(&op,false);	//[0]      BACnetObjectIdentifier
			vb->ps.psdopr[vb->narray].propid=dUINT(&op);		//[1]      BACnetPropertyIdentifier
			if((*op&0xF0)==0x20)								//[2]      Unsigned OPTIONAL
				vb->ps.psdopr[vb->narray].arrayindex=dUINT(&op);
			else
				vb->ps.psdopr[vb->narray].arrayindex=noindex;
			if(((*op&0xF0)==0x30))								//[3]      BACnetObjectIdentifier OPTIONAL
				vb->ps.psdopr[vb->narray].deviceid=dDWORD(&op,false);
			else
				vb->ps.psdopr[vb->narray].deviceid=nodevice;
			if(*op!=0x2F)			//either an end tag
			{
				if(*op==0x0C)		//or this begins another element
				{
					vb->narray++;
					if(vb->narray>=MAXDOPR) 
					{
						r=vsbe_resourcesother;
						SendError(am,id,writePropertyMultiple,(word)(r>>8),(word)(r&0xFF));
						goto wpmfree;
					}
					goto nextdopr;
				}
				else	
					goto wpminvtag;
			}
			else
			{
				*op++;
				vb->narray++;			//the count is zero based			
			}
		}
		else if(propid==DATE_LIST)
		{
			if(*op++!=0x2E) goto wpminvtag;	
			vb->pdtype=adtDateList;
			vb->narray=0;
nextcal:
			op=DecodeCalendarEntry(op,&vb->ps.pscal[vb->narray],&r);
			if(r==vsbe_invalidtag) 
				goto wpminvtag;
			else if (r!=vsbe_noerror)
			{
calerr:
				SendError(am,id,writePropertyMultiple,(word)(r>>8),(word)(r&0xFF));
				goto wpmfree;
			}
			if(*op!=0x2F)
			{
				if((*op==0x0C) || (*op==0x1E) || (*op==0x2B))		//or this begins another element
				{
					vb->narray++;
					if(vb->narray>=MAXCALENTRY) 
					{
						r=vsbe_resourcesother;
						goto calerr;
					}
					goto nextcal;
				}
				else	
					goto wpminvtag;				
			}
			else
			{
				*op++;
				vb->narray++;			//the count is zero based	
			}
		}
		else if((propid==ALARM_VALUES) || (propid==FAULT_VALUES))	
		{
			if((objtype!=MULTI_STATE_INPUT_OBJTYPE)&&					
				(objtype!=MULTI_STATE_VALUE_OBJTYPE)&&
				(objtype!=LIFE_SAFETY_POINT_OBJTYPE)&&
				(objtype!=LIFE_SAFETY_ZONE_OBJTYPE))
			{
				r=vsbe_unknownproperty;
				goto avfverr;								
			}	
			r=vsbe_noerror;
			op=dUnsignedList(op,vb,&r);
			if(r==vsbe_invalidtag) 
				goto wpminvtag;
			else if (r!=vsbe_noerror)
			{	
avfverr:
				SendError(am,id,writePropertyMultiple,(word)(r>>8),(word)(r&0xFF));
				goto wpmfree;														
			}
			if(*op==0x2F) *op++;		//increment to the x'1F' for WPM
		}
		else if ((propid == LOG_DEVICE_OBJECT_PROPERTY ) ||					//	***1223 Begin
                 (propid==EVENT_TIME_STAMPS)           ||
                 (propid==PRIORITY_ARRAY)              ||
                 (propid==LOG_BUFFER)                 ||
                 (propid==STATE_TEXT))              
		{
			r = vsbe_writeaccessdenied;
			goto firstfailed;         
		}																	//	***1223 End
		else if(*op++==0x2E)									//we found Value Open Tag
		{ 
			vb->pdtype=((*op)>>4)&0x0F;						//datatype
			r=dPRIMITIVE(&op,vb);		
			if (r==(vsbe_invalidtag))   
				goto wpminvtag;
			else if (r!=vsbe_noerror)
			{	
				SendError(am,id,writePropertyMultiple,(word)(r>>8),(word)(r&0xFF));
				goto wpmfree;								
			}											
			if(*op++!=0x2F) goto wpminvtag;
			if((*op&0xF0)==0x30)									//there's a priority 
				vb->priority=(byte)dDWORD(&op,false);				//decode priority
			else
				vb->priority=0;										//no priority
		}						
		else														//invalid tag
			goto wpminvtag;
		r=ReadWriteOneProperty(am,objid,propid,arrayindex,true,vb,&nextpid);
		if (r!=vsbe_noerror)
		{	

firstfailed:
			tp=RSVP(am,0);
			stp=ttp=tp;
			*tp++=(ERROR_PDU<<ASDUpdutypeshr)&ASDUpdutype;					//PDU header
			*tp++=id;										//invoke id
			*tp++=writePropertyMultiple;									//service
			*tp++=0x0E;										//[0]opening tag error
			tp=eENUM(tp,(dword)((r>>8)&0xFF));				// error class
			tp=eENUM(tp,(dword)(r&0xFF));					// error code
			*tp++=0x0F;										//[0]closing tag error
			*tp++=0x1E;										//[1] opening tag first failed write attempt
			tp=etagOBJECTID(tp,0x08,objid);					//[0]objectid
			tp=eDWORD(tp,propid,0x18,false);					//[1]propertyid
			if(arrayindex!=noindex) tp=eDWORD(tp,arrayindex,0x28,false);	//[2]arrayindex
			*tp++=0x1F;										//[1] closing tag first failed write attempt
			ttp=tp;											//sendit needs this!
			n=(word)(am->nlen+(ttp-stp));
			bytecpy(tp,stp,(word)(ttp-stp));
			transmit(am,n,am->npdu,macisResponse,false);
			goto wpmfree;
		}
		appliedatleastone=0x01;
		if((*op&0xF0)==0)										//multiple properties for this objid
		{	
			if ((word)(op-sop)>=dsize)
				goto wpminvtag;																
			r=vsbe_noerror;										//must reset the error code	
			goto nextprop;
		}
		if(*op++!=0x1F)											//[1]closing tag BACnetPropertyRef
		{
wpminvtag:
			if(appliedatleastone!=0x00) 
			{	
				r=vsbe_svcsinvalidtag;
				goto firstfailed;
			}
			SendReject(am,id,REJECT_InvalidTag);
			goto wpmfree;			
		}
	}
	SendSACK(am,id,writePropertyMultiple);
wpmfree:
	npoolFreePacket(am);	
}																	//				***1221 End
	

///////////////////////////////////////////////////////////////////////
//	Handler for AcknowledgeAlarm
//
//  in:	am		incoming packet	
//		op		points to the PDU
//		id		is the invoke id

static	void HandleAcknowledgeAlarm(frNpacket *am,octet *op,octet id)	//					***1205
{	
	static	frAckAlarm	alr;							//									***1200
	byte		t;
	word		yr;										//									***004
	dword		dw;										//									***1200x
 	int			r;

	if ((*op&0xF0)!=0) goto haainvtag;
	//alr.processid=dDWORD(&op,false);					//[0]acknowledgingProcessIdentifier	***1200x Begin
	if ((r=dDWORDEx(&op,false,&dw))!=vsbe_noerror)
	{
		SendError(am,id,acknowledgeAlarm,frGetErrorClass(r),frGetErrorCode(r));
		return;
	}
	alr.processid=dw;									//									***1200x End
	if ((*op&0xF0)!=0x10) goto haainvtag;
	alr.objid=dDWORD(&op,false);						//[1]eventObjectidentifier
	if ((*op&0xF0)!=0x20) goto haainvtag;
	alr.eventstate=dDWORD(&op,false);					//[2]eventStateAcknowledged
	if (*op++!=0x3E) goto haainvtag;
	t=*op++;											//[3]timeStamp
	switch (t&0xF0)
	{
	case 0x00:											//	[0]time
		op--;
		alr.timestamp.type=0;
		alr.timestamp.date.year=0xFF;
		alr.timestamp.date.month=0xFF;
		alr.timestamp.date.day=0xFF;
		alr.timestamp.date.dow=0xFF;
		dTIMESTRING (&op,&alr.timestamp.time.hour,
					&alr.timestamp.time.minute,
					&alr.timestamp.time.second,
					&alr.timestamp.time.hundredths);
		break;

	case 0x10:											// [1]sequence
		op--;
		alr.timestamp.type=1;
		alr.timestamp.sequence=(word)dUINT(&op);
		break;

	case 0x20:											//	[2]bacnetdatetime
		alr.timestamp.type=2;
		if ((*op&0xF0)!=0xA0) goto haainvtag;
		dDATESTRING (&op,&yr,
					&alr.timestamp.date.month,
					&alr.timestamp.date.day,
					&alr.timestamp.date.dow);
		alr.timestamp.date.year=(byte)(yr-1900);
		if ((*op&0xF0)!=0xB0) goto haainvtag;
		dTIMESTRING (&op,&alr.timestamp.time.hour,
					&alr.timestamp.time.minute,
					&alr.timestamp.time.second,
					&alr.timestamp.time.hundredths);
		if (*op++!=0x2F) goto haainvtag;
		break;
	default:
		SendReject(am,id,REJECT_InvalidParameterDataType);			
		return;
	}
	if (*op++!=0x3F) goto haainvtag;
	if ((*op&0xF0)!=0x40) goto haainvtag;
	buildfrString(&alr.whoackedstring,(octet *)alr.whoacked,true,
		          (byte)sizeof(alr.whoacked),(byte)sizeof(alr.whoacked));		//	***1205	
	r=(int)dCHARSTRING(&op,&alr.whoackedstring);		//[4]acknowledgementSource	***1220
	if((r!=vsbe_noerror) &&														//	***1221 Begin
	   (r!=vsbe_charactersetnotsupported))
	{
		SendError(am,id,acknowledgeAlarm,frGetErrorClass(r),frGetErrorCode(r));
		return;
	}																			//	***1221 End
	if (*op++!=0x5E) goto haainvtag;
	t=*op++;											//[5]timeOfAcknowledgement
	switch (t&0xF0)
	{
	case 0x00:											//	[0]time
		op--;
		alr.acktimestamp.type=0;
		alr.acktimestamp.date.year=0xFF;
		alr.acktimestamp.date.month=0xFF;
		alr.acktimestamp.date.day=0xFF;
		alr.acktimestamp.date.dow=0xFF;
		dTIMESTRING (&op,&alr.acktimestamp.time.hour,
					&alr.acktimestamp.time.minute,
					&alr.acktimestamp.time.second,
					&alr.acktimestamp.time.hundredths);
		break;
	case 0x10:											// [1]sequence
		op--;
		alr.acktimestamp.type=1;
		alr.acktimestamp.sequence=(word)dUINT(&op);
		break;
	case 0x20:											//	[2]bacnetdatetime
		if ((*op&0xF0)!=0xA0) goto haainvtag;
		alr.acktimestamp.type=2;
		dDATESTRING (&op,&yr,
					&alr.acktimestamp.date.month,
					&alr.acktimestamp.date.day,
					&alr.acktimestamp.date.dow);
		alr.acktimestamp.date.year=(byte)(yr-1900);
		if ((*op&0xF0)!=0xB0) goto haainvtag;
		dTIMESTRING (&op,&alr.acktimestamp.time.hour,
					&alr.acktimestamp.time.minute,
					&alr.acktimestamp.time.second,
					&alr.acktimestamp.time.hundredths);
		if (*op++!=0x2F) goto haainvtag;
		break;
	default:
		SendReject(am,id,REJECT_InvalidParameterDataType);			
		return;
	}
	if (*op++!=0x5F)
	{
haainvtag:	
		SendReject(am,id,REJECT_InvalidTag);
		return;
	}
	if ((r=fraAcknowledgeAlarm(&alr))==0) 
		SendSACK(am,id,acknowledgeAlarm);
	else
		SendError(am,id,acknowledgeAlarm,(word)(r>>8),(word)(r&0xFF));
}

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Handler for GetAlarmSummary
//
//  in:	am		incoming packet	
//		id		is the invoke id
//
//NOTE:	The GetAlarmSummary service was deprecated in early versions
//		of the standard and the GetEventInformation service should 
//		be used instead.
//		A potential issue with the use of GetAlarmSummary is that
//		the server can only return N alarms without any indication
//		that there are more alarms that may need to be sent.  

static	void HandleGetAlarmSummary(frNpacket *am,octet id)	//							***1205
{
word			i;
word			n;										//THIS DEPENDS ON THE MEDIA TYPE!!!
static frAlarmSummary as[1476/10];
frAlarmSummary	*asp=as;
bool			more;									// ignored in GetAlarmSummary
octet			*pp,*sp;
frNpacket* np=NULL;								//								***1208
frNpacket *nm=am;								//								***1208

	if(!fraCanDoAlarms())
	{
		SendReject(am,id,REJECT_UnrecognizedService);									//	***263
		return;
	}
	n=macMaxAPDU(nm->src.port)/10;					//how many we can hold
	fraGetAlarms(0,&n,asp,&more);					//how many do we have?				//	***408 moved here						
	if((n*10)>macMaxAPDU(nm->src.port))				//more than this port can TX?		//	***408 Begin
	{	
		SendAbort(nm,id,vsbe_segmentationnotsupported&0x00FF);		
		return;	
	}
	//the incoming frNpacket came from the small pool,									
	//but we need one from the big pool because
	//we have more alarms than we can fit in a small one
	if((!nm->nflag&Nisbig)&&((n*10)>smallalloc))				
	{
		np=npoolGetPacket(maxnl);							//we need a big packet for this
		if (np==NULL)
		{	
			SendAbort(nm,id,ABORT_OutOfResources);						//	***1223			
			return;									
		}
		frcpy(&np->src,&nm->src,sizeof(nm->src));	
		nm=np;										
	}																				
	sp=pp=RSVP(nm,0);										//				***408 End	
//Build complex ack response
	*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
	*pp++=id;												//invoke id
	*pp++=getAlarmSummary;									//service
	for (i=0;i<n;i++)										//if n==0, sequence is empty			
//NOTE: a sequence of sequence of primitive types
	{	pp=etagOBJECTID(pp,0xC0,asp->objid);				//objectIdentifier
		pp=eENUM(pp,(dword)asp->eventstate);				//alarmState
		pp=eBITSTRING(pp,&asp->transitionsacked,0x80,3);	//acknowledgedTransitions
		asp++;
	}	
	if(WouldOverflow(&nm->src,nm->npdu,pp,(word)(nm->nlen+(pp-sp))))				//		***408 Begin
	{
		SendAbort(nm,id,vsbe_segmentationnotsupported&0x00FF);
		goto exit;			
	}																						
	//macTransmit(nm->src.port,nm->src.sadr,(word)(nm->nlen+(pp-sp)),nm->npdu,macisResponse);	
	transmit(nm,(word)(nm->nlen+(pp-sp)),nm->npdu,macisResponse,false);				//	***1210
exit:
	if(np!=NULL)														
	{
#ifdef DEBUGVSBHP														
		sprintf(dbg,"%s (%d) calling npoolFreePacket\n",__FUNCTION__,__LINE__);
		fraLog(dbg);
#endif													
		npoolFreePacket(np);								
	}																					//	***408 End
}

///////////////////////////////////////////////////////////////////////							***240 Begin
//	Handler for GetEventInformation
//
//  in:	am		incoming packet	
//		op		points to the PDU
//		id		is the invoke id

//how many events (57 octets encoded length) can we get into
//one MS/TP frame (works out to 8)
#define MINEVENTSIZE (57)						//	***1224
#define MAXEVENTS	 8							//	***1224 (floor(MAXTXB/MINEVENTSIZE)
#define MAXTXB		 480					

static	void HandleGetEventInformation(frNpacket *am,octet *op,octet id)	//					***1205
{
word			i,j;									//										***1206
word			n,x;									//depends on media type and client		***405	
static frEventSummary es[MAXEVENTS];					//statically defined, for MS/TP only	***1205
static octet 	mstptxb[MAXTXB];						//statically defined, for MS/TP only	***1205
frEventSummary	*esp=NULL;
octet			*pp,*sp,*tp,*txb=NULL;
bool			more=false;								//in case we have none					***004
frNpacket* np=NULL;							//										***1208
frNpacket *nm=am;								//										***1208
	
	if(!fraCanDoEventInfo()) 
	{
		SendReject(nm,id,REJECT_UnrecognizedService);				//		***405 Begin
		return;
	}
	n=macMaxAPDU(nm->src.port);								//what we can hold					
	x=calcClientMaxAPDU(nm->src.maxresponse);				//what they can hold				
	if(n>x) n=x;											//use the min of the two					
	n/=MINEVENTSIZE;										//one event is 55 octets	
	//in ***1220 and below, we used to return segmentation-not-supported				//		***1221 Begin
	//here is n==0, but BTL 9.8.6 says we can return an empty list of event summaries
	//with more events==TRUE															//		***1221 End
	//the incoming frNpacket came from the small pool,									//		***408 Begin
	//but we need one from the big pool
	if((!nm->nflag&Nisbig)&&((n*MINEVENTSIZE)>smallalloc))				
	{
		np=npoolGetPacket(maxnl);							//we need a big packet for this
		if (np==NULL)
		{	
			SendAbort(nm,id,ABORT_OutOfResources);						//	***1223			
			return;										
		}
		frcpy(&np->src,&nm->src,sizeof(nm->src));	
		nm=np;										
	}																					//		***408 End
	//we do it this way because we don't want to put the requirement on the
	//application to search it's object list twice, so we take a best guess at
	//how many we can hold up front based on the minimum of what the port can handle
	//and what the client can handle.  If this comes out to more than we've statically
	//allocated (room for MAXEVENTS), then we need to dynamically allocate more room
	//if we took the approach of always being an B/IP or 8802-3 server, then we'd 
	//be using over 2Kbytes of stack space here between es and txb, and that could lead 
	//to stack overflows
	if(n<=MAXEVENTS)										//we can only hold enough for an MS/TP frame						
	{
		esp=es;												//point it to the statically allocated array
		txb=mstptxb;										//point it to the statically allocated buffer
	}
	else if(n>MAXEVENTS)									//we can only handle MAXEVENTS
	{
        SendError(nm,id,getEventInformation,frGetErrorClass(vsbe_resourcesother),           //  ***1202 Begin
                                            frGetErrorCode(vsbe_resourcesother));
        esp=NULL;
        txb=NULL;
        goto geifree;                                                                       //  ***1202 End
	}			
	tp=pp=txb;												//arrange the frame pointers	***405 End
	esp->objid=noobject;									//default start at beginning
	if(*op==0x0C)											//lastReceivedIdentifier
		esp->objid=dDWORD(&op,false);                       //decode object id				***1206
	fraGetAlarms(1,&n,esp,&more);							
//Build complex ack response
	*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
	*pp++=id;												//invoke id
	*pp++=getEventInformation;								//service
	*pp++=0x0E;												//[0] opening tag
	for (i=0;i<n;i++)										//if n==0, sequence is empty			
	//NOTE: a sequence of sequence of primitive types
	{	pp=etagOBJECTID(pp,0x08,esp->objid);				//[0]objectIdentifier
		sp=pp;
		pp=eENUM(pp,(dword)esp->eventstate);				//[1]eventState
		*sp=(*sp&0x7)+0x18;									//modify tag
		pp=eBITSTRING(pp,&esp->transitionsacked,0x28,3);	//[2]acknowledgedTransitions
		*pp++=0x3E;											//[3]opening tag for eventTimeStamps
		for (j=0;j<3;j++)
		{	switch(esp->timestamp[j].type)
			{
			case 0:
				sp=pp;														//[0] time
				pp=eTIMESTRING(pp,esp->timestamp[j].time.hour,esp->timestamp[j].time.minute,
								esp->timestamp[j].time.second,esp->timestamp[j].time.hundredths);
				*sp=0x0C;								//change tag
				break;
			case 1:															//[1] sequence
				pp=eDWORD(pp,esp->timestamp[j].sequence,0x18,false);
				break;
			case 2:
				*pp++=0x2E;													//[2] opening tag datetime
				pp=eDATESTRING(pp,(word)(esp->timestamp[j].date.year+1900),esp->timestamp[j].date.month,
								esp->timestamp[j].date.day,esp->timestamp[j].date.dow);
				pp=eTIMESTRING(pp,esp->timestamp[j].time.hour,esp->timestamp[j].time.minute,
								esp->timestamp[j].time.second,esp->timestamp[j].time.hundredths);
				*pp++=0x2F;													//[2] closing tag
			}
		}
		*pp++=0x3F;															//[3]closing tag
		sp=pp;
		pp=eENUM(pp,(dword)esp->notifytype);								//[4]notifyType
		*sp=(*sp&0x7)+0x48;													//modify tag
		pp=eBITSTRING(pp,&esp->transitionbits,0x58,3);						//[5]eventEnable
		*pp++=0x6E;															//[6]opening tag for eventPriorities
		for(j=0;j<3;j++)
			pp=eUINT(pp,esp->eventpriorities[j]);
		*pp++=0x6F;															//[6]
		esp++;
	}		
	*pp++=0x0F;																//[0] closing tag
	*pp++=0x19;																//[1]moreEvents
	*pp++=(more)?0x01:0x00;
	sp=tp;	
	tp=RSVP(nm,0);																					//	***408
	bytecpy(tp,sp,(word)(pp-sp));	
	//macTransmit(am->src.port,am->src.sadr,(word)(am->nlen+(pp-sp)),am->npdu,macisResponse);	//	***240 End
	//if(WouldOverflow(&nm->src,nm->npdu,pp,(word)(nm->nlen+(pp-sp))))							//					***408 Begin
	if(WouldOverflow(&nm->src,sp,pp,0))									//	***1223
	{
		SendAbort(nm,id,vsbe_segmentationnotsupported&0x00FF);
		goto geifree;			
	}		
	//macTransmit(nm->src.port,nm->src.sadr,(word)(nm->nlen+(pp-sp)),nm->npdu,macisResponse);		//	***240 End		***408	End
	transmit(nm,(word)(nm->nlen+(pp-sp)),nm->npdu,macisResponse,false);							//	***1210
geifree:																						//	***408 Begin
	//DO NOT FREE am here -- am is freed from the pool by the caller of this function!!!	
	if(np!=NULL)														
	{
#ifdef DEBUGVSBHP														
		sprintf(dbg,"%s (%d) calling npoolFreePacket\n",__FUNCTION__,__LINE__);
		fraLog(dbg);
#endif													
		npoolFreePacket(np);								
	}																							//	***408 End		
}


///////////////////////////////////////////////////////////////////////									***1205 Begin
//	Handler for CreateObject
//
//  in:	am->src		source address info
//		op			points to the PDU
//		id			is the invoke id
//
//CreateObject-Request ::= SEQUENCE {
//	objectSpecifier [0] CHOICE {											0E
//							objectType [0] BACnetObjectType,					09 tt			object type, or
//							objectIdentifier [1] BACnetObjectIdentifier			0E xx xx xx xx	objid
//								},											0F
//	listOfInitialValues [1] SEQUENCE OF BACnetPropertyValue OPTIONAL }		1E
//																				09 xx			propid
//																				2E primtag primval 2F
//																			... more properties
//																			1F

static	void HandleCreateObject(frNpacket *am,octet *op,octet id)
{
dword		objid,u;
octet		nc,tag,*pp=RSVP(am,0),*sp,room;					//									***1206
int			r,i;
static frCOprop	props[maxcoprops];
frCOprop	*pco;
byte		nprops=0;

	if(*op++!=0x0E)	goto hcoinvtag;							//must be open tag 0
	if(*op==0x09)											//its an object type
	{
		op++;												//skip tag
		objid=(((dword) *op++)<<22L)+0x3FFFFF;				//make wild instance for this type
	}
	else if(*op==0x0E)										//its an object id
		objid=dDWORD(&op,false);							//get the id
	else
		goto hcoinvtag;										//we expected object type or id
	if(*op++!=0x0F)	goto hcoinvtag;							//must be close tag 0
	if(*op++==0x1E)											//got properties
	{
		while(nprops<=maxcoprops)							//we can still handle another property
		{
			if(*op++!=0x09)	goto hcoinvtag;					//we only deal with propids<256
			pco=&props[nprops++];							//point to this property container
			pco->propid=*op++;								//save property ID
			if(*op++!=0x2E)	goto hcoinvtag;
			pco->pdtype=fraGetPropertyDatatype(objid,pco->propid); //expected datatype for this property
			tag=(*op>>4);									//get tag for datatype
			switch(pco->pdtype)	
			{
			case adtBoolean:
				if(tag!=adtBoolean) goto hcoinvtag;
				pco->iv.pb=(byte)dBOOL(&op);
				break;
			case adtUnsigned:
				if(tag!=adtUnsigned) goto hcoinvtag;
				pco->iv.pd=dDWORD(&op,false);
				break;
			case adtReal:
				if(tag!=adtReal) goto hcoinvtag;
				pco->iv.pf=dREAL(&op);
				break;
			case adtCharString:
				byteset((byte *)pco->iv.ps,0x00,maxcopropstring); //init string to all zero			***1206
				if(tag!=adtCharString) goto hcoinvtag;
				if((*op&7)<=4)
					nc=(*op++)&7;								//tag 6, length
				else
				{	
					op++;										//tag 6, extended length
					if(*op<=maxcopropstring)					//									***1206
						nc=*op++;
					else
						goto hcooor;							//string is too long for us
				}
				nc--;											//account for cs code	
				if(*op++!=chrsUTF8)
				{
					r=vsbe_charactersetnotsupported;
					goto hcoerr;
				}
				if(nc>(maxcopropstring-1))
				{
hcooor:				SendReject(am,id,REJECT_ParameterOutOfRange);
					return;
				}
				for(i=0;i<nc;i++)								//store all characters in order
						pco->iv.ps[i]=*op++;
				//don't need to set zero string end because we did byteset earlier
				break;
			case adtBitString:
				if(*op++!=0x82)	goto hcoinvtag;					//we only deal with bitstrings that have one octet
				op++;											//ignore unused bits octet
				pco->iv.pb=*op++;								//get the bitstring
				break;
			case adtEnumerated:
				if(tag!=adtEnumerated) goto hcoinvtag;
				pco->iv.pd=dDWORD(&op,false);
				break;
			case adtObjPropRef:
				//BACnetObjectPropertyReference ::= SEQUENCE {
				//objectIdentifier [0] BACnetObjectIdentifier,
				//propertyIdentifier [1] BACnetPropertyIdentifier,
				//propertyArrayIndex [2] Unsigned OPTIONAL -- used only with array datatype. If omitted with an array the entire array is referenced
				if(*op!=0x0C)	goto hcoinvtag;					//must be context tag 0
				pco->iv.popr.arrayindex=noindex;
				pco->iv.popr.listelement=0;
				pco->iv.popr.objid=dDWORD(&op,false);
				if((*op&0xF8)!=0x18) goto hcoinvtag;			//must be context tag 1
				pco->iv.popr.propid=dDWORD(&op,false);
				break;
			case adtStringArray:								//									***1206 Begin
				byteset((byte *)pco->iv.ps,0x00,maxcopropstring); //init string to all zero
				sp=(octet *)pco->iv.ps;							//point to where we can store strings
				room=maxcopropstring;							//how much room we have left
				while(*op!=0x2F)								//eat and append strings until end of PD tag
				{
					if((*op>>4)!=adtCharString) goto hcoinvtag;	//can only have strings in a string array
					if((*op&7)<=4)
						nc=(*op++)&7;								//tag 6, length
					else
					{	
						op++;										//tag 6, extended length
						if(*op<=maxcopropstring)
							nc=*op++;
						else
							goto hcooor;							//string is too long for us
					}
					if(room<=nc)									//no room left to append this string
							goto hcooor;							//string is too long for us
					nc--;											//account for cs code	
					if(*op++!=chrsUTF8)
					{
						r=vsbe_charactersetnotsupported;
						goto hcoerr;
					}
					if(room!=maxcopropstring)						//not the first string so stuff delimiter first
					{
						*sp++=stringarraydelimiter;
						room--;
					}
					for(i=0;i<nc;i++)								//store all characters in order
							*sp++=*op++;
				}
				//don't need to set zero string end because we did byteset earlier
				break;											//										***1206 End
			case adtUnsignedList:
				byteset(pco->iv.plou,0x00,maxcopropstring);		//init string to all zero
				nc=0;
				while(*op!=0x2F && nc<maxmsvstates)
				{
					u=dDWORD(&op,false);
					if(u==0 || u>maxmsvstates) goto hcooor;
					pco->iv.plou[nc++]=(byte)u;
				}
				break;
			default:
				SendReject(am,id,REJECT_InvalidParameterDataType);
				return;
			}
			if(*op++!=0x2F)	goto hcoinvtag;
			nprops++;											//did one more property
		}
	}
	if(*op++!=0x1F)	goto hcoinvtag;								//must be close tag 1
	if((r=fraCreateObject(objid,nprops,props))<0)				//failed
hcoerr: SendError(am,id,createObject,(word)(r>>8),(word)(r&0xFF));
	else
	{
//Succeeded, build complex ack response
		*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
		*pp++=id;												//invoke id
		*pp++=createObject;										//service
		pp=etagOBJECTID(pp,0xC0,objid);							//[0]objectid
		//macTransmit(am->src.port,am->src.sadr,(word)(pp-am->npdu),am->npdu,macisResponse);
		transmit(am,(word)(pp-am->npdu),am->npdu,macisResponse,false);			//		***1210
	}
	return;

hcoinvtag:	
	SendReject(am,id,REJECT_InvalidTag);
}																//						***1205 End

///////////////////////////////////////////////////////////////////////					***1205 Begin
//	Handler for DeleteObject
//
//  in:	am->src		source address info
//		op			points to the PDU
//		id			is the invoke id

static	void HandleDeleteObject(frNpacket *am,octet *op,octet id)
{	
int				r;
dword			dw;

	if (*op!=0xC4)											//must be an objectID
	{
		SendReject(am,id,REJECT_InvalidTag);
		return;
	}
	dw=dDWORD(&op,false);									//get the id
	if((r=fraDeleteObject(dw))==0) 
		SendSACK(am,id,deleteObject);
	else
		SendError(am,id,deleteObject,(word)(r>>8),(word)(r&0xFF));
	return;
}															//							***1205 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Handler for AtomicFileWrite
//
//  in:	am		incoming frNpacket
//		op		points to the PDU
//		id		is the invoke id

static	void HandleAtomicFileWrite(frNpacket *am,octet *op,octet id)	//				***1205
{
union { word w; byte b[2];} u;
octet *pp=RSVP(am,0);
dword	fid,start;
word	objtype,n;
int		r;													//							***1200x
long	i;													//							***1200x

	if(*op!=0xC4)											//BACnetObjectIdentifier
	{	SendReject(am,id,REJECT_InvalidTag);				//							***200
		return;			
	}
	fid=dOBJECTID(&op,&objtype);							//decode object id
	if(objtype!=FILE_OBJTYPE)
	{	r=vsbe_inconsistentparameters;
		goto hafwerr;
	}
	if(*op++!=0x0E)											//[0]streamAccess
	{	r=vsbe_invalidaccessmethod;
		goto hafwerr;
	}
//	start=(dword)dINT(&op);									//fileStartPosition			***1200x Begin
	if ((r=dINTEx(&op,&i))!=vsbe_noerror)
		goto hafwerr;
	start=(dword)i;											//							***1200x End
	if((*op&7)<=4)
		n=(*op++)&7;										//tag 6, length
	else
	{	op++;												//tag 6, extended length
		if(*op<=253)
			n=*op++;
		else												//assume n<=65535
		{	op++;
#if bigEndian												//							***240 Begin
			u.b[0]=(byte)*op++;								//msb of length first
			u.b[1]=(byte)*op++;  
#else
			u.b[1]=(byte)*op++;								//msb of length first
			u.b[0]=(byte)*op++;  
#endif														 //							***240 End
			n=u.w;
		}	
	}
	if((r=fraAtomicWriteFile(fid,&start,n,op))!=0)
	{
//r has error class/code in it here
hafwerr:	SendError(am,id,atomicFileWrite,(word)(r>>8),(word)(r&0xFF));	//		***200
			return;
	}

//Succeeded, build complex ack response
	*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
	*pp++=id;												//invoke id
	*pp++=atomicFileWrite;									//service
	pp=eDWORD(pp,start,0x08,true);							//[0]fileStartPosition
	//macTransmit(am->src.port,am->src.sadr,(word)(pp-am->npdu),am->npdu,macisResponse);	//	***240 End
	transmit(am,(word)(pp-am->npdu),am->npdu,macisResponse,false);				//	***1210
}

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Handler for AtomicFileRead
//
//  in:	am		incoming frNpacket
//		op		points to the PDU
//		id		is the invoke id

static	void HandleAtomicFileRead(frNpacket *am,octet *op,octet id)	//					***1205
{
union { dword d; word w; byte b[4];} u;						//							***1205
octet		*pp;											//							***408
octet 		*pos,*peof;										//							***200
dword		fid,start;
word		i,objtype,clientroom,used;						//							***1206
bool		eof;
int			r;
frNpacket	*bm=NULL;								//							***1209
frNpacket	*nm=am;									//							***1208

	//am was allocated from the small pool, but
	//in order to fit all of dsize, we need a buffer
	//from the large pool
	if((am->nflag&Nisbig)==0)								//it was small				***1211
	{
		bm=npoolGetPacket(maxmac);							//							***1209		
		if (bm==NULL)										//							***1209
		{	
			SendAbort(nm,id,ABORT_OutOfResources);									//	***1223			
			return;											//							***1209
		}
		frcpy(&bm->src,&am->src,sizeof(am->src));			//							***1209			
		nm=bm;												//							***1209
	}
	pp=RSVP(nm,0);											//point to buffer start		***1209

	if(*op!=0xC4)											//BACnetObjectIdentifier
	{	
		SendReject(nm,id,REJECT_InvalidTag);				//							***408
		goto rffree;
	}
	fid=dOBJECTID(&op,&objtype);							//decode object id
	if(objtype!=FILE_OBJTYPE)
	{	r=vsbe_inconsistentparameters;
		goto hafrerr;
	}
	if(*op++!=0x0E)											//[0]streamAccess
	{	r=vsbe_invalidaccessmethod;
		goto hafrerr;
	}
	if ((r=dINTEx(&op, (long *)&start))!=vsbe_noerror)		//fileStartPosition			***1205
		goto hafrerr;
	if ((r=dUINTEx(&op, &u.d))!=vsbe_noerror)				//requestedOctetCount
		goto hafrerr;										//							***1200x End
	if(u.d>(word)(macMaxRPDU(nm->src.port)-15))				//max the port could handle	***408
	{
hafnoseg:
		SendAbort(nm,id,vsbe_segmentationnotsupported&0x00FF);	//						***408
		goto rffree;									//								***408
	}
//Assume it will succeed, build complex ack response
	*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
	*pp++=id;												//invoke id
	*pp++=atomicFileRead;									//service
	peof=pp;
	pp=eBOOL(pp,false);										//place holder for endOfFile	***003
	*pp++=0x0E;												//[0]streamAccess
	pp=eINT(pp,(long)start);								//fileStartPosition
	pos=pp;													//remember where encoded tag is
	*pp++=0x65;												//octet string, 2 byte length
	*pp++=254;												//length is extended
	*pp++=0;												//set length to zero initially
	*pp++=0;
	u.w = (word)u.d;										// Fix to get correct size	***1206
	if((r=fraAtomicReadFile(fid,start,&u.w,pp,&eof))!=0)
	{
//r has error class/code in it here
hafrerr:	SendError(nm,id,atomicFileRead,(word)(r>>8),(word)(r&0xFF));	//		***200	***408
			goto rffree;						//											***408
	}
//Succeeded
	if (u.w < 254)												//buffer will need to be squished
	{
		pp = &pos[4];											//point to where bytes are
#if bigEndian												//								***230 Begin	***1224 Begin
		if (u.w < 5)
			*pos++=0x60+u.b[1];
		else													//5..253
		{
			pos++;												//leave tag as 0x65											   
			*pos++=u.b[1];						            //save new length, pos points to dest
		}														//											***1224 End
#else
		if (u.w < 5)											//really small
			*pos++=0x60+u.b[0];								//reencode tag, leave pos pointing to dest
		else												//5..253
		{
			pos++;											//leave tag as 0x65
			*pos++=u.b[0];									//save new length, pos points to dest
		}
#endif														 //								***230 End
		for (i=0;i<u.w; i++) *pos++ = *pp++;				//squish buffer
		pp = pos;											//leave pp pointing past bytes
	}
	else													//just update buffer length
	{
#if bigEndian												//								***230 Begin
		pos[2]=u.b[0];
		pos[3]=u.b[1];
#else
		pos[2]=u.b[1];
		pos[3]=u.b[0];
#endif														 //								***230 End
		pp=&pos[4]+u.w;										//point past data				***211
	}
	*pp++=0x0F;												//close tag
	eBOOL(peof,eof);										//real endOfFile

	used=(word)(pp-nm->npdu);								//how much have we used?		***1206 Begin
	clientroom=calcClientMaxAPDU(nm->src.maxresponse);		//what did the client say they could take?			
	if(clientroom<used) goto hafnoseg;						//can't do it					***1206 End

	//macTransmit(nm->src.port,nm->src.sadr,used,nm->npdu,macisResponse);	//					***1209
	transmit(nm,(word)(pp-nm->npdu),nm->npdu,macisResponse,false);			//					***1210
rffree:																					//	***408 Begin
	//DO NOT FREE am here -- am is freed from the pool by the caller of this function!!!	
	if(bm!=NULL)											//								***1209														
	{
#ifdef DEBUGVSBHP														
		sprintf(dbg,"%s (%d) calling npoolFreePacket\n",__FUNCTION__,__LINE__);
		fraLog(dbg);
#endif													
		npoolFreePacket(bm);								//								***1209
	}																					//	***408 End
}

///////////////////////////////////////////////////////////////////////					***1227 Begin
//	Handle ReinitializeDevice service
//in:	am				incoming packet
//		rap				location in APDU
//		rr				ptr to reject code, if any
//		id				invoke id
//		goes_to_rrej	should we jump to SendReject upon return?
//out:
//		true if it was handled here
//		false otherwise

static bool HandleReinitDevice(frNpacket* am, octet** rap, octet* rr, octet id, bool* goes_to_hcrrej)
{
	dword dw;
	int r;
	frString sp;
	octet pass[20];
	bool resetComm = false;
	bool retcode= true;
	*goes_to_hcrrej = false;								//	***1228
	*rap += 4;												//point past Service Request
	*rr = REJECT_InvalidTag;
	if ((**rap & 0xF0) != 0)
	{
		*goes_to_hcrrej = true;
		return false;
	}
	
	dw = dENUM(&*rap);										//[0]reinitializedStateOfDevice
	if (dw <= vsb_abortrestore)
	{
		if ((dw == vsb_warmstart)||(dw==vsb_coldstart))
		{
			if (commEnabled == false)
			{
				commEnabled = true;		//we MUST reply for warmstart & coldstart, so reenabled
				resetComm = true;		//but indicate that it must be disabled when we're done
			}
		}
		else 				
		{	//all others
			if (commEnabled == false)
			{
				commEnabled = true;		//we must send the error, so enable temporarily
				resetComm = true;		//but indicate that it must be disabled when we're done
				SendError(am, id, reinitializeDevice,
					frGetErrorClass(vsbe_servicecommunicationdisabled),
					frGetErrorCode(vsbe_servicecommunicationdisabled));	// services, communication-disabled
				if((commEnabled==true)&&(resetComm==true))
					commEnabled=false;
				return true;
			}
		}		
	}
	else if (dw>vsb_abortrestore)
	{
		*rr = REJECT_ParameterOutOfRange;
		*goes_to_hcrrej = true;
		return false;
	} 
	if ((**rap & 0xF0)==0x10)												//[1]password
	{
		buildfrString(&sp, pass, false, sizeof(pass), sizeof(pass));
		r = dCHARSTRING(&*rap, &sp);
		if (r != vsbe_noerror)
		{
			SendError(am, id, reinitializeDevice,
			          frGetErrorClass(vsbe_passwordfailure),
			          frGetErrorCode(vsbe_passwordfailure));				//security,password failure		
			return true;
		}
		else
			r = fraReinitDevice((int)dw, &sp);
	}
	else
		r = fraReinitDevice((int)dw, NULL);
	if (r==0)																//succeeded
	{
		SendSACK(am, id, reinitializeDevice);
		retcode=true;
	}		
	else
	{
		if (r==-1)															//bad password
		{
			SendError(am, id, reinitializeDevice,
			          frGetErrorClass(vsbe_passwordfailure),
			          frGetErrorCode(vsbe_passwordfailure));				//security,password failure	
			retcode=true;
		}
		else if (r==2)
		{
			SendError(am, id, reinitializeDevice,
			          frGetErrorClass(vsbe_configurationinprogress),
			          frGetErrorCode(vsbe_configurationinprogress));		//devices,configuration in progress
			retcode=true;
		}
		else																//can't do it
		{
			SendError(am, id, reinitializeDevice,
			          frGetErrorClass(vsbe_servicerequestdenied),
			          frGetErrorCode(vsbe_servicerequestdenied));			//services,service request denied
			retcode=true;
		}
	}
	if((commEnabled==true)&&(resetComm==true))
	{
		commEnabled=false;
	}		
	return retcode;
}														//								***1227 End

///////////////////////////////////////////////////////////////////////					***1227 Begin
//	Handle ConfirmedPrivateTransfer service
//in:	am				incoming packet
//		rap				location in APDU
//		rr				ptr to reject code, if any
//		id				invoke id
//		goes_to_rrej	should we jump to SendReject upon return?
//out:
//		true if it was handled here
//		false otherwise

static bool HandledConfirmedPvtTransfer(frNpacket* am, octet** rap, octet* rr, octet id, bool* goes_to_hcrrej)
{
	static frVbag vb;
	octet		*pp,*rsp;
	word		vendorid,svcnum,n;
	int r;
	frNpacket	*nm=am,*np=NULL;
	*goes_to_hcrrej=false;							//	***1228
	rsp=*rap;
	*rap+=4;
	if ((**rap&0xF0)!=0)
	{
hcrit:		*rr=REJECT_InvalidTag;
		*goes_to_hcrrej=true;
		return false;
	}			
	vendorid=(word)dCONTEXT(&*rap,false);			//[0]vendorID
	if (vendorid!=di.vendor_id) goto hcrit;
	if ((**rap&0xF0)!=0x10) goto hcrit;				//better be for us	
	svcnum=(word)dCONTEXT(&*rap,false);				//[1]serviceNumber
	if (**rap==0x2E)									//[2]optional serviceParameters
	{
		*rap++;
		if ((dword)(*rap-rsp)>=am->nlen) goto hcrit;
		if (rsp[am->nlen-1]!=0x2F) goto hcrit;
		n=(word)(am->nlen-(*rap-rsp)-1);
	}
	else
		n=(word)(am->nlen-(*rap-rsp));
	fraConfirmedPrivateTransfer(&am->src,svcnum,*rap,n,&vb);
	if(vb.pdtype==adtError)							//failed
	{
		r=vb.pd.errval;
		if((r&0xFF00)==abortreason)
			SendAbort(am,id,(octet)(r&0xFF));
		else if((r&0xFF00)==rejectreason)
			SendReject(am,id,(octet)(r&0xFF));
		else
			SendError(am,id,confirmedPrivateTransfer,(word)(r>>8),(word)(r&0xFF));
	}
	else
	{
		if(vb.ps.pscpt.blocklen==0)					//only need a small response
			pp=RSVP(am,0);
		else										//need a big packet
		{
			np=npoolGetPacket(maxnl);
			if (np==NULL)
			{
				SendAbort(am,id,ABORT_OutOfResources);			//	***1223
				goto hcrfp;
			}
			else
			{
				frcpy(&np->src,&am->src,sizeof(am->src));
				nm=np;
				pp=RSVP(np,0);						//point to buffer start
			}
		}
		*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
		*pp++=id;									//invoke id
		*pp++=confirmedPrivateTransfer;				//service
		pp=eDWORD(pp,vb.ps.pscpt.vendorid,0x08,false);	//[0]vendorid
		pp=eDWORD(pp,vb.ps.pscpt.service,0x18,false);	//[1]servicenumber
		if(vb.ps.pscpt.blocklen>0)
		{
			*pp++=0x2E;
			bytecpy(pp,vb.ps.pscpt.block,vb.ps.pscpt.blocklen);
			pp+=vb.ps.pscpt.blocklen;
			*pp++=0x2F;
		}		
		transmit(nm,(word)(pp-nm->npdu),nm->npdu,macisResponse,false);		//		***1210
		if(np!=NULL)								//we had to get a big packet													
			npoolFreePacket(np);								
	}
hcrfp:	npoolFreePacket(am);
	return true;
}

///////////////////////////////////////////////////////////////////////					***1227 Begin
//	Handle DeviceCommunicationControl service
//in:	am				incoming packet
//		rap				location in APDU
//		rr				ptr to reject code, if any
//		id				invoke id
//		goes_to_rrej	should we jump to SendReject upon return?
//out:
//		true if it was handled here
//		false otherwise

static bool HandleDCC(frNpacket* am, octet** rap, octet* rr, octet id, bool* goes_to_hcrrej)
{
	dword dw;
	int r;
	long timer;
	frString sp;
	octet pass[20];
	dword mode;
	*goes_to_hcrrej = false;							//						***1228
	*rap+=4;									     // point past Service Request
	*rr = REJECT_InvalidTag;
	if ((**rap & 0xF0) != 0)						//	[0] timer				***1228 Begin
	{		
		timer=-1;									//time parameter is absent	
	}
	else
	{
		if (dDWORDEx(&*rap,false,&dw)!=vsbe_noerror)
		{	
dccvor:		SendError(am,id,deviceCommunicationControl,
				frGetErrorClass(vsbe_valueoutofrange),
				frGetErrorCode(vsbe_valueoutofrange));	
			return true;
		}
		if (dw>(0xFFFFFFFF/60000L))
			goto dccvor;
		timer=(long)(dw*60000L);	
	}																		//	***1228 End
	if((**rap&0xF0)==0x10)	
		mode=dENUM(&*rap);							//	[1] mode
	else
	{
		*rr=REJECT_InvalidTag;						//							***1205									
		*goes_to_hcrrej = true;
		return false;								//							***1205	
	}																		
	if((mode!=enable)  &&													
		(mode!=disable) &&
		(mode!=disable_initiation))		//unknown mode, reject it
	{
		*rr=REJECT_UndefinedEnumeration;				//						***1205				
		*goes_to_hcrrej = true;
		return false;								//							***1205	
	}
	if((mode!=enable)&&(timer==-1))						//						***1228 Begin
	{
		SendError(am,id,deviceCommunicationControl,
			          frGetErrorClass(vsbe_optionalfunctionalitynotsupported),
			          frGetErrorCode(vsbe_optionalfunctionalitynotsupported));	
			return true;		
	}													//						***1228 End
	buildfrString(&sp,pass,false,sizeof(pass),sizeof(pass));				//	***1200		
	if (((**rap&0xF0)==0x20)&&(am->nlen>(word)(*rap-(octet *)am->apdu)))	// [2] password			
	{
		r=(int)dCHARSTRING(&*rap,&sp);				//get 20 bytes of string chars
		if(r!=vsbe_noerror)							//any failure causes an error-PDU
		{				
			SendError(am,id,deviceCommunicationControl,
			          frGetErrorClass(vsbe_passwordfailure),
			          frGetErrorCode(vsbe_passwordfailure));	
			return true;				
		}
	}
	else                                                                    //	***1227 Begin
	{
		sp.len=0;															//no password was provided
		if(di.password->len!=0)												//but our application requires one!
		{
			goto dccpwf;
		}
	}			                                                            //	***1227 End
        if ((sp.len==0&&di.password->len==0) ||                                 //both passwords are empty -OR-
            ((sp.len!=0&&di.password->len!=0)       &&                              //Neither password is empty -AND-
            (sp.len==(dword)(di.password->len)  &&                              //the password lengths are equal -AND-
            (_strnicmp(sp.text,di.password->text,(size_t)sp.len)==0))       //the password strings are equivalent
	))
	{							
		if(mode==enable)				  //0
		{				
			commEnableTimer=0;
			commEnabled=true;
			initiationDisabled=false;
			SendSACK(am,id,deviceCommunicationControl);		
		}
		else if(mode==disable)			  //1
		{
			commEnabled=true;				//it may have already been false, but we need to ACK it
			SendSACK(am,id,deviceCommunicationControl);	//SendSACK doesn't work when commEnabled==false
			commEnableTimer=timer;
			commEnabled=false;
			initiationDisabled=true;	  //disable ALL, even initiation
		}
		else if(mode==disable_initiation) //2
		{
			commEnabled=true;													
			SendSACK(am,id,deviceCommunicationControl);		
			commEnableTimer=timer;
			initiationDisabled=true;	  //disable initiation											
		}				
	}																				//											
	else if(sp.len!=0)																//we got a password, but it wasn't valid	***1200
	{
dccpwf:	if(commEnabled==false)	//temporarily set to true and send the error
		{
			commEnabled=true;
			SendError(am,id,deviceCommunicationControl,
			          frGetErrorClass(vsbe_passwordfailure),
			          frGetErrorCode(vsbe_passwordfailure));			//security,password failure //	***1200
			commEnabled=false;
		}
		else					//otherwise just send it
		{
			SendError(am,id,deviceCommunicationControl,
			          frGetErrorClass(vsbe_passwordfailure),
			          frGetErrorCode(vsbe_passwordfailure));			//security,password failure		***1200
		}
	}																				//					***404 End
	return true;
}

///////////////////////////////////////////////////////////////////////
//	Handler for SubscribeCOV
//
//  in:	am->src		source address info
//		op			points to the PDU
//		dsize		is the number of octets to decode
//		id			is the invoke id

static	void HandleSubscribeCOV(frNpacket *am,octet *op,word dsize,octet id)
{	
	static frSubscribeCOV	scov;							//	***414
	int				r;
	octet           *sop=op;								//							***263
	static frVbag			vb;								//							***263 ***414
	dword			dw;										//							***1200x
	buildvbString(&vb);										//							***1205
	if ((*op&0xF0)!=0) goto hscinvtag;						//[0] Subscriber Process Identifier
	if ((r=dDWORDEx(&op,false,&dw))!=vsbe_noerror)
	{
vor:	SendError(am,id,subscribeCOV,frGetErrorClass(r),frGetErrorCode(r));
		return;
	}
	scov.processid=(dword)dw;								//													***1202
	if ((*op&0xF0)!=0x10) goto hscinvtag;
	scov.objid=dDWORD(&op,false);							//[1] Monitored Object identifier
	if (((word)(op-sop)<dsize) &&							//we have another tag	***263
		(*op&0xF0)==0x20)									//[2] Issue Confirmed Notifications
	{	
		op++;
		scov.cancel=false;
		scov.confirmed=dBOOL(&op);
		if ((*op&0xF0)!=0x30)        // Fixed DH1-184
		{
			scov.time=0;
		}
		else
		{
			if ((r=dDWORDEx(&op,false,&dw))!=vsbe_noerror)    //[3]lifetime
				goto vor;
			scov.time=dw;
		}
/*		if(scov.time==0)									//	don't allow 0 lifetime ***263 Begin
		{
			r=vsbe_valueoutofrange;
			goto coverr;
		}	*/												//							***263 End
	}
	else
		scov.cancel=true;
	bytecpy((byte *)&scov.src,(byte *)&am->src,sizeof(scov.src));			//			***1210
	scov.numberpvs=0;											//						***263
	if ((r=fraSubscribeCOV(&scov))==0) 
	{
		SendSACK(am,id,subscribeCOV);
		if(scov.numberpvs!=0)		//wasn't filled in, just ACK						***263
			frcCOVNotify(&scov,&vb);						//							***263
	}
	else
	{
//coverr:														//							***263
		SendError(am,id,subscribeCOV,(word)(r>>8),(word)(r&0xFF));
	}
	return;
hscinvtag:	
	SendReject(am,id,REJECT_InvalidTag);
}															//							***210 End

///////////////////////////////////////////////////////////////////////					***1223 Begin
//	Handler for SubscribeCOVProperty
//
//  in:	am->src		source address info
//		op			points to the PDU
//		dsize		is the number of octets to decode
//		id			is the invoke id

static	void HandleSubscribeCOVProperty(frNpacket *am,octet *op,word dsize,octet id)
{	
	static frSubscribeCOVProperty	scov;	
	static frSubscribeCOV			cov;
	int				r;
	octet           *sop=op;								
	static frVbag			vb;								
	dword			dw;										
	buildvbString(&vb);										
	if ((*op&0xF0)!=0) goto hscinvtag;
	if ((r=dDWORDEx(&op,false,&dw))!=vsbe_noerror)			//[0] Subscriber Process Identifier
	{
vor:	SendError(am,id,subscribeCOV,frGetErrorClass(r),frGetErrorCode(r));
		return;
	}														
	scov.processid=(dword)dw;								
	if ((*op&0xF0)!=0x10) goto hscinvtag;
	scov.objid=dDWORD(&op,false);							//[1] Monitored Object Identifier
	if (((word)(op-sop)<dsize) &&							//we have another tag
		(*op&0xF0)==0x20)									//[2] Issue Confirmed Notifications
	{	
		op++;
		scov.cancel=false;
		scov.confirmed=dBOOL(&op);
		if ((*op&0xF0)!=0x30) goto hscinvtag;		
		if ((r=dDWORDEx(&op,false,&dw))!=vsbe_noerror)		//[3] Lifetime
			goto vor;
		scov.time=dw;
		if(scov.time==0)									// a value of zero shall not be allowed
		{
			r=vsbe_valueoutofrange;
			goto covperr;
		}	
		if ((*op++&0xFE)==0x4E)								//[4] Monitored Property Identifier
		{
			scov.propval[0].propid=dDWORD(&op,false);			
			op++;
			scov.hasincrement=false;
			if((*op&0xF8)==0x58)							//[5] COV Increment		
			{				
				scov.hasincrement=true;
				scov.cov_increment=dREAL(&op);
			}
		}
	}
	else
		scov.cancel=true;
	bytecpy((byte *)&scov.src,(byte *)&am->src,sizeof(scov.src));	
	scov.numberpvs=0;
	if ((r=fraSubscribeCOVProperty(&scov))==0) 
	{
		SendSACK(am,id,subscribeCOVProperty);
		if(scov.numberpvs!=0)					//wasn't filled in, just ACK
		{			
			cov.choice=scov.choice;
			cov.devid=scov.devid;			
			bytecpy((byte *)&cov.src,(byte *)&scov.src,sizeof(cov.src));
			cov.processid=scov.processid;
			cov.deviceid=scov.deviceid;
			cov.objid=scov.objid;
			cov.time=scov.time;
			cov.cancel=scov.cancel;
			cov.confirmed=scov.confirmed;
			cov.numberpvs=scov.numberpvs;
			bytecpy((byte*)&cov.propval,(byte*)&scov.propval,(sizeof(frPropVal)*scov.numberpvs));
			frcCOVNotify(&cov,&vb);
		}
	}
	else
	{
covperr:
		SendError(am,id,subscribeCOVProperty,(word)(r>>8),(word)(r&0xFF));
	}
	return;
hscinvtag:	
	SendReject(am,id,REJECT_InvalidTag);
}															//							***1223 End
///////////////////////////////////////////////////////////////////////					***240 Begin
//  Send a ranged Who-Is
//
//in:	dev		the device to send it to
//out:	true	we sent it

static	bool SendWhoIs(dword dev)
{	octet		*tp;
	frNpacket	*np;
#ifdef DEBUGVSBHP
	char		b[32];
#endif

	if(commEnabled==false)
		return false;										//							***010
	if(initiationDisabled) return false;					//	***400
#ifdef useMSTP
	if((portid==portMSTP) && (di.maxmaster==0)) return false;			//we're in slave mode	***1223
#endif
	if((np=UnconfirmedRequest(BroadcastDNET,whoIs,false))==NULL)	//try for a packet	***1205
		return false;										//can't get one right now
#ifdef DEBUGVSBHP
	sprintf(b,"\r\nSendWhoIs %lu,%lu",dev,dev);
	fraLog(b);
#endif
	tp=np->apdu;
	tp=eDWORD(tp,dev,0x08,false);
	tp=eDWORD(tp,dev,0x18,false);
	np->nlen+=(word)(tp-np->apdu);							//add in stuff after header
	np->src.snet.w=0;										//force broadcast			***1208
	np->src.slen=0;											//							***1208
	UnconfirmedSend(np); 									//broadcast it and release it
	return true;											//							***240 End
}

///////////////////////////////////////////////////////////////////////					***1224 Begin
//	Transmit a ConfirmedPrivateTransfer or UnconfirmedPrivateTransfer packet
//
//in:	device	the device instance to transfer to (if nodevice then src has frSource)
//		xsrc	the alternative pointer to an frSource (may be NULL if device !=nodevice)
//		vb		points to a frVbag with transfer parameters and to receive result if confirmed
//		cnf		true for ConfirmedPrivateTransfer, false for UnconfirmedPrivateTransfer
//out:	0		if success
//		else	error 
//				NOTE: vsbe_resourcesother		indicates tx buffer is not free
//				      vsbe_unknowndevice		indicates device has not yet sent an Iam
//					  vsbe_parameteroutofrange	indicate parameter block is too long

int SendPrivateTransfer(dword device, frSource *xsrc, frVbag *vb, bool cnf)
{
	frNpacket	*np;
	octet		*tp;
	int			r;
	word		dvx;
	frSource	*src;
	if (commEnabled==false)
		return vsbe_servicerequestdenied;			//can't transmit right now
	if (initiationDisabled) return vsbe_initiationdisabled;
#ifdef useMSTP
	if ((portid==portMSTP) && (di.maxmaster==0)) return vsbe_slavemodeenabled;
#endif

	if (vb->ps.pscpt.blocklen > maxcptblocklen)
		return vsbe_parameteroutofrange;			//can't fit this parameter block
	if ((device==nodevice) && (xsrc==NULL))
	{
		if ((np = UnconfirmedRequest(BroadcastDNET, (octet)unconfirmedPrivateTransfer, true))==NULL) //try for a packet
			return vsbe_outofresources;									//unknown device or can't get one right now
	}
	else
	{
		if (device==nodevice)
		{
			if (xsrc==NULL) return vsbe_unknowndevice;
			src = xsrc;
			if (cnf)
			{
				if ((np = ConfirmedRequestMAC(src, (octet)confirmedPrivateTransfer, true, &r, vb))==NULL) //try for a packet
					return r;
			}
			else
			{
				if ((np = UnconfirmedRequestMAC(src, (octet)unconfirmedPrivateTransfer, true))==NULL) //try for a packet
					return vsbe_outofresources;
			}
		}
		else
		{
			if (cnf)
			{
				if ((np = ConfirmedRequest(device, (octet)confirmedPrivateTransfer, true, &r, vb))==NULL) //try for a packet
					return r;									//unknown device or can't get one right now
			}
			else
			{
				if ((dvx = SearchForDevice(device))==notheardfrom)	//need to lookup the device's network number, 0 means local.
					return vsbe_unknowndevice;
				src = &devlist[dvx].src;
				if ((np = UnconfirmedRequest(src->snet.w, (octet)unconfirmedPrivateTransfer, true))==NULL) //try for a packet
					return vsbe_outofresources;									//unknown device or can't get one right now
			}
		}
	}
	tp = np->apdu;
	vb->pdtype = adtPrivateTransfer;
	tp = eDWORD(tp, vb->ps.pscpt.vendorid, 0x08, false);				// [0] vendorID
	tp = eDWORD(tp, vb->ps.pscpt.service, 0x18, false);					// [1] serviceNumber
	if (vb->ps.pscpt.blocklen)											// got a block to send also
	{
		*tp++ = 0x2E;													// [2] serviceParameters
		bytecpy((byte*)tp, vb->ps.pscpt.block, vb->ps.pscpt.blocklen);	//copy block
		tp += vb->ps.pscpt.blocklen;
		*tp++ = 0x2F;
	}
	if (cnf) vb->status = vbsPending;
	np->nlen += (word)(tp - np->apdu);					//add in stuff after header
	if (cnf)
		ConfirmedSend(np); 								//send it
	else
		UnconfirmedSend(np);
	return 0;
}																				//		***1224 End

///////////////////////////////////////////////////////////////////////					***243 Begin
//  Send a ComplexACK packet for a readProperty  
//     
//in:	am			incoming packet
//		id			the invokeid
//		objid		the object id
//		propid		the property id
//		arrayindex	the array index
//		vb			pointer to a value bag

static	void SendCACK(frNpacket *am,octet id,dword objid,dword propid,dword arrayindex,frVbag *vb)
{	
octet *pp;		
frNpacket *np=NULL;												//								***1208
	word overhead;
frNpacket *nm=am;												//								***1208
	bool	isarray=false;										//								***400 Begin
	bool	islist=false;
	word i=0;													//								***400 End

	if(arrayindex!=noindex)										//								***402 Begin
		isarray=true;
	else						//could be the whole array being returned, so check
	{
		while(listofarrays[i]!=0xFFFFFFFF)
		{
			if(propid==listofarrays[i++]) 
			{
				isarray=true;
				break;
			}			
		}
		if(false==isarray)		//cannot be both an array AND a list, so avoid the check			
		{
			i=0;													
			while(listoflists[i]!=0xFFFFFFFF)
			{
				if(propid==listoflists[i++])					//								***412
				{
					islist=true;
					break;
				}				
			}
		}															
	}															//								***402 End		
	if(commEnabled==false)	goto scfree;						//								***256	
	if((false==isarray)	&&										//does it need a big packet?
	   (false==islist)  &&										//								***402
	   (vb->pdtype!=adtCharString))								//								***400
	{
		pp=RSVP(am,0);
	}
	else	
	{	
		np=npoolGetPacket(maxnl);								//we need a big packet for this
		if (np==NULL)
		{	
			SendAbort(nm,id,ABORT_OutOfResources);						//						***1223			
			goto scfree;										//								***256			
		}		
		frcpy(&np->src,&am->src,sizeof(am->src));			//									***255		
		nm=np;												//									***400
		pp=RSVP(np,0);										//point to buffer start
	}														//									***252 End														
	*pp++=(COMPLEX_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
	*pp++=id;												//invoke id
	*pp++=readProperty;										//service
	pp=etagOBJECTID(pp,0x08,objid);							//[0]objectid
	pp=eDWORD(pp,propid,0x18,false);						//[1]propertyid
	if(arrayindex!=noindex)
		pp=eDWORD(pp,arrayindex,0x28,false);				//[2]arrayindex
	*pp++=0x3E;												//context open tag 3	
	overhead=(word)((pp-nm->npdu)+1);                       //amount used incl 3F				***400
	if(EncodePropertyValue(&pp,vb,(word)(npoolNPDUsize(nm)-overhead)))	//try to encode in the space left		***400
	{					
		SendAbort(am,id,vsbe_segmentationnotsupported&0x00FF);								//	***1223
		goto scfree;																		//	***256		
	}
	*pp++=0x3F;												//context close tag 3	
	transmit(nm,(word)(pp-nm->npdu),nm->npdu,macisResponse,false);							//	***1210
scfree:																						//	***256 Begin
	//DO NOT FREE am here -- am is freed from the pool by the caller of this function!!!	
	if(np!=NULL)														
	{
#ifdef DEBUGVSBHP														
		sprintf(dbg,"%s (%d) calling npoolFreePacket\n",__FUNCTION__,__LINE__);
		fraLog(dbg);
#endif													
		npoolFreePacket(np);								
	}																	
}																							//	***256 End

///////////////////////////////////////////////////////////////////////							***405 Begin
//  Determine the max apdu size from the APDU header  
//
// in:	maxresponse the encoded size of the max apdu from the request
//
// out:	the calculated size

static	word calcClientMaxAPDU(octet maxresponse)
{
	switch(maxresponse&0x0F)																//	***406 mask lower nibble						
	{
	case APDUSIZE50:
		return 50;
	case APDUSIZE128:
		return 128;
	case APDUSIZE206:
		return 206;
	case APDUSIZE480:
		return 480;
	case APDUSIZE1024:
		return 1024;
	case APDUSIZE1476:
		return 1476;
	default:
		return 50;
	}
}																	//							***405 End	

///////////////////////////////////////////////////////////////////////							***222 Begin
//  Test if an ecoded value can fit in the transmit buffer  
//
// in:	da		source structure
//		stp		APDU start
//		op		current position in transmit buffer
//		wte		number of octets we want to encode
//
// out:	false	it fits
//		true	would overflow

static	bool WouldOverflow(frSource *da,octet *stp,octet *op,word wte)
{	word	used,room,clientroom;						//												***405
	
	used=(word)(op-stp);								//used this much so far
	room=macMaxRPDU(da->port)-used;						//how much room is left in the buffer
	if(wte>room) 
		return true;							//it would overflow
	clientroom=calcClientMaxAPDU(da->maxresponse);		//what did the client say they could take		***408 Begin			
	if(clientroom<used) 
		return true;					//have we already overflowed?
	clientroom-=used;									//subtract what we used	
	if(wte>clientroom) 
		return true;						//it would overflow								***408 End
	return false;										//it's OK
}														//												***222 End

///////////////////////////////////////////////////////////////////////					***243 Begin
//  Encode a property value  
//
// in:	op		pointer to pointer to place to put encoded property value
//		vb		pointer to frVbag to encode
//		mxn		max octets available in buffer *op points to
//
// out:	true	could not encode (not enough room)
//		false	encode was OK
//		  *op set to advanced pointer

static	bool EncodePropertyValue(octet **op,frVbag *vb,word mxn)
{	dword	*dw,i,j;
	byte	x;
	static frVbag	 tv;								//								***414
	char	*ts,*sp;
octet	*pp=*op,*qp,*ppp=NULL,*bpp=NULL;		//								***1208
frDateTimeStamp *dtp;							//								***1208
#ifdef FLOATLIB
	float	*fp;
#else
	dword	*fp;
#endif
	word net;											//								***1200

	buildvbString(&tv);								//									***1205
	switch(vb->pdtype)
	{	
		case adtUnsigned:
			if(mxn<5) return true;					//can't fit it		
			if (vb->narray)							//									***005 Begin
			{	dw=vb->ps.psdval;
				for (i=0;i<vb->narray;i++)
				{	if(*dw!=nullvalue)
					{	if(mxn<5) return true;		//can't fit it
						qp=pp;
						pp=eUINT(pp,*dw);
						mxn-=(word)(pp-qp);
					}
					else
					{	if(mxn==0) return true;		//can't fit it
						pp=eNULL(pp);
						mxn--;
					}
					dw++;
				}
			}
			else
			{
				pp=eUINT(pp,vb->pd.uval);			//									***005 End
			}
			break;
		case adtInteger:
			if(mxn<5) return true;					//can't fit it
			pp=eINT(pp,vb->pd.sval);
			break;
		case adtReal:
			if(mxn<5) return true;					//can't fit it
			if (vb->narray)							//									***005 Begin
			{	fp=vb->ps.psfval;
				dw=(dword *)fp;
				for (i=0;i<vb->narray;i++)
				{	if(*dw!=nullvalue)
					{	if(mxn<5) return true;		//can't fit it
						qp=pp;
						pp=eREAL(pp,0x44,*fp);
						mxn-=(word)(pp-qp);
					}
					else
					{	if(mxn==0) return true;		//can't fit it
						pp=eNULL(pp);
						mxn--;
					}
					fp++;
					dw++;
				}
			}
			else
				pp=eREAL(pp,0x44,vb->pd.fval);		//									***005 End
			break;
		case adtDouble:
			if(mxn<9) return true;					//can't fit it
			#ifdef FLOATLIB							//									***002 Begin
			pp=eDOUBLE(pp,&vb->pd.dval);
			#else
			pp=eDOUBLE(pp,&vb->pd.dval[0]);
			#endif									//									***002 End
			break;
		case adtBoolean:
			if(mxn==0) return true;					//can't fit it
			pp=eBOOL(pp,(bool)vb->pd.uval);
			break;
		case adtObjectID:
			if(mxn<5) return true;					//can't fit it
			pp=etagOBJECTID(pp,0xC0,vb->pd.uval);
			break;
		case adtEnumerated:
			if(mxn<5) return true;					//can't fit it
			if (vb->narray)							//									***005 Begin
			{	dw=vb->ps.psdval;
				for (i=0;i<vb->narray;i++)
				{	if(*dw!=nullvalue)
					{	if(mxn<5) return true;		//can't fit it
						qp=pp;
						pp=eENUM(pp,*dw);
						mxn-=(word)(pp-qp);
					}
					else
					{	if(mxn==0) return true;		//can't fit it
						pp=eNULL(pp);
						mxn--;
					}
					dw++;
				}
			}
			else
				pp=eENUM(pp,vb->pd.uval);			//									***005 End
			break;
		case adtCharString:
			if (vb->narray)							//									***005 Begin
			{																	//		***1218 Begin
				sp=(char*)vb->ps.psval;					//	***262
				buildfrString(&tv.pd.stval,tv.ps.psval,false,vb->pd.stval.len,vb->pd.stval.maxlen);
				j=0;
				for (i=0;i<vb->narray;i++)
				{						
					qp=pp;
					ts=(char*)tv.ps.psval;				//	***262
					tv.pd.stval.len=0;
					while (j<vb->pd.stval.len)
					{	j++;
						if (*sp!=charstringarraydelimiter)
						{	*ts++=*sp++;
							tv.pd.stval.len++;
						}
						else
						{	sp++;
							break;
						}
					}
					if(mxn<(5+tv.pd.stval.len)) return true;	//can't fit it			***411
					pp=eCHARSTRING(pp,&tv.pd.stval);
					mxn-=(word)(pp-qp);								//decrement remaining octet space
				}																	//	***1218 End
			}
			else
			{																			
				if(mxn<(5+vb->pd.stval.len)) return true;	//can't fit it			***411
				pp=eCHARSTRING(pp,&vb->pd.stval);	//								***005 End
			}
			break;
		case adtBitString:
			if(mxn<(4+vb->pd.uval)) return true;	//can't fit it
			pp=eBITSTRING(pp,vb->ps.psval,0x80,(word)vb->pd.uval);	//					***005
            break;
		case adtOctetString:
			if(mxn<(4+vb->pd.uval)) return true;	//can't fit it
			pp=eOCTETSTRING(pp,vb->ps.psval,(word)vb->pd.uval);		//					***005
			break;
        case adtDate:
			if(mxn<5) return true;					//can't fit it
			pp=eDATESTRING(pp,(word)vb->pd.dtval.year,vb->pd.dtval.month,vb->pd.dtval.day,vb->pd.dtval.dow);
            break;
        case adtTime:
			if(mxn<5) return true;					//can't fit it
			pp=eTIMESTRING(pp,vb->pd.tval.hour,vb->pd.tval.minute,vb->pd.tval.second,vb->pd.tval.hundredths);
            break;
 		case adtNull:
			if(mxn==0) return true;					//can't fit it
 			pp=eNULL(pp);
 			break;
		case adtObjectList:
			dw=vb->ps.psdval;						//									***005
			if(mxn<(5*vb->pd.uval)) return true;	//can't fit it
			for(i=0;i<vb->pd.uval;i++)
			{
				pp=etagOBJECTID(pp,0xC0,*dw);
				dw++;
			}
			break;
		case adtEmptyList:
			break;
		case adtDateTime:							//intentional fall through			***1206
		case adtDateTimeStamp:													//		***400 Begin
			dtp=vb->ps.psdts;						//point to array of these values
			if(vb->narray==0) vb->narray=1;
			if(mxn<(12*vb->narray)) return true;	//can't fit it
			pp=eDATESTRING(pp,(word)dtp->year, dtp->month, dtp->day, dtp->dow);
			pp=eTIMESTRING(pp,dtp->hour,dtp->minute,dtp->second,dtp->hundredths);
			break;
		case adtEventTimeStamp:
			dtp=vb->ps.psdts;						//point to array of these values
			if(vb->narray==0) vb->narray=1;
			if(mxn<(12*vb->narray)) return true;	//can't fit it
			for (i=0;i<vb->narray;i++)
			{
				*pp++=0x2E;
				pp=eDATESTRING(pp,(word)dtp->year, dtp->month, dtp->day, dtp->dow);
				pp=eTIMESTRING(pp,dtp->hour,dtp->minute,dtp->second,dtp->hundredths);
				*pp++=0x2F;
				dtp++;
			}
			break;
		case adtLogBuffer:														
			if(vb->narray==0) vb->narray=1;
			if(vb->narray>MAXLOGRECORDS) return true;	//can't fit it		
			if(mxn<(22*vb->narray)) return true;		//								***407
			for(i=0;i<vb->narray;i++)
				pp=EncodeLogRecord(pp,&vb->ps.pslr[i]);	//								***1206
			break;										//								***400 End
		case adtEventLogBuffer:							//								***1206 Begin
			if(vb->narray==0) vb->narray=1;
			if(vb->narray>MAXEVENTLOGRECORDS) return true;	//can't fit it		
			if(mxn<(124*vb->narray)) return true;
			for(i=0;i<vb->narray;i++)
				pp=EncodeEventLogRecord(pp,&vb->ps.pselr[i]);
			break;										//								***1206 End
		case adtDeviceObjectReference:					//								***600 Begin
			if(vb->narray==0) vb->narray=1;
			if(vb->narray>MAXDOR) return true;			//can't fit it		
			if(mxn<(16*vb->narray)) return true;		//can't fit it 
			for(i=0;i<vb->narray;i++)
			{	
				if (vb->ps.psdor[i].deviceid!=noobject)
				{
					pp=etagOBJECTID(pp,0x08,vb->ps.psdor[i].deviceid);		//[0]deviceIdentifier (optional)					
				}
				pp=etagOBJECTID(pp,0x18,vb->ps.psdor[i].objid);				//[1]objectIdentifier				
			}
			break;										//								***600 End
		case adtDeviceObjectPropertyReference:
			if(vb->narray==0) vb->narray=1;
			if(vb->narray>MAXDOPR) return true;			//can't fit it		
			if(mxn<(16*vb->narray)) return true;		//can't fit it					***407
			for(i=0;i<vb->narray;i++)
			{	
				pp=etagOBJECTID(pp,0x08,vb->ps.psdopr[i].objid);				//[0]objectIdentifier
				pp=eDWORD(pp,vb->ps.psdopr[i].propid,0x18,false);				//[1]propertyIdentifier
				if (vb->ps.psdopr[i].arrayindex!=noindex)
					pp=eDWORD(pp,vb->ps.psdopr[i].arrayindex,0x28,false);		//[2]propertyArrayIndex (optional)
				if (vb->ps.psdopr[i].deviceid!=noobject)
					pp=etagOBJECTID(pp,0x38,vb->ps.psdopr[i].deviceid);			//[3]deviceIdentifier (optional)				
			}	
			break;
		case adtObjPropRef:								//								***1206 Begin
			pp=etagOBJECTID(pp,0x08,vb->ps.pspropref.objid);					//[0]objectIdentifier
			pp=eDWORD(pp,vb->ps.pspropref.propid,0x18,false);					//[1]propertyIdentifier
			if (vb->ps.pspropref.arrindex!=noindex)
				pp=eDWORD(pp,vb->ps.pspropref.arrindex,0x28,false);				//[2]propertyArrayIndex (optional)
			break;										//								***1206 End
		case adtRecipientList:													
			if(vb->narray==0) vb->narray=1;
			if(vb->narray>MAXDESTS) return true;		//can't fit it
			if(mxn<(36*vb->narray)) return true;		//can't fit it					***407
			for(i=0;i<vb->narray;i++)
				pp=EncodeDestination(pp,&vb->ps.psdest[i]);	//							***1206
			break;	
		case adtDateList:
			if(vb->narray==0) vb->narray=1;
			if(vb->narray>MAXCALENTRY) return true;
			if(mxn<(10*vb->narray)) return true;		//can't fit it					***407
			for(i=0;i<vb->narray;i++)
				if(!((vb->ps.pscal[i].type==0) && ((vb->ps.pscal[i].dt.day==0)&&(vb->ps.pscal[i].dt.year ==0)&&(vb->ps.pscal[i].dt.month==0))))	//	***1223
					pp=EncodeCalendarEntry(pp,&vb->ps.pscal[i]); //							***1206
			break;		
		case adtEffectivePeriod:	
			if(mxn<10) return true;						//								***407
			pp=eDATESTRING(pp,(word)vb->ps.ep.start.year,vb->ps.ep.start.month,vb->ps.ep.start.day,vb->ps.ep.start.dow);
			pp=eDATESTRING(pp,(word)vb->ps.ep.end.year,vb->ps.ep.end.month,vb->ps.ep.end.day,vb->ps.ep.end.dow);
			break;		
		case adtTimeValue:							
			if(vb->narray>MAXTIMEVAL) return true;			
			if(vb->pd.uval==1)							//only return 1 day
			{
				if(mxn<(7*vb->narray)) return true;		//								***407
				*pp++=0x0E;								//[0] Open Tag
				for(i=0;i<vb->narray;i++)				//# of elements
					pp=EncodeTimeValue(pp,&vb->ps.tv[i].tv);
				*pp++=0x0F;								//[0] Close Tag
			}
			else
			{											// return all seven days
				//65 is the number of octets required to encode 7 time value pairs
				if (mxn<(65*vb->narray)) return true;	//can't fit it					***1224
				vb->pd.uval=7;							//just in case
				for(i=0;i<vb->pd.uval;i++)				//# of days, if it's not 1 it's 7 and we need to match here
				{
					*pp++=0x0E;							//[0] Open Tag
					for(x=0;x<vb->narray;x++)			//# of elements
					{
						if(vb->ps.tv[x].day==(i+1))		//if it's for the current day, add it to the stream		//	***408
							pp=EncodeTimeValue(pp,&vb->ps.tv[x].tv);
					}
					*pp++=0x0F;							//[0] Close Tag
				}
			}
			break;
		case adtSpecialEvent:
			if(vb->narray>MAXSPECIALEVNTS) return true;
			if(mxn<(31*vb->narray)) return true;        //can't fit it with at least one time value  //	***407  
			for(i=0;i<vb->narray;i++)
			{
				if(vb->ps.se[i].caltype==0)				//10 bytes worst case
				{	
					*pp++=0x0E;															//[0]open tag for period-calendarEntry
					if((pp=EncodeCalendarEntry(pp,&vb->ps.se[i].calendarEntry))==NULL)	//				***1206
						return true;
					*pp++=0x0F;															//[0]close tag for period-calendarEntry
				}
				else if (vb->ps.se[i].caltype==1)										//[1]period-calendar-reference(BACnetObjectID)
					pp=etagOBJECTID(pp,0x18,vb->ps.se[i].calendarreference);
				else
					return true;														//not period		
				*pp++=0x2E;					//16 bytes including 2E/2F					//[2]open tag listOfTimeValues						
				for(x=0;x<vb->ps.se[i].numvalues;x++)
				{
					pp=EncodeTimeValue(pp,&vb->ps.se[i].tv[x]);	
				}
				*pp++=0x2F;																//[2]close tag listOfTimeValues						
				pp=eDWORD(pp,vb->ps.se[i].eventPriority,0x38,false);					//[3]eventPriority
			}																	//		***400 End
			break;
		case adtCOVSubscription:																//	***410 Begin
			bpp=pp;
			if(mxn<(48*vb->narray)) return true;												//can't fit it			
			for(i=0;i<vb->narray;i++)
			{					
				*pp++=0x0E;																		//[0]open tag RecipientProcess
				*pp++=0x0E;																		//[0]open tag recipient
				if (vb->ps.cs[i].recip.type==0)
					pp=etagOBJECTID(pp,0x08,vb->ps.cs[i].recip.recipient.device);				//[0]device
				else
				{	
					*pp++=0x1E;																	//[1]address open tag
#if bigEndian																					//								***416 Begin
					pp=eUINT(pp,vb->ps.cs[i].recip.recipient.addr.network);						//address (network-number, leave in bigEndian)
#else
					pp=eUINT(pp,flipw(vb->ps.cs[i].recip.recipient.addr.network));				//address (network-number, flip to bigEndian)
#endif																							//								***416 End
					pp=eOCTETSTRING(pp,vb->ps.cs[i].recip.recipient.addr.address,vb->ps.cs[i].recip.recipient.addr.noctets);//address (mac-address)
					*pp++=0x1F;																	//[1]address close tag
				}
				*pp++=0x0F;																		//[0]closed tag recipient	
				pp=eDWORD(pp,vb->ps.cs[i].procid,0x18,false);									//[1]processIdentifier			
				*pp++=0x0F;																		//[0]closed tag RecipientProcess
				*pp++=0x1E;																		//[1]open tag MonitoredPropertyReference
				pp=etagOBJECTID(pp,0x08,vb->ps.cs[i].propref.objid);							//[0]objectIdentifier
				pp=eDWORD(pp,vb->ps.cs[i].propref.propid,0x18,false);							//[1]propertyIdentifier
				if (vb->ps.cs[i].propref.arrayindex!=noindex) pp=eDWORD(pp,vb->ps.cs[i].propref.arrayindex,0x28,false);	//[2]propertyArrayIndex (OPT)
				*pp++=0x1F;																		//[1]closed tag MonitoredPropertyReference
				pp=eDWORD(pp,(vb->ps.cs[i].issue)?1:0,0x28,false);								//[2]IssueConfirmedNotifications	
				pp=eDWORD(pp,vb->ps.cs[i].timeremaining,0x38,false);							//[3]TimeRemaining
				if (vb->ps.cs[i].hasincrement)
				{	
					ppp=pp;
					pp=eREAL(pp,0x44,vb->ps.cs[i].increment);									//[4]COVIncrement
					*ppp=0x4C;																	//tagged real
				}			
				mxn-=(word)(pp-bpp);				
			}
			break;																				//	***410 end
		case adtScale:																			//	***603 Begin
			bpp=pp;
			if(vb->ps.psscale.choice==0)
			{				
				pp=eREAL(pp,0x0C,vb->ps.psscale.sv.f);											//[0]REAL
			}
			else if(vb->ps.psscale.choice==1)
			{	
				pp=eDWORD(pp,vb->ps.psscale.sv.i,0x18,true);									//[1]INTEGER
			}
			break;																				//	***603 End
		case adtTimeStamp:									//										***1200 Begin				
			switch(vb->ps.ts.type)
			{
			case 0:													// [0]time
				bpp=pp;
				pp=eTIMESTRING(pp,vb->ps.ts.time.hour,vb->ps.ts.time.minute,vb->ps.ts.time.second,vb->ps.ts.time.hundredths);
				*bpp=(*bpp&0x07)+0x08;								//tag 0
				break;
			case 1:													// [1]sequence
				pp=eDWORD(pp,vb->ps.ts.sequence,0x18,false);
				break;
			case 2:													// [2]bacnetdatetime
				*pp++=0x2E;											// open tag
				pp=eDATESTRING(pp,vb->ps.ts.date.year,vb->ps.ts.date.month,vb->ps.ts.date.day,vb->ps.ts.date.dow);
				pp=eTIMESTRING(pp,vb->ps.ts.time.hour,vb->ps.ts.time.minute,vb->ps.ts.time.second,vb->ps.ts.time.hundredths);
				*pp++=0x2F;											// close tag
				break;
			default:
				return true;
			}								
			break;	
		case adtRecipientAddr:
			for(i=0;i<vb->narray;i++)
			{					
				if (vb->ps.ra[i].choice==0)
					pp=eDWORD(pp,vb->ps.ra[i].device,0x08,false);				//[0]device
				else if (vb->ps.ra[i].choice==1)
				{	
					*pp++=0x1E;															//[1]address open tag					
					net=(word)((vb->ps.ra[i].address.network_number<<8)&0xFF00)+(word)((vb->ps.ra[i].address.network_number>>8)&0xFF);	//this has to be little-endian
					pp=eUINT(pp,net);
					pp=eOCTETSTRING(pp,vb->ps.ra[i].address.mac_address,vb->ps.ra[i].address.mac_len);	//address (mac-address)
					*pp++=0x1F;															//[1]address close tag
				}
				else
					return true;														//invalid data type			
			}												//										***1200 End
			break;
 	}
	*op=pp;											//update pointer
	return false;									//succeeded
}													//									***243 End

///////////////////////////////////////////////////////////////////////
//	Fill in the return information for a reply
//
//in:	am			points to the incoming packet info
//		xpci		extra NPCI info
//out:	points past filled-in NL header to where APDU can go
//		am->apdu	also points there
//		am->nlen	length filled-in so far

static	octet *RSVP(frNpacket *am,octet xpci)
{	octet *pp=am->npdu;							//point to where npdu goes
	int		i;

	*pp++=1;										//BACnet version 1
	if(am->src.snet.w)								//it was routed to us
	{	
		*pp++=NSDUnormalreply|NSDUhasDNET|xpci;     //NPDU contains the return path
        //                                                              ***1210 Begin
        //IMPORTANT: In this routing scenario, the frSource is structured
        //  as follows:
        //      snet.w contains the original SNET
        //      slen contains the original SLEN
        //      sadr contains the original SADR
        //      dradr contains the address of the router,
        //          which is the local address that we'll be sending to
#if bigEndian														//	***1218 Begin
		*pp++=am->src.snet.b[0];
        *pp++=am->src.snet.b[1];
#else
		*pp++=am->src.snet.b[1]; 
        *pp++=am->src.snet.b[0];
#endif																//	***1218 End
        *pp++=am->src.slen;                         //SLEN becomes DLEN
        for(i=0;i<am->src.slen;i++)                 //original SADR become DADR
            *pp++=am->src.sadr[i];                  //                  ***1210 End
		*pp++=255;									//hopcount
	}
	else											//local message
		*pp++=NSDUnormalreply;
	am->apdu=pp;
	am->nlen=(word)(pp-am->npdu);					//calculate what we used
	return pp;										//return pointer to where APDU goes
}

///////////////////////////////////////////////////////////////////////
//  Send an ABORT packet 
//     
//in:	am		incoming packet
//		id		the invokeid of the transaction
//		reason	the reason for aborting

static	void SendAbort(frNpacket *am,octet id, octet reason)
{	octet *pp=RSVP(am,0);

	if (commEnabled==false) return;							//							***010
	*pp++=((ABORT_PDU<<ASDUpdutypeshr)&ASDUpdutype)|ASDUserver; //PDU header
	*pp++=id;										//invoke id
	*pp++=reason;									//abort reason
	//macTransmit(am->src.port,am->src.sadr,(word)(am->nlen+3),am->npdu,macisResponse); //	***240
    transmit(am,(word)(am->nlen+3),am->npdu,macisResponse,false);						//	***1210
}

///////////////////////////////////////////////////////////////////////
//  Send a REJECT packet 
//
//in:	am		incoming packet
//		id		the invokeid of the transaction
//		reason	the reason for rejecting

static	void SendReject(frNpacket *am,octet id, octet reason)
{	octet *pp=RSVP(am,0);

	if (commEnabled==false) return;							//							***010
	*pp++=((REJECT_PDU<<ASDUpdutypeshr)&ASDUpdutype); //PDU header
	*pp++=id;										//invoke id
	*pp++=reason;									//abort reason
	//macTransmit(am->src.port,am->src.sadr,(word)(am->nlen+3),am->npdu,macisResponse); //	***240
    transmit(am,(word)(am->nlen+3),am->npdu,macisResponse,false);						//	***1210
}

///////////////////////////////////////////////////////////////////////
//  Send a Simple ACK packet 
//     
//in:	am			incoming packet
//		id			the invokeid of the transaction
//		serviceack	the service ack choice

static	void SendSACK(frNpacket *am,octet id, octet serviceack)
{	octet *pp=RSVP(am,0);

	if (commEnabled==false) return;							//							***010
	*pp++=((SIMPLE_ACK_PDU<<ASDUpdutypeshr)&ASDUpdutype); //PDU header
	*pp++=id;										//invoke id
	*pp++=serviceack;								//ack reason
	//macTransmit(am->src.port,am->src.sadr,(word)(am->nlen+3),am->npdu,macisResponse); //	***240
    transmit(am,(word)(am->nlen+3),am->npdu,macisResponse,false);						//	***1210
}

///////////////////////////////////////////////////////////////////////
//  Send an ERROR packet 
//
//in:	am		incoming packet
//		id		the invokeid of the transaction
//		service	the service
//		eclass	the error class
//		ecode	the error code
//		tp		points past NL to reply APDU

static	void SendError(frNpacket *am,octet id,octet service,word eclass,word ecode)
{	octet *pp=RSVP(am,0);

	if (commEnabled==false) return;							//							***010
	*pp++=(ERROR_PDU<<ASDUpdutypeshr)&ASDUpdutype;	//PDU header
	*pp++=id;										//invoke id
	*pp++=service;									//error choice
	pp=eENUM(pp,eclass);
	pp=eENUM(pp,ecode);
	//macTransmit(am->src.port,am->src.sadr,(word)(pp-am->npdu),am->npdu,macisResponse); //	***240
    transmit(am,(word)(pp-am->npdu),am->npdu,macisResponse,false);						//	***1210
}

///////////////////////////////////////////////////////////////////////					***240 Begin
//Prepare an Npacket with a confirmed request
//
//in:	todevice	the device to send the request to
//		service		the service that will be encoded
//		big			true if a big packet will be needed
//		rerr		filled with error code if NULL returned
//		vb			frVbag to use
//out:	NULL		unknown device (at this time) or can't get Npacket
//					these errors must be retried by the application itself
//		else		pointer to frNpacket filled-in with header info
//					ptr->nlen	number of bytes used so far by header info
//					ptr->apdu	points to &ptr->npdu[ptr->nlen]
//					ptr->nflag	has Nreplyexpected set
//					ptr->src.sadr,slen is the MAC to transmit directly to
//					ptr->timer	set to APDU_TIMEOUT
//					ptr->ntries set to 0
//					ptr->invokeid set to a new invokeid
//		NOTE:		Must release frNpacket using npoolFreePacket after reply is received or timeout.

static	frNpacket *ConfirmedRequest(dword todevice, octet service, bool big, int *rerr, frVbag *vb)
{	frNpacket	*np;
	octet		*tp;
	word		dvx,j;

	if((dvx=SearchForDevice(todevice))==notheardfrom)
	{	*rerr=vsbe_unknowndevice;
		return NULL;									//don't know this guy, stop right now
	}
	if((np=npoolGetPacket((word)((big)?maxmac:smallalloc)))==NULL)
	{	
		*rerr=vsbe_outofresources;						//	***1223
		return NULL;									//can't get a packet right now
	}
	bytecpy((byte*)&np->src,(byte*)&devlist[dvx].src,sizeof(frSource));	//copy the device's source info		***1213
	tp=np->npdu;
	*tp++=1;											//BACnet version 1
	if (devlist[dvx].src.snet.w==0)						//no routing
	{	
		*tp++=NSDUexpectsreply;		
	}
	else
	{	
		*tp++=NSDUhasDNET+NSDUexpectsreply;
		*tp++=(devlist[dvx].src.snet.w>>8)&0xFF;
        *tp++=(devlist[dvx].src.snet.w&0xFF);
        *tp++=devlist[dvx].src.slen;                    //destination address length            ***1210 Begin
        for (j=0;j<devlist[dvx].src.slen;j++)
            *tp++=devlist[dvx].src.sadr[j];             //destination address
        *tp++=255;                                      //hopcount								***1210 End
	}
	*tp++=CONF_REQ_PDU<<4;
	*tp++=(portid==portMSTP)?3:5;						//max response (ie, 480/1476)
	*tp++=nextInvokeID;
	*tp++=service;
	np->apdu=tp;
	np->nlen=(word)(tp-np->npdu);						//calculate number of octets stuffed
	np->nflag|=Nreplyexpected;
	np->vb=(void *)vb;
	np->ntries=0;
	np->timer=di.apdu_timeout;
	np->invokeid=nextInvokeID++;						//a new invoke id
	np->svc=service;
	return np;											//								***240 End
}

///////////////////////////////////////////////////////////////////////					***240 Begin
//Prepare an Npacket with a confirmed request based on MAC address
//
//in:	src			the MAC address of the device to send the request to
//		service		the service that will be encoded
//		big			true if a big packet will be needed
//		rerr		filled with error code if NULL returned
//		vb			frVbag to use
//out:	NULL		an't get Npacket
//					this error must be retried by the application itself
//		else		pointer to frNpacket filled-in with header info
//					ptr->nlen	number of bytes used so far by header info
//					ptr->apdu	points to &ptr->npdu[ptr->nlen]
//					ptr->nflag	has Nreplyexpected set
//					ptr->src.sadr,slen is the MAC to transmit directly to
//					ptr->timer	set to APDU_TIMEOUT
//					ptr->ntries set to 0
//					ptr->invokeid set to a new invokeid
//		NOTE:		Must release frNpacket using npoolFreePacket after reply is received or timeout.

static	frNpacket *ConfirmedRequestMAC(frSource *src, octet service, bool big, int *rerr, frVbag *vb)
{
frNpacket	*np;								//								***1208
	octet		*tp;
	word		j;

	if((np=npoolGetPacket((word)((big)?maxmac:smallalloc)))==NULL)	//					***253
	{	
		*rerr=vsbe_outofresources;									//					***1223
		return NULL;									//can't get a packet right now
	}
	bytecpy((byte*)&np->src,(byte*)src,sizeof(frSource));	//copy the incoming source info					***1213
	tp=np->npdu;
	*tp++=1;											//BACnet version 1
	if (src->snet.w==0)									//no routing									***413 Begin										
	{	
		*tp++=NSDUexpectsreply;		
	}
	else
	{	
		*tp++=NSDUhasDNET+NSDUexpectsreply;
		*tp++=(src->snet.w>>8)&0xFF;					//												***1210 Begin
        *tp++=(src->snet.w&0xFF);
		*tp++=src->slen;
		for (j=0;j<src->slen;j++)						//destination MAC address
			*tp++=src->sadr[j]; 
		*tp++=255;										//hop count										***1210 End        
	}													//												***413	End				
	*tp++=CONF_REQ_PDU<<4;
	*tp++=(portid==portMSTP)?3:5;						//max response (ie, 480/1476)
	*tp++=nextInvokeID;
	*tp++=service;
	np->apdu=tp;
	np->nlen=(word)(tp-np->npdu);						//calculate number of octets stuffed
	np->nflag|=Nreplyexpected;
	np->vb=(void *)vb;
	np->ntries=0;
	np->timer=di.apdu_timeout;
	np->invokeid=nextInvokeID++;						//a new invoke id
	np->svc=service;
	return np;											//								***240 End
}

///////////////////////////////////////////////////////////////////////					***240 Begin
//Prepare an Npacket with an unconfirmed request
//
//in:	dnet		(big endian) 0=local broadcast, FFFF=global, else specific network	***1205
//		service		the service that will be encoded
//		big			true if a big packet will be needed
//out:	NULL		can't get Npacket (must be retried by the application itself)
//		else		pointer to frNpacket filled-in with header info
//					ptr->nlen	number of bytes used so far by header info
//					ptr->apdu	points to &ptr->npdu[ptr->nlen]
//					ptr->nflag	has Nreplyexpected cleared
//					ptr->timer	0
//					ptr->ntries 0
//					ptr->invokeid 0
//		NOTE:		Must release frNpacket using UnconfirmedSend

static	frNpacket *UnconfirmedRequest(word dnet, octet service, bool big)	//			***1205
{
union { dword d; word w; byte b[4];} u;					//								***1205
frNpacket	*np;								//								***1208
octet		*tp;

	if((np=npoolGetPacket((word)((big)?maxmac:smallalloc)))==NULL)	//					***253
		return NULL;									//can't get a packet right now	***253
	tp=np->npdu;
	*tp++=1;											//BACnet version 1
	if(dnet==0)											//local network only			***1205 Begin
		*tp++=NSDUnormalreply;
	else												//have a specific dnet
	{
		*tp++=NSDUhasDNET;
		u.w=dnet;										//get DNET, ALWAYS big endian
#if bigEndian
		*tp++=u.b[0];									//msb of length first
		*tp++=u.b[1];
#else
		*tp++=u.b[1];									//msb of length first
		*tp++=u.b[0];
#endif
		*tp++=0;										//broadcast dlen
		*tp++=255;										//hop count
	}													//								***1205 End
	*tp++=UNCONF_REQ_PDU<<4;
	*tp++=service;
	np->apdu=tp;
	np->nlen=(word)(tp-np->npdu);						//calculate number of octets stuffed
	np->vb=NULL;
	np->nflag&=~Nreplyexpected;
	np->ntries=0;
	np->timer=0;
	np->invokeid=0;
	return np;
}														//								***240 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//Prepare an Npacket with a unconfirmed request based on MAC address
//
//in:	src			the MAC address of the device to send the request to
//		service		the service that will be encoded
//		big			true if a big packet will be needed
//out:	NULL		can't get Npacket
//		else		pointer to frNpacket filled-in with header info
//					ptr->nlen	number of bytes used so far by header info
//					ptr->apdu	points to &ptr->npdu[ptr->nlen]
//					ptr->nflag	has Nreplyexpected set
//					ptr->src.sadr,slen is the MAC to transmit directly to
//					ptr->timer	set to APDU_TIMEOUT
//					ptr->ntries set to 0
//					ptr->invokeid set to a new invokeid
//		NOTE:		Must release frNpacket using npoolFreePacket after reply is received or timeout.

static	frNpacket *UnconfirmedRequestMAC(frSource *src, octet service, bool big)
{
frNpacket	*np;								//								***1208
	octet		*tp;
	word		j;

	if((np=npoolGetPacket((word)((big)?maxmac:smallalloc)))==NULL)	//					***253
		return NULL;									//can't get a packet right now	***253
	bytecpy((byte*)&np->src,(byte*)src,sizeof(frSource));	//copy the incoming source info	***1213
	tp=np->npdu;
	*tp++=1;											//BACnet version 1
	if(src->snet.w==globalnetwork)						//								***1208
	{
		*tp++=NSDUhasDNET;								//broadcast to global network
		*tp++=0xFF;
		*tp++=0xFF;
		*tp++=0;
		*tp++=255;										//hop count		
	}
	else if(src->snet.w==0)								//local network					***1208 begin
	{
		*tp++=0;
	}													//								***1208 End
	else
	{
		*tp++=NSDUhasDNET;
		*tp++=(src->snet.w>>8)&0xFF;					//								***1210 Begin
        *tp++=(src->snet.w&0xFF);
		*tp++=src->slen;
		for (j=0;j<src->slen;j++)						//destination MAC address
			*tp++=src->sadr[j]; 
		*tp++=255;										//hop count        
	}
	*tp++=UNCONF_REQ_PDU<<4;
	*tp++=service;
	np->apdu=tp;
	np->nlen=(word)(tp-np->npdu);						//calculate number of octets stuffed
	np->vb=NULL;
	np->nflag&=~Nreplyexpected;
	np->ntries=0;
	np->timer=0;
	np->invokeid=0;
	return np;
}														//								***240 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Transmit a Confirmed Request
//
//in:	np		has the prepared request

static	void ConfirmedSend(frNpacket *np)
{
#ifdef DEBUGVSBHP
	char t[128];
#endif
	 if(np->src.snet.w==0&&np->src.slen>0)				//local guy						***1211 Begin
        transmit(np,np->nlen,np->npdu,macisRequest,false);
    else if((np->src.snet.w!=0)&&(np->src.drlen!=0))
        transmit(np,np->nlen,np->npdu,macisRequest,false);
    else
        transmit(np,np->nlen,np->npdu,macisRequest,true);               //				***1211 End

#ifdef DEBUGVSBHP
		sprintf(t,"%s %d PutQ %08X\n",__FILE__,__LINE__,(unsigned int)&np);
		fraLog(t);
#endif	
	PutQ(&confirmq,np);									//								***256
}														//								***240 End

///////////////////////////////////////////////////////////////////////					***240 Begin
//	Transmit an Unconfirmed request
//
//in:	np		has the prepared request

static	void UnconfirmedSend(frNpacket *np)
{
	 if(np->src.snet.w==0&&np->src.slen>0)				//local guy                    	***1211 Begin
        transmit(np,np->nlen,np->npdu,macisUnconfirmed,false);
    else if((np->src.snet.w!=0)&&(np->src.drlen!=0))
        transmit(np,np->nlen,np->npdu,macisUnconfirmed,false);
    else
        transmit(np,np->nlen,np->npdu,macisUnconfirmed,true);            //				***1211 End

#ifdef DEBUGVSBHP										//								***256 Begin
	sprintf(dbg,"%s (%d) calling npoolFreePacket\n",__FUNCTION__,__LINE__);
	fraLog(dbg);
#endif													//								***256 End
	npoolFreePacket(np);								//done with it
}														//								***240 End

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Null Value
//in:	op		points to an octet buffer to put the results in
//out:	return	advanced pointer to buffer

static	octet *eNULL(octet *op)
{
	*op++=0;
	return op;
} 															//

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Unsigned Integer Value
//in:	op		points to an octet buffer to put the results in
//		val		has the value to encode
//out:	return	advanced pointer to buffer

static	octet *eUINT(octet *op, dword val)
{
	return eDWORD(op,val,0x20,false);				//minimal encode using tag 2
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Signed Integer Value
//in:	op		points to an octet buffer to put the results in
//		val		has the long value to encode
//out:	return	advanced pointer to buffer

static	octet *eINT(octet *op, long val)
{
	return eDWORD(op,val,0x30,true);				//minimal encode using tag 3
}

///////////////////////////////////////////////////////////////////////
//	Encode an Application Tagged Real Value
//in:	op		points to an octet buffer to put the results in
//		tag		tag octet (0x44 or 0x?C)
//		val		has the fp value to encode
//out:	return	advanced pointer to buffer

#ifdef FLOATLIB										//									***002 Begin
static	octet *eREAL(octet *op, octet tag, float val)
{	union { float rv; byte b[4];} u;
	int		i;

	u.rv=val;										//so we can get at individual bytes
	*op++=tag;
#if bigEndian
	for(i=0;i<4;i++)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];			
#else
	for(i=3;i>=0;i--)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];
#endif			
	return op;
}
#else
static	octet *eREAL(octet *op, octet tag, dword val)
{	union { dword rv; byte b[4];} u;
	int		i;

	u.rv=val;										//so we can get at individual bytes
	*op++=tag;
#if bigEndian
	for(i=0;i<4;i++)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];			
#else
	for(i=3;i>=0;i--)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];
#endif			
	return op;
}
#endif												//									***002 End

///////////////////////////////////////////////////////////////////////
//	Encode an Application Tagged Double Value
//in:	op		points to an octet buffer to put the results in
//		val		has pointer the fp value to encode	//									***002
//out:	return	advanced pointer to buffer

#ifdef FLOATLIB										//									***002 Begin
static	octet *eDOUBLE(octet *op, double *val)
{	union { double dv; byte b[8];} u;
	int		i;
	u.dv=*val;										//so we can get at individual bytes

	*op++=0x55;										//tag
	*op++=8;										//extended length
#if bigEndian
	for(i=0;i<8;i++)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];			
#else
	for(i=7;i>=0;i--)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];
#endif			
	return op;
}
#else
static	octet *eDOUBLE(octet *op, dword *val)
{	union { dword dv[2]; byte b[8];} u;
	int		i;
	u.dv[0]=val[0];									//so we can get at individual bytes
	u.dv[1]=val[1];

	*op++=0x55;										//tag
	*op++=8;										//extended length
#if bigEndian
	for(i=0;i<8;i++)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];			
#else
	for(i=7;i>=0;i--)								//store all bytes, ms first, byte 0 last
		*op++=(octet)u.b[i];
#endif			
	return op;
}
#endif												//									***002 End

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Boolean Value
//in:	op		points to an octet buffer to put the results in
//		val		has the boolean state(true or false) to encode
//out:	return	advanced pointer to buffer

static	octet *eBOOL(octet *op, bool val)
{
	*op++=((val) ? 0x11:0x10);						//tag 1, and boolean value
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Character String
//in:	op		points to an octet buffer to put the results in
//		cs		points to the character string to encode
//		n		has the number of octets to encode(0-65535)
//				NOTE: this is the length of the string NOT including the
//					  characterset or code page octets!
//		cscode	has the characterset code(see BCstring enumeration)
//		cpage	is the code page for string type MS_DBCS
//out:	return	advanced pointer to buffer

static	octet *eCHARSTRING(octet *op,frString *fs)
{
	union { word w; byte b[2];} u;
	word	i,nc;
	octet *cs=(octet *)fs->text;

	nc=(word)fs->len+1;								//string length plus characterset octet
	if(nc<=4)
		*op++=0x70+(octet)nc;						//tag 7, length
	else
	{	
		*op++=0x75;									//tag 7, extended length
		if(nc<=253)
			*op++=(octet)nc;
		else										//assume nc<=65535
		{	
			*op++=0xFE;
			u.w=nc;
#if bigEndian
			*op++=(octet)u.b[0];					//msb of length first
			*op++=(octet)u.b[1];
#else
			*op++=(octet)u.b[1];					//msb of length first
			*op++=(octet)u.b[0];
#endif
		}	
	}
	
	*op++=chrsUTF8;									//***1200
	for(i=0;i<fs->len;i++)							//store all characters in order
		*op++=(octet)*cs++;			
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Enumerated Value
//in:	op		points to an octet buffer to put the results in
//		eval	has the dword enumerated value to encode
//out:	return	advanced pointer to buffer

static	octet *eENUM(octet *op, dword eval)
{
	return eDWORD(op,eval,0x90,false);				//minimal encode using tag 9
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged 4 octet minimal encoding value
//in:	op		points to an octet buffer to put the results in
//		val		has the value to encode
//		utag	is the tag value to use, e.g. 0x20 is tag 2
//		sgnd	true if signed, false if unsigned
//out:	return	advanced pointer to buffer

static	octet *eDWORD(octet *op, dword val,octet utag,bool sgnd)
{	union { dword dw; byte b[4];} u;
	int		i;
    bool 	ffndb=false;							//found first non-discardable byte
    bool	db;										//true if this is a discardable byte
    
	u.dw=val;										//so we can get at individual bytes
	*op++=utag+0x04;								//tag(utag), assume 4 octets(dword)
#if bigEndian
	for(i=0;i<4;i++)								//store all bytes from first non-0 ms byte, ms first, ls last
	{	db=(u.b[i]==0);								//if 0, it's discardable
		if(sgnd&&(u.b[i]==0xFF)) db=true;			//also discardable if signed and it's FF
//
// The following code is added so that the following cases of SIGNED numbers are encoded properly:
//		0x00000080	encoded as 32 00 80
//		0xFFFFFF80	encoded as 31 80
//		0xFFFFFF7F	encoded as 32 FF 7F
//		etc.
		if(sgnd && db && !ffndb &&(i!=0))			 //it's signed and possibly discardable
		{	if((u.b[i]==0) &&((u.b[i+1]&0x80) != 0)) db=false;	//don't discard
			else if((u.b[i]==0xFF) &&((u.b[i+1]&0x80)==0)) db=false;	//don't discard 
		}
//
		//if(!db||ffndb||i==0)
		if(!db||ffndb||i==3)						//	***256
		{	*op++=(octet)u.b[i];					//save byte
			ffndb=true;								//we found first non-discardable byte
		}
		else
			op[-1]--;								//one less digit than assumed
	}			
#else
	for(i=3;i>=0;i--)								//store all bytes from first non-0 ms byte, ms first, ls last
	{	db=(u.b[i]==0);								//if 0, it's discardable
		if(sgnd&&(u.b[i]==0xFF)) db=true;			//also discardable if signed and it's FF
//
// The following code is added so that the following cases of SIGNED numbers are encoded properly:
//		0x00000080	encoded as 32 00 80
//		0xFFFFFF80	encoded as 31 80
//		0xFFFFFF7F	encoded as 32 FF 7F
//		etc.
		if(sgnd && db && !ffndb &&(i!=0))							//it's signed and possibly discardable
		{	if((u.b[i]==0) &&((u.b[i-1]&0x80) != 0)) db=false;	//don't discard
			else if((u.b[i]==0xFF) &&((u.b[i-1]&0x80)==0)) db=false;	//don't discard 
		}
//
		if(!db||ffndb||i==0)
		{	*op++=(octet)u.b[i];					//save byte
			ffndb=true;								//we found first non-discardable byte
		}
		else
			op[-1]--;								//one less digit than assumed
	}			
#endif
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Date String
//
//NOTE:	Error cases generate *don't care* values!
//		Bad dates, e.g. Feb 30 and Feb 29 for non-leap years and
//		incorrect day-of-week for a given date are not checked!
//
//in:	op		points to an octet buffer to put the results in
//		year	the actual year, e.g. 1995, or FFFF means don't care
//				(values <255 are interpreted as 1900+year)
//		month	the month, 1=Jan or FF=don't care 
//		day		the day 1-31 or FF=don't care
//		dow		the day of week, 1=Mon, 7=Sun or FF=don't care
//out:	return advanced pointer to buffer

static	octet *eDATESTRING(octet *op, word year, octet month, octet day, octet dow)
{	*op++=0xA4;										//tag 10, 4 octets
	if(year>=1900) year-=1900;						//convert large year format
	if(year>254) year=0xFF;							//bad ones are don't cares
	*op++=(octet)year;
	if(month<1||month>14) month=0xFF;				//	***1223
   	if(day<1||day>34) day=0xFF;						//	***1223
	if(dow<1||dow>7) dow=0xFF;
	*op++=month;
	*op++=day;
	*op++=dow;
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Time String
//
//	NOTE:	bad values generate *don't care* !
//
//in:	op 		points to an octet buffer to put the results in
//		hour	the hour 0..23 or FF=don't care
//		min		the minute 0..59 or FF=don't care
//		sec		the second 0..59 or FF=don't care
//		hun		hundredths 0..99 or FF=don't care
//out:	return	advanced pointer to buffer

static	octet *eTIMESTRING(octet *op, octet hour, octet min, octet sec, octet hun)
{	*op++=0xB4;									//tag 11, 4 octets
	if(hour>23) hour=0xFF;
	if(min>59) min=0xFF;
	if(sec>59) sec=0xFF;
	if(hun>99) hun=0xFF;
	*op++=hour;
	*op++=min;
	*op++=sec;
	*op++=hun;
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Object Identifier Value
//in:	op		points to an octet buffer to put the results in
//		tag 	has the tag to encode (eg. 0xC0)
//		objid 	has the object identifier
//out:	return	advanced pointer to buffer

static	octet *etagOBJECTID(octet *op,octet tag,dword objid)
{	union { dword dw; byte b[4];} u;
	int		i;
    
	u.dw=objid;										//so we can get at individual bytes
	*op++=tag+4;									//tag ?, 4 octets
#if bigEndian
	for(i=0;i<4;i++)								//store all bytes ms first, ls last
		*op++=(octet)u.b[i];						//save byte
#else
	for(i=3;i>=0;i--)								//store all bytes ms first, ls last
		*op++=(octet)u.b[i];						//save byte
#endif
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Bit String
//in:	op		points to an octet buffer to put the results in
//		bp		points to the bit string to encode(MUST be BACnet-order bytes/bits)
//		tag		tag value to use, e.g. 0x80
//		nb		is the number of significant bits in the bit string
//out:	return	advanced pointer to buffer

static	octet *eBITSTRING(octet *op,byte *bp,octet tag,word nb)
{	union { word w; byte b[2];} u;
	word	i,n;

	n=((nb+7)/8)+1;									//n is the number of bytes+1
	if(n<=4)
		*op++=tag+(octet)n;							//tag ?, length
	else
	{	*op++=tag+5;								//tag ?, extended length
		if(n<=253)
			*op++=(octet)n;
		else										//assume n<=65535
		{	*op++=0xFE;
			u.w=n;
#if bigEndian
			*op++=(octet)u.b[0];					//msb of length first
			*op++=(octet)u.b[1];
#else
			*op++=(octet)u.b[1];					//msb of length first
			*op++=(octet)u.b[0];
#endif
		}	
	}
	i=((n-1)*8)-nb;
	*op++=(octet)i;									//number of unused bits
	for(i=0;i<(n-1);i++)
		*op++=*bp++;	
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Encode an Application Tagged Octet String
//in:	op		points to an octet buffer to put the results in
//		os		points to the octet string to encode
//		n		has the number of octets to encode(0-65535)
//out:	return	advanced pointer to buffer

static	octet *eOCTETSTRING(octet *op, octet *os, word n)
{	union { word w; byte b[2];} u;
	word	i;

	u.w=n;											//so we can get byte access
	if(n<=4)
		*op++=0x60+(octet)n;						//tag 6, length
	else
	{	*op++=0x65;									//tag 6, extended length
		if(n<=253)
			*op++=(octet)n;
		else										//assume n<=65535
		{	*op++=0xFE;
#if bigEndian
			*op++=(octet)u.b[0];					//msb of length first
			*op++=(octet)u.b[1];
#else
			*op++=(octet)u.b[1];					//msb of length first
			*op++=(octet)u.b[0];
#endif
		}	
	}
	for(i=0;i<n;i++)								//store all octets in order
		*op++=*os++;			
	return op;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Null Value(Tag 0)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	octet value of null
//		np 		advanced

static	octet dNULL(octet *(*np))
{
	(*np)++;										//advance pointer, trust us
	return 0;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Boolean Value(Tag 1)
//in:	op		is the address of the pointer to the tag in an octet buffer
//out:	return	true/false
//		op		advanced

static	bool dBOOL(octet *(*np))
{
	return((((*(*np)++)&7)==1) ? true:false);
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Signed Integer Value(tag 3)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	dword value
//		np		advanced

static	long dINT(octet *(*np))								//argument is passed by reference, ie. the address of a pointer
{
	return dDWORD(np,true);						//minimal decode
}

///////////////////////////////////////////////////////////////////////					***1200x Begin
//	Decode an Application Tagged Signed Integer Value(tag 3) that may exceed 4 octets
//in:	np		is the address of the pointer to an octet buffer
//		*dw		place to put a 32-bit value
//out:	return	vsbe_noerror=success, else vsbe_valueoutofrange (exceeds 32 bits)
//		np		advanced

static int dINTEx(octet *(*np), long *l)				//argument is passed by reference, ie. the address of a pointer
{	union	{dword	dw; long l;}u;
	int		err;

	err=dDWORDEx(np,true,&u.dw);						//minimal decode
	*l=u.l;
	return err;
}													//									***1200x End

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Real Value(Tag 4)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	fp real value
//		np		advanced

#ifdef FLOATLIB										//									***002 Begin
static	float dREAL(octet *(*np))
{	union { float rv; byte b[4];} u;               
	int		i;

	(*np)++;										//advance pointer, trust us
#if bigEndian
	for(i=0;i<4;i++)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;			
#else
	for(i=3;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;
#endif			
	return u.rv;
}
#else
static	dword dREAL(octet *(*np))
{	union { dword rv; byte b[4];} u;                 
	int		i;

	(*np)++;										//advance pointer, trust us
#if bigEndian
	for(i=0;i<4;i++)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;			
#else
	for(i=3;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;
#endif			
	return u.rv;
}
#endif												//									***002 End

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Double Value(Tag 5)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		vp		pointer to return the value			//									***002 Begin
//out:	np		advanced

#ifdef FLOATLIB
static	void dDOUBLE(octet *(*np),double *vp)
{	union { double rv; byte b[8];} u;
	int		i;

	(*np)++;										//advance pointer past tag, trust us
	(*np)++;										//advance pointer past 08 ext length, trust us
#if bigEndian
	for(i=0;i<8;i++)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;
#else
	for(i=7;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;
#endif
	*vp=u.rv;
}
#else
static	void dDOUBLE(octet *(*np),dword *vp)
{	union { dword rv[2]; byte b[8];} u;                 
	int		i;

	(*np)++;										//advance pointer past tag, trust us
	(*np)++;										//advance pointer past 08 ext length, trust us
#if bigEndian
	for(i=0;i<8;i++)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;
	vp[0]=u.rv[0];
	vp[1]=u.rv[1];
#else
	for(i=7;i>=0;i--)								//store all bytes, ls first, ms last
		u.b[i]=(byte)*(*np)++;
	vp[0]=u.rv[0];
	vp[1]=u.rv[1];
#endif
}
#endif												//									***002 End

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Character String(Tag 7)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		fs		points to the character string to decode into
//out:	np		advanced
//				vsbe_charactersetnotsupported -or-
//				vsbe_valueoutofrange (string exceeds the provided maxlen)

static word dCHARSTRING(octet *(*np),frString *fs)
{	
//	union { word w; byte b[2];} u;					//									***1200
	word	i,nc;
	//octet	*cs=(octet *)(fs+1);					//									***1200 
	octet *cs=(octet *)fs->text;					//									***1200
	byte charset;
	byteset((byte*)(fs->text),0x00,fs->maxlen);

	if((*(*np)&7)<=4)
		nc=(*(*np)++)&7;							//tag 6, length
	else
	{	
		*(*np)++;									//tag 6, extended length
		if(*(*np)<=253)
			nc=*(*np)++;
		else										//									***1205
			return vsbe_valueoutofrange;			//									***1205
	}
	if(nc==0) return vsbe_invalidtag;				//									***1223
	nc--;											//account for cs code	
	charset=*(*np)++;								//									***1200 Begin
	if(charset!=chrsUTF8)
	{
		// we need to increment through the data here because some services				***1221 Begin 
		// requires us to not return an error when an invalid character set is provided.
		for(i=0;i<nc;i++)							//just increment through data
			*(*np)++;								//									
		fs->len=0;									//									***1221 End
		return vsbe_charactersetnotsupported;		
	}
	if(nc>fs->maxlen)
		return vsbe_valueoutofrange;				
	if(!cs)
	{
		for(i=0;i<nc;i++)							//just increment through data
			*(*np)++;
	}
	else
	{
		for(i=0;i<nc;i++)							//store all characters in order
			*cs++=*(*np)++;
	}												//									***1200 End	
	fs->len=(byte)nc;								//									***1200
	return vsbe_noerror;							//									***1200
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Unsigned Integer Value(tag 2)
//in:	np		is the address of the pointer to the tag in an octet buffer
//out:	return	dword value
//		np		advanced

static	dword dUINT(octet *(*np))						//argument is passed by reference, ie. the address of a pointer
{
	return dDWORD(np,false);							//minimal decode
}

///////////////////////////////////////////////////////////////////////					***1200x Begin
//	Decode an Application Tagged Unsigned Integer Value(tag 2) that may exceed 4 octets
//in:	np		is the address of the pointer to an octet buffer
//		*dw		place to put a 32-bit value
//out:	return	vsbe_noerror=success, else vsbe_valueoutofrange (exceeds 32 bits)
//		np		advanced

int dUINTEx(octet *(*np), dword *dw)			//argument is passed by reference, ie. the address of a pointer
{	dword	t;
	int		err;

	err=dDWORDEx(np,false,&t);						//minimal decode
	*dw=t;
	return err;
}													//									***1200x End

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Enumerated Value(tag 9)
//in:	np		is the address of the pointer to an octet buffer
//out:	return	dword value
//		np		advanced

static	dword dENUM(octet *(*np))						//argument is passed by reference, ie. the address of a pointer
{
	return dDWORD(np,false);						//minimal decode
}

///////////////////////////////////////////////////////////////////////					***1200x Begin
//	Decode an Application Tagged Enumerated Value(tag 9) that may exceed 4 octets
//in:	np		is the address of the pointer to an octet buffer
//		*dw		place to put a 32-bit value
//out:	return	vsbe_noerror=success, else vsbe_valueoutofrange (exceeds 32 bits)
//		np		advanced

static int dENUMEx(octet *(*np), dword *dw)			//argument is passed by reference, ie. the address of a pointer
{	dword	t;
	int		err;

	err=dDWORDEx(np,false,&t);						//minimal decode
	*dw=t;
	return err;
}													//									***1200x End

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Date String(Tag 10)
//
//NOTE:	Error cases generate *don't care* values!
//		Bad dates, e.g. Feb 30 and Feb 29 for non-leap years and
//		incorrect day-of-week for a given date are not checked!
//
//in:	np		is the address of the pointer to the tag in an octet buffer
//		year	points to the actual year to retrun, e.g. 1995, or FFFF means don't care
//				(values <255 are interpreted as 1900+year)
//		month	points to the month to return, 1=Jan or FF=don't care 
//		day		points to the day to return 1-31 or FF=don't care
//		dow		points to the day of week to return, 1=Mon, 7=Sun or FF=don't care
//out:	return 	np advanced

static	void dDATESTRING(octet *(*np),word *year,octet *month,octet *day,octet *dow)
{	(*np)++;										//advance pointer, trust us
	*year=(word)*(*np)++;
	if(*year<1900) *year+=1900;						//convert large year format
	*month=*(*np)++;
	*day=*(*np)++;
	*dow=*(*np)++;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Time String(Tag 11)
//
//	NOTE:	bad values generate *don't care* !
//
//in:	np		is the address of the pointer to the tag in an octet buffer
//		hour	points to the hour 0..23 or FF=don't care
//		min		points to the minute 0..59 or FF=don't care
//		sec		points to the second 0..59 or FF=don't care
//		hun		points to hundredths 0..99 or FF=don't care
//out:	return	np advanced

static	void dTIMESTRING(octet *(*np),octet *hour,octet *min,octet *sec,octet *hun)
{	(*np)++;										//advance pointer, trust us
	*hour=*(*np)++;
	*min=*(*np)++;
	*sec=*(*np)++;
	*hun=*(*np)++;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Object Identifier Value(tag 12)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		type 	returns the decoded object type
//out:	return	instance number
//		np		advanced

static	dword dOBJECTID(octet *(*np),word *type)			//argument is passed by reference, i.e. the address of a pointer
{	
	union { dword dw; word w[2]; byte b[4];} u;
//	octet	len;							//											***003 Begin
	int		i;

	(*np)++;								//advance pointer, len should always be 4	***254 
#if bigEndian 
      for(i=0;i<4;i++)						//store all bytes ms last, ms last 
            u.b[i]=*(*np)++;				//											***254 
      *type=(u.w[0]>>6)&0x3FF;				//return device type 
#else 
      for(i=3;i>=0;i--) 
            u.b[i]=*(*np)++;				//											***254 
      *type=(u.w[1]>>6)&0x3FF;				//return device type 
#endif 

	return(u.dw&0x3FFFFFL);					//return instance							***003 End
}

/////////////////////////////////////////////////////////////////////// 
//	Decode a Context Specific Tagged Value
//in:	np		is the address of the pointer to the tag in an octet buffer
//		sgnd	true if return value is to be signed
//out:	return	dword value
//		np		advanced

static	dword dCONTEXT(octet *(*np), bool sgnd)				//argument is passed by reference, i.e. the address of a pointer
{
	return dDWORD(np,sgnd);					//minimal decode
}

///////////////////////////////////////////////////////////////////////				***1200x Begin
//	Decode an Application Tagged 4 octet minimal encoded value
//in:	np		is the address of the pointer to the tag in an octet buffer
//		sgnd	true if signed, false if unsigned
//out:	return	the dword value
//		np		advanced

static	dword dDWORD(octet *(*np),bool sgnd)				//argument is passed by reference, i.e. the address of a pointer
{	
	dword	dw;
	byte	i,n;
    
	n=(*(*np)++)&7;
	dw=((sgnd)&&((*(*np)&0x80)!=0))?0xFFFFFFFF:0;	//initialize for signed or unsigned 
	for(i=0;i<n;i++)								//this is endian proof
	{
		dw<<=8;
		dw+=(*(*np)++);
	}
	if(n>4)											//we support only 32-bit unsigned
		dw=0xFFFFFFFF;
	return dw;
}

///////////////////////////////////////////////////////////////////////
//	Decode an Application Tagged minimal encoded value that may exceed 4 octets
//in:	np		is the address of the pointer to the tag in an octet buffer
//		sgnd	true if signed, false if unsigned
//		*dw		place to put a 32-bit value
//out:	return	vsbe_noerror=success, else vsbe_valueoutofrange (exceeds 32 bits)
//		np		advanced

static int dDWORDEx(octet *(*np),bool sgnd,dword *dw)		//argument is passed by reference, ie. the address of a pointer
{	
	byte	i,n;
	int		err=vsbe_noerror;
    
	n=(*(*np)++)&7;
	*dw=((sgnd)&&((*(*np)&0x80)!=0))?0xFFFFFFFF:0;	//initialize for signed or unsigned 
	for(i=0;i<n;i++)								//this is endian proof
	{
		*dw<<=8;
		*dw+=(*(*np)++);
	}
	if(n>4)											//we support only 32-bit unsigned
		err=vsbe_valueoutofrange;
	return err;
}													//								***1200x End

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Bit String(Tag 8)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		bs		points to the bit string string to decode into
//      max     max length of bit string. This param is added to handle
//              the max string that can be copied to src (CSA 2.2.5) 
//out:	return	number of bytes decoded(0-65535)
//		np		advanced

static	word dBITSTRING(octet *(*np),byte *bs,word max)
{	
	union { word w; byte b[2];} u;
	word	i,nb;

	if((*(*np)&7)<=4)
		nb=(*(*np)++)&7;							//tag 8, length
	else
	{	*(*np)++;									//tag 8, extended length
		if(*(*np)<=253)
			nb=*(*np)++;
		else										//assume n<=65535
		{	*(*np)++;
#if bigEndian
			u.b[0]=(byte)*(*np)++;					//msb of length first
			u.b[1]=(byte)*(*np)++;  
#else
			u.b[1]=(byte)*(*np)++;					//msb of length first
			u.b[0]=(byte)*(*np)++;
#endif  
			nb=u.w;
		}	
	}
	if(nb>0)											//	***1223 Begin
	{
		nb--;											//account unused bits octet
		(*np)++;										//skip unused bits
        if(nb>max)
        {
          nb = max;
        }
		for(i=0;i<nb;i++)								//store all bytes in order
			*bs++=*(*np)++;
	}													//	***1223 End
	return nb;
}

/////////////////////////////////////////////////////////////////////// 
//	Decode an Application Tagged Octet String(Tag 6)
//in:	np		is the address of the pointer to the tag in an octet buffer
//		os		points to the octet string to decode into
//		max		has the max number of octets to decode(0-65535)
//out:	return	number of octets decoded(0-65535)
//		np		advanced

static	word dOCTETSTRING(octet *(*np),octet *os,word max)
{	
	union { word w; byte b[2];} u;
	word	i,n;

	if((*(*np)&7)<=4)
		n=(*(*np)++)&7;								//tag 6, length
	else
	{	*(*np)++;									//tag 6, extended length
		if(*(*np)<=253)
			n=*(*np)++;
		else										//assume n<=65535
		{	*(*np)++;
#if bigEndian
			u.b[0]=(byte)*(*np)++;					//msb of length first
			u.b[1]=(byte)*(*np)++;  
#else
			u.b[1]=(byte)*(*np)++;					//msb of length first
			u.b[0]=(byte)*(*np)++;
#endif  
			n=u.w;
		}	
	}
	if(n>max) n=max;
	for(i=0;i<n;i++)								//store all octets in order
		*os++=*(*np)++;
	return n;
}

///////////////////////////////////////////////////////////////////////			***214 Begin
//	Decode a primitive Datatype
//in:	np		is the address of the pointer to the tag in a primitive dtattype buffer
//		vp		points to a value bage to decode into
//out:	return	vsbe_noerror or error code
//		np		advanced

static	int dPRIMITIVE(octet *(*np),frVbag *vp)		//							***1205
{
octet	*op,t;
int		r=vsbe_noerror;								//the default
word	nc;

	op=*np;											//op points at the buffer
	t=*op&0x7;										//this better be (0-5)
	if (t>5) return vsbe_invalidtag;                //                          ***1202
	vp->pdtype=((*op)>>4)&0x0F;						//datatype
	switch(vp->pdtype)
	{	
	case adtNull:
		if (*op!=0x00) return vsbe_invalidtag;         //                       ***1202
		dNULL(&op);									//we don't need the value...
		vp->pd.uval=0;								//							***233
		break;
	case adtBoolean:
		if (*op!=0x10&&*op!=0x11) return vsbe_invalidtag;           //          ***1202
		vp->pd.uval=dBOOL(&op);
		break;
    case adtUnsigned:
//		vp->pd.uval=dUINT(&op);						//							***1200x
		r=dUINTEx(&op,&vp->pd.uval);				//							***1200x
		break;
    case adtInteger:
//		vp->pd.sval=dINT(&op);						//							***1200x
		r=dINTEx(&op,&vp->pd.sval);					//							***1200x
		break;
    case adtReal:
		if (*op!=0x44) return vsbe_invalidtag;      //                          ***1202
		vp->pd.fval=dREAL(&op);
		break;
    case adtEnumerated:
		//vp->pd.uval=dENUM(&op);					//							***1200x
		r=dENUMEx(&op,&vp->pd.uval);				//							***1200x
		break;
    case adtObjectID:
		if (*op!=0xC4) return vsbe_invalidtag;      //                          ***1202
		vp->pd.uval=dDWORD(&op,false);
		break;
    case adtDouble: 
		if (*op!=0x55&&op[1]!=0x08) return vsbe_invalidtag; //			        ***1202
		#if FLOATLIB
		dDOUBLE(&op,&vp->pd.dval);
		#else
		dDOUBLE(&op,&vp->pd.dval[0]);
		#endif
		break;
    case adtDate:
		if (*op!=0xA4) return vsbe_invalidtag;		//							***1202
		dDATESTRING(&op,&nc,&vp->pd.dtval.month,&vp->pd.dtval.day,&vp->pd.dtval.dow);
		vp->pd.dtval.year=(byte)(nc-1900);
		break;										
	case adtTime:
		if (*op!=0xB4) return vsbe_invalidtag;      //					        ***1202
		dTIMESTRING(&op,&vp->pd.tval.hour,&vp->pd.tval.minute,&vp->pd.tval.second,&vp->pd.tval.hundredths);
		break;
    case adtCharString:
		vp->narray=0;
		buildvbString(vp);							//							***1205
		r=dCHARSTRING(&op,&vp->pd.stval);
		//dCHARSTRING(&op,&vp->pd.stval,sizeof(vp->ps.psval));
		break;
    case adtOctetString:
		vp->pd.uval=dOCTETSTRING(&op,vp->ps.psval,sizeof(vp->ps.psval));
		break;
    case adtBitString:
		if((*op&0x0F)==0) return vsbe_invalidtag;	//							***1223
		vp->pd.uval=dBITSTRING(&op,vp->ps.psval,sizeof(vp->ps.psval));
		break;
     default:
		r=vsbe_invaliddatatype;
	}
	*np=op;											//advance the pointer
	return r;
}

///////////////////////////////////////////////////////////////////////	
//	Check whether requect came from trusted client. BACnet whitlisting LTK-3135
//in:	am		Has the prepared request
//out:	return	true: IP is trusted false: IP is untrusted
// PX Green Modified - LTK-3872
static bool Is_Request_Trusted (frNpacket *am )
{
	bool is_ip_trusted = true;
	uint32_t ip_addr = 0;
	int32_t	pdutype;
	uint16_t bacnet_port;
	
	// PX Green Modified - LTK-3872
	bytecpy((byte *)&ip_addr,(byte *)(am->src.sadr),4);		//am->src.sadr has big endian ip address

	//LTK-4874: Pass destination port number to trusted ip layer
	bacnet_port = di.bipudp;
	
	//Check if request came from trusted client
	is_ip_trusted = Trusted_IP_Filter::Is_Remote_Ip_Trusted( ip_addr, bacnet_port );

	//Check request type is Confirmed or Unconfirmed
	pdutype=((am->apdu[0]&ASDUpdutype)>>ASDUpdutypeshr); //get the pdu type field

	if ( is_ip_trusted == false )
	{
		if( pdutype != UNCONF_REQ_PDU )
		{
			//Send Error reponse to untrusted confirmed request
			SendError( am, am->invokeid, am->svc, frGetErrorClass(vsbe_servicerequestdenied), 
					   frGetErrorCode(vsbe_servicerequestdenied));
		}
		else
		{
			//Unconfirmed BACnet request came from untrusted client
			//Do not send Error reponse to untrusted unconfirmed request
		}
	}
	else
	{
		//Request came from trusted client
		// Misra
	}

	return is_ip_trusted;
}

///////////////////////////////////////////////////////////////////////			***1205 Begin
//	Build an frString in an frVbag
//in:	vb				points to an frVbag
//		vb->pd.stval	will get the resulting frString
//		vb->ps.psval	contains the string to use (max len is 255!)

void bpublic buildvbString(frVbag *vb)
{
frString *fp=&vb->pd.stval;

	fp->text=(char*)vb->ps.psval;							//point to psval where text is
	fp->len=(byte)strlen((char *)vb->ps.psval);
	fp->maxlen=255;
	fp->readonly=false;
}													//							***1205 End

///////////////////////////////////////////////////////////////////////			***1205 Begin
//	Max Master
// Function is used to updated the value of Max Master when changed using Keypad

void bpublic maxMaster(byte maxmaster)
{
   di.maxmaster = maxmaster;
}
