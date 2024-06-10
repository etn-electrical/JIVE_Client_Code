/**
 *****************************************************************************************
 *	@file
 *
 *	@brief It is used to set out the output status positively. While we say the port pin
 *	status on, it sets the connected output.
 *
 *	@details This module provides the port pin output control and is called output positive
 *	as we sets the port pin same as the input status in the appropriate conditions.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OUTPUT_POS_H
#define OUTPUT_POS_H

#include "Output.h"
#include "uC_IO_Config.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is a Output_Neg class and it is used to write the port pin.
 * @details While we say the port pin status on, it sets the state of connected
 * output and hence called Output_Pos.
 * @n @b Usage:
 * It provides public methods to
 * @n @b 1. To set or reset the port pin and hence the output connected.
 * @n @b 2. To write the status of the port pin.
 ****************************************************************************************
 */
class Output_Pos : public BF_Lib::Output
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Performs initialization related to the output control, sets the particular
		 *  pin to output.
		 *  @param[in] output_ctrl: The user output control.
		 *  @param[in] initial_state: The state of the pin to be either set or reset, initially zero.
		 *  @return None
		 */
		Output_Pos( uC_USER_IO_STRUCT const* output_ctrl, bool initial_state = OUTPUT_STATE_OFF )
		{
			m_output_ctrl = output_ctrl;
			uC_IO_Config::Set_Pin_To_Output( output_ctrl, initial_state );
		}

		/**
		 *  @brief Destructor
		 *  @details It does nothing.
		 *  @return None
		 */
		~Output_Pos()   {}

		/**
		 *  @brief
		 *  @details This sets the port pin status.
		 *  @return None
		 */
		inline void On( void )
		{
			uC_IO_Config::Set_Out( m_output_ctrl );
		}

		/**
		 *  @brief
		 *  @details This clears the port pin status.
		 *  @return None
		 */
		inline void Off( void )
		{
			uC_IO_Config::Clr_Out( m_output_ctrl );
		}

		/**
		 *  @brief
		 *  @details This is to clear or set the particular port pin.
		 *  @param[in] state: This is written into to clear or set the state of port pin.
		 *  false shall clear the port pin status.
		 *  @return None
		 */
		inline void Set_To( bool state )
		{
			if ( state != false )
			{
				uC_IO_Config::Set_Out( m_output_ctrl );
			}
			else
			{
				uC_IO_Config::Clr_Out( m_output_ctrl );
			}
		}

		/**
		 *  @brief
		 *  @details This is to know the state of the particular port pin.
		 *  @return This tells the state of the port pin.
		 */
		inline bool State( void )
		{
			return ( uC_IO_Config::Get_Out( m_output_ctrl ) );
		}

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Output_Pos( const Output_Pos& rhs );
		Output_Pos & operator =( const Output_Pos& object );

		uC_USER_IO_STRUCT const* m_output_ctrl;
};

}

#endif	/* OUTPUT_POS_H */
