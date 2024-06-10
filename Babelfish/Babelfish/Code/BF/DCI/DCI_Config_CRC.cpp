/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_Config_CRC.h"
#include "NV_Mem.h"
#include "DCI.h"
#include "CRC16.h"

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Config_CRC::DCI_Config_CRC( DCI_ID_TD config_crc_owner,
								crc_list_t const* crc_list ) :
	m_config_crc( new DCI_Owner( config_crc_owner ) ),
	m_cumulative_crc( CRC_INIT ),
	m_crc_calc_dci_index( 0U ),
	m_change_tracker( new Change_Tracker( &Trigger_CRC_Update_Static,
										  reinterpret_cast<Change_Tracker::cback_param_t>( this ),
										  true,
										  Change_Tracker::NV_ATTRIB_MASK ) ),
	m_cr_task( reinterpret_cast<CR_Tasker*>( nullptr ) )
{
	DCI_DATA_BLOCK_TD const* dci_block;

	if ( crc_list == reinterpret_cast<crc_list_t const*>( nullptr ) )
	{
		for ( DCI_ID_TD i = 0U; i < DCI_TOTAL_DCIDS; i++ )
		{
			dci_block = DCI::Get_Data_Block( i );
			if ( BF_Lib::Bit::Test( dci_block->owner_attrib_info, DCI_OWNER_ATTRIB_INFO_INIT_VAL ) )
			{
				m_change_tracker->Track_ID( i );
			}
		}
	}
	else
	{
		for ( DCI_ID_TD i = 0U; i < crc_list->dci_list_len; i++ )
		{
			dci_block = DCI::Get_Data_Block( crc_list->dci_list[i] );

			if ( dci_block != reinterpret_cast<DCI_DATA_BLOCK_TD const*>( nullptr ) )
			{
				if ( BF_Lib::Bit::Test( dci_block->owner_attrib_info,
										DCI_OWNER_ATTRIB_INFO_INIT_VAL ) == true )
				{
					m_change_tracker->Track_ID( crc_list->dci_list[i] );
				}
			}
		}
	}

	m_cr_task = new CR_Tasker( &Update_Config_CRC_uTask_Static,
							   reinterpret_cast<CR_TASKER_PARAM>( this ), CR_TASKER_IDLE_PRIORITY,
							   "DCI Config CRC" );

	Trigger_CRC_Update();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Config_CRC::~DCI_Config_CRC( void )
{
	delete m_config_crc;
	delete m_change_tracker;
	delete m_cr_task;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Config_CRC::Trigger_CRC_Update( void )
{
	m_cumulative_crc = CRC_INIT;

	m_crc_calc_dci_index = 0U;

	m_config_crc->Check_In( CRC_CALC_INCOMPLETE );

	m_cr_task->Resume();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Config_CRC::Update_Config_CRC_uTask( void )
{
	uint16_t temp_crc = 0U;
	DCI_DATA_BLOCK_TD const* dci_block;

	while ( ( !m_change_tracker->ID_Tracked( m_crc_calc_dci_index ) ) &&
			( m_crc_calc_dci_index < DCI_TOTAL_DCIDS ) )
	{
		m_crc_calc_dci_index++;
	}

	if ( m_crc_calc_dci_index < DCI_TOTAL_DCIDS )
	{
		dci_block = DCI::Get_Data_Block( m_crc_calc_dci_index );

		NV::NV_Mem::Read_Checksum( &temp_crc, dci_block->nv_loc, *dci_block->length_ptr );

		m_cumulative_crc = BF_Lib::CRC16::Calc_On_String( reinterpret_cast<uint8_t*>( &temp_crc ),
														  sizeof( uint16_t ), m_cumulative_crc );
	}

	m_crc_calc_dci_index++;
	if ( m_crc_calc_dci_index > DCI_TOTAL_DCIDS )
	{
		// If the CRC matches the incomplete value run the CRC again until it doesn't.
		// Use zero as the next calc value until it is not incomplete.
		while ( m_cumulative_crc == CRC_CALC_INCOMPLETE )
		{
			m_cumulative_crc = BF_Lib::CRC16::Calc_On_Byte( 0U, m_cumulative_crc );
		}

		m_config_crc->Check_In( m_cumulative_crc );

		m_cr_task->Suspend();	// Micro_Task::Destroy_Task( Update_Config_CRC_uTask_Static,
								// (MICRO_TASK_FUNC_PARAM_TD*)this );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t DCI_Config_CRC::Get_CRC( void ) const
{
	uint16_t temp_crc;

	m_config_crc->Check_Out( temp_crc );

	return ( temp_crc );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Config_CRC::Store_CRC( void ) const
{
	bool success;

	success = m_config_crc->Load_Current_To_Init();

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool DCI_Config_CRC::Retrieve_Stored_CRC( uint16_t* crc ) const
{
	return ( m_config_crc->Check_Out_Init( reinterpret_cast<uint8_t*>( crc ) ) );
}
