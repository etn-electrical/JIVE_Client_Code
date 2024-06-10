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
		OS_List_Item( void );

		/**
		 * @brief Destroys an instance of the class.
		 */
		~OS_List_Item( void );

		/**
		 * @brief Gets pointer to the object which contains us (typically a list).
		 */
		inline void* Get_Container()
		{
			return ( m_container );
		}

		/**
		 * @brief Sets owner (typically a task).
		 * @param owner: Pointer to our owner.
		 */
		inline void Set_Owner( void* owner )
		{
			m_owner = owner;
		}

		/**
		 * @brief Gets item value.
		 */
		inline OS_LIST_VALUE_TD Get_Value()
		{
			return ( m_value );
		}

		/**
		 * @brief Sets item value.
		 */
		inline void Set_Value( OS_LIST_VALUE_TD value )
		{
			m_value = value;
		}

		/**
		 * @brief We are friends of OS_Lists.
		 */
		friend class OS_List;

	private:
		/**
		 * @brief Pointer to next item in list.
		 */
		volatile OS_List_Item* m_next;

		/**
		 * @brief Pointer to previous item in list.
		 */
		volatile OS_List_Item* m_previous;

		/**
		 * @brief Item value.
		 */
		OS_LIST_VALUE_TD m_value;

		/**
		 * @brief Pointer to owner (i.e. the task we belong to).
		 */
		void* m_owner;

		/**
		 * @brief Pointer to the list we are contained in.
		 */
		void* m_container;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		OS_List_Item( const OS_List_Item& object );
		OS_List_Item & operator =( const OS_List_Item& object );

		/**
		 * @brief Sets pointer to the object which contains us (typically a list).
		 * @param container: Given container.
		 * @note For unit test only!
		 */
		inline void Set_Container( void* container )
		{
			m_container = container;
		}

		/**
		 * @brief Gets owner (typically a task).
		 * @return Pointer to our owner.
		 * @note For unit test only!
		 */
		inline void* Get_Owner( void )
		{
			return ( m_owner );
		}

		/**
		 * @brief Gets pointer to next item.
		 * @note For unit test only!
		 */
		inline OS_List_Item* Get_Next( void )
		{
			return ( const_cast<OS_List_Item*>( m_next ) );
		}

		/**
		 * @brief Sets pointer to next item.
		 * @note For unit test only!
		 */
		inline void Set_Next( OS_List_Item* next )
		{
			m_next = next;
		}

		/**
		 * @brief Gets pointer to previous item.
		 * @note For unit test only!
		 */
		inline OS_List_Item* Get_Previous( void )
		{
			return ( const_cast<OS_List_Item*>( m_previous ) );
		}

		/**
		 * @brief Sets pointer to previous item.
		 * @note For unit test only!
		 */
		inline void Set_Previous( OS_List_Item* previous )
		{
			m_previous = previous;
		}

};

#endif
