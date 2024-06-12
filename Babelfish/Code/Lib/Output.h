/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module provides the port pin output control in the positive way (Output_Pos)
 *	and in the opposite way (Output_Neg)
 *
 *	@details This contains the functionality to work with the connected output, to change
 *	the state, read, and use the status for further functionality.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef OUTPUT_H
#define OUTPUT_H

#include "uC_IO_Config.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is a Output class and it is used to read and write the port pin.
 * @details While we say the port pin status on, it sets the state of connected output
 * (Output_Pos) and While we say the port pin status on, it clears the state of connected
 * output (Output_Neg).
 * @n @b Usage:
 * It provides public methods that are inherited in Output_Neg and Output_Pos to:
 * @n @b 1. To set or reset the port pin and hence the output connected.
 * @n @b 2. To write the status of the port pin.
 ****************************************************************************************
 */
class Output
{
	public:

		/// This is used to set the the output state to logic zero.
		static const uint8_t OUTPUT_STATE_OFF = 0U;
		/// This is used to set the the output state to logic one.
		static const uint8_t OUTPUT_STATE_ON = 1U;

		/**
		 *  @brief Constructor
		 *  @details It does not do anything here.
		 *  @return None
		 */
		Output( void )
		{}

		/**
		 *  @brief Destructor
		 *  @details It does nothing here and is kept virtual as the Output_Neg and Output_Pos
		 *  are inherited from Output.
		 *  @return None
		 */
		virtual ~Output()
		{}

		/**
		 *  @brief All the virtual functions implemented in the Output_Pos and Output_Neg.
		 */
		virtual inline void On( void ) = 0;

		virtual inline void Off( void ) = 0;

		virtual inline void Set_To( bool state ) = 0;

		virtual inline bool State( void ) = 0;

	private:
		/**
		 *  Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Output( const Output& rhs );
		Output & operator =( const Output& object );

};

}

#endif	/* OUTPUT_H */
