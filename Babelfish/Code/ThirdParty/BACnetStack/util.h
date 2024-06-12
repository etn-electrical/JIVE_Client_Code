//Contents of UTIL.H
//	©2000-2010, PolarSoft® Inc., All Rights Reserved
//----------------------------------------------------------------------------

#ifndef __UTIL_H_INCLUDED
#define __UTIL_H_INCLUDED

#include "core.h"
#include "npool.h"
#include "semaphore.h"

//#define tolower(_c) ( (_c)-'A'+'a' ) //TODO- Pramod

typedef struct _frfifo { 					//a generic fifo
	void	*front;
	void	*rear;
	word		count;
	semaphore	mutex;
	} frfifo;

typedef struct	_frlist {					//a generic linked list
	void		*next;
} frlist;

void bytecpy(byte *,byte *,word);
void byteset(byte *,byte,word);
void bpublic buildfrString(frString *,octet *,bool,byte,byte);		//	***1200
byte bpublic frStringCpy(frString *,frString *,byte );				//	***1200
void *frcpy(void *,void *,unsigned long);							//	***255
int frcmp(void *,void *,unsigned long);								//	***404
bpublic frNpacket *GetQ(frfifo *q);									//	***007, ***256
void  bpublic PutQ(frfifo *, frNpacket *);							//	***007, ***256
#ifndef _WINDOWS
#ifndef _strnicmp						//**300 Begin
int _strnicmp(const char *s1, const char *s2, int n);
#endif
#endif


#endif //__UTIL_H_INCLUDED
