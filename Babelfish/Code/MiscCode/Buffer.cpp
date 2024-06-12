/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Buffer.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

namespace BF_Misc
{
/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Buffer::Buffer( int16_t buffer_size ) :
	m_buffer_start( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_buffer_end( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_enque( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_deque( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_buffer_free( 0 )
{
	if ( buffer_size > 0 )
	{
		m_buffer_start =
			reinterpret_cast<uint8_t*>( Ram::Allocate( static_cast<uint16_t>
													   ( buffer_size ) ) );
		m_buffer_end = m_buffer_start + buffer_size;
		m_buffer_free = buffer_size;
		m_enque = m_buffer_start;
		m_deque = m_buffer_start;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Buffer::Get_String( uint8_t* dest, int_fast16_t length )
{
	BF_ASSERT( length <= ( ( m_buffer_end - m_buffer_start ) - m_buffer_free ) );
	m_buffer_free += length;
	while ( length != 0 )
	{
		*dest = *m_deque;
		dest++;
		m_deque++;
		if ( m_deque >= m_buffer_end )
		{
			m_deque = m_buffer_start;
		}
		length--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Buffer::Set_String( uint8_t* src, int_fast16_t length )
{
	BF_ASSERT( length <= m_buffer_free );
	m_buffer_free -= length;
	while ( length != 0 )
	{
		*m_enque = *src;
		src++;
		m_enque++;
		if ( m_enque >= m_buffer_end )
		{
			m_enque = m_buffer_start;
		}
		length--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Buffer::Flush( int_fast16_t length )
{
	if ( length < m_buffer_free )
	{
		if ( ( m_deque + length ) >= m_buffer_end )
		{
			m_deque = m_buffer_start + ( length - ( m_deque - m_buffer_end ) );
		}
		else
		{
			m_deque = m_deque + length;
		}
	}
	else
	{
		m_buffer_free = m_buffer_start - m_buffer_end;
		m_deque = m_enque;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Buffer::Get( void )
{
	uint8_t temp_return = 0U;

	BF_ASSERT( m_buffer_free < ( m_buffer_end - m_buffer_start ) );
	m_buffer_free++;
	temp_return = *m_deque;
	m_deque++;
	if ( m_deque >= m_buffer_end )
	{
		m_deque = m_buffer_start;
	}

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Buffer::Set( uint8_t data )
{
	BF_ASSERT( m_buffer_free != 0 );
	m_buffer_free--;
	*m_enque = data;
	m_enque++;
	if ( m_enque >= m_buffer_end )
	{
		m_enque = m_buffer_start;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Buffer::Is_Empty( void ) const
{
	return ( m_buffer_free == ( m_buffer_end - m_buffer_start ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Buffer::Is_Full( void ) const
{
	return ( m_buffer_free == 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Buffer::FIFO_Free( void ) const
{
	return ( static_cast<uint16_t>( m_buffer_free ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Buffer::Attach_String( uint8_t* string, int16_t length )
{
	m_buffer_start = string;
	m_buffer_end = m_buffer_start + length;
	m_buffer_free = length;
	m_enque = m_buffer_start;
	m_deque = m_buffer_start;
}

}
