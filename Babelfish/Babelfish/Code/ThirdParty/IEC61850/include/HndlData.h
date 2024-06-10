/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/** @HndlData.h
 *
 *  This file contains declarations for default Handler Functions
 *
 *  for the basic MMS datatypes from sclclasses/ed2primitives.xml
 *
 */

#ifndef HNDLDATA_H_
#define HNDLDATA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Error Handlers, for read- and write-only variables */
TMW_CLIB_API int  MMSd_readFailure(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeFailure(void *N, void *D, int size, void *parameter, void *E);
TMW_CLIB_API int  MMSd_readDenied(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeDenied(void *N, void *D, int size, void *parameter, void *E);

TMW_CLIB_API int  MMSd_component( char *name, int len, void *E );
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrStructure( void *N, int len, void *parameter, void *E );
#else
TMW_CLIB_API int  MMSd_attrStructure( int len, void *parameter, void *E );
#endif


/* Boolean Data Types */
TMW_CLIB_API int  MMSd_readBoolean(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeBoolean(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrBoolean(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrBoolean( int len, void *parameter, void * E );
#endif

/* BitString Data Types */
TMW_CLIB_API int  MMSd_readBitString(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeBitString(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrBitString(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrBitString( int len, void *parameter, void * E );
#endif

/* Integer Data Types */
TMW_CLIB_API int  MMSd_readInteger(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeInteger(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrInteger(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrInteger( int len, void *parameter, void * E );
#endif

TMW_CLIB_API int  MMSd_readInteger2Any( void * N, int size, void * parameter, void * E );
TMW_CLIB_API void MMSd_writeInteger2Any( void * N, void *D, int size, void * parameter, void * E );

/* Long Integer Data Types */
TMW_CLIB_API int  MMSd_readLongInteger(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeLongInteger(void *N, void *D, int size, void *parameter, void *E);

TMW_CLIB_API int  MMSd_readLong64Integer(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeLong64Integer(void *N, void *D, int size, void *parameter, void *E);

#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrLongInteger(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrLongInteger( int len, void *parameter, void * E );
#endif

/* Unsigned Data Types */
TMW_CLIB_API int  MMSd_readUnsigned(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeUnsigned(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrUnsigned(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrUnsigned( int len, void *parameter, void * E );
#endif

/* Unsigned Long Data Types */
TMW_CLIB_API int  MMSd_readLongUnsigned(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeLongUnsigned(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrLongUnsigned( void *N, int len, void *parameter, void * E );
#else
TMW_CLIB_API int  MMSd_attrLongUnsigned( int len, void *parameter, void * E );
#endif

/* Float Data Types */
TMW_CLIB_API int  MMSd_readFloat(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeFloat(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrFloat(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrFloat( int len, void *parameter, void * E );
#endif

TMW_CLIB_API int  MMSd_readFloat64( void *N, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeFloat64( void *N, void *D, int size, void *parameter, void *E );

/* ASN.1 Real Data Types */
#ifdef MMSD_REAL_TYPE
/* Currently unimplemented. Use FLOAT instead. */
#endif

/* OctetString Data Types */
TMW_CLIB_API int  MMSd_readOctetString(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeOctetString(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrOctetString(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrOctetString( int len, void *parameter, void * E );
#endif

/* VisibleString Data Types */
#ifdef MMSD_STRING_TYPE
TMW_CLIB_API int  MMSd_readString(void *N, int size, void *parameter, void *E);
TMW_CLIB_API void MMSd_writeString(void *N, void *D, int size, void *parameter, void *E);
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrString(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrString( int len, void *parameter, void * E );
#endif
#endif

/* GeneralizedTime Data Types */
#ifdef MMSD_GENERALIZEDTIME_TYPE
TMW_CLIB_API int  MMSd_readGeneralTime( void *N, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeGeneralTime( void *N, void *D, int size, void *parameter, void *E );
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrGeneralTime( void *N, int len, void *parameter, void * E );
#else
TMW_CLIB_API int  MMSd_attrGeneralTime( int len, void *parameter, void * E );
#endif
#endif

/* BinaryTime Data Types */
#ifdef MMSD_BINARYTIME_TYPE
TMW_CLIB_API int  MMSd_readBtime( void *N, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeBtime( void *N, void *D, int size, void *parameter, void *E );
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrBtime(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrBtime( int len, void *parameter, void * E );
#endif
#endif

/* UtcBinaryTime Data Types */
#ifdef MMSD_UTCTIME_TYPE
TMW_CLIB_API int  MMSd_readUtcBtime( void *N, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeUtcBtime( void *N, void *D, int size, void *parameter, void *E );
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrUtcBtime(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrUtcBtime( int len, void *parameter, void * E );
#endif
#endif

/* BCD Data Types */
#ifdef MMSD_BCD_TYPE
TMW_CLIB_API int  MMSd_readBCD( void *N, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeBCD( void *N, void *D, int size, void *parameter, void *E );
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrBCD( void *N, int len, void *parameter, void * E );
#else
TMW_CLIB_API int  MMSd_attrBCD( int len, void *parameter, void * E );
#endif
#endif

/* ObjectIdentifier Data Types */
#ifdef MMSD_OBJECT_IDENTIFIER_TYPE
TMW_CLIB_API int  MMSd_readObjectId( void *N, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeObjectId( void *N, void *D, int size, void *parameter, void *E );
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrObjectId( void *N, int len, void *parameter, void * E );
#else
TMW_CLIB_API int  MMSd_attrObjectId( int len, void *parameter, void * E );
#endif
#endif


TMW_CLIB_API int  MMSd_readObjectName( void *N, int size, void *parameter, void *E ) ;
TMW_CLIB_API void MMSd_writeObjectName(void *N, void *D, int size, void *parameter, void *E);
TMW_CLIB_API int  MMSd_attrObjectName(void *N, int size, void *parameter, void *E);

TMW_CLIB_API int  TMW_readMod(void *N, int size, void *parameter, void *E);

TMW_CLIB_API int  TMW_readBeh(void *N, int size, void *parameter, void *E);

TMW_CLIB_API int  TMW_readGrRef(void *N, int size, void *parameter, void *E);

TMW_CLIB_API void  TMW_writeGrRef(void *N, void *D, int size, void *parameter, void *E);

TMW_CLIB_API int  TMW_readSim(void *N, int size, void *parameter, void *E);




/* MmsString (UNICODE) Data Types */
#if defined(MMSD_UNICODE_TYPE)
TMW_CLIB_API int  MMSd_readMmsString( void *N, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeMmsString( void *N, void *D, int size, void *parameter, void *E );
#if defined(MMSD_ATTR_HANDLER_NAME)
TMW_CLIB_API int  MMSd_attrMmsString(void *N, int len, void *parameter, void * E);
#else
TMW_CLIB_API int  MMSd_attrMmsString( int len, void *parameter, void * E );
#endif

#else   /* defined(MMSD_UNICODE_TYPE) */

/*
 * Without Unicode, MmsStrings are basic strings.
 */
#define  MMSd_readMmsString     MMSd_readString
#define  MMSd_writeMmsString    MMSd_writeString
#define  MMSd_attrMmsString     MMSd_attrString
#define  MMSd_indexMmsString    MMSd_indexString

#endif  /* defined(MMSD_UNICODE_TYPE) */

/*
 * Index handlers
 *
 * resolve indexes into arrays of the types above.
 *
 */
TMW_CLIB_API void *MMSd_indexBoolean(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexBitString(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexInteger(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexLongInteger(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexUnsigned(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexLongUnsigned(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexFloat(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexFloat64(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexOctetString(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexString(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexGeneralTime(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexBtime(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexUtcBtime(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexBCD(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexObjectId(int index, void *parameter, int size);
TMW_CLIB_API void *MMSd_indexMmsString(int index, void *parameter, int size);

TMW_CLIB_API void MMSd_writeEnumerated( void *N, void *D, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeCounter( void *N, void *D, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeLongCounter( void *N, void *D, int size, void *parameter, void *E );
TMW_CLIB_API void MMSd_writeCounterUtcBtime( void *N, void *D, int size, void *parameter, void *E );

TMW_CLIB_API void *MMSd_indexEnumerated( int index, void *parameter, int size );
TMW_CLIB_API void *MMSd_indexCounter( int index, void *parameter, int size );
TMW_CLIB_API void *MMSd_indexLongCounter( int index, void *parameter, int size );
TMW_CLIB_API void *MMSd_indexCounterUtcBtime( int index, void *parameter, int size );

#ifdef __cplusplus
};
#endif


#endif /* HNDLDATA_H_ */
