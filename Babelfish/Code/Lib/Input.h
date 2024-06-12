/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module provides the port pin read control in the positive way (Input_Pos)
 *	and in the negative way (Input_Neg)
 *
 *	@details This contains the functionality to read the connected input, and use it
 *	for further functionality.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef INPUT_H
#define INPUT_H

#include "uC_IO_Config.h"

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is a Input class and it is used to read the port pin.
 * @details While we say the port pin status on, it sets the state of connected input
 * (Input_Pos) and while we say the port pin status on, it clears the state of connected
 * output (Input_Neg).
 * @n @b Usage:
 * It provides public methods that are inherited in Input_Neg and Input_Pos to:
 * @n @b 1. To read the port pin and hence the connection.
 * @n @b 2. To read the status of the port pin.
 ****************************************************************************************
 */
class Input
{
	public:

		/**
		 *  @brief Constructor
		 *  @details It does not do anything here.
		 *  @return None
		 */
		Input( void ) {}

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Input goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Input() {}

		/**
		 *  @brief All the virtual functions implemented in the Input_Pos and Input_Neg.
		 *  @return None
		 */
		virtual inline bool Get( void ) = 0;

		virtual inline bool Get_Raw( void ) = 0;

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Input( const Input& rhs );
		Input & operator =( const Input& object );

};

}

#endif
