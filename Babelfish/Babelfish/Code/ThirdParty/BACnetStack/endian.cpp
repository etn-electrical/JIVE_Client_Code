/*---------------------------------------------------------------------------
	©2003-2010, PolarSoft® Inc., All Rights Reserved
	
module:		ENDIAN.C
desc:		big/little endian resolver
author:		David M. Fisher
last edit:	01-Apr-2003 [001] DMF First Cut
-----------------------------------------------------------------------------------------*/ 

#include "endian.h"

#if bigEndian==0

//flip a word around on a little endian machine
bpublic  word flipw(word w)
{	union { word w; byte b[2];} u;
	byte	b;
	u.w=w;
	b=u.b[0];
	u.b[0]=u.b[1];
	u.b[1]=b;
	return u.w;
}

bpublic dword flipdw(dword dw)
{	union { dword dw; byte b[4];} u;
	byte	b;
	u.dw=dw;
	b=u.b[0];
	u.b[0]=u.b[3];
	u.b[3]=b;
	b=u.b[1];
	u.b[1]=u.b[2];
	u.b[2]=b;
	return u.dw;
}
#endif

