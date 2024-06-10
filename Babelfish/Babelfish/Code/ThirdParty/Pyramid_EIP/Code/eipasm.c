/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPASM.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** I/O data storage and handling
**
*****************************************************************************
*/

#include "eipinc.h"

#ifdef ET_IP_SCANNER
/* Preallocated space to store all scanner input I/O data */
static UINT8  gInputScannerAssembly[NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES][INPUT_SCANNER_ASSEMBLY_SIZE];
/* Preallocated space to store all scanner output I/O data */
static UINT8  gOutputScannerAssembly[NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES][OUTPUT_SCANNER_ASSEMBLY_SIZE];
#endif

#ifdef EIP_LARGE_BUF_ON_HEAP
static UINT8* gAssemblyBuffer;		/* Preallocated space to store all I/O data */
ASSEMBLY* gAssemblies;				/* Assembly objects array */
#else
static UINT8 gAssemblyBuffer[ASSEMBLY_SIZE];	/* Preallocated space to store all I/O data */
ASSEMBLY gAssemblies[MAX_ASSEMBLIES];			/* Assembly objects array */
#endif

ASSEMBLY* gpnAssemblies;

static UINT16 assemblyOptionalAttributes[ASSEMBLY_ATTR_OPTIONAL_COUNT] = {ASSEMBLY_INSTANCE_SIZE};


/*---------------------------------------------------------------------------
** assemblyInit( )
**
** Initialize assembly array
**---------------------------------------------------------------------------
*/
void assemblyInit(void)
{
#ifdef ET_IP_SCANNER
	UINT8 i;
#endif

#ifdef EIP_LARGE_BUF_ON_HEAP
	gAssemblyBuffer = EtIP_mallocHeap((UINT16)(ASSEMBLY_SIZE * sizeof(UINT8 )));
	if(gAssemblyBuffer == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gAssemblyBuffer to %d bytes\n", ASSEMBLY_SIZE);
		return;
	}

	gAssemblies = (ASSEMBLY*)EtIP_mallocHeap((UINT16)(MAX_ASSEMBLIES * sizeof(ASSEMBLY)));
	if(gAssemblies == NULL)
	{
		DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_MSG, 0, 0, "Error allocating gAssemblies to %d entries\n", MAX_ASSEMBLIES);
		return;
	}
#endif

#ifdef ET_IP_SCANNER
	for (i = 0; i < NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES; i++)
		memset(&gInputScannerAssembly[i], 0, INPUT_SCANNER_ASSEMBLY_SIZE);

	for (i = 0; i < NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES; i++)
		memset(&gOutputScannerAssembly[i], 0, OUTPUT_SCANNER_ASSEMBLY_SIZE);
#endif

	memset( gAssemblyBuffer, 0, ASSEMBLY_SIZE );
	gpnAssemblies = gAssemblies;
}

/*---------------------------------------------------------------------------
** assemblyInitialize( )
**
** Initialize an assembly object
**---------------------------------------------------------------------------
*/

static void assemblyInitialize( ASSEMBLY* pAssembly )
{
	memset( pAssembly, 0, sizeof(ASSEMBLY) );
	pAssembly->pnMembers = pAssembly->Members;
}

/*---------------------------------------------------------------------------
** assemblyNew( )
**
** Allocate and initialize new assembly in the array 
**---------------------------------------------------------------------------
*/
INT32 assemblyNew( UINT32 iInstance, INT32  nOffset, UINT16 iSize, UINT16 iType, INT8* szAssemblyName )
{
	ASSEMBLY* pAssembly;
	INT32 nTagLen;
	INT32 i, nUpperBound;

	if ( iSize > MAX_ASSEMBLY_SIZE )
		return ERR_ASSEMBLY_SIZE_EXCEED_LIMIT;

	/* Check if assembly array limit has been reached */
	if ( gpnAssemblies >= &gAssemblies[MAX_ASSEMBLIES] )
		return ERR_ASSEMBLY_OBJECTS_LIMIT_REACHED;

	if ((iInstance == gHeartbeatConnPoint) ||
		(iInstance == gListenOnlyConnPoint)
#ifdef CIP_SAFETY
		|| (iInstance == TIME_COORDINATION_CONN_POINT)
#endif
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		|| assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance)
#endif
		)
	{
		return ERR_ASSEMBLY_INVALID_INSTANCE;
	}

#ifndef EIP_NO_DYNAMIC_ASSEMBLY
	if ((iType & AssemblyDynamic) &&
		((iInstance < ASSEMBLY_START_DYNAMIC_INSTANCE_RANGE1) || (iInstance > ASSEMBLY_END_DYNAMIC_INSTANCE_RANGE2) ||
		((iInstance > ASSEMBLY_END_DYNAMIC_INSTANCE_RANGE1) && (iInstance < ASSEMBLY_START_DYNAMIC_INSTANCE_RANGE2))))  /* Dynamic assemblies must have instances in the vendor specific range */
	{
		return ERR_ASSEMBLY_INVALID_INSTANCE;
	}
#endif

	for( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )        /* Find if assembly already exist */
	{
		if ( pAssembly->iInstance == iInstance )
			return ERR_ASSEMBLY_INSTANCE_ALREADY_EXIST;
	}

	if ( szAssemblyName != NULL )
	{
		nTagLen = (INT32)strlen( (const char*)szAssemblyName );	/* Ignore the empty tag */
		if ( nTagLen > 0 )
		{
			pAssembly = assemblyGetByTagName( (UINT8*)szAssemblyName, (UINT16)nTagLen );
			if ( pAssembly != NULL )
				return ERR_ASSEMBLY_INSTANCE_ALREADY_EXIST;	/* Duplicate tag names are not allowed just as duplicate instances are not allowed */
		}
	}

	nUpperBound = ASSEMBLY_SIZE;
	if ( nUpperBound % 2 )
		nUpperBound--;

	/* Assign offset automatically */
	if ( nOffset == (INT32)INVALID_OFFSET )
	{
		/* Assign at the end of the buffer since the beginning might be assigned manually */
		for ( i = (nUpperBound-iSize); i >= 0; i -= 2 )
		{
			/* Find if assembly already exist */
			for( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
			{
				if ( BUFFER_INTERSECT( pAssembly->nOffset, pAssembly->iSize, i, iSize ) )
					break;
			}

			/* If no assembly occupies it, we found an open slot */
			if ( pAssembly == gpnAssemblies )
			{
				nOffset = i;
				break;
			}
		}

		if ( nOffset == (INT32)INVALID_OFFSET )
			return ERR_ASSEMBLY_NO_BUFFER_SPACE_LEFT;
	}
	/* Offset specified by the client */
	else
	{
		if ( nOffset < 0 || (nOffset+iSize) > ASSEMBLY_SIZE )
			return ERR_ASSEMBLY_SPECIFIED_OFFSET_INVALID;

		/* Check that it does not overlap with any of the already configured assemblies */
		for( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
		{
			if ( BUFFER_INTERSECT( pAssembly->nOffset, pAssembly->iSize, nOffset, iSize ) )
				break;
		}

		/* There is already assembly that occipies it */
		if ( pAssembly != gpnAssemblies )
			return ERR_ASSEMBLY_SPECIFIED_OFFSET_INVALID;
	}

	pAssembly = gpnAssemblies;
	gpnAssemblies++;

	assemblyInitialize( pAssembly );

	pAssembly->iInstance = iInstance;
	pAssembly->nOffset = nOffset;
	pAssembly->iSize = iSize;
	pAssembly->iType = iType;

	if ( szAssemblyName != NULL )
	{
		pAssembly->iTagSize = (UINT16)strlen( (const char*)szAssemblyName );
		pAssembly->pTagName = EtIP_malloc((UINT8*)szAssemblyName, pAssembly->iTagSize );
	}

	return 0;
}

/*---------------------------------------------------------------------------
** assemblyRemove( )
**
** Remove the asembly object from the array
**---------------------------------------------------------------------------
*/
INT32 assemblyRemove( UINT32 iInstance )
{
	ASSEMBLY* pAssembly;
	ASSEMBLY* pLoopAssembly;
	CONNECTION* pConnection;

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
		return ERR_ASSEMBLY_INSTANCE_USED;
#endif

	if ( (pAssembly = assemblyGetByInstance(iInstance)) == NULL )
		return ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID;

	/* Check that assembly is not used by one of the active or pending connections */
	pConnection = connectionGetFromAssmInstance(iInstance);
	if ( pConnection != NULL )
		return ERR_ASSEMBLY_INSTANCE_USED;

	EtIP_free( &pAssembly->pTagName, &pAssembly->iTagSize );

	gpnAssemblies--;

	/* Shift the requests with the bigger index to fill in the void */
	for( pLoopAssembly = pAssembly; pLoopAssembly < gpnAssemblies; pLoopAssembly++ )
	{
		memcpy( pLoopAssembly, pLoopAssembly+1, sizeof(ASSEMBLY) );

		/* Adjust next member pointer */
		pLoopAssembly->pnMembers = (EtIPAssemblyMemberConfig*) (((UINT8*) pLoopAssembly->pnMembers) - sizeof(ASSEMBLY));
	}

	return 0;
}

/*---------------------------------------------------------------------------
** assemblyRemoveAll( )
**
** Remove all assemblies from the array
**---------------------------------------------------------------------------
*/
void assemblyRemoveAll(void)
{
	ASSEMBLY* pAssembly;

	for( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
		EtIP_free( &pAssembly->pTagName, &pAssembly->iTagSize );

	gpnAssemblies = gAssemblies;
}

/*---------------------------------------------------------------------------
** assemblyGetByInstance( )
**
** Find an assembly based on the Instance number
**---------------------------------------------------------------------------
*/
ASSEMBLY* assemblyGetByInstance( UINT32 iInstance )
{
	ASSEMBLY* pAssembly;

	for( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
	{
		if ( pAssembly->iInstance == iInstance )
			return pAssembly;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** assemblyGetByTagName( )
**
** Find an assembly based on the associated tag name
**---------------------------------------------------------------------------
*/
ASSEMBLY* assemblyGetByTagName( UINT8* pData, UINT16 iDataSize )
{
	ASSEMBLY* pAssembly;
	UINT16 iStrDataSize;
	UINT16 iStrTagSize;
	UINT16 i;
	UINT8 *str1, *str2;

	if ( iDataSize == 0 )
		return NULL;
	
	/* Allow for the trailing zero be either included or not in tag names */
	iStrDataSize = (UINT16)((pData[iDataSize-1] == 0) ? (iDataSize-1) : iDataSize);

	for( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
	{
		if ( pAssembly->iTagSize == 0 )
			continue;

		/* Allow for the trailing zero be either included or not in tag names */
		iStrTagSize = (UINT16)((pAssembly->pTagName[pAssembly->iTagSize-1] == 0) ? (pAssembly->iTagSize-1) : pAssembly->iTagSize);

		if ( iStrDataSize == iStrTagSize )
		{
			for (i = 0, str1 = pData, str2 = pAssembly->pTagName; 
				 i < iStrTagSize; i++, str1++, str2++)
			{
				/* Tag names are case insensitive */
				if (toupper(*str1) != toupper(*str2))
					break;
			}

			if (i >= iStrTagSize)
				return pAssembly;
		}
	}

	return NULL;
}


/*---------------------------------------------------------------------------
** assemblyGetMember( )
**
** Find a member index based on the member Id
**---------------------------------------------------------------------------
*/
EtIPAssemblyMemberConfig* assemblyGetMember(ASSEMBLY* pAssembly, UINT32 iMember)
{
	EtIPAssemblyMemberConfig* pMember;

	for(pMember = pAssembly->Members; pMember < pAssembly->pnMembers; pMember++)
	{
		if ( pMember->iMember == iMember )
			return pMember;
	}

	return NULL;
}

/*---------------------------------------------------------------------------
** assemblyAddMember( )
**
** Add a new member for the assembly object. If members are used, this function 
** will be called multiple times after calling EtIPAddAssemblyInstance. 
** The first call will be for offset 0. All sebsequent ones will be for the 
** offset starting where the previous EtIPAddAssemblyMember ended. 
** EtIPAddAssemblyMember should cover all data size allocated for this
** assembly instance. If a few bits should be skipped EtIPAddAssemblyMember
** will be called with nMember set to INVALID_MEMBER. There is no byte or word 
** padding.
** iInstance must be a valid instance previously allocated with a 
** EtIPAddAssemblyInstance successful call. iMember is a member id associated
** with this particular member. iBitSize is the member size in bits.  
** Returns 0 in case of success or one of the ERR_ASSEMBLY_... error codes.
**---------------------------------------------------------------------------
*/
INT32 assemblyAddMember(UINT32 iInstance, UINT32 iMember, UINT16 iBitSize)
{
	ASSEMBLY* pAssembly;
	CONNECTION* pConnection;
	EtIPAssemblyMemberConfig *pMember, *pPrevMember;

	if ( (pAssembly = assemblyGetByInstance(iInstance)) == NULL )
		return ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID;

	/* Check that assembly is not used by one of the active or pending connections */
	pConnection = connectionGetFromAssmInstance(iInstance);
	if ( pConnection != NULL )
		return ERR_ASSEMBLY_INSTANCE_USED;

	if ((iMember != INVALID_MEMBER) && (assemblyGetMember(pAssembly, iMember) != NULL))
		return ERR_ASSEMBLY_MEMBER_ALREADY_EXIST;

	if (pAssembly->pnMembers >= &pAssembly->Members[MAX_MEMBERS])
		return ERR_ASSEMBLY_MEMBER_LIMIT_REACHED;

	/* quick reference */
	pMember = pAssembly->pnMembers;

	pMember->iMember = iMember;
	pMember->iSize = iBitSize;

	if (pAssembly->pnMembers > pAssembly->Members)
	{
		pPrevMember = pMember-1;
		pMember->iOffset = (UINT16)(pPrevMember->iOffset + pPrevMember->iSize);
	}
	else
		pMember->iOffset = 0;

	/* Make sure we have enough space in the instance to store all member data */
	if ( ((pMember->iOffset+pMember->iSize+7)/8) > pAssembly->iSize)
		return ERR_ASSEMBLY_MEMBER_SIZE_EXCEED_LIMIT;

	pAssembly->pnMembers++;

	return 0;
}

/*---------------------------------------------------------------------------
** assemblyRemoveAllMembers( )
**
** Removes previosly configured assembly members for this instance. Gives the
** client application a chance to repopulate member array if needed. There
** should be no active or pending connections using this assembly instance for
** this call to succeed.
** iAssemblyInstance is previously configured instance of the assembly object.
** Returns 0 in case of success or ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID
** the instance specified is invalid, or ERR_ASSEMBLY_INSTANCE_USED if
** there is an active or pending connection that is configured to use
** this instance.
**---------------------------------------------------------------------------
*/
INT32 assemblyRemoveAllMembers(UINT32 iAssemblyInstance)
{
	ASSEMBLY* pAssembly;
	CONNECTION* pConnection;

	if ( (pAssembly = assemblyGetByInstance(iAssemblyInstance)) == NULL )
		return ERR_ASSEMBLY_SPECIFIED_INSTANCE_INVALID;

	/* Check that assembly is not used by one of the active or pending connections */
	pConnection = connectionGetFromAssmInstance(iAssemblyInstance);
	if ( pConnection != NULL )
		return ERR_ASSEMBLY_INSTANCE_USED;

	pAssembly->pnMembers = pAssembly->Members;
	return 0;
}

/*---------------------------------------------------------------------------
** assemblyIncrementSequenceCount()
**
** Increment data sequence count for all I/O connection that share the
** iInstance.
**---------------------------------------------------------------------------
*/
static void assemblyIncrementSequenceCount(UINT32 iInstance)
{
	CONNECTION* pConnection;
	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if (!pConnection->cfg.bOriginator && pConnection->cfg.iTargetProducingConnPoint == iInstance )
		{
			pConnection->iOutDataSeqNbr++;

			if (((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) == ChangeOfState ) ||
				((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) == ApplicationTriggered ))
				connListTriggerCnxnProduction(pConnection);
		}
	}
}

#ifdef ET_IP_SCANNER
/*---------------------------------------------------------------------------
** assemblyAssignInpOffsForConnect()
**
** For the new connection returns the first unoccupied offset big enough 
** to accomodate. Return INVALID_OFFSET if out of space.
**---------------------------------------------------------------------------
*/
INT32 assemblyAssignInpOffsForConnect(CONNECTION* pConnection)
{
	INT32  i, nUpperBound;
#ifdef EIP_LARGE_MEMORY
	INT32 nTable;
#endif
	CONNECTION* pLoopConnection;

	if (pConnection->cfg.iInputScannerSize == 0)
	{
		pConnection->cfg.nInputScannerOffset = 0;
#ifdef EIP_LARGE_MEMORY
		pConnection->cfg.nInputScannerTable = 0;
#endif
		return 0;
	}

	nUpperBound = INPUT_SCANNER_ASSEMBLY_SIZE;
	if ( nUpperBound % 2 )
		nUpperBound--;

	if ((pConnection->cfg.nInputScannerOffset < 0) || 
		(pConnection->cfg.nInputScannerOffset > (nUpperBound - pConnection->cfg.iInputScannerSize))
#ifdef EIP_LARGE_MEMORY
		|| (pConnection->cfg.nInputScannerTable < 0) 
		|| (pConnection->cfg.nInputScannerTable > NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES-1)
#endif
		)
	{
		pConnection->cfg.nInputScannerOffset = INVALID_OFFSET;
#ifdef EIP_LARGE_MEMORY
		if ((pConnection->cfg.nInputScannerTable < 0) || 
			(pConnection->cfg.nInputScannerTable > NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES-1))
			pConnection->cfg.nInputScannerTable = INVALID_OFFSET;
#endif
	}
	else
	{	/* check that declared offset is not already used by another connection */
		for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
		{
			if ((pLoopConnection == pConnection)
#ifdef EIP_LARGE_MEMORY
				|| (pLoopConnection->cfg.nInputScannerTable != pConnection->cfg.nInputScannerTable)
#endif
				)
				continue;

			/* Check if this connection occupies any part of the space starting with offset pConnection->cfg.nInputScannerOffset and ending at (pConnection->cfg.nInputScannerOffset+pConnection->cfg.iInputScannerSize) */
			if ( BUFFER_INTERSECT( pLoopConnection->cfg.nInputScannerOffset, pLoopConnection->cfg.iInputScannerSize, pConnection->cfg.nInputScannerOffset, pConnection->cfg.iInputScannerSize ) )
			{
				pConnection->cfg.nInputScannerOffset = INVALID_OFFSET;
				break;
			}
		}
	}

	if ( pConnection->cfg.nInputScannerOffset == (INT32)INVALID_OFFSET )
	{
#ifdef EIP_LARGE_MEMORY
		if (pConnection->cfg.nInputScannerTable != (INT32)INVALID_OFFSET)
		{
#endif
			for ( i = nUpperBound - pConnection->cfg.iInputScannerSize; i >= 0; i-=2 )
			{
				/* Check if any connection in this table occupies space starting with offset i and ending at (i+pLoopConnection->cfg.iInputScannerSize) */
				for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
				{
					if ((pLoopConnection == pConnection)
#ifdef EIP_LARGE_MEMORY
						|| (pLoopConnection->cfg.nInputScannerTable != pConnection->cfg.nInputScannerTable)
#endif
						)
						continue;

					/* Check if this connection occupies any part of the space starting with offset i and ending at (i+pConnection->cfg.iInputScannerSize) */
					if ( BUFFER_INTERSECT( pLoopConnection->cfg.nInputScannerOffset, pLoopConnection->cfg.iInputScannerSize, i, pConnection->cfg.iInputScannerSize ) )
						break;
				}

				/* If no connection occipes it, we found an open slot */
				if (pLoopConnection == gpnConnections )
				{
					pConnection->cfg.nInputScannerOffset = i;
					break;
				}
			}
#ifdef EIP_LARGE_MEMORY
		}
		else
		{
			/* Go through each table and find an available offset */
			for (nTable = 0; nTable < NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES; nTable++)
			{
				for ( i = nUpperBound - pConnection->cfg.iInputScannerSize; i >= 0; i-=2 )
				{

					for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
					{
						if ((pLoopConnection == pConnection) || 
							(pLoopConnection->cfg.nInputScannerTable != pConnection->cfg.nInputScannerTable))
							continue;

						/* Check if this connection occupies any part of the space starting with offset i and ending at (i+pConnection->cfg.iInputScannerSize) */
						if ( BUFFER_INTERSECT( pLoopConnection->cfg.nInputScannerOffset, pLoopConnection->cfg.iInputScannerSize, i, pConnection->cfg.iInputScannerSize ) )
							break;
					}

					/* If no connection occipes it, we found an open slot */
					if (pLoopConnection == gpnConnections )
					{
						pConnection->cfg.nInputScannerOffset = i;
						pConnection->cfg.nInputScannerTable = nTable;
						break;
					}
				}

				/* we found an open slot */
				if (i >= 0)
					break;
			}
		}
#endif
	}

	DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"assemblyAssignInpOffsForConnect assigned input offset %d to connection instance %d", pConnection->cfg.nInputScannerOffset, pConnection->cfg.nInstance);
	
	return pConnection->cfg.nInputScannerOffset;
}

/*---------------------------------------------------------------------------
** assemblyAssignOutOffsForConnect()
**
** For the new connection returns the first unoccupied offset big enough 
** to accomodate. Return INVALID_OFFSET if out of space.
**---------------------------------------------------------------------------
*/
INT32 assemblyAssignOutOffsForConnect(CONNECTION* pConnection)
{
	INT32  i, nUpperBound;
	CONNECTION* pLoopConnection;
#ifdef EIP_LARGE_MEMORY
	INT32 nTable;
#endif

	if (pConnection->cfg.iOutputScannerSize == 0)
	{
		pConnection->cfg.nOutputScannerOffset = 0;
		return 0;
	}

	nUpperBound = OUTPUT_SCANNER_ASSEMBLY_SIZE;
	if ( nUpperBound % 2 )
		nUpperBound--;

	if ((pConnection->cfg.nOutputScannerOffset < 0) || 
		(pConnection->cfg.nOutputScannerOffset > (nUpperBound - pConnection->cfg.iOutputScannerSize))
#ifdef EIP_LARGE_MEMORY
		|| (pConnection->cfg.nOutputScannerTable < 0) 
		|| (pConnection->cfg.nOutputScannerTable > NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES-1)

#endif
		)
	{
		pConnection->cfg.nOutputScannerOffset = INVALID_OFFSET;
#ifdef EIP_LARGE_MEMORY
		if ((pConnection->cfg.nOutputScannerTable < 0) || 
			(pConnection->cfg.nOutputScannerTable > NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES-1))
			pConnection->cfg.nOutputScannerTable = INVALID_OFFSET;
#endif
	}
	else
	{	/* check that declared offset is not already used by another connection */
		for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
		{
			if ((pLoopConnection == pConnection)
#ifdef EIP_LARGE_MEMORY
				|| (pLoopConnection->cfg.nOutputScannerTable != pConnection->cfg.nOutputScannerTable)
#endif
				)
				continue;

			/* Check if this connection occupies any part of the space starting with offset pConnection->cfg.nOutputScannerOffset and ending at (pConnection->cfg.nOutputScannerOffset+pConnection->cfg.iOutputScannerSize) */
			if ( BUFFER_INTERSECT( pLoopConnection->cfg.nOutputScannerOffset, pLoopConnection->cfg.iOutputScannerSize, pConnection->cfg.nOutputScannerOffset, pConnection->cfg.iOutputScannerSize ) )
			{
				pConnection->cfg.nOutputScannerOffset = INVALID_OFFSET;
				break;
			}
		}
	}

	if ( pConnection->cfg.nOutputScannerOffset == (INT32)INVALID_OFFSET )
	{
#ifdef EIP_LARGE_MEMORY
		if (pConnection->cfg.nInputScannerTable != (INT32)INVALID_OFFSET)
		{
#endif
			for ( i = nUpperBound - pConnection->cfg.iOutputScannerSize; i >= 0; i-=2 )
			{
				/* Check if any connection occupies space starting with offset i and ending at (i+pLoopConnection->cfg.iOutputScannerSize) */
				for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
				{
					if ((pLoopConnection == pConnection)
#ifdef EIP_LARGE_MEMORY
						|| (pLoopConnection->cfg.nOutputScannerTable != pConnection->cfg.nOutputScannerTable)
#endif
						)
						continue;

					/* Check if this connection occupies any part of the space starting with offset i and ending at (i+pConnection->cfg.iOutputScannerSize) */
					if ( BUFFER_INTERSECT( pLoopConnection->cfg.nOutputScannerOffset, pLoopConnection->cfg.iOutputScannerSize, i, pConnection->cfg.iOutputScannerSize ) )					
						break;
				}

				/* If no connection occipes it, we found an open slot */
				if (pLoopConnection == gpnConnections )
				{
					pConnection->cfg.nOutputScannerOffset = i;
					break;
				}
			}
#ifdef EIP_LARGE_MEMORY
		}
		else
		{
			/* Go through each table and find an available offset */
			for (nTable = 0; nTable < NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES; nTable++)
			{
				for ( i = nUpperBound - pConnection->cfg.iOutputScannerSize; i >= 0; i-=2 )
				{

					for ( pLoopConnection = gConnections; pLoopConnection < gpnConnections; pLoopConnection++ )
					{
						if ((pLoopConnection == pConnection) || 
							(pLoopConnection->cfg.nOutputScannerTable != pConnection->cfg.nOutputScannerTable ))
							continue;

						/* Check if this connection occupies any part of the space starting with offset i and ending at (i+pConnection->cfg.iOutputScannerSize) */
						if ( BUFFER_INTERSECT( pLoopConnection->cfg.nOutputScannerOffset, pLoopConnection->cfg.iOutputScannerSize, i, pConnection->cfg.iOutputScannerSize ) )
							break;
					}

					/* If no connection occipes it, we found an open slot */
					if (pLoopConnection == gpnConnections )
					{
						pConnection->cfg.nOutputScannerOffset = i;
						pConnection->cfg.nOutputScannerTable = nTable;
						break;
					}
				}

				/* we found an open slot */
				if (i >= 0)
					break;
			}
		}
#endif
	}

	DumpStr2(TRACE_LEVEL_NOTICE, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
				"assemblyAssignOutOffsForConnect assigned output offset %d to connection instance %d", pConnection->cfg.nOutputScannerOffset, pConnection->cfg.nInstance);
	
	return pConnection->cfg.nOutputScannerOffset;
}
#endif /* #ifdef ET_IP_SCANNER */

/*---------------------------------------------------------------------------
** assemblyGetBitOffsetData( )
**
** Fills the pDest buffer from the beginning with the pSrc data offseted 
** by iSrcOffset. iLength is the data length to copy in bits.
**---------------------------------------------------------------------------
*/
static void assemblyGetBitOffsetData( UINT8* pSrc, UINT8* pDest, UINT16 iSrcOffset, UINT16 iLength )
{
	UINT8 leftShift;		/* amount to shift data from hi byte */
	UINT8 rightShift;		/* amount to shift overflow from low byte */

	/* reset the pSrc pointer and offset so that we are working with the first byte of the pSrc that we are interested in */
	pSrc += (iSrcOffset >> 3);
	iSrcOffset &= 7;

	/* set the shift sizes based on the offset into the message */
	leftShift  = (UINT8)iSrcOffset;
	rightShift = (UINT8)((8 - iSrcOffset) & 7);

	/* copy 8 bits at a time */
	while( iLength > 0 )
	{
		*pDest = (UINT8)((*pSrc) >> leftShift);

		if ( rightShift && (iLength > rightShift) )
			*pDest |= (*(pSrc+1) << rightShift);

		if (iLength < 8)
		{
			*pDest &= (0xFF >> (8 - iLength));
			break;
		}

		pSrc++;
		pDest++;
		iLength -= 8;
	}
}

/*-----------------------------------------------------------------------------
** assemblySetBitOffsetData( )
**
** Fills the pDest buffer offseted by iDestOffset with pSrc data from offset 0.
** iLength is the data length to copy in bits.
**-----------------------------------------------------------------------------
*/
static void assemblySetBitOffsetData( UINT8* pSrc, UINT8* pDest, UINT16 iDestOffset, UINT16 iLength )
{
	UINT8 leftShift;	/* amount to shift data from hi byte */
	UINT8 rightShift;	/* amount to shift overflow from low byte */
	UINT8 mask0;		/* low byte mask */
	UINT8 mask1;		/* high byte overflow mask */

	/* reset the pSrc pointer and offset so that we are working with the first byte of the pSrc that we are interested in */
	pDest += (iDestOffset >> 3);
	iDestOffset &= 7;

	/* set the shift sizes based on the offset into the message */
	leftShift  = (UINT8)iDestOffset;
	rightShift = (UINT8)(8 - iDestOffset);

	mask0 = (UINT8)(0xFF << leftShift);
	mask1 = (UINT8)(0xFF >> rightShift);

	/* copy 8 bits at a time */
	while( iLength > 0 )
	{
		if (iLength < 8)
		{
			mask0 &= (UINT8)(~(0xFF << (iLength + leftShift)));
			if (iLength > rightShift)
				mask1 &= (UINT8)(~(0xFF << (iLength - rightShift)));
			else
				mask1 = 0;

			iLength = 8;
		}

		/* stuff the new data into the masked spot in the current data */
		*pDest = (UINT8)((*pDest & ~mask0) | ((*pSrc << leftShift) & mask0));
		if (mask1)
			*(pDest+1)    = (UINT8)((*(pDest+1) & ~mask1) | ((*pSrc >> rightShift) & mask1));

		pSrc++;
		pDest++;
		iLength -= 8;
	}
}

/*---------------------------------------------------------------------------
** assemblyGetMemberData( )
**
** Fills the provided buffer with the assembly data for the particular 
** member of an assembly instance. pData is the pointer to the buffer. 
** iSize is the buffer length in bytes. 
** Returns the actual size of copied data which will be the least of 
** the member size and the passed buffer length.
**---------------------------------------------------------------------------
*/
UINT16 assemblyGetMemberData( UINT32 iInstance, UINT32 iMember, UINT8* pData, UINT16 iSize ) 
{
	ASSEMBLY* pAssembly;
	EtIPAssemblyMemberConfig* pMember;
	UINT16 iBitSize;
	UINT16 iByteSize;

	/* Check if assembly exists */
	if ((pAssembly = assemblyGetByInstance(iInstance)) == NULL )
		return 0;

	/* Check if member exists */
	if ((pMember = assemblyGetMember(pAssembly, iMember)) == NULL)
		return 0;

	iBitSize = pMember->iSize;

	/* Convert to bits */
	iSize *= 8;
	if ( iBitSize > iSize )
		iBitSize = iSize;

	/* Number of bytes used in pData */
	iByteSize = (UINT16)((iBitSize+7)/8);

	assemblyGetBitOffsetData(&gAssemblyBuffer[pAssembly->nOffset], pData, pMember->iOffset, iBitSize );

	/* Return number of bytes used in pData */
	return iByteSize;
}

/*---------------------------------------------------------------------------
** assemblySetMemberData( )
**
** Fills the assembly member with the data from the passed data buffer.
** iInstance and iMember identify the target member.
** pData is the pointer to the buffer to copy data from. 
** iSize is the buffer length in bytes. If member size is less than
** iSize only member size bits will be copied from the pData.
** Returns the actual size of copied data which will be the least of
** the member size and the passed buffer length.
**---------------------------------------------------------------------------
*/
UINT16 assemblySetMemberData( UINT32 iInstance, UINT32 iMember, UINT8* pData, UINT16 iSize ) 
{
	ASSEMBLY* pAssembly;
	EtIPAssemblyMemberConfig* pMember;
	UINT16 iBitSize;
	UINT16 iByteSize;

	if (iSize == 0 || iMember == INVALID_MEMBER)
		return 0;

	if ( (pAssembly = assemblyGetByInstance(iInstance)) == NULL )
	{
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, 0, 0, "Assembly instance not found");
		return 0;
	}

	pMember = assemblyGetMember(pAssembly, iMember);

	if (pMember == NULL)
	{
		DumpStr0(TRACE_LEVEL_NOTICE, TRACE_TYPE_EXPLICIT, 0, 0, "Member Id not found");
		return 0;
	}

	iBitSize = pMember->iSize;

	/* Convert to bits */
	iSize *= 8;
	if ( iBitSize > iSize )
		iBitSize = iSize;

	/* Number of bytes used in pData */
	iByteSize = (UINT16)((iBitSize+7)/8);

	assemblySetBitOffsetData(pData, &gAssemblyBuffer[pAssembly->nOffset], pMember->iOffset, iBitSize );

	assemblyIncrementSequenceCount(iInstance);

	return iByteSize;
}

/*---------------------------------------------------------------------------
** assemblyGetInstanceData( )
**
** Fills the provided buffer with the assembly data for the particular
** assembly instance. pData is the pointer to the buffer. iSize is the
** buffer length. Returns the actual size of copied data which will be
** the least of assembly object size and the passed buffer length.
**---------------------------------------------------------------------------
*/
UINT16 assemblyGetInstanceData( UINT32 iInstance, UINT8* pData, UINT16 iSize ) 
{
	ASSEMBLY* pAssembly = assemblyGetByInstance(iInstance);

	if ( pAssembly == NULL )
		return 0;

	if ( iSize > pAssembly->iSize )
		iSize = pAssembly->iSize;

	memcpy( pData, &gAssemblyBuffer[pAssembly->nOffset], iSize );

	return iSize;
}

/*---------------------------------------------------------------------------
** assemblySetInstanceData( )
**
** Fills the assembly object with the passed buffer data. pData is the
** pointer to the buffer. iSize is the buffer length. 
** Returns the actual size of copied data, which may be less than iSize
** if the assembly object size is less than iSize.
**---------------------------------------------------------------------------
*/
UINT16 assemblySetInstanceData( UINT32 iInstance, UINT8* pData, UINT16 iSize )
{
	ASSEMBLY* pAssembly;

	pAssembly = assemblyGetByInstance(iInstance);
	if ( pAssembly == NULL )
		return 0;

	if ( iSize > pAssembly->iSize)
		iSize = pAssembly->iSize;

	memcpy(&gAssemblyBuffer[pAssembly->nOffset], pData, iSize );

#ifdef CONNECTION_STATS
	pAssembly->lLastUpdateTimestamp = gdwTickCount;
#endif
	assemblyIncrementSequenceCount(iInstance);

	return iSize;
}

/*---------------------------------------------------------------------------
** assemblySetInstanceDataOffset( )
** //EATON_EIP_Changes:MAV - Created so I don't have to update the whole darn thing when it is made up of pieces.
**
** Fills the assembly object with the passed buffer data. pData is the
** pointer to the buffer. iSize is the buffer length.
** Returns the actual size of copied data, which may be less than iSize
** if the assembly object size is less than iSize.
**---------------------------------------------------------------------------
*/
UINT16 assemblySetInstanceDataOffset( UINT32 iInstance, UINT8* pData, UINT16 iOffset, UINT16 iSize )
{
    ASSEMBLY* pAssembly;


    pAssembly = assemblyGetByInstance(iInstance);
    if ( pAssembly == NULL )
        return 0;

    if ( ( iSize + iOffset ) > pAssembly->iSize)
        iSize = pAssembly->iSize - iOffset;

//Sanity check for the data.  In the case of debugging.
//	if ( ( pData == 0 ) || ( gAssemblyBuffer == 0 ) ||
//			( iSize >= 8 ) || ( iOffset >= 8 ) )
//	{
//		while(1){};
//	}

    memcpy(&gAssemblyBuffer[pAssembly->nOffset + iOffset], pData, iSize );

#ifdef CONNECTION_STATS
	pAssembly->lLastUpdateTimestamp = gdwTickCount;
#endif
    assemblyIncrementSequenceCount(iInstance);

    return iSize;
}
/*---------------------------------------------------------------------------
** assemblyGetCombinedData( )
**
** Fills the provided buffer with the assembly data for one or more
** assembly instances. pData is the pointer to the buffer. iSize is the
** buffer length. nOffset is the start copy offset in the assembly buffer.
** Return the number of bytes copied in the provided buffer.
**---------------------------------------------------------------------------
*/
UINT16 assemblyGetCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize )
{
	if ( nOffset < 0 || (nOffset+iSize) > ASSEMBLY_SIZE )
		return 0;

	memcpy( pData, &gAssemblyBuffer[nOffset], iSize );

	return iSize;
}

/*---------------------------------------------------------------------------
** assemblySetCombinedData( )
**
** Fills one or more assembly instances with the passed buffer data. pData is
** the pointer to the buffer. iSize is the buffer length.
** nOffset is the start copy offset in the assembly buffer.
** Return the number of bytes copied from the provided buffer.
**---------------------------------------------------------------------------
*/
UINT16 assemblySetCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize )
{
	ASSEMBLY* pAssembly;

	if ( nOffset < 0 || (nOffset+iSize) > ASSEMBLY_SIZE )
		return 0;

	memcpy( &gAssemblyBuffer[nOffset], pData, iSize );

	for ( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
	{
		if ( BUFFER_INTERSECT( pAssembly->nOffset, pAssembly->iSize, nOffset, iSize ) )
		{
			assemblyIncrementSequenceCount(pAssembly->iInstance);
		}
	}

	return iSize;
}

#ifdef ET_IP_SCANNER

/*-------------------------------------------------------------------------------
** assemblyGetScannerInputData( )
**
** Fills the provided buffer with the input scanner data for one or more
** connections. iInstance is the assembly instance that contains the buffer.
** pData is the pointer to the buffer. nSize is the number of bytes to copy.
** nOffset is the start copy offset in the input scanner buffer INPUT_SCANNER_ASSEMBLY_SIZE
** long where input data for all connections is stored.
** Return the number of bytes copied in the provided buffer, which will be equal
** to nSize if successful or 0 if nOffset+nSize exceed the INPUT_SCANNER_ASSEMBLY_SIZE.
**-------------------------------------------------------------------------------
*/
INT32 assemblyGetScannerInputData(UINT32 iInstance, UINT8* pData, INT32 nOffset, UINT16 iSize )
{
	if ( nOffset < 0 || (nOffset+iSize) > INPUT_SCANNER_ASSEMBLY_SIZE )
		return 0;

#if defined EIP_LARGE_MEMORY && !defined (EIP_NO_CCO)
	if (!assemblyIsScannerInputInstance(iInstance))
		return 0;
#else
	iInstance = ASSEMBLY_SCANNER_INPUT_INSTANCE;
#endif

	memcpy( pData, &gInputScannerAssembly[iInstance-ASSEMBLY_SCANNER_INPUT_INSTANCE][nOffset], iSize );

	return iSize;
}

/*-------------------------------------------------------------------------------
** assemblyGetScannerOutputData( )
**
** Fills the provided buffer with the output scanner data for one or more 
** connections. iInstance is the assembly instance that contains the buffer.
** pData is the pointer to the buffer. nSize is the number of bytes to copy. 
** nOffset is the start copy offset in the output scanner buffer OUTPUT_SCANNER_ASSEMBLY_SIZE 
** long where input data for all connections is stored.
** Return the number of bytes copied in the provided buffer, which will be equal 
** to nSize if successful or 0 if nOffset+nSize exceed the OUTPUT_SCANNER_ASSEMBLY_SIZE.
** Note that it is the client who is writing the output scanner data, so this call 
** will be made only if a task of the client would want to read back the data 
** another client task just wrote.
**-------------------------------------------------------------------------------
*/
INT32 assemblyGetScannerOutputData(UINT32 iInstance, UINT8* pData, INT32 nOffset, UINT16 iSize )
{
	if ( nOffset < 0 || (nOffset+iSize) > OUTPUT_SCANNER_ASSEMBLY_SIZE )
		return 0;

#if defined EIP_LARGE_MEMORY && !defined (EIP_NO_CCO)
	if (!assemblyIsScannerOutputInstance(iInstance))
		return 0;
#else
	iInstance = ASSEMBLY_SCANNER_OUTPUT_INSTANCE;
#endif

	memcpy( pData, &gOutputScannerAssembly[iInstance-ASSEMBLY_SCANNER_OUTPUT_INSTANCE][nOffset], iSize );

	return iSize;
}

/*------------------------------------------------------------------------------------------
** assemblySetScannerInputData( )
**
** Fills scanner input for one or more connection instances with the passed
** buffer data. iInstance is the assembly instance that contains the buffer.
** pData is the pointer to the buffer. nSize is the number of bytes to copy.
** nOffset is the start offset in the input scanner buffer of INPUT_SCANNER_ASSEMBLY_SIZE
** bytes where input data for all scanner connections is stored.
** Return the number of bytes copied from the provided buffer, which will be 
** equal to nSize if successful or 0 if nOffset+nSize exceed the INPUT_SCANNER_ASSEMBLY_SIZE.
**-------------------------------------------------------------------------------------------
*/
INT32 assemblySetScannerInputData(UINT32 iInstance, UINT8* pData, INT32 nOffset, UINT16 iSize )
{
	if ( nOffset < 0 || (nOffset+iSize) > INPUT_SCANNER_ASSEMBLY_SIZE )
		return 0;

#if defined EIP_LARGE_MEMORY && !defined (EIP_NO_CCO)
	if (!assemblyIsScannerInputInstance(iInstance))
		return 0;
#else
	iInstance = ASSEMBLY_SCANNER_INPUT_INSTANCE;
#endif

	memcpy( &gInputScannerAssembly[iInstance-ASSEMBLY_SCANNER_INPUT_INSTANCE][nOffset], pData, iSize );

	return iSize;
}

/*-----------------------------------------------------------------------------
** assemblySetScannerOutputData( )
**
** Fills scanner output for one or more connection instances with the passed
** buffer data. iInstance is the assembly instance that contains the buffer.
** pData is the pointer to the buffer. nSize is the number of bytes to copy.
** nOffset is the start offset in the output scanner buffer of 
** OUTPUT_SCANNER_ASSEMBLY_SIZE bytes where output data for all scanner connections is stored.
** Return the number of bytes copied from the provided buffer, which will be 
** equal to iSize if successful or 0 if nOffset+iSize exceed the OUTPUT_SCANNER_ASSEMBLY_SIZE.
**------------------------------------------------------------------------------
*/
INT32 assemblySetScannerOutputData(UINT32 iInstance, UINT8* pData, INT32 nOffset, UINT16 iSize)
{
	CONNECTION* pConnection;

	if ( nOffset < 0 || (nOffset+iSize) > OUTPUT_SCANNER_ASSEMBLY_SIZE )
		return 0;

#if defined EIP_LARGE_MEMORY && !defined (EIP_NO_CCO)
	if (!assemblyIsScannerOutputInstance(iInstance))
		return 0;
#else
	iInstance = ASSEMBLY_SCANNER_OUTPUT_INSTANCE;
#endif

	memcpy( &gOutputScannerAssembly[iInstance-ASSEMBLY_SCANNER_OUTPUT_INSTANCE][nOffset], pData, iSize );

	for ( pConnection = gConnections; pConnection < gpnConnections; pConnection++ )
	{
		if (((pConnection->cfg.bOriginator)
#ifndef EIP_NO_CCO
			|| (assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint))
#endif
			) &&
#ifdef EIP_LARGE_MEMORY
			(pConnection->cfg.nOutputScannerTable == ((INT32)iInstance-ASSEMBLY_SCANNER_OUTPUT_INSTANCE)) &&
#endif
			((BUFFER_INTERSECT(pConnection->cfg.nOutputScannerOffset, pConnection->cfg.iOutputScannerSize, nOffset, iSize)) ||
			 ((pConnection->cfg.bClass1VariableProducer) && (iSize == 0) && (pConnection->cfg.nOutputScannerOffset == nOffset))))
		{
			pConnection->iOutDataSeqNbr++;

			if (((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) == ChangeOfState ) ||
				((pConnection->cfg.bTransportType & CLASS_TYPE_MASK) == ApplicationTriggered ))
				connListTriggerCnxnProduction(pConnection);
		}
	}

	return iSize;
}

#endif

/*---------------------------------------------------------------------------
** assemblyNotifyMembersChange( )
**
** Compare the new data with the current data for the assembly instance members
** and send new member data notifications.
**---------------------------------------------------------------------------
*/
BOOL assemblyNotifyMembersChange( UINT32 iInstance, UINT32 iMember, UINT8* pData, UINT16 iSize )
{
#ifdef EIP_APP_MEMCMP
	return TRUE;
#else
	UINT16 iNumBytes;
	UINT8  curData[MAX_ASSEMBLY_SIZE];
	UINT8  newData[MAX_ASSEMBLY_SIZE];
	EtIPAssemblyMemberConfig* pMember;
	ASSEMBLY* pAssembly;
	BOOL bRet = FALSE;

	if ( (pAssembly = assemblyGetByInstance(iInstance)) == NULL )
		return FALSE;

	if ( iSize > pAssembly->iSize )
		iSize = pAssembly->iSize;

	for(pMember = pAssembly->Members; pMember < pAssembly->pnMembers; pMember++ )
	{
		if (iMember == INVALID_MEMBER || pMember->iMember == iMember)
		{
			iNumBytes = assemblyGetMemberData( iInstance, pMember->iMember, (UINT8*)curData, iSize );
			if ( iNumBytes )
			{
				assemblyGetBitOffsetData(pData, (UINT8*)newData, pMember->iOffset, pMember->iSize );

				if ( memcmp(curData, newData, iNumBytes) )
				{
					notifyEvent( NM_ASSEMBLY_NEW_MEMBER_DATA, iInstance, pMember->iMember );
					bRet = TRUE;
				}
			}
		}
	}

	return bRet;
#endif
}


/*---------------------------------------------------------------------------
** assemblyNotifyInstanceChange( )
**
** Compare the new data with the current data for the assembly instance 
** and send new instance data notifications.
**---------------------------------------------------------------------------
*/
BOOL assemblyNotifyInstanceChange( UINT32 iInstance, UINT8* pData, UINT16 iSize, BOOL bForceNotify)
{
	ASSEMBLY* pAssembly;

	pAssembly = assemblyGetByInstance(iInstance);

	if ( pAssembly == NULL )
		return FALSE;

	if ( iSize > pAssembly->iSize )
		iSize = pAssembly->iSize;

	if ((bForceNotify) ||
		(memcmp(pData, &gAssemblyBuffer[pAssembly->nOffset], iSize)))
	{
		notifyEvent( NM_ASSEMBLY_NEW_INSTANCE_DATA, iInstance, iSize);
		return TRUE;
	}

	return FALSE;
}

#ifdef ET_IP_SCANNER
#ifndef EIP_NO_CCO
/*---------------------------------------------------------------------------
** assemblyIsScannerInputInstance( )
**
** Encapulate check of scanner input assemblies
**---------------------------------------------------------------------------
*/
BOOL assemblyIsScannerInputInstance( UINT32 iInstance)
{
	if ((iInstance >= ASSEMBLY_SCANNER_INPUT_INSTANCE) && 
		(iInstance < ASSEMBLY_SCANNER_INPUT_INSTANCE+NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES))
		return TRUE;

	return FALSE;
}

/*---------------------------------------------------------------------------
** assemblyIsScannerInputInstance( )
**
** Encapulate check of scanner input assemblies
**---------------------------------------------------------------------------
*/
BOOL assemblyIsScannerOutputInstance( UINT32 iInstance)
{
	if ((iInstance >= ASSEMBLY_SCANNER_OUTPUT_INSTANCE) && 
		(iInstance < ASSEMBLY_SCANNER_OUTPUT_INSTANCE+NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES))
		return TRUE;

	return FALSE;
}
#endif /* EIP_NO_CCO */

/*---------------------------------------------------------------------------
** assemblyNtfyScannerInputChange( )
**
** Compare the new data with the current data for the scanner input data
** and send new scanner data notifications.
**---------------------------------------------------------------------------
*/
BOOL assemblyNtfyScannerInputChange( CONNECTION* pConnection, UINT8* pData, UINT16 iSize, BOOL bForceNotify)
{
	if (iSize > pConnection->cfg.iInputScannerSize)
		iSize = pConnection->cfg.iInputScannerSize;

#ifdef EIP_LARGE_MEMORY

	if ((bForceNotify) || 
		((iSize > 0) && (memcmp(pData, &gInputScannerAssembly[pConnection->cfg.nInputScannerTable-ASSEMBLY_SCANNER_INPUT_INSTANCE][pConnection->cfg.nInputScannerOffset], iSize) != 0)))
	{
		memcpy( &gInputScannerAssembly[pConnection->cfg.nInputScannerTable-ASSEMBLY_SCANNER_INPUT_INSTANCE][pConnection->cfg.nInputScannerOffset], pData, iSize );
#else
	if ((bForceNotify) ||
		((iSize > 0) && (memcmp(pData, &gInputScannerAssembly[0][pConnection->cfg.nInputScannerOffset], iSize) != 0)))
	{
		memcpy( &gInputScannerAssembly[0][pConnection->cfg.nInputScannerOffset], pData, iSize );
#endif
		notifyEvent( NM_CONNECTION_NEW_INPUT_SCANNER_DATA, pConnection->cfg.nInstance, iSize);
		return TRUE;
	}

	return FALSE;
}

#endif

#ifndef EIP_NO_DYNAMIC_ASSEMBLY
/*---------------------------------------------------------------------------
** assemblyCreateDynamic( )
**
** Respond to the Create dynamic instance request
**---------------------------------------------------------------------------
*/
static void assemblyCreateDynamic( REQUEST* pRequest )
{
	UINT16 iInstance = 0;
	UINT16 i, iVal;
	ASSEMBLY* pAssembly;

	/* If new instance is specified use it. Otherwise assigned automatically from the vendor specific range. */
	if ( pRequest->cfg.objectCfg.iDataSize >= 2 )
	{
		iInstance = UINT16_GET( pRequest->cfg.objectCfg.pData );

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
		{
			pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}
#endif
		pAssembly = assemblyGetByInstance( iInstance );
		if ( pAssembly != NULL )
		{
			/* Instance already created */
			pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}
	}
	else
	{
		for( i = ASSEMBLY_START_DYNAMIC_INSTANCE_RANGE1; i < ASSEMBLY_END_DYNAMIC_INSTANCE_RANGE1; i++ )
		{
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
				continue;
#endif
			pAssembly = assemblyGetByInstance( i );
			if ( pAssembly == NULL )
			{
				iInstance = i;
				break;
			}
		}

		if ( iInstance == 0 )
		{
			/* Object in a state where all dynamic instances are already used */
			pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}
	}

	/* Create a new dynamic assembly of max size available for any data - consuming, producing or configuration */
	if ( assemblyNew( iInstance, INVALID_OFFSET, MAX_ASSEMBLY_SIZE, AssemblyDynamic|AssemblyConsuming|AssemblyProducing|AssemblyConfiguration, NULL ) )
	{
		pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	iVal = ENCAP_TO_HS(iInstance);
	EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
}
#endif

/*--------------------------------------------------------------------------------
** assemblySendClassAttrSingle( )
**
** GetAttributeSingle request has been received for the Assembly object class
**--------------------------------------------------------------------------------
*/
static void assemblySendClassAttrSingle( REQUEST* pRequest )
{
	UINT16 iMaxInstance = 0;
	UINT16 iVal;
	UINT16 iTagSize;
	ASSEMBLY* pAssembly;
	UINT8* pData;
	UINT16* p16;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	if ( iTagSize == 0 )
	{
		/* Attribute is required */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	switch( iAttribute )
	{
	case ASSEMBLY_CLASS_ATTR_REVISION:
		iVal = ENCAP_TO_HS(ASSEMBLY_CLASS_REVISION);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ASSEMBLY_CLASS_ATTR_MAX_INSTANCE:
		for( pAssembly = gAssemblies; pAssembly < gpnAssemblies; pAssembly++ )
		{
			if ( pAssembly->iInstance > iMaxInstance )
				iMaxInstance = (UINT16)pAssembly->iInstance;
		}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if ( ASSEMBLY_SCANNER_INPUT_INSTANCE+NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES-1 > iMaxInstance )
			iMaxInstance = ASSEMBLY_SCANNER_INPUT_INSTANCE+NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES-1;
		if ( ASSEMBLY_SCANNER_OUTPUT_INSTANCE+NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES-1 > iMaxInstance )
			iMaxInstance = ASSEMBLY_SCANNER_OUTPUT_INSTANCE+NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES-1;
#endif
		iVal = ENCAP_TO_HS(iMaxInstance);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ASSEMBLY_CLASS_ATTR_NUM_INSTANCES:
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		iVal = (UINT16)ENCAP_TO_HS((gpnAssemblies-gAssemblies+NUM_ASSEMBLY_SCANNER_INPUT_INSTANCES+NUM_ASSEMBLY_SCANNER_OUTPUT_INSTANCES));
#else
		iVal = (UINT16)ENCAP_TO_HS((gpnAssemblies-gAssemblies));
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ASSEMBLY_CLASS_ATTR_OPT_ATTR_LIST:
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, NULL, sizeof(UINT16)*(ASSEMBLY_ATTR_OPTIONAL_COUNT+1));

		pData = pRequest->cfg.objectCfg.pData;

		UINT16_SET(pData, ASSEMBLY_ATTR_OPTIONAL_COUNT);
		pData += sizeof(UINT16);
		for (p16 = assemblyOptionalAttributes;
			p16 < &assemblyOptionalAttributes[ASSEMBLY_ATTR_OPTIONAL_COUNT];
			p16++, pData += sizeof(UINT16))
		{
			UINT16_SET(pData, *p16);
		}
		break;
	case ASSEMBLY_CLASS_ATTR_MAX_CLASS_ATTR:
		iVal = ENCAP_TO_HS(ASSEMBLY_CLASS_ATTR_MAX_INST_ATTR);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	case ASSEMBLY_CLASS_ATTR_MAX_INST_ATTR:
		iVal = ENCAP_TO_HS(ASSEMBLY_INSTANCE_SIZE);
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;
	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

#ifndef EIP_NO_DYNAMIC_ASSEMBLY
/*---------------------------------------------------------------------------
** assemblyDeleteDynamic( )
**
** Respond to the Delete dynamic instance request
**---------------------------------------------------------------------------
*/
static void assemblyDeleteDynamic( REQUEST* pRequest, UINT32 iInstance )
{
	ASSEMBLY* pAssembly;
	CONNECTION* pConnection;

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
#endif

	pAssembly = assemblyGetByInstance( iInstance );
	if ( pAssembly == NULL )
	{
		/* Instance not found */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if ( pAssembly->iType & AssemblyStatic )
	{
		/* Only dynamic assemblies can be deleted */
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	/* Delete not allowed for an active assembly */
	pConnection = connectionGetFromAssmInstance( iInstance );
	if ( pConnection != NULL )
	{
		pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	assemblyRemove( iInstance );
}
#endif

/*--------------------------------------------------------------------------------
** assemblySendConnectionInstanceAttrSingle( )
**
** GetAttributeSingle request has been received for the Assembly object instance
**--------------------------------------------------------------------------------
*/
static void assemblySendInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	UINT16 iVal;
	ASSEMBLY* pAssembly = NULL;
	UINT8 Buf[MAX_REQUEST_DATA_SIZE];
	UINT8* pData = Buf;
	UINT8* pAssemblyData;
	UINT16 iTagSize;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );
	INT32 nSize;

	EtIPAssemblyMemberConfig *pMember, *pMembers, *pMaxMember;
	EPATH pdu;
	pduInit( &pdu, TRUE );

	if ( iTagSize == 0 )
	{
		/* Attribute is required */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	if ((!assemblyIsScannerInputInstance(iInstance)) &&
		(!assemblyIsScannerOutputInstance(iInstance)))
	{
#endif

	pAssembly = assemblyGetByInstance( iInstance );
	if ( pAssembly == NULL )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	}
#endif

	switch( iAttribute )
	{
	case ASSEMBLY_MEMBER_COUNT:
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}
#endif
		/* If the assembly instance doesn't have members, the attribute won't be supported */
		if ((pAssembly->iType & AssemblyStatic) && (pAssembly->Members == pAssembly->pnMembers))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		iVal = (UINT16)ENCAP_TO_HS((pAssembly->pnMembers-pAssembly->Members));
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;

	case ASSEMBLY_MEMBER_LIST:

		pdu.bPadded = FALSE;
		pdu.bNumTags = 3;

		pdu.Tags[0].TagData = ASSEMBLY_CLASS;
		pdu.Tags[0].iTagDataSize = 1;
		pdu.Tags[0].iTagType = TAG_TYPE_CLASS;

		pdu.Tags[1].TagData = iInstance;

		pdu.Tags[1].iTagDataSize = (UINT16)(IS_WORD(iInstance) ? (IS_BYTE(iInstance) ? 1 : 2) : 4);
		pdu.Tags[1].iTagType = TAG_TYPE_INSTANCE;

		pdu.Tags[2].iTagType = TAG_TYPE_MEMBER;

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}
#endif
		pMembers = pAssembly->Members;
		pMaxMember = pAssembly->pnMembers;

		/* If the assembly instance doesn't have members, the attribute won't be supported */
		if ((pAssembly->iType & AssemblyStatic) && (pMembers == pMaxMember))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		for(pMember = pMembers; pMember < pMaxMember; pMember++ )
		{
			pdu.Tags[2].TagData = pMember->iMember;
			pdu.Tags[2].iTagDataSize = (UINT16)(IS_BYTE(pMember->iMember) ? 1 : 2);

			iVal = pMember->iSize;
			UINT16_SET(pData, iVal);
			pData += 2;

			if (  pMember->iMember == INVALID_MEMBER ) /* empty path */
				iVal = 0;
			else
				iVal = (UINT16)pduGetBuildLen( &pdu );

			UINT16_SET(pData, iVal);
			pData += 2;

			if ( iVal )
			{
				nSize = pduBuild( &pdu, pData );
				pData += nSize;
			}
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, Buf, (UINT16)(pData - Buf) );
		break;

	case ASSEMBLY_INSTANCE_SIZE:
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (assemblyIsScannerInputInstance(iInstance))
		{
			iVal = (UINT16)ENCAP_TO_HS(INPUT_SCANNER_ASSEMBLY_SIZE);
		}
		else if (assemblyIsScannerOutputInstance(iInstance))
		{
			iVal = (UINT16)ENCAP_TO_HS(OUTPUT_SCANNER_ASSEMBLY_SIZE);
		}
		else
		{
#endif
			iVal = ENCAP_TO_HS((pAssembly->iSize));
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		}
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, (UINT8*)&iVal, sizeof(UINT16) );
		break;

	case ASSEMBLY_INSTANCE_DATA:
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (assemblyIsScannerInputInstance(iInstance))
		{
			pAssemblyData = (UINT8*)&gInputScannerAssembly[iInstance-ASSEMBLY_SCANNER_INPUT_INSTANCE];
			nSize = INPUT_SCANNER_ASSEMBLY_SIZE;
		}
		else if (assemblyIsScannerOutputInstance(iInstance))
		{
			pAssemblyData = (UINT8*)&gOutputScannerAssembly[iInstance-ASSEMBLY_SCANNER_OUTPUT_INSTANCE];
			nSize = OUTPUT_SCANNER_ASSEMBLY_SIZE;
		}
		else
		{
#endif
			pAssemblyData = &gAssemblyBuffer[pAssembly->nOffset];
			nSize = pAssembly->iSize;
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		}
#endif
		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, pAssemblyData, (UINT16)nSize);
		break;

	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
	}
}

/*--------------------------------------------------------------------------------
** assemblySetInstanceAttrSingle( )
**
** SetAttributeSingle request has been received for the Assembly object instance
**--------------------------------------------------------------------------------
*/
static void assemblySetInstanceAttrSingle( REQUEST* pRequest, UINT32 iInstance )
{
	CONNECTION* pConnection;
	ASSEMBLY* pAssembly;
	UINT16 iTagSize, iLen, iMemberSize, iPathSize;
	UINT32 iMember;
	UINT32 iAttribute = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_ATTRIBUTE, 0, &iTagSize );

	INT32 nSize;
	EPATH pdu;
	UINT8* pData;
	BOOL bForceNotify = FALSE;

	pduInit( &pdu, TRUE );

	if ( iTagSize == 0 )
	{
		/* Attribute is required */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	if ((!assemblyIsScannerInputInstance(iInstance)) &&
		(!assemblyIsScannerOutputInstance(iInstance)))
	{
#endif

	pAssembly = assemblyGetByInstance( iInstance );
	if ( pAssembly == NULL )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	/* No attributes are allowed to be set for an active assembly */
	pConnection = connectionGetFromAssmInstance( pAssembly->iInstance );
	if ( pConnection != NULL )
	{
		pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	}
#endif

	switch( iAttribute )
	{
	case ASSEMBLY_MEMBER_LIST:

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			break;
		}
#endif

		/* Not allowed to set member list for a static assembly */
		if ( pAssembly->iType & AssemblyStatic )
		{
			if (pAssembly->Members == pAssembly->pnMembers)
				pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			else
				pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
			break;
		}

		assemblyRemoveAllMembers( pAssembly->iInstance );

		pData = pRequest->cfg.objectCfg.pData;
		nSize = pRequest->cfg.objectCfg.iDataSize;

		while( nSize >= 4 )
		{
			iMemberSize = UINT16_GET( pData );
			pData += 2;

			iPathSize = UINT16_GET( pData );
			pData += 2;

			if ( iPathSize == 0 )
			{
				assemblyAddMember( pAssembly->iInstance, INVALID_MEMBER, iMemberSize );
			}
			else if ( nSize >= (4 + iPathSize) )
			{
				pduParse(pData, &pdu, FALSE, iPathSize);
				iMember = (UINT32)pduGetTagByType(&pdu, TAG_TYPE_MEMBER, 0, &iTagSize);
				if ( iTagSize > 0 )
					assemblyAddMember( pAssembly->iInstance, iMember, iMemberSize );
				pData += iPathSize;
			}

			nSize -= (4 + iPathSize);
		}
		break;

	case ASSEMBLY_INSTANCE_DATA:

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
		if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
			break;
		}
#endif

		if ((pAssembly->iType & ASSEMBLY_DIRECTION_TYPE_FLAG) == AssemblyProducing)
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
			break;
		}

		iLen = pRequest->cfg.objectCfg.iDataSize;

		if ( iLen > pAssembly->iSize )
		{
			pRequest->bGeneralError = ROUTER_ERROR_TOO_MUCH_DATA;
			break;
		}

		if ( iLen < pAssembly->iSize )
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			break;
		}

		/* Check if member data changed and notify about individual members being updated */
		assemblyNotifyMembersChange( pAssembly->iInstance, INVALID_MEMBER, pRequest->cfg.objectCfg.pData, iLen );

#ifdef EIP_APP_MEMCMP
		bForceNotify = TRUE;
#endif
		/* Check if instance data as a whole has been changed and notify if needed */
		if ( assemblyNotifyInstanceChange( pAssembly->iInstance, pRequest->cfg.objectCfg.pData, iLen, bForceNotify) )
			assemblySetInstanceData(pAssembly->iInstance, pRequest->cfg.objectCfg.pData, iLen);
		break;

	case ASSEMBLY_MEMBER_COUNT:

		if (
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
			(assemblyIsScannerInputInstance(iInstance)) ||
			(assemblyIsScannerOutputInstance(iInstance)) ||
#endif
			((pAssembly->iType & AssemblyStatic) && (pAssembly->Members == pAssembly->pnMembers)))
		{
			pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
			break;
		}

		/* intentional break */
	case ASSEMBLY_INSTANCE_SIZE:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		break;

	default:
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SUPPORTED;
		break;
	}

	EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
}

/*--------------------------------------------------------------------------------
** assemblySendMember( )
**
** GetMember request has been received for the Assembly object instance
**--------------------------------------------------------------------------------
*/
static void assemblySendMember( REQUEST* pRequest, UINT32 iInstance )
{
	ASSEMBLY* pAssembly;
	UINT16 iTagSize;
	UINT32 iStartMember;
	UINT32 iMaxMember = 0;
	UINT8 Buf[MAX_REQUEST_DATA_SIZE];
	UINT16 iSize;
	UINT16 iMemberCount = 0;
	UINT16 iMembers = 1;			/* Default is a single member */
	BOOL bExtendedFormat = FALSE;
	UINT8* pData = NULL;
	EtIPAssemblyMemberConfig *pMember;

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	INT32 nOffset;

	if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
	{
		iStartMember = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize );
		nOffset = iStartMember * 2;

		if (assemblyIsScannerInputInstance(iInstance))
		{
			if ( (nOffset + sizeof(UINT16)) > INPUT_SCANNER_ASSEMBLY_SIZE )
			{
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				return; 
			}
			
			pData = &gInputScannerAssembly[iInstance-ASSEMBLY_SCANNER_INPUT_INSTANCE][nOffset];
		}

		else if (assemblyIsScannerOutputInstance(iInstance))
		{
			if ( (nOffset + sizeof(UINT16)) > OUTPUT_SCANNER_ASSEMBLY_SIZE )
			{
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				return;
			}

			pData = &gOutputScannerAssembly[iInstance-ASSEMBLY_SCANNER_OUTPUT_INSTANCE][nOffset];
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, pData, sizeof(UINT16) );        
	}
	else
	{
#endif
		pAssembly = assemblyGetByInstance( iInstance );

		if ( pAssembly == NULL )
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		if (pAssembly->pnMembers == pAssembly->Members)
		{
			/* Member list has not been set yet for this assembly */
			if (pAssembly->iType & AssemblyStatic)
			{
				/* If members weren't already created, the service isn't valid */
				pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
			}
			else
			{
				pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
			}

			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		iStartMember = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize );

		if ( iTagSize == 0 )
		{
			/* Member is required */
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return; 
		}

		if ( iStartMember & ASSEMBLY_EXTENDED_FORMAT_FLAG )
			bExtendedFormat = TRUE;

		if ( bExtendedFormat )
		{
			if ( pRequest->cfg.objectCfg.iDataSize < 1 )
			{
				pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				return;
			}

			pData = pRequest->cfg.objectCfg.pData;

			/* The only extended format supported is Multiple Sequential Members one */
			if ( (*pData) != ASSEMBLY_MULTIPLE_MEMBER_FLAG )
			{
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				return;
			}

			pData++;
			iMembers = UINT16_GET( pData );
		}

		for(pMember = pAssembly->Members; pMember < pAssembly->pnMembers; pMember++ )
		{
			if ( pMember->iMember == iStartMember )    /* Exact match is found */
				break;

			if ( pMember->iMember > iMaxMember )
				iMaxMember = pMember->iMember;
		}

		if (pMember == pAssembly->pnMembers)
		{
			/* Exact match not found */
			if ( iStartMember > iMaxMember )
			{
				/* If specified member larger than the max member id - provide one last member in the list */
				pMember = pAssembly->pnMembers - 1;
				iMembers = 1;
			}
			else
			{
				/* Otherwise return invalid member error */
				pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
				EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
				return;
			}
		}

		pData = Buf;
		if ( bExtendedFormat )
		{
			UINT16_SET(pData, iMembers);
			pData += sizeof(UINT16);
		}

		for(; pMember < pAssembly->pnMembers; pMember++)
		{
			UINT16_SET(pData, pMember->iMember);
			pData += sizeof(UINT16);

			iSize = assemblyGetMemberData( iInstance, pMember->iMember, pData, (UINT16)(MAX_REQUEST_DATA_SIZE - (pData - Buf)) );
			pData += iSize;

			if ( (++iMemberCount == iMembers) || ((pData - Buf) >= MAX_REQUEST_DATA_SIZE) )
				break;
		}

		EtIP_realloc( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize, Buf, (UINT16)(pData - Buf) );        

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	}
#endif
}

/*--------------------------------------------------------------------------------
** assemblySetMember( )
**
** SetMember request has been received for the Assembly object instance
**--------------------------------------------------------------------------------
*/
static void assemblySetMember( REQUEST* pRequest, UINT32 iInstance )
{
	ASSEMBLY* pAssembly;
	EtIPAssemblyMemberConfig* pMember;
	UINT16 iTagSize, iSize, iDataSize;
	UINT32 iStartMember;
	UINT16 iMemberCount = 0;
	UINT16 iMembers = 1;			/* Default is a single member */
	BOOL bExtendedFormat = FALSE;
	UINT8* pData;

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
	{
		pRequest->bGeneralError = ROUTER_ERROR_ATTR_NOT_SETTABLE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
#endif

	pAssembly = assemblyGetByInstance( iInstance );

	if ( pAssembly == NULL )
	{
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	if (pAssembly->pnMembers == pAssembly->Members)
	{
		/* Member list has not been set yet for this assembly */
		if (pAssembly->iType & AssemblyStatic)
		{
			/* If members weren't already created, the service isn't valid */
			pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		}
		else
		{
			pRequest->bGeneralError = ROUTER_ERROR_OBJECT_STATE_CONFLICT;
		}
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	if (assemblyIsScannerInputInstance(iInstance) || assemblyIsScannerOutputInstance(iInstance))
	{
		pRequest->bGeneralError = ROUTER_ERROR_MEMBER_NOT_SETTABLE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}
#endif

	if ((pAssembly->iType & ASSEMBLY_DIRECTION_TYPE_FLAG) == AssemblyProducing)
	{
		pRequest->bGeneralError = ROUTER_ERROR_MEMBER_NOT_SETTABLE;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	iStartMember = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_MEMBER, 0, &iTagSize );

	if ( iTagSize == 0 )
	{
		/* Member is required */
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		return;
	}

	pData = pRequest->cfg.objectCfg.pData;
	iDataSize = pRequest->cfg.objectCfg.iDataSize;

	if ( iStartMember & ASSEMBLY_EXTENDED_FORMAT_FLAG )
		bExtendedFormat = TRUE;

	if ( bExtendedFormat )
	{
		if ( iDataSize < 3 )
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		/* The only extended format supported is Multiple Sequential Members one */
		if ( (*pData) != ASSEMBLY_MULTIPLE_MEMBER_FLAG )
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		pData++;

		iMembers = UINT16_GET( pData );
		pData += sizeof(UINT16);

		iDataSize -= 3;
	}

	/* Verify there is enough data to go around */
	while( (++iMemberCount) <= iMembers )
	{
		pMember = assemblyGetMember(pAssembly, iMembers);
		if (pMember == NULL)
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		if (iDataSize < ((pMember->iSize+7)/8))
		{
			pRequest->bGeneralError = ROUTER_ERROR_NOT_ENOUGH_DATA;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		iDataSize += ((pMember->iSize+7)/8);
	}

	iMemberCount = 0;
	/* Apply the member data */
	while( (++iMemberCount) <= iMembers )
	{
		iSize = assemblySetMemberData( iInstance, iStartMember, pData, (UINT16)(pRequest->cfg.objectCfg.iDataSize - (pData - pRequest->cfg.objectCfg.pData)) );
		pData += iSize;

		if ( iSize == 0 )
		{
			pRequest->bGeneralError = ROUTER_ERROR_INVALID_MEMBER;
			EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
			return;
		}

		notifyEvent( NM_ASSEMBLY_NEW_MEMBER_DATA, iInstance, iStartMember );

		if ( (pData - pRequest->cfg.objectCfg.pData) >= pRequest->cfg.objectCfg.iDataSize )
			break;
	}
}

/*---------------------------------------------------------------------------
** assemblyParseClassRequest( )
**
** Respond to the class request
**---------------------------------------------------------------------------
*/
static void assemblyParseClassRequest( REQUEST* pRequest )
{
	switch( pRequest->cfg.objectCfg.bService )
	{
#ifndef EIP_NO_DYNAMIC_ASSEMBLY
	case SVC_CREATE:
		assemblyCreateDynamic( pRequest );
		break;
#endif
	case SVC_GET_ATTR_SINGLE:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		assemblySendClassAttrSingle( pRequest );
		break;
	default:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		break;
	}
}

/*---------------------------------------------------------------------------
** assemblyParseInstanceRequest( )
**
** Respond to the instance request
**---------------------------------------------------------------------------
*/
static void assemblyParseInstanceRequest( REQUEST* pRequest, UINT32 iInstance )
{
	ASSEMBLY* pAssembly;

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	if ((!assemblyIsScannerInputInstance(iInstance)) &&
		(!assemblyIsScannerOutputInstance(iInstance)))
	{
#endif

	pAssembly = assemblyGetByInstance( iInstance );
	if ( pAssembly == NULL )
	{
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return;
	}

#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
	}
#endif

	switch( pRequest->cfg.objectCfg.bService )
	{
#ifndef EIP_NO_DYNAMIC_ASSEMBLY
	case SVC_DELETE:
		assemblyDeleteDynamic( pRequest, iInstance );
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;
#endif

	case SVC_SET_ATTR_SINGLE:
		assemblySetInstanceAttrSingle( pRequest, iInstance );
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;

	case SVC_SET_MEMBER:
		assemblySetMember( pRequest, iInstance );
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		break;

	case SVC_GET_ATTR_SINGLE:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		assemblySendInstanceAttrSingle( pRequest, iInstance );
		break;

	case SVC_GET_MEMBER:
		assemblySendMember( pRequest, iInstance );
		break;

	default:
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_BAD_SERVICE;
		break;
	}
}

/*---------------------------------------------------------------------------
** assemblyParseClassInstanceRqst( )
**
** Determine if it's request for the Class or for the particular instance
**---------------------------------------------------------------------------
*/
void assemblyParseClassInstanceRqst( REQUEST* pRequest )
{
	UINT16 iTagSize;
	UINT32 iInstance = (UINT32)pduGetTagByType( &pRequest->cfg.objectCfg.PDU, TAG_TYPE_INSTANCE, 0, &iTagSize );

	if ( iTagSize == 0 )
	{
		/* Instance is required even for a class attribute */
		EtIP_free( &pRequest->cfg.objectCfg.pData, &pRequest->cfg.objectCfg.iDataSize );
		pRequest->bGeneralError = ROUTER_ERROR_INVALID_DESTINATION;
		return;
	}

	if ( iInstance == 0 )
		assemblyParseClassRequest( pRequest );
	else
		assemblyParseInstanceRequest( pRequest, iInstance );
}
