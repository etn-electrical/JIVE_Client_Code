/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains the Rand_Num class.
 *
 *	@details The Rand Num Class is a simple software psuedo random number generator.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RAND_NUM_H
#define RAND_NUM_H

namespace BF_Lib
{
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Psuedo_Soft_Rand_Num
{
	public:
		Psuedo_Soft_Rand_Num( uint32_t rand_seed = RAND_NUM_DEFAULT_SEED_VAL );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Psuedo_Soft_Rand_Num( void ) {}

		uint32_t Get( void );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Psuedo_Soft_Rand_Num( const Psuedo_Soft_Rand_Num& object );
		Psuedo_Soft_Rand_Num & operator =( const Psuedo_Soft_Rand_Num& object );

		static const uint32_t RAND_NUM_DEFAULT_SEED_VAL = 0xC24A290AU;
		uint32_t random_num;
};

}

#endif
