@example  External_Interrupt
@brief    The sample code to use the External Interrupt Class available in the Babelfish
			library.
 
@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run the sample code successfully.
@n        1. STM32F407 Card is fitted on the board.
 
@par Pin Configurations 
@details Make sure that LED switch is ON for PORTD/L
 
@par Dependency
@details Not applicable
 
@par Sample Code Listing
 
~~~~{.cpp}
 
#include "uC_Ext_Int.h"
#include "LED.h"

/**
 *****************************************************************************************
 * @Variables
 *****************************************************************************************
 */
static BF_Misc::LED *int_led_0;
static BF_Misc::LED *int_led_1;
static BF_Misc::LED *int_led_2;
uC_Ext_Int *Push_Button_0;
uC_Ext_Int *Push_Button_1;
uC_Ext_Int *Push_Button_2;

/**
 *****************************************************************************************
 * @Function Definitions
 *****************************************************************************************
 */
void Ext_Int_Example( void );
void LD_Push_Button0_ExtInt( void );
void LD_Push_Button1_ExtInt( void );
void LD_Push_Button2_ExtInt( void );

/**
 **********************************************************************************************
 * @brief Provides an example for the use of external interrupt.
 * @details This function will attach and initialize the interrupt at multiple buttons.
 * It will also attach multiple pins with LEDs which will serve as the output for the
 * interrupts.
 * 1. The buttons attached at the pin 4, 5 and 6 of port A will act as the input for
 * external interrupt.
 * 2. The LEDs attached at the pin 0, 1 and 2 of port D will act as the output for respective
 * interrupt pins.
 **********************************************************************************************
 */
void Ext_Int_Example( void )
{
	/* 
	 * Push Button x: Input for the external interrupt
	 * uC_Ext_Int(): This constructor will set passed argument pin as the external interrupt pin.
	 * Here we are making pin 4, 5 and 6 of port A as the input pins for external interrupt.
	 */
	Push_Button_0 = new uC_Ext_Int( &PIOA_PIN_4_STRUCT );
	Push_Button_1 = new uC_Ext_Int( &PIOA_PIN_5_STRUCT );
	Push_Button_2 = new uC_Ext_Int( &PIOA_PIN_6_STRUCT );

	/* 
	 * Attach_IRQ(): This method does the following things
	 * 1. LD_Push_Button0_ExtInt: Attaches vector callback.
	 * 2. EXT_INT_RISING_EDGE_TRIG: Defines triggering method(rising edge, falling edge and level trigger).
	 * 3. uC_IO_IN_CFG_HIZ: I/O configuration of the pin(high impedance, pull up, pull down)
	 * 4. uC_INT_HW_PRIORITY_15: Set priority level(0-15), 0 being highest and 15 being lowest priority.
	 */
	Push_Button_0->Attach_IRQ( LD_Push_Button0_ExtInt, EXT_INT_RISING_EDGE_TRIG,
							  uC_IO_IN_CFG_HIZ, uC_INT_HW_PRIORITY_15 );
	Push_Button_1->Attach_IRQ( LD_Push_Button1_ExtInt, EXT_INT_RISING_EDGE_TRIG,
							  uC_IO_IN_CFG_HIZ, uC_INT_HW_PRIORITY_15 );
	Push_Button_2->Attach_IRQ( LD_Push_Button2_ExtInt, EXT_INT_RISING_EDGE_TRIG,
							  uC_IO_IN_CFG_HIZ, uC_INT_HW_PRIORITY_1 );

	/* 
	 * Enable_Int(): This method will enable the interrupt encapsulated by the object.
	 */
	Push_Button_0->Enable_Int();
	Push_Button_1->Enable_Int();
	Push_Button_2->Enable_Int();
	
	/* 
	 * Assigning pin 0, 1 and 2 of port D as output and attaching it to the LED object.
	 */
	int_led_0 = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOD_PIN_0_STRUCT ) );
	int_led_1 = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOD_PIN_1_STRUCT ) );
	int_led_2 = new BF_Misc::LED( new BF_Lib::Output_Pos( &PIOD_PIN_2_STRUCT ) );
}

/**
 **********************************************************************************************
 * @brief An interrupt sub-routine function.
 * @details This function will toggle the LED and clears the interrupt pending request bit 
 * when interrupt at pin PA4(Push_Button_0) occurs.
 **********************************************************************************************
 */
void LD_Push_Button0_ExtInt( void )
{
	int_led_0->Toggle();	///< Toggles the LED.
	Push_Button_0->Clear_Int();	///< Clears the interrupt pending request bit.
}

/**
 **********************************************************************************************
 * @brief An interrupt sub-routine function.
 * @details This function will toggle the LED and clears the interrupt pending request bit 
 * when interrupt at pin PA5(Push_Button_1) occurs.
 **********************************************************************************************
 */
void LD_Push_Button1_ExtInt( void )
{
	int_led_1->Toggle();
	Push_Button_1->Clear_Int();
}

/**
 **********************************************************************************************
 * @brief An interrupt sub-routine function.
 * @details This function will toggle the LED and clears the interrupt pending request bit 
 * when interrupt at pin PA6(Push_Button_2) occurs.
 **********************************************************************************************
 */
void LD_Push_Button2_ExtInt( void )
{
	int_led_2->Toggle();
	Push_Button_2->Clear_Int();
}
~~~~