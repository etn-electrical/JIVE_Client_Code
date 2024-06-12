/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPDATBL.C
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Handling CIP Data Table Read/Write requests
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>0----<major>         Customization: <none>0----<major>
**
*****************************************************************************
*****************************************************************************
*/

#include "eipinc.h"

#ifdef EIP_DATATABLE_SUPPORT

/* local functions */
DataTableEntry* dataTableGetEntry( REQUEST* pRequest );
void  dataTableHandleReadRequest( REQUEST* pRequest, DataTableEntry* pDataTableEntry );
void  dataTableHandleWriteRequest( REQUEST* pRequest, DataTableEntry* pDataTableEntry );

DataTableEntry dataTableEntries[DATA_TABLE_ENTRIES];

/*---------------------------------------------------------------------------
** dataTableInit( )
**
** Initialize data table entries. The following code may only be used as an 
** example. These should be substituted with the real values appropriate for
** a particular application.
**---------------------------------------------------------------------------
*/
void dataTableInit(void)
{
    dataTableEntries[0].elementType = typeSINT;
    dataTableEntries[0].nOffset = ASSEMBLY_SIZE - 128;
    dataTableEntries[0].iSize = 32;
    strcpy( dataTableEntries[0].Tag, "DataTableEntrySampleTag1" );
    
    dataTableEntries[1].elementType = typeSINT;
    dataTableEntries[1].nOffset = ASSEMBLY_SIZE - 64;
    dataTableEntries[1].iSize = 64;
    strcpy( dataTableEntries[1].Tag, "DataTableEntrySampleTag2" );

    dataTableEntries[2].elementType = typeDINT;
    dataTableEntries[2].nOffset = ASSEMBLY_SIZE - 256;
    dataTableEntries[2].iSize = 48;
    strcpy( dataTableEntries[2].Tag, "DataTableEntrySampleTag3" );

    dataTableEntries[3].elementType = typeREAL;
    dataTableEntries[3].nOffset = ASSEMBLY_SIZE - 128;
    dataTableEntries[3].iSize = 64;
    strcpy( dataTableEntries[3].Tag, "DataTableEntrySampleTag4" );
}

/*---------------------------------------------------------------------------
** dataTableParseRequest( )
**
** Handle CIP Data Table Read/Write request
**---------------------------------------------------------------------------
*/

void dataTableParseRequest( REQUEST* pRequest )
{
    DataTableEntry* pDataTableEntry = dataTableGetEntry( pRequest );
    if ( pDataTableEntry == NULL )
        pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
    else
    {
        switch( pRequest->cfg.objectCfg.bService )
        {
            case SVC_CIP_DATA_TABLE_READ:
                dataTableHandleReadRequest( pRequest, pDataTableEntry );
                break;
            case SVC_CIP_DATA_TABLE_WRITE:
                dataTableHandleWriteRequest( pRequest, pDataTableEntry );
                break;
            default:        
                pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
                break;
        }    
    }
}

/*---------------------------------------------------------------------------
** dataTableGetEntry( )
**
** Return data table entry index
**---------------------------------------------------------------------------
*/

DataTableEntry* dataTableGetEntry( REQUEST* pRequest )
{
    DataTableEntry* pDataTableEntry;
    char   tag[MAX_REQUEST_TAG_SIZE+1];
    uint16_t iTagSize;
    uint32_t data;

    data = pduGetTagByType(&pRequest->cfg.objectCfg.PDU, TAG_TYPE_EXT_SYMBOL, 0, &iTagSize);
    if ( iTagSize == 0 )
        return NULL;

    memcpy( tag, (uint8_t*)data, iTagSize );
    tag[iTagSize] = 0;

    DumpStr1(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, pRequest->lHostIPAddr, pRequest->lIPAddress, "Received CIP Data Table request for tag '%s'", tag);
    
    for(pDataTableEntry = dataTableEntries; 
		pDataTableEntry < &dataTableEntries[DATA_TABLE_ENTRIES]; 
		pDataTableEntry++ )
    {
        /* Check if the last char is a pad byte */
        if ( !stricmp(pDataTableEntry->Tag, tag) )
            return pDataTableEntry;
    }

    return NULL;
}
               


/*---------------------------------------------------------------------------
** dataTableHandleReadRequest( )
**
** Handle CIP Data Table Read request
**---------------------------------------------------------------------------
*/

void dataTableHandleReadRequest( REQUEST* pRequest, DataTableEntry* pDataTableEntry )
{
    uint32_t  nLen;
    uint16_t  iNbrOfElements;

    if ( pRequest->cfg.objectCfg.iDataSize < sizeof(uint16_t) )
    {
        pRequest->bGeneralError = ROUTER_ERROR_PARTIAL_DATA;
        return;
    }

    iNbrOfElements = UINT16_GET( pRequest->cfg.objectCfg.pData );

    switch( pDataTableEntry->elementType )
    {
        case typeINT:
            nLen = iNbrOfElements * sizeof(uint16_t);            
            break;

        case typeDINT:
            nLen = iNbrOfElements * sizeof(uint32_t);
            break;

        case typeREAL:
            nLen = iNbrOfElements * sizeof(uint32_t);
            break;

        default:
            nLen = iNbrOfElements;
            break;
    }

    if ( nLen > pDataTableEntry->iSize )
        nLen = pDataTableEntry->iSize;
    
    EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, (uint16_t)(nLen+sizeof(uint16_t)) );        

    if ( pRequest->cfg.objectCfg.iDataSize && pRequest->cfg.objectCfg.pData == NULL)
    {
        pRequest->bGeneralError = ROUTER_ERROR_NO_RESOURCE;    /* Out of memory */    
        return;
    }
            
    UINT16_SET( pRequest->cfg.objectCfg.pData, pDataTableEntry->elementType);
    
    assemblyGetCombinedData( (uint8_t*)(pRequest->cfg.objectCfg.pData+sizeof(uint16_t)), (INT32)pDataTableEntry->nOffset, (uint16_t)nLen );
}

/*---------------------------------------------------------------------------
** dataTableHandleWriteRequest( )
**
** Handle CIP Data Table Write request
**---------------------------------------------------------------------------
*/

void dataTableHandleWriteRequest( REQUEST* pRequest, DataTableEntry* pDataTableEntry )
{
    INT32  nLen = ( pDataTableEntry->iSize > (pRequest->cfg.objectCfg.iDataSize-sizeof(uint32_t)) ) ?
        (pRequest->cfg.objectCfg.iDataSize-sizeof(uint32_t)) : pDataTableEntry->iSize;                   

    /* Adjust for the tag type and number of elements */
    assemblySetCombinedData( (uint8_t*)(pRequest->cfg.objectCfg.pData+sizeof(uint32_t)), (INT32)pDataTableEntry->nOffset, (uint16_t)nLen );
}

#endif /* EIP_DATATABLE_SUPPORT */

