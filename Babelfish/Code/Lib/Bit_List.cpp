/**
 *****************************************************************************************
 *  @file
 *  @details See header file for module overview.
 *  @copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Bit_List.h"
#include "Bit.hpp"
#include "StdLib_MV.h"

namespace BF_Lib
{

#ifdef BIT_LIST_TABLE_OPTION
const Bit_List::bit_list_word_td Bit_List::BIT_LIST_LOOKUP[256] =
{
	BIT_LIST_LOOKUP_INVALID_BIT,
	0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	5U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	6U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	5U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	7U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	5U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	6U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	5U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	4U, 0U, 1U, 0U, 2U, 0U, 1U, 0U,
	3U, 0U, 1U, 0U, 2U, 0U, 1U, 0U
};
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bit_List::Bit_List( bit_list_bit_td num_bits ) :
	m_bit_list( reinterpret_cast<bit_list_word_td*>( nullptr ) ),
	m_num_bits( 0U ),
	m_active_bits( 0U ),
	m_previous_bit( 0U ),
	m_num_words( 0U ),
	m_read_only( false ),
	m_dynamic_bit_flag( true )
{
	if ( num_bits == 0U )
	{
		num_bits = 1U;		// This avoids an issue with creating essentially a null list.
	}

	m_num_bits = num_bits;
	m_num_words = bit_list_num_words( m_num_bits );
	m_bit_list = reinterpret_cast<bit_list_word_td*>( Ram::Allocate(
														  bit_list_num_words( m_num_bits ) *
														  sizeof( bit_list_word_td ) ) );

	Dump();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bit_List::Bit_List( bit_list_bit_td num_bits, const bit_list_word_td* bit_list ) :
	m_bit_list( reinterpret_cast<bit_list_word_td*>( nullptr ) ),
	m_num_bits( num_bits ),
	m_active_bits( 0U ),
	m_previous_bit( 0U ),
	m_num_words( bit_list_num_words( m_num_bits ) ),
	m_read_only( true ),
	m_dynamic_bit_flag( false )
{
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 5-2-5 Attempt to cast away
	 * const/volatile from a pointer or reference.
	 * @n Justification: We are using the m_read_only tag to verify access.
	 * This is to reduce duplicate code and complexity associated
	 * with duplicate pointers.
	 */
	/*lint -e{1960} # MISRA Deviation */
	m_bit_list = const_cast<bit_list_word_td*>( bit_list );

	for ( uint16_t i = 0U; i < num_bits; i++ )
	{
		if ( Test( i ) )
		{
			m_active_bits++;
		}
	}

	Dump();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bit_List::~Bit_List()
{
	if ( ( m_bit_list != nullptr ) && ( m_dynamic_bit_flag == true ) )
	{
		delete m_bit_list;
		m_bit_list = nullptr;
	}
	else if ( ( m_bit_list != nullptr ) && ( m_dynamic_bit_flag == false ) )
	{
		m_bit_list = nullptr;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Bit_List::Test( bit_list_bit_td bit_num )
{
	bool bit_state = false;

	if ( ( bit_num < m_num_bits ) && bit_list_test_bit( bit_num ) )
	{
		bit_state = true;
	}

	return ( bit_state );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bit_List::Set( bit_list_bit_td bit_num )
{
	bool bit_list_test;

	if ( !m_read_only )
	{
		Push_TGINT();

		bit_list_test = bit_list_test_bit( bit_num );
		if ( ( bit_num < m_num_bits ) && ( bit_list_test == false ) )
		{
			bit_list_set_bit( bit_num );
			m_active_bits++;
		}

		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bit_List::Set( void )
{
	bit_list_bit_td bit_num;

	Push_TGINT();

	m_previous_bit = 0U;
	if ( !m_read_only )
	{
		m_active_bits = m_num_bits;
		for ( bit_num = 0U; bit_num < m_num_bits; bit_num++ )
		{
			bit_list_set_bit( bit_num );
		}
	}
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bit_List::Dump( bit_list_bit_td bit_num )
{
	bool bit_list_test;

	if ( !m_read_only )
	{
		Push_TGINT()
		;
		bit_list_test = bit_list_test_bit( bit_num );
		if ( ( bit_num < m_num_bits ) && ( bit_list_test == true ) )
		{
			bit_list_clr_bit( bit_num );
			m_active_bits--;
		}
		Pop_TGINT();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bit_List::Dump( void )
{
	Push_TGINT();

	m_previous_bit = 0U;
	if ( !m_read_only )
	{
		m_active_bits = 0U;
		for ( uint_fast16_t i = 0U; i < m_num_words; i++ )
		{
			m_bit_list[i] = 0U;
		}
	}
	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Bit_List::Get_Next( void )
{
#ifdef BIT_LIST_TABLE_OPTION

	bit_list_bit_td return_bit = NO_BIT_FOUND;
	uint_fast16_t bit_counter;
	bit_list_word_td temp_word;
	uint16_t word_index;

	Push_TGINT();

	if ( m_active_bits > 0U )
	{
		bit_counter = m_previous_bit;

		word_index = static_cast<uint16_t>( bit_list_word_select( bit_counter ) );
		temp_word = static_cast<bit_list_word_td>(
			static_cast<bit_list_word_td>
			( NO_BIT_FOUND ) << bit_list_bit_shift( bit_counter ) );
		temp_word = static_cast<bit_list_word_td>( m_bit_list[word_index] & temp_word );
		return_bit = BIT_LIST_LOOKUP[temp_word];
		if ( return_bit == BIT_LIST_LOOKUP_INVALID_BIT )
		{
			do
			{
				word_index++;
				if ( word_index >= m_num_words )
				{
					word_index = 0U;
				}
			} while ( BIT_LIST_LOOKUP[m_bit_list[word_index]] == BIT_LIST_LOOKUP_INVALID_BIT );

			return_bit = BIT_LIST_LOOKUP[m_bit_list[word_index]];
		}

		if ( !m_read_only )
		{
			m_active_bits--;
			Bit::Clr( m_bit_list[word_index], return_bit );
		}
		return_bit += static_cast<uint16_t>( word_index * 8U );
		m_previous_bit = return_bit + 1U;
		if ( m_previous_bit >= m_num_bits )
		{
			m_previous_bit = 0U;
		}
	}
	Pop_TGINT();

	return ( return_bit );

#else

	uint_fast16_t iteration_counter;
	bit_list_bit_td return_bit = BIT_LIST_NO_BIT_FOUND;
	bit_list_bit_td bit_counter;
	bit_list_bit_td max_bits;

	Push_TGINT();
	if ( m_active_bits > 0 )
	{
		max_bits = m_num_words * ( sizeof( bit_list_word_td ) * 8 );
		iteration_counter = 0U;
		bit_counter = m_previous_bit;
		while ( ( iteration_counter < max_bits ) &&
				!bit_list_test_bit( bit_counter ) )
		{
			if ( ( ( bit_counter & BIT_LIST_SINGLE_WORD_BIT_MASK ) == 0 ) &&
				 ( m_bit_list[bit_list_word_select( bit_counter )] == 0 ) )
			{
				bit_counter += ( sizeof( bit_list_word_td ) * 8 );
				iteration_counter += ( sizeof( bit_list_word_td ) * 8 );
			}
			else
			{
				bit_counter++;
				iteration_counter++;
			}
			if ( bit_counter >= m_num_bits )
			{
				bit_counter = 0U;
			}
		}

		m_previous_bit = bit_counter + 1;
		if ( m_previous_bit >= m_num_bits )
		{
			m_previous_bit = 0U;
		}

		if ( bit_list_test_bit( bit_counter ) )
		{
			if ( !m_read_only )
			{
				bit_list_clr_bit( bit_counter );
				m_active_bits--;
			}
		}
		return_bit = bit_counter;
	}
	Pop_TGINT();

	return ( return_bit );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Bit_List::Peek_Next( void )
{
	bit_list_bit_td return_bit = NO_BIT_FOUND;
	uint_fast16_t bit_counter;
	uint16_t word_index;

	Push_TGINT()
	;
	if ( m_active_bits > 0U )
	{
		bit_counter = m_previous_bit;

		word_index = static_cast<uint16_t>( bit_list_word_select( bit_counter ) );

		while ( BIT_LIST_LOOKUP[m_bit_list[word_index]] == BIT_LIST_LOOKUP_INVALID_BIT )
		{
			word_index++;
			if ( word_index >= m_num_words )
			{
				word_index = 0U;
			}
		}

		return_bit = BIT_LIST_LOOKUP[m_bit_list[word_index]];
		return_bit += static_cast<uint16_t>( word_index * 8U );
		m_previous_bit = return_bit;
	}
	Pop_TGINT();
	return ( return_bit );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Bit_List::Enable_Write( void )
{
	m_read_only = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bit_List::Bit_List( const Bit_List& object ) :
	m_bit_list( reinterpret_cast<bit_list_word_td*>( nullptr ) ),
	m_num_bits( object.m_num_bits ),
	m_active_bits( 0U ),
	m_previous_bit( 0U ),
	m_num_words( object.m_num_words ),
	m_read_only( object.m_read_only ),
	m_dynamic_bit_flag( false )
{
	uint_fast16_t list_byte_length;
	bit_list_word_td* ptr_storage;

	Push_TGINT();

	m_active_bits = object.m_active_bits;
	m_previous_bit = object.m_previous_bit;

	if ( object.m_read_only == false )
	{
		list_byte_length = bit_list_num_words( m_num_bits ) * sizeof( bit_list_word_td );
		m_bit_list = reinterpret_cast<bit_list_word_td*>( Ram::Allocate( list_byte_length ) );
		BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( m_bit_list ),
							 reinterpret_cast<uint8_t*>( object.m_bit_list ),
							 list_byte_length );
		m_dynamic_bit_flag = true;
	}
	else
	{
		ptr_storage = object.m_bit_list;
		m_bit_list = ptr_storage;
	}

	Pop_TGINT();
}

}	/* end namespace BF_Lib for this module */
