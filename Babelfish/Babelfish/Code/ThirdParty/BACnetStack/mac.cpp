/*--------------------------------------------------------------------
	©2002-2014, PolarSoft® Inc., All Rights Reserved
	
module:		MAC.C
desc:		MAC Layer for FreeRange VSBHP
authors:	David M. Fisher, Jack Neyer
last edit:	18-Jan-2014 [1205] DMF add frhInit, frhTransmit
			09-Sep-2005 [241]  DMF fix macTransmit for MSTP
			25-Jul-2005 [233]  DMF revise for VSBHP
			08-Dec-2003 [002]  DMF simplify for CBG eth/bip
			26-Apr-2002 [001]  JN  First Cut adapted from FRR_MAC VSB MAC.C
----------------------------------------------------------------------*/
#include <stdio.h>

#include "mac.h"
#ifdef useEth
#include "eth.h"
#endif
#ifdef useBIP
#include "bip.h"
#endif
#ifdef useMSTP
#include "mstp.h"
#endif

///////////////////////////////////////////////////////////////////////
//	initialize MAC layers
//
// in:	portid	the port id to initialize
//		*di		pointer to frDevInfo struct to fill in
// out:	0		invalid portid
//		else	success

bpublic byte macInit(byte portid,frDevInfo *di)
{
	//frhInit(portid,di);						//tell hook we are initing	***1205
	
	switch (portid)
	{
#ifdef useMSTP
	case portMSTP:
		mstpInit(di);						//init MSTP layer
		break;
#endif
#ifdef useEth
	case portEth:
		ethInit(di);						//init 8802-3 layer
		break;
#endif
#ifdef useBIP
	case portBIP:
		bipInit(di);						//init BACnet/IP layer
		break;
#endif
	default:
		return 0;							//invalid port
	}
	return 1;								//success
}

///////////////////////////////////////////////////////////////////////
//	get mac address
//
// in:	port	the portid whose mac address is desired
//		maddr	buffer to put it in (8 bytes long)
//out:	the length (or 0 meaning invalid portid)

bpublic byte macGetAddr(byte portid, octet *maddr)
{
#ifdef useMSTP
	if(portid==portMSTP) return mstpGetAddr(maddr);
#endif
#ifdef useEth
	if(portid==portEth) return ethGetAddr(maddr);
#endif
#ifdef useBIP
	if(portid==portBIP) return bipGetAddr(maddr);
#endif
	return 0;								//invalid port
}

///////////////////////////////////////////////////////////////////////
//	get max apdu length
//
// in:	port	the portid whose length is desired
//out:	the length (or 0 meaning invalid portid)

bpublic word macMaxAPDU(byte portid)
{
#ifdef useMSTP
	if(portid==portMSTP) return mstpMaxAPDU();
#endif
#ifdef useEth
	if(portid==portEth) return ethMaxAPDU();
#endif
#ifdef useBIP
	if(portid==portBIP) return bipMaxAPDU();
#endif
	return 0;								//invalid port
}

///////////////////////////////////////////////////////////////////////
//	get max npdu length
//
// in:	port	the portid whose length is desired
//out:	the length (or 0 meaning invalid portid)

bpublic word macMaxNPDU(byte portid)
{
#ifdef useMSTP
	if(portid==portMSTP) return mstpMaxNPDU();
#endif
#ifdef useEth
	if(portid==portEth) return ethMaxNPDU();
#endif
#ifdef useBIP
	if(portid==portBIP) return bipMaxNPDU();
#endif
	return 0;								//invalid port
}

///////////////////////////////////////////////////////////////////////
//	get max response pdu length
//
// in:	port	the portid whose length is desired
//out:	the length (or 0 meaning invalid portid)

bpublic word macMaxRPDU(byte portid)
{
#ifdef useMSTP
	if(portid==portMSTP) return mstpMaxRPDU();
#endif
#ifdef useEth
	if(portid==portEth) return ethMaxRPDU();
#endif
#ifdef useBIP
	if(portid==portBIP) return bipMaxRPDU();
#endif
	return 0;								//invalid port
}

///////////////////////////////////////////////////////////////////////
//	try to transmit data (like a DL-UNITDATA.request)
//
//in:	portid			the destination port
//		da				the destination mac address to send to or NULL meaning broadcast
//		dlen			the length of data np points to
//		np				points to dlen bytes of data to send (copied by lower layers)
//		reqflg			macisRequest, macisResponse, or macisUnconfirmed					***240

bpublic void macTransmit(byte portid, octet *da, word dlen, octet *np, byte reqflg)
{
	//if(frhTransmit(portid,da,dlen,np,reqflg)) return;	//									***1205
		
#ifdef useMSTP
	if(portid==portMSTP)
	{	if(da==NULL)						//broadcast										***241 Begin
			mstpTransmit(mstpBroadcast,dlen,np,reqflg);
		else
			mstpTransmit(*da,dlen,np,reqflg);
	}										//												***241 End
#endif
#ifdef useEth
	if(portid==portEth)
		ethTransmit(da,dlen,np);
#endif
#ifdef useBIP
	if(portid==portBIP)	
		bipTransmit(da,dlen,np);
#endif
}

///////////////////////////////////////////////////////////////////////
//	Main Loop (call this every 5ms)
//
//in:	portid	the port id to initialize
//		actualperiod	the time in milliseconds that macWork actually
//						gets called. Ideally it should be every 5ms, but
//						if the best you can do is say 8ms then actualperiod=8.

bpublic void macWork(byte portid,byte actualperiod)
{
	switch (portid)
	{
#ifdef useMSTP
	case portMSTP:
		mstpWork(actualperiod);
		break;
#endif
#ifdef useEth
	case portEth:
		ethWork(actualperiod);
		break;
#endif
#ifdef useBIP
	case portBIP:
		bipWork(actualperiod);
		break;
#endif
	default:
		break;
	}
}
