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
#ifndef BUFFER_H
   #define BUFFER_H


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class BBuffer
{
	public:
		BBuffer( uint16_t buffer_size );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~BBuffer() {}

		// BOOL 		Get_String( uint8_t* dest, uint_fast16_t length );

		bool Set_String( uint8_t* src, uint_fast16_t length );


		// If you use the following you have to promise not to write using the pointer
		// passed to you.  That is verboten.
		bool Get_Block_Ptr( const uint8_t** dest, uint_fast16_t& length );

		bool Goto_Next_Block( void );



	// BOOL		Flush( uint_fast16_t length );
	//
	// uint8_t 		Get( void );
	// void		Set( uint8_t data );
	//
	// BOOL 		Is_Empty( void );
	// BOOL 		Is_Full( void );
	// uint16_t 		FIFO_Free( void );
	//
	// uint8_t& 		operator[] ( uint16_t index )
	// {
	// if ( ( index + m_enque ) >= m_buffer_end )
	// {
	// index = index - ( m_enque - m_buffer_end );
	// }
	// return ( m_buffer_start[index] );
	// }

	// void		Attach_String( uint8_t* string, uint16_t length );

	private:
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
};



#endif
