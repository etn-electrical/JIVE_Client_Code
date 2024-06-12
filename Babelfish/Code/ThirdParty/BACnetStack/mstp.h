//Contents of MSTP.H
//	©2000-2013, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------
//[1200] 04-Apr-2013	Added slave node optional functionality
//[ 216] 24-Apr-2002
//[ 008] 08-Mar-2001
#ifndef __MSTP_H_INCLUDED
#define __MSTP_H_INCLUDED

#include "core.h"

#define		mstpmaxapdu			480
#define		maxrx				mstpmaxapdu+13+8	//max chars in rx buffer (NPDU size)	***008
#define		maxtx				mstpmaxapdu+13+8	//max chars transmitted					***008
#define		mstpNeedsPad		0					//0=no, 1=yes							***1205
#define		mpreserve			10					//number of reserved transmit buffers	***240 Begin

typedef struct _mstpFrame {
struct _mstpFrame *next;
	word	plen;						//transmit length
	octet	req;						//0=no reply expected, 1=reply expected
	octet	pre[2];						//preamble 55 FF
	octet	type;						//frame type
	octet	da;							//dest addr
	octet	sa;							//source addr
	octet	dlenhi;						//big endian data length
	octet	dlenlo;
	octet	hcrc;
	octet	data[maxtx+3];				//data+2CRC+pad
} mstpFrame;							//													***240 End

#define mstpBroadcast					0xFF

//MSTP frame types
#define	mftToken						0x00
#define	mftPollForMaster				0x01
#define	mftReplyToPollForMaster			0x02
#define	mftTestRequest					0x03
#define	mftTestResponse					0x04
#define	mftBACnetDataExpectingReply		0x05
#define	mftBACnetDataNotExpectingReply	0x06
#define	mftReplyPostponed				0x07
#define mftUnknown						0x08

#define MasterMode						0x00				//	***1200
#define SlaveMode						0x01				//	***1200

//Low Level Receive State Machine states
#define lrsmIdle						0
#define lrsmPreamble					1
#define	lrsmHeader						2
#define	lrsmData						3
//Master Node State Machine states
#define	mnsmInitialize					0
#define mnsmIdle						1
#define mnsmUseToken					2
#define mnsmWaitForReply				3
#define mnsmDoneWithToken				4
#define mnsmPassToken					5
#define mnsmNoToken						6
#define mnsmPollForMaster				7
#define mnsmAnswerDataRequest			8

#define snsmInitialize					0					//	***1200 Begin
#define snsmIdle						1
#define snsmAnswerDataRequest			2					//	***1200 End


//------------------------------------------------------------------------
//Functions provided by MSTP.C:
void bpublic	mstpDoReplyPostponed(void);					//								***264
void bpublic	mstpInit(frDevInfo *);						//								***233
void bpublic	mstpTransmit(octet,word,octet *,byte);		//								***240
void bpublic	mstpWork(byte);
word bpublic	mstpMaxAPDU(void);
word bpublic	mstpMaxNPDU(void);							//								***233
word bpublic	mstpMaxRPDU(void);
byte bpublic	mstpGetAddr(octet *);						//								***233
void bpublic    mstpMaster(byte);    // DH1-131

//------------------------------------------------------------------------
//Functions BACnet Stack must provide for MSTP.C to call:

//This is called when a message is received from mstp that is broadcast						***200 Begin
//or addressed to this station
//in:	rpe		0=no reply expected, 1=reply expected
//		port	the port this is coming from
//		salen	the source MAC address length
//		sa		source MAC address
//		rlen	received length (always >0)
//		rp		points to received NPDU octets
//out:	true if packet was processed												***004

bool bpublic macReceiveIndicate(octet rpe, byte port, byte salen, octet *sa, int rlen, octet *rp);

//------------------------------------------------------------------------					***200 End
//Functions you must provide for MSTP.C to call:

//This is called to find out if a character is available from the serial port for mstp
//out:	-1		no chars available
//		-2		error occurred (framing error, overrun)
//		0..255	the byte code for the character received
int bpublic		mstpSerialRx(void);

//This is called to transmit a buffer to the serial port for mstp
//If the EIA-485 driver is not enabled, then it should be enabled by
//this routine prior to sending and disabled after the last byte is shifted out
//in:	txb		buffer to transmit (enable TxDriver if needed)
//		txlen	number of bytes to send
//out:	false	transmit succeeded
//		true	transmit failed
bool  bpublic mstpSerialTxBuf(byte *txb,word txlen);							//			***200

#endif //__MSTP_H_INCLUDED
