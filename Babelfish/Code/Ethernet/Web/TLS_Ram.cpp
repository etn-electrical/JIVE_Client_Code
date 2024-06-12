/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram.h"
#include "Exception.h"
#include "TLS_Ram.h"


#ifdef __cplusplus
extern "C" {
#endif

static const size_t MAX_TLS_FRAGMENT_SIZE = 16384U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Tls_Mem_Calloc( size_t num, size_t size )
{
	void* temp_return = nullptr;

	if ( !( size > MAX_TLS_FRAGMENT_SIZE ) )
	{
		temp_return = Ram::Allocate_Basic( num * size );
	}

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Tls_Mem_Free( void* ptr )
{
	if ( ptr != nullptr )
	{
		Ram::De_Allocate_Basic( ptr );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Tls_Mem_Malloc( size_t size )
{
	void* temp_return = nullptr;

	if ( !( size > MAX_TLS_FRAGMENT_SIZE ) )
	{
		temp_return = Ram::Allocate_Basic( size );
	}

	return ( temp_return );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* Tls_Mem_Realloc( void* ptr, size_t size )
{
	void* temp_return = nullptr;

	if ( !( size > MAX_TLS_FRAGMENT_SIZE ) )
	{
		temp_return = Ram::Reallocate_Basic( ptr, size );
	}

	return ( temp_return );
}

#ifdef __cplusplus
}
#endif



