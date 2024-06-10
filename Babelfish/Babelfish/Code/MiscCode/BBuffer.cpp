/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "BBuffer.h"
#include "Ram.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define BBUFF_STRUCT_SIZE       sizeof( BBUFF_STRUCT_TD )

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
Buffer::Buffer( uint16_t buffer_size )
{
	typedef struct BBUFF_STRUCT_TD
	{
		BBUFF_STRUCT_TD* next;
		uint16_t length;
		uint8_t data;				// This needs to be at the end.
	} BBUFF_STRUCT_TD;

	BBUFF_STRUCT_TD* m_enque;
	BBUFF_STRUCT_TD* m_deque;
	uint8_t* m_buffer;
	uint8_t* m_buffer_start;
	uint8_t* m_buffer_end;
	uint_fast16_t m_buffer_free;

	if ( buffer_size > BBUFF_STRUCT_SIZE )
	{
		m_buffer_start = ( uint8_t* )Ram::Allocate( buffer_size );
		m_buffer_end = m_buffer_start + buffer_size;
		m_buffer_free = buffer_size;
		m_enque = BBUFF_STRUCT_TD;
		m_deque = m_buffer_start;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Buffer::Get_String( uint8_t* dest, uint_fast16_t length )
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
void Buffer::Set_String( uint8_t* src, uint_fast16_t length )
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
void Buffer::Flush( uint_fast16_t length )
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
	uint8_t temp_return;

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
bool Buffer::Is_Empty( void )
{
	return ( m_buffer_free == ( m_buffer_end - m_buffer_start ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Buffer::Is_Full( void )
{
	return ( m_buffer_free == 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t Buffer::FIFO_Free( void )
{
	return ( m_buffer_free );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Buffer::Attach_String( uint8_t* string, uint16_t length )
{
	m_buffer_start = string;
	m_buffer_end = m_buffer_start + length;
	m_buffer_free = length;
	m_enque = m_buffer_start;
	m_deque = m_buffer_start;
}
