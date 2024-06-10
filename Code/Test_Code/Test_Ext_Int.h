/**
 *****************************************************************************************
 * @file      Test_Ext_Int.cpp
 * @details   Contains the test application function prototype for external interrupt
 *            configured for gpio's.
 *            There are two gpio's configured as an interrupt. By default one is enabled.
 *            Second gpio configuration can be enabled by un-commenting "TWO_EXT_INTERRUPT"
 *            On pressing push button( boot button ) on board, interrupt will be triggered
 *            and log will be available on terminal.
 *
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TEST_EXT_INT_H_
#define TEST_EXT_INT_H_

/**
 ******************************************************************************************
 * @brief			: This function will attach and initialize the external interrupts
 * @return Void		: None
 ******************************************************************************************
 */
void Test_Ext_Int_App_Main();

#endif	// TEST_EXT_INT_H_
