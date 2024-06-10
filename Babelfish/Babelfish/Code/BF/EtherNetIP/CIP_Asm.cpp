/*
 ********************************************************************************
 *		$Workfile: CIP_Asm.cpp$
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 3/5/2009 8:49:08 AM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *		$Header: CIP_Asm.h, 22, 3/5/2009 8:49:08 AM, Mark A Verheyen$
 ********************************************************************************
 */
#include "Includes.h"
#include "CIP_Asm.h"
#include "Scratch_Ram.h"
#include "EIP_Obj_Svc.h"
#include "eipid.h"
#include "Stdlib_MV.h"
#ifdef ESP32_SETUP
#include "Prod_Spec_LTK_ESP32.h"
#include "Prod_Spec_EIP.h"
#else
#include "Prod_Spec_LTK_STM32F.h"
#endif
#include "EIP_Debug.h"
/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

static inline DCI_LENGTH_TD Align_Size( DCI_LENGTH_TD len, uint8_t align )
{
	return ( static_cast<DCI_LENGTH_TD>( ( len + ( static_cast<DCI_LENGTH_TD>( align ) - 1U ) ) &
										 ( static_cast<DCI_LENGTH_TD>( ~( static_cast<uint16_t>
																		  ( align ) - 1U ) ) ) ) );
}

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */

/*
 ********************************************************************************
 *		Prototypes
 ********************************************************************************
 */

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
CIP_Asm::CIP_Asm( CIP_ASM_STRUCT_TD const* asm_list,
				  uint16_t asm_list_len, uint8_t alignment ) :
	BOOL_DEFAULT_INITIALISED( false ),
	m_src_id( 0U ),
	m_max_asm_size( 0U ),
	m_asm_list_len( 0U ),
	m_incoming_assembly_scratch( reinterpret_cast<uint8_t*>( NULL ) ),
	m_outgoing_assembly_scratch( reinterpret_cast<uint8_t*>( NULL ) ),
	m_patron( reinterpret_cast<DCI_Patron*>( NULL ) ),
	m_change_tracker( reinterpret_cast<Change_Tracker*>( NULL ) ),
	m_dynamic_owner( reinterpret_cast<DCI_Owner*>( NULL ) ),
	m_asm_list( reinterpret_cast<CIP_ASM_STRUCT_TD const*>( NULL ) ),
	m_end_asm_struct( reinterpret_cast<CIP_ASM_STRUCT_TD const*>( NULL ) )
{
	DCI_Owner* owner = reinterpret_cast<DCI_Owner*>( NULL );
	DCI_LENGTH_TD temp_asm_len = 0U;
	DCI_LENGTH_TD asm_list_member_len = 0U;

	BOOL_DEFAULT_INITIALISED = false;

	alignment = alignment;

	m_change_tracker = new Change_Tracker( &Change_Track_Handler_Static, reinterpret_cast
										   <Change_Tracker::cback_param_t>( this ), true );

	m_patron = new DCI_Patron( true );

	m_src_id = DCI_SOURCE_IDS_Get();

	m_asm_list = asm_list;
	m_asm_list_len = asm_list_len;

	for ( BF_Lib::MUINT16 i = 0U; i < asm_list_len; i++ )
	{
		if ( asm_list[i].type == CIP_ASM_TYPE_LIST_HEAD )
		{
			/*  We do this only to provide a memory location to point to and
			    avoid assigning values. */
#ifndef ESP32_SETUP
			owner = new DCI_Owner( m_asm_list[i].dci_id, DCI_OWNER_ALLOC_NO_MEM );
#else
			/*
			 *  With DCI_OWNER_ALLOC_NO_MEM system is crashing.
			 *  Functionality is working with one parameterized constructor,
			 *  since we are calling Init_Owner() from both the constructors.
			 */
			owner = new DCI_Owner( m_asm_list[i].dci_id );
#endif
			temp_asm_len = 0U;
			for ( BF_Lib::MUINT8 j = 0U; j < asm_list_len; j++ )
			{
				if ( ( asm_list[j].type == CIP_ASM_TYPE_LIST_MEMBER ) &&
					 ( asm_list[i].instance == asm_list[j].instance ) )
				{
					m_patron->Get_Length( asm_list[j].dci_id, &asm_list_member_len );
					asm_list_member_len = Align_Size( asm_list_member_len, m_asm_list[i].asm_offset );
					temp_asm_len += asm_list_member_len;
				}
			}
			owner->Attach_Callback( &Access_Handler_Static, reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
									( static_cast<uint32_t>( DCI_ACCESS_SET_RAM_CMD_MSK |
															 DCI_ACCESS_GET_RAM_CMD_MSK ) ) );

			owner->Check_In_Length( temp_asm_len );
		}
		else
		{

			m_change_tracker->Track_ID( m_asm_list[i].dci_id );

			m_patron->Get_Length( asm_list[i].dci_id, &temp_asm_len );
		}
		if ( temp_asm_len > m_max_asm_size )
		{
			m_max_asm_size = temp_asm_len;
		}
	}

	m_incoming_assembly_scratch = reinterpret_cast<uint8_t*>( Ram::Allocate( m_max_asm_size ) );

	m_outgoing_assembly_scratch = reinterpret_cast<uint8_t*>( Ram::Allocate( m_max_asm_size ) );
}

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
void CIP_Asm::Initialize( void )
{
	CIP_ASM_STRUCT_TD const* asm_list_ptr;
	uint16_t asm_type;
	DCI_LENGTH_TD asm_length = 0U;
	DCI_ACCESS_ST_TD val_access_struct;
	DCI_ERROR_TD dci_error = DCI_ERR_TOTAL_ERRORS;

	asm_list_ptr = m_asm_list;
	m_end_asm_struct = &m_asm_list[m_asm_list_len - 1U];

	for ( BF_Lib::MUINT8 asm_ctr = 0U; asm_ctr < m_asm_list_len; asm_ctr++ )
	{
		if ( asm_list_ptr->type != CIP_ASM_TYPE_LIST_MEMBER )
		{
			if ( asm_list_ptr->produced == true )
			{
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from
				 * enum to unsigned short
				 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9
				 * involve casting pointers, not enums.
				 */
				/*lint -e{930} # MISRA Deviation */
				asm_type = ( static_cast<uint16_t>( AssemblyStatic )
							 | static_cast<uint16_t>( AssemblyProducing ) );
			}
			else
			{
				/**
				 *@remark Coding Standard Deviation:
				 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from
				 * enum to unsigned short
				 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9
				 * involve casting pointers, not enums.
				 */
				/*lint -e{930} # MISRA Deviation */
				asm_type = ( static_cast<uint16_t>( AssemblyStatic )
							 | static_cast<uint16_t>( AssemblyConsuming ) );
			}
			m_patron->Get_Length( asm_list_ptr->dci_id, &asm_length );
			/**
			 * @remark Coding Standard Deviation:
			 * @n MISRA-C++[2008] Rule 2-13-3: Unsigned octal and hexadecimal
			 * literals require a 'U' suffix
			 * @n PCLint: 1960
			 * @n Justification: INVALID_OFFSET is defined in third party code
			 */
			/*lint -e{1960}*/
			clientAddAssemblyInstance( asm_list_ptr->instance, asm_type,
									   static_cast<int32_t>( INVALID_OFFSET ),
									   asm_length,
									   reinterpret_cast<char_t*>( nullptr ) );

			val_access_struct.command = DCI_ACCESS_GET_RAM_CMD;
			val_access_struct.data_id = asm_list_ptr->dci_id;
			val_access_struct.source_id = m_src_id;
			val_access_struct.data_access.data = m_outgoing_assembly_scratch;
			val_access_struct.data_access.length = asm_length;
			val_access_struct.data_access.offset = 0U;

			dci_error = m_patron->Data_Access( &val_access_struct );
			if ( dci_error != DCI_ERR_NO_ERROR )
			{
				EIP_DEBUG_PRINT( DBG_MSG_ERROR, "Error in Data Access from DCI_ACCESS_GET_RAM_CMD" );
			}

			clientSetAssemblyInstanceData( asm_list_ptr->instance, m_outgoing_assembly_scratch, asm_length );
		}
		asm_list_ptr++;
	}

	m_update_default_dynamic_asm_callback();
	BOOL_DEFAULT_INITIALISED = true;
}

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
void CIP_Asm::Process_RXed_Assembly( uint32_t instance )
{
	DCI_ACCESS_ST_TD access_struct;
	CIP_ASM_STRUCT_TD const* asm_list_ptr;
	DCI_ERROR_TD dci_error = DCI_ERR_TOTAL_ERRORS;

	asm_list_ptr = m_asm_list;

	while ( ( asm_list_ptr->type == CIP_ASM_TYPE_LIST_MEMBER ) ||
			( asm_list_ptr->instance != instance ) )
	{
		asm_list_ptr++;
	}
	access_struct.data_id = asm_list_ptr->dci_id;

	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.source_id = m_src_id;
	access_struct.data_access.data = m_incoming_assembly_scratch;
	access_struct.data_access.offset = 0U;

	access_struct.data_access.length = clientGetAssemblyInstanceData( instance,
																	  m_incoming_assembly_scratch, m_max_asm_size );
	dci_error = m_patron->Data_Access( &access_struct );
	if ( dci_error != DCI_ERR_NO_ERROR )
	{
		EIP_DEBUG_PRINT( DBG_MSG_ERROR, "Error in Data Access from DCI_ACCESS_SET_RAM_CMD" );
	}

	// CIP_SET_STRUCT set_struct;
	//
	// set_struct.cia.cip_class = EIP_SVC_ASSEMBLY_CLASS;
	// set_struct.cia.cip_instance = instance;
	// set_struct.cia.cip_attrib = EIP_SVC_ASSEMBLY_DATA_ATTRIBUTE;
	// set_struct.src_data = m_incoming_assembly_scratch;
	// set_struct.length = m_eip_target_info->asm_max_data_len;
	// set_struct.length = clientGetAssemblyInstanceData( instance,
	// set_struct.src_data, set_struct.length );
	// m_eip_dci->Set( &set_struct );
}

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
DCI_CB_RET_TD CIP_Asm::Access_Handler( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;

	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ID_TD dci_id_ctr = 0U;
	DCI_ID_TD dci_id_ctr2 = 0U;
	DCI_ID_TD present_dcid = 0U;
	DCI_PATRON_ATTRIB_SUPPORT_TD attrib_sup = 0U;
	DCI_LENGTH_TD offset = 0U;
	DCI_LENGTH_TD dcid_len = 0U;
	DCI_LENGTH_TD dcid_len2 = 0U;
	DCI_LENGTH_TD dcid_len3 = 0U;
	DCI_LENGTH_TD dci_data_index = 0U;
	DCI_LENGTH_TD index = 0U;
	DCI_LENGTH_TD end_index = 0U;
	DCI_LENGTH_TD align_mask = 0U;
	uint32_t max_d_out_data = 0U;
	uint8_t* data_ptr = reinterpret_cast<uint8_t*>( NULL );
	uint32_t* max_d_out_data_ptr = reinterpret_cast<uint32_t*>( NULL );
	CIP_ASM_STRUCT_TD const* asm_list_ptr =
		reinterpret_cast<CIP_ASM_STRUCT_TD const*>( NULL );

	DCI_ACCESS_ST_TD val_access_struct;

	// We verify that this DCI_ID is actually not part of an assembly.
	asm_list_ptr = Find_ASM_From_DCID( access_struct->data_id );

	if ( asm_list_ptr != nullptr )
	{
		if ( asm_list_ptr->type == CIP_ASM_TYPE_LIST_HEAD )
		{
			switch ( access_struct->command )
			{
				case DCI_ACCESS_GET_RAM_CMD:
					val_access_struct.command = DCI_ACCESS_GET_RAM_CMD;
					break;

				case DCI_ACCESS_SET_RAM_CMD:
					dci_error = m_patron->Get_Attribute_Support( access_struct->data_id, &attrib_sup );
					if ( dci_error == DCI_ERR_NO_ERROR )
					{
						if ( Test_Bit( attrib_sup, DCI_PATRON_ATTRIB_RAM_WR_DATA ) )
						{
							val_access_struct.command = DCI_ACCESS_SET_RAM_CMD;
						}
						else
						{
							dci_error = DCI_ERR_ACCESS_VIOLATION;
						}
						break;
					}
					else
					{
						EIP_DEBUG_PRINT( DBG_MSG_ERROR, "Error in getting attribute support from data id" );
					}

				default:
					// We should never get here.
					break;
			}

			if ( dci_error == DCI_ERR_NO_ERROR )
			{
				val_access_struct.source_id = access_struct->source_id;
				align_mask = static_cast<DCI_LENGTH_TD>( asm_list_ptr->asm_offset ) - 1U;
				dci_data_index = 0U;
				dcid_len = 0U;
				index = 0U;
				dci_id_ctr = 0U;
				end_index = access_struct->data_access.offset +
					access_struct->data_access.length;

				data_ptr = reinterpret_cast<uint8_t*>( access_struct->data_access.data );

				while ( index < end_index )
				{
					if ( ( ( index & align_mask ) == 0U ) && ( dci_data_index == 0U ) )
					{
						present_dcid = asm_list_ptr->dcid_list[dci_id_ctr];
						dci_id_ctr++;
						m_patron->Get_Length( present_dcid, &dcid_len );
						dci_data_index = dcid_len;
					}
					if ( index >= access_struct->data_access.offset )
					{
						if ( dci_data_index == 0U )
						{
							if ( access_struct->command == DCI_ACCESS_GET_RAM_CMD )
							{
								*data_ptr = 0U;	// If we are doing a get we need to zero fill the gaps.
							}
							data_ptr++;
							index++;
							// dci_data_index is always 0 because of the if we're in.
							/*if ( dci_data_index > 0U )
							   {
							    dci_data_index--;
							   }*/
						}
						else
						{
							val_access_struct.data_access.offset = dcid_len - dci_data_index;
							val_access_struct.data_access.length = ( dcid_len - val_access_struct.data_access.offset );

							if ( ( val_access_struct.data_access.length + index ) > end_index )
							{
								val_access_struct.data_access.length = end_index - index;
							}
							val_access_struct.data_access.data = data_ptr;
							val_access_struct.data_id = present_dcid;
							// ------------------------------------------------------------------------------
							if ( asm_list_ptr->instance == D_OUT_INSTANCE )
							{
								if ( BOOL_DEFAULT_INITIALISED )
								{
									// Commented by Shekhar during LTKBI, since no dynamic output  assembly is used as
									// of now
									dci_id_ctr2 = dci_id_ctr - 1U;
									val_access_struct.data_id =
										Return_Mapped_DCID( static_cast<uint8_t>( dci_id_ctr2 ) );
									m_patron->Get_Length( val_access_struct.data_id, &dcid_len2 );
									max_d_out_data_ptr = reinterpret_cast<uint32_t*>(
										reinterpret_cast<void*>( data_ptr ) );

									max_d_out_data = *max_d_out_data_ptr;

									if ( Check_RxData_Range( dcid_len2, max_d_out_data ) )
									{
										dcid_len3 = val_access_struct.data_access.length;
										val_access_struct.data_access.length = dcid_len2;
										offset = val_access_struct.data_access.offset;
										val_access_struct.data_access.offset = 0U;
										val_access_struct.command = DCI_ACCESS_SET_RAM_CMD;	//
										dci_error = m_patron->Data_Access( &val_access_struct );

										// -------------------------//#k:
										// New-Start-----------------------

										if ( dci_error != DCI_ERR_NO_ERROR )// #k: New-Start
										{
											val_access_struct.command = DCI_ACCESS_GET_RAM_CMD;
											dci_error = m_patron->Data_Access( &val_access_struct );
											if ( dci_error != DCI_ERR_NO_ERROR )
											{
												EIP_DEBUG_PRINT( DBG_MSG_ERROR,
																 "Error in Data Access from DCI_ACCESS_GET_RAM_CMD" );
											}
											clientSetAssemblyInstanceData( asm_list_ptr->instance,
																		   m_incoming_assembly_scratch,
																		   m_max_asm_size );
										}
										// -------------------------End-----------------------
										val_access_struct.command = DCI_ACCESS_SET_INIT_CMD;
										dci_error = m_patron->Data_Access( &val_access_struct );

										// -------------------------//#k:
										// New-Start-----------------------
										if ( dci_error != DCI_ERR_NO_ERROR )// #k: New-Start
										{
											val_access_struct.command = DCI_ACCESS_GET_RAM_CMD;
											dci_error = m_patron->Data_Access( &val_access_struct );
											if ( dci_error != DCI_ERR_NO_ERROR )
											{
												EIP_DEBUG_PRINT( DBG_MSG_ERROR,
																 "Error in Data Access from DCI_ACCESS_GET_RAM_CMD" );
											}
											val_access_struct.command = DCI_ACCESS_GET_INIT_CMD;
											dci_error = m_patron->Data_Access( &val_access_struct );
											if ( dci_error != DCI_ERR_NO_ERROR )
											{
												EIP_DEBUG_PRINT( DBG_MSG_ERROR,
																 "Error in Data Access from DCI_ACCESS_GET_INIT_CMD" );
											}

											clientSetAssemblyInstanceData( asm_list_ptr->instance,
																		   m_incoming_assembly_scratch,
																		   m_max_asm_size );
										}
										// -------------------------//#k: New-End-----------------------
										val_access_struct.data_access.offset = offset;
										val_access_struct.data_access.length = dcid_len3;
									}
								}
							}
							else
							{	// DCI_ERR_RAM_READ_ONLY
								// ------------------------------------------------------------------------------
								dci_error = m_patron->Data_Access( &val_access_struct );
								// -------------------------//#k: New-Start-----------------------
								if ( dci_error != DCI_ERR_NO_ERROR )// #k: New-Start
								{
									val_access_struct.command = DCI_ACCESS_GET_RAM_CMD;
									dci_error = m_patron->Data_Access( &val_access_struct );
									if ( dci_error != DCI_ERR_NO_ERROR )
									{
										EIP_DEBUG_PRINT( DBG_MSG_ERROR,
														 "Error in Data Access from DCI_ACCESS_GET_RAM_CMD" );
									}

									val_access_struct.command = DCI_ACCESS_GET_INIT_CMD;
									dci_error = m_patron->Data_Access( &val_access_struct );
									if ( dci_error != DCI_ERR_NO_ERROR )
									{
										EIP_DEBUG_PRINT( DBG_MSG_ERROR,
														 "Error in Data Access from DCI_ACCESS_GET_INIT_CMD" );
									}

									clientSetAssemblyInstanceData( asm_list_ptr->instance, m_incoming_assembly_scratch,
																   m_max_asm_size );
								}
								// -------------------------//#k: New-End-----------------------
							}
							data_ptr += val_access_struct.data_access.length;
							index += val_access_struct.data_access.length;
							dci_data_index = dcid_len - ( val_access_struct.data_access.offset
														  + val_access_struct.data_access.length );
						}
					}
					else
					{
						index++;
						if ( dci_data_index > 0U )
						{
							dci_data_index--;
						}
					}
				}
			}
		}
	}
	return ( return_status );
}

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * checks that for the inserted Dynamic o/p DCID,
 * the received data's range is within limit of that DCID's datatype.
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
bool_t CIP_Asm::Check_RxData_Range( DCI_LENGTH_TD dcid_len2, uint32_t max_d_out_data ) const
{
	bool_t RECEIVED_DATA_SIZE_CORRECT = false;

	switch ( dcid_len2 )
	{
		case LENGTH_ONE_BYTE:

			if ( !( static_cast<bool_t>( max_d_out_data & 0xFFFFFF00U ) ) )
			{
				RECEIVED_DATA_SIZE_CORRECT = true;
			}
			break;

		case LENGTH_TWO_BYTE:
			if ( !( static_cast<bool_t>( max_d_out_data & 0xFFFF0000U ) ) )
			{
				RECEIVED_DATA_SIZE_CORRECT = true;
			}
			break;

		case LENGTH_THREE_BYTE:
			if ( !( static_cast<bool_t>( max_d_out_data & 0xFF000000U ) ) )
			{
				RECEIVED_DATA_SIZE_CORRECT = true;
			}
			break;

		case LENGTH_FOUR_BYTE:
			RECEIVED_DATA_SIZE_CORRECT = true;
			break;

		default:
			RECEIVED_DATA_SIZE_CORRECT = false;
	}
	return ( RECEIVED_DATA_SIZE_CORRECT );
}

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
void CIP_Asm::Change_Track_Handler( DCI_ID_TD dci_id )
{
	CIP_ASM_STRUCT_TD const* asm_list_ptr;
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error = DCI_ERR_TOTAL_ERRORS;

	asm_list_ptr = Find_ASM_From_DCID( dci_id );
	if ( asm_list_ptr != reinterpret_cast<CIP_ASM_STRUCT_TD const*>( NULL ) )
	{
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;
		access_struct.source_id = m_src_id;
		access_struct.data_id = dci_id;
		access_struct.data_access.data = m_outgoing_assembly_scratch;
		access_struct.data_access.length = 0U;
		access_struct.data_access.offset = 0U;
		dci_error = m_patron->Data_Access( &access_struct );
		if ( dci_error != DCI_ERR_NO_ERROR )
		{
			EIP_DEBUG_PRINT( DBG_MSG_ERROR, "Error in Data Access from DCI_ACCESS_GET_RAM_CMD" );
		}

		do
		{
			clientSetAssemblyInstanceDataOffset(
				asm_list_ptr->instance,
				m_outgoing_assembly_scratch,
				asm_list_ptr->asm_offset,
				access_struct.data_access.length );
			asm_list_ptr++;
		} while ( ( asm_list_ptr->dci_id == dci_id ) &&
				  ( asm_list_ptr <= m_end_asm_struct ) );
	}
}

/*
 ********************************************************************************
 * ----- Function Header --------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 * ------------------------------------------------------------------------------
 ********************************************************************************
 */
bool_t CIP_Asm::Dynamic_Change_Track( DCI_ID_TD header_dci_id,
									  DCI_ID_TD member_dci_id,
									  uint8_t Assembly_offset )
{
	bool_t Return_Flag = false;
	uint8_t resettozero[4] = {0U, 0U, 0U, 0U};
	uint8_t* Ptrresettozero;
	uint16_t offset = 0U;
	CIP_ASM_STRUCT_TD const* asm_list_ptr;
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error = DCI_ERR_TOTAL_ERRORS;

	Ptrresettozero = &resettozero[0];
	asm_list_ptr = Find_ASM_From_DCID( header_dci_id );
	if ( asm_list_ptr != reinterpret_cast<CIP_ASM_STRUCT_TD const*>( NULL ) )
	{
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;
		access_struct.source_id = m_src_id;
		access_struct.data_id = member_dci_id;	// Ori-access_struct.data_id = dci_id;
		access_struct.data_access.data = m_outgoing_assembly_scratch;
		access_struct.data_access.length = 0U;
		access_struct.data_access.offset = 0U;
		dci_error = m_patron->Data_Access( &access_struct );
		if ( dci_error != DCI_ERR_NO_ERROR )
		{
			EIP_DEBUG_PRINT( DBG_MSG_ERROR, "Error in Data Access from DCI_ACCESS_GET_RAM_CMD" );
		}

		while ( Assembly_offset != 0U )
		{
			Assembly_offset--;
			offset = offset + 4U;
		}

		clientSetAssemblyInstanceDataOffset( asm_list_ptr->instance,
											 Ptrresettozero,
											 ( offset ),
											 4U );

		clientSetAssemblyInstanceDataOffset( asm_list_ptr->instance,
											 m_outgoing_assembly_scratch,
											 ( offset ),
											 access_struct.data_access.length );
		Return_Flag = true;
	}
	else
	{
		Return_Flag = false;
	}
	return ( Return_Flag );
}

/*
 *******************************************************************************
 * ----- Function Header -------------------------------------------------------
 * -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 * -----------------------------------------------------------------------------
 *******************************************************************************
 */
CIP_ASM_STRUCT_TD const* CIP_Asm::Find_ASM_From_DCID( DCI_ID_TD dci_id ) const
{
	bool_t Loop_Flag = true;
	DCI_ID_TD begin = 0U;
	DCI_ID_TD end;
	DCI_ID_TD middle;
	CIP_ASM_STRUCT_TD const* search_struct;
	CIP_ASM_STRUCT_TD const* Return_Address;

	Return_Address = reinterpret_cast<CIP_ASM_STRUCT_TD*>( NULL );
	search_struct = m_asm_list;
	end = m_asm_list_len;
	while ( ( begin <= end ) && ( Loop_Flag ) )
	{
		middle = ( begin + end ) >> 1U;
		if ( search_struct[middle].dci_id < dci_id )
		{
			begin = middle + 1U;
		}
		else if ( search_struct[middle].dci_id > dci_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				Return_Address = reinterpret_cast<CIP_ASM_STRUCT_TD*>( NULL );
				Loop_Flag = false;

			}
		}
		else
		{
			Return_Address = ( &search_struct[middle - search_struct[middle].dci_id_offset] );
			Loop_Flag = false;
		}
	}
	return ( Return_Address );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void CIP_Asm::UnTrack_ID( DCI_ID_TD default_dci_id )
{
	m_change_tracker->UnTrack_ID( default_dci_id );
}

#if 0
// DEBUG DEBUG DEBUG
DCI_Owner* temp_owner;
temp_owner = new DCI_Owner( DCI_TEST_U32_DCID );

m_outgoing_assembly_scratch[0] = 0x32;
m_outgoing_assembly_scratch[1] = 0x33;
m_outgoing_assembly_scratch[2] = 0x34;
m_outgoing_assembly_scratch[3] = 0x35;
temp_owner->Check_In( m_outgoing_assembly_scratch );
temp_owner = new DCI_Owner( DCI_TEST_U8_ARRAY_DCID );

m_outgoing_assembly_scratch[0] = 0xA8;
m_outgoing_assembly_scratch[1] = 0xA9;
m_outgoing_assembly_scratch[2] = 0xAA;
temp_owner->Check_In( m_outgoing_assembly_scratch );
temp_owner = new DCI_Owner( DCI_TEST_U8_0_DCID );

m_outgoing_assembly_scratch[0] = 0x80;
temp_owner->Check_In( m_outgoing_assembly_scratch );
temp_owner = new DCI_Owner( DCI_TEST_U8_1_DCID );

m_outgoing_assembly_scratch[0] = 0x81;
temp_owner->Check_In( m_outgoing_assembly_scratch );
temp_owner = new DCI_Owner( DCI_TEST_U16_DCID );

m_outgoing_assembly_scratch[0] = 0xF0;
m_outgoing_assembly_scratch[1] = 0xF1;
temp_owner->Check_In( m_outgoing_assembly_scratch );

DCI_ACCESS_ST_TD access_struct;
for ( uint8_t i = 0; i < m_max_asm_size; i++ )
{
	m_outgoing_assembly_scratch[i] = i + 1;
}

DCI_LENGTH_TD asm_len;
m_patron->Get_Length( DCI_CIP_ASM_WRITABLE_DCID, &asm_len );
access_struct.source_id = m_src_id;
access_struct.data_access.data = m_outgoing_assembly_scratch;
access_struct.data_id = DCI_CIP_ASM_WRITABLE_DCID;
for ( uint8_t toffset = 0; toffset < asm_len; toffset++ )
{
	for ( uint8_t tlen = 1; tlen <= ( asm_len - toffset ); tlen++ )
	{
		access_struct.data_access.offset = toffset;
		access_struct.data_access.length = tlen;
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;
		m_patron->Data_Access( &access_struct );

		access_struct.command = DCI_ACCESS_SET_RAM_CMD;
		m_patron->Data_Access( &access_struct );
	}
}

access_struct.command = DCI_ACCESS_GET_RAM_CMD;
access_struct.data_id = DCI_CIP_ASM_WRITABLE_DCID;
access_struct.data_access.data = m_outgoing_assembly_scratch;
access_struct.data_access.offset = 0;
access_struct.data_access.length = asm_len;
m_patron->Data_Access( &access_struct );
#endif
