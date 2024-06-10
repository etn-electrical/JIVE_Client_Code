//Contents of MAC.H
//	©2002-2014, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------

#ifndef __MAC_H_INCLUDED
#define __MAC_H_INCLUDED
#define portEth		0
#define portBIP		1
#define portMSTP	2
#define portMax		3

#include "BACnet_Config.h"
//Comment or uncomment these to include support for each LAN
//#define useMSTP			1			//Shrikant enabled useMSTP
//#define useBIP			1			//Prateek enabled useBIP
//#define useEth			1
//----------------------------------------------------------------------------
#include "core.h"

//#define portDefault	portMSTP			//change to whatever default you want

//macTransmit uses these constants
#define	macisRequest		0		//we expect a response
#define	macisResponse		1		//this is a response
#define	macisUnconfirmed	2		//no response expected

//------------------------------------------------------------------------
//Functions provided by MAC.C:
byte bpublic	macInit(byte,frDevInfo *);
byte bpublic	macGetAddr(byte,octet *);
void bpublic	macTransmit(byte,octet *,word,octet *,byte);
void bpublic	macWork(byte,byte);
word bpublic	macMaxAPDU(byte);
word bpublic	macMaxNPDU(byte);
word bpublic	macMaxRPDU(byte);

//------------------------------------------------------------------------
//Functions provided by Application:
void bpublic	frhInit(byte,frDevInfo *);						//	***1204
bool bpublic	frhTransmit(byte,octet *,word,octet *,byte);	//	***1204

//This is called when a message is received from a MAC layer that is broadcast
//or addressed to this station
//in:	rpe		0=no reply expected, 1=reply expected
//		port	the port this is coming from
//		salen	the source MAC address length
//		sa		source MAC address
//		rlen	received length (always >0)
//		rp		points to received NPDU octets
//out:	true	if packet was processed

bool bpublic macReceiveIndicate(octet rpe, byte port, byte salen, octet *sa, int rlen, octet *rp);	//	***1204
bool bpublic frhReceiveIndicate(octet rpe, byte port, byte salen, octet *sa, int rlen, octet *rp);	//	***1204

#endif //__MAC_H_INCLUDED
