/**
 *****************************************************************************************
 *  @file
 *
 *  @brief The Bit List is essentially a compacted Queue.
 *
 *  @details It is designed to work with a contiguous enum list.
 *  For example a bunch of values in the range from 0 to X with little to no gaps in
 *  between 0 and X.  It works best with long lists where your queue may need to
 *  support all potential values being loaded.  So for example if you have a list of
 *  values from 0 to 100.  A queue would need to be at least 100bytes deep to be able
 *  to support all values being added at once.  The Bit List would only need 13 bytes
 *  to support the same basic behavior.  One drawback is that the values are not
 *  extracted as they were put in.  The list index remembers where
 *  it was last so it will always move over a value that is continuously put in.  This
 *  makes sure that all values are eventually hit.  The list is stepped through from 0
 *  to X.  So if ID 1 and 3 are set and you are just starting, you will get 1 first
 *  then 3.  A get next will clear the bit ID.
 *
 *  @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BIT_LIST_H_
#define BIT_LIST_H_

#include "Exception.h"
#include "OS_Tasker.h"
#include "Ram.h"

namespace BF_Lib
{

/**
 * This will turn on an abbreviated version of the bit list get next.
 * The drawback is that you need to support a 256 byte table.
 * The BIT_LIST_WORD_TD must be a uint8_t if this is defined.
 */
#define BIT_LIST_TABLE_OPTION

/*
 **************************************************************************************************
 *  @brief This is the Bit_List class and is designed to work with a contiguous enum list.
 *  @details If provides functionalities to check if the bit is set, to set a bit, to get the bit
 *  and bit number that is set, to check status if the bit list is full or empty, to get the
 *  active bit numbers.
 *  @n @b Usage: An example if you have a list of values from 0 to 100. A queue would need to
 *  be at least 100bytes deep to be able to support all values being added at once. The Bit List
 *  would only need 13 bytes to support the same basic behavior.
 **************************************************************************************************
 */
class Bit_List
{
	public:
		/// Type alias for uint16_t, this defines the array word size.
		typedef uint8_t bit_list_word_td;

		/// Type alias for uint16_t, this defines the bit index data type.
		typedef uint16_t bit_list_bit_td;

		/**
		 *  @brief Constructor
		 *  @details Performs initialization of number of bits, number of words and the bit list.
		 *  Later resets the whole module.
		 *  @param[in] num_bits: The number of bits value for the bit list.
		 *  @return None
		 */
		Bit_List( bit_list_bit_td num_bits );

		/**
		 *  @brief Constructor
		 *  @details Performs initialization of number of bits, number of words and the bit list.
		 *  Later resets the whole module.
		 *  @param[in] num_bits: The total number of bits in the bit list.
		 *  @param[in] bit_list: The length of the bit list in bytes.
		 *  @return None
		 */
		Bit_List( bit_list_bit_td num_bits, const bit_list_word_td* bit_list );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Bit_List goes out of scope or deleted.
		 *  @return None
		 */
		~Bit_List( void );

		/**
		 *  @brief Will test a bit to see if it is set.
		 *  @details Kind of like a peek.  Will not change state of bit.
		 *  @param[in] bit_num: The bit to be tested .
		 *  @return Status of the bit.
		 */
		bool Test( bit_list_bit_td bit_num );

		/**
		 *  @brief This will set a bit.
		 *  @details This will not set a bit multiple times.  It will only set a bit once.
		 *  @param[in] bit_num: The bit number to be set.
		 *  @return none.
		 */
		void Set( bit_list_bit_td bit_num );

		/**
		 *  @brief This will set the whole bit field list.
		 *  @return none.
		 */
		void Set( void );

		/**
		 * @brief Dumps the single bit.
		 * @details Dumps just the single bit.
		 * @param[in] bit_num: The bit number to be dumped.
		 * @return none.
		 */
		void Dump( bit_list_bit_td bit_num );

		/**
		 * @brief Dumps the whole bit field list.
		 * @details Essentially resets the whole module.
		 * @return none.
		 */
		void Dump( void );

		/**
		 * @brief This gives the next bit number that was set.
		 * @details  It will then clear that bit and move one bit more.
		 * If a bit is not found this function will return the const of NO_BIT_FOUND.
		 * @return The next bit number that was set.
		 */
		bit_list_bit_td Get_Next( void );

		/**
		 * @brief This gives the next bit number that is set.
		 * @details It will not clear the bit.  It will however move to that bit.
		 * If a bit is not found this function will return the const of NO_BIT_FOUND.
		 * @return The next bit number that is set.
		 */
		bit_list_bit_td Peek_Next( void );

		/**
		 * @brief
		 * @details Will return whether or not the bit list is empty.
		 * @return The status bit indicating whether bit list is empty.
		 * @retval true This means that the bit list is empty.
		 * @retval false This means that the bit list is not empty.
		 */
		bool Is_Empty( void )
		{
			return ( m_active_bits == 0U );
		}

		/**
		 * @brief
		 * @details Will return whether or not the bit list is full.
		 * @return The status bit indicating whether bit list is full.
		 * @retval true This means that the bit list is full.
		 * @retval false This means that the bit list is not full.
		 */
		bool Is_Full( void )
		{
			return ( m_active_bits == m_num_bits );
		}

		/**
		 * @brief
		 * @details This gives the number of active bit numbers in the bit list.
		 * @return The active bit numbers.
		 */
		bit_list_bit_td Active_Bits( void )
		{
			return ( m_active_bits );
		}

		/// Used to indicate whether or not a bit was found.  Tied to Get_Next.
		static const bit_list_bit_td NO_BIT_FOUND =
			static_cast<bit_list_bit_td>( ~static_cast<bit_list_bit_td>( 0U ) );

		/**
		 * @brief
		 * @details This will enable write access to a read only bit list. This should only be
		 * used by an advanced user who understands what will happen, if it is used incorrectly.
		 * @return None
		 */
		void Enable_Write();

		/**
		 * @brief Sets the pointer back to the start of the bit list sequence.
		 * @details This function will set the last place back to the beginning of the array of
		 * bits.  The benefit is that if you have arranged the bits in some sort of order
		 * and you want to always start at the beginning you can do so by calling this function
		 * to start back at the beginning of the bit list.  The bit list typically works by
		 * sequentially returning the next set bit number, clearing it then waiting in that spot
		 * for the next request.  This will set it back to the beginning.  This function is
		 * ONLY necessary if you have some order built into the bits.  If you use this function
		 * on a regular basis you have the potential of starving bits in the high order
		 * bytes.
		 * @return none
		 */
		void Restart( void )
		{
			m_previous_bit = 0;
		}

		/**
		 * @brief Copy Constructor.
		 * @param object: The object to copy.  This is necessary to copy over the full bitlist array.
		 * @return The copied object.
		 */
		Bit_List( const Bit_List& object );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Bit_List & operator =( const Bit_List& object );


		// Below is the list of members used for bit list operations
		typedef uint8_t bit_word_td;
		bit_list_word_td* m_bit_list;
		bit_list_bit_td m_num_bits;
		bit_list_bit_td m_active_bits;
		bit_list_bit_td m_previous_bit;
		uint_fast16_t m_num_words;
		bool m_read_only;

		// This flag is used to track the type of constuctor used during instantiation
		bool m_dynamic_bit_flag;

		// Below is the list of constants used for bit list operations
		static const bit_list_word_td BIT_LIST_LOOKUP_INVALID_BIT =
			~static_cast<bit_list_word_td>( 0U );
		static const bit_list_word_td BIT_LIST_LOOKUP[256];
		static const bit_word_td BIT_LIST_WORD_MULT_FCTR = sizeof( bit_list_word_td ) * 8U;
		static const bit_word_td BIT_LIST_SINGLE_WORD_BIT_MASK = BIT_LIST_WORD_MULT_FCTR - 1U;
		static const bit_word_td BIT_LIST_WORD_SHFT_FCTR = ( sizeof( bit_list_word_td ) / 2U ) + 3U;

		/**
		 * @brief Function calculate the number of words in the bit list
		 * @param[in] bit_count: bit count used in the bit list.
		 * @return 	number of words calculated as per the bitcount
		 */
		inline uint_fast16_t bit_list_num_words( bit_list_bit_td bit_count )
		{
			return ( ( bit_count + BIT_LIST_SINGLE_WORD_BIT_MASK ) / BIT_LIST_WORD_MULT_FCTR );
		}

		/**
		 * @brief Function to select the bit in the word
		 * @param[in] bit_ctr: bit counter for the bit to be selected
		 * @return 	selected bit list word
		 */
		inline uint_fast16_t bit_list_word_select( uint_fast16_t bit_ctr )
		{
			return ( bit_ctr >> BIT_LIST_WORD_SHFT_FCTR );
		}

		/**
		 * @brief Function to select the bit in the word
		 * @param[in] bit_ctr: bit counter for the bit to be selected
		 * @return 	selected bit list word
		 */
		inline uint_fast16_t bit_list_bit_shift( uint_fast16_t bit_ctr )
		{
			return ( bit_ctr & BIT_LIST_SINGLE_WORD_BIT_MASK );
		}

		/**
		 * @brief Function to set the bit in the bit list
		 * @param[in] bit: bit to be set
		 * @return None
		 */
		inline void bit_list_set_bit( bit_list_bit_td bit )
		{
			m_bit_list[bit >> BIT_LIST_WORD_SHFT_FCTR] |= 1U
				<< ( bit & BIT_LIST_SINGLE_WORD_BIT_MASK );
		}

		/**
		 * @brief Function to clear the bit in the bit list
		 * @param[in] bit: bit to be cleared
		 * @return None
		 */
		inline void bit_list_clr_bit( bit_list_bit_td bit )
		{
			m_bit_list[bit >> BIT_LIST_WORD_SHFT_FCTR] &= ~( 1U
															 << ( bit & BIT_LIST_SINGLE_WORD_BIT_MASK ) );
		}

		/**
		 * @brief Function to test the bit is set/cleared in the bit list
		 * @param[in] bit: bit to be checked
		 * @return set/cleared information is returned as boolean
		 */
		inline bool bit_list_test_bit( bit_list_bit_td bit )
		{
			return ( ( ( m_bit_list[bit >> BIT_LIST_WORD_SHFT_FCTR] )
					   & ( 1U << ( bit & BIT_LIST_SINGLE_WORD_BIT_MASK ) ) ) != 0U );
		}

};

}

#endif
