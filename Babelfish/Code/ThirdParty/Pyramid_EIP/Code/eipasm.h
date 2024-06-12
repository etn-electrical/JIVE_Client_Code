/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPASM.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Assembly data storage and handling
**
*****************************************************************************
*/

#ifndef EIPASM_H
#define EIPASM_H


#define ASSEMBLY_SIZE					(MAX_ASSEMBLIES*MAX_ASSEMBLY_SIZE)		/* Size of each input and output I/O data tables */

#define ASSEMBLY_CLASS					4		/* Assembly class identifier */
#define ASSEMBLY_CLASS_REVISION			2		/* Assembly class revision */

#define ASSEMBLY_CLASS_ATTR_REVISION		1	/* Revision */
#define ASSEMBLY_CLASS_ATTR_MAX_INSTANCE	2	/* Max instance */
#define ASSEMBLY_CLASS_ATTR_NUM_INSTANCES	3	/* Number of instances */
#define ASSEMBLY_CLASS_ATTR_OPT_ATTR_LIST	4	/* List of optional attributes */
#define ASSEMBLY_CLASS_ATTR_MAX_CLASS_ATTR	6	/* Max class attribute number implemented */
#define ASSEMBLY_CLASS_ATTR_MAX_INST_ATTR	7	/* Max instance attribute number implemented */


#define ASSEMBLY_MEMBER_COUNT				0x01	/* Number of members in the assembly instance */
#define ASSEMBLY_MEMBER_LIST				0x02	/* Member List where each member is represented by its EPATH */
#define ASSEMBLY_INSTANCE_DATA				0x03	/* Data as a byte array */
#define ASSEMBLY_INSTANCE_SIZE				0x04	/* Size of data (in bytes) */

#define ASSEMBLY_EXTENDED_FORMAT_FLAG		0x80	/* 15th bit of teh Member Id indicates whether extended format should be used */

#define ASSEMBLY_MULTIPLE_MEMBER_FLAG		0x1		/* If extended flag is set to 1 use multiple member Id service spec */

#define ASSEMBLY_START_DYNAMIC_INSTANCE_RANGE1		0x64	/* Start of the dynamic instance range */
#define ASSEMBLY_END_DYNAMIC_INSTANCE_RANGE1		0xC7	/* End of the dynamic instance range */
#define ASSEMBLY_START_DYNAMIC_INSTANCE_RANGE2		0x300	/* Start of the dynamic instance range */
#define ASSEMBLY_END_DYNAMIC_INSTANCE_RANGE2		0x4ff	/* End of the dynamic instance range */

#define ASSEMBLY_ATTR_OPTIONAL_COUNT				1		/* Number of optional attributes implemented as defined by the CIP spec */

#define ASSEMBLY_CLASS_ATTR_SIZE    10

/* Structure used to store Assembly instance configuration */
typedef struct tagASSEMBLY
{
	UINT32 iInstance;								/* Unique assembly object instance. Examples are connection connection points and connection configuration instance. */
	INT32  nOffset;									/* Offset from the beginning of the buffer. */
	UINT16 iSize;									/* Assembly size */
	UINT16 iType;									/* Combination of a type from EtIPAssemblyAccessType with a type from EtIPAssemblyDirectionType and, optionally, one of the types from tagEtIPAssemblyAdditionalFlag */
	EtIPAssemblyMemberConfig Members[MAX_MEMBERS];	/* Member list */
	EtIPAssemblyMemberConfig* pnMembers;			/* Pointer to the next available member*/
	UINT8* pTagName;								/* ASCII name associated with this instance */
	UINT16 iTagSize;								/* Size of the assembly ASCII name */
#ifdef CONNECTION_STATS
	UINT32 lLastUpdateTimestamp;
#endif
}
ASSEMBLY;

#ifdef EIP_LARGE_BUF_ON_HEAP
extern ASSEMBLY* gAssemblies;						/* Assembly objects array */
#else
extern ASSEMBLY  gAssemblies[MAX_ASSEMBLIES];		/* Assembly objects array */
#endif

extern ASSEMBLY* gpnAssemblies;

extern void  assemblyInit(void);
extern INT32 assemblyNew( UINT32 iInstance, INT32  nOffset, UINT16 iSize, UINT16 iType, INT8* szAssemblyName );
extern INT32 assemblyRemove( UINT32 iInstance );
extern void  assemblyRemoveAll(void);

extern ASSEMBLY* assemblyGetByInstance( UINT32 iInstance );

#ifdef ET_IP_SCANNER
#ifndef EIP_NO_CCO
extern BOOL assemblyIsScannerInputInstance( UINT32 iInstance);
extern BOOL assemblyIsScannerOutputInstance( UINT32 iInstance);
#endif
extern BOOL assemblyNtfyScannerInputChange( CONNECTION* pConnection, UINT8* pData, UINT16 iSize, BOOL bForceNotify);

extern INT32 assemblyGetScannerInputData(UINT32 iInstance, UINT8* pData, INT32 nOffset, UINT16 iSize);
extern INT32 assemblyGetScannerOutputData(UINT32 iInstance,  UINT8* pData, INT32 nOffset, UINT16 iSize);
extern INT32 assemblySetScannerInputData(UINT32 iInstance, UINT8* pData, INT32 nOffset, UINT16 iSize);
extern INT32 assemblySetScannerOutputData(UINT32 iInstance, UINT8* pData, INT32 nOffset, UINT16 iSize);
#endif

extern EtIPAssemblyMemberConfig* assemblyGetMember(ASSEMBLY* pAssembly, UINT32 iMember);
extern INT32 assemblyAddMember(UINT32 iInstance, UINT32 iMember, UINT16 iBitSize);
extern INT32 assemblyRemoveAllMembers(UINT32 iAssemblyInstance);

#ifdef ET_IP_SCANNER
extern INT32 assemblyAssignInpOffsForConnect(CONNECTION* pConnection);
extern INT32 assemblyAssignOutOffsForConnect(CONNECTION* pConnection);
#endif

extern UINT16 assemblyGetMemberData( UINT32 iInstance, UINT32 iMember, UINT8* pData, UINT16 iSize ); 
extern UINT16 assemblySetMemberData( UINT32 iInstance, UINT32 iMember, UINT8* pData, UINT16 iSize ); 

extern UINT16 assemblyGetInstanceData( UINT32 iInstance, UINT8* pData, UINT16 iSize ); 
/*EATON_EIP_Changes:MAV*/
extern uint16_t assemblySetInstanceDataOffset( uint32_t iInstance, uint8_t* pData, uint16_t iOffset, uint16_t iSize ); //#k: Ori- Not present & hernce giving compile error in "eipclien.c"
extern UINT16 assemblySetInstanceData( UINT32 iInstance, UINT8* pData, UINT16 iSize ); 

extern UINT16 assemblyGetCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize ); 
extern UINT16 assemblySetCombinedData( UINT8* pData, INT32 nOffset, UINT16 iSize ); 

extern BOOL assemblyNotifyMembersChange( UINT32 iInstance, UINT32 iMember, UINT8* pData, UINT16 iSize );
extern BOOL assemblyNotifyInstanceChange( UINT32 iInstance, UINT8* pData, UINT16 iSize, BOOL bForceNotify);

extern void assemblyParseClassInstanceRqst( REQUEST* pRequest );

extern ASSEMBLY* assemblyGetByTagName( UINT8* pData, UINT16 iDataSize );

#endif /* #ifndef EIPASM_H */
