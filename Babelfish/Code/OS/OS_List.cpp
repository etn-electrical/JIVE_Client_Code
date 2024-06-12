/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "OS_List.h"

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
OS_List::OS_List( void ) :
	m_index( reinterpret_cast<volatile OS_List_Item*>( nullptr ) ),
	m_last_item( reinterpret_cast<volatile OS_List_Item*>( nullptr ) ),
	m_num_items( 0U )
{
	// The list structure contains a list item which is used to mark the
	// end of the list.  To initialise the list the list end is inserted
	// as the only list entry.
	m_last_item = new OS_List_Item();

	m_index = m_last_item;

	// The list end value is the highest possible value in the list to
	// ensure it remains at the end of the list.
	m_last_item->m_value = MAX_LIST_ITEM_VALUE_VALUE;

	// The list end next and previous pointers point to itself so we know
	// when the list is empty.
	m_last_item->m_next = m_last_item;
	m_last_item->m_previous = m_last_item;

	m_num_items = 0U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
OS_List::~OS_List( void )
{
	delete m_last_item;
}
