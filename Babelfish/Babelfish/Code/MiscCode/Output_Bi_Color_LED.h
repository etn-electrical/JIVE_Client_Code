/**
 *****************************************************************************************
 *	@file
 *
 *	@brief It is used to set out the output status negatively. While we say the port pin
 *	status on, it clears the state of connected output.
 *
 *	@details This module provides the port pin output control and is called output negative
 *	as we resets the port pin opposite as the input status in the appropriate conditions.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OUTPUT_BI_COLOR_LED_H
#define OUTPUT_BI_COLOR_LED_H

#include "Output.h"
#include "Bi_Color_LED.h"

namespace BF_Misc
{
/**
 ****************************************************************************************
 * @brief This class will accommodate the instance where an LED control wants a single output
 * control but we have LEDs.
 * @details The class will allow the ability to select which LED is active at any single time
 * but the LED class will not be aware of this change.
 ****************************************************************************************
 */
class Output_Bi_Color_LED : public BF_Lib::Output
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Performs initialization related to the output control, sets the particular
		 *  pin to output.
		 *  @param[in] output_ctrl: The user output control.
		 *  @param[in] initial_state: The state of the pin to be either set or reset, initially
		 * zero.
		 *  @return None
		 */
		Output_Bi_Color_LED( Bi_Color_LED* bi_color,
							 Bi_Color_LED::bi_color_state_t initial_color =
							 Bi_Color_LED::COLOR_ONE ) :
			m_bi_color( bi_color ),
			m_present_color( initial_color ),
			m_is_on( false )
		{}

		/**
		 *  @brief Destructor
		 *  @details
		 *  @return None
		 */
		~Output_Bi_Color_LED( void )
		{
			m_bi_color = reinterpret_cast<Bi_Color_LED*>( nullptr );
		}

		/**
		 *  @brief
		 *  @details This turns off the active LED.
		 *  @return None
		 */
		inline void On( void )
		{
			m_is_on = true;
			m_bi_color->On( m_present_color );
		}

		/**
		 *  @brief
		 *  @details This sets the port pin status.
		 *  @return None
		 */
		inline void Off( void )
		{
			m_is_on = false;
			m_bi_color->Off();
		}

		/**
		 *  @brief
		 *  @details This is to clear or set the particular port pin.
		 *  @param[in] state: This is written into to clear or set the state of port pin.
		 *  true shall clear the port pin status.
		 *  @return None
		 */
		inline void Set_To( bool state )
		{
			if ( state != false )
			{
				m_bi_color->On( m_present_color );
			}
			else
			{
				m_bi_color->Off();
			}
		}

		/**
		 *  @brief
		 *  @details This is to know the state of the particular port pin.
		 *  @return This tells the state of the port pin.
		 */
		inline bool State( void )
		{
			return ( m_is_on );
		}

		/**
		 *  @brief Changes the color which is active.
		 *  @details This is to know the state of the particular port pin.
		 */
		inline void Set_Color( Bi_Color_LED::bi_color_state_t color )
		{
			m_present_color = color;
		}

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Output_Bi_Color_LED( const Output_Bi_Color_LED& rhs );
		Output_Bi_Color_LED & operator =( const Output_Bi_Color_LED& object );

		Bi_Color_LED* m_bi_color;
		Bi_Color_LED::bi_color_state_t m_present_color;
		bool m_is_on;
};

}

#endif	/* OUTPUT_NEG_H */
