/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains the Bit manipulator class which replaces a more brute force macro bit
 *	manipulation function.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BIT_H
#define BIT_H

#include "Includes.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief Provides basic bit set/clr/test functionality.
 *
 * @details This class shall provide basic bit manipulation functions.  It is designed
 * to accept only unsigned integers and should flag you at compile time if you try
 * passing in a signed value or float for example.  The functions shall be inlined
 * to provide the compiler some freedom of implementation.
 *
 ****************************************************************************************
 */
class Bit
{
	public:
		/**
		 * @brief Clears a bit.  It is inline to reduce overhead. If there
		 * is a risk of an interrupt causing tearing use PClr.
		 * @param[in/out] value: The value to change.
		 * @param[in] bit: The bit to clear.
		 */
		static inline void Clr( uint8_t& value, uint_fast8_t bit )
		{
			value &= ~( static_cast<uint8_t>( 1U ) << bit );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint16_t& value, uint_fast8_t bit )
		{
			value &= ~( static_cast<uint16_t>( 1U ) << bit );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint32_t& value, uint_fast8_t bit )
		{
			value &= ~( static_cast<uint32_t>( 1U ) << bit );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint64_t& value, uint_fast8_t bit )
		{
			value &= ~( static_cast<uint64_t>( 1U ) << bit );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint8_t* value_array, uint16_t bit )
		{
			uint16_t byte_index;
			uint8_t bit_index;

			byte_index = static_cast<uint16_t>( bit >> BYTE_INDEX_SHIFT );
			bit_index = bit & BIT_INDEX_MASK;

			return ( Clr( value_array[byte_index], bit_index ) );
		}

		/**
		 * @brief Sets a bit.  It is inline to reduce overhead. If there
		 * is a risk of an interrupt causing tearing use PSet.
		 * @param[in/out] value: The value to change.
		 * @param[in] bit: The bit to set.
		 */
		static inline void Set( uint8_t& value, uint_fast8_t bit )
		{
			value |= static_cast<uint8_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint16_t& value, uint_fast8_t bit )
		{
			value |= static_cast<uint16_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint32_t& value, uint_fast8_t bit )
		{
			value |= static_cast<uint32_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint64_t& value, uint_fast8_t bit )
		{
			value |= static_cast<uint64_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint8_t* value_array, uint16_t bit )
		{
			uint16_t byte_index;
			uint8_t bit_index;

			byte_index = static_cast<uint16_t>( bit >> BYTE_INDEX_SHIFT );
			bit_index = bit & BIT_INDEX_MASK;

			return ( Set( value_array[byte_index], bit_index ) );
		}

		/**
		 * @brief Tests a bit.  It is inline to reduce overhead.
		 * @param[in/out] value: The value to test.
		 * @param[in] bit: The bit to test.
		 * @return
		 * @retval true: The bit is set.
		 * @retval false: The bit is clr.
		 */
		static inline bool Test( uint8_t value, uint_fast8_t bit )
		{
			return ( ( value & ( static_cast<uint8_t>( 1U ) << bit ) ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint16_t value, uint_fast8_t bit )
		{
			return ( ( value & ( static_cast<uint16_t>( 1U ) << bit ) ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint32_t value, uint_fast8_t bit )
		{
			return ( ( value & ( static_cast<uint32_t>( 1U ) << bit ) ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint64_t value, uint_fast8_t bit )
		{
			return ( ( value & ( static_cast<uint64_t>( 1U ) << bit ) ) != 0U );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint8_t* value_array, uint16_t bit )
		{
			uint16_t byte_index;
			uint8_t bit_index;

			byte_index = static_cast<uint16_t>( bit >> BYTE_INDEX_SHIFT );
			bit_index = bit & BIT_INDEX_MASK;

			return ( Test( value_array[byte_index], bit_index ) );
		}

		/**
		 * @brief Sets a bit.  It is inline to reduce overhead.  If there
		 * is a risk of an interrupt causing tearing use PToggle.
		 * @param[in/out] value: The value to change.
		 * @param[in] bit: The bit to set.
		 */
		static inline void Toggle( uint8_t& value, uint_fast8_t bit )
		{
			value ^= static_cast<uint8_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint16_t& value, uint_fast8_t bit )
		{
			value ^= static_cast<uint16_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint32_t& value, uint_fast8_t bit )
		{
			value ^= static_cast<uint32_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint64_t& value, uint_fast8_t bit )
		{
			value ^= static_cast<uint64_t>( 1U ) << bit;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint8_t* value_array, uint16_t bit )
		{
			uint16_t byte_index;
			uint8_t bit_index;

			byte_index = static_cast<uint16_t>( bit >> BYTE_INDEX_SHIFT );
			bit_index = bit & BIT_INDEX_MASK;

			return ( Toggle( value_array[byte_index], bit_index ) );
		}

		/**
		 * @brief Finds total high bits in value.  It is inline to reduce overhead.
		 * @param[in] value: The value to test.
		 * @return Number of high bit count
		 */
		static inline uint8_t Total_Highs( uint8_t value )
		{
			uint8_t totals = 0U;

			for ( uint_fast8_t bit = 0; bit < 8U; bit++ )
			{
				totals += static_cast<uint8_t>( Test( value, bit ) );
			}
			return ( totals );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline uint8_t Total_Highs( uint16_t value )
		{
			uint8_t totals = 0U;

			for ( uint_fast8_t bit = 0; bit < 16U; bit++ )
			{
				totals += static_cast<uint8_t>( Test( value, bit ) );
			}
			return ( totals );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline uint8_t Total_Highs( uint32_t value )
		{
			uint8_t totals = 0U;

			for ( uint_fast8_t bit = 0; bit < 32U; bit++ )
			{
				totals += static_cast<uint8_t>( Test( value, bit ) );
			}
			return ( totals );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline uint8_t Total_Highs( uint64_t value )
		{
			uint8_t totals = 0U;

			for ( uint_fast8_t bit = 0; bit < 64U; bit++ )
			{
				totals += static_cast<uint8_t>( Test( value, bit ) );
			}
			return ( totals );
		}

	private:
		/**
		 * @brief Constructor, Destructor, Copy Constructor and
		 * Copy Assignment Operator definitions made private to disallow usage.
		 */
		Bit( void );
		~Bit( void );
		Bit( const Bit& rhs );
		Bit & operator =( const Bit& object );

		static const uint16_t BYTE_INDEX_SHIFT = 0x3U;
		static const uint8_t BIT_INDEX_MASK = 0x7U;
};

}

#endif
