/**
 *****************************************************************************************
 * @file		Test_IPCC.h
 *
 * @brief		Contains the test application function prototype for IPCC API's.
 *				IPC-Inter-Processor Communication Control.
 *				This tests communication between two cores by calling Send() available
 *				in uC_IPCC. First test by calling Blocking call followed by normal call.
 *				Communication is verified by checking shared data is modified or not.
 *				For non-blocking call, calling task should get executed first followed by
 *				shared data updated.
 *				For blocking call, calling task should be blocked till shared data
 *				gets updated.
 *
 * @copyright  	2021 Eaton Corporation. All Rights Reserved.
 *****************************************************************************************
 */

#ifndef TEST_IPCC_H_
#define TEST_IPCC_H_

/**
 * @brief  : Inter-Processor call test
 * @details: This function test blocking and non blocking IPC calls.
 * @return void.
 */
void Test_IPCC_App_Main( void );

#endif	/* TEST_IPCC_H_ */
