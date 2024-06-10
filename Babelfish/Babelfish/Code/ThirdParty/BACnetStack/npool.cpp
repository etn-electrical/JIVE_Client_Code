/*---------------------------------------------------------------------------
	©2003-2014, PolarSoft® Inc., All Rights Reserved
	
module:		NPOOL.C
desc:		frNpacket Pool Manager
authors:	David M. Fisher, Jack R. Neyer
last edit:	13-Nov-2014 [1213] CLB expanded npoolbzero to make everything 0x00
			11-Nov-2014 [1212] CLB call npoolbzero in npoolGetPacket
			01-Nov-2014 [1209] DMF remove volatile keyword
			21-Oct-2014 [1208] DMF Add volatile keyword to prevent optimizer issues
			05-Jun-2014 [1206] DMF fix init of npool
			17-Feb-2011 [413] CLB added npoolbzero et al
			23-Oct-2007 [255] CLB Add debugging code
			12-Jun-2006 [252] CLB Use Linux specific mutex initialization
								  Use static buffers instead of malloc'd space
			28-Feb-2006 [243] DMF add npoolNPDUsize
			06-Dec-2003 [001] DMF convert from eth.c version to generalized
----------------------------------------------------------------------------*/ 
//#include <malloc.h>										//						***252
// Note: In order that we don't have to free malloc'd space when vsbhp is killed
//       I've made the npool buffers defined in the data segment. WARNING--
//       DO NOT initialize the buffers when declaring them or the executable 
//       is increased by that amount!!!
// 
#include <stdio.h>
#include <stdlib.h>
//#include <memory.h> //SRP: Disabled to get stack compiled
#include <string.h>
#include <assert.h>
#include "semaphore.h"
#include "npool.h"
#include "util.h"											//						***413

frNpacket	*bigfree=NULL;									//large packets			***1209
frNpacket	*smallfree=NULL;								//small packets			***1209
word		smallpayload=0;
#ifdef _WINDOWS												//						***252 Begin
static semaphore		npoolmutex;
#else
static semaphore	npoolmutex = {0}; //={PTHREAD_MUTEX_INITIALIZER,0}; //SRP: Disabled, later RTOS mutex will be added
#endif
static byte nsbuffer[npsmall][smallalloc];
static byte nbbuffer[npbig][sizeof( frNpacket )];			//						***252 End

#ifdef DEBUGVSBHP											//						***255 Begin
static word		smallcnt=0;
static word		bigcnt=0;
#endif														//						***255 End

///////////////////////////////////////////////////////////////////////				***413 Begin
//	Calculate the size of the frNpacket header

static dword calcPktHdrSize(frNpacket*q)
{
    byte *np;
    byte *sp;
    dword hdrsize;

    sp=(byte*)q;
    np=q->npdu;
    hdrsize=np-sp;   //gives us the diff between where the struct starts and the NPDU starts (the header!)
    return hdrsize;
}

///////////////////////////////////////////////////////////////////////
//	Sets the NPDU of an frNpacket to all 0x00

void bpublic npoolbzero(frNpacket* q)
{
	word hdrsize;
	if(q->nflag&Nisbig)
	{
		byteset(q->npdu,0x00,maxmac);		
	}
	else
	{
		hdrsize=(word)calcPktHdrSize(q);
		byteset(q->npdu,0x00,(smallalloc-hdrsize));
	}
	byteset((byte*)&q->src,0x00,sizeof(frSource));						//			**1213 Begin
	q->nlen=0;
	q->apdu=NULL;
	q->timer=0;
	q->ntries=0;
	q->svc=0;
	q->invokeid=0;														//			**1213 End
}																		//			***413 End

///////////////////////////////////////////////////////////////////////
//	Initialize this module

void bpublic npoolInit(void)
{
int					i;
frNpacket			*mp;						//									***1209

	//semaClear(&npoolmutex);
	semaCapture(&npoolmutex);					//capture npool's semaphore
	smallpayload=(word)(smallalloc-strucoffset(frNpacket,npdu));	//				***1206
	for(i=0;i<npbig;i++)
	{	
		mp=(frNpacket *)(&nbbuffer[i][0]);		//									***1208
		mp->nflag=Nisbig;						//this one is a big one
		mp->next=bigfree;						//link onto free list
		bigfree=mp;
	}
	for(i=0;i<npsmall;i++)
	{	
		mp=(frNpacket *)(&nsbuffer[i][0]);		//									***1208
		mp->nflag=0;							//not a big one
		mp->next=smallfree;						//link onto free list
		smallfree=mp;
	}
	semaRelease(&npoolmutex);					//release npool's mutex
}

///////////////////////////////////////////////////////////////////////
//	Get a packet from the pool
//
//in:	payload		minimum size needed for the payload
//out:	NULL		none available
//		else		pointer to the packet

bpublic frNpacket *npoolGetPacket(word payload)
{
frNpacket	*mp;								//									***1209

#ifdef DEBUGVSBHP
	char dbg[256];
#endif
	semaCapture(&npoolmutex);					//capture npool's mutex
	mp=NULL;									//assume none available				***1208
	if(payload>smallpayload)					//must get a big one
	{
		mp=bigfree;								//									***1208
		if(mp!=NULL)							//got a big one						***1208
		{
			bigfree=mp->next;					//delink this one from the big free pool
#ifdef DEBUGVSBHP								//									***255 Begin
			bigcnt++;
#endif											//									***255 End
		}
	}
	else										//either size will do
	{	
		if((mp=smallfree)!=NULL)				//got a small one
		{
			smallfree=mp->next;					//delink this one from the small free pool
#ifdef DEBUGVSBHP								//									***255 Begin
			smallcnt++;
#endif											//									***255 End
		}
		else if((mp=bigfree)!=NULL)				//got a big one
		{
			bigfree=mp->next;					//delink this one from the big free pool
#ifdef DEBUGVSBHP								//									***255 Begin
			bigcnt++;
#endif											//									***255 End
		}
	}
	if(mp!=NULL)														//			***1212 
		npoolbzero(mp);													//			***1212
	semaRelease(&npoolmutex);					//release npool's mutex
#ifdef DEBUGVSBHP								//									***255 Begin
	sprintf(dbg,"%s\tsmallcnt=%d\tbigcnt=%d\n",__FUNCTION__,smallcnt,bigcnt);
	fraLog(dbg);	
#endif											//									***255 End
	return mp;									//return pointer to it
}

///////////////////////////////////////////////////////////////////////
//	Release a packet to the pool
//
//in:	mp		pointer to the packet

void bpublic npoolFreePacket(frNpacket *mp)
{
#ifdef DEBUGVSBHP
	char dbg[256];
#endif
	semaCapture(&npoolmutex);					//capture npool's mutex
	if(mp->nflag&Nisbig)						//its a big one
	{	
		mp->next=bigfree;
		bigfree=mp;								//link this one to the big free pool
#ifdef DEBUGVSBHP								//	***255 Begin
		bigcnt--;
#endif											//	***255	End
	}
	else										//either size will do
	{	mp->next=smallfree;
		smallfree=mp;							//link this one to the small free pool
#ifdef DEBUGVSBHP								//	***255 Begin
		smallcnt--;
#endif											//	***255 End
	}
	semaRelease(&npoolmutex);					//release npool's mutex
#ifdef DEBUGVSBHP								//	***255 Begin
	sprintf(dbg,"%s\tsmallcnt=%d\tbigcnt=%d\n",__FUNCTION__,smallcnt,bigcnt);
	fraLog(dbg);
#endif											//	***255 End
}

///////////////////////////////////////////////////////////////////////			***243 Begin
//	Find out how much NPDU space a packet has
//
//in:	mp		pointer to the packet
//out:	available space in packet for NPDU payload

word bpublic npoolNPDUsize(frNpacket *mp)
{
	if(mp->nflag&Nisbig)						//its a big one
		return maxmac;							//these have a lot of room
	return smallpayload;						//small ones only have a little space
}												//								***243 End



