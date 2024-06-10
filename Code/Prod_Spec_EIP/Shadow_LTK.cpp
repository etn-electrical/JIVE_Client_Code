/**
 *****************************************************************************************
 * @file       Shadow_LTK.cpp
 * @details    This file contains declaration and Macros related to EIP
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "includes.h"
#include "Shadow_LTK.h"
#include "Stdlib_MV.h"
#include "System_Reset.h"
#include "Services_List_App.h"
#include "Services.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "DCI.h"
#include "DCI_Defines.h"

/*
 ********************************************************************************
 *		Macros and Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Shadow_LTK::Shadow_LTK( void ) :
	m_source_id( 0U ),
	m_patron( reinterpret_cast<DCI_Patron*>( NULL ) ),
	m_shadow_ctrl( reinterpret_cast<Shadow*>( NULL ) ),
	m_change_tracker( reinterpret_cast<Change_Tracker*>( NULL ) )
{
	m_patron = new DCI_Patron( false );
	m_source_id = DCI_SOURCE_IDS_Get();

	m_shadow_ctrl = new Shadow( m_patron, m_source_id );

	m_change_tracker = new Change_Tracker( &Change_Track_Handler_Static,
										   reinterpret_cast<Change_Tracker::cback_param_t>( this ),
										   true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Shadow_LTK::Change_Track_Handler( DCI_ID_TD dci_id,
									   Change_Tracker::attrib_mask_t attribute_mask )
{}
