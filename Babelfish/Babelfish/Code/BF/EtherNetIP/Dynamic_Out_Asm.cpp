/*
 ********************************************************************************
 *		$Workfile: Dynamic_out_Asm.cpp$
 *
 *		$Author: Pranav$
 *		$Date: 26/05/2014 12:18:38 PM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 ********************************************************************************
 */
#include "includes.h"
#include "Stdlib_MV.h"
// #include "vintage_stm32_eth.h"
#include "Dynamic_Out_Asm.h"

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


/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::Dynamic_out_Asm
**  Created By:      Pranav Kodre
**  Date Created:    5-26-2014
**
**  Description:
**                  Constructor
**  Inputs:
**                  None
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/

Dynamic_out_Asm::Dynamic_out_Asm() :
	OUT_TERMINATOR_PRESENT( false ),
	terminated_dcid( 0U ),
	m_source_id( 0U ),
	m_assembly_profile( reinterpret_cast<D_OUT_ASM_TARGET_INFO_ST_TD*>( NULL ) ),
	m_patron( reinterpret_cast<DCI_Patron*>( NULL ) ),
	m_shadow_ctrl( reinterpret_cast<Shadow*>( NULL ) ),
	m_change_tracker( reinterpret_cast<Change_Tracker*>( NULL ) )
{
	OUT_TERMINATOR_PRESENT = false;
	terminated_dcid = 0U;
	m_patron = new DCI_Patron( false );
	m_source_id = DCI_SOURCE_IDS_Get();

	m_shadow_ctrl = new Shadow( m_patron, m_source_id );
	m_change_tracker = new Change_Tracker( &Change_Track_Handler_Static,
										   reinterpret_cast<Change_Tracker::cback_param_t>( this ),
										   true );

	m_assembly_profile = &d_out_asm_dci_data_target_info;
	init_D_OUT_ASM_Selection_owners();
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::init_D_OUT_ASM_Selection_owners
**  Created By:      Pranav Kodre
**  Date Created:    5-26-2014
**
**  Description:
**                  Creats the owner of Dynamic o/p Assembly Interface object's
**                  attributes, fetches the default ASM IDs from FRAM,
**                  finds the corresponding DCIDs & stores them in
**                  mapped ASM ID to DCID Array.
**  Inputs:
**                  None
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/

void Dynamic_out_Asm::init_D_OUT_ASM_Selection_owners( void )
{
	uint8_t temp_uint8;

	// new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_1_DCID);
	// new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_2_DCID);
	// new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_3_DCID);
	// new DCI_Owner( DCI_DYN_OUT_ASM_MEMBER_4_DCID);
	//
	// d_out_asm_dcid[D1_ASM] = DCI_DYN_OUT_ASM_SELECT_1_DCID;
	// d_out_asm_dcid[D2_ASM] = DCI_DYN_OUT_ASM_SELECT_2_DCID;
	// d_out_asm_dcid[D3_ASM] = DCI_DYN_OUT_ASM_SELECT_3_DCID;
	// d_out_asm_dcid[D4_ASM] = DCI_DYN_OUT_ASM_SELECT_4_DCID;

	init_dyn_output_asm_dcids( d_out_asm_dcid, m_d_out_Asm_Selection );

	for ( temp_uint8 = 0U; temp_uint8 < D_OUT_ASM_MAX; temp_uint8++ )
	{
		default_dynamic_dcid[temp_uint8] =
			DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST[temp_uint8];

		// m_d_out_Asm_Selection[temp_uint8]=
		// new DCI_Owner(d_out_asm_dcid[temp_uint8]);
		m_d_out_Asm_Selection[temp_uint8]->Attach_Callback(
			&Check_Terminator_And_Length_CBack_Static,
			reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
			static_cast<uint32_t>( DCI_ACCESS_SET_RAM_CMD_MSK ) );

		m_change_tracker->Track_ID( d_out_asm_dcid[temp_uint8] );

		mapped_OUT_ASM_id_to_dcid[temp_uint8] = default_dynamic_dcid[temp_uint8];
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::update_default_DCID_in_D_OUT_ASMBuffer
**  Created By:      Pranav Kodre
**  Date Created:    5-26-2014
**
**  Description:
**                  Creats the owner of Dynamic o/p Assembly Interface object's
**                  attributes, fetches the default ASM IDs from FRAM,
**                  finds the corresponding DCIDs & stores them in
**                  mapped ASM ID to DCID Array.
**  Inputs:
**                  None
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/

void Dynamic_out_Asm::update_default_DCID_in_D_OUT_ASMBuffer( void )
{
	DCI_ID_TD temp1uint16;
	DCI_ID_TD current_assembly_number[D_OUT_ASM_MAX] = {0U};

	for ( temp1uint16 = 0U; temp1uint16 < D_OUT_ASM_MAX; temp1uint16++ )
	{
		m_shadow_ctrl->Get_DCID_Ram_Data(
			d_out_asm_dcid[temp1uint16],
			&current_assembly_number[temp1uint16],
			static_cast<DCI_LENGTH_TD>(
				sizeof( default_dynamic_dcid[temp1uint16] ) ) );

		if ( !OUT_TERMINATOR_PRESENT )
		{
			if ( current_assembly_number[temp1uint16] == 0U )
			{
				terminated_dcid = d_out_asm_dcid[temp1uint16];
				OUT_TERMINATOR_PRESENT = true;
			}
		}
		Map_Dynamic_ID_To_DCID( current_assembly_number[temp1uint16],
								&mapped_OUT_ASM_id_to_dcid[temp1uint16] );
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::Map_Dynamic_ID_To_DCID
**  Created By:      Pranav Kodre
**  Date Created:    5-9-2014
**
**  Description:
**                  It finds the DCID from the Dynamic ID
**  Inputs:
**                  ASM ID, Pointer where Corresponding DCID will be stored
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_out_Asm::Map_Dynamic_ID_To_DCID( DCI_ID_TD assembly_id,
											  DCI_ID_TD* dcid_mapped_to_assembly_id ) const
{
	const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* D_OUT_ASM_struct;

	D_OUT_ASM_struct =
		Find_Dyn_Out_Asm_DCID( static_cast<DYNAMIC_ASSEMBLY_PID_TD>( assembly_id ) );

	if ( D_OUT_ASM_struct != LOOKUP_D_OUT_ASM_TO_DCI_NOT_FOUND )
	{
		*dcid_mapped_to_assembly_id = D_OUT_ASM_struct->dcid;
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::Return_DCID
**  Created By:      Pranav Kodre
**  Date Created:    5-9-2014
**
**  Description:
**                  It finds the DCID from the Dynamic ID
**  Inputs:
**                  Assembly Index
**  Outputs:
**                  DCID stored at that index in mapped ASM ID to DCID Array
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
DCI_ID_TD Dynamic_out_Asm::Return_DCID( uint8_t index ) const
{
	return ( mapped_OUT_ASM_id_to_dcid[index] );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::Find_Dyn_In_Asm_DCID
**  Created By:      Pranav Kodre
**  Date Created:    5-9-2014
**
**  Description:
**                  It searches the received Assembly from the Assembly List.
**  Inputs:
**                  None
**  Outputs:
**                  structure: searched structure if it's present
**                             or structure indication that structure not found.
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* Dynamic_out_Asm::Find_Dyn_Out_Asm_DCID( DYNAMIC_ASSEMBLY_PID_TD assembly_id )
const
{
	const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* search_struct;
	const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* return_value =
		LOOKUP_D_OUT_ASM_TO_DCI_NOT_FOUND;
	uint_fast16_t begin = 0U;
	uint_fast16_t end;
	uint_fast16_t middle = 0U;

	search_struct = m_assembly_profile->asmout_to_dcid;
	end = static_cast<uint_fast16_t>(
		m_assembly_profile->total_d_in_assemblies ) - 1U;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1U;
		if ( search_struct[middle].assembly_id < assembly_id )
		{
			begin = middle + 1U;
		}
		else if ( search_struct[middle].assembly_id > assembly_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				// We need to exit out here.  Set Begin to end + 1 to exit.
				// We did not find what we were looking for.
				begin = end + 1U;
			}
		}
		else
		{
			// We need to exit out here.  Set Begin to end + 1 to exit.
			return_value = ( &search_struct[middle] );
			begin = end + 1U;
		}
	}

	// only one return value as part of MISRA code changes
	return ( return_value );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::Change_Track_Handler
**  Created By:      Pranav Kodre
**  Date Created:    6-2-2014
**
**  Description:
**                  Triggered by respective assigned DCIDs when they
**                  are modified.
**                  This function checks that whether a new Dynamic ID is
**                  inserted as a member of
**                  Dynamic o/p Assembly & calls the related functions.
**  Inputs:
**                  dci_id
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/

void Dynamic_out_Asm::Change_Track_Handler( DCI_ID_TD dci_id )
{
	check_changed_assembly( dci_id );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::check_changed_assembly
**  Created By:      Pranav Kodre
**  Date Created:    6-2-2014
**
**  Description:
**                  It identifies the DCID from the Dynamic ASM ID.
**                  Also identified that whether an Assembly TERMINATOR
**                  is inserted or not & then calls the related functions.
**  Inputs:
**                  dci_id: New inserted DCID
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_out_Asm::check_changed_assembly( DCI_ID_TD dci_id )
{
	uint16_t temp1uint16 = 0U;
	uint16_t temp2uint16 = 0U;

	m_shadow_ctrl->Get_DCID_Ram_Data(
		dci_id,
		&temp2uint16,
		static_cast<DCI_LENGTH_TD>( sizeof( temp1uint16 ) ) );

	Map_Dynamic_ID_To_DCID( temp2uint16, &temp1uint16 );

	if ( temp1uint16 != DCI_DYN_OUT_ASM_TERMINATOR_DCID )
	{
		update( dci_id, temp1uint16 );
	}
	else
	{
		TerminateAssembly( dci_id );
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::TerminateAssembly
**  Created By:      Pranav Kodre
**  Date Created:    6-2-2014
**
**  Description:
**                  It inserts zero values from TERMINATED index to last index
**                  of Dynamic o/p Assembly in mapped ASM ID to DCID Array
**                  & updates the correspond values in FRAM with Terminator
**  Inputs:
**                  dci_id: terminated_dcid.
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_out_Asm::TerminateAssembly( DCI_ID_TD dci_id )
{
	DCI_ID_TD terminator_dcid_value = 0U;
	uint8_t temp1_uint8 = 0U;
	uint8_t temp2_uint8 = 0U;

	if ( !OUT_TERMINATOR_PRESENT )
	{
		terminated_dcid = dci_id;
		OUT_TERMINATOR_PRESENT = true;

		m_shadow_ctrl->Get_DCID_Ram_Data(
			DCI_DYN_OUT_ASM_TERMINATOR_DCID,
			&terminator_dcid_value,
			static_cast<DCI_LENGTH_TD>(
				sizeof( terminator_dcid_value ) ) );

		for ( temp1_uint8 = 0U; temp1_uint8 < D_OUT_ASM_MAX; temp1_uint8++ )
		{
			if ( dci_id == d_out_asm_dcid[temp1_uint8] )
			{
				for ( temp2_uint8 = temp1_uint8; temp2_uint8 < D_OUT_ASM_MAX; temp2_uint8++ )
				{
					mapped_OUT_ASM_id_to_dcid[temp2_uint8] =
						DCI_DYN_OUT_ASM_TERMINATOR_DCID;

					m_d_out_Asm_Selection[temp2_uint8]->Check_In( terminator_dcid_value );
					m_d_out_Asm_Selection[temp2_uint8]->Load_Current_To_Init();
				}
			}
		}
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::update
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
**
**  Description:
**                  It updates the new inserted DCID into
**                  mapped ASM ID to DCID Array
**  Inputs:
**                  dcid: An index number whose value is modified.
**                  temp1uint16: DCID of new inserted Dynamic ID
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_out_Asm::update( DCI_ID_TD dci_id, uint16_t temp1uint16 )
{
	uint8_t temp1_uint8;

	for ( temp1_uint8 = 0U; temp1_uint8 < D_OUT_ASM_MAX; temp1_uint8++ )
	{
		if ( dci_id == d_out_asm_dcid[temp1_uint8] )
		{
			mapped_OUT_ASM_id_to_dcid[temp1_uint8] = temp1uint16;
			break;
		}
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_out_Asm::Check_Terminator_And_Length_CBack
**  Created By:      Pranav Kodre
**  Date Created:    6-3-2014
**
**  Description:
**                  It checks that whether new inserted Dynamic ID length
**                  is not more than 32 bits & returns the appropriate flag.
**                  It also checks that whether modified index is ahead of
**                  TERMINATED index or not & returns the appropriate flag.
**  Inputs:
**                  dcid: An index number whose value is modified.
**                  temp1uint16: DCID of new inserted Dynamic ID
**  Outputs:
**                  Flag: further Processing or No further Processing
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
DCI_CB_RET_TD Dynamic_out_Asm::Check_Terminator_And_Length_CBack(
	DCI_ACCESS_ST_TD* access_struct )
{
	bool_t RETURN_FLAG = false;
	DCI_ID_TD received_dcid = 0U;
	DCI_ID_TD* received_asm_id;
	DCI_ID_TD modified_selection_dcid;
	DCI_LENGTH_TD received_dcid_length = 0U;
	DCI_CB_RET_TD return_status =
		static_cast<uint8_t>( DCI_CBACK_RET_NO_FURTHER_PROCESSING );

	modified_selection_dcid = access_struct->data_id;
	received_asm_id = static_cast<DCI_ID_TD*>( access_struct->data_access.data );

	Map_Dynamic_ID_To_DCID( *received_asm_id,
							&received_dcid );

	m_patron->Get_Length( received_dcid, &received_dcid_length );

	if ( ( OUT_TERMINATOR_PRESENT ) &&
		 ( ( modified_selection_dcid ) > terminated_dcid ) )
	{
		switch ( access_struct->command )
		{
			case DCI_ACCESS_SET_RAM_CMD:
			case DCI_ACCESS_SET_INIT_CMD:
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to
				 * unsigned short
				 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve
				 * casting pointers, not enums.
				 */
				/*lint -e{930} # MISRA Deviation */
				return_status = ( DCI_CBACK_RET_ERROR_ENCODED
								  | ( static_cast<DCI_CB_RET_TD>( DCI_ERR_INVALID_COMMAND ) ) );
				break;

			default:
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to
				 * unsigned short
				 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve
				 * casting pointers, not enums.
				 */
				/*lint -e{930} # MISRA Deviation */
				return_status = ( DCI_CBACK_RET_ERROR_ENCODED
								  | ( static_cast<DCI_CB_RET_TD>( DCI_ERR_ACCESS_VIOLATION ) ) );
				break;
		}
		RETURN_FLAG = true;
	}
	else if ( ( OUT_TERMINATOR_PRESENT ) &&
			  ( modified_selection_dcid < terminated_dcid ) &&
			  ( ( *received_asm_id ) == 0U )
			   )
	{
		OUT_TERMINATOR_PRESENT = false;
		terminated_dcid = modified_selection_dcid;
	}
	else
	{
		if ( ( OUT_TERMINATOR_PRESENT ) &&
			 ( modified_selection_dcid == terminated_dcid ) &&
			 ( ( *received_asm_id ) != 0U )
			  )
		{
			OUT_TERMINATOR_PRESENT = false;
			terminated_dcid = 0U;
		}
	}

	if ( ( !RETURN_FLAG ) && ( received_dcid_length < 5U ) )
	{
		return_status = static_cast<uint8_t>( DCI_CBACK_RET_PROCESS_NORMALLY );
	}
	else
	{
		switch ( access_struct->command )
		{
			case DCI_ACCESS_SET_RAM_CMD:
			case DCI_ACCESS_SET_INIT_CMD:
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to
				 * unsigned short
				 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve
				 * casting pointers, not enums.
				 */
				/*lint -e{930} # MISRA Deviation */
				return_status = ( DCI_CBACK_RET_ERROR_ENCODED
								  | ( static_cast<DCI_CB_RET_TD>( DCI_ERR_EXCEEDS_RANGE ) ) );
				break;

			default:
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to
				 * unsigned short
				 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve
				 * casting pointers, not enums.
				 */
				/*lint -e{930} # MISRA Deviation */
				return_status = ( DCI_CBACK_RET_ERROR_ENCODED
								  | ( static_cast<DCI_CB_RET_TD>( DCI_ERR_ACCESS_VIOLATION ) ) );
				break;
		}
	}
	return ( return_status );
}
