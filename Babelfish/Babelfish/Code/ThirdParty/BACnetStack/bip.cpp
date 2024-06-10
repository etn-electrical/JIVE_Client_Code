/*---------------------------------------------------------------------------
	©1997-2016, PolarSoft® Inc., All Rights Reserved
	
module:		BIP.C
desc:		Windows BACnet/IP Handling for VSBHP
authors:	David M. Fisher, Jack R. Neyer, Coleman L. Brumley
last edit:	07-Jul-2016 [1225] CLB BBMDTTL is word, not dword (matters on BigEndian!)
			26-Mar-2016 [1224] CLB correct endian declarations 
			24-Feb-2016 [1223] CLB Non-Windows socket() can return 0 as valid socket
			03-Jun-2015 [1220] CLB address endian issues in non-windows bipListen
			12-Jul-2012 [ 605] CLB correct calls to debug_error_printf
			19-Jun-2012 [ 604] CLB declare bipListen prototype as static
			21-Sep-2010 [ 260] CLB initialize bipmaxnpdu correctly in bipInit
			22-Apr-2009 [ 259] CLB Don't send extra 4 bytes in bipTransmitBVLC for Linux
			17-Jun-2008 [ 258] CLB Foreign Device registration for Linux
			24-Oct-2007 [ 257] CLB initialize bipmaxapdu correctly (must be <= 1476)
			01-Aug-2007 [ 255] CLB Added Foreign Device Registration
								  Use frcpy, not memcpy
			17-Jan-2007 [ 254] CLB address endian issues
			13-Apr-2006 [ 251] CLB initialize bipmaxapdu correctly
							  	  added some debug code
			03-Oct-2005 [ 250] CLB adapt for Linux
			06-Dec-2003 [ 002] DMF adapt from FR_BIP.C
			29-Oct-2001 [ 001] JRN adapt from FRR_BIP.C for VSB
-----------------------------------------------------------------------------
Note:	This is a cross platform (Linux and Windows) version of this module.
		The Windows code is marked with //Win...Win
		The Linux code is marked with //Linux...Linux, and separated using
			the #ifdef _WINDOWS compiler directive
-----------------------------------------------------------------------------*/ 
#if 0
#ifdef _WINDOWS
//Win---------------------------------------------------------------Win
#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <string.h>
#include <stdio.h>
//Win---------------------------------------------------------------Win
#else
//Linux-------------------------------------------------------------Linux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <memory.h>
#include <errno.h>      
#include <unistd.h>     
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netdb.h>
#include <fcntl.h>
#include <termios.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#endif
//Linux-------------------------------------------------------------Linux
#endif
#include "util.h"
#include "vsbhp.h"
#include "bip.h"
#include "napdu.h"
#include "bvlc.h"			//	***255
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "OS_Tasker.h"

//#ifndef _WINDOWS
//#if defined IP_RECVDSTADDR
/*
# define DSTADDR_SOCKOPT IP_RECVDSTADDR
# define DSTADDR_DATASIZE (CMSG_SPACE(sizeof(struct in_addr)))
# define dstaddr(x) (CMSG_DATA(x))
#elif defined IP_PKTINFO
# define DSTADDR_SOCKOPT IP_PKTINFO
# define DSTADDR_DATASIZE (CMSG_SPACE(sizeof(struct in_pktinfo)))
# define dstaddr(x) (&(((struct in_pktinfo *)(CMSG_DATA(x)))->ipi_addr))
*/
/*
#else
# error "can't determine socket option"
#endif
*/
static void		bipListen(void *);	//	***604
//#endif

///////////////////////////////////////////////////////////////////////
// Global variables
#ifdef _WINDOWS
//Win---------------------------------------------------------------Win
SOCKET		bipsock=INVALID_SOCKET;
static isockaddr	addr={AF_INET,0,INADDR_ANY};
#else
int bipsock=-1;
//static pthread_t	bipThread;
#endif

///////////////////////////////////////////////////////////////////////
//local variables
static word		bipmaxapdu=0,bipmaxnpdu=0;
static bool		bipinitialized=false;
static dword		OurIP=0;						//this computer's IP address
static short		OneSecond=1000;					// 1 second timer			***255
#if bigEndian															//		***1224 Begin
static dword		OurMask = 0xFFFFFF00;			//default is 255.255.255.0	***005
#else
static dword		OurMask = 0x00FFFFFF;			//default is 255.255.255.0	***005
#endif
///////////////////////////////////////////////////////////////////////
//variables also used by other modules
#if bigEndian
word		BACnetIPport = 0xBAC0;					//note: Big Endian!
#else
word		BACnetIPport=0xC0BA;					//note: Big Endian!
#endif																	//		***1224 End
dword		BACnetIPbbmd=0;							//default BBMD IP is 0		***255
//dword		BACnetIPbbmdttl=0;						//registration TTL
word		BACnetIPbbmdttl = 0;						//registration TTL		***1225
UINT8 protocolOperational = 0;		// Prateek //Statusflag to track Protocol status operational
UINT8 monitorTimeout = 0;           // Prateek //Monitor communication timeout
UINT8 BACnetIPForeignDeviceEnabled;

//Win---------------------------------------------------------------Win

#ifndef _WINDOWS													// ***300 Begin
//Linux-------------------------------------------------------------Linux
/* function to open the UDP socket for BACnet/IP */
static int open_bip_socket(int port)
{
	struct sockaddr_in addr;
	int sock_fd = -1;
	int sockopt;
	
	//debug_printf(5,"NET: Opening UDP socket...\n" );
	
	/* Attempt to create a new socket */
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock_fd < 0)  
	{
		//debug_error_printf(__LINE__,__FILE__,"Error opening UDP socket %s\n", strerror(errno));	// ***605
		//exit(-1);
	}
	
	//allow us to get the destination address on the incoming request
	sockopt = 1;
/*	if (setsockopt(sock_fd, IPPROTO_IP, DSTADDR_SOCKOPT, &sockopt, sizeof sockopt) == -1) {
        perror("set dstaddr sockopt");
        exit(EXIT_FAILURE);
    }*/
	

	// Note: We must add this to be able to send and receive broadcasts 
	if (setsockopt(sock_fd,SOL_SOCKET,SO_BROADCAST,&sockopt,sizeof sockopt) == -1) {
       // perror("set bcast sockopt");
       // exit(EXIT_FAILURE);
	}

	/* Success */
	//debug_printf(5,"BIP: UDP socket open on file descriptor %d\n", sock_fd);
	
	/* We need to get the interface address to bind to */
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);  /* set port to listen on */
	
	/* Otherwise, attempt to bind it to and adress */
	if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		//debug_error_printf(__LINE__,__FILE__,"NET: Error binding UDP socket %s\n", strerror(errno));	//	***605
		//exit(-1);
	}
	
	/* Success */
	//debug_printf(5,"BIP: UDP socket listening on local port %d\n", ntohs(addr.sin_port));
	
	return sock_fd;
}
//Linux-------------------------------------------------------------Linux
#endif																// ***300 End


///////////////////////////////////////////////////////////////////////
//	initialize BIP subsystem

bpublic void bipInit(frDevInfo *di)
{	
	//union { dword d; word w; octet b[4];} u;
	//octet	*p;
	//char	b[64];
#ifdef _WINDOWS
//Win---------------------------------------------------------------Win
	WSADATA		wsaData;
	LINGER		ling;
	int			i,optval=1;
	u_long		cmd=1;
//Win---------------------------------------------------------------Win
#endif

	OurIP=ntohl(di->bipipaddr);
	OurMask=ntohl(di->bipipsubnet);
	BACnetIPport=ntohs(di->bipudp);
	bipmaxapdu=maxbip;					//	***251 ***257
	bipmaxnpdu=maxnbip;					//	***260
	BACnetIPbbmd=ntohl(di->bbmdipaddr);		//	***255
	BACnetIPbbmdttl=di->bbmdttl;		//	***255
	//Note:  as of v2.50, I-Ams are no longer handled here nor are they pre-baked
#ifdef _WINDOWS
//Win---------------------------------------------------------------Win
    i=WSAStartup (MAKEWORD(1,1),&wsaData);			//initiate use of WINSOCK32/WS2_32.DLL
    if(i!=0)
    {	sprintf(b,"bipInit: WSAStartup returned %d",i);
		fraLog(b);
		return;
	}
	bipsock=socket(AF_INET,SOCK_DGRAM,0);			//UDP socket
	if(bipsock==INVALID_SOCKET)						//failed
	{	sprintf(b,"bipInit: socket returned %d",WSAGetLastError());;
		fraLog(b);
		return;
	}
// Note: We must add this to be able to send broadcasts and for Win NT to receive broadcasts.
	if(setsockopt(bipsock,SOL_SOCKET,SO_BROADCAST,(char *)&optval,sizeof(optval))==SOCKET_ERROR)
	{	sprintf(b,"bipInit: setsockopt returned %d",WSAGetLastError());;
		fraLog(b);
		return;
	}
	ling.l_onoff=1;									//do a hard close
	ling.l_linger=0;
	setsockopt(bipsock,SOL_SOCKET,SO_LINGER,(char *)&ling,sizeof (ling));

    addr.sin_family=AF_INET;
	addr.sin_port=BACnetIPport;
	addr.sin_addr.s_addr=OurIP;						//use this IP				***423 Begin

	i=bind(bipsock,(SOCKADDR *)&addr,sizeof(addr));	//bind a local address with a socket
    if (i==SOCKET_ERROR)
	{	sprintf(b,"bipInit: bind returned %d",WSAGetLastError());;
		fraLog(b);
		return;
	}
	ioctlsocket(bipsock,FIONBIO,&cmd);
	bipinitialized=true;
//Win---------------------------------------------------------------Win
#else
	if (bipsock==-1)
	{
		bipsock=open_bip_socket(di->bipudp);
		if(bipsock >= 0)													//		***1223
		{
			bipinitialized=true;
			sys_thread_new(BIP_THREAD_NAME, bipListen, NULL, BIP_THREAD_STACKSIZE, BIP_THREAD_PRIO);
		}
	}
/*	else
		debug_error_printf(__LINE__,__FILE__,"bipInit: socket error\n");	//	***605
	if(pthread_create(&bipThread,NULL,(void *)&bipListen,NULL))
	{	
		printf("bipInit: failed to create listener thread");
		exit(-1);
	} */
#endif
}

///////////////////////////////////////////////////////////////////////
//	BIP shutdown called from application

/*bpublic void bipDeinit(void)
{
#ifdef _WINDOWS
//Win---------------------------------------------------------------Win
    if(bipsock!=INVALID_SOCKET)
	    closesocket(bipsock);						//close an existing socket
    if(bipinitialized)
		WSACleanup();								//terminate use of the WINSOCK32/WS2_32.DLL
//Win---------------------------------------------------------------Win
#else
	pthread_cancel(bipThread);
	close(bipsock);
#endif
}*/

///////////////////////////////////////////////////////////////////////
//	get our mac address
//
//in:	maddr	points to where mac address goes
//out:	length of mac address

bpublic byte bipGetAddr(octet *maddr)
{	union { dword d; word w; octet b[4];} u;
	bytecpy(maddr,(byte *)&OurIP,4);
	u.w=BACnetIPport;
#if bigEndian
	maddr[4]=u.b[0];								//big-endian port
	maddr[5]=u.b[1];
#else
	maddr[4]=u.b[1];								//big-endian port
	maddr[5]=u.b[0];
#endif												//												***230 End
	return 6;
}

///////////////////////////////////////////////////////////////////////
//	get max apdu length
//
//out:	the length

bpublic word bipMaxAPDU(void)
{
	return bipmaxapdu;
}

///////////////////////////////////////////////////////////////////////
//	get max npdu length
//
//out:	the length

bpublic word bipMaxNPDU(void)
{
	return bipmaxnpdu;
}

///////////////////////////////////////////////////////////////////////
//	get max response pdu length
//
//out:	the length

bpublic word bipMaxRPDU(void)
{
	return bipmaxapdu-1;							//response could include hopcount so it is one less
}

///////////////////////////////////////////////////////////////////////
//	try to transmit data (like a DL-UNITDATA.request)
//
//in:	da			the destination bip address to send to (6 octets) (NULL=Broadcast)
//		dlen		the length of the data (=0 means send IAm)
//		tbuf		has the data to transmit

bpublic void bipTransmit(octet *da, word dlen, octet *tbuf)
{
//Win---------------------------------------------------------------Win
#ifdef _WINDOWS
	isockaddr		toaddr;
//Win---------------------------------------------------------------Win
#else
	struct sockaddr_in toaddr;						//								***300
#endif

	int				n;
	bvlc			rb;								//max sized receive buffer
	union { word w; byte b[2];} u;

    if (!bipinitialized) return;					//								***423
	rb.type=0x81;									//BACnet/IP
	if(da==NULL)									//it's broadcast
	{	rb.function=Original_Broadcast_NPDU;
#ifdef _WINDOWS
		toaddr.sin_addr.s_addr=(u_long)((OurIP&OurMask)|(~OurMask));
		toaddr.sin_port=BACnetIPport;
#else
		toaddr.sin_addr.s_addr=(OurIP&OurMask)|(~OurMask); //OurMask; //no need to calculate the bcast addr!!!
		//toaddr.sin_addr.s_addr=OurMask; //no need to calculate the bcast addr!!!
		toaddr.sin_port=BACnetIPport;
		protocolOperational = 1;   // Protocol Status set to operation when I am transmitted.
#endif
		
	}
	else											//unicast
	{	rb.function=Original_Unicast_NPDU;
		bytecpy((byte *)&toaddr.sin_addr,da,4);
#if bigEndian										//												***230 Begin
		u.b[0]=da[4];								//da has big endian port
		u.b[1]=da[5];
#else
		u.b[1]=da[4];								//da has big endian port
		u.b[0]=da[5];
#endif												//												***230 End
		toaddr.sin_port=u.w;						//but we need it little endian
		monitorTimeout = 1;
	}
    toaddr.sin_family=AF_INET;
	rb.length=flipw((word)(dlen+4));
	bytecpy(rb.buf,tbuf,dlen);						//copy buffer from caller
//Win---------------------------------------------------------------Win
#ifdef _WINDOWS
	n=sendto(bipsock,(char *)&rb,dlen+4,0,(SOCKADDR *)&toaddr,sizeof(toaddr));
//Win---------------------------------------------------------------Win
#else
	n=sendto(bipsock,(unsigned char*)&rb,dlen+4,0,(struct sockaddr*)&toaddr,sizeof(struct sockaddr));
	//debug_printf(5,"BIP: sent %d bytes\n",n);
	if (n < 0)
	{
		//perror("BIP");
	}
#endif
	fd_transmit((word)(dlen+4),&rb);		// Distribute_Broadcast_To_Network			//	***255

}

///////////////////////////////////////////////////////////////////////						***255 Begin
//	try to transmit a fully formed BVLC to a non-broadcast address
//    
//  Note: bipTransmit sends only Original_Broadcasts and Original_Unicasts
//
//in:	da			the destination bip address to send to (6 octets)
//		dlen		the length of the data
//		tbuf		has the data to transmit

bpublic void bipTransmitBVLC(octet *da, word dlen, octet *tbuf)
{	isockaddr		toaddr;
	static bvlc			tb;			// Fixed DH1-183					//max sized transmit buffer
	union { word w; byte b[2];} u;	

	frcpy(&toaddr.sin_addr,da,4);
	u.b[0]=da[4];
	u.b[1]=da[5];
	frcpy((byte *)&tb,tbuf,dlen);							//copy buffer from caller
    toaddr.sin_family=AF_INET;
	toaddr.sin_port=u.w;

#ifdef _WINDOWS
	sendto(bipsock,(char *)&tb,dlen,0,(SOCKADDR *)&toaddr,sizeof(toaddr));
#else
	sendto(bipsock,(unsigned char*)&tb,dlen,0,(struct sockaddr*)&toaddr,sizeof(struct sockaddr));	//	***259
#endif
}																					//		***255 End

///////////////////////////////////////////////////////////////////////
//	Main Loop (call this frequently)
//
//in:	actualperiod	the time in milliseconds that bipWork actually gets called.

bpublic void bipWork(byte actualperiod)
{
#ifdef _WINDOWS
	isockaddr		from;
	int				n,fromlen=sizeof(from);
	octet			*prb,sadr[6];
	dword			*padr;
	bvlc			rb;								//max sized receive buffer
	union { dword dw; word w[2]; byte b[4];} u;
	union { byte b[8]; word w[4]; dword d[2];} sa;	//from address					***501
#endif											//		***258 Begin

    if (!bipinitialized) return;					//nothing to do

	OneSecond-=actualperiod;						// maintain FD registration ***255 Begin
	if(OneSecond<=0)
	{
		OneSecond=1000;
		if(BACnetIPForeignDeviceEnabled)
		{
		   fd_second();
		}
	}												//							***255 End

#ifdef _WINDOWS										//		***258 End
	n=recvfrom(bipsock,(char *)&rb,sizeof(rb),0,(SOCKADDR *)&from,&fromlen);
	if( n==SOCKET_ERROR||							//nothing cookin, or
		rb.type!=0x81)								//we only do BACnet/IP
		return;										//in all these cases we toss it
	padr=(dword *)&from.sin_addr;
	if (*padr==OurIP) return;						//we've heard our own broacast
	rb.length=flipw(rb.length);
	if(rb.length<4||rb.length!=n) return;			//can't be valid				*** 255
	prb=&rb.buf[0];
	memmove(sa.b,&from.sin_addr.s_addr,4);			//this gets IP address			**255 Begin
	sa.w[2]=from.sin_port;							//this is the UDP part
	//at this point sa.b[0]...b[5] has the source BIP address as an octet string
	bvlcWork(&rb,sa.b);								//pass on to bbmd				***255 End
	switch(rb.function)								//eventually we will handle all the functions
	{
	case Forwarded_NPDU:
		n=rb.length-10;
		bytecpy(sadr,prb,6);
		prb+=6;										//skip over source address
		goto rnpdu;
	case Original_Broadcast_NPDU:
													//fall through intentionally
	case Original_Unicast_NPDU:
		n=rb.length-4;
		bytecpy(sadr,(byte *)&from.sin_addr,4);
		u.w[0]=from.sin_port;						//we get little endian port
#if bigEndian										//												***230 Begin
		sadr[4]=u.b[0];								//sadr needs big endian
		sadr[5]=u.b[1];
#else
		sadr[4]=u.b[1];
		sadr[5]=u.b[0];
#endif												//												***230 End
rnpdu:	macReceiveIndicate(0,portBIP,6,sadr,n,prb);
		break;
	default:
		break;										//unhandled ones are ignored
	}
#endif
}

#ifndef _WINDOWS
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
static void bipListen(void *p)
{	
	fd_set 			read_fds;
	//struct timeval	select_timeout={0};
	struct sockaddr_in 	from={0};
	socklen_t		addrlen=0;
	word			nb=0;
	octet			*prb,sadr[6];
	bvlc			rb={0};								//max sized receive buffer
	union { dword dw; word w[2]; byte b[4];} u;
#if DEBUGVSBHP
	char t[1024];
	char b[128];
	int i;
#endif
	union { byte b[8]; word w[4]; dword d[2];} sa;	//from address					***501
	
	while(true)
	{	
		//select_timeout.tv_sec = 1;					//wait a second if no activity 	***258 
		//select_timeout.tv_usec = 0;					//				***258
		
		FD_ZERO(&read_fds);							//clear the file handle set 
		FD_SET(bipsock, &read_fds);					//UDP interface 
//		if(select(bipsock+1, &read_fds, NULL, NULL, &select_timeout) > 0)	//	***258
		if(select(bipsock+1, &read_fds, NULL, NULL, NULL) > 0)			//	***258
		{	
			if(FD_ISSET(bipsock, &read_fds))		//activity on UDP socket 
			{	
				addrlen = sizeof( struct sockaddr_in );
				nb = recvfrom(bipsock, &rb, sizeof(rb), 0, (struct sockaddr *)&from, &addrlen);
				rb.length=flipw(rb.length);
				if(rb.type==0x81&&					//we only do BACnet/IP, and
					rb.length>=4&&
					rb.length==nb)					//length appears valid
				{
#ifdef DEBUGVSBHP				//	DEBUG CODE ***251 Begin
					sprintf(t,"bipListen: got %u bytes rb.length=%d rb.function=%d\r\n",nb,rb.length,rb.function);
					fraLog(t);
					t[0]=(char)0;
					prb=(octet *)&rb;
					for(i=0;i<nb;i++) 
					{
						sprintf(b,"%.2X ",prb[i]);
						strcat(t,b);						
					}
					if(strlen(t)<1022) 
						strcat(t,"\r\n");
					fraLog(t);
					//printf("\n");
#endif							//	DEBUG CODE ***251 End
					prb=&rb.buf[0];
					memmove(sa.b,&from.sin_addr.s_addr,4);			//this gets IP address			**255 Begin
					sa.w[2]=from.sin_port;							//this is the UDP part
					//at this point sa.b[0]...b[5] has the source BIP address as an octet string
					bvlcWork(&rb,sa.b);								//pass on to bbmd				***255 End
					switch(rb.function)								//eventually we will handle all the functions
					{
					case Forwarded_NPDU:
						nb=rb.length-10;
						frcpy(sadr,prb,6);
						prb+=6;										//skip over source address
						goto rnpdu;
					case Original_Broadcast_NPDU:
						//fall through intentionally
					case Original_Unicast_NPDU:
						nb=rb.length-4;
						frcpy(sadr,&from.sin_addr.s_addr,4);
						u.w[0]=from.sin_port;						//we get big endian port
#if bigEndian										//												***1220 Begin
						sadr[4]=u.b[0];								//sadr needs big endian
						sadr[5]=u.b[1];
#else
						sadr[4]=u.b[1];
						sadr[5]=u.b[0];
#endif												//												***1220 End
rnpdu:					macReceiveIndicate(0,portBIP,6,sadr,nb,prb);	//at this point sa.b[0]...b[5] has the source BIP address as an octet string
						break;
					default:
						break;										//unhandled ones are ignored
					}
				}
			}
		}
	}
	//	printf("bipListen: terminating\n");
}
#endif


    
