/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
typedef unsigned char MMSd_Quality[2];

#define MAX_STATUS_POINTS 4
#define MAX_ANALOG_POINTS 4
#define INITIAL_GOOSE_WAIT 31000

typedef struct MyMV
{
	float mag_f;
	MMSd_UtcBTime t;
	MMSd_Quality q;
	char* pName;
} MyMV;

typedef struct MyINS
{
	int stVal;
	MMSd_UtcBTime t;
	MMSd_Quality q;
	char* pName;
} MyINS;

/* Configuration values for Goose streams */
/* This contains all of the parameters required for
   Goose inputs and output streams */

typedef enum { Test_Display, Test_Loopback, Test_LoopbackIncrement } TestMode;

typedef struct MyGooseConfig* MyGooseConfigPtr;

typedef struct MyGooseConfig
{
	char goID[MMSD_OBJECT_NAME_LEN];
	char gocbRef[MMSD_OBJECT_NAME_LEN];
	char datSetName[MMSD_OBJECT_NAME_LEN];
	TMW_Mac_Address multicastAddr;
	IEC_Goose_Strategy* pStrategy;
	unsigned long confRev;
	unsigned char ndsCommissioning;
	unsigned char test;
	unsigned int vlan_AppID;
	unsigned int vlan_VID;
	unsigned char vlan_Priority;
	int nInsPoints;
	MyINS* pInsPoints;
	int nMvPoints;
	MyMV* pMvPoints;
	TestMode nTestMode;
	MyGooseConfigPtr pOther;
	IEC_Goose_Context* pIecGooseContext;
	char** memberNames;
	int nMemberNames;
	int nQueryDone;
} MyGooseConfig;

typedef struct MyTestContext
{
	TestMode nTestMode;
	MyGooseConfig* pInputGoose;
	MyGooseConfig* pOutputGoose;
	IEC_Goose_Context* pIecGooseContext;
} MyTestContext;

int SendGooseData( IEC_Goose_Context* pIecGooseContext,
				   MyGooseConfig* pGooseParams );

void service_goose( IEC_Goose_Context* pIecGooseContext );

void* StartGooseTest( TestMode nTestMode, int bSwitch, int bEtherLoop, char* pInAddr, char* pOutAddr );

void StopGooseTest( void* pTestHandle );

void SendTestData( void* pTestHandle );

void ServiceTest( void* pTestHandle );

void SendGooseFirst( void* pTestHandle );