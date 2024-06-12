/**
 **********************************************************************************************
 * @file            Fault Catcher base class for fault information capturing and printing
 *
 * @brief           Fault Catch shall capture a fault/fail scenario from certain sources and log
 * 					them and give a away for later retrieval.
 *					This requires some hooks be placed in the fault areas.
 *
 * @details			 - This file contains Fault Catcher base class and pure virtual
 * 					functions.
 * 					- Other Fault catcher class will be derived from this base class
 *					Based on the requirements Fault catch implementation can be done on derived
 *					class
 *
 * @copyright       2022 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef FAULT_CATCHER_H
#define FAULT_CATCHER_H

#include "Etn_Types.h"
#include "Debug_Interface.h"
/**
 **************************************************************************************************
 * @brief  : This class is responsible for capturing and printing fault messages
 *
 * @details: Different type of faults (Hard fault,watchdog fault etc,,,) captured and printed on
 * 			 selected debug print interface
 **************************************************************************************************
 */
class Fault_Catcher
{
	public:
		/**
		 * @brief : Function to print Hard fault capture data.
		 * @details: This function get hard fault data from DCI variables and will print
		 * 			 the same on Debug interface.
		 * @return void.
		 */
		virtual void Print_Hard_Fault_Data( Debug_Interface* dm_base_ptr ) = 0;

		/**
		 * @brief: Information related to tasks when exception occurs is captured in this function
		 * @details: This is called from the HardFault_HandlerAsm with a pointer the Fault stack
		 *			 as the parameter. We can then read the values from the stack and place them
		 *			 into local variables for ease of reading.
		 *			 We then read the various Fault Status and Address Registers to help decode
		 *			 cause of the fault.
		 * @param[in] hardfault_args: hard fault information captured from hardfault registers
		 * @return void.
		 */
		virtual void Hardfault_Exception_Capture( ULONG* hardfault_args ) = 0;

};

#endif	// #ifndef FAULT_CATCHER_H