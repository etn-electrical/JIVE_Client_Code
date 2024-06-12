/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPFILE.h
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** File object
**
*****************************************************************************
*/

#ifndef EIPFILE_H
#define EIPFILE_H

#if defined EIP_FILE_OBJECT

#define FILE_CLASS				0x37		/* File class identifier */
#define FILE_CLASS_REVISION		1			/* File class revision */

/* Class and instance attribute numbers */
#define FILE_CLASS_ATTR_REVISION			1
#define FILE_CLASS_ATTR_MAX_INSTANCE		2
#define FILE_CLASS_ATTR_NUM_INSTANCE		3
#define FILE_CLASS_ATTR_MAX_CLASS_ATTR		6
#define FILE_CLASS_ATTR_MAX_INST_ATTR		7
#define FILE_CLASS_ATTR_INSTANCE_LIST		32


#define FILE_ATTR_STATE						1
#define FILE_ATTR_INSTANCE_NAME				2
#define FILE_ATTR_INSTANCE_FORMAT_VERSION	3
#define FILE_ATTR_FILE_NAME					4
#define FILE_ATTR_FILE_REVISION				5
#define FILE_ATTR_FILE_SIZE					6
#define FILE_ATTR_FILE_CHECKSUM				7
#define FILE_ATTR_FILE_INVOCATION_METHOD	8
#define FILE_ATTR_FILE_SAVE_PARAMETERS		9
#define FILE_ATTR_FILE_TYPE					10
#define FILE_ATTR_FILE_ENCODING_FORMAT		11

/* Additional service codes */
#define FILE_INITIATE_UPLOAD				0x4B
#define FILE_INITIATE_DOWNLOAD				0x4C
#define FILE_UPLOAD_TRANSFER				0x4F
#define FILE_DOWNLOAD_TRANSFER				0x50
#define FILE_CLEAR_FILE						0x51

/* Define max file size allowed to download here */
#define FILE_MAX_SIZE						0x4000

/* Additional error codes */
#define FILE_ERROR_CHECKSUM								0xD0

#define FILE_EXT_ERROR_FILE_SIZE_TOO_LARGE				4
#define FILE_EXT_ERROR_INCOMPATIBLE_FORMAT_VERSION		5
#define FILE_EXT_ERROR_OUT_OF_SEQUENCE					6
#define FILE_EXT_ERROR_OTHER							7
#define FILE_EXT_ERROR_ZERO_SIZE						8
#define FILE_EXT_ERROR_FILE_DOES_NOT_EXIST				0xFF

#define FILE_EDS_FILE_INSTANCE							0xC8
#define FILE_EDS_RELATED_FILE_INSTANCE					0xC9

typedef struct tagFILE_INSTANCE_LIST
{
	UINT16 iInstance;
	STRINGI	strInstanceName;
	STRINGI	strFileName;
} FILE_INSTANCE_LIST;

typedef enum tagFileTransferPacketType
{
	fileFirstTransferPacket		= 0,
	fileMiddleTransferPacket	= 1,
	fileLastTransferPacket		= 2,
	fileAbortTransfer			= 3,
	fileFirstLastTransferPacket	= 4,
}
FileTransferPacketType;


typedef struct tagAppFileBurnStats
{
	UINT32 lIncrementalBurn;
	UINT16 iIncrementalBurnTime;
	UINT8  bTransferSize;

} AppFileBurnStats;


typedef struct tagFileInstance
{
	UINT16 iInstance;
	EtIPFileInstance cfg;
	PLATFORM_FILE_HANDLE hFile;
	UINT8 bMaxTransferSize;
	UINT32 lBytesTransfered;
	UINT8 bTempMajRev;					/* Used to store Major Rev of new file during download */
	UINT8 bTempMinRev;					/* Used to store Minor Rev of new file during download */
	STRINGI szTempFilename;				/* Used to store name of new file during download */
	UINT32 lTempFilesize;
	UINT16	iTempChecksum;
	UINT8 bTempTransferNumber;
	UINT8* pCachePacket;				/* Stores last packet in case its requested again */
	UINT16 bCachePacketSize;
} FileInstance;


extern void eipFileInit();
extern void eipFileParseClassInstanceRqst(REQUEST* pRequest);
extern INT32 eipFileAddInstance(INT32 iInstance, EtIPFileInstance* pInstance);
extern BOOL eipFileRemoveInstance(INT32 iInstance);
extern BOOL eipFileGetInstanceProperties(INT32 iInstance, EtIPFileInstance* pInstance);
extern BOOL eipFileSetBaseDirectory(char* strDirectory);

extern BOOL platformFileCreate(EtIPFileInstance* pFileInfo, UINT16* piNewInstance, UINT8* pbGeneralError);

extern BOOL gbFileObjectEnabled;

#endif /* #if defined EIP_FILE_OBJECT */

#endif
