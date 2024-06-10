/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPDU.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** EPATH PDU handling
**
*****************************************************************************
*/

#ifndef EIPPDU_H
#define EIPPDU_H

typedef struct tagPDU_HDR
{
	UINT8 bService;
	UINT8 bSize;
}
PDU_HDR;

#define PDU_HDR_SIZE		2


#define MAX_LINK_ADDRESS	64
#define MAX_SYMBOL_SIZE		128

/*---------------------------------------------------------------------------
**
** IOI Segment Type Mask and Values
**
**---------------------------------------------------------------------------
*/

#define SEGMENT_TYPE_MASK			0xE0	/* IOI Segment Type Mask */

#define PATH_SEGMENT				0x00	/* Path type segment indicator */
#define LOGICAL_SEGMENT				0x20	/* logical type segment indicator */
#define NETWORK_SEGMENT				0x40	/* when/how to send */
#define SYMBOLIC_SEGMENT			0x60	/* symbolic type segment indicator */
#define DATA_SEGMENT				0x80	/* data segment indicator */

#define LOGICAL_SEG_TYPE_MASK		0x1C

/* The following constants are bitwise ORed with the LOGICAL_SEGMENT constant to form one of the
   Logical Segment types */
#define LOGICAL_SEG_CLASS			0x00
#define LOGICAL_SEG_INSTANCE		0x04
#define LOGICAL_SEG_MEMBER			0x08
#define LOGICAL_SEG_CONN_POINT		0x0C
#define LOGICAL_SEG_ATTRIBUTE		0x10
#define LOGICAL_SEG_KEY				0x14

#define LOGICAL_SEG_FORMAT_MASK		0x03

#define LOGICAL_SEG_8_BIT			0x00	/* Indicates that the value is 8 bit long */
#define LOGICAL_SEG_16_BIT			0x01	/* Indicates that the value is 16 bits long */
#define LOGICAL_SEG_32_BIT			0x02	/* Indicates that the value is 32 bits long */

#define EXTENDED_LINK_MASK			0x10
#define PORT_ID_MASK				0x0F
#define EXTENDED_PORT_FLAG			0x0F
#define COMMON_KEY_FORMAT			0x04

#define NET_SEGMENT_TYPE_MASK		0x1F
#define PRODUCTION_INHIBIT_TYPE		0x03

#define SYM_SEGMENT_SIZE_MASK		0x1F
#define EXTENDED_SYM_TYPE_MASK		0xE0

#define EXTENDED_SYM_TYPE_DOUBLEBYTE	0x20
#define EXTENDED_SYM_TYPE_TRIPLEBYTE	0x40
#define EXTENDED_SYM_TYPE_SPECIAL		0xC0

#define PORT_SEGMENT				0x1		/* Port segment is used to indicate the slot the target device is in */
#define SIMPLE_DATA_SEGMENT			0x80	/* Simple data segment is used to pass device specific configuration with the Forward Open */
#define EXT_SYMBOL_SEGMENT			0x91	/* Used to include simple ANSI strings in the request or response */


extern void pduInit(EPATH* pPDU, BOOL bPadded);
extern BOOL pduAddTag(EPATH* pPDU, UINT16 iTagType, UINT16 iTagDataSize, ETAG_DATATYPE lData);
extern void pduRelease( EPATH* pPDU );
extern void pduConvertToInternalStorage( EPATH* pExternalPDU, EPATH* pStackPDU );
extern INT32 pduParse( UINT8* pPacketPtr, EPATH* pPDU, BOOL bPDUHeader, UINT32 nPDUFreeLength);
extern ETAG_DATATYPE pduGetTagByType( EPATH* pPDU, UINT16 iTagType, INT32 nIndex, UINT16* piTagDataSize );
extern ETAG_DATATYPE pduGetTagByIndex( EPATH* pPDU, UINT16* piTagType, INT32 nIndex, UINT16* piTagDataSize );
extern INT32 pduGetIndexByType(EPATH* pPDU, UINT16 iTagType, INT32 nIndex);
extern BOOL pduRemoveTagByIndex( EPATH* pPDU, INT32 nIndex );
extern INT32 pduBuild( EPATH* pPDU, UINT8* pPacketPtr );
extern INT32 pduGetBuildLen( EPATH* pPDU );
extern BOOL pduParseNetworkPath( char* szNetworkPath, EPATH* pPDU );
extern void pduAllocateCopy( EPATH* pDestPDU, EPATH* pSrcPDU );

#ifdef EIP_STACK_MEMORY
extern void pduDefragmentMemHeap( EPATH* pPDU, UINT8* pData, UINT16 iDataSize );
#endif

#endif /* #ifndef EIPPDU_H */
