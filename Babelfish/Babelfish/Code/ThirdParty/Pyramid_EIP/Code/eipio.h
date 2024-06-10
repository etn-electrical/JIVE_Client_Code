/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPIO.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Handles IO packets
**
*****************************************************************************
*/

#ifndef EIPIO_H
#define EIPIO_H

/*
** Common Packet Format (CPF) header for CLASS 1 I/O packets.
*/
typedef struct tagCPFHDR
{
	UINT16 iS_count;	/* Structure count */
	UINT16 iAs_type;	/* Address structure type */
	UINT16 iAs_length;	/* Address structure length */
	UINT32 aiAs_cid;	/* Address structure connection id */
	UINT32 aiAs_seq;	/* Address structure sequence number */
	UINT16 iDs_type;	/* Data structure type */
	UINT16 iDs_length;	/* Data structure length */
} CPFHDR, *CPFHDR_P;

#define CPFHDR_SIZE			18

#define IO_RUN_IDLE_MASK	0x0001
#define IO_COO_BSHIFT		1
#define IO_COO_MASK			0x0002
#define IO_ROO_BSHIFT		2
#define IO_ROO_MASK			0x000c


extern INT32 ioParseIOPacket( UINT32 lOriginatingAddress, SOCKET lOriginatingSocket, UINT8* pData, INT32 lBytesReceived);
extern INT32 ioSendIOPacket( CONNECTION* pConnection, CONNECTION* pMulticastProducer );
extern UINT8* ioFormatIOPacketHeader(CONNECTION* pConnection, UINT16 iLen, UINT8* pDataBuffer);

#endif /* #ifndef EIPIO_H */
