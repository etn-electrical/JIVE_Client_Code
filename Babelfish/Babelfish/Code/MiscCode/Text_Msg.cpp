/**
 *****************************************************************************************
 *	@file Text_Msg.cpp
 *	@details See header file for module overview.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Text_Msg.h"
/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule 8-0-1: A +headerwarn option was previously issued for header 'Stdlib.h'
 * @n PCLint:
 * @n Justification: Stdlib.h is part of IAR includes, needed for atof function.
 */
/*lint -e829*/
#include <stdlib.h>		/* atof */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule 8-0-1: A +headerwarn option was previously issued for header 'Stdio.h'
 * @n PCLint:
 * @n Justification: Stdio.h is part of IAR includes, needed for snprintf function.
 */
/*lint -e829*/
#include <stdio.h>

namespace BF_Misc
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Bind( uint8_t* buffer, uint16_t length, bool wrap_around )
{
	m_read_buffer = buffer;
	m_write_buffer = buffer;
	m_length = length;
	m_wrap_around = wrap_around;

	Empty();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Bind( const uint8_t* buffer, uint16_t length, bool wrap_around )
{
	m_read_buffer = buffer;
	m_write_buffer = reinterpret_cast<uint8_t*>( nullptr );
	m_length = length;
	m_wrap_around = wrap_around;

	Empty();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Add_Byte( uint8_t byte )
{
	if ( nullptr != m_write_buffer )
	{
		if ( m_wrap_around )
		{
			m_write_buffer[m_index] = byte;

			m_index += 1U;

			if ( m_index >= m_length )
			{
				m_index = 0U;
			}
		}
		else
		{
			if ( m_index < m_length )
			{
				m_write_buffer[m_index] = byte;

				m_index += 1U;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Add_Array( const uint8_t* bytes, uint16_t length )
{
	while ( 0U < length )
	{
		length -= 1U;

		Add_Byte( *bytes );
		bytes += 1U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Compare( const uint8_t* bytes, uint16_t length, bool advance )
{
	bool result = ( m_length >= ( length + m_index ) ) ? true : false;
	uint16_t offset = length;

	while ( ( 0U < offset ) && result )
	{
		offset -= 1U;

		if ( bytes[offset] != m_read_buffer[offset + m_index] )
		{
			result = false;
		}
	}

	if ( result && advance )
	{
		m_index += length;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Add_U32( uint32_t value, uint16_t flags )
{
	uint8_t digit;
	uint8_t decimal_loc = static_cast<uint8_t>
		( ( flags & ( static_cast<uint16_t>( DECIMAL_MASK ) ) ) >> 4U );
	uint32_t divisor = 1000000000U;
	uint8_t position = 10U;

	if ( 0U < ( flags & static_cast<uint16_t>( POSITIONS_MASK ) ) )
	{
		while ( position > ( flags & static_cast<uint16_t>( POSITIONS_MASK ) ) )
		{
			position -= 1U;
			divisor /= 10U;
		}
	}

	while ( 0U < position )
	{
		position -= 1U;
		digit = static_cast<uint8_t>( value / divisor );
		value -= digit * divisor;
		divisor /= 10U;

		if ( ( 0U < digit ) || ( decimal_loc == position ) ||
			 ( 0U != ( flags & static_cast<uint16_t>( SHOW_LEADING_ZEROS ) ) ) )
		{
			if ( ( flags & static_cast<uint16_t>( INSERT_PLUS ) ) != 0U )
			{
				flags &= ~( static_cast<uint16_t>( INSERT_PLUS ) );
				Add_Byte( static_cast<uint8_t>( '+' ) );
			}
			else if ( ( flags & static_cast<uint16_t>( INSERT_MINUS ) ) != 0U )
			{
				flags &= ~( static_cast<uint16_t>( INSERT_MINUS ) );
				Add_Byte( static_cast<uint8_t>( '-' ) );
			}
			else
			{
				// empty else
			}
			flags |= static_cast<uint16_t>( SHOW_LEADING_ZEROS );
			Add_Byte( ( static_cast<uint8_t>( '0' ) + digit ) );

			if ( ( 0U < decimal_loc ) && ( decimal_loc == position ) )
			{
				Add_Byte( static_cast<uint8_t>( '.' ) );
			}
		}
		else if ( 0U < ( flags & static_cast<uint16_t>( POSITIONS_MASK ) ) )
		{
			Add_Byte( static_cast<uint8_t>( ' ' ) );
		}
		else
		{
			// empty else
		}
	}

	if ( ( flags & static_cast<uint16_t>( APPEND_COMMA ) ) > 0U )
	{
		Add_Byte( static_cast<uint8_t>( ',' ) );
	}

	if ( ( flags & static_cast<uint16_t>( APPEND_LF ) ) > 0U )
	{
		Add_Byte( static_cast<uint8_t>( LINE_FEED ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Add_S32( int32_t value, uint16_t flags )
{
	if ( 0 > value )
	{
		value = -value;
		flags |= static_cast<uint16_t>( INSERT_MINUS );
	}
	else if ( 0U != ( static_cast<uint16_t>( SHOW_SIGN ) & flags ) )
	{
		flags |= static_cast<uint16_t>( INSERT_PLUS );
	}
	else
	{
		// empty else
	}

	Add_U32( static_cast<uint32_t>( value ), flags );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Add_F32( float32_t value, uint16_t flags )
{

	char_t float_string[20];

	float_string[sizeof( float_string ) - 1U] = '\0';
	snprintf( &float_string[0], static_cast<size_t>( sizeof( float_string ) - 1U ),
			  "%E", static_cast<float64_t>( value ) );
	Add_String( reinterpret_cast<uint8_t*>( &float_string[0] ), 0U );

	if ( ( flags & static_cast<uint16_t>( APPEND_COMMA ) ) > 0U )
	{
		Add_Byte( static_cast<uint8_t>( ',' ) );
	}

	if ( ( flags & static_cast<uint16_t>( APPEND_LF ) ) > 0U )
	{
		Add_Byte( static_cast<uint8_t>( LINE_FEED ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Add_Hex_Byte( uint8_t byte )
{
	static const uint8_t hex_chars[] = "0123456789ABCDEF";

	Add_Byte( hex_chars[( byte >> 4U )] );
	Add_Byte( hex_chars[( byte & 0x0FU )] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Add_String( const uint8_t* data, uint16_t flags )
{
	if ( 0U != ( flags & static_cast<uint16_t>( TAG_NAME ) ) )
	{
		Add_Byte( static_cast<uint8_t>( '<' ) );

		if ( 0U != ( flags & static_cast<uint16_t>( CLOSE_TAG ) ) )
		{
			Add_Byte( static_cast<uint8_t>( '/' ) );
		}
	}

	while ( 0U != ( *data ) )
	{
		Add_Byte( *data );
		data += 1;
	}

	if ( 0U != ( flags & static_cast<uint16_t>( TAG_NAME ) ) )
	{
		Add_Byte( static_cast<uint8_t>( '>' ) );
	}

	if ( 0U != ( flags & static_cast<uint16_t>( ADD_CR ) ) )
	{
		Add_Byte( static_cast<uint8_t>( CARRIAGE_RETURN ) );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Extract_Byte( uint8_t& byte )
{
	bool result = true;

	if ( m_wrap_around )
	{
		byte = m_read_buffer[m_index];

		m_index += 1U;

		if ( m_index >= m_length )
		{
			m_index = 0U;
		}
	}
	else
	{
		if ( m_index < m_length )
		{
			byte = m_read_buffer[m_index];

			m_index += 1U;
		}
		else
		{
			result = false;
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Extract_U32( uint32_t& u32t, uint8_t delimiter )
{
	bool result = ( m_index < m_length ) ? true : false;
	bool negative = false;
	bool positive = false;
	uint8_t byte;

	u32t = 0U;

	while ( result && ( delimiter != m_read_buffer[m_index] ) && ( m_index < m_length ) )
	{
		result = Extract_Byte( byte );

		if ( static_cast<uint8_t>( ' ' ) < byte )
		{
			if ( static_cast<uint8_t>( '-' ) == byte )
			{
				if ( ( !negative ) && ( !positive ) )
				{
					negative = true;
				}
				else
				{
					result = false;		/* multiple negative signs */
				}
			}
			else
			{
				positive = true;

				byte -= static_cast<uint8_t>( '0' );

				if ( 10U > byte )
				{
					if ( ( 429496729U > u32t ) || ( ( 429496729U == u32t ) && ( 5U >= byte ) ) )
					{
						u32t = ( u32t * 10U ) + byte;
					}
					else
					{
						result = false;		/* overflow */
					}
				}
				else
				{
					result = false;		/* non numeric character */
				}
			}
		}
	}

	/* Move past delimeter */
	if ( result )
	{
		Extract_Byte( byte );
	}

	if ( negative )
	{
		if ( 0x7FFFFFFFU > u32t )
		{
			u32t = ( ~u32t ) + 1U;
		}
		else
		{
			result = false;		/* overflow */
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Extract_F32( float32_t& f32t, uint8_t delimiter )
{
	bool result = ( m_index < m_length ) ? true : false;
	char_t float_string[30];
	uint8_t float_index = 0U;
	uint8_t byte;

	while ( result && ( delimiter != m_read_buffer[m_index] ) && ( m_index < m_length ) )
	{
		result = Extract_Byte( byte );
		if ( static_cast<uint8_t>( ' ' ) < byte )
		{
			if ( ( sizeof( float_string ) - 1U ) > float_index )
			{
				if ( ( static_cast<uint8_t>( 'E' ) == byte ) ||
					 ( static_cast<uint8_t>( 'e' ) == byte ) ||
					 ( static_cast<uint8_t>( '+' ) == byte ) ||
					 ( static_cast<uint8_t>( '-' ) == byte ) ||
					 ( static_cast<uint8_t>( '.' ) == byte ) ||
					 ( ( static_cast<uint8_t>( '0' ) <= byte ) &&
					   ( static_cast<uint8_t>( '9' ) >= byte ) ) )
				{
					float_string[float_index] = static_cast<char_t>( byte );
					float_index += 1U;
				}
				else
				{
					result = false;
				}
			}
			else
			{
				result = false;
			}
		}
	}

	if ( result )
	{
		/* Move past delimiter */
		Extract_Byte( byte );

		float_string[float_index] = '\0';
		f32t = static_cast<float32_t>( atof( &float_string[0] ) );
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Extract_S32( int32_t& s32t, uint8_t delimiter )
{
	uint32_t u32t;
	bool result;

	result = Extract_U32( u32t, delimiter );
	s32t = static_cast<int32_t>( u32t );

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Extract_Hex_Byte( uint8_t& byte )
{
	bool result;
	uint8_t high_nibble = 0U;
	uint8_t low_nibble = 0U;

	result = Extract_Byte( high_nibble );

	if ( result )
	{
		result = Extract_Byte( low_nibble );
	}

	if ( result )
	{
		if ( high_nibble >= static_cast<uint8_t>( 'a' ) )
		{
			high_nibble -= static_cast<uint8_t>( 'a' ) - 10U;
		}
		else if ( high_nibble >= static_cast<uint8_t>( 'A' ) )
		{
			high_nibble -= static_cast<uint8_t>( 'A' ) - 10U;
		}
		else
		{
			high_nibble -= static_cast<uint8_t>( '0' );
		}

		if ( low_nibble >= static_cast<uint8_t>( 'a' ) )
		{
			low_nibble -= static_cast<uint8_t>( 'a' ) - 10U;
		}
		else if ( low_nibble >= static_cast<uint8_t>( 'A' ) )
		{
			low_nibble -= static_cast<uint8_t>( 'A' ) - 10U;
		}
		else
		{
			low_nibble -= static_cast<uint8_t>( '0' );
		}

		if ( ( 16U > high_nibble ) && ( 16U > low_nibble ) )
		{
			byte = ( static_cast<uint8_t>( ( high_nibble << 4U ) ) | low_nibble );
		}
		else
		{
			result = false;
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Trim( void )
{
	while ( ( static_cast<uint8_t>( ' ' ) == m_read_buffer[m_index] ) &&
			( m_index < m_length ) )
	{
		m_index += 1U;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Text_Msg::Advance( uint16_t length )
{
	m_index += length;

	if ( m_index >= m_length )
	{
		if ( m_wrap_around )
		{
			m_index -= m_length;
		}
		else
		{
			m_index = m_length;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Advance_Past( const uint8_t* bytes, int16_t length )
{
	bool is_found = false;
	bool compare_active;
	int16_t offset;

	while ( ( ( m_index + ( static_cast<uint16_t>( length ) ) ) <= m_length ) &&
			( !is_found ) )
	{
		offset = length - 1;
		compare_active = true;

		while ( compare_active )
		{
			if ( m_read_buffer[m_index + ( static_cast<uint16_t>( offset ) )] ==
				 bytes[offset] )
			{
				if ( 0 < offset )
				{
					offset -= 1;
				}
				else
				{
					m_index += static_cast<uint16_t>( length );
					is_found = true;
					compare_active = false;
				}
			}
			else
			{
				m_index += 1U;
				compare_active = false;
			}
		}
	}

	return ( is_found );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Text_Msg::Is_Clear_Until( uint8_t delimiter )
{
	bool is_done = false;
	bool is_clear = true;

	while ( ( m_index < m_length ) && ( !is_done ) )
	{
		if ( delimiter == m_read_buffer[m_index] )
		{
			is_done = true;
			m_index += 1U;
		}
		else if ( static_cast<uint8_t>( ' ' ) != m_read_buffer[m_index] )
		{
			is_done = true;
			is_clear = false;
		}
		else
		{
			m_index += 1U;
		}
	}

	return ( is_clear );
}

}
