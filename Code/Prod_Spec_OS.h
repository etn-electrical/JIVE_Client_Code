/**
 *****************************************************************************************
 * @file
 *
 * @brief
 * @details : this file contains declaration and Macros related to OS
 *
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_OS_H

#define PROD_SPEC_OS_H

/*
 *****************************************************************************************
 *		Function Prototypes
 *****************************************************************************************
 */
void PROD_SPEC_Target_Start_OS( void );

void PROD_SPEC_Target_OS_Init( void );

void PROD_SPEC_OS_Cpu_Usage_Monitor( void );

void PROD_SPEC_OS_Oscr_Dog_Init( void );

#endif
