/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "OS_List_Item.h"

/*
 *****************************************************************************
 *		Variables
 *****************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_List_Item::OS_List_Item( void ) :
	m_next( reinterpret_cast<volatile OS_List_Item*>( nullptr ) ),
	m_previous( reinterpret_cast<volatile OS_List_Item*>( nullptr ) ),
	m_value( 0U ),
	m_owner( reinterpret_cast<void*>( nullptr ) ),
	m_container( reinterpret_cast<void*>( nullptr ) )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_List_Item::~OS_List_Item( void )
{}
