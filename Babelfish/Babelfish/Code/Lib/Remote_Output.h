/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This is the Remote_Output class and helps write into the connected remote output.
 *
 *	@details This module is used to write into the remote output (LED)
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef REMOTE_OUTPUT_H_
#define REMOTE_OUTPUT_H_

#include "Includes.h"
#include "Output.h"

/**
 ****************************************************************************************
 * @brief This is the Remote_Output class and helps write into the connected remote output.
 * @details It is used to set, reset, basically write into the output state member.
 * @n @b Usage:
 * @n @b 1. It provides public methods to: set, reset, read or write into the output
 * state member (LED).
 ****************************************************************************************
 */
class Remote_Output : public BF_Lib::Output
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Initializes the output state data member.
		 */
		Remote_Output();

		/**
		 *  @brief Destructor for the Remote_Output Module.
		 *  @details This will get called when object of Remote_Output goes out of scope.
		 */
		virtual ~Remote_Output();

		/**
		 *  @brief
		 *  @details This sets the output state date member to logic 1 and hence sets the output.
		 *  @return None
		 */
		void On( void );

		/**
		 *  @brief
		 *  @details This sets the output state date member to logic 0 and hence resets the output.
		 *  @return None
		 */
		void Off( void );

		/**
		 *  @brief
		 *  @details This sets the output state date member to the assigned parameter.
		 *  param[in] state: Used to set or reset the output state.
		 *  @n @b Usage:
		 *  @n @b 1. True to set the LED as ON.
		 *  @n @b 2. False to reset the LED as OFF.
		 *  @return None
		 */
		void Set_To( bool state );

		/**
		 *  @brief
		 *  @details This sets returns the output state.
		 *  @return The status of LED.
		 *  @n @b Usage:
		 *  @n @b 1. True implies the LED is ON.
		 *  @n @b 2. False implies the LED is OFF.
		 */
		bool State( void );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Remote_Output( const Remote_Output& object );
		Remote_Output & operator =( const Remote_Output& object );

		bool m_state;
};

#endif	/* REMOTE_OUTPUT_H_ */
