/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RANGE_CHECK_H
#define RANGE_CHECK_H

#include "DCI_Defines.h"

/**
 *
 * @brief  Provides a interface to validate the range of a DCI parameter
 *
 */
class Range_Check
{
	public:

		/**
		 * Check the range on the data passed in.
		 * @param[in] data_access contains the string to check.
		 * @param[in] data_block contains the details (range, datatype) of the data.
		 * @param[in] If fix_value is set to true then the data string will be pegged to either the min or max.
		 * @return range was satisfied.  If fix_value is true then the return will always be true
		 * (value good).
		 */
		static bool Check_Range( DCI_ACCESS_DATA_TD* data_access,
								 DCI_DATA_BLOCK_TD const* data_block, bool fix_value = false );

	private:
		/**
		 * Range_Check is all static and thus the constructor is static and
		 * serves no real purpose.
		 */
		Range_Check( void );
		~Range_Check( void );
		Range_Check( const Range_Check& rhs );
		Range_Check & operator =( const Range_Check& object );

};

#endif
