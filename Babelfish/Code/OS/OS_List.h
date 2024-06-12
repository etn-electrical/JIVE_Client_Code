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
#ifndef OS_LIST_H
#define OS_LIST_H

#include "OS_List_Item.h"

// OS_List and OS_List_Item have to be protected externally (disabling
// interrupts, etc.)

/**
 * @brief Class represents a list of items. See OS_List_Item::.
 * @note OS_List and OS_List_Item have to be protected externally (disabling interrupts, etc.)
 */
class OS_List
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		OS_List( void );

		/**
		 * @brief Gets pointer to the class object which contains us.
		 */
		~OS_List( void );

		/**
		 * @brief Adds an item to the end of the list.
		 * @param list_item: The item to be added.
		 * @note Breaks the sort order!
		 */
		void Add_To_End( OS_List_Item* list_item )
		{
			GINT_TDEF temp_gint;

			Push_GINT( temp_gint );

			list_item->m_next = m_index->m_next;
			list_item->m_previous = m_index;
			m_index->m_next->m_previous = ( volatile OS_List_Item* )list_item;
			m_index->m_next = ( volatile OS_List_Item* )list_item;
			m_index = ( volatile OS_List_Item* )list_item;
			list_item->m_container = ( void* )this;
			m_num_items++;

			Pop_GINT( temp_gint );
		}

		/**
		 * @brief Inserts item into list, with position sorted in ascending order.
		 * @param list_item: The item to be added.
		 */
		void Insert( OS_List_Item* list_item )
		{
			volatile OS_List_Item* iterator;
			OS_LIST_VALUE_TD insertion_value;

			GINT_TDEF temp_gint;

			Push_GINT( temp_gint );

			insertion_value = list_item->m_value;

			if ( insertion_value == MAX_LIST_ITEM_VALUE_VALUE )
			{
				iterator = m_last_item->m_previous;
			}
			else
			{
				for ( iterator = m_last_item; iterator->m_next->m_value <= insertion_value;
					  iterator = iterator->m_next )
				{}
			}

			list_item->m_next = iterator->m_next;
			list_item->m_next->m_previous = ( volatile OS_List_Item* )list_item;
			list_item->m_previous = iterator;
			iterator->m_next = ( volatile OS_List_Item* )list_item;

			m_num_items++;
			list_item->m_container = ( void* )this;

			Pop_GINT( temp_gint );
		}

		/**
		 * @brief Removes an item from the list.
		 * @param list_item: The item to be removed.
		 */
		void Remove( OS_List_Item* list_item )
		{
			GINT_TDEF temp_gint;

			Push_GINT( temp_gint );

			if ( list_item->m_container == this )
			{
				list_item->m_next->m_previous = list_item->m_previous;
				list_item->m_previous->m_next = list_item->m_next;

				if ( m_index == list_item )
				{
					m_index = list_item->m_previous;
				}

				m_num_items--;
				list_item->m_container = NULL;
			}

			Pop_GINT( temp_gint );
		}

		/**
		 * @brief Gets the owner of the item at the head of the list.
		 * @return Pointer to the owner, NULL if list is empty.
		 */
		inline void* Get_Head_Entry( void )
		{
			return ( ( m_num_items != 0 ) ? m_last_item->m_next->m_owner : NULL );
		}

		/**
		 * @brief Tests whether an item is contained in the list.
		 * @param list_item: The given item.
		 * @return True if contained, false otherwise.
		 */
		inline bool Contained_Within( OS_List_Item* list_item )
		{
			return ( list_item->m_container == this );
		}

		/**
		 * @brief Tests whether an item is contained in any list.
		 * @param list_item: The given item.
		 * @return True if contained, false otherwise.
		 */
		inline bool Not_Contained( OS_List_Item* list_item )
		{
			return ( list_item->m_container == NULL );
		}

		/**
		 * @brief Tests whether list is empty.
		 * @return True if empty, false otherwise.
		 */
		inline bool Is_Empty( void )
		{
			return ( m_num_items == 0 );
		}

		/**
		 * @brief Gets owner of next item.
		 * @return Next item.
		 */
		void* Get_Next( void )
		{
			m_index = m_index->m_next;
			if ( m_index == m_last_item )
			{
				m_index = m_index->m_next;
			}
			return ( m_index->m_owner );
		}

		/**
		 * @brief Gets a count of items in list.
		 * @return Number of items.
		 */
		inline BF_Lib::MBASE Get_List_Length( void )
		{
			return ( m_num_items );
		}

	private:

		/**
		 * @brief Pointer to current item in list.
		 */
		volatile OS_List_Item* m_index;

		/**
		 * @brief Pointer to last item in list.
		 */
		volatile OS_List_Item* m_last_item;

		/**
		 * @brief Number of items in list.
		 */
		volatile BF_Lib::MBASE m_num_items;
};

#endif
