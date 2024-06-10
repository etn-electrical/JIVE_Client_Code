/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Input.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants / Macros
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uC_BASE_PIO_CTRL_STRUCT const* uC_Input::m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS];

uC_Input::INPUT_WRD_TD uC_Input::m_deb_array[uC_BASE_MAX_PIO_CTRLS][UC_INPUT_DEBOUNCE_ARRAY_COUNT];
uC_Input::INPUT_WRD_TD uC_Input::m_good_input[uC_BASE_MAX_PIO_CTRLS];

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Input::uC_Input( void )
{
	for ( uint_fast8_t i = 0U; i < static_cast<uint_fast8_t>( uC_BASE_MAX_PIO_CTRLS ); i++ )
	{
		m_pio_ctrls[i] = ( uC_Base::Self() )->Get_PIO_Ctrl( i );
		BF_ASSERT( m_pio_ctrls[i] != 0U );
	}

	for ( uint_fast8_t i = 0U; i < static_cast<uint_fast8_t>( UC_INPUT_DEBOUNCE_ARRAY_COUNT ); i++ )
	{
		Debounce_Pins( reinterpret_cast<CR_Tasker*>( nullptr ),
					   reinterpret_cast<CR_TASKER_PARAM>( nullptr ) );
	}

	new CR_Tasker( &Debounce_Pins, reinterpret_cast<CR_TASKER_PARAM>( nullptr ),
				   CR_TASKER_IDLE_PRIORITY, "uC_Input Debounce" );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Input::Debounce_Pins( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
#ifdef UC_INPUT_XOR_DEBOUNCE
	static uint8_t sample_index = UC_INPUT_DEBOUNCE_ARRAY_COUNT;
	INPUT_WRD_TD change_state;
	INPUT_WRD_TD* working_array;
	INPUT_WRD_TD current_state;

	sample_index++;
	if ( sample_index >= UC_INPUT_DEBOUNCE_ARRAY_COUNT )
	{
		sample_index = 0U;
	}

	for ( uint_fast8_t j = 0U; j < static_cast<uint_fast8_t>( uC_BASE_MAX_PIO_CTRLS ); j++ )
	{
		working_array = m_deb_array[j];
		working_array[sample_index] = m_pio_ctrls[j]->reg_ctrl->IDR;

		current_state = m_good_input[j];

		change_state = ~0x0U;
		for ( uint_fast8_t i = 0U;
			  i < static_cast<uint_fast8_t>( UC_INPUT_DEBOUNCE_ARRAY_COUNT );
			  i++ )
		{
			change_state &= ( working_array[i] ^ current_state );
		}
		m_good_input[j] ^= change_state;
	}
#else
	INPUT_WRD_TD* working_array;
	INPUT_WRD_TD working_value;

	for ( uint_fast8_t i = 0U; i < static_cast<uint_fast8_t>( uC_BASE_MAX_PIO_CTRLS ); i++ )
	{
		working_array = m_deb_array[i];

		working_array[3] = working_array[2];
		working_array[2] = working_array[1];
		working_array[1] = working_array[0];

		working_array[0] = static_cast<uint16_t>( m_pio_ctrls[i]->reg_ctrl->IDR );

		working_value = m_good_input[i] & ( working_array[0] |
											( working_array[2] & working_array[3] ) |
											( working_array[1] &
											  ( working_array[2] | working_array[3] ) ) );
		working_value = working_value | ( working_array[0] &
										  ( working_array[1] | working_array[2] ) &
										  working_array[3] );
		m_good_input[i] = working_value |
			( working_array[0] & working_array[1] & working_array[2] );
	}
#endif
	if ( param == reinterpret_cast<CR_TASKER_PARAM>( nullptr ) )
	{}
	if ( cr_task == reinterpret_cast<CR_Tasker*>( nullptr ) )
	{}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Input::Debounce_Quick_Update( uC_USER_IO_STRUCT const* pio_ctrl )
{
#ifdef UC_INPUT_XOR_DEBOUNCE
	INPUT_WRD_TD change_state;
	INPUT_WRD_TD* working_array;
	INPUT_WRD_TD current_state;

	working_array = m_deb_array[pio_ctrl->port_index];
	for ( uint_fast8_t j = 0U; j < UC_INPUT_DEBOUNCE_ARRAY_COUNT; j++ )
	{
		working_array[j] = m_pio_ctrls[pio_ctrl->port_index]->reg_ctrl->IDR;

		current_state = m_good_input[pio_ctrl->port_index];

		change_state = ~0x0U;
		for ( uint_fast8_t i = 0U; i < UC_INPUT_DEBOUNCE_ARRAY_COUNT; i++ )
		{
			change_state &= ( working_array[i] ^ current_state );
		}
		m_good_input[pio_ctrl->port_index] ^= change_state;
	}
#else
	INPUT_WRD_TD* working_array;
	INPUT_WRD_TD working_value;

	working_array = m_deb_array[pio_ctrl->port_index];

	for ( uint_fast8_t i = 0U; i < static_cast<uint_fast8_t>( UC_INPUT_DEBOUNCE_ARRAY_COUNT ); i++ )
	{
		working_array[3] = working_array[2];
		working_array[2] = working_array[1];
		working_array[1] = working_array[0];

		working_array[0] =
			static_cast<uint16_t>( m_pio_ctrls[pio_ctrl->port_index]->reg_ctrl->IDR );

		working_value = m_good_input[pio_ctrl->port_index] & ( working_array[0] |
															   ( working_array[2] &
																 working_array[3] ) |
															   ( working_array[1] &
																 ( working_array[2] |
																   working_array[3] ) ) );
		working_value = working_value | ( working_array[0] &
										  ( working_array[1] | working_array[2] ) &
										  working_array[3] );
		m_good_input[pio_ctrl->port_index] = working_value |
			( working_array[0] & working_array[1] & working_array[2] );
	}
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Input::Debounce_Quick_Update( void )
{
	for ( uint_fast8_t i = 0U; i < static_cast<uint_fast8_t>( UC_INPUT_DEBOUNCE_ARRAY_COUNT ); i++ )
	{
		Debounce_Pins( reinterpret_cast<CR_Tasker*>( nullptr ),
					   reinterpret_cast<CR_TASKER_PARAM>( nullptr ) );
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************

   BOOL uC_Input::Get_Input( uC_USER_IO_STRUCT const* pio_ctrl )
   {
   return ( false );
   }
 */
