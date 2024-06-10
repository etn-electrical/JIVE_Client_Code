//BIP.H Header file
//	©1997-2010, PolarSoft® Inc., All Rights Reserved
//---------------------------------------------------------------------
//[257] 24-Oct-2007 CLB Address maxbip max_apdu caclulation
//[300] 16-Sep-2005 CLB Modifications for VSBMD
//[003] 08-Dec-2003 DMF
//[002] 23-May-2002 DMF

#ifndef __BIP_H_INCLUDED
#define __BIP_H_INCLUDED

#include "core.h"

#define npduhdr		13							//size of max npdu header
#define	hdrbip		4							//BVLL header size
#define maxbip		1493-(npduhdr+hdrbip)		//max APDU is our receive buffer size minus (max NPDU header size+BIP header) (must be <= 1476)
#define maxnbip		1493-hdrbip					//max NPDU is our receive buffer size minus (max NPDU header size+BIP header) (must be <= 1476+13)

#ifndef bpublic
#define bpublic 	extern
#endif



typedef struct sockaddr_in isockaddr;

typedef struct _bvlc {
	octet	type;							//always 0x81 for BIP
	octet	function;
	word	length;
	octet	buf[1493];
} bvlc;

enum	_BVLCfunctions {
		BVLC_Result,							//0
		Write_Broadcast_Distribution_Table,		//1
		Read_Broadcast_Distribution_Table,		//2
		Read_Broadcast_Distribution_Table_Ack,	//3                                                           
		Forwarded_NPDU,							//4
		Register_Foreign_Device,				//5
		Read_Foreign_Device_Table,				//6
		Read_Foreign_Device_Table_Ack,			//7
		Delete_Foreign_Device_Table_Entry,		//8
		Distribute_Broadcast_To_Network,		//9
		Original_Unicast_NPDU,					//10
		Original_Broadcast_NPDU					//11
		};

//------------------------------------------------------------------------
//Functions provided by BIP.C:

void bpublic	bipInit(frDevInfo *);
byte bpublic	bipGetAddr(octet *);
void bpublic	bipTransmit(octet *,word,octet *);
void bpublic	bipTransmitBVLC(octet *da, word dlen, octet *tbuf);		// ***255
//void bpublic	bipTransmitIAm(void);
void bpublic	bipWork(byte);
word bpublic	bipMaxAPDU(void);
word bpublic	bipMaxNPDU(void);
word bpublic	bipMaxRPDU(void);
void bpublic	bipDeinit(void);

//------------------------------------------------------------------------
//Functions BACnet Stack must provide for BIP.C to call:

//This is called when a message is received from mstp that is broadcast
//or addressed to this station
//in:	rpe		0=no reply expected, 1=reply expected
//		port	the port this is coming from
//		salen	the source MAC address length
//		sa		source MAC address
//		rlen	received length (always >0)
//		rp		points to received NPDU octets
//out:	true	if packet was processed

bool bpublic macReceiveIndicate(octet rpe, byte port, byte salen, octet *sa, int rlen, octet *rp);	//	***002

#endif //__BIP_H_INCLUDED
