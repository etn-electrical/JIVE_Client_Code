/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RANGE_TPLATE_HPP
#define RANGE_TPLATE_HPP

#include "Exception.h"
#include "OS_Tasker.h"
#include "DCI.h"
#include "StdLib_MV.h"
#include "Bit.hpp"

/**
 *  @brief This class template facilitates an interface to accommodate range check for all different data types
 *  @n@b Usage: This interface should not be used directly it is used by Range_Check
 */
template<typename T>
class Range_TPlate
{
	public:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static bool Check( DCI_ACCESS_DATA_TD* data_access,
						   DCI_DATA_BLOCK_TD const* data_block, bool fix_value )
		{
			T data;
			uint8_t* src_data;
			T const* min = nullptr;
			T const* max = nullptr;
			DCI_LENGTH_TD element_ctr;
			bool enum_valid = true;	// This value is only set to false in the loop below.  No true setting down there.
			bool range_valid = true;// This value is only set to false in the loop below.  No true setting down there.
			T const* enum_item_ptr = nullptr;
			DCI_ENUM_COUNT_TD i;
			GINT_TDEF temp_gint = 0U;
			DCI_RANGE_BLOCK const* range_block = nullptr;
			DCI_ENUM_COUNT_TD enum_count = 0U;
			DCI_ENUM_BLOCK const* enum_block = nullptr;

			if ( data_block->range_block_index != DCI::INVALID_INDEX )
			{
				range_block = DCI::Get_Range_Block( data_block->range_block_index );
				if ( range_block != nullptr )
				{
					enum_count = DCI::Get_Enum_Count( range_block->enum_block_index );
					enum_block = DCI::Get_Enum_Block( range_block->enum_block_index );
					min = reinterpret_cast<T const*>( range_block->min );
					max = reinterpret_cast<T const*>( range_block->max );
				}
			}

			element_ctr = data_access->length;
			src_data = reinterpret_cast<uint8_t*>( data_access->data );

			while ( ( element_ctr != 0 ) &&
					( ( range_valid == true ) || ( enum_valid == true ) ) )
			{
				Push_GINT( temp_gint );
				Switch_Copy( reinterpret_cast<uint8_t*>( &data ),
							 &src_data[element_ctr - sizeof( T )] );
				Pop_GINT( temp_gint );

				if ( ( enum_block != nullptr ) && ( enum_count != 0xFFFF ) &&
					 BF_Lib::Bit::Test( data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_ENUM_VAL ) )
				{
					enum_item_ptr = ( T* )enum_block->enum_list;

					i = 0U;
					while ( ( i < enum_count ) &&
							( data != enum_item_ptr[i] ) )
					{
						i++;
					}
					if ( i == enum_count )
					{
						enum_valid = false;
					}
				}
				else
				{
					enum_valid = false;
				}

				if ( BF_Lib::Bit::Test( data_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_RANGE_VAL ) )
				{
					if ( ( data < *min ) || ( data > *max ) )
					{
						if ( fix_value == true )
						{
							Push_GINT( temp_gint );
							if ( data < *min )
							{
								Switch_Copy( &src_data[element_ctr - sizeof( T )],
											 reinterpret_cast<uint8_t const*>( min ) );
							}
							else
							{
								Switch_Copy( &src_data[element_ctr - sizeof( T )],
											 reinterpret_cast<uint8_t const*>( max ) );
							}
							Pop_GINT( temp_gint );
						}
						else
						{
							range_valid = false;
						}
					}
				}
				else
				{
					range_valid = false;
				}
				element_ctr -= sizeof( T );
			}

			return ( ( range_valid == true ) || ( enum_valid == true ) );
		}

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Range_TPlate( const Range_TPlate& rhs );
		Range_TPlate & operator =( const Range_TPlate& object );

		Range_TPlate( void );
		~Range_TPlate( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static inline void Switch_Copy( uint8_t* dest_ptr, uint8_t const* src_ptr )
		{
			switch ( sizeof( T ) )
			{
				case 8:
					dest_ptr[7] = src_ptr[7];
					dest_ptr[6] = src_ptr[6];
					dest_ptr[5] = src_ptr[5];
					dest_ptr[4] = src_ptr[4];

				/// @MISRA_DEV: break left out to make this code compile in an optimized fashion.
				/// Because the size of T will be hard coded the optimizer will only place the right
				/// code in place without the switch.  Smart Optimizer.
				// no break
				case 4:
					dest_ptr[3] = src_ptr[3];
					dest_ptr[2] = src_ptr[2];

				/// @MISRA_DEV: break left out to make this code compile in an optimized fashion.
				/// Because the size of T will be hard coded the optimizer will only place the right
				/// code in place without the switch.  Smart Optimizer.
				// no break
				case 2:
					dest_ptr[1] = src_ptr[1];

				/// @MISRA_DEV: break left out to make this code compile in an optimized fashion.
				/// Because the size of T will be hard coded the optimizer will only place the right
				/// code in place without the switch.  Smart Optimizer.
				// no break
				case 1:
					dest_ptr[0] = src_ptr[0];

				/// @MISRA_DEV: break left out to make this code compile in an optimized fashion.
				/// Because the size of T will be hard coded the optimizer will only place the right
				/// code in place without the switch.  Smart Optimizer.
				// no break
				default:
					break;
			}
		}

};

#endif
