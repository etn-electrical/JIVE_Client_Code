/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module will try and solve any datatype alignment issues.
 *
 *	@details This module will try and solve any datatype alignment issues that may arise
 *	due to using a uint8_t string and not having a guaranteed alignment behavior.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DATA_ALIGN_HPP
   #define DATA_ALIGN_HPP

#include "Exception.h"
#include "StdLib_MV.h"

/**
 ****************************************************************************************
 *	@brief This template class will take any datatype and try to machine align it.
 *
 *	@details This template class will take any datatype and try to machine align it
 *	both going into a string and leaving a string.  It is written in such a way that it
 *	will hopefully just be optimized to the hilt.
 *
 ****************************************************************************************
 */
template<typename T>
class Data_Align
{
	public:

		enum endianness_t
		{
			ENDIAN_LITTLE,
			ENDIAN_BIG,
		};

		/**
		 *  @brief Take the string and align the data along a constant datatype.
		 *  @param string - Source string to copy from.
		 *  @return the appropriately aligned data value.
		 *
		 *  @note Yes - I know I skipped the "breaks".
		 */
		static T From_Array( uint8_t* const array, endianness_t endianness = SYSTEM_ENDIANNESS )
		{
			T return_data;

			if ( endianness == SYSTEM_ENDIANNESS )
			{
				return_data = From_Same_Endiannes_Array( array );
			}
			else
			{
				return_data = From_Reverse_Endian_Array( array );
			}

			return ( return_data );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static T From_Same_Endiannes_Array( uint8_t* const array )
		{
			T return_data;
			uint8_t* dest_data;

			dest_data = reinterpret_cast<uint8_t*>( &return_data );
			switch ( sizeof( T ) )
			{
				case 8:
					dest_data[7] = array[7];
					dest_data[6] = array[6];
					dest_data[5] = array[5];
					dest_data[4] = array[4];

				case 4:
					dest_data[3] = array[3];
					dest_data[2] = array[2];

				case 2:
					dest_data[1] = array[1];

				case 1:
					dest_data[0] = array[0];
					break;

				default:
					BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &return_data ), array,
										 sizeof( T ) );
					break;
			}

			return ( return_data );
		}

		/**
		 *  @brief Take the string and align the data along a constant datatype.
		 *  @param string - Source string to copy from.
		 *  @return the appropriately aligned data value.
		 *
		 */
		static T From_Reverse_Endian_Array( uint8_t* const array )
		{
			T return_data;
			uint8_t* dest_data;

			dest_data = reinterpret_cast<uint8_t*>( &return_data );
			switch ( sizeof( T ) )
			{
				case 8:
					dest_data[7] = array[0];
					dest_data[6] = array[1];
					dest_data[5] = array[2];
					dest_data[4] = array[3];
					dest_data[3] = array[4];
					dest_data[2] = array[5];
					dest_data[1] = array[6];
					dest_data[0] = array[7];
					break;

				case 4:
					dest_data[3] = array[0];
					dest_data[2] = array[1];
					dest_data[1] = array[2];
					dest_data[0] = array[3];
					break;

				case 2:
					dest_data[1] = array[0];
					dest_data[0] = array[1];
					break;

				case 1:
					dest_data[0] = array[0];
					break;
					break;

				default:
					BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &return_data ), array,
										 sizeof( T ) );
					break;
			}

			return ( return_data );
		}

		/**
		 *  @brief Take the string and align the data along a constant datatype.
		 *  @param string - Source string to copy from.
		 *  @return the appropriately aligned data value.
		 *
		 *  @note Yes - I know I skipped the "breaks".
		 */
		static void To_Array( T data, uint8_t* const array, endianness_t endianness =
							  SYSTEM_ENDIANNESS )
		{
			if ( endianness == SYSTEM_ENDIANNESS )
			{
				To_Same_Endianness_Array( data, array );
			}
			else
			{
				To_Reverse_Endian_Array( data, array );
			}
		}

		/**
		 *  @brief Take "data" in and convert it into a string.
		 *  @param data - The data that needs to be loaded into the string.
		 *  @param string - The destination string which will hold the data.
		 *
		 *  @note Yes - I know I skipped the "breaks".
		 */
		static void To_Same_Endianness_Array( T data, uint8_t* const array )
		{
			uint8_t* src_data;

			src_data = reinterpret_cast<uint8_t*>( &data );
			switch ( sizeof( T ) )
			{
				case 8:
					array[7] = src_data[7];
					array[6] = src_data[6];
					array[5] = src_data[5];
					array[4] = src_data[4];

				case 4:
					array[3] = src_data[3];
					array[2] = src_data[2];

				case 2:
					array[1] = src_data[1];

				case 1:
					array[0] = src_data[0];
					break;

				default:
					BF_Lib::Copy_String( array, reinterpret_cast<uint8_t*>( &data ), sizeof( T ) );
					break;
			}
		}

		/**
		 *  @brief Take "data" in and convert it into a string.
		 *  @param data - The data that needs to be loaded into the string.
		 *  @param string - The destination string which will hold the data.
		 *
		 */
		static void To_Reverse_Endian_Array( T data, uint8_t* const array )
		{
			uint8_t* src_data;

			src_data = reinterpret_cast<uint8_t*>( &data );
			switch ( sizeof( T ) )
			{
				case 8:
					array[7] = src_data[0];
					array[6] = src_data[1];
					array[5] = src_data[2];
					array[4] = src_data[3];
					array[3] = src_data[4];
					array[2] = src_data[5];
					array[1] = src_data[6];
					array[0] = src_data[7];
					break;

				case 4:
					array[3] = src_data[0];
					array[2] = src_data[1];
					array[1] = src_data[2];
					array[0] = src_data[3];
					break;

				case 2:
					array[1] = src_data[0];
					array[0] = src_data[1];
					break;

				case 1:
					array[0] = src_data[0];
					break;

				default:
					BF_Lib::Copy_String( array, reinterpret_cast<uint8_t*>( &data ), sizeof( T ) );
					break;
			}
		}

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Data_Align( void );
		~Data_Align( void );
		Data_Align( const Data_Align& rhs );
		Data_Align & operator =( const Data_Align& object );

		static const endianness_t SYSTEM_ENDIANNESS = ENDIAN_LITTLE;
};



#endif
