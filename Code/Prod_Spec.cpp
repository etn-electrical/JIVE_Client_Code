/*
 *****************************************************************************************
 *
 *		Copyright 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Prod_Spec.h"
#include "Config/App_IO_Config.h"
#include "DCI_NV_Mem.h"
#include "Services.h"
#include "STDLib_MV.h"
#include "DCI.h"
#include "Toolkit_Revision.h"
#include "Prod_Spec_MEM.h"
#include "NV_Mem.h"
#include "Prod_Spec_OS.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
/// This is a key which is used to reset all the DCI parameters.  This can be used
/// to refresh the NV when the DCI database gets shuffled.  This is mostly used during
/// the development phase for when the NV memory is re-arranged.
// const uint8_t DCI_NV_FACTORY_RESET_KEY = 0x57;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

// External prototypes for project specific functions.  Decided by which file is compiled.
extern void PROD_SPEC_Target_RAM_Init( void );		///< Should be done first.

extern void PROD_SPEC_Target_Micro_Init( void );	///< Should be done second.

extern void PROD_SPEC_Target_OS_Init( void );		///< Should be third.  To initialize the OS.

extern void PROD_SPEC_Target_Main_Init( void );		///< Should be done before we kick off the global

// interrupts and the OS.  We assume the DCI is up at
// this point.

extern void PROD_SPEC_Target_Start_OS( void );		///< Should happen right after we enable the interrupts

// and we don't plan on returning.

extern void PROD_SPEC_MEM_Init( void );			// Initializes the NV interfaces.

extern void PROD_SPEC_Target_NV_Init( void );		///< Initializes the NV interfaces.

extern void PROD_SPEC_Factory_Reset_Or_No_FRAM( void );	///< Should be done after

// PROD_SPEC_Target_NV_Init and before
// application start.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_RAM_Init( void )
{
	PROD_SPEC_Target_RAM_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_Micro_Init( void )
{
	PROD_SPEC_Target_Micro_Init();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_OS_Init( void )
{
	PROD_SPEC_Target_OS_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_Main_Init( void )
{
	DCI::Init();

	PROD_SPEC_MEM_Init();

	/*
	   Update the toolkit revision parameters. This call must not be modified.
	 */
	// coverity[leaked_storage]
	new Toolkit_Revision();

	PROD_SPEC_Target_Main_Init();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_Start_OS( void )
{
	PROD_SPEC_Target_Start_OS();
}
