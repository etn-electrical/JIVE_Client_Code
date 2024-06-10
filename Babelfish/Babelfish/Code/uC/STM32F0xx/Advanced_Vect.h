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
#ifndef ADVANCED_VECT_H
#define ADVANCED_VECT_H

#include "Includes.h"
#include "Exception.h"

/**
 ****************************************************************************************
 * @brief
 *
 * @details
 *
 *
 ****************************************************************************************
 */
class Advanced_Vect
{
	public:
		/**
		 *  @brief Controls how many ram vector locations are created.  If this
		 *  is not run then the max vectors will be set to the max.  To conserve
		 *  RAM define this number to the bare minimum needed.  We will
		 *  catch an excess during the constructor if it happens.
		 */
		static void Init( uint8_t max_vectors );

		Advanced_Vect( void );
		~Advanced_Vect( void );

		/**
		 *  @brief Definitions for the different callback types.  int_cback is the
		 *  void argument function type.  The cback typedefs are associated with the
		 *  param argument passing function type.
		 */
		typedef void (* int_cback_t)( void );
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @details Returns the vector that should be put into the vector.  Designed to fill
		 * vectors or callbacks where the param is not an option.  This will allow
		 * us to create a more C++ friendly function pointer or callback.
		 * @return The pointer to the void function that we are going to need.
		 */
		int_cback_t Get_Int_Func( void ) const;

		/**
		 * @brief This function will set the parameter and actual function pointer.
		 * @param func - Function that should be called by the vector with a parameter.
		 * @param param - Parameter to be passed into the function on call.
		 */
		void Set_Vect( cback_func_t func, cback_param_t param = NULL ) const;

	private:
		/**
		 * @brief Member variable to indicate which vector we have.
		 */
		uint8_t m_func_index;

		static const uint8_t DEF_MAX_VECTORS = 16;

		/**
		 *  @brief Static vectors to be put in the actual interrupt vector.  These
		 *  are private and are handed back in the Get Int Function.
		 *
		 */
		static void Settable_Vect0( void )
		{
			( *m_cback_funcs[0] )( m_cback_params[0] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect1( void )
		{
			( *m_cback_funcs[1] )( m_cback_params[1] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect2( void )
		{
			( *m_cback_funcs[2] )( m_cback_params[2] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect3( void )
		{
			( *m_cback_funcs[3] )( m_cback_params[3] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect4( void )
		{
			( *m_cback_funcs[4] )( m_cback_params[4] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect5( void )
		{
			( *m_cback_funcs[5] )( m_cback_params[5] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect6( void )
		{
			( *m_cback_funcs[6] )( m_cback_params[6] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect7( void )
		{
			( *m_cback_funcs[7] )( m_cback_params[7] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect8( void )
		{
			( *m_cback_funcs[8] )( m_cback_params[8] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect9( void )
		{
			( *m_cback_funcs[9] )( m_cback_params[9] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect10( void )
		{
			( *m_cback_funcs[10] )( m_cback_params[10] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect11( void )
		{
			( *m_cback_funcs[11] )( m_cback_params[11] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect12( void )
		{
			( *m_cback_funcs[12] )( m_cback_params[12] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect13( void )
		{
			( *m_cback_funcs[13] )( m_cback_params[13] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect14( void )
		{
			( *m_cback_funcs[14] )( m_cback_params[14] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Settable_Vect15( void )
		{
			( *m_cback_funcs[15] )( m_cback_params[15] );
		}

		static uint8_t m_cback_funcs_counter;
		static uint8_t m_cback_funcs_max;
		static cback_param_t* m_cback_params;
		static cback_func_t* m_cback_funcs;

		static const int_cback_t INT_CBACKS[DEF_MAX_VECTORS];

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Null_Advanced_Vect( cback_param_t param )
		{}

};

#endif
