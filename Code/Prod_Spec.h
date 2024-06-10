/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_H
	#define PROD_SPEC_H

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */

void PROD_SPEC_RAM_Init( void );		// Should be first to initialize the RAM.

void PROD_SPEC_Micro_Init( void );		// Should be second to initialize the micro type (ie 107, 105 etc.)

void PROD_SPEC_OS_Init( void );			// Should be third.  To initialize the OS.

void PROD_SPEC_Main_Init( void );		// Should be right before we start the OS and enable globa interrupts.

void PROD_SPEC_Start_OS( void );		// Should be where we enter and never leave.

void PROD_SPEC_Main_Unit_Tests( void );	// Run the main product specific unit tests

#endif
