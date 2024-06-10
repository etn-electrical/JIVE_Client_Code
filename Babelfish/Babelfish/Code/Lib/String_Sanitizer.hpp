/**
 *****************************************************************************************
 *  @file
 *
 *  @brief The ASCII Character Sanitizer for input Strings.
 *
 *  @details It is designed to work as basic ASCII character sanitizer for string parameters.
 *  Once an object is created by passing (constant) Bit-addressable ASCII white-list,
 *  it then validates the input string against the associated white-list provided.
 *
 *  @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef STR_SANITIZER_H_
#define STR_SANITIZER_H_

namespace BF_Lib
{

/*
 **************************************************************************************************
 *  @brief It is designed to work as basic ASCII character sanitizer for string parameters.
 *  Once an object is created by passing (constant) Bit-addressable ASCII white-list,
 *  it then validates the input string against the associated white-list provided.
 *  @n @b Usage: An example if you have input string parameters and you want to sanitize the input
 *  values before it gets saved into NV.This can be achieved by creating a String_Sanitizer object.
 *  This Class expects the input ASCII characters in the range from 0 to 128 (ASCII value) only.
 **************************************************************************************************
 */
class String_Sanitizer
{
	public:
		/// Type alias for size of constant bit addressable ASCII lookup(128 bits = 16 bytes) used for sanitizing
		/// strings
		static const uint8_t STR_SANTIZE_LOOKUP_WORD_LEN = 16U;

		/// Type alias for uint8_t, this defines the array word size.
		typedef uint8_t str_sanitize_word_t;

		/**
		 *  @brief Constructor to initialize white-list
		 *  @details Performs initialization ASCII character white list.
		 *  @param[in] lookup_list: constant list of lookup(white-list) array chracters of length
		 *STR_SANTIZE_LOOKUP_WORD_LEN
		 *  @return None
		 */
		String_Sanitizer( const str_sanitize_word_t lookup_list[STR_SANTIZE_LOOKUP_WORD_LEN] ) :
			m_sanitize_lookup( lookup_list )
		{}

		/**
		 *  @brief Will test each ASCII character of a string to see if it is allowed.
		 *  @details compares with constant white-list lookup passed in init function
		 *  @param[in] string: The string to be tested .
		 *  @param[in] str_length: The length of the string .
		 *  @returns length of the input str on success, returns (index)location of the rejected char when failed.
		 */
		uint32_t Sanitize( const char_t* string, uint32_t str_length ) const
		{
			uint_fast32_t str_index = 0U;

			for ( str_index = 0U; str_index < str_length; str_index++ )
			{
				if ( false == Test_Char( string[str_index], m_sanitize_lookup ) )
				{
					break;
				}
			}
			return ( str_index );
		}

		/**
		 *  @brief Destructor
		 *  @details should called when object goes out of scope
		 *  @return None
		 */
		~String_Sanitizer( void )
		{}

	private:

		typedef uint8_t bit_word_td;

		static const bit_word_td BIT_LIST_WORD_MULT_FCTR = sizeof( str_sanitize_word_t ) * 8U;
		static const bit_word_td BIT_LIST_SINGLE_WORD_BIT_MASK = BIT_LIST_WORD_MULT_FCTR - 1U;
		static const bit_word_td BIT_LIST_WORD_SHFT_FCTR = ( sizeof( str_sanitize_word_t ) / 2U )
			+ 3U;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		bool Test_Char( char_t ch, const str_sanitize_word_t* lookup ) const
		{
			return ( ( ( lookup[ch >> BIT_LIST_WORD_SHFT_FCTR] )
					   & ( 1U << ( ch & BIT_LIST_SINGLE_WORD_BIT_MASK ) ) ) != 0U );
		}

		const str_sanitize_word_t* m_sanitize_lookup;
};

}

#endif	// STR_SANITIZER_H_
