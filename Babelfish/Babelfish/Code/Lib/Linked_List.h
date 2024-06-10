/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains implementation of the Linked List, which is basically used
 *	to store the collection of the data.
 *
 *	@details This is the Linked_List class with functionalities on working with
 *	linked list.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef Linked_List_H
#define Linked_List_H

namespace BF_Lib
{
/**
 ****************************************************************************************
 * @brief This is the Linked_List class with functionalities on working with
 * linked list.
 * @details It contains the implementations to add and remove the node,
 * to restart the linked list based on the node address, to get the address
 * of the present node and the length of the list.
 * @n @b Usage: Use of Linked List has certain advantages over the array, which
 * allocates memory for all its elements together whereas a linked list
 * allocates space for each element called 'node' separately in a block.
 ****************************************************************************************
 */
class Linked_List
{
	public:

		/**
		 *  This is the type alias for void pointer, used for the address of the data
		 *  element inside a node.
		 */
		typedef void* LL_PAYLOAD;

		/**
		 *  @brief Constructor
		 *  @details Initializes the start node, the present node and the linked list length to zero.
		 *  @return None
		 */
		Linked_List( void );

		/**
		 *  @brief Destructor for the Linked_List Module.
		 *  @details This will get called when object of Linked_List goes out of scope.
		 */
		~Linked_List( void );

		/**
		 *  @brief Adds the node to the end of the linked list.
		 *  @details As the node element is added, the list length is increased.
		 *  Depending on the address of the node, if it is null it becomes the start node.
		 *  If it is not null, new node is assigned to the next pointer of the last node.
		 *  @return None
		 */
		void Add( LL_PAYLOAD payload );

		/**
		 *  @brief Removes the node from the linked list.
		 *  @details If the start node and its data element address passed is not null, depending on the
		 *  address of the data of the node, the node to be deleted is decided.
		 *  As the node element is deleted, the list length is decreased and the memory is deallocated.
		 *  @return None
		 */
		void Remove( LL_PAYLOAD payload );

		/**
		 *  @brief This is used to restart the Linked List.
		 *  @details Assigns the start node address to the present node.
		 *  @return None
		 */
		void Restart( void );

		/**
		 *  @brief
		 *  @details This is used to get the value of the present node.
		 *  @return Pointer to the address of the present node.
		 */
		LL_PAYLOAD Get( void ) const;

		/**
		 *  @brief
		 *  @details Simply asks the present node to be pointing to the next.
		 *  @return None
		 */
		void Next( void );

		/**
		 *  @brief
		 *  @details It provides the address to the data of the last node and
		 *  assigns the next node to be the present one.
		 *  @return Pointer to the address of the last node.
		 */
		LL_PAYLOAD Get_And_Inc( void );

		/**
		 *  @brief The linked list length.
		 *  @details Calculated while adding nodes and decremented while removing.
		 *  @return The length of the linked list.
		 */
		uint16_t List_Length( void ) const;

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Linked_List( const Linked_List& rhs );
		Linked_List & operator =( const Linked_List& object );

		/**
		 *  This is the linked list node, it contains the pointer to the next node in the list and
		 *  the data element address.
		 */
		typedef struct Linked_List_NODE
		{
			Linked_List_NODE* next;
			LL_PAYLOAD payload;
		} Linked_List_NODE;

		/// The first node of the linked list.
		Linked_List_NODE* m_start_node;

		/// The present node of the linked list.
		Linked_List_NODE* m_present_node;

		/// The length of the linked list, calculated while Adding nodes and decremented while removing.
		uint16_t m_list_length;
};

}

#endif	/* Linked_List_H */
