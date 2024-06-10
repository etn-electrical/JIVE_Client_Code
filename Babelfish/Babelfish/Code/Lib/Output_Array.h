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
#ifndef OUTPUT_ARRAY_H
   #define OUTPUT_ARRAY_H


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Output_Array
{
	public:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		Output_Array( void ) {}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Output_Array() {}

		// This is for setting an array of bits which correspond to a list of outputs.
		// Best be sure you have the right length of states for the get.  Otherwise...
		virtual void Set( uint8_t* state_array ) = 0;

		virtual void Get( uint8_t* state_array ) = 0;

		// This is for setting a single element in the list of bits.
		virtual void Set_Single( uint8_t index, bool state ) = 0;

		virtual bool Get_Single( uint8_t index ) = 0;

		virtual uint8_t Byte_Size( void ) = 0;

	private:
};



#endif
