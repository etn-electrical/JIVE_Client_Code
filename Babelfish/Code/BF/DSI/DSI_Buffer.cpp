/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "DSI_Buffer.h"
#include "Ram.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void* DSI_Buffer::Allocate( size_t size )
{
	return ( Ram::Allocate( size ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DSI_Buffer::Deallocate( void* ptr )
{
	Ram::De_Allocate( ptr );
}
