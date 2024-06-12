/**
 *****************************************************************************************
 *	@file My_FIFO.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "My_FIFO.h"
#include "Ram.h"
#include "Babelfish_Assert.h"
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
namespace BF_Misc
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
My_FIFO::My_FIFO( uint16_t buffer_size ) :
	m_buffer_start( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_buffer_end( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_buffer_free( 0U ),
	m_enque( reinterpret_cast<uint8_t*>( nullptr ) ),
	m_deque( reinterpret_cast<uint8_t*>( nullptr ) )
{
	BF_ASSERT( buffer_size > 0U );
	m_buffer_max_size = buffer_size;
	m_buffer_start = reinterpret_cast<uint8_t*>( Ram::Allocate( buffer_size ) );
	m_buffer_end = m_buffer_start + buffer_size;
	m_buffer_free = buffer_size;
	m_enque = m_buffer_start;
	m_deque = m_buffer_start;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void My_FIFO::Get_String( uint8_t* dest, uint16_t size )
{
	while ( size > 0U )
	{
		BF_ASSERT( m_buffer_free <= ( ( m_buffer_end - m_buffer_start ) ) );
		m_buffer_free++;
		*dest = *m_deque;
		dest++;
		m_deque++;
		if ( m_deque >= m_buffer_end )
		{
			m_deque = m_buffer_start;
		}
		size--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void My_FIFO::Set_String( uint8_t* src, uint16_t size )
{
	while ( size > 0U )
	{
		BF_ASSERT( m_buffer_free != 0U );
		m_buffer_free--;
		*m_enque = *src;
		src++;
		m_enque++;
		if ( m_enque >= m_buffer_end )
		{
			m_enque = m_buffer_start;
		}
		size--;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t My_FIFO::Get_Byte( void )
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
void My_FIFO::Set_Byte( uint8_t data )
{
	BF_ASSERT( m_buffer_free != 0U );
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
bool My_FIFO::Is_Empty( void ) const
{
	return ( m_buffer_free == ( m_buffer_end - m_buffer_start ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool My_FIFO::Is_Full( void ) const
{
	return ( m_buffer_free == 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t My_FIFO::FIFO_Free( void ) const
{
	return ( m_buffer_free );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t My_FIFO::FIFO_Size( void )
{
	return ( m_buffer_max_size - m_buffer_free );
}

}
