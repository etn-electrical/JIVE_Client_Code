/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef EIP_DISPLAY_H
   #define EIP_DISPLAY_H

#include "LED.h"
#include "Timers_Lib.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class EIP_Display
{
	public:
		EIP_Display( BF_Misc::LED* grn_mod_led_ctrl, BF_Misc::LED* red_mod_led_ctrl,
					 BF_Misc::LED* grn_net_led_ctrl = NULL,
					 BF_Misc::LED* red_net_led_ctrl = NULL );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~EIP_Display( void ){}

		// Module Status
		void MS_Operational( void );

		void MS_Standby( void );

		void MS_Minor_Fault( void );

		void MS_Unrecoverable_Fault( void );

		void MS_Self_Test( void );

		// Network Status
		void NS_Offline( void );

		void NS_Online_Disconnected( void );

		void NS_Online_Connected( void );

		void NS_Connection_Timeout( void );

		void NS_Critical_Link_Failure( void );

		void NS_Comm_Faulted_With_Identify( void );

		void LED_Test( void );

	private:
		void Flash_Task( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Flash_Task_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
		{ ( ( EIP_Display* )param )->Flash_Task(); }

		uint16_t Update_Display( BF_Misc::LED* green, BF_Misc::LED* red, uint8_t& state, uint8_t prev_state )const;

		void Combined_Flash_Task( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Combined_Flash_Task_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
		{ ( ( EIP_Display* )param )->Combined_Flash_Task(); }

		void LED_Test_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void LED_Test_Handler_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
		{ ( ( EIP_Display* )param )->LED_Test_Handler(); }

		void Reset_Display( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Toggle( BF_Misc::LED* led )
		{
			if ( led->Is_On() )
			{
				led->Off();
			}
			else
			{
				led->On();
			}
		}

		BF_Misc::LED* m_grn_mod_led_ctrl;
		BF_Misc::LED* m_red_mod_led_ctrl;

		BF_Misc::LED* m_grn_net_led_ctrl;
		BF_Misc::LED* m_red_net_led_ctrl;

		uint8_t m_ms_state;
		uint8_t m_prev_ms_state;
		uint8_t m_ns_state;
		uint8_t m_prev_ns_state;
		uint8_t m_cs_state;
		uint8_t m_prev_cs_state;

		uint16_t m_ms_tick;
		uint16_t m_ns_tick;

		uint16_t m_ms_identify_ctr;
		uint16_t m_ns_identify_ctr;

		uint8_t m_led_test_state;

		BF_Lib::Timers* m_timer;

		// Combined Module/Network
		// void	Operational_Online_Connected( void );
		// void	Operational_Online_NotConnected( void );
		// void 	Minor_Fault_Connection_Timeout( void );
		// void	Critical_Fault_Or_Link_Failure( void );
		// void 	Comm_Faulted_With_Identify_Combined( void );
};



#endif
