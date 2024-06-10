/**
 *****************************************************************************************
 *	@file		Test_App_multiple_HAL.h
 *
 *	@brief		The file contains test Application for testing multiple HAL interfaces.
 *	@n          Supporting functionalities listed with Test_App_Multiple_HAL_Main()
 *****************************************************************************************
 */

#ifndef TEST_HAL_MULTIPLE
#define TEST_HAL_MULTIPLE

/**
 * @brief		Main Function to test multiple HAL interfaces. This test containing below test cases together
 * @n			Configure IO  : Configure Default IO ports using uC_Base_HW_IDs_ESP32.cpp file.
 * @n			Configure DCI : DCI main is called before using this interface call
 * @n			Read Some Value from DCI Flash and Place into DCI RAM
 * @n			Write Some value from DCI RAM to DCI Flash
 * @n			Create function that Reads ADC and Sends Via Can
 * @n			Create function that reads CAN and sets PWM duty cycle
 * @n			Create function that Blinks LEDS
 * @n			Setup WatchDog (Already initializing with DCI main task)
 * @return Void		None
 */
void Test_App_Multiple_HAL_Main();

/**
 * @brief			Function to test basic DCI owner Check IN & OUT Data variables functions
 * @n               This will make sure the DCI RAM interface is working correctly with below tests
 * @n				DCI_CheckIn_CheckOut_Data_Variables_Test();
 * @n				DCI_Test_Owner_Attributes();
 * @n				DCI_owner_Test_Lock_Unlock();
 * @n				DCI_Patron_access_test();
 * @return Void		None */

void DCI_Basic_Test_cases( void );

/**
 * @brief			Function to test DCI read value from flash interface & Write it to RAM
 * @n				Also it will test a value read from RAm & write to Flash as well.
 * @return Void		None */


void DCI_Read_from_Flash_write_to_RAM_vice_versa_Test( void );

/**
 * @brief			Function to configure ADC & CAN .It will read ADC value from GPIO pin &
 * @n				send as CAN transmit message which can be captured in PCAN view or similar methods
 * @n				There is ADC_WAIT_COUNT variable which can be set for the ADC value looping time
 * @return Void		None */

void Read_ADC_Send_to_CAN_Test( void );

/**
 * @brief			Function to send ADC value as CAN transmit message which can be captured in
 * @n				PCAN view or similar methods
 * @param[in]       voltage from ADC
 * @return          None */

void Send_ADC_Voltage_Value_to_CAN( uint32_t voltage_adc );

/**
 * @brief			Function to Read Duty cycle value form CAN Message (PCAN) & Configure
 * @n               it to generate PWM signal.
 * @return          None */

void Read_PCAN_Value_Set_PWM_Duty_cycle();

/**
 * @brief			Function to blink LED using GPIO toggling methord. Here Wrover kit RGB LED's are used
 * @n				for display the output.Dev kit board don't have the same so needs to connect LED
 * @n				with BLINK_GPIO pin to see the output.
 * @return          None */

void Blink_LED( void );


#endif	// TEST_HAL_MULTIPLE
