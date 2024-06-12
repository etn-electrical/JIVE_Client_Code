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
#ifndef OS_LIST_ITEM_H
#define OS_LIST_ITEM_H

#include "uC_OS_Device_Defines.h"
#include "FreeRTOS.h"
#include "list.h"

/**
 * @brief Type of value held by list item.
 */
typedef OS_TICK_TD OS_LIST_VALUE_TD;

/**
 * @brief Maximum value which can be represented by OS_TICK_TD type.
 */
#define MAX_LIST_ITEM_VALUE_VALUE       ( ~( static_cast<OS_LIST_VALUE_TD>( 0U ) ) )

/**
 * @brief Class represents an item which is contained in a list. See OS_List::.
 * @note OS_List and OS_List_Item have to be protected externally (disabling interrupts, etc.)
 */
class OS_List_Item
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 */
		OS_List_Item( void )
		{
			// This sets the list container to null so the item does not think
			// that it is already contained in a list.
			vListInitialiseItem( &m_list_item );
		}

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_List_Item( void ){}

		/**
		 * @brief Gets pointer to the object which contains us (typically a list).
		 */
		inline void* Get_Container()
		{
			return ( listLIST_ITEM_CONTAINER( &m_list_item ) );
		}

		/**
		 * @brief Sets owner (typically a task).
		 * @param owner: Pointer to our owner.
		 */
		inline void Set_Owner( void* owner )
		{
			listSET_LIST_ITEM_OWNER( &m_list_item, owner );
		}

		/**
		 * @brief Gets item value.
		 */
		inline OS_LIST_VALUE_TD Get_Value()
		{
			return ( listGET_LIST_ITEM_VALUE( &m_list_item ) );
		}

		/**
		 * @brief Sets item value.
		 */
		inline void Set_Value( OS_LIST_VALUE_TD value )
		{
			listSET_LIST_ITEM_VALUE( &m_list_item, value );
		}

		/**
		 * @brief We are friends of OS_Lists.
		 */
		friend class OS_List;

	private:

		/**
		 * @brief structure object of ListItem_t.
		 */
		ListItem_t m_list_item;

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		OS_List_Item( const OS_List_Item& object );
		OS_List_Item & operator =( const OS_List_Item& object );

};

#endif
