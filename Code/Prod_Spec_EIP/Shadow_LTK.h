/**
 *****************************************************************************************
 * @file       Shadow_LTK.cpp
 * @details    This file contains declaration and Macros related to EIP.
 *             This file is not fully implemented.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */


#ifndef SHADOW_LTK_H
#define SHADOW_LTK_H

#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Change_Tracker.h"
#include "Shadow.h"
#include "Shadow_Product.h"
#include "Com_Dog.h"
#include "Prod_Spec_LTK_ESP32.h"
/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------
 */
class Shadow_LTK : public Shadow_Product
{
	public:
		/**
		 **********************************************************************************************
		 * @brief                   Constructor to create instance of EIP_Service
		 * @param[in] none          none
		 * @return[out]             none
		 * @n
		 **********************************************************************************************
		 */
		Shadow_LTK( void );
		/**
		 **********************************************************************************************
		 * @brief                   Destructor to delete instance of Shadow_LTK
		 * @param[in] none          none
		 * @return[out]             none
		 * @n
		 **********************************************************************************************
		 */
		~Shadow_LTK( void )
		{
			delete      m_patron;
			delete      m_shadow_ctrl;
			delete      m_change_tracker;
		}

		Shadow* m_shadow_ctrl;

	private:

		/**
		 **********************************************************************************************
		 * @brief                   Copy Constructor to copy instance of EIP_Service
		 * @param[in] object        object to copy
		 * @return[out]             none
		 * @n
		 **********************************************************************************************
		 */
		Shadow_LTK( const Shadow_LTK& object );
		Shadow_LTK & operator =( const Shadow_LTK& object );

		/**
		 **********************************************************************************************
		 * @brief                    Change track handler
		 * @param[in] dci_id         object to copy
		 * @param[in] attribute_mask attribute_mask
		 * @return[out]              none
		 * @note                     empty body
		 **********************************************************************************************
		 */
		void Change_Track_Handler( DCI_ID_TD dci_id,
								   Change_Tracker::attrib_mask_t attribute_mask );

		/**
		 **********************************************************************************************
		 * @brief                    Change track handler static callback
		 * @param[in] handle         callback handle
		 * @param[in] dci_id         dci_id
		 * @param[in] attribute_mask attribute_mask
		 * @return[out]              none
		 * @note
		 **********************************************************************************************
		 */
		static void Change_Track_Handler_Static(
			Change_Tracker::cback_param_t handle,
			DCI_ID_TD dci_id,
			Change_Tracker::attrib_mask_t attribute_mask )
		{
			reinterpret_cast<Shadow_LTK*>( handle )->Change_Track_Handler(
				dci_id,
				attribute_mask );
		}

		DCI_SOURCE_ID_TD m_source_id;
		DCI_Patron* m_patron;
		Change_Tracker* m_change_tracker;

};
#endif
