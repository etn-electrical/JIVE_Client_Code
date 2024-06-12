/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Advanced_Vect.h"
#include "Ram.h"
#include "Babelfish_Assert.h"
/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
const Advanced_Vect::int_cback_t Advanced_Vect::INT_CBACKS[DEF_MAX_VECTORS] =
{
	&Settable_Vect0, &Settable_Vect1, &Settable_Vect2, &Settable_Vect3,
	&Settable_Vect4, &Settable_Vect5, &Settable_Vect6, &Settable_Vect7,
	&Settable_Vect8, &Settable_Vect9, &Settable_Vect10, &Settable_Vect11,
	&Settable_Vect12, &Settable_Vect13, &Settable_Vect14, &Settable_Vect15,
	&Settable_Vect16, &Settable_Vect17, &Settable_Vect18, &Settable_Vect19,
	&Settable_Vect20, &Settable_Vect21, &Settable_Vect22, &Settable_Vect23,
	&Settable_Vect24
};
uint8_t Advanced_Vect::m_cback_funcs_counter = 0U;
uint8_t Advanced_Vect::m_cback_funcs_max = DEF_MAX_VECTORS;
Advanced_Vect::cback_param_t* Advanced_Vect::m_cback_params;
Advanced_Vect::cback_func_t* Advanced_Vect::m_cback_funcs;



/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Advanced_Vect::Advanced_Vect( void ) :
	m_func_index( 0U )
{
	// This should be done with interrupts disabled.
	Push_TGINT();

	// This is the first time in the class so we do some init.
	if ( m_cback_funcs_counter == 0U )
	{
		m_cback_params = static_cast<cback_param_t*>( Ram::Allocate( sizeof( cback_param_t ) * m_cback_funcs_max ) );
		m_cback_funcs = static_cast<cback_func_t*>( Ram::Allocate( sizeof( cback_func_t ) * m_cback_funcs_max ) );
	}
	BF_ASSERT( m_cback_funcs_counter < m_cback_funcs_max );

	m_func_index = m_cback_funcs_counter;
	m_cback_funcs_counter++;

	m_cback_params[m_func_index] = static_cast<cback_param_t>( this );
	m_cback_funcs[m_func_index] = &Null_Advanced_Vect;

	Pop_TGINT();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Advanced_Vect::~Advanced_Vect( void )
{
	Ram::De_Allocate( m_cback_params );
	Ram::De_Allocate( m_cback_funcs );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Advanced_Vect::int_cback_t Advanced_Vect::Get_Int_Func( void ) const
{
	return ( INT_CBACKS[m_func_index] );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Advanced_Vect::Set_Vect( cback_func_t func, cback_param_t param ) const
{
	m_cback_params[m_func_index] = param;
	m_cback_funcs[m_func_index] = func;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Advanced_Vect::Init( uint8_t max_vectors )
{
	if ( max_vectors >= DEF_MAX_VECTORS )
	{
		max_vectors = DEF_MAX_VECTORS;
	}

	m_cback_funcs_max = max_vectors;
}
