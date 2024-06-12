/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPUTIL.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** General utilities.
**
*****************************************************************************
*/

#ifndef EIPUTIL_H
#define EIPUTIL_H

#define IS_TICK_GREATER(tick1, tick2)  ( (tick1 < 0xffffff && tick2 > 0xff000000) ? TRUE : ( (tick2 < 0xffffff && tick1 > 0xff000000) ? FALSE : (tick1 > tick2) ) )
#define IS_TICK_GREATER_OR_EQUAL(tick1, tick2)  ( (tick1 < 0xffffff && tick2 > 0xff000000) ? TRUE : ( (tick2 < 0xffffff && tick1 > 0xff000000) ? FALSE : (tick1 >= tick2) ) )
#define MAX_TICK(tick1, tick2)    ( IS_TICK_GREATER(tick1, tick2) ? tick1 : tick2 )

#define BUFFER_INTERSECT(start1, size1, start2, size2) ( !( ((start1)+(size1)) <= (start2) || (start1) >= ((start2)+(size2)) ) )

#ifdef EIP_STACK_MEMORY

#ifdef EIP_LARGE_MEMORY
#define MEMORY_POOL_SIZE		((0xffff)*24)
#else
#define MEMORY_POOL_SIZE		(0xffff)
#endif

/* Union is only needed to ensure compiler alignes the array at a multiple of 4*/
typedef union MEMORYPOOL
{
#ifdef EIP_LARGE_BUF_ON_HEAP
	UINT8* au8_array;
#else
	UINT8 au8_array[MEMORY_POOL_SIZE];
#endif
	UINT32 u32_dummy;  /* not accessed but used to force compiler to align byte-buffer */
} MEMORYPOOL;

#endif

#ifdef EIP_LARGE_BUF_ON_HEAP
extern UINT8* EtIP_mallocHeap( UINT16 iDataSize );
#endif

extern UINT8* EtIP_malloc( UINT8* data, UINT16 iDataSize );
extern void EtIP_free( UINT8** piData, UINT16* piDataSize );
extern void EtIP_realloc( UINT8** piData, UINT16* piDataSize, UINT8* data, UINT16 iDataSize );
extern void utilInit(void);
extern BOOL utilParseNetworkPath( char* szNetworkPath, BOOL* pbLocal, char* extendedPath, UINT16* piExtendedPathLen );
extern BOOL utilBuildNetworkPath( char* szNetworkPath, UINT32 lIPAddr, EPATH* pPDU );
extern INT32 utilCheckNetworkPath(const char* networkPath );
extern INT32 utilCheckHostIPAddress( UINT32 lHostIPAddr );
extern UINT32 utilGetUniqueID(void);
extern BOOL utilIsIDUnique(UINT32 lVal);
extern UINT32 utilCalculatePseudoRandomDelay(UINT32 lMaxWaitTime);
#if defined EIP_FILE_OBJECT
extern BOOL utilComputeFileSizeAndChecksum(char* pstrFilename, UINT32* pFileSize, INT16* piChecksum);
#endif

#endif /* #ifndef EIPUTIL_H */
