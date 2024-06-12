/*---------------------------------------------------------------------------
	©2006-2011, PolarSoft® Inc., All Rights Reserved
	
module:		bvlc.c
desc:		BACnet/IP foreign device module
authors:	David M. Fisher, Jack R. Neyer
last edit:	28-Aug-2011 [418] CLB Correct NAK for RegisterForeignDeviceRequest
			01-Aug-2007 [255] CLB Foreign device addition to VSBHP
									adapt from full FreeRange			
-----------------------------------------------------------------------------*/ 
/*#ifdef _WINDOWS
//Win---------------------------------------------------------------Win
#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <string.h>
#include <stdio.h>
//Win---------------------------------------------------------------Win
#else
//Linux-------------------------------------------------------------Linux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <memory.h>
#include <errno.h>      
#include <unistd.h>     
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netdb.h>
#include <fcntl.h>
#include <termios.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#endif
*/
//Linux-------------------------------------------------------------Linux
#include "mac.h"
#include "bip.h"
#include "bvlc.h"
#include "util.h"
#ifdef _DEBUG		
#include <string.h> //for debugging only to call fraLog
#include <stdlib.h>
#include <stdio.h>
#endif				

word		bvlcreply=0xFFFF;						//no reply yet				
word		fdrstate=fdrLocalSubnet;				//state for FDR				
int			FDRtimer=0;								//downcounter for reregistration
word		FDRretry=0;								//count of register retries

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */
bpublic word	BACnetIPport;						//note: Big Endian!
bpublic dword	BACnetIPbbmd;						//BBMD to register with
bpublic word	BACnetIPbbmdttl;					//time to live
#ifdef _WINDOWS
bpublic SOCKET	bipsock;						//socket to operate on
#else
bpublic unsigned int bipsock;
#define INVALID_SOCKET  (unsigned int)(~0)
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

///////////////////////////////////////////////////////////////////////
//	Main Foreign Device Registration entry point, call once per second
//

void bpublic fd_second(void)
{
	if(bipsock==INVALID_SOCKET) return;		//not ready yet			
	FDRsecond();
}

///////////////////////////////////////////////////////////////////////
//	BBMD Handler for outgoing BVLC's 
//
// in:	dlen	the length of bvlc be be sent
//		tb		points to a bvlc to be sent
//			

void bpublic fd_transmit(word dlen,bvlc *tb)
{	
	octet	bbmdaddr[6];	

	if(tb->function==Original_Broadcast_NPDU)						//we only need to worry about along Original_Broadcast_NPDUs
	{			
		if(BACnetIPbbmd!=0)											//and Foreign Device Registration is enabled
		{			
			if(fdrRegistered==fd_getfdrstate())						//and we've successfully registered as an FD with a BBMD
			{
				frcpy(bbmdaddr,&BACnetIPbbmd,4);
				frcpy(&bbmdaddr[4],&BACnetIPport,2);			
				tb->function=Distribute_Broadcast_To_Network;		//instead of Original_Broadcast we'll do a Distribute_Broadcast_To_Network
				bipTransmitBVLC(bbmdaddr,dlen,(octet *)tb);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////
//	Set the reply 
//

static void bbmd_reply(word reply)
{
	bvlcreply=reply;
}

///////////////////////////////////////////////////////////////////////
//	Return the state of a FD registration
//
//	out:the state aof a FD registration

static word fd_getfdrstate(void)
{
	return fdrstate;
}

///////////////////////////////////////////////////////////////////////
//
// Send a FD Registration Request
//
// in:	bbmd		the BIP addres of the bbmd to register with as an octet[6]
//		timetolive	the time to live in seconds

static void SendForeignDeviceReg(octet *bbmd, word timetolive)		
{	smallbvlc	bvlcRegFD={0x81,Register_Foreign_Device,{0,6},{0,0}};
	
	bvlcRegFD.buf[0]=(octet)((timetolive>>8)&0xFF);
	bvlcRegFD.buf[1]=(octet)(timetolive&0xFF);
	bipTransmitBVLC(bbmd,6,(octet *)&bvlcRegFD);
}

///////////////////////////////////////////////////////////////////////
// Send a BVLC message
//
// in:	bbmd		the BIP addres of the bbmd to register with as an octet[6]
//		resultcode	the code to include in the message


static void bbmd_sendResult(octet *bbmd,word resultcode)
{	smallbvlc	bvlcResult={0x81,BVLC_Result,{0,6},{0,0}};
	
	bvlcResult.buf[0]=(octet)((resultcode>>8)&0xFF);
	bvlcResult.buf[1]=(octet)(resultcode&0xFF);
	bipTransmitBVLC(bbmd,6,(octet *)&bvlcResult);
}

///////////////////////////////////////////////////////////////////////
// Peform foreign device maintainence
//
// call once per second
// maintains the ForeignDeviceRegistration state

void bpublic FDRsecond(void)
{	
	octet	bbmdaddr[6];
	
	if(BACnetIPbbmd==0) 
	{	fdrstate=fdrLocalSubnet;						//change our state to not registered
		FDRretry=0;
		return;											//no work to do
	}
	if(--FDRtimer<=0)									//timed out, so re-register	
	{
		fdrstate=fdrRegister;							//change our state to registering
	}
	switch(fdrstate)
	{
	case fdrRegister:
		FDRtimer=BACnetIPbbmdttl;						//refresh timer, in seconds	
		if(FDRtimer<3) FDRtimer=3;						//wait at least 3 seconds		
		if(++FDRretry>10) 
		{
			FDRretry=0;
			FDRtimer=60;								//no response, give up for 1 minute
		}
		bvlcreply=0xFFFF;								//no reply yet
		frcpy(bbmdaddr,&BACnetIPbbmd,sizeof(dword));				
		frcpy(&bbmdaddr[4],&BACnetIPport,sizeof(word));	
		SendForeignDeviceReg(bbmdaddr,BACnetIPbbmdttl);		//send the foreign registration
		fdrstate=fdrWaitACK;
		break;
	case fdrWaitACK:
		if(bvlcreply==Register_Foreign_Device_ACK)		//we got a reply that we're registered
		{	
			FDRretry=0;
			FDRtimer=BACnetIPbbmdttl;						//refresh timer
			fdrstate=fdrRegistered;
			break;
		}
		break;
	default:
		break;
	}

}	

///////////////////////////////////////////////////////////////////////
//	BBMD Handler for incoming BVLC's 
//
// in:	rb		points to a bvlc
//		sa		has a BIP address as an octet[6] string
// out:	true	we handled it here	

bool bpublic bvlcWork (bvlc *rb, octet *sa)
{	
	octet		*prb=rb->buf;
	union		{byte b[2]; word w;}u;
	bool		handled=true;

	switch(rb->function)
	{	
	case BVLC_Result:
		u.b[0]=prb[0];
		u.b[1]=prb[1];
		u.w=flipw(u.w);
		if (u.w==Register_Foreign_Device_ACK ||
			u.w==Register_Foreign_Device_NAK) 
			bbmd_reply(u.w);
		break;
	case Delete_Foreign_Device_Table_Entry:
		bbmd_sendResult(sa,Delete_Foreign_Device_Table_Entry_NAK);  //send back a NAK
		break;	
	case Distribute_Broadcast_To_Network:
		bbmd_sendResult(sa,Distribute_Broadcast_To_Network_NAK);	//send back a NAK
		break;	
	case Read_Broadcast_Distribution_Table:
		bbmd_sendResult(sa,Read_Broadcast_Distribution_Table_NAK);	//send back a NAK
		break;	
	case Read_Foreign_Device_Table:
		bbmd_sendResult(sa,Read_Foreign_Device_Table_NAK);			//send back a NAK
		break;
	case Register_Foreign_Device:	
		bbmd_sendResult(sa,Register_Foreign_Device_NAK);			//send back a NAK	***418
		break;
	case Write_Broadcast_Distribution_Table:				
		bbmd_sendResult(sa,Write_Broadcast_Distribution_Table_NAK);  //send back a NAK
		break;	
	default:
		handled=false;
		break;														//unhandled ones are ignored
	}
	return handled;
}


