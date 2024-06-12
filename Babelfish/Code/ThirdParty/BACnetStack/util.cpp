/*--------------------------------------------------------------------
	©2001-2016, PolarSoft® Inc., All Rights Reserved
	
module:		UTIL.C
desc:		Utility Routines for VSBHP
author:		David M. Fisher, Jack Neyer
last edit:	05-Dec-2016 [1229] CLB correct frStringCpy for empty strings
								   clean up comment formating
			26-Mar-2016 [1224] CLB corrections to tolower
								   added pragma to disable 4142 warnings
			10-Mar-2014 [1205] DMF added tolower for non-Windows
			01-Aug-2011 [ 257] CLB correct call to debug_error_printf
			24-Oct-2007 [ 256] CLB PutQ and GetQ use frNpacket 
									to avoid unecessary casting
			24-Oct-2007 [ 255] CLB frcpy introduction								  
			13-Apr-2006 [ 250] CLB Adapt for Linux
			09-Dec-2003 [ 002] DMF adapt from CBG
			11-May-2001 [ 001] DMF First Cut
----------------------------------------------------------------------*/
#include "util.h"
#include "npool.h"
#include "core.h"	
/*
#ifdef DEBUGVSBHP
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#ifndef _WINDOWS
#include <fcntl.h>
#include <termios.h>
#include <limits.h>
#include <unistd.h>     
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif
*/


///////////////////////////////////////////////////////////////////////
//	Our own memory copy function
//
//  in:	dest	the destination buffer
//		src		the source buffer
//		count	the number of bytes to copy
//  out: pointer to the dest

bpublic void *frcpy(void *dest,void *src,unsigned long count)
{	unsigned long	i;
	void			*r=dest;
	unsigned char	*d=(unsigned char *)dest,*s=(unsigned char*)src;	//C++

	for (i=0;i<count;i++)
		*d++=*s++;
	return r; 
}

///////////////////////////////////////////////////////////////////////
//	Our own memory compare function
//
//  in:	buf1	first buffer
//		buf2	second buffer
//		count	the number of bytes to compare
//  out: <0		buf1<buf2
//		 =0		buf1==buf2
//		>0		buf1>buf2

int bpublic frcmp(void *buf1,void *buf2,unsigned long count)
{	int		r=0;
	unsigned long	i;
	unsigned char	*b1=(unsigned char*)buf1,*b2=(unsigned char*)buf2;

	for (i=0;i<count;i++)
	{	if (*b1!=*b2)
		{	r=(*b1<*b2)?-1:1;
			break;
		}
		b1++;
		b2++;
	}
	return r;
}

///////////////////////////////////////////////////////////////////////
//	Build an frString
//in:	fp		pointer to an frString structure
//		dp		pointer to a data buffer
//		ro		true for readonly, otherwise false
//		len		length of the buffer
//		ml		maxlen of the buffer
void bpublic buildfrString(frString *fp,octet *dp,bool ro,byte len,byte ml)
{
	if(!dp) return;
	if(!fp) return;
	fp->text=(char*)dp;
	fp->len=len;
	fp->maxlen=ml;
	if(ro) fp->readonly=true;
	else fp->readonly=false;
}

///////////////////////////////////////////////////////////////////////
//	Copy an frString from src to dest
//in:	dest		pointer to the src frString structure
//		src		pointer to a data buffer
//		len		bytes to copy
//out:	byte	bytes copied
//		0x00	for failure
byte bpublic frStringCpy(frString *dest,frString *src,byte len)
{
	if((!src) || (!dest)) 
		return 0x00;
	if((!src->text) || (!dest->text)) 
		return 0x00;
	if((!len) || (len>dest->maxlen) || (dest->maxlen<src->len))
	{
		if((!len)&&(dest->readonly==false))															//	***1229 Begin
		{
			byteset((byte*)(dest->text),0x00,dest->maxlen);
			dest->len=0;
		}																							//	***1229 End
		return 0x00;												//no length or dest doesn't have enough room
	}
	if(dest->readonly) return 0x00;			//can't overwrite a readonly string
	byteset((byte*)(dest->text),0x00,dest->maxlen);
	bytecpy((byte*)(dest->text),(byte*)(src->text),src->len);
	dest->len=src->len;						//IMPORTANT, do not change dest maxlen!!!
	return dest->len;
}

///////////////////////////////////////////////////////////////////////
//	our own bytecpy
//in:	ds		pointer to destination buffer
//		ss		pointer to source buffer
//		n		the count to move

void bpublic bytecpy(byte *ds,byte *ss,word n)
{	word	i;
	for(i=0;i<n;i++) *ds++=*ss++;
}

///////////////////////////////////////////////////////////////////////
//	our own byteset
//in:	ds		pointer to destination buffer
//		v		value to set
//		n		the count to set

void bpublic byteset(byte *ds,byte v,word n)
{	word	i;
	for(i=0;i<n;i++) *ds++=v;
}

///////////////////////////////////////////////////////////////////////
//	Add a packet to a frfifo
//
//in:	q		points to the frfifo to insert into
//		p		points to the packet to be inserted

void bpublic PutQ(frfifo *q, frNpacket *p)		//	***255
{	
	frNpacket	far *lg;//=q->rear;				//lg is the last guy in present queue

	semaCapture(&q->mutex);
    lg=(frNpacket*)q->rear;									//	**423, do it within the lock
	if(lg==NULL)								//p is the first and only guy
		q->front=p;								//so front also points to him
	else										//in all other cases, rear guy must point to new guy
		lg->next=p;
	q->rear=p;									//new guy is now the last one
	p->next=NULL;
	q->count++;
	semaRelease(&q->mutex);
}

///////////////////////////////////////////////////////////////////////
//	remove a packet from a frfifo
//
//in:	q		points to the frfifo to remove from
//out:	NULL	it's empty
//		else	pointer to a packet

bpublic frNpacket *GetQ(frfifo *q)		//	***255, return an frNpacket *
{	
	frNpacket	*fg;								//fg is the first guy in present queue		
	semaCapture(&q->mutex);							//capture queue semaphore
    fg=(frNpacket*)q->front;									//get a lock first	***254
	if(fg==NULL)									
	{
		semaRelease(&q->mutex);
		return NULL;								//empty
	}												//	***423 End
	if(--q->count==0) q->rear=NULL;
	q->front=fg->next;
	semaRelease(&q->mutex);
	return fg;										//return the first one
}													//							***103 End

#ifndef _WINDOWS
#ifndef tolower										//							***1205 Begin
char tolower(char c)
{
	return (c>='A'&&c<='Z')?(c+('a'-'A')):c;          //							***1224
}
#endif												//							***1205 End
#ifndef _strnicmp									//							**300 Begin
int _strnicmp(const char *s1, const char *s2, int n)
{
    int i;
    char c1, c2;
    for (i=0; i<n; i++)
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (!c1) return 0;
    }
    return 0;

} 
#endif												//							**300 End

//
//Relevent network ioctl functions
//
//SIOCGIFADDR:       /* Get interface address */
//SIOCGIFBRDADDR:    /* Get the broadcast address */
//SIOCGIFNETMASK:    /* Get the netmask for the interface */
/*bpublic void get_local_ip_address(char *interface_name,char *addr)
{
   struct ifreq ifr;
   unsigned char dstaddr[10];
   int fd;
   strcpy(ifr.ifr_name, interface_name );
   fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
   if(ioctl(fd, SIOCGIFADDR, &ifr) != -1)
   {
     memcpy(dstaddr, ifr.ifr_dstaddr.sa_data, 6);
     sprintf(addr,"%d.%d.%d.%d", dstaddr[2]&0xFF, dstaddr[3]&0xFF,dstaddr[4]&0xFF,dstaddr[5]);
     debug_printf(5,"NET: %s IP addr is %s\n",interface_name,addr);
   }
   else
   {
      debug_error_printf(__LINE__,__FILE__,"Unable to determine IP address.\n");	//	***257
      exit(-1);
   }
   return;
}*/

/* Determines the broacast address for a given interface */
/*bpublic void get_bcast_address(char *interface_name,char *addr)
{
   struct ifreq ifr;
   unsigned char dstaddr[10];
   int fd;
   strcpy(ifr.ifr_name, interface_name );
   fd = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
   if(ioctl(fd, SIOCGIFBRDADDR, &ifr) != -1)
   {
     memcpy(dstaddr, ifr.ifr_dstaddr.sa_data, 6);
     sprintf(addr,"%d.%d.%d.%d", dstaddr[2]&0xFF, dstaddr[3]&0xFF,dstaddr[4]&0xFF,dstaddr[5]&0xFF);
     debug_printf(5,"%s Broadcast IP addr is %s\n",interface_name,addr);
   }
   else
   {
      debug_error_printf(__LINE__,__FILE__,"Unable to determine broadcast address.\n");	//	***257
      exit(-1);
   }
   return;
}*/

#endif



