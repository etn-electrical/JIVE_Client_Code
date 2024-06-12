/*
 ********************************************************************************
 *		$Workfile: Dynamic_In_Asm.h$
 *
 *		$Author: Pranav$
 *		$Date: 24/02/2014 12:18:38 PM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 ********************************************************************************
 */

#ifndef DYNAMIC_IN_ASSEMBLY_H
#define DYNAMIC_IN_ASSEMBLY_H

#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Change_Tracker.h"
#include "Shadow.h"
#include "EIP_DCI_Data.h"
#include "CIP_Asm.h"
#ifdef ESP32_SETUP
#include "Prod_Spec_LTK_ESP32.h"
#else
#include "Prod_Spec_LTK_STM32F.h"
#endif
#include "DynamicAssembly_DCI_Data.h"
#include "EIP_Service.h"
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
class Dynamic_In_Asm
{
	public:
		Dynamic_In_Asm( EIP_Service* m_EIP_Service );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Dynamic_In_Asm() {}

		void update_default_DCID_in_D_IN_ASMBuffer( void );

	// static const uint8_t D1_ASM = 0;
	// static const uint8_t D2_ASM = 1;
	// static const uint8_t D3_ASM = 2;
	// static const uint8_t D4_ASM = 3;
	// static const uint8_t D5_ASM = 4;
	// static const uint8_t D6_ASM = 5;
	// static const uint8_t D7_ASM = 6;
	// static const uint8_t D8_ASM = 7;
	// static const uint8_t D_IN_ASM_MAX=8;

	private:
		Dynamic_In_Asm( const Dynamic_In_Asm& object );
		Dynamic_In_Asm & operator =( const Dynamic_In_Asm& object );

		void Change_Track_Handler( DCI_ID_TD dci_id );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Change_Track_Handler_Static(
			Change_Tracker::cback_param_t handle,
			DCI_ID_TD dci_id,
			Change_Tracker::attrib_mask_t attribute_mask )
		{
			reinterpret_cast<Dynamic_In_Asm*>( handle )->Change_Track_Handler( dci_id );
		}

		DCI_CB_RET_TD Check_Terminator_And_Length_CBack(
			DCI_ACCESS_ST_TD* access_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Check_Terminator_And_Length_CBack_Static(
			DCI_CBACK_PARAM_TD handle, DCI_ACCESS_ST_TD* access_struct )
		{
			return ( ( ( Dynamic_In_Asm* )handle )->Check_Terminator_And_Length_CBack
						 ( access_struct ) );
		}

		void check_changed_assembly( DCI_ID_TD dci_id );

		void init_D_IN_ASM_Selection_owners( void );

		void TerminateAssembly( DCI_ID_TD dci_id );

		void update( DCI_ID_TD dci_id, uint16_t temp1uint16 );

		bool check_previous_entry( uint8_t array_number ) const;

		bool check_double_entry( DCI_ID_TD dci_id ) const;

		void Map_Dynamic_ID_To_DCID( uint16_t assembly_id,
									 DCI_ID_TD* dcid_mapped_to_assembly_id ) const;

		const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* Find_Dyn_In_Asm_DCID(
			DYNAMIC_ASSEMBLY_PID_TD assembly_id ) const;

		bool IN_TERMINATOR_PRESENT;
		uint16_t mapped_IN_ASM_id_to_dcid[D_IN_ASM_MAX];
		uint16_t default_dynamic_dcid[D_IN_ASM_MAX];
		DCI_ID_TD d_in_asm_dcid[D_IN_ASM_MAX];

		DCI_ID_TD terminated_dcid;
		DCI_SOURCE_ID_TD m_source_id;

		const D_IN_ASM_TARGET_INFO_ST_TD* m_assembly_profile;
		DCI_Patron* m_patron;
		Shadow* m_shadow_ctrl;
		Change_Tracker* m_change_tracker;


		DCI_Owner* m_d_in_Asm_Selection[D_IN_ASM_MAX];
		EIP_Service* m_dyn_EIP_Service;
		bool_t update_newDCID_in_DynamicAssemblyBuffer( DCI_ID_TD header_dci_id, DCI_ID_TD member_dci_id,
														uint8_t Assembly_offset );

		bool_t update_DefaultDCID_in_DynamicAssemblyBuffer( DCI_ID_TD header_dci_id, DCI_ID_TD member_dci_id,
															uint8_t Assembly_offset );

		void UnTrack_Default_dcid( DCI_ID_TD header_dci_id );

};

#endif
