/*--------------------------------------------------------------------
	©1998-2016, PolarSoft® Inc., All Rights Reserved

module:		MSTP.C
desc:		Minimal BACnet MS/TP Driver for Embedding
author:		David M. Fisher, Jack Neyer, Coleman L. Brumley
last edit: 	16-Feb-2016 [1223] CLB Modified SendFrame to send data packets, too.
								   Only send pad in mstpTransmit if mstpNeedsPad is 1
								   Cleaned up comments.
			01-Jun-2015 [1220] CLB Correct slave mode mstpTransmit issue
								   Set ReplyTimer to 0 in SlaveMode correctly
								   Correct snsmIdle reset in SlaveMode correctly
			19-Nov-2014 [1215] CLB Added rpWait "semaphore"
			18-Nov-2014 [1214] CLB Relaxed Treplydelay constant
								   Avoid race condition when queuing Answer
			07-Aug-2013 [1201] CLB init maxapdu accordingly
			04-Apr-2013	[1200] CLB Added slave node functionality
								   removed myIAm
			28-Feb-2013 [ 266] CLB Added GetMoreData to RFSM for "Not For Us" in HEADER_CRC
								   don't allow SendFrame to send to ourselves
			11-Dec-2009 [ 265] CLB changes to avoid collisions
								   make sure mflags is set/reset correctly
								   perform header not for us checks
			21-Oct-2009 [ 264] CLB Added mstpDoReplyPostponed
								   Relaxed the Tusagetimeout variables
			26-Mar-2009 [ 263] CLB set mftBACnetDataExpectingReply before calling macReceiveIndicate
								   handle not for us case in the HEADER_CRC state to increase speed
			25-Oct-2007 [ 257] CLB fix maxapdu calculation
			12-Jun-2006 [ 252] CLB Linux semaphore implementation
			25-Jul-2005 [ 240] DMF revise for VSBHP
----------------------------------------------------------------------*/
//#include <malloc.h>
#include "Stdlib.h"
#include "includes.h"
#include "mac.h"								//										***200
#include "mstp.h"
#include "napdu.h"
#include "semaphore.h"							//										***240
#include "Ram.h"
#ifndef _WINDOWS
//#include <pthread.h>
#endif

/*SRP: Added to interface the RS485 driver with BACnet MS/TP*/
#include "BACnetUARTInterface.h"
extern BACnet_UART* bacnet_uart_for_485;
extern UINT8 recvBuf[rxmax];
extern UINT16 recvBufindex;
extern UINT16 recvBufCount;
UINT16 dispatchIndex;
BOOL isSoleMaster = TRUE;

bool Slave_SABus_CommTOut_flag = false;

///////////////////////////////////////////////////////////////////////
//	Module Options
//#define	keepframecount		1				//uncomment to keep frame count statistics

///////////////////////////////////////////////////////////////////////
//	Module Constants

#define		Npoll				50				//number of tokens received or used before Poll for Master (fixed)
#define		Nretrytoken			1				//number of retries on sending the token (fixed)
#define		Nminoctets			4				//number of "events (octets) for active line (fixed)
#define		Tframeabort			100				//(60 bit times) 100 ms max (fixed)
#define		Tframegap			20				//(20 bit times) (fixed)
#define		Tnotoken			500				//silence time for loss of token (fixed)
#define		Tpostdrive			15				//(15 bit times) (fixed)
//Note:		Normally, Treplydelay is 250ms, this being the time we have to reply to a request	***240
//			that requires confirmed reply. However, this implementation sets the value at a
//			much lower number and makes the decision to reply postponed if the answer is
//			not available quickly.
//#define		Treplydelay			250				//250 ms (fixed)
#define		Treplydelay			120				//much lower than normal threshold for reply postponed		***1214
#define		Treplytimeout		300				//255-300 ms (fixed)
#define		Tslot				10				//10 ms		 (fixed)
#define		Tturnaround			5				//40 bit times (for 9600 baud 4.1 millisecs) (fixed)
#define		TusagetimeoutTP		50				//40ms
#define		TusagetimeoutPFM	25				//25ms

//possible flags
#define		receivedinvalidframe	4
#define		receivedvalidframe		8
#define		solemaster				16

///////////////////////////////////////////////////////////////////////
//	Module variables
static	int			Nmaxinfoframes=1;			//number of frames to send before passing token
												//Max_Info_Frames Property of the DeviceObject
static	octet		Nmaxmaster=127;				//Max_Master Property of the DeviceObject
static	octet		mstpTS=0;					//this station
static	byte		lrstate=lrsmIdle;			//receive frame state machine state
static	byte		mnstate=mnsmInitialize;		//master node state machine state
static  byte		snstate=snsmInitialize;		//slave node state machine state		***1200
static  byte		mode=MasterMode;			//default mode is master				***1200
static	word		dcrc=0;						//data CRC
static	word		maxapdu=mstpmaxapdu;		//										***1201
static	word		hindex=0;					//header index
static	octet		hcrc=0;						// header CRC
static	byte		mflag=0;					//various flags:
static	octet		hb[6];						//header buffer for  receive			***006
												// [0]=type
												// [1]=D.A.
												// [2]=S.A.
												// [3]=lenhi
												// [4]=lenlo
												// [5]=crc
static	int			eventcount=0;
static	int			framecount=0;				//frames sent during token hold
static	octet		ns=0;						//next station to get token
static	octet		ps=0;						//last station to which we send Poll for Master
static	int			ReplyTimer=0;
static	int			retrycount=0;
volatile int		SilenceTimer=0;
static	int			tokencount=0;				//tokens received by us
static	word		rlen=0;
static	octet		rbuf[maxrx];
static	word		elen=0;
static	word		TxQcount;					//count of tx waiting sending			***240 Begin
static	mstpFrame	*TxQfront;					//front and rear queue pointers
static	mstpFrame	*TxQrear;
static	mstpFrame	*Answer;
static	mstpFrame	*mpfree=NULL;				//list of reserved transmit buffers
static	octet		rpWait=0;					//flag indicating we've sent an RP		***1215
#ifndef _WINDOWS
static	semaphore       mpmutex ={NULL};       		// access semaphore for mpfree ***252  ***240 End
#else
static	semaphore		mpmutex;
#endif

#ifdef keepframecount
static	dword		validframecount=0;					//frame counters (optional)
static	dword		invalidframecount=0;
static	dword		unwantedframecount=0;
#endif

///////////////////////////////////////////////////////////////////////
//	function prototypes

static	word		CalcDataCRC(octet,word);
static	octet		CalcHeaderCRC(octet,octet);
static	mstpFrame 	*GetTxQ(void);						//								***240
static	void		MasterNodeStateMachine(void);
static	void		mpFreePacket(mstpFrame *);			//								***240
static	mstpFrame	*mpGetPacket(void);					//								***240
static	void		ReceiveFrameStateMachine(void);
static	bool		SendFrame(int ft,octet da,word len,octet *buf);					//	***1223
static  void		SlaveNodeStateMachine(void);		//								***1200

///////////////////////////////////////////////////////////////////////
//	initialize MS/TP subsystem

bpublic void mstpInit(frDevInfo *di)					//								***233
{
	mstpFrame	*mp;							//										***240 Begin
	word	i;

	//semaClear(&mpmutex);
	semaCapture(&mpmutex);						//capture mp semaphore
	for(i=0;i<mpreserve;i++)
	{	//if((mp=(mstpFrame *)malloc(sizeof(mstpFrame)))==NULL)
	    if((mp=(mstpFrame *)Ram::Allocate((size_t)sizeof(mstpFrame)))==NULL)
			break;								//FAILED to get all the packets we need
		mp->next=mpfree;						//link onto free list
		mpfree=mp;
	}
	TxQfront=TxQrear=Answer=NULL;
	TxQcount=0;
	semaRelease(&mpmutex);						//release mp's semaphore				***240 End

	if(di->maxmaster==0)						//										***1200 Begin
		mode=SlaveMode;
	if((mode==MasterMode)&&(di->mstpstation>127))
		mstpTS=di->mstpstation%127;					//force <= 127
	else
		mstpTS=di->mstpstation;						//this station						***233
	if((di->maxmaster<mstpTS)&&(mode==MasterMode))
		Nmaxmaster=mstpTS+1;
	else
		Nmaxmaster=di->maxmaster;				//										***233
	if(mode==SlaveMode)
		Nmaxinfoframes=1;
	else
		Nmaxinfoframes=di->maxinfoframes;		//										***1200 End

	rpWait=0;									//										***1215
	maxapdu=mstpmaxapdu;						//										***1201
}

///////////////////////////////////////////////////////////////////////					***233 Begin
//	Get MAC address
//
//in:	p		points to buffer for it
//out:	length

bpublic byte mstpGetAddr(octet *p)
{
	*p=mstpTS;
	return 1;
}										//												***233 End

///////////////////////////////////////////////////////////////////////					***004 Begin
//	get max apdu length
//
//out:	the length

bpublic word mstpMaxAPDU(void)
{
	return maxapdu;
}										//												***004 End

///////////////////////////////////////////////////////////////////////					***004 Begin
//	get max response pdu length
//
//out:	the length

bpublic word mstpMaxRPDU(void)
{
	return maxtx-13;					//response could include hopcount so it is 13 instead of 12!
}										//												***004 End

///////////////////////////////////////////////////////////////////////					***233 Begin
//	get max npdu length
//
//out:	the length

bpublic word mstpMaxNPDU(void)
{
	return maxtx;
}										//												***233 End

///////////////////////////////////////////////////////////////////////
//	try to transmit data (like a DL-UNITDATA.request)
//
//in:	da				the destination mstp address to send to
//		dlen			the length
//		tp				has the data to transmit										***240
//		reqflag			macisRequest, macisResponse, or macisUnconfirmed				***240

bpublic void mstpTransmit(octet da, word dlen, octet *tp, byte reqflag)	//				***240 Begin
{	mstpFrame	*fp,*lg;						//for easy access
	octet		*p,tcrc=0xFF;
	union {	word dw; octet db[2]; }u;
	word	i;

	if(dlen>maxtx)
		return;										//can't do it
	if((fp=mpGetPacket())==NULL)
		return;										//can't get a transmit buffer

	fp->pre[0]=0x55;								//preamble
	fp->pre[1]=0xFF;								//preamble
	if(reqflag==macisRequest)
		fp->type=mftBACnetDataExpectingReply;
	else
		fp->type=mftBACnetDataNotExpectingReply;
	tcrc=CalcHeaderCRC(fp->type,tcrc);
	fp->da=da;
	tcrc=CalcHeaderCRC(da,tcrc);
	fp->sa=mstpTS;									//sa always=mstpTS
	tcrc=CalcHeaderCRC(mstpTS,tcrc);
	u.dw=dlen;
#if bigEndian
	fp->dlenhi=u.db[0];
	tcrc=CalcHeaderCRC(fp->dlenhi,tcrc);
	fp->dlenlo=u.db[1];
	tcrc=CalcHeaderCRC(fp->dlenlo,tcrc);
#else
	fp->dlenhi=u.db[1];
	tcrc=CalcHeaderCRC(fp->dlenhi,tcrc);
	fp->dlenlo=u.db[0];
	tcrc=CalcHeaderCRC(fp->dlenlo,tcrc);
#endif
	fp->hcrc=(~tcrc);
	p=fp->data;
	if(dlen!=0)
	{	u.dw=0xFFFF;								//data CRC is a word
		for(i=0;i<dlen;i++)							//calculate CRC for message
		{	*p=tp[i];
			u.dw=CalcDataCRC(*p++,u.dw);
		}
#if bigEndian
		*p++=(~u.db[1]);							//stuff CRC bytes
		*p++=(~u.db[0]);
#else
		*p++=(~u.db[0]);							//stuff CRC bytes
		*p++=(~u.db[1]);
#endif
		dlen+=2;
	}

#if mstpNeedsPad																				//		***1223 Begin
	*p=0xFF;										//pad
	dlen+=9;										//account for preamble,hdr and pad
#else
	dlen+=8;										//account for preamble,hdr
#endif
	fp->plen=dlen;									//account for preamble,hdr and pad			//		***1223 End
	semaCapture(&mpmutex);							//capture mp's semaphore
	if(mode==MasterMode)							//													***1220 Begin
	{
		if((rpWait==0)						 &&		//we're not waiting for ReplyPostponed to clear		***1215
			(mnstate==mnsmAnswerDataRequest) &&		//we're answering a data request, and
			(reqflag==macisResponse)		 &&		//this is a response, and
			(Answer==NULL)					 &&		//we don't have a pending answer to go out			***1214
			(ReplyTimer<(Treplydelay-10)))			//are we close to sending a reply postponed?		***1214
		{
			Answer=fp;								//remember this one needs to go out first;
		}
		else										//in all other cases, queue it
		{
			lg=TxQrear;								//lg is the last guy in present queue
			if(lg==NULL)							//p is the first and only guy
				TxQfront=fp;						//so front also points to him
			else									//in all other cases, rear guy must point to new guy
				lg->next=fp;
			TxQrear=fp;								//new guy is now the last one
			fp->next=NULL;
			TxQcount++;
		}
	}
	else if(mode==SlaveMode)
	{
		if((snstate==snsmAnswerDataRequest)	&&		//we're answering a data request
		   (reqflag==macisResponse)			&&		//this is a response, and
		   (Answer==NULL)					&&		//we don't have a pending answer to go out
		   (ReplyTimer<(Treplydelay-10)))			//are we close to returning to IDLE?
		{
			Answer=fp;								//here is our answer
		}
	}												//													***1220 End
	semaRelease(&mpmutex);							//release mp's semaphore
}													//													***240 End

///////////////////////////////////////////////////////////////////////
//	Main Loop (call this every 5ms)
//
//in:	actualperiod	the time in milliseconds that mstpWork actually
//						gets called. Ideally it should be every 5ms, but
//						if the best you can do is say 8ms then actualperiod=8.

bpublic void mstpWork(byte actualperiod)
{
	SilenceTimer+=actualperiod;
	ReplyTimer+=actualperiod;
	ReceiveFrameStateMachine();
	if((mflag&receivedinvalidframe)&&(SilenceTimer==0))				//	***265 Begin
	{
		//not evaluating the MNSM at this point avoids line
		//contention if we've just received an invalid frame
		return;	//just ignore the bad message to sync the MNSM states
	}																//	***265 End
	if(mode==MasterMode)
		MasterNodeStateMachine();
	else if(mode==SlaveMode)
		SlaveNodeStateMachine();
}

///////////////////////////////////////////////////////////////////////	***264 Begin
//	Do Reply Postponed
//
// This tells the MS/TP state machine to issue a reply postponed frame
// instead of continuing to wait for a response from the upper layers

bpublic void mstpDoReplyPostponed(void)
{
	ReplyTimer=Treplydelay+1;
}																	//	***264 End

///////////////////////////////////////////////////////////////////////
//	Receive Frame State Machine
//
// Note: When a complete frame has been received, this machine leaves the
//       recieved frame in hb and rbuf with receivedvalidframe==true OR
//       receivedinvalidframe==true. It's then up to MasterNodeStateMachine
//		 to process the frame.

static void ReceiveFrameStateMachine(void)
{	int		c;

	while(true)
	{
		if((SilenceTimer>Tframeabort)&&(lrstate!=lrsmIdle))			//timed out waiting
			goto ResetToIDLEx;
GetMoreData:													//					***266
		if ((c=mstpSerialRx())==-1) break;		//no characters available, quit now	***240
		if (c<0)								//some error occurred				***240
		{
			eventcount++;
ResetToIDLEx:
			if ((lrstate==lrsmHeader)||(lrstate==lrsmData))
			{
				mflag&=~receivedvalidframe;									//	***265
				mflag|=receivedinvalidframe;
			}
ResetToIDLE:
			lrstate=lrsmIdle;					//reset our state to IDLE
			hindex=0;
			SilenceTimer=0;
			break;
		}
		switch(lrstate)							//handle it based on present state
		{
//[IDLE]---------------------------------------------------------------------------
		case lrsmIdle:
			SilenceTimer=0;
			eventcount++;
			if (c==0x55)						//preamble start
				lrstate=lrsmPreamble;			//PREAMBLE state
			hindex=0;							//not a preamble start
			break;
//[PREAMBLE]-----------------------------------------------------------------------
		case lrsmPreamble:
			SilenceTimer=0;
			eventcount++;
			if(c==0x55)
				break;							//preamble was repeated
			if (c!=0xFF)						//not the second preamble octet
				goto ResetToIDLE;
			hcrc=0xFF;
			hindex=0;
			lrstate=lrsmHeader;					//HEADER state
			break;
//[HEADER]-----------------------------------------------------------------------
		case lrsmHeader:
			SilenceTimer=0;
			eventcount++;
			hb[hindex]=(octet)(c&0xFF);
			hcrc=CalcHeaderCRC(hb[hindex++],hcrc);
			if(hindex==6)						//got a header
			{
//[HEADER_CRC]-----------------------------------------------------------------------
				if(hcrc!=0x55)
					goto ResetToIDLEx;			//bad header CRC						***265
				rlen=(hb[3]<<8)+hb[4];			//get data length
				if ((hb[1]!=0xFF)&&(hb[1]!=mstpTS))	//Not for us						***266 Begin
                {
                    lrstate=lrsmIdle;
                    hindex=0;
                    SilenceTimer=0;
                    goto GetMoreData;
                }									//									***266 End
				if(rlen>sizeof(rbuf))			//Received a too large frame
					goto ResetToIDLEx;					//								***265
				if(rlen==0)
				{
					mflag&=~receivedinvalidframe;		//								***265
					mflag|=receivedvalidframe;			//								***265
					goto ResetToIDLE;
				}
				hindex=0;
				dcrc=0xFFFF;
				elen=rlen+2;					//data+2 CRC octets
				lrstate=lrsmData;				//DATA state
			}
			break;
//[DATA]-----------------------------------------------------------------------
		case lrsmData:
			SilenceTimer=0;
			eventcount++;
			rbuf[hindex]=(octet)(c&0xFF);
			dcrc=CalcDataCRC(rbuf[hindex++],dcrc);
			if(hindex==elen)					//got a complete frame
			{
//[DATA_CRC]-----------------------------------------------------------------------
				if(dcrc==0xF0B8)
				{
					mflag&=~receivedinvalidframe;		//								***265
					mflag|=receivedvalidframe;			//								***265
					goto ResetToIDLE;
				}
				else							//BAD CRC on received frame
					goto ResetToIDLEx;
			}
			break;
//---------------------------------------------------------------------------------
		}
	}
}



///////////////////////////////////////////////////////////////////////
//	Master Node State Machine

static void MasterNodeStateMachine(void)
{	octet		tsplus1,psplus1,nsplus1;
	mstpFrame	*fp;							//										***240

    if ( mflag&solemaster )
     {
       isSoleMaster = TRUE;
     }
     else
     {
       isSoleMaster = FALSE;
     }

	switch (mnstate)							//handle it based on present state
	{
//[INITIALIZE]----------------------------------------------------------------------
	case mnsmInitialize:						//master node initialize
		ns=mstpTS;								//next station is unknown
		ps=mstpTS;
		tokencount=Npoll;						//so we Poll for Master when we receive token
		mflag&=~solemaster;
		mnstate=mnsmIdle;
		//intentionally fall into next state

//[IDLE]----------------------------------------------------------------------
	case mnsmIdle:								//wait for a frame
idle:	if (mflag&receivedinvalidframe)			//ReceivedInvalidFrame
		{
#ifdef keepframecount
			invalidframecount++;
#endif
			mflag&=~receivedinvalidframe;
			mnstate=mnsmIdle;
		}
		else if (mflag&receivedvalidframe)		//we saw a valid frame
		{
#ifdef keepframecount
			validframecount++;
#endif
			if (hb[1]==0xFF)					//we have a broadcast
			{	if(hb[0]==mftBACnetDataNotExpectingReply)
					goto ReceivedDataNoReply;
ReceivedUnwantedFrame:
#ifdef keepframecount
				unwantedframecount++;			//ReceivedUnwantedFrame
#endif
				mnstate=mnsmIdle;
			}
			else if (hb[1]==mstpTS)				//it's for us
			{	switch (hb[0])
				{
				case mftToken:					//ReceivedToken
					framecount=0;
					mflag&=~solemaster;
					mnstate=mnsmUseToken;
					mflag&=~receivedvalidframe;	//needed because we no longer break
					goto usetoken;
//					break;
				case mftPollForMaster:			//ReceivedPFM
					SendFrame(mftReplyToPollForMaster,hb[2],0,NULL);
					mnstate=mnsmIdle;
					break;
				case mftTestRequest:
					//SendFrame(mftTestResponse,hb[2],rlen,rbuf);
					if(!SendFrame(mftTestResponse,hb[2],rlen,rbuf))
						SendFrame(mftTestResponse,hb[2],0,NULL);	//we couldn't get a buffer so return empty TestResponse
					mnstate=mnsmIdle;
					break;
				case mftBACnetDataNotExpectingReply:
ReceivedDataNoReply:
					mnstate=mnsmIdle;											//		***263
					if(rlen)					//if no data, there's nothing to do...
						macReceiveIndicate(0,portMSTP,1,&hb[2],rlen,rbuf);		//		***200
					//mnstate=mnsmIdle;											//		***263
					break;
				case mftBACnetDataExpectingReply:	//ReceivedDataNeedingReply
/*					if(rlen)					//if no data, there's nothing to do...	***263 Begin
						macReceiveIndicate(1,portMSTP,1,&hb[2],rlen,rbuf);		//		***200
					ReplyTimer=0;
					mnstate=mnsmAnswerDataRequest;
*/
					ReplyTimer=0;
					if(rlen)					//if no data, there's nothing to do...
					{	mnstate=mnsmAnswerDataRequest;
						mflag&=~receivedvalidframe;
						macReceiveIndicate(1,portMSTP,1,&hb[2],rlen,rbuf);		//this may call mstpWork during processing
						return;					//do not break here...we may have recursively called mstpWork from macReceiveIndicate
					}
					else
						mnstate=mnsmIdle;		//										***263 End
					break;
				default:
					goto ReceivedUnwantedFrame;
				}
			}
			mflag&=~receivedvalidframe;
		}
		else if (SilenceTimer>Tnotoken)			//LostToken
		{	mnstate=mnsmNoToken;
			goto notoken;
		}
		break;

//[USE_TOKEN]----------------------------------------------------------------------
	case mnsmUseToken:							//we have the token, send some frames
usetoken:
		if(TxQcount)							//we need to send a request-pdu			***240 Begin
		{
			fp=GetTxQ();						//get one
			if(fp->type==mftBACnetDataExpectingReply)
				mnstate=mnsmWaitForReply;		//SendAndWait
			else
				mnstate=mnsmDoneWithToken;		//SendNoWait
			while (true)
				if(mstpSerialTxBuf(fp->pre,fp->plen)==false) break;	//send everything
			SilenceTimer=0;
			framecount++;
			mpFreePacket(fp);					//release its space
			if(TxQcount==0)						//										***1215
				rpWait=0;						//we've sent everything, clear rpWait	***1215
			break;								//										***009
		}
		else
		{
			framecount=Nmaxinfoframes;			//NothingToSend
			mnstate=mnsmDoneWithToken;
			goto donewithtoken;
		}
		break;

//--------------------------------------------------------------------------------------
// NOTE:	The WAIT_FOR_REPLY state is only needed in instances where the
//			master node needs to initiate a request that expects a reply.
//			In this implementation, there are no cases where this occurs,
//			so the state is commented out. If you add code to the USE_TOKEN
//			state which causes a mftBACnetDataExpectingReply frame to be sent
//			then the state must change to WAIT_FOR_REPLY and the following
//			code must be added back in:
//[WAIT_FOR_REPLY]---------------------------------------------------------------------- //***005 Begin

	case mnsmWaitForReply:						//wait for a reply from another node
		if (SilenceTimer>=Treplytimeout)
		{	framecount=Nmaxinfoframes;			//ReplyTimeout
			mnstate=mnsmDoneWithToken;
		}
		else if (mflag&receivedinvalidframe)		//InvalidFrame
		{
#ifdef keepframecount
			invalidframecount++;
#endif
			mflag&=~receivedinvalidframe;
			mnstate=mnsmDoneWithToken;
		}
		else if (mflag&receivedvalidframe)		//we saw a valid frame
		{
#ifdef keepframecount
			validframecount++;
#endif
			if (hb[1]==mstpTS)					//it's for us
			{	switch (hb[0])
				{
				case mftBACnetDataNotExpectingReply:
												//ReceivedReply
					macReceiveIndicate(0,portMSTP,1,&hb[2],rlen,rbuf);	//				***200
					mnstate=mnsmDoneWithToken;
					break;
				case mftReplyPostponed:
					mnstate=mnsmDoneWithToken;	//ReceivedPostpone
					break;
				default:
ReceivedUnexpectedFrame:
#ifdef keepframecount
					unwantedframecount++;		//ReceivedUnexpectedFrame
#endif
					mnstate=mnsmIdle;
					break;
				}
			}
			else //(if ((hb[1]==0xFF)||(hb[1]!=mstpTS)) //we have a broadcast or not for us
				goto ReceivedUnexpectedFrame;
			mflag&=~receivedvalidframe;
		}
		break;
//																						***005 End
//------------------------------------------------------------------------------------

//[DONE_WITH_TOKEN]-------------------------------------------------------------------	***231 Begin
	case mnsmDoneWithToken:						//send another frame, pass the token or poll for a master
donewithtoken:
		if (framecount<Nmaxinfoframes)
		{	mnstate=mnsmUseToken;				//SendAnotherFrame
			ReceiveFrameStateMachine();			//this is required or we drop bytes
			goto usetoken;
		}
		tsplus1=(mstpTS!=Nmaxmaster)?mstpTS+1:0;
		if (ns==tsplus1) goto sendtoken;		//(normally we pass token to mstpTS+1)
		if (tokencount<Npoll)
		{	if (mflag&solemaster)
			{	framecount=0;					//SoleMaster
				//tokencount++;					//												***245
				tokencount=Npoll;				//do ths so we try only once and not 50 times	***245
				mnstate=mnsmUseToken;
				goto usetoken;
			}
			else								//we're not the sole master
				goto sendtoken;					//SendToken
		}
		else //(tokencount>=Npoll)
		{	psplus1=(ps!=Nmaxmaster)?ps+1:0;
			if (psplus1!=ns)
			{	ps=psplus1;						//SendMaintenancePFM
				goto sendpfm;
			}
			else //(psplus1==ns)
			{
				if((mflag&solemaster)==0)
				{	ps=mstpTS;					//ResetMaintenancePFM
					tokencount=0;
					goto sendtoken2;			//we know who to send the token to
sendtoken:			tokencount++;
sendtoken2:			retrycount=0;
					eventcount=0;
					if(ns==mstpTS) goto dwtrestart;	//can't pass to ourselves, restart PFM
					SendFrame(mftToken,ns,0,NULL);
					mnstate=mnsmPassToken;
				}
				else
				{
dwtrestart:			nsplus1=(ns!=Nmaxmaster)?ns+1:0;	//SoleMasterRestartMaintenancePFM
					ps=nsplus1;
					ns=mstpTS;					//no known successor
					tokencount=0;
					eventcount=0;
sendpfm:			retrycount=0;
					SendFrame(mftPollForMaster,ps,0,NULL);	//we have to find a node to send the token to
					mnstate=mnsmPollForMaster;
				}
			}
		}
		break;									//										***231 End

//[PASS_TOKEN]----------------------------------------------------------------------
	case mnsmPassToken:							//listen for a successor to begin using the token we just passed
		if ((SilenceTimer<TusagetimeoutTP)&&(eventcount>Nminoctets))	//				***240
		{	mnstate=mnsmIdle;					//SawTokenUser
			goto idle;
		}
		if (SilenceTimer>=TusagetimeoutTP)		//										***240
		{	if (retrycount<Nretrytoken)
			{	retrycount++;					//RetrySendToken
				SendFrame(mftToken,ns,0,NULL);
				eventcount=0;
			}
			else
			{	nsplus1=(ns!=Nmaxmaster)?ns+1:0;	//FindNewSuccessor
				ps=nsplus1;
				SendFrame(mftPollForMaster,ps,0,NULL);
				ns=mstpTS;					//no known successor
				retrycount=0;
				tokencount=0;
				eventcount=0;
				mnstate=mnsmPollForMaster;
			}
		}
		break;

//[NO_TOKEN]----------------------------------------------------------------------
	case mnsmNoToken:							//the line has been silent, wait some more to see if we can create a token
notoken:
		if (SilenceTimer>=(Tnotoken+(Tslot*mstpTS)))	//&&(SilenceTimer<(Tnotoken+(Tslot*(mstpTS+1)))))
		{	tsplus1=(mstpTS!=Nmaxmaster)?mstpTS+1:0;	//GenerateToken
			ps=tsplus1;
			SendFrame(mftPollForMaster,ps,0,NULL);
			ns=mstpTS;							//next station is unknown
			tokencount=0;
			retrycount=0;
			eventcount=0;
			mnstate=mnsmPollForMaster;
			goto pfm;
		}
		else if (eventcount>Nminoctets)
		{	mnstate=mnsmIdle;					//SawFrame
//			goto idle;							//										***003
		}
		break;

//[POLL_FOR_MASTER]----------------------------------------------------------------------
	case mnsmPollForMaster:						//listen for reply to Poll for Master to find successor node
pfm:	if (mflag&receivedvalidframe)
		{
#ifdef keepframecount
			validframecount++;
#endif
			if ((hb[1]==mstpTS)&&(hb[0]==mftReplyToPollForMaster))
			{	ns=hb[2];						//ReceivedReplyToPFM
				SendFrame(mftToken,ns,0,NULL);
				ps=mstpTS;
				tokencount=0;
				retrycount=0;
				mflag&=~solemaster;
				mnstate=mnsmPassToken;
			}
			else
			{
#ifdef keepframecount
				unwantedframecount++;			//ReceivedUnexpectedFrame
#endif
				mnstate=mnsmIdle;				//this will drop the token
			}
			mflag&=~receivedvalidframe;
		}
		else if (mflag&receivedinvalidframe)
		{
#ifdef keepframecount
			invalidframecount++;
#endif
			mflag&=~receivedinvalidframe;
			goto pfmfailed;
		}
		else if (SilenceTimer>=TusagetimeoutPFM)	//									***240
		{
pfmfailed:
			if (mflag&solemaster)
			{	framecount=0;					//SoleMaster
				mnstate=mnsmUseToken;
				goto usetoken;					//this saves actualperiod(=5) msec		***245
			}
			else
			{	if (ns!=mstpTS)
				{	eventcount=0;				//DoneWithPFM
					SendFrame(mftToken,ns,0,NULL);
					retrycount=0;
					mnstate=mnsmPassToken;
				}
				else
				{	psplus1=(ps!=Nmaxmaster)?ps+1:0;
					if (psplus1!=mstpTS)
					{	ps=psplus1;				//SendNextPFM
						SendFrame(mftPollForMaster,ps,0,NULL);
						retrycount=0;
					}
					else	//(psplus1==mstpTS)
					{	mflag|=solemaster;		//DeclareSoleMaster
						framecount=0;
						mnstate=mnsmUseToken;
						goto usetoken;			//this saves actualperiod(=5) msec		***245
					}
				}
			}
		}
		break;

//[ANSWER_DATA_REQUEST]----------------------------------------------------------------------
	case mnsmAnswerDataRequest:					//we received a request that requires an answer
		if((Answer!=NULL)&&						//we have one to send					***240 Begin
		   (rpWait==0))							//and we're not waiting on rpWait		***1215
		{
			while (true)
			{
				if (mstpSerialTxBuf(Answer->pre,Answer->plen)==false)
					break;
			}
			SilenceTimer=0;
			mpFreePacket(Answer);				//give this one back
			Answer=NULL;
			mnstate=mnsmIdle;
			goto idle;							//										***240 End
		}
		else if ((ReplyTimer>Treplydelay) ||
			     (rpWait==1))													//		***1215 Begin
		{
			//once we've determined we exceeded Treplydelay for one reply				***1215 Begin
			//only send ReplyPostponed for DER until the TxQ has been
			//emptied. This forces all responses to go into TxQ
			//instead of the Answer slot.
			//rpWait gets cleared once we get into USE_TOKEN
			//and we've completely emptied our TxQ
			rpWait=1;									//								***1215 End
			SendFrame(mftReplyPostponed,hb[2],0,NULL);	//DeferredReply
			mnstate=mnsmIdle;
			goto idle;
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////					***1200 Begin
//	Slave Node State Machine
static void SlaveNodeStateMachine(void)
{
	static bool first_Req_DataExpectResponse = true;
    static bool Start_SABusTout_Ctr_flag = false;

	switch(snstate)
	{
	case snsmInitialize:
		mflag&=~receivedinvalidframe;
		mflag&=~receivedvalidframe;
		snstate=snsmIdle;
		break;
	case snsmIdle:
		if(mflag&receivedinvalidframe)
		{
			mflag&=~receivedinvalidframe;
			snstate=snsmIdle;
		}
		if(mflag&receivedvalidframe)
		{
			//ReceivedUnwantedFrame
			if((hb[1]!=mstpTS) &&
			   (hb[1]!=0xFF))					//(a)
			{
ResetSNIdle:
				mflag&=~receivedvalidframe;
				snstate=snsmIdle;
				break;
			}
#ifdef ESP32_SETUP
			if(((hb[1]==0xFF) &&
			   (hb[0]==mftBACnetDataExpectingReply)) ||
			   (hb[0]==mftTestRequest))			// For this compiler is giving error like(suggest parentheses around '&&' within '||' [-Werror=parentheses])
#else
			if ( ( hb[1] == 0xFF ) &&
				( hb[0] == mftBACnetDataExpectingReply ) ||
				( hb[0] == mftTestRequest ) )   //(b)
#endif
			{
				goto ResetSNIdle;
			}
			if((hb[0]==mftToken) ||
			   (hb[0]==mftPollForMaster) ||
			   (hb[0]==mftReplyToPollForMaster) ||
			   (hb[0]==mftReplyPostponed))		//(c)
			{
				goto ResetSNIdle;
			}
			if((hb[1]==mstpTS)||
			   (hb[1]==0xFF))
			{
				//ReceivedDataNoReply
				if(hb[0]==mftBACnetDataNotExpectingReply)
				  //||(hb[0]==mftTestResponse))
				{
					macReceiveIndicate(0,portMSTP,1,&hb[2],rlen,rbuf);
					goto ResetSNIdle;
				}
			}
			//ReceivedDataNeedingReply
			if(hb[1]==mstpTS)
			{
				if(hb[0]==mftBACnetDataExpectingReply)
				{
					macReceiveIndicate(1,portMSTP,1,&hb[2],rlen,rbuf);
					ReplyTimer=0;										//	***1220
					mflag&=~receivedvalidframe;
					snstate=snsmAnswerDataRequest;

                    //Nishchal: as we are receiving valid frame, we are not in time out.
                    Slave_SABus_CommTOut_flag = false;
					//Nishchal: Once we receive first message on our SA Bus Slave, we can start counter for Comm Loss in SA Bus.
                    if (true == first_Req_DataExpectResponse){
                    	Start_SABusTout_Ctr_flag = true;
                    	first_Req_DataExpectResponse = false;
                    }

					break;
				}
				if(hb[0]==mftTestRequest)
				{
					//SendFrame(mftTestResponse,hb[2],rlen,rbuf);
					if(!SendFrame(mftTestResponse,hb[2],rlen,rbuf))
						SendFrame(mftTestResponse,hb[2],0,NULL);	//we couldn't get a buffer so return empty TestResponse
					mflag&=~receivedvalidframe;
					goto ResetSNIdle;
				}
			}
		}

        //Nishchal: we came here means we did not receive any frame either valid or invalid.
        if ( true == Start_SABusTout_Ctr_flag)
            Slave_SABus_CommTOut_flag = true; //Nishchal

		break;

	case snsmAnswerDataRequest:
		if(Answer!=NULL)		//Reply
		{
			while (true)
				if (mstpSerialTxBuf(Answer->pre,Answer->plen)==false) break;	//send everything
			SilenceTimer=0;
			mpFreePacket(Answer);
			Answer=NULL;
			snstate=snsmIdle;
		}
		else if (ReplyTimer>Treplydelay)	//CannotReply
		{
			snstate=snsmIdle;										//					***1220
		}
		break;
	}
}																	//					***1200 End

///////////////////////////////////////////////////////////////////////		//			***1223 Begin
//	Transmit a frame
//in:	ft		frame type
//		da		destination address (0xFF=broadcast)
//		len		length of data buf points to (0..501)
//		buf		points to data to send (ignored if len==0)
//
//out:  true if success, false otherwise

bool SendFrame(int ft,octet da, word len, octet* buf)
{
	mstpFrame *fp;
	octet *p,tcrc=0xFF;
	union {	word dw; octet db[2]; }u;
	word i;
	word dlen=len;
	static byte tb[16]={0};

	SilenceTimer=0;
	if(da==mstpTS)
		return true;																//	***266
	if((da==0xFF) &&
		(mode!=MasterMode))						//slaves can't broadcast			//	***1200 Begin
	{
		return true;
	}
	if(dlen==0)										// no data, use local buffer
		fp=(mstpFrame*)&tb[0];
	else
	{
		fp=mpGetPacket();							// get a packet pointer
	}
	if(!fp) return false;
	fp->pre[0]=0x55;								//preamble
	fp->pre[1]=0xFF;								//preamble
	fp->type=(octet)(ft&0xFF);						//frame type
	tcrc=CalcHeaderCRC(fp->type,tcrc);
	fp->da=da;										//destination
	tcrc=CalcHeaderCRC(da,tcrc);
	fp->sa=mstpTS;									//sa always=mstpTS
	tcrc=CalcHeaderCRC(mstpTS,tcrc);
	u.dw=dlen;
#if bigEndian
	fp->dlenhi=u.db[0];
	tcrc=CalcHeaderCRC(fp->dlenhi,tcrc);
	fp->dlenlo=u.db[1];
	tcrc=CalcHeaderCRC(fp->dlenlo,tcrc);
#else
	fp->dlenhi=u.db[1];
	tcrc=CalcHeaderCRC(fp->dlenhi,tcrc);
	fp->dlenlo=u.db[0];
	tcrc=CalcHeaderCRC(fp->dlenlo,tcrc);
#endif
	fp->hcrc=(~tcrc);
	p=fp->data;
	if(dlen!=0)
	{
		u.dw=0xFFFF;								//data CRC is a word
		for(i=0;i<len;i++)							//calculate CRC for message
		{
			*p=buf[i];
			u.dw=CalcDataCRC(*p++,u.dw);
		}
#if bigEndian
		*p++=(~u.db[1]);							//stuff CRC bytes
		*p++=(~u.db[0]);
#else
		*p++=(~u.db[0]);							//stuff CRC bytes
		*p++=(~u.db[1]);
#endif
		dlen+=2;
	}

#if mstpNeedsPad
	*p=0xFF;										//pad
	dlen+=9;										//account for preamble,hdr and pad
#else
	dlen+=8;										//account for preamble,hdr
#endif
	fp->plen=dlen;									//account for preamble,hdr and pad

	while (true)
		if(mstpSerialTxBuf(fp->pre,fp->plen)==false) break;	//send everything
	framecount++;
	if(len!=0)
	{
		mpFreePacket(fp);								//release its space
	}
	SilenceTimer=0;
	return true;
}																			//			***1223 End

///////////////////////////////////////////////////////////////////////
//	calculate Header CRC (from BACnet Appendix G)
//in:	dv	data value to accumulate
//		cv	current crc value
//out:	new crc

static octet CalcHeaderCRC(octet dv,octet cv)
{	word	crc;

	crc=cv^dv;
	crc=crc^(crc<<1)^(crc<<2)^(crc<<3)^(crc<<4)^(crc<<5)^(crc<<6)^(crc<<7);
	return (octet)((crc&0xFE)^((crc>>8)&1));
}

///////////////////////////////////////////////////////////////////////
//	calculate Data CRC (from BACnet Appendix G)
//in:	dv	data value to accumulate
//		cv	current crc value
//out:	new crc

static	word CalcDataCRC(octet dv,word cv)
{	word	crcLow;
	crcLow=(cv&0xFF)^dv;
	return (cv>>8)^(crcLow<<8)^(crcLow<<3)^(crcLow<<12)^(crcLow>>4)^(crcLow&0x0F)^((crcLow&0x0F)<<7);
}

///////////////////////////////////////////////////////////////////////			***240 Begin
//	Get a transmit packet from the pool
//
//out:	NULL		none available
//		else		pointer to the packet

static	mstpFrame *mpGetPacket(void)
{	mstpFrame	*mp=NULL;						//assume none available

	semaCapture(&mpmutex);						//capture mp's semaphore
	if((mp=mpfree)!=NULL)						//got one
		mpfree=mp->next;						//delink this one from the free pool
	semaRelease(&mpmutex);						//release mp's semaphore
	return mp;									//return pointer to it
}

///////////////////////////////////////////////////////////////////////
//	Release a transmit packet to the pool
//
//in:	mp		pointer to the packet

static	void mpFreePacket(mstpFrame *mp)
{
	semaCapture(&mpmutex);						//capture mp's semaphore
	mp->next=mpfree;
	mpfree=mp;									//link this one to the free pool
	semaRelease(&mpmutex);						//release mp's semaphore
}

///////////////////////////////////////////////////////////////////////
//	remove a packet from TxQ
//
//out:	NULL	it's empty
//		else	pointer to a frNpacket

static	mstpFrame *GetTxQ(void)
{	mstpFrame *fg=TxQfront;						//fg is the first guy in present queue

	semaCapture(&mpmutex);						//capture mp's semaphore
	if(fg!=NULL)								//not empty
	{	if(--TxQcount==0)
			TxQrear=NULL;
		TxQfront=fg->next;
	}
	semaRelease(&mpmutex);						//release mp's semaphore
	return fg;									//return the first one
}												//								***240 End

/** @fn bool  bpublic mstpSerialTxBuf(byte *b,word n)
*   @brief Passes the MS/TP message from BACnet stack to SoftDMA USART
*   @details This function is used by BACnet stack, stack calls this function
*             whenever required and sends the complete frame
*   @param[in] b pointer to buffer
*   @param[in] n number of bytes in buffer
*   @return false
*/
bool  bpublic mstpSerialTxBuf(byte *b,word n)
{
  bool retVal = false;
  bacnet_uart_for_485->BACnet_Send_Frame(b,n);
  return retVal;
}

/** @fn int mstpSerialRx()
*   @brief Receives the MS/TP message from SoftDMA USART buffer
*   @details This function is used by BACnet stack, stack calls this function
*             periodically and processes the data byte by byte
*   @param[in] None
*   @return byte
*/
int mstpSerialRx()
{
    int data = -1;

    if( recvBufCount == 0 )      					// Byte available ?
        return -1;
    /*
    if( sawOverflow == 1 )						// Buffer overflow error
    {
        sawOverflow = 0;
        return -2;
    }
    */
   	Push_TGINT();
    data = recvBuf[dispatchIndex];					///get front octet
    if (++dispatchIndex == rxmax) 					///increment modulo rxmax
        dispatchIndex = 0; 						///wrap around if needed
    --recvBufCount;   //decrease the count by one
   	Pop_TGINT();
    return data;
}


///////////////////////////////////////////////////////////////////////
//	mstpMaster(byte)
//  Change the value of Max Master


bpublic void mstpMaster(byte maxmaster)  // DH1-131
{
	Nmaxmaster = maxmaster;
}
