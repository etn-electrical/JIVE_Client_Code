/*
 ********************************************************************************
 *		$Workfile: Dynamic_Out_Asm.h$
 *
 *		$Author: Pranav$
 *		$Date: 24/02/2014 12:18:38 PM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 ********************************************************************************
 */

#ifndef DYNAMIC_OUT_ASSEMBLY_H
#define DYNAMIC_OUT_ASSEMBLY_H

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
class Dynamic_out_Asm
{
	public:
		Dynamic_out_Asm( void );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Dynamic_out_Asm() {}

		// static const uint8_t D1_ASM = 0U;
		// static const uint8_t D2_ASM = 1U;
		// static const uint8_t D3_ASM = 2U;
		// static const uint8_t D4_ASM = 3U;
		// static const uint8_t D_OUT_ASM_MAX=4U;

		void update_default_DCID_in_D_OUT_ASMBuffer( void );

		DCI_ID_TD Return_DCID( uint8_t index ) const;

	private:
		Dynamic_out_Asm ( Dynamic_out_Asm& object );
		Dynamic_out_Asm & operator =( Dynamic_out_Asm& object );

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
			reinterpret_cast<Dynamic_out_Asm*>( handle )->Change_Track_Handler( dci_id );
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
			return ( ( ( Dynamic_out_Asm* )handle )->Check_Terminator_And_Length_CBack
						 ( access_struct ) );
		}

		void check_changed_assembly( DCI_ID_TD dci_id );

		void init_D_OUT_ASM_Selection_owners( void );

		void TerminateAssembly( DCI_ID_TD dci_id );

		void update( DCI_ID_TD dci_id, uint16_t temp1uint16 );

		// bool check_previous_entry(uint8_t array_number);
		bool check_double_entry( DCI_ID_TD dci_id, uint8_t array_number );

		void Map_Dynamic_ID_To_DCID( DCI_ID_TD assembly_id,
									 DCI_ID_TD* dcid_mapped_to_assembly_id ) const;

		const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* Find_Dyn_Out_Asm_DCID(
			DYNAMIC_ASSEMBLY_PID_TD assembly_id ) const;

		bool OUT_TERMINATOR_PRESENT;
		DCI_ID_TD mapped_OUT_ASM_id_to_dcid[D_OUT_ASM_MAX];
		uint16_t default_dynamic_dcid[D_OUT_ASM_MAX];
		DCI_ID_TD d_out_asm_dcid[D_OUT_ASM_MAX];

		DCI_ID_TD terminated_dcid;
		DCI_SOURCE_ID_TD m_source_id;

		const D_OUT_ASM_TARGET_INFO_ST_TD* m_assembly_profile;
		DCI_Patron* m_patron;
		Shadow* m_shadow_ctrl;
		Change_Tracker* m_change_tracker;
		DCI_Owner* m_d_out_Asm_Selection[D_OUT_ASM_MAX];

};

#endif
