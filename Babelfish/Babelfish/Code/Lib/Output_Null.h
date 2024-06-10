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
#ifndef OUTPUT_NULL_H
#define OUTPUT_NULL_H

#include "Output.h"

namespace BF_Lib
{

/**
 ****************************************************************************************
 * @brief This is a Output_Null class and it is used to simulate an actual output.
 * @details This module is for when you don't want to dedicate an IO pin and just want to
 * simulate an output.
 ****************************************************************************************
 */
class Output_Null : public BF_Lib::Output
{
	public:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		Output_Null( bool initial_state = BF_Lib::Output::OUTPUT_STATE_OFF )
		{
			m_output_state = initial_state;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Output_Null() {}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void On( void )
		{
			m_output_state = true;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Off( void )
		{
			m_output_state = false;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_To( bool state )
		{
			if ( state != 0 )
			{
				m_output_state = true;
			}
			else
			{
				m_output_state = false;
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline bool State( void )
		{
			return ( m_output_state );
		}

	private:
		bool m_output_state;
};
}
#endif
