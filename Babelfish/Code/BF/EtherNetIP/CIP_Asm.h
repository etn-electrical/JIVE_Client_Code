/*
 *****************************************************************************************
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 2/27/2008 5:08:46 PM$
 *		Copyright © Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *****************************************************************************************
 */
#ifndef CIP_ASM_H
	#define CIP_ASM_H

#include "EtherNetIP_DCI.h"
#include "DCI_Patron.h"
#include "Change_Tracker.h"
#include "eipinc.h"
#include "EIP_Display.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
static const uint8_t CIP_ASM_ALIGN_8BIT = 1U;
static const uint8_t CIP_ASM_ALIGN_16BIT = 2U;
static const uint8_t CIP_ASM_ALIGN_32BIT = 4U;
static const DCI_LENGTH_TD LENGTH_ONE_BYTE = 1;
static const DCI_LENGTH_TD LENGTH_TWO_BYTE = 2;
static const DCI_LENGTH_TD LENGTH_THREE_BYTE = 3;
static const DCI_LENGTH_TD LENGTH_FOUR_BYTE = 4;
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class CIP_Asm
{
	public:
		CIP_Asm( CIP_ASM_STRUCT_TD const* asm_list,
				 uint16_t asm_list_len,
				 uint8_t alignment = CIP_ASM_ALIGN_16BIT );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~CIP_Asm() {}

		static const uint8_t D_OUT_INSTANCE = 151U;
		bool_t BOOL_DEFAULT_INITIALISED;
		void Initialize( void );// this must happen after the stack is up and running.

		void Process_RXed_Assembly( uint32_t instance );

		bool_t Dynamic_Change_Track( DCI_ID_TD header_dci_id,
									 DCI_ID_TD member_dci_id,
									 uint8_t Assembly_offset );

		bool_t Dynamic_Change_Track_Init( DCI_ID_TD header_dci_id,
										  DCI_ID_TD member_dci_id,
										  uint8_t Assembly_offset );

		void UnTrack_ID( DCI_ID_TD default_dci_id );

		typedef void (* CIP_ASM_SVC_CALLBACK)( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Set_Update_Dynamic_Asm_Callback( CIP_ASM_SVC_CALLBACK pCallback )
		{
			m_update_default_dynamic_asm_callback = pCallback;
		}

	private:

		CIP_Asm( const CIP_Asm& object );
		CIP_Asm & operator =( const CIP_Asm& object );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Access_Handler_Static(
			DCI_CBACK_PARAM_TD cback_param,
			DCI_ACCESS_ST_TD* access_struct )
		{ return ( ( ( CIP_Asm* )cback_param )->Access_Handler( access_struct ) ); }

		DCI_CB_RET_TD Access_Handler( DCI_ACCESS_ST_TD* access_struct );


		void Change_Track_Handler( DCI_ID_TD dci_id );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Change_Track_Handler_Static(
			Change_Tracker::cback_param_t param,
			DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask )
		{ ( ( CIP_Asm* )param )->Change_Track_Handler( dci_id ); }

		CIP_ASM_STRUCT_TD const* Find_ASM_From_DCID( DCI_ID_TD dci_id ) const;

		bool_t Check_RxData_Range( DCI_LENGTH_TD dcid_len2,
								   uint32_t max_d_out_data ) const;

		DCI_SOURCE_ID_TD m_src_id;
		uint16_t m_max_asm_size;
		uint16_t m_asm_list_len;

		uint8_t* m_incoming_assembly_scratch;
		uint8_t* m_outgoing_assembly_scratch;

		DCI_Patron* m_patron;
		Change_Tracker* m_change_tracker;
		DCI_Owner* m_dynamic_owner;
		CIP_ASM_STRUCT_TD const* m_asm_list;
		CIP_ASM_STRUCT_TD const* m_end_asm_struct;

		static CIP_ASM_SVC_CALLBACK m_update_default_dynamic_asm_callback;
		// DCI_ID_TD Return_Mapped_DCID( uint8_t index );
};

#endif
