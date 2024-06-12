/**
 *****************************************************************************************
 *	@file
 *
 *	@brief It is used to read the input status negatively. The particular port pin input
 *	is non-zero while we get the status as false.
 *
 *	@details This contains the Input_Pos class and it is used for the read the value of
 *	the port pin as it is. i.e. if the input port pin status is at logic 1 we get 'false'
 *	as the return value and the vice versa.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef INPUT_NEG_H
#define INPUT_NEG_H

#include "Input.h"
#include "uC_Input.h"
#include "uC_IO_Config.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is a Input_Neg class and it is used for the read the value of the port pin
 * and give the value opposite to the actual and hence called Input_Neg.
 * @details It does the initialization related to the input negative control and make
 * the status and the value of the input available.
 * @n @b Usage:
 * It provides public methods to
 * @n @b 1. Know the status of the port pin to be zero or non-zero.
 * @n @b 2. Read the actual value of the input.
 ****************************************************************************************
 */
class Input_Neg : public Input
{
	public:
		/**
		 *  @brief Constructor
		 *  @details Performs initialization related to the input control, sets the particular
		 *  pin to input and asks for the debounce logic to get executed.
		 *  @param[in] pio_ctrl: The user Port i/o control structure.
		 *  @param[in] input_config: uC i/o input configuration. The state of the pin to be
		 *  either pull up or pull down, initially kept at the high impedance state.
		 *  @return None
		 */
		Input_Neg( uC_USER_IO_STRUCT const* pio_ctrl, uC_IO_INPUT_CFG_EN input_config =
				   uC_IO_IN_CFG_HIZ )
		{
			m_pio_ctrl = pio_ctrl;
			uC_IO_Config::Set_Pin_To_Input( m_pio_ctrl, input_config );

			uC_Input::Debounce_Quick_Update( m_pio_ctrl );
		}

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Input_Neg goes out of scope or deleted.
		 *  @return None
		 */
		~Input_Neg()
		{}

		/**
		 *  @brief
		 *  @details This tests the port pin input is it is non-zero or not.
		 *  @return The status if the input at the port bin is non-zero or zero.
		 *  @retval false The particular port pin input is non-zero.
		 *  @retval true The particular port pin input is zero.
		 */
		inline bool Get( void )
		{
			return ( !uC_Input::Get_Input( m_pio_ctrl ) );
		}

		/**
		 *  @brief
		 *  @details This tests the port pin input is it is non-zero or not.
		 *  @return The status if the input at the port bin is non-zero or zero.
		 *  @retval false The particular port pin input is non-zero.
		 *  @retval true The particular port pin input is zero.
		 */
		inline bool Get_Raw( void )
		{
			return ( !uC_IO_Config::Get_In( m_pio_ctrl ) );
		}

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */

		Input_Neg( const Input_Neg& rhs );
		Input_Neg & operator =( const Input_Neg& object );

		uC_USER_IO_STRUCT const* m_pio_ctrl;
};

}
#endif	/* INPUT_NEG_H */
