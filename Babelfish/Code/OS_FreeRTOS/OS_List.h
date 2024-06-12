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
		OS_List( void )
		{
			// This initialises all the members of the list structure and
			// inserts the xListEnd item into the list as a marker to the back
			// of the list.
			vListInitialise( &m_list );
		}

		/**
		 * @brief Gets pointer to the class object which contains us.
		 */
		~OS_List( void ){}

		/**
		 * @brief Adds an item to the end of the list.
		 * @param list_item: The item to be added.
		 * @note Breaks the sort order!
		 */
		void Add_To_End( OS_List_Item* list_item )
		{
			GINT_TDEF temp_gint = 0U;

			Push_GINT( temp_gint );

			// The item will be inserted in a position such that it will
			// be the last item within the list
			vListInsertEnd( &m_list, &list_item->m_list_item );

			Pop_GINT( temp_gint );
		}

		/**
		 * @brief Inserts item into list, with position sorted in ascending order.
		 * @param list_item: The item to be added.
		 */
		void Insert( OS_List_Item* list_item )
		{
			GINT_TDEF temp_gint = 0U;

			Push_GINT( temp_gint );

			// The item will be inserted into the list in a position
			// determined by its item value (descending item value order).
			vListInsert( &m_list, &list_item->m_list_item );

			Pop_GINT( temp_gint );
		}

		/**
		 * @brief Removes an item from the list.
		 * @param list_item: The item to be removed.
		 */
		void Remove( OS_List_Item* list_item )
		{
			GINT_TDEF temp_gint = 0U;

			Push_GINT( temp_gint );

			// The list item has a pointer to the list that it is in,
			// so only the list item need be passed into the function.
			uxListRemove( &list_item->m_list_item );

			Pop_GINT( temp_gint );
		}

		/**
		 * @brief Gets the owner of the item at the head of the list.
		 * @return Pointer to the owner, NULL if list is empty.
		 */
		inline void* Get_Head_Entry( void )
		{
			return ( ( m_list.uxNumberOfItems != 0 ) ? m_list.xListEnd.pxNext->pvOwner : NULL );
		}

		/**
		 * @brief Tests whether an item is contained in the list.
		 * @param list_item: The given item.
		 * @return True if contained, false otherwise.
		 */
		inline bool Contained_Within( OS_List_Item* list_item )
		{
			return ( listIS_CONTAINED_WITHIN( &m_list, &list_item->m_list_item ) );
		}

		/**
		 * @brief Tests whether an item is contained in any list.
		 * @param list_item: The given item.
		 * @return True if contained, false otherwise.
		 */
		inline bool Not_Contained( OS_List_Item* list_item )
		{
			return ( listLIST_ITEM_CONTAINER( &list_item->m_list_item ) == NULL );
		}

		/**
		 * @brief Tests whether list is empty.
		 * @return True if empty, false otherwise.
		 */
		inline bool Is_Empty( void )
		{
			return ( listLIST_IS_EMPTY( &m_list ) );
		}

		/**
		 * @brief Gets owner of next item.
		 * @return Next item.
		 */
		void* Get_Next( void )
		{
			m_list.pxIndex = m_list.pxIndex->pxNext;
			if ( reinterpret_cast<void*>( m_list.pxIndex ) == reinterpret_cast<void*>( &m_list.xListEnd ) )
			{
				m_list.pxIndex = m_list.pxIndex->pxNext;
			}
			return ( m_list.pxIndex->pvOwner );
		}

		/**
		 * @brief Gets a count of items in list.
		 * @return Number of items.
		 */
		inline BF_Lib::MBASE Get_List_Length( void )
		{
			return ( listCURRENT_LIST_LENGTH( &m_list ) );
		}

	private:

		/**
		 * @brief structure object of list.
		 */
		List_t m_list;
};

#endif
