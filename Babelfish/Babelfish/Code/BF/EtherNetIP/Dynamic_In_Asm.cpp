/*
 ********************************************************************************
 *		$Workfile: Dynamic_In_Asm.cpp$
 *
 *		$Author: Pranav$
 *		$Date: 24/02/2014 12:18:38 PM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 ********************************************************************************
 */
#include "includes.h"
#include "Stdlib_MV.h"
// #include "vintage_stm32_eth.h"
#include "Dynamic_In_Asm.h"
#include "EIP_Service.h"
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
**  Function Name:   Dynamic_In_Asm::Dynamic_In_Asm
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
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
Dynamic_In_Asm::Dynamic_In_Asm( EIP_Service* m_EIP_Service ) :
	IN_TERMINATOR_PRESENT( false ),
	m_dyn_EIP_Service( m_EIP_Service ),
	terminated_dcid( 0U ),
	m_source_id( 0U ),
	m_assembly_profile( reinterpret_cast<D_IN_ASM_TARGET_INFO_ST_TD*>( NULL ) ),
	m_patron( reinterpret_cast<DCI_Patron*>( NULL ) ),
	m_shadow_ctrl( reinterpret_cast<Shadow*>( NULL ) ),
	m_change_tracker( reinterpret_cast<Change_Tracker*>( NULL ) ),
	mapped_IN_ASM_id_to_dcid{ 0U },
	default_dynamic_dcid{ 0U }
{
	IN_TERMINATOR_PRESENT = false;
	terminated_dcid = 0U;
	m_patron = new DCI_Patron( false );
	m_source_id = DCI_SOURCE_IDS_Get();
	m_shadow_ctrl = new Shadow( m_patron, m_source_id );
	m_change_tracker = new Change_Tracker( &Change_Track_Handler_Static,
										   reinterpret_cast<Change_Tracker::cback_param_t>( this ),
										   true );

	m_assembly_profile = &d_in_asm_dci_data_target_info;
	init_D_IN_ASM_Selection_owners();
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::init_D_IN_ASM_Selection_owners
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
**
**  Description:
**                  Creats the owner of Dynamic Interface object's attributes
**                  & assign Callback & change Tracker to them.
**  Inputs:
**                  None
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_In_Asm::init_D_IN_ASM_Selection_owners( void )
{
	uint8_t temp_uint8 = 0U;

	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_1_DCID);
	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_2_DCID);
	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_3_DCID);
	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_4_DCID);
	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_5_DCID);
	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_6_DCID);
	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_7_DCID);
	// new DCI_Owner( DCI_DYN_IN_ASM_MEMBER_8_DCID);
	//
	// d_in_asm_dcid[D1_ASM] = DCI_DYN_IN_ASM_SELECT_1_DCID;
	// d_in_asm_dcid[D2_ASM] = DCI_DYN_IN_ASM_SELECT_2_DCID;
	// d_in_asm_dcid[D3_ASM] = DCI_DYN_IN_ASM_SELECT_3_DCID;
	// d_in_asm_dcid[D4_ASM] = DCI_DYN_IN_ASM_SELECT_4_DCID;
	// d_in_asm_dcid[D5_ASM] = DCI_DYN_IN_ASM_SELECT_5_DCID;
	// d_in_asm_dcid[D6_ASM] = DCI_DYN_IN_ASM_SELECT_6_DCID;
	// d_in_asm_dcid[D7_ASM] = DCI_DYN_IN_ASM_SELECT_7_DCID;
	// d_in_asm_dcid[D8_ASM] = DCI_DYN_IN_ASM_SELECT_8_DCID;

	init_dyn_input_asm_dcids( d_in_asm_dcid, m_d_in_Asm_Selection );
	for ( temp_uint8 = 0U; temp_uint8 < D_IN_ASM_MAX; temp_uint8++ )
	{
		// m_d_in_Asm_Selection[temp_uint8]= new DCI_Owner( d_in_asm_dcid[temp_uint8]);
		m_d_in_Asm_Selection[temp_uint8]->Attach_Callback(
			&Check_Terminator_And_Length_CBack_Static,
			reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
			static_cast<uint32_t>( DCI_ACCESS_SET_RAM_CMD_MSK ) );

		m_change_tracker->Track_ID( d_in_asm_dcid[temp_uint8] );
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::update_default_DCID_in_D_IN_ASMBuffer
**  Created By:      Pranav Kodre
**  Date Created:    5-9-2014
**
**  Description:
**                  It updates the previously inserted dynamic IDs
**                  into the Assembly buffer.
**  Inputs:
**                  None
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_In_Asm::update_default_DCID_in_D_IN_ASMBuffer( void )
{
	DCI_ID_TD temp1uint16 = 0U;
	DCI_ID_TD current_assembly_number[D_IN_ASM_MAX] = {0U};

	for ( temp1uint16 = 0U; temp1uint16 < D_IN_ASM_MAX; temp1uint16++ )
	{
		default_dynamic_dcid[temp1uint16] =
			DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST[temp1uint16];

		m_shadow_ctrl->Get_DCID_Ram_Data(
			d_in_asm_dcid[temp1uint16],
			&current_assembly_number[temp1uint16],
			static_cast<DCI_LENGTH_TD>( sizeof( d_in_asm_dcid[temp1uint16] ) ) );

		if ( !IN_TERMINATOR_PRESENT )
		{
			if ( current_assembly_number[temp1uint16] == 0U )
			{
				terminated_dcid = d_in_asm_dcid[temp1uint16];
				IN_TERMINATOR_PRESENT = true;
			}
		}
		Map_Dynamic_ID_To_DCID( current_assembly_number[temp1uint16],
								&mapped_IN_ASM_id_to_dcid[temp1uint16] );

		update_DefaultDCID_in_DynamicAssemblyBuffer(
			default_dynamic_dcid[temp1uint16],
			mapped_IN_ASM_id_to_dcid[temp1uint16],
			static_cast<uint8_t>( temp1uint16 ) );

		m_change_tracker->Track_ID( mapped_IN_ASM_id_to_dcid[temp1uint16] );

		UnTrack_Default_dcid( default_dynamic_dcid[temp1uint16] );
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::Map_Dynamic_ID_To_DCID
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
void Dynamic_In_Asm::Map_Dynamic_ID_To_DCID( uint16_t assembly_id,
											 DCI_ID_TD* dcid_mapped_to_assembly_id ) const
{
	const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* D_IN_ASM_struct;

	D_IN_ASM_struct =
		Find_Dyn_In_Asm_DCID( static_cast<DYNAMIC_ASSEMBLY_PID_TD>( assembly_id ) );

	if ( D_IN_ASM_struct != LOOKUP_D_IN_ASM_TO_DCI_NOT_FOUND )
	{
		*dcid_mapped_to_assembly_id = D_IN_ASM_struct->dcid;
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::Find_Dyn_In_Asm_DCID
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
const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* Dynamic_In_Asm::Find_Dyn_In_Asm_DCID( DYNAMIC_ASSEMBLY_PID_TD assembly_id ) const
{
	uint_fast16_t begin = 0U;
	uint_fast16_t middle = 0U;
	uint_fast16_t end = 0U;
	const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* search_struct;
	const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* return_value =
		LOOKUP_D_IN_ASM_TO_DCI_NOT_FOUND;

	search_struct = m_assembly_profile->asmin_to_dcid;
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
**  Function Name:   Dynamic_In_Asm::Change_Track_Handler
**  Created By:      Pranav Kodre
**  Date Created:    5-9-2014
**
**  Description:
**                  Triggered by respective assigned DCIDs when they
**                  are modified.
**                  This function checks that whether a value of dynamic member
**                  is changed or a new Dynamic ID is inserted as a member of
**                  Dynamic i/p Assembly & calls the related functions.
**                  For modifed value of exsisting member, it updates the
**                  assembly buffer with chnaged value.
**  Inputs:
**                  dci_id
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_In_Asm::Change_Track_Handler( DCI_ID_TD dci_id )
{
	bool_t BOOL_IN_ASM_INSERTED = false;
	uint8_t temp1_uint8 = 0U;

	for ( temp1_uint8 = 0U; temp1_uint8 < D_IN_ASM_MAX; temp1_uint8++ )
	{
		if ( dci_id == mapped_IN_ASM_id_to_dcid[temp1_uint8] )
		{
			BOOL_IN_ASM_INSERTED = true;
			update_newDCID_in_DynamicAssemblyBuffer(
				default_dynamic_dcid[temp1_uint8],
				mapped_IN_ASM_id_to_dcid[temp1_uint8], temp1_uint8 );
		}
	}

	if ( !BOOL_IN_ASM_INSERTED )
	{
		BOOL_IN_ASM_INSERTED = false;
		check_changed_assembly( dci_id );
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::check_changed_assembly
**  Created By:      Pranav Kodre
**  Date Created:    5-9-2014
**
**  Description:
**                  It identifies the DCID from the Dynamic ID.
**                  Also identified that whether an Assembly TERMINATOR
**                  is inserted or not & then calls the related functions.
**  Inputs:
**                  dci_id
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_In_Asm::check_changed_assembly( DCI_ID_TD dci_id )
{
	uint16_t temp1uint16 = 0U;
	uint16_t temp2uint16 = 0U;

	m_shadow_ctrl->Get_DCID_Ram_Data(
		dci_id,
		&temp2uint16,
		static_cast<uint16_t>( sizeof( temp2uint16 ) ) );

	Map_Dynamic_ID_To_DCID( temp2uint16,
							&temp1uint16 );

	if ( temp1uint16 != DCI_DYN_IN_ASM_TERMINATOR_DCID )
	{
		update( dci_id, temp1uint16 );
	}
	else
	{
		TerminateAssembly( dci_id );
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::check_previous_entry
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
**
**  Description:
**                  It checks that whether same Dynamic ID is inserted for
**                  more than one times as assembly member.
**  Inputs:
**                  array_number: Double entry will be checked from zero to
**                  this number.
**  Outputs:
**                  Flag: More than one entries of Dynamic ID Present ot not
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
bool_t Dynamic_In_Asm::check_previous_entry( uint8_t array_number ) const
{
	uint8_t dcid_present_count = 0U;
	uint8_t i;
	bool_t DOUBLE_ENTRY = false;

	for ( i = 0U; i < array_number; i++ )
	{
		if ( mapped_IN_ASM_id_to_dcid[array_number] == mapped_IN_ASM_id_to_dcid[i] )
		{
			dcid_present_count++;
		}
	}
	if ( dcid_present_count >= 1U )
	{
		DOUBLE_ENTRY = true;
	}
	return ( DOUBLE_ENTRY );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::TerminateAssembly
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
**
**  Description:
**                  It inserts zero values from TERMINATED index to last index
**                  of Dynamic i/p Assembly & removes change tracker for all
**                  next Dynamic IDs except for those which are inserted
**                  from 0 to this TERMINATED index.
**  Inputs:
**                  terminated_dcid.
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_In_Asm::TerminateAssembly( DCI_ID_TD dci_id )
{
	DCI_ID_TD terminator_dcid_value = 0U;
	uint8_t temp1_uint8 = 0U;
	uint8_t temp2_uint8 = 0U;

	if ( !IN_TERMINATOR_PRESENT )
	{
		terminated_dcid = dci_id;
		IN_TERMINATOR_PRESENT = true;

		m_shadow_ctrl->Get_DCID_Ram_Data(
			DCI_DYN_IN_ASM_TERMINATOR_DCID,
			&terminator_dcid_value,
			static_cast<DCI_ID_TD>( sizeof( terminator_dcid_value ) ) );

		for ( temp1_uint8 = 0U; temp1_uint8 < D_IN_ASM_MAX; temp1_uint8++ )
		{
			if ( dci_id == d_in_asm_dcid[temp1_uint8] )
			{
				for ( temp2_uint8 = temp1_uint8; temp2_uint8 < D_IN_ASM_MAX; temp2_uint8++ )
				{
					if ( !check_previous_entry( temp2_uint8 ) )
					{
						m_change_tracker->UnTrack_ID( mapped_IN_ASM_id_to_dcid[temp2_uint8] );
					}
					mapped_IN_ASM_id_to_dcid[temp2_uint8] =
						DCI_DYN_IN_ASM_TERMINATOR_DCID;

					m_d_in_Asm_Selection[temp2_uint8]->Check_In( terminator_dcid_value );
					m_d_in_Asm_Selection[temp2_uint8]->Load_Current_To_Init();

					update_newDCID_in_DynamicAssemblyBuffer(
						default_dynamic_dcid[temp2_uint8],
						DCI_DYN_IN_ASM_TERMINATOR_DCID,
						temp2_uint8 );
				}
			}
		}
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::check_double_entry
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
**
**  Description:
**                  It inserts zero values from TERMINATED index to last index
**                  of Dynamic i/p Assembly & removes change tracker for all
**                  next Dynamic IDs except for those which are inserted
**                  from 0 to this TERMINATED index.
**  Inputs:
**                  dcid whose double entry want to confirm.
**  Outputs:
**                  Flag: More than one entries of dcid is Present or not
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
bool_t Dynamic_In_Asm::check_double_entry( DCI_ID_TD dci_id ) const
{
	bool_t DOUBLE_ENTRY = false;
	uint8_t dcid_present_count = 0U;
	uint8_t i = 0U;

	for ( i = 0U; i < D_IN_ASM_MAX; i++ )
	{
		if ( dci_id == mapped_IN_ASM_id_to_dcid[i] )
		{
			dcid_present_count++;
		}
	}
	if ( dcid_present_count > 1U )
	{
		DOUBLE_ENTRY = true;
	}
	return ( DOUBLE_ENTRY );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::update
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
**
**  Description:
**                  It untracks the previous Dynamic ID & inserts
**                  the new Dynamic ID into Assembly buffer.
**                  Starts tracking new Dynamic ID & untracks the previous ID
**                  if it's not inserted more than one times into the
**                  Dynamic i/p Assembly.
**  Inputs:
**                  dcid: An index number whose value is modified.
**                  temp1uint16: DCID of new inserted Dynamic ID
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
void Dynamic_In_Asm::update( DCI_ID_TD dci_id, uint16_t temp1uint16 )
{
	uint8_t temp1_uint8 = 0;

	for ( temp1_uint8 = 0U; temp1_uint8 < D_IN_ASM_MAX; temp1_uint8++ )
	{
		if ( dci_id == d_in_asm_dcid[temp1_uint8] )
		{
			if ( mapped_IN_ASM_id_to_dcid[temp1_uint8] !=
				 DCI_DYN_IN_ASM_TERMINATOR_DCID )
			{
				if ( !check_double_entry( mapped_IN_ASM_id_to_dcid[temp1_uint8] ) )
				{
					m_change_tracker->UnTrack_ID( mapped_IN_ASM_id_to_dcid[temp1_uint8] );
				}
			}
			mapped_IN_ASM_id_to_dcid[temp1_uint8] = temp1uint16;

			update_newDCID_in_DynamicAssemblyBuffer( default_dynamic_dcid[temp1_uint8],
													 mapped_IN_ASM_id_to_dcid[temp1_uint8],
													 temp1_uint8 );
			m_change_tracker->Track_ID( mapped_IN_ASM_id_to_dcid[temp1_uint8] );
			break;
		}
	}
}

/**-----------------------------------------------------------------------------
**  Function Name:   Dynamic_In_Asm::Check_Terminator_And_Length_CBack
**  Created By:      Pranav Kodre
**  Date Created:    2-24-2014
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
DCI_CB_RET_TD Dynamic_In_Asm::Check_Terminator_And_Length_CBack(
	DCI_ACCESS_ST_TD* access_struct )
{
	bool_t RETURN_FLAG = false;
	DCI_ID_TD received_dcid = 0U;
	DCI_LENGTH_TD received_dcid_length = 0U;
	DCI_ID_TD modified_selection_dcid = 0U;
	DCI_CB_RET_TD return_status =
		static_cast<uint8_t>( DCI_CBACK_RET_NO_FURTHER_PROCESSING );
	DCI_ID_TD* received_asm_id;

	modified_selection_dcid = access_struct->data_id;
	received_asm_id =
		reinterpret_cast<DCI_ID_TD*>( access_struct->data_access.data );

	Map_Dynamic_ID_To_DCID( *received_asm_id, &received_dcid );

	m_patron->Get_Length( received_dcid, &received_dcid_length );

	if ( ( IN_TERMINATOR_PRESENT ) && ( ( modified_selection_dcid ) > terminated_dcid ) )
	{
		switch ( access_struct->command )
		{
			case DCI_ACCESS_SET_RAM_CMD:
			case DCI_ACCESS_SET_INIT_CMD:
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum
				 * to unsigned short
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
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum
				 * to unsigned short
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
	else
	{
		if ( ( IN_TERMINATOR_PRESENT ) &&
			 ( modified_selection_dcid < terminated_dcid ) &&
			 ( ( *received_asm_id ) == 0U )
			  )
		{
			IN_TERMINATOR_PRESENT = false;
			terminated_dcid = modified_selection_dcid;
		}
		else
		{
			if ( ( IN_TERMINATOR_PRESENT ) &&
				 ( modified_selection_dcid == terminated_dcid ) &&
				 ( ( *received_asm_id ) != 0U )
				  )
			{
				IN_TERMINATOR_PRESENT = false;
				terminated_dcid = 0U;
			}
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
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum
				 * to unsigned short
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Dynamic_In_Asm::update_newDCID_in_DynamicAssemblyBuffer( DCI_ID_TD header_dci_id, DCI_ID_TD member_dci_id,
																uint8_t Assembly_offset )
{
	bool_t Return_Flag;

	Return_Flag = m_dyn_EIP_Service->GetCIP_Asm_Object()->Dynamic_Change_Track( header_dci_id,
																				member_dci_id, Assembly_offset );
	return ( Return_Flag );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Dynamic_In_Asm::update_DefaultDCID_in_DynamicAssemblyBuffer( DCI_ID_TD header_dci_id,
																	DCI_ID_TD member_dci_id, uint8_t Assembly_offset )
{
	bool_t Return_Flag;

	Return_Flag = m_dyn_EIP_Service->GetCIP_Asm_Object()->Dynamic_Change_Track( header_dci_id,
																				member_dci_id, Assembly_offset );
	return ( Return_Flag );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Dynamic_In_Asm::UnTrack_Default_dcid( DCI_ID_TD header_dci_id )
{
	m_dyn_EIP_Service->GetCIP_Asm_Object()->UnTrack_ID( header_dci_id );
}
