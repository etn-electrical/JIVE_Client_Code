//Contents of ENDIAN.H
#ifndef __ENDIAN_H_INCLUDED
#define __ENDIAN_H_INCLUDED

#include "core.h"

#ifdef _WINDOWS
typedef unsigned long dword;
typedef unsigned short word;
typedef unsigned char byte;
#endif

//Note: if this is little Endian (Intel) architecture then make this flag 0
#define bigEndian	0

#if bigEndian
#define flipw(w)	w
#define flipdw(d)	d
#else
extern unsigned short flipw(unsigned short);
extern unsigned long flipdw(unsigned long);
#endif

#endif //__ENDIAN_H_INCLUDED
