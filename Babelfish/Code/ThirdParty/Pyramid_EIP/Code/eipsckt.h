/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPSCKT.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Using BSD socket library to read and write TCP and UDP packets
**
*****************************************************************************
*/


#ifndef EIPSCKT_H
#define EIPSCKT_H

#define ENCAP_SERVER_PORT				0xaf12			/* Ethernet/IP predetermined TCP port */
#define CLASS1_UDP_PORT					0x8ae			/* Ethernet/IP predetermined UDP port */

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#define MALFORMED_PACKET_ERROR			0x1FFFF

#define CIP_MCAST_BASE					0xefc00100		/* 239.192.1.0 - beginning of the multicast address space */
#define CIP_MCAST_BSHIFT				5				/* Shift for the above */
#define CIP_MCAST_HMASK					0x000003ff		/* Host mask, 10 bits */
#define NUM_MCAST_BLOCKS				64				/* Address Block Size */
#define CIP_MAX_NUM_MULTICAST_ADDRESS	32				/* Maximum number of multicast address for a device */

#define ENCAPH_SIZE						24				/* Gets around alignment */

#ifdef EIP_LARGE_MEMORY
/* Maximum packet sizes allowed by EtherNet/IP spec */
#define MAX_TCP_PACKET_SIZE		0xFFFF
#define MAX_UDP_PACKET_SIZE		0xFFFF
#else

/* Typical packet size used in EtherNet/IP applications*/

/* Max size of a packet needed for a connected explicit message to retreive assembly data (attribute 3) */
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
#define SCANNER_ASSEMBLY_SIZE ((INPUT_SCANNER_ASSEMBLY_SIZE > OUTPUT_SCANNER_ASSEMBLY_SIZE) ? INPUT_SCANNER_ASSEMBLY_SIZE : OUTPUT_SCANNER_ASSEMBLY_SIZE)
#define MAX_ASSEMBLY_PACKET_SIZE (SCANNER_ASSEMBLY_SIZE+ENCAPH_SIZE+6+sizeof(UINT16)+(CPF_TAG_SIZE*2)+14)
#else
#define MAX_ASSEMBLY_PACKET_SIZE (MAX_ASSEMBLY_SIZE+ENCAPH_SIZE+6+sizeof(UINT16)+(CPF_TAG_SIZE*2)+14)
#endif
#define MAX_TCP_PACKET_SIZE	(MAX_ASSEMBLY_PACKET_SIZE > 600 ? MAX_ASSEMBLY_PACKET_SIZE : 600)
#define MAX_UDP_PACKET_SIZE	(MAX_ASSEMBLY_SIZE + ENCAPH_SIZE)
#endif 

#define MAX_PACKET_SIZE (MAX_UDP_PACKET_SIZE > MAX_TCP_PACKET_SIZE ? MAX_UDP_PACKET_SIZE : MAX_TCP_PACKET_SIZE)   
#define MAX_DATA_FIELD_SIZE	(MAX_PACKET_SIZE-ENCAPH_SIZE-ENCAP_DT_HDR_SIZE-(CPF_TAG_SIZE*2)-sizeof(UINT16))
#define MAX_UCMM_SIZE	504


/* Return TRUE if this is a multicast address, FALSE otherwise */
#define IS_MULTICAST(i) (((UINT32)(i) & ntohl(0xf0000000)) == ntohl(0xe0000000))

extern UINT32 gnHostIPAddrCount;					/* Number of host addresses */
extern UINT32 galHostIPAddr[MAX_HOST_ADDRESSES];	/* Host address array */

extern UINT32 gnClaimedHostIPAddrCount;				/* Number of claimed host addresses */

#ifdef EIP_LARGE_BUF_ON_HEAP
extern UINT8* gmsgBuf;								/* Buffer storing incoming and outgoing packets */
#else
extern UINT8 gmsgBuf[MAX_PACKET_SIZE];				/* buffer to store incoming and outgoing packets */
#endif

#define COMMON_BUFFER_SIZE			128000			/* Reserve buffer size in bytes for shared UDP use */
#define SMALL_BUFFER_SIZE			4096			/* Reserve buffer size in bytes for shared UDP use */

#define PARTIAL_PACKET_TIMEOUT		1000

extern void socketInit(void);

#ifndef EIP_CUSTOM_SOCKET
extern void socketPostListen(SOCKET* pWaitSocket, UINT32 lClaimedHostIPAddr, UINT16 iPort);
extern SOCKET socketClass1Init( BOOL bBroadcast, UINT32 lBufferSize, UINT32 lHostIPAddress, UINT16 iPort, BOOL bConsumeMcast );
extern void socketCheckIncomingSession(void);
extern INT32 socketClass1Recv(SOCKET lClass1Socket, UINT32 lHostIp );
extern void socketGetBroadcasts(void);

#if defined(ET_IP_SCANNER) || defined (TARGET_RECEIVE_MULTICAST)
extern SOCKET socketJoinMulticastGroup(CONNECTION* pConnection);
extern SOCKET socketDropMulticastGroup(CONNECTION* pConnection);
#endif

#endif /* EIP_CUSTOM_SOCKET */

#if !defined SYNCHRONOUS_CONNECTION && !defined EIP_NO_ORIGINATE_UCMM
extern BOOL socketStartTCPConnection( SESSION* pSession );
extern BOOL socketContinueTCPConnection( SESSION* pSession, BOOL* bTCPConnectionSuccessfullyCompleted );
#endif

extern INT32 socketClass1Send( CONNECTION*, INT32 len );
extern INT32 socketEncapRecv( SESSION* pSession );
extern BOOL socketEncapSendData(SESSION* pSession);
extern BOOL socketEncapSendPartial(SESSION* pSession);

extern INT8* socketGetAddressString( UINT32 lIPAddress );
extern void socketHandleBroadcasts(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig, struct sockaddr_in* pClientAddr, UINT8* pBuf, INT32 lBytesReceived );
extern BOOL socketSendBroadcasts(TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig, ENCAPH* encap);
extern void socketSendBroadcast(ENCAPH* psEncapHdr, TCPIP_INTERFACE_INSTANCE_ATTRIBUTES* pTcpIpConfig, 
								struct sockaddr_in* pClientAddr);

#endif  /* EIPSCKT_H */
