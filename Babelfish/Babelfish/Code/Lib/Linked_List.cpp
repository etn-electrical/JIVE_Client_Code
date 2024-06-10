/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Linked_List.h"
#include "Ram.h"

namespace BF_Lib
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Linked_List::Linked_List( void ) :
	m_start_node( reinterpret_cast<Linked_List_NODE*>( nullptr ) ),
	m_present_node( m_start_node ),
	m_list_length( 0U )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Linked_List::~Linked_List( void )
{
	Restart();

	while ( m_list_length > 0U )
	{
		Remove( Get_And_Inc() );
	}

	m_start_node = reinterpret_cast<Linked_List_NODE*>( nullptr );
	m_present_node = reinterpret_cast<Linked_List_NODE*>( nullptr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Linked_List::Add( LL_PAYLOAD payload )
{
	Linked_List_NODE* new_node;

	new_node = reinterpret_cast<Linked_List_NODE*>
		( Ram::Allocate( sizeof( Linked_List_NODE ) ) );
	new_node->next = reinterpret_cast<Linked_List_NODE*>( nullptr );
	new_node->payload = payload;

	m_list_length++;

	if ( m_start_node == nullptr )
	{
		m_start_node = new_node;
		m_present_node = new_node;
	}
	else
	{
		Linked_List_NODE* walker;

		walker = m_start_node;

		while ( walker->next != nullptr )
		{
			walker = walker->next;
		}

		walker->next = new_node;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Linked_List::Remove( LL_PAYLOAD payload )
{
	Linked_List_NODE* walker;
	Linked_List_NODE* node_to_delete;

	if ( ( m_start_node != nullptr ) &&
		 ( payload != nullptr ) )
	{
		if ( m_present_node->payload == payload )
		{
			Next();
		}

		walker = m_start_node;

		if ( walker->payload == payload )
		{
			node_to_delete = walker;
			m_start_node = walker->next;
		}
		else
		{
			while ( ( walker->next != nullptr ) &&
					( walker->next->payload != payload ) )
			{
				walker = walker->next;
			}

			node_to_delete = walker->next;
			if ( walker->next != nullptr )
			{
				walker->next = walker->next->next;
			}
		}

		if ( node_to_delete != nullptr )
		{
			if ( ( node_to_delete->payload == payload ) )
			{
				Ram::De_Allocate( node_to_delete );
				m_list_length--;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Linked_List::Restart( void )
{
	m_present_node = m_start_node;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Linked_List::LL_PAYLOAD Linked_List::Get_And_Inc( void )
{
	LL_PAYLOAD return_payload;

	if ( m_present_node != nullptr )
	{
		return_payload = m_present_node->payload;
		m_present_node = m_present_node->next;
	}
	else
	{
		return_payload = reinterpret_cast<LL_PAYLOAD>( nullptr );
	}

	return ( return_payload );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Linked_List::LL_PAYLOAD Linked_List::Get( void ) const
{
	LL_PAYLOAD return_payload;

	if ( m_present_node != nullptr )
	{
		return_payload = m_present_node->payload;
	}
	else
	{
		return_payload = reinterpret_cast<LL_PAYLOAD>( nullptr );
	}

	return ( return_payload );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void BF_Lib::Linked_List::Next( void )
{
	if ( m_present_node != nullptr )
	{
		m_present_node = m_present_node->next;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t BF_Lib::Linked_List::List_Length( void ) const
{
	return ( m_list_length );
}

}	/* end namespace BF_Lib for this module */
