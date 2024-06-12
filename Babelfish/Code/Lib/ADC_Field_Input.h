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
#ifndef ADC_FIELD_INPUT_H
   #define ADC_FIELD_INPUT_H

#include "AtoD.h"
#include "DCI_Owner.h"
#include "Input.h"
#include "Device_Identity.h"

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
class ADC_Field_Input : public Input
{
	public:
		ADC_Field_Input( AtoD* atod_ctrl, DEVICE_IDENT_IO_ENUM input_type,
						 BF_Lib::Timers::TIMERS_TIME_TD refresh_rate = 0 );			// Expects a 16bit atod control.

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~ADC_Field_Input() {}


		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Get( void )
		{ return ( m_input_status ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool Get_Raw( void )
		{ return ( m_input_status ); }

	private:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Refresh_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{ ( ( ADC_Field_Input* )handle )->Refresh(); }

		uint16_t Get_Voltage( void );			// Returned in milliVolts

		void Refresh( void );

		AtoD* m_atod_ctrl;
		uint32_t m_raw_input;
		uint16_t m_on_threshold;
		uint16_t m_off_threshold;
		bool m_input_status;
};



#endif
