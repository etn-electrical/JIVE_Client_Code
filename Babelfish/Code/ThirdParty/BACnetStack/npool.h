//Contents of NPOOL.H
//	©2003-2014, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------

#ifndef __NPOOL_H_INCLUDED
#define __NPOOL_H_INCLUDED

#include "core.h"
#include "BACnet_Config.h"
//bits in nflag

/* The pool size parameters are shifted to BACnet_Config.h */

//#define npsmall		60							//number of small buffers
//#define npbig		20							//number of big buffers
#define npmax		npsmall+npbig
#define smallalloc	128							//size to malloc for small buffers

/*	Issue number LTK-1490 - Read Property multiple problem
	We relocated the defines useMSTP and useBIP in the BACnet_Config.h
	So now the below #if 	gets a definition for useMSTP even when we have
	useBIP. By making #if 0, maxmac is defined 1500 in all use cases as per
	previous working code.

	TODO: Read the value of interface type ( mstp or bip ) at run time and then 
	allocate maxmac sized array (npdu) at runtime.
	Also need to take care of dependant variables. (eg. maxal and maxnl at below
	lines, There may be few other variable in other files).
	Caution: sizeof for frNpacket also used at some location.
	-Sadanand */

#if 0	//useMSTP										//								***1209 Begin
#define maxmac		501
#else
#define maxmac		1500						//max sized MAC packet
#endif											//								***1209 End

#define npduhdr		13							//size of max npdu header
#define	hdr8802		17							//eth 802.3 and 802.2 header size
#define maxal		maxmac-(npduhdr+hdr8802)	//max APDU is our receive buffer size minus (max NPDU header size+802.3/2 header)
#define maxnl		maxmac-hdr8802				//max NPDU

#define strucoffset(s,m)   (size_t)( (char *)&(((s *)0)->m) - (char *)0 )

#ifndef bpublic
#define bpublic 	extern
#endif

#ifdef _WINDOWS
#ifndef bool
typedef unsigned char bool;
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
#ifndef false
#define false		0
#endif
#ifndef true
#define true		1
#endif
#ifndef NULL
#define NULL		0
#endif
#endif

typedef struct	_frSource {
	octet	port;					//the source port
	octet	maxresponse;			//the client's max-apdu						//	***405
	octet	slen;					//the length of sadr (or unknowndevice)
	octet	sadr[8];				//the source address
	octet	drlen;					//the length of dradr
	octet	dradr[8];				//the dest address when routed
	union _snet {					//the source network number (0=local else source routed)
		word	w;
		octet b[2];
	}	snet;						//note: this is always stored big endian!	
} frSource;

typedef struct _frNpacket {
	struct _frNpacket *next;		//link to next one
	frSource	src;				//where it came from
	word		nlen;				//number of bytes in the bag
	octet		*apdu;				//pointer into npdu
	void		*vb;				//points to frVbag when used with confirmed guys
	octet		timer;				//retry timer
	octet		ntries;				//number of tries
	octet		invokeid;			//invokeid (for matching replies)
	octet		svc;				//service (for matching replies)
	octet		nflag;				//(see defines below)
	octet		npdu[maxmac];		//bag for bytes
} frNpacket;

//bits in nflag
#define Nisbig			1			//0=small, 1=big packet
#define Nreplyexpected	2			//0=no, 1=yes

//------------------------------------------------------------------------
//Functions provided by NPOOL.C:

void bpublic		npoolbzero(frNpacket*);
void bpublic		npoolInit(void);
bpublic frNpacket	*npoolGetPacket(word);
void bpublic		npoolFreePacket(frNpacket *);
word bpublic		npoolNPDUsize(frNpacket *);		//						***243

#endif //__NPOOL_H_INCLUDED
