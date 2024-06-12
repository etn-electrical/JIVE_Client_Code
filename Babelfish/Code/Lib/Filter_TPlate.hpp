/**
 *****************************************************************************************
 *	@file
 *
 *	@brief A very basic and simple filter.  Specialized for unsigned integers.
 *
 *	@details This filter template should only be used with unsigned integers.  Any
 *	other type of value might very well explode on you unless the compiler can
 *	properly understand what the shift is supposed to do (not).
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FILTER_TPLATE_HPP
	#define FILTER_TPLATE_HPP

#include "Includes.h"
#include "Exception.h"

/**
 ****************************************************************************************
 *	@brief A very basic and simple filter.  Specialized for unsigned integers.
 *
 *	@details This filter template should only be used with unsigned integers.  Any
 *	other type of value might very well explode on you unless the compiler can
 *	properly understand what the shift is supposed to do.  It might work with
 *	signed but a float is right out.
 * 	Template Types:
 * 		- value_t: is the value which is to be used outside.
 * 		- sum_t: must be a datatype which is big enough to hold the multiple of the filter.
 * 			It is recommended to just use a datatype one step bigger than the value type.  For
 * 			example for a uint8 with a filter of 2 we need a sum val which is 8+2 bits (so uint16)
 *			However it should be noted that if you have an ADC value that will always be
 *			less than 12bits then it is possible to use a uint16 assuming your filter
 *			power is less than 4.
 *
 ****************************************************************************************
 */
template<typename value_t, typename sum_t>
class Filter_TPlate
{
	public:
		enum filter_power_t
		{
			FILTER_POWER_2,		// These are the divide levels.  So a 2 is a summer divide by 2.
			FILTER_POWER_4,		// The filters work slower the higher the divider.  A divide by
			FILTER_POWER_8,		// 4 is pretty average.  A divide by 8 is pretty slow.
			FILTER_POWER_16
		};

		/**
		 * @brief The initial value constructor allows you to initialize the filter.
		 * @param initial_value: The initial value to set.
		 * @param power: The power level to set for the filter.
		 * @param max_change: The maximum amount of change permitted on each update.  It is a plus or minus value.
		 * 		For example a max change of 100 will allow the value to change +100 or -100 and that is it.
		 */
		Filter_TPlate( value_t initial_value, filter_power_t power = FILTER_POWER_4, value_t max_change = 0 )
		{
			m_max_change = max_change;

			m_power = static_cast<uint8_t>( power ) + 1;

			Init( initial_value );
		}

		/**
		 * @brief This constructor allows you to create the filter but it won't be initialized.
		 * This is normally ok as long as you update it at least once.  If you just try to read it immediately
		 * YOU WILL FAIL.
		 * @param power: See above.
		 * @param max_change: See above.
		 */
		Filter_TPlate( filter_power_t power = FILTER_POWER_4, value_t max_change = 0 )
		{
			m_max_change = max_change;

			m_power = static_cast<uint8_t>( power ) + 1;
			m_power |= FILTER_UNINITIALIZED_MASK;

			m_summer = 0U;
			m_value = 0U;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Filter_TPlate( void ) {}

		/**
		 * @brief A quick way of getting the current value.
		 * @return The current filtered value.  If the value is uninitialized
		 * then a value of zero will be returned.
		 */
		value_t Value( void ) { return ( m_value ); }

		/**
		 * @brief Will both update the filter AND return the new value.  If you have not initialized the filter
		 * yet I will initialize it here for you.
		 * @param new_value: The new value to put into the filter.
		 * @return The current filtered value.
		 */
		value_t Value( value_t new_value )
		{
			if ( ( m_power & FILTER_UNINITIALIZED_MASK ) != 0 )
			{
				Init( new_value );
			}

			if ( new_value != m_value )
			{
				if ( m_max_change != 0 )
				{
					if ( ( new_value > m_value ) &&
						 ( ( new_value - m_value ) > m_max_change ) )
					{
						new_value = m_value + m_max_change;
					}
					else if ( ( m_value - new_value ) > m_max_change )
					{
						new_value = m_value - m_max_change;
					}
				}

				m_summer += new_value;
				m_summer -= m_value;

				m_value = Round_Shift_Div( m_summer, m_power );
			}

			return ( m_value );
		}

		/**
		 * @brief Sets the initial value.  In cases where you want to essentially reset the filter.
		 * @param initial_value: The new value to be used.
		 */
		void Init( value_t initial_value )
		{
			if ( sizeof( sum_t ) < sizeof( value_t ) )
			{
				// You chose your datatypes poorly.  The summer datatype must be
				// at least big enough to handle the multiple of the power.

				/* Can not use BF_ASSERT() call here because loop is getting form
				   because of file inclusion. BF_ASSERT() uses CR tasker and CR_Tasker
				   uses Filter_TPlate file.
				 */
				__asm( "BKPT #0\n" );
				while ( 1 )
				{}
			}

			m_power &= ~FILTER_UNINITIALIZED_MASK;
			m_value = initial_value;
			m_summer = initial_value;
			m_summer = m_summer << m_power;
		}

		/**
		 * @brief Reinitializes the filter to start over again.  This will mean the next sample
		 * will actually define the filter starting point.
		 */
		void Re_Init( void )
		{
			m_power |= FILTER_UNINITIALIZED_MASK;

			m_summer = 0U;
			m_value = 0U;
		}

	private:
		static const uint8_t FILTER_UNINITIALIZED_MASK = ( 0x80U );

		sum_t m_summer;
		uint8_t m_power;
		value_t m_value;
		value_t m_max_change;
};


#endif
