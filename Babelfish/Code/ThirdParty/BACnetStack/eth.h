//Contents of ETH.H
//	©2001-2010, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------
//[010] 08-Dec-2003 DMF
//[009] 23-May-2002 DMF

#ifndef __ETH_H_INCLUDED
#define __ETH_H_INCLUDED

#include "core.h"

#define maxmac		1500						//max sized MAC packet
#define npduhdr		13							//size of max npdu header
#define	hdr8802		17							//eth 802.3 and 802.2 header size
#define maxal		maxmac-(npduhdr+hdr8802)	//max APDU is our receive buffer size minus (max NPDU header size+802.3/2 header)
#define maxnl		maxmac-hdr8802				//max NPDU


typedef struct _PKT8022 {			//Eth8022 packet
	byte	pktDADR[6];		//destination address
	byte	pktSADR[6];		//source address
	byte	pktLEN[2];		//length ms/ls (bytes after these two)
	byte	pktLLC[3];		//LLC DSAP, SSAP, UI
	byte	pktNPDU[1497];	//the NPDU
} PKT8022;

typedef struct	_ethIam {	//ethIAm
	octet	version;					//BACnet version (always 1)
	octet	npci;						//control for NL
	word	dnet;						//always FFFF
	octet	dlen;						//always 0
	octet	hopcount;					//always 255
	octet	pdutype;					//always 0x10
	octet	service;					//always 0 (IAm)
	octet	devtag;						//always 0xC4
	octet	objtype;					//always 0x02 (device object)
	octet	devinst[3];					//big-endian device instance
	octet	msv[8];						//maxapdu, segmentation, vendorid
} ethIAm;

//------------------------------------------------------------------------
//Functions provided by ETH.C:
void bpublic	ethInit(frDevInfo *);
byte bpublic	ethGetAddr(octet *);
void bpublic	ethTransmit(octet *,word,octet *);
void bpublic	ethTransmitIAm(void);
void bpublic	ethWork(byte);
word bpublic	ethMaxAPDU(void);
word bpublic	ethMaxNPDU(void);
word bpublic	ethMaxRPDU(void);

//------------------------------------------------------------------------
//Functions BACnet Stack must provide for ETH.C to call:

//This is called when a message is received from mstp that is broadcast
//or addressed to this station
//in:	rpe		0=no reply expected, 1=reply expected
//		port	the port this is coming from
//		salen	the source MAC address length
//		sa		source MAC address
//		rlen	received length (always >0)
//		rp		points to received NPDU octets
//out:	true	if packet was processed

bool bpublic macReceiveIndicate(octet rpe, byte port, byte salen, octet *sa, int rlen, octet *rp);	//	***009

#endif //__ETH_H_INCLUDED
