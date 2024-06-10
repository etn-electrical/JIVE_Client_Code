/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPFILE.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** File object implementation
**
*****************************************************************************
*/

#include "eipinc.h"

#if defined EIP_FILE_OBJECT

static FileInstance	gFileInstances[MAX_FILE_OBJECT_INSTANCES];
static FileInstance* pnFileInstances;
static char gstrFile[MAX_REQUEST_TAG_SIZE+1];
BOOL gbFileObjectEnabled = FALSE;

/*---------------------------------------------------------------------------
** eipFileInit( )
**
** Initializes file object
**---------------------------------------------------------------------------
*/
void eipFileInit()
{
	pnFileInstances = gFileInstances;
}

BOOL eipFileSetBaseDirectory(char* strDirectory)
{
	platformFileSetBaseDirectory(strDirectory);
	return TRUE;
}

/*---------------------------------------------------------------------------
** eipFileAddInstance( )
**
** Add a file instance
**---------------------------------------------------------------------------
*/
INT32 eipFileAddInstance(INT32 iInstance, EtIPFileInstance* pInstance)
{
	FileInstance* pFileInstance;

	if ((pInstance->State != fileLoaded) &&
		(pInstance->State != fileEmpty) &&
		(pInstance->State != fileNonexistent))
		return ERR_FILE_OBJECT_NOT_INITIALIZED_PROPERLY;

	if (pnFileInstances >= &gFileInstances[MAX_FILE_OBJECT_INSTANCES])
		return ERR_FILE_OBJECT_COUNT_LIMIT_REACHED;

	for(pFileInstance = gFileInstances; pFileInstance < pnFileInstances; pFileInstance++)
	{
		if (pFileInstance->iInstance == iInstance)
			return ERR_FILE_OBJECT_INSTANCE_ALREADY_EXISTS;
	}

	pnFileInstances->iInstance = iInstance;
	pnFileInstances->cfg = *pInstance;
	pnFileInstances->hFile = NULL;
	pnFileInstances->lBytesTransfered = 0;

	pnFileInstances++;

	return 0;
}

BOOL eipFileRemoveInstance(INT32 iInstance)
{
	FileInstance *pFileInstance, *pRemoveFileInstance;

	for(pRemoveFileInstance = gFileInstances; pRemoveFileInstance < pnFileInstances; pRemoveFileInstance++)
	{
		if (pRemoveFileInstance->iInstance == iInstance)
			break;
	}

	if (pRemoveFileInstance >= pnFileInstances)
		return FALSE;

	if ( pnFileInstances > gFileInstances)
		pnFileInstances--;									/* Adjust the total number of files */

	/* Shift the files with the higher index to fill in the void */
	for( pFileInstance = pRemoveFileInstance; pFileInstance < pnFileInstances; pFileInstance++ )
		memcpy( pFileInstance, pFileInstance+1, sizeof(FileInstance) );

	return TRUE;
}

/*---------------------------------------------------------------------------
** eipFileGetInstanceProperties( )
**
** Get file instance properties
**---------------------------------------------------------------------------
*/
BOOL eipFileGetInstanceProperties(INT32 iInstance, EtIPFileInstance* pInstance)
{
	FileInstance* pFileInstance;

	for(pFileInstance = gFileInstances; pFileInstance < pnFileInstances; pFileInstance++)
	{
		if (pFileInstance->iInstance == iInstance)
		{
			if (pInstance != NULL)
				memcpy(pInstance, &pFileInstance->cfg, sizeof(EtIPFileInstance));

			return TRUE;
		}
	}

	return FALSE;
}

/*--------------------------------------------------------------------------------
** eipFileSendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the File class
**--------------------------------------------------------------------------------
*/
static void eipFileSendClassAttrSingle(REQUEST* pRequest)
{
	UINT16 iVal, iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	UINT16 iMaxInstance = 0;
	FileInstance* pFileInstance;
	UINT8* pData;

	switch(iAttribute )
	{
	case FILE_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(FILE_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case FILE_CLASS_ATTR_MAX_INSTANCE:
		/* compute max file instance */
		for(pFileInstance = gFileInstances; pFileInstance < pnFileInstances; pFileInstance++)
		{
			if (pFileInstance->iInstance > iMaxInstance )
				iMaxInstance = pFileInstance->iInstance;
		}

		iVal = ENCAP_TO_HS(iMaxInstance);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case FILE_CLASS_ATTR_NUM_INSTANCE:
		iVal = (UINT16)ENCAP_TO_HS(pnFileInstances-gFileInstances);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case FILE_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(FILE_CLASS_ATTR_INSTANCE_LIST);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case FILE_CLASS_ATTR_MAX_INST_ATTR:
		iVal = ENCAP_TO_HS(FILE_ATTR_FILE_ENCODING_FORMAT);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case FILE_CLASS_ATTR_INSTANCE_LIST:
		iTagSize = 0;
		/* Calculate the response size needed */
		for(pFileInstance = gFileInstances; pFileInstance < pnFileInstances; pFileInstance++)
		{
			iTagSize += (sizeof(UINT16) + 8+pFileInstance->cfg.szInstanceName.bLength + 8+pFileInstance->cfg.szFileName.bLength);
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iTagSize);
		if ( pRequest->cfg.objectCfg.pData == NULL )
		{
			pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
			return;
		}

		pData = pRequest->cfg.objectCfg.pData;
		for(pFileInstance = gFileInstances; pFileInstance < pnFileInstances; pFileInstance++)
		{
			/* set the instance */
			UINT16_SET(pData, pFileInstance->iInstance);
			pData += sizeof(UINT16);

			/* set the instance name */
			clientSetSTRINGI(pData, &pFileInstance->cfg.szInstanceName);
			pData += (8+pFileInstance->cfg.szInstanceName.bLength);

			/* set the file name */
			clientSetSTRINGI(pData, &pFileInstance->cfg.szFileName);
			pData += (8+pFileInstance->cfg.szFileName.bLength);
		}
		break;
	default:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		break;
	}
}

/*--------------------------------------------------------------------------------
** eipFileSendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the File class
**--------------------------------------------------------------------------------
*/
static void eipFileCreate(REQUEST* pRequest)
{
	EtIPFileInstance fileInstance;
	UINT16 iInstance;
	UINT8* pData;

	if (pRequest->cfg.objectCfg.iDataSize < 8)
	{
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if (pRequest->cfg.objectCfg.pData[7]+8 != pRequest->cfg.objectCfg.iDataSize)
	{
		pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	/* Extract the Instance name */
	clientGetSTRINGI(&fileInstance.szInstanceName, pRequest->cfg.objectCfg.pData, pRequest->cfg.objectCfg.iDataSize);

	/* Have the platform file populate the rest of the EtIPFileInstance structure */
	if (!platformFileCreate(&fileInstance, &iInstance, &pRequest->bGeneralError))
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if (eipFileAddInstance(iInstance, &fileInstance))
	{
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	/* File instance successfully added, provide new instance */
	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16) + sizeof(UINT8));
	pData = pRequest->cfg.objectCfg.pData;
	UINT16_SET(pData, iInstance);
	pData += sizeof(UINT16);
	*pData = fileInstance.InvocationType;
}

/*---------------------------------------------------------------------------
** eipFileParseClassRequest( )
**
** Respond to the class request
**---------------------------------------------------------------------------
*/
static void eipFileParseClassRequest(REQUEST* pRequest)
{
	switch(pRequest->cfg.objectCfg.bService)
	{
	case SVC_GET_ATTR_SINGLE:
		eipFileSendClassAttrSingle(pRequest);
		break;
	case SVC_CREATE:
		eipFileCreate(pRequest);
		break;
	default:
		/* Do not support any other services */
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*--------------------------------------------------------------------------------
** eipFileSendInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the File object instance
**--------------------------------------------------------------------------------
*/
static void eipFileSendInstanceAttrSingle(UINT32 iInstance, REQUEST* pRequest, EtIPFileInstance* pInstance)
{
	UINT16 iVal, iTagSize;
	UINT32 lVal;
	UINT8* pData;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	switch(iAttribute)
	{
	case FILE_ATTR_STATE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&pInstance->State, sizeof(UINT8) );
		break;
	case FILE_ATTR_INSTANCE_NAME:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 8+pInstance->szInstanceName.bLength);
		pData = pRequest->cfg.objectCfg.pData;
		clientSetSTRINGI(pData, &pInstance->szInstanceName);
		break;
	case FILE_ATTR_INSTANCE_FORMAT_VERSION:
		iVal = ENCAP_TO_HS(pInstance->iFormatRevision);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case FILE_ATTR_FILE_NAME:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 8+pInstance->szFileName.bLength);
		pData = pRequest->cfg.objectCfg.pData;
		clientSetSTRINGI(pData, &pInstance->szFileName);
		break;
	case FILE_ATTR_FILE_REVISION:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (UINT16)(2*sizeof(UINT8)));
		pData = pRequest->cfg.objectCfg.pData;

		*pData = pInstance->bFileMajorRevision; pData++;
		*pData = pInstance->bFileMinorRevision;
		break;
	case FILE_ATTR_FILE_SIZE:
		lVal = ENCAP_TO_HL(pInstance->dwFileSize);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&lVal, sizeof(UINT32) );
		break;
	case FILE_ATTR_FILE_CHECKSUM:
		iVal = ENCAP_TO_HS(pInstance->iChecksum);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case FILE_ATTR_FILE_INVOCATION_METHOD:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&pInstance->InvocationType, sizeof(UINT8) );
		break;
	case FILE_ATTR_FILE_SAVE_PARAMETERS:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&pInstance->bSaveToNonVolatile, sizeof(UINT8) );
		break;
	case FILE_ATTR_FILE_TYPE:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&pInstance->AccessType, sizeof(UINT8) );
		break;
	case FILE_ATTR_FILE_ENCODING_FORMAT:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&pInstance->bEncoded, sizeof(UINT8) );
		break;
	default:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
	}
}

/*--------------------------------------------------------------------------------
** eipFileSetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the File object instance
**--------------------------------------------------------------------------------
*/
static void eipFileSetInstanceAttrSingle(UINT32 iInstance, REQUEST* pRequest, EtIPFileInstance* pInstance)
{
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	switch(iAttribute)
	{
	case FILE_ATTR_STATE:
	case FILE_ATTR_INSTANCE_NAME:
	case FILE_ATTR_INSTANCE_FORMAT_VERSION:
	case FILE_ATTR_FILE_NAME:
	case FILE_ATTR_FILE_REVISION:
	case FILE_ATTR_FILE_SIZE:
	case FILE_ATTR_FILE_CHECKSUM:
	case FILE_ATTR_FILE_INVOCATION_METHOD:
	case FILE_ATTR_FILE_SAVE_PARAMETERS:
	case FILE_ATTR_FILE_ENCODING_FORMAT:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		break;
	case FILE_ATTR_FILE_TYPE:
		/* special handling for EDS file instances */
		if ((iInstance == FILE_EDS_FILE_INSTANCE) ||
			(iInstance == FILE_EDS_RELATED_FILE_INSTANCE))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		}
		else
		{
			if (pRequest->cfg.objectCfg.iDataSize < 1)
			{
				pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
				break;
			}
			if (pRequest->cfg.objectCfg.iDataSize > 1)
			{
				pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
				break;
			}

			pInstance->AccessType = (EtIPFileAccessType)pRequest->cfg.objectCfg.pData[0];
		}
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*--------------------------------------------------------------------------------
** eipFileInitiateUpload( )
**
** Initiate Upload service initialize the uploading of a file for the File
** object instance specified
**--------------------------------------------------------------------------------
*/
static void eipFileInitiateUpload(REQUEST* pRequest, FileInstance* pInstance)
{
	UINT8* pData;

	/* check if it's already in the process of being uploaded */
	switch(pInstance->cfg.State)
	{
	case fileLoaded:
	case fileUploadInitiated:
	case fileUploadInProgress:
		break;
	case fileNonexistent:
	case fileEmpty:
	case fileDownloadInProgress:
	case fileDownloadInitiated:
	case fileStoring:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		return;
	default:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_DEV_IN_WRONG_STATE;
		return;
	}

	if (pRequest->cfg.objectCfg.iDataSize != 1)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
		return;
	}

	/* format the ISTRING into a null terminated string */
	memcpy(gstrFile, pInstance->cfg.szFileName.stringData, pInstance->cfg.szFileName.bLength);
	gstrFile[pInstance->cfg.szFileName.bLength] = 0;

	pInstance->hFile = platformFileOpenFile(gstrFile, FALSE, pInstance->iInstance);
	if (pInstance->hFile == NULL)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		pRequest->iExtendedError = FILE_EXT_ERROR_FILE_DOES_NOT_EXIST;
		return;
	}

	pInstance->bMaxTransferSize = pRequest->cfg.objectCfg.pData[0];
	pInstance->bTempTransferNumber = (unsigned char)-1;
	pInstance->pCachePacket = NULL;

	if ( pInstance->bMaxTransferSize == 0 )
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		pRequest->iExtendedError = FILE_EXT_ERROR_ZERO_SIZE;
		return;
	}

	if (pInstance->cfg.State != fileUploadInProgress)
		pInstance->lBytesTransfered = 0;

	/* set the state */
	pInstance->cfg.State = fileUploadInitiated;

	/* set the data */
	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 5 );
	pData = pRequest->cfg.objectCfg.pData;

	UINT32_SET(pData, pInstance->cfg.dwFileSize);
	pData += sizeof(UINT32);

	*pData = pInstance->bMaxTransferSize;

	pRequest->bGeneralError = ROUTER_ERROR_SUCCESS;
}

/*--------------------------------------------------------------------------------
** eipFileUpload( )
**
** Upload service sends a chunk of the uploading file for the File
** object instance specified
**--------------------------------------------------------------------------------
*/
static void eipFileUpload(REQUEST* pRequest, FileInstance* pInstance)
{
	INT32 iBytesRead;
	UINT8 bTransferNumber;
	UINT16 iLastPacketSize = 0;

	/* check if it's already in the process of being uploaded */
	if ((pInstance->cfg.State != fileUploadInitiated) &&
		(pInstance->cfg.State != fileUploadInProgress))
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		return;
	}

	if (pRequest->cfg.objectCfg.iDataSize != 1)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
		return;
	}

	/* Verify transfer number */
	if ((pRequest->cfg.objectCfg.pData[0] != pInstance->bTempTransferNumber) &&
		(pRequest->cfg.objectCfg.pData[0] != (UINT8)(pInstance->bTempTransferNumber+1)))
	{
		/* reset state */
		pInstance->cfg.State = fileLoaded;
		platformFileCloseFile(pInstance->hFile);
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		pRequest->iExtendedError = FILE_EXT_ERROR_OUT_OF_SEQUENCE;
		return;
	}

	pRequest->bGeneralError = ROUTER_ERROR_SUCCESS;
	if ((pRequest->cfg.objectCfg.pData[1] == fileLastTransferPacket) ||
		(pRequest->cfg.objectCfg.pData[1] == fileFirstLastTransferPacket))
	{
		iLastPacketSize += 2;
	}

	bTransferNumber = pRequest->cfg.objectCfg.pData[0];
	if (bTransferNumber == pInstance->bTempTransferNumber)
	{
		/* client must have missed a packet becuase he is requesting
		   it again.  Use cached copy */
		iBytesRead = pInstance->bCachePacketSize;

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, iBytesRead+2+iLastPacketSize );
		memcpy(&pRequest->cfg.objectCfg.pData[2], pInstance->pCachePacket, iBytesRead);
	}
	else
	{
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, pInstance->bMaxTransferSize+2+iLastPacketSize );
		iBytesRead = platformFileGetFileData(pInstance->hFile, &pRequest->cfg.objectCfg.pData[2], pInstance->bMaxTransferSize);
		if (iBytesRead < 0)
		{
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 2 );

			/* Abort the transfer */
			pRequest->cfg.objectCfg.pData[0] = bTransferNumber;
			pRequest->cfg.objectCfg.pData[1] = fileAbortTransfer;
			pRequest->cfg.objectCfg.iDataSize = 2;
			pInstance->cfg.State = fileLoaded;
			platformFileCloseFile(pInstance->hFile);
			return;
		}

		/* Cache the packet in case it's requested again */
		EtIP_realloc(&pInstance->pCachePacket, &pInstance->bCachePacketSize, &pRequest->cfg.objectCfg.pData[2], iBytesRead);
	}

	pInstance->cfg.State = fileUploadInProgress;

	/* determine the transfer packet type */
	if (pInstance->lBytesTransfered == 0)
	{
		if (iBytesRead == (int)pInstance->cfg.dwFileSize)
		{
			pRequest->cfg.objectCfg.pData[1] = fileFirstLastTransferPacket;
		}
		else
		{
			pRequest->cfg.objectCfg.pData[1] = fileFirstTransferPacket;
		}
	}
	else
	{
		if (bTransferNumber != pInstance->bTempTransferNumber)
		{
			if (pInstance->lBytesTransfered+iBytesRead == pInstance->cfg.dwFileSize)
			{
				pRequest->cfg.objectCfg.pData[1] = fileLastTransferPacket;
			}
			else
			{
				pRequest->cfg.objectCfg.pData[1] = fileMiddleTransferPacket;
			}
		}
		else
		{
			pRequest->cfg.objectCfg.pData[1] = fileMiddleTransferPacket;
		}
	}

	pRequest->cfg.objectCfg.iDataSize = iBytesRead + 2;
	if (bTransferNumber != pInstance->bTempTransferNumber)
		pInstance->lBytesTransfered += iBytesRead;

	/* echo the transfer number */
	pInstance->bTempTransferNumber = pRequest->cfg.objectCfg.pData[0] = bTransferNumber;

	if ((pRequest->cfg.objectCfg.pData[1] == fileLastTransferPacket) ||
		(pRequest->cfg.objectCfg.pData[1] == fileFirstLastTransferPacket))
	{
		free(pInstance->pCachePacket);
		UINT16_SET(&pRequest->cfg.objectCfg.pData[2+iBytesRead], pInstance->cfg.iChecksum);
		pRequest->cfg.objectCfg.iDataSize += sizeof(UINT16);
		pInstance->cfg.State = fileLoaded;
		platformFileCloseFile(pInstance->hFile);
	}
	else
	{
		pInstance->cfg.State = fileUploadInProgress;
	}
}

/*--------------------------------------------------------------------------------
** eipFileInitiateDownload( )
**
** Initiate download service initialize the downloading of a file for the File
** object instance specified
**--------------------------------------------------------------------------------
*/
static void eipFileInitiateDownload(REQUEST* pRequest, FileInstance* pInstance)
{
	UINT8* pRequestData = pRequest->cfg.objectCfg.pData;
	UINT8* pResponseData;
	UINT16 iFormatVersion;
	AppFileBurnStats Stats;

	/* make sure the instance isn't read only */
	if (pInstance->cfg.AccessType != fileReadWrite)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		return;
	}

	/* check if it's already in the process of being downloaded */
	switch(pInstance->cfg.State)
	{
	case fileLoaded:
	case fileEmpty:
	case fileDownloadInProgress:
	case fileDownloadInitiated:
		break;
	case fileNonexistent:
	case fileUploadInitiated:
	case fileUploadInProgress:
	case fileStoring:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		return;
	default:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_DEV_IN_WRONG_STATE;
		return;
	}

	if (pRequest->cfg.objectCfg.iDataSize < 16)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
		return;
	}

	/* Get the request parameters */
	pInstance->lTempFilesize = UINT32_GET(pRequestData);
	pRequestData += sizeof(UINT32);
	iFormatVersion = UINT32_GET(pRequestData);
	pRequestData += sizeof(UINT16);

	if ( pInstance->lTempFilesize > FILE_MAX_SIZE )
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		pRequest->iExtendedError = FILE_EXT_ERROR_FILE_SIZE_TOO_LARGE;
		return;
	}

	/* Format version must match */
	if (iFormatVersion != pInstance->cfg.iFormatRevision)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		pRequest->iExtendedError = FILE_EXT_ERROR_INCOMPATIBLE_FORMAT_VERSION;
		return;
	}

	pInstance->bTempMajRev = *pRequestData++;
	pInstance->bTempMinRev = *pRequestData++;
	clientGetSTRINGI(&pInstance->szTempFilename, pRequestData, pRequest->cfg.objectCfg.iDataSize);
	pRequestData += (8+pInstance->szTempFilename.bLength);

	/* format the ISTRING into a null terminated string */
	memcpy(gstrFile, pInstance->szTempFilename.stringData, pInstance->szTempFilename.bLength);
	gstrFile[pInstance->szTempFilename.bLength] = 0;

	pInstance->hFile = platformFileOpenFile(gstrFile, TRUE, pInstance->iInstance);
	if (pInstance->hFile == NULL)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;
		pRequest->iExtendedError = FILE_EXT_ERROR_FILE_DOES_NOT_EXIST;
		return;
	}

	if (!platformFileGetBurnStats(&Stats))
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_SERVICE_ERROR;
		return;
	}

	pInstance->lBytesTransfered = 0;
	pInstance->iTempChecksum = 0;
	pInstance->bTempTransferNumber = (unsigned char)-1;

	/* set the state */
	pInstance->cfg.State = fileDownloadInitiated;

	/* set the data */
	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, 7 );
	pResponseData = pRequest->cfg.objectCfg.pData;

	UINT32_SET(pResponseData, Stats.lIncrementalBurn);
	pResponseData += sizeof(UINT32);
	UINT16_SET(pResponseData, Stats.iIncrementalBurnTime);
	pResponseData += sizeof(UINT16);
	*pResponseData = Stats.bTransferSize;

	pRequest->bGeneralError = ROUTER_ERROR_SUCCESS;
}

/*--------------------------------------------------------------------------------
** eipFileDownload( )
**
** Download service sends a chunk of the downloading file for the File
** object instance specified
**--------------------------------------------------------------------------------
*/
static void eipFileDownload(REQUEST* pRequest, FileInstance* pInstance)
{
	UINT8 bBytesToWrite = 0;
	UINT16 iChecksum;
	INT32 i, iBytesRead;
	UINT8* pData;

	/* make sure the instance isn't read only */
	if (pInstance->cfg.AccessType != fileReadWrite)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		return;
	}

	/* check if it's already in the process of being downloaded */
	if ((pInstance->cfg.State != fileDownloadInitiated) &&
		(pInstance->cfg.State != fileDownloadInProgress))
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		return;
	}

	if (pRequest->cfg.objectCfg.iDataSize < 2)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
		return;
	}

	/* Abort? */
	if (pRequest->cfg.objectCfg.pData[1] == fileAbortTransfer)
	{
		pInstance->cfg.State = fileEmpty;
		platformFileCloseFile(pInstance->hFile);
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_SUCCESS;
		return;
	}

	/* Verify transfer number */
	if (pRequest->cfg.objectCfg.pData[0] != (UINT8)(pInstance->bTempTransferNumber+1))
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		pRequest->iExtendedError = FILE_EXT_ERROR_OUT_OF_SEQUENCE;
		return;
	}

	/* Determine if packet type is right */
	if (pInstance->lBytesTransfered == 0)
	{
		if ((pRequest->cfg.objectCfg.pData[1] != fileFirstTransferPacket) &&
			(pRequest->cfg.objectCfg.pData[1] != fileFirstLastTransferPacket))
		{
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
			pRequest->iExtendedError = ROUTER_EXT_ERR_NO_EXTENDED;
			pInstance->cfg.State = fileEmpty;
			return;
		}
	}
	else if (pInstance->lBytesTransfered+pRequest->cfg.objectCfg.iDataSize-4 == pInstance->lTempFilesize)
	{
		if (pRequest->cfg.objectCfg.pData[1] != fileLastTransferPacket)
		{
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
			pRequest->iExtendedError = FILE_EXT_ERROR_OUT_OF_SEQUENCE;
			return;
		}
	}
	else if (pRequest->cfg.objectCfg.pData[1] != fileMiddleTransferPacket)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_PARAMETER;
		pRequest->iExtendedError = FILE_EXT_ERROR_OUT_OF_SEQUENCE;
		return;
	}

	/* Determine the number of data bytes */
	switch(pRequest->cfg.objectCfg.pData[1])
	{
	case fileFirstTransferPacket:
	case fileMiddleTransferPacket:
		bBytesToWrite = pRequest->cfg.objectCfg.iDataSize-2;
		break;
	case fileFirstLastTransferPacket:
	case fileLastTransferPacket:
		bBytesToWrite = pRequest->cfg.objectCfg.iDataSize-4;
		break;
	}

	/* mimic the transfer sequence */
	pInstance->bTempTransferNumber = *pRequest->cfg.objectCfg.pData;

	/* write to the file */
	iBytesRead = platformFileSetFileData(pInstance->hFile, &pRequest->cfg.objectCfg.pData[2], bBytesToWrite);
	if ((iBytesRead < 0) || (iBytesRead != bBytesToWrite))
	{
		/* Fail to write */
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_DATA_WRITE_FAILURE;
		pRequest->iExtendedError = FILE_EXT_ERROR_FILE_DOES_NOT_EXIST;
		pInstance->cfg.State = fileEmpty;
		platformFileCloseFile(pInstance->hFile);
		return;
	}

	pInstance->lBytesTransfered += bBytesToWrite;
	pInstance->cfg.State = fileDownloadInProgress;

	/* Compute checksum */
	pData = &pRequest->cfg.objectCfg.pData[2];
	for (i = 0; i < bBytesToWrite; i++)
		pInstance->iTempChecksum += *pData++;

	/* Is this the last packet? */
	if ((pRequest->cfg.objectCfg.pData[1] == fileFirstLastTransferPacket) ||
		(pRequest->cfg.objectCfg.pData[1] == fileLastTransferPacket))
	{
		if (pInstance->lBytesTransfered != pInstance->lTempFilesize)
		{
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			pRequest->bGeneralError = ROUTER_ERROR_DATA_WRITE_FAILURE;
			pRequest->iExtendedError = FILE_EXT_ERROR_FILE_DOES_NOT_EXIST;
			pInstance->cfg.State = fileEmpty;
			platformFileCloseFile(pInstance->hFile);
			return;
		}

		/* Verify checksum */
		iChecksum = UINT16_GET(&pRequest->cfg.objectCfg.pData[pRequest->cfg.objectCfg.iDataSize-2]);
		if ((UINT16)((~pInstance->iTempChecksum) + 1) != iChecksum)
		{
			EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pInstance->bTempTransferNumber, sizeof(UINT8) );
			pRequest->bGeneralError = FILE_ERROR_CHECKSUM;
			pRequest->iExtendedError = FILE_EXT_ERROR_FILE_DOES_NOT_EXIST;
			pInstance->cfg.State = fileEmpty;
			platformFileCloseFile(pInstance->hFile);
			return;
		}

		/* Successfully downloaded a file */
		pInstance->cfg.bFileMajorRevision = pInstance->bTempMajRev;
		pInstance->cfg.bFileMinorRevision = pInstance->bTempMinRev;
		pInstance->cfg.dwFileSize = pInstance->lTempFilesize;
		pInstance->cfg.iChecksum = iChecksum;
		memcpy(&pInstance->cfg.szFileName, &pInstance->szTempFilename, 7+pInstance->szTempFilename.bLength);

		platformFileCloseFile(pInstance->hFile);

		pInstance->cfg.State = fileLoaded;
	}

	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, &pInstance->bTempTransferNumber, sizeof(UINT8) );
	pRequest->bGeneralError = ROUTER_ERROR_SUCCESS;
}

/*--------------------------------------------------------------------------------
** eipFileClearFile( )
**
** Clear service removes the file for the File object instance specified
**--------------------------------------------------------------------------------
*/
static void eipFileClearFile(REQUEST* pRequest, FileInstance* pInstance)
{
	switch(pInstance->cfg.State)
	{
	case fileNonexistent:
		pRequest->bGeneralError = ROUTER_ERROR_OBJ_DOES_NOT_EXIST;
		break;
	case fileEmpty:
		pRequest->bGeneralError = ROUTER_ERROR_SUCCESS;
		break;
	case fileLoaded:
	case fileDownloadInProgress:
	case fileDownloadInitiated:
	case fileUploadInitiated:
	case fileUploadInProgress:
	case fileStoring:
		/* make sure the instance isn't read only */
		if (pInstance->cfg.AccessType != fileReadWrite)
		{
			pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
			break;
		}

		/* format the ISTRING into a null terminated string */
		memcpy(gstrFile, pInstance->cfg.szFileName.stringData, pInstance->cfg.szFileName.bLength);
		gstrFile[pInstance->cfg.szFileName.bLength] = 0;

		platformFileClearFile(gstrFile, pInstance->iInstance);

		pInstance->cfg.State = fileEmpty;
		pInstance->cfg.szFileName.bLength = 0;
		pInstance->cfg.bFileMajorRevision = 0;
		pInstance->cfg.bFileMinorRevision = 0;
		pInstance->cfg.dwFileSize = 0;
		pInstance->cfg.iChecksum = 0;
		pInstance->cfg.bSaveToNonVolatile = 0;
		pRequest->bGeneralError = ROUTER_ERROR_SUCCESS;
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_DEV_IN_WRONG_STATE;
		break;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*--------------------------------------------------------------------------------
** eipFileClearFile( )
**
** Clear service removes the file for the File object instance specified
**--------------------------------------------------------------------------------
*/
static void eipFileDelete(REQUEST* pRequest, FileInstance* pInstance)
{
	/* format the ISTRING into a null terminated string */
	memcpy(gstrFile, pInstance->cfg.szFileName.stringData, pInstance->cfg.szFileName.bLength);
	gstrFile[pInstance->cfg.szFileName.bLength] = 0;

	platformFileClearFile(gstrFile, pInstance->iInstance);

	if (!eipFileRemoveInstance(pInstance->iInstance))
	{
		/* This shouldn't happen */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*---------------------------------------------------------------------------
** eipFileParseInstanceRequest( )
**
** Respond to the instance request
**---------------------------------------------------------------------------
*/
static void eipFileParseInstanceRequest(REQUEST* pRequest, UINT32 iInstance)
{
	FileInstance* pFileInstance;

	for(pFileInstance = gFileInstances; pFileInstance < pnFileInstances; pFileInstance++)
	{
		if (pFileInstance->iInstance == iInstance)
			break;
	}

	if (pFileInstance >= pnFileInstances)
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return;
	}

	switch(pRequest->cfg.objectCfg.bService)
	{
	case SVC_GET_ATTR_SINGLE:
		eipFileSendInstanceAttrSingle(iInstance, pRequest, &pFileInstance->cfg);
		break;
	case SVC_SET_ATTR_SINGLE:
		eipFileSetInstanceAttrSingle(iInstance, pRequest, &pFileInstance->cfg);
		break;
	case FILE_INITIATE_UPLOAD:
		eipFileInitiateUpload(pRequest, pFileInstance);
		break;
	case FILE_INITIATE_DOWNLOAD:
		eipFileInitiateDownload(pRequest, pFileInstance);
		break;
	case FILE_UPLOAD_TRANSFER:
		eipFileUpload(pRequest, pFileInstance);
		break;
	case FILE_DOWNLOAD_TRANSFER:
		eipFileDownload(pRequest, pFileInstance);
		break;
	case FILE_CLEAR_FILE:
		eipFileClearFile(pRequest, pFileInstance);
		break;
	case SVC_DELETE:
		eipFileDelete(pRequest, pFileInstance);
		break;
	default:    /* Do not support any other services */
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}


/*---------------------------------------------------------------------------
** eipFileParseClassInstanceRqst( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/
void eipFileParseClassInstanceRqst(REQUEST* pRequest)
{
	UINT16 iTagSize;
	UINT32 iInstance;

	iInstance = (UINT32)pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize);
	if ( iTagSize == 0 )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if ( iInstance == 0 )
		eipFileParseClassRequest(pRequest);
	else
		eipFileParseInstanceRequest(pRequest, iInstance);
}

#endif /* #if defined EIP_FILE_OBJECT */
