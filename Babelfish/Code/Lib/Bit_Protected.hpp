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
#ifndef BIT_PROTECTED_H
#define BIT_PROTECTED_H

#include "Includes.h"
#include "Bit.hpp"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief Provides basic bit set/clr/test functionality but with extra interrupt protection.
 *
 * @details This class shall provide basic bit manipulation functions.  It is designed
 * to accept only unsigned integers and should flag you at compile time if you try
 * passing in a signed value or float for example.  The functions shall be inlined
 * to provide the compiler some freedom of implementation.  This class is the same as Bit.hpp
 * but adds the protection against accidental interrupts.
 *
 ****************************************************************************************
 */
class Bit_Protected
{
	public:
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
			return ( Bit::Test( value, bit ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint16_t value, uint_fast8_t bit )
		{
			return ( Bit::Test( value, bit ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint32_t value, uint_fast8_t bit )
		{
			return ( Bit::Test( value, bit ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint64_t value, uint_fast8_t bit )
		{
			return ( Bit::Test( value, bit ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline bool Test( uint8_t* value_array, uint16_t bit )
		{
			return ( Bit::Test( value_array, bit ) );
		}

		/**
		 * @brief Clears a bit.  It is inline to reduce overhead.  It will also
		 * lock the interrupts to verify that a secondary operation does not come in.
		 * @param[in/out] value: The value to change.
		 * @param[in] bit: The bit to clear.
		 */
		static inline void Clr( uint8_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Clr( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint16_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Clr( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint32_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Clr( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint64_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Clr( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Clr( uint8_t* value_array, uint16_t bit )
		{
			Push_TGINT();
			Bit::Clr( value_array, bit );
			Pop_TGINT();
		}

		/**
		 * @brief Sets a bit.  It is inline to reduce overhead.  It will also
		 * lock the interrupts to verify that a secondary operation does not come in.
		 * @param[in/out] value: The value to change.
		 * @param[in] bit: The bit to set.
		 */
		static inline void Set( uint8_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Set( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint16_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Set( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint32_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Set( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint64_t& value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Set( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Set( uint8_t* value_array, uint16_t bit )
		{
			Push_TGINT();
			Bit::Set( value_array, bit );
			Pop_TGINT();
		}

		/**
		 * @brief Toggles a bit.  It is inline to reduce overhead.  It will also
		 * lock the interrupts to verify that a secondary operation does not come in.
		 * The preceding "P" is for protected.
		 * @param[in/out] value: The value to change.
		 * @param[in] bit: The bit to toggle.
		 */
		static inline void Toggle( uint8_t value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Toggle( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint16_t value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Toggle( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint32_t value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Toggle( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint64_t value, uint_fast8_t bit )
		{
			Push_TGINT();
			Bit::Toggle( value, bit );
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Toggle( uint8_t* value_array, uint16_t bit )
		{
			Push_TGINT();
			Bit::Toggle( value_array, bit );
			Pop_TGINT();
		}

	private:
		/**
		 * @brief Constructor, Destructor, Copy Constructor and
		 * Copy Assignment Operator definitions made private to disallow usage.
		 */
		Bit_Protected( void );
		~Bit_Protected( void );
		Bit_Protected( const Bit_Protected& rhs );
		Bit_Protected & operator =( const Bit_Protected& object );

};

}

#endif
