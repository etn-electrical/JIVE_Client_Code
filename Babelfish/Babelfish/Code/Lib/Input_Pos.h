/**
 *****************************************************************************************
 *	@file
 *
 *	@brief It is used to read the input status positively. The particular port pin input
 *	is non-zero while we get the status as true.
 *
 *	@details This contains the Input_Pos class and it is used for the read the value of
 *	the port pin as it is. i.e. if the input port pin status is at logic 1 we get 'true'
 *	as the return value and the vice versa.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef INPUT_POS_H
#define INPUT_POS_H

#include "Input.h"
#include "uC_Input.h"
#include "uC_IO_Config.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is a Input_Pos class and it is used for the read the value of the port pin
 * and give the value same as the actual and hence called Input_Pos.
 * @details It does the initialization related to the input positive control and make
 * the status and the value of the input available.
 * @n @b Usage:
 * It provides public methods to
 * @n @b 1. Know the status of the port pin to be zero or non-zero.
 * @n @b 2. Read the actual value of the input.
 ****************************************************************************************
 */
class Input_Pos : public Input
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Performs initialization related to the input control, sets the particular
		 *  pin to input and asks for the debounce logic to get executed.
		 *  @param[in] pio_ctrl: The user input control structure.
		 *  @param[in] input_cfg: The state of the pin to be either pull up or pull down,
		 *  initially kept at the high impedance state.
		 *  @return None
		 */
		Input_Pos( uC_USER_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_cfg =
				   uC_IO_IN_CFG_HIZ )
		{
			m_pio_ctrl = pio_ctrl;
			uC_IO_Config::Set_Pin_To_Input( m_pio_ctrl, input_cfg );

			uC_Input::Debounce_Quick_Update( m_pio_ctrl );
		}

		/**
		 *  @brief Destructor
		 *  @details This gets called when the object of the Input_Pos goes out of scope.
		 *  @return None
		 */
		~Input_Pos()
		{}

		/**
		 *  @brief
		 *  @details This tests the port pin input is it is non-zero or not.
		 *  @return The status if the input at the port bin is non-zero or zero.
		 *  @retval true The particular port pin input is non-zero.
		 *  @retval false The particular port pin input is zero.
		 */
		inline bool Get( void )
		{
			return ( uC_Input::Get_Input( m_pio_ctrl ) );
		}

		/**
		 *  @brief
		 *  @details This reads the port pin actual value of input if it is one or zero.
		 *  @return The input logic at the port pin.
		 *  @retval true The particular port pin input is one.
		 *  @retval false The particular port pin input is zero.
		 */
		inline bool Get_Raw( void )
		{
			return ( uC_IO_Config::Get_In( m_pio_ctrl ) );
		}

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Input_Pos( const Input_Pos& rhs );
		Input_Pos & operator =( const Input_Pos& object );

		uC_USER_IO_STRUCT const* m_pio_ctrl;
};

}

#endif	/* INPUT_POS_H */
