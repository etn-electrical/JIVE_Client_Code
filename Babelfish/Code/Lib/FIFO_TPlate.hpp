/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FIFO_TPLATE_HPP
   #define FIFO_TPLATE_HPP

#include "OS_Tasker.h"
#include "Ram.h"
#include "Babelfish_Assert.h"
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
template<typename T>
class FIFO_TPlate
{
	public:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		FIFO_TPlate( T* allocated_buff, uint16_t buffer_size )
		{
			Init_FIFO( allocated_buff, buffer_size );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		FIFO_TPlate( uint16_t buffer_size )
		{
			Init_FIFO( ( T* )Ram::Allocate( sizeof( T ) * buffer_size ), buffer_size );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~FIFO_TPlate()
		{ Ram::De_Allocate( m_buffer_start ); }


		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Get_Data( T* dest, uint_fast16_t length )
		{
			Push_TGINT();
			BF_ASSERT( length <= ( m_buffer_size - m_buffer_free ) );
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
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Set_Data( T* src, uint_fast16_t length )
		{

			Push_TGINT();
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
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Dump_Data( uint_fast16_t length )
		{
			Push_TGINT();
			m_deque += length;
			if ( m_deque >= m_buffer_end )
			{
				m_deque = m_buffer_start + ( m_deque - m_buffer_end );
			}
			Pop_TGINT();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		T Get( void )
		{
			T temp_return;

			Push_TGINT();
			BF_ASSERT( m_buffer_free < m_buffer_size );
			m_buffer_free++;
			temp_return = *m_deque;
			m_deque++;
			if ( m_deque >= m_buffer_end )
			{
				m_deque = m_buffer_start;
			}
			Pop_TGINT();

			return ( temp_return );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Set( T data )
		{
			Push_TGINT();
			BF_ASSERT( m_buffer_free != 0 );
			m_buffer_free--;
			*m_enque = data;
			m_enque++;
			if ( m_enque >= m_buffer_end )
			{
				m_enque = m_buffer_start;
			}
			Pop_TGINT();
		}

		bool Is_Empty( void )    { return ( m_buffer_free == m_buffer_size ); }

		bool Is_Full( void )     { return ( m_buffer_free == 0 ); }

		uint16_t FIFO_Free( void )   { return ( m_buffer_free ); }

	private:
		T* m_buffer_start;
		T* m_buffer_end;
		T* m_enque;
		T* m_deque;
		uint_fast16_t m_buffer_free;
		uint_fast16_t m_buffer_size;


		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		void Init_FIFO( T* allocated_buff, uint16_t buffer_size )
		{
			Push_TGINT();
			BF_ASSERT( buffer_size > 0 );
			m_buffer_start = allocated_buff;
			m_buffer_end = m_buffer_start + buffer_size;
			m_buffer_size = buffer_size;
			m_buffer_free = buffer_size;
			m_enque = m_buffer_start;
			m_deque = m_buffer_start;
			Pop_TGINT();
		}

};



#endif
