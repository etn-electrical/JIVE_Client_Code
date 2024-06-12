/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Flash_NV.h"
#include "Ram.h"
#include "Exception.h"
#include "StdLib_MV.h"
#include "System_Reset.h"
#include "NV_Ctrl_Debug.h"
#include "Mem_Check.h"
#include "CRC16.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define FLASH_NV_CHECKSUM_LEN            2U
#define FLASH_NV_LENGTH                  2U

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Flash_NV::Flash_NV( NV_Ctrl* nv_ctrl, NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const* flash_config,
					uint32_t nv_data_memory_size, uint32_t waitstate_timeout_ms,  bool_t checksum_enabled,
					cback_func_t cb_func ) :
	NV_Ctrl(),
	m_real_nv_ctrl( nv_ctrl ),
	m_flash_config( flash_config ),
	m_state( STATE_IDLE ),
	m_plain_text_buff( nullptr ),
	m_cb_func( cb_func ),
	m_wait_state_timeout_counter( 0U ),
	m_wait_state_timeout( waitstate_timeout_ms ),
	m_nv_block_size( nv_data_memory_size ),
	m_checksum_enabled( checksum_enabled )
{
	NV_Ctrl::nv_status_t nv_status = NV_Ctrl::BUSY_ERROR;
	NV_CTRL_ADDRESS_TD crc_add = 0U;
	uint16_t block_crc = 0;
	uint16_t length = 0;

	m_real_data_size = m_nv_block_size - FLASH_NV_CHECKSUM_LEN;

	m_plain_text_buff = reinterpret_cast<uint8_t*>( Ram::Allocate( m_real_data_size ) );

	BF_ASSERT( m_plain_text_buff );
	BF_Lib::Copy_Val_To_String( m_plain_text_buff, 0, m_real_data_size );

	new BF::System_Reset( &Reset_CallBack_Static,
						  reinterpret_cast<BF::System_Reset::cback_param_t>( this ) );

	/* Read Length from active copy of flash nv */
	NV_CTRL_ADDRESS_TD length_add = m_flash_config->start_address;

	nv_status = m_real_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &length ), length_add, FLASH_NV_LENGTH, false );

	// If length of map changes, checking CRC of existing map stored in NV
	if ( ( ( m_real_data_size > length ) && ( nv_status == NV_Ctrl::SUCCESS ) ) )
	{
		/* Read CRC from active copy of flash nv */
		crc_add = m_flash_config->start_address;
		nv_status = m_real_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &block_crc ), crc_add, FLASH_NV_CHECKSUM_LEN,
										  false );
		/* Verify active copy of flash nv */
		nv_status = m_real_nv_ctrl->Read( m_plain_text_buff, m_flash_config->start_address, length, false );
		if ( BF_Lib::CRC16::Calc_On_String( m_plain_text_buff, length, MEM_CHECK_CRC_INIT ) != block_crc )
		{
			nv_status = NV_Ctrl::DATA_ERROR;
		}
		/* Read CRC from backup copy of flash nv */
		crc_add = m_flash_config->mirror_start_address + length;
		nv_status = m_real_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &block_crc ), crc_add, FLASH_NV_CHECKSUM_LEN,
										  false );
		/* Verify backup copy of flash nv */
		nv_status = m_real_nv_ctrl->Read( m_plain_text_buff, m_flash_config->mirror_start_address, length, false );
		if ( BF_Lib::CRC16::Calc_On_String( m_plain_text_buff, length, MEM_CHECK_CRC_INIT ) != block_crc )
		{
			nv_status = NV_Ctrl::DATA_ERROR;
		}
	}
	else
	{
		/* Read CRC from active copy of flash nv */
		crc_add = m_flash_config->start_address + m_real_data_size;
		nv_status = m_real_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &block_crc ), crc_add, FLASH_NV_CHECKSUM_LEN,
										  false );
		/* Verify active copy of flash nv */
		nv_status = m_real_nv_ctrl->Read( m_plain_text_buff, m_flash_config->start_address, m_real_data_size, false );
		if ( BF_Lib::CRC16::Calc_On_String( m_plain_text_buff, m_real_data_size, MEM_CHECK_CRC_INIT ) != block_crc )
		{
			nv_status = NV_Ctrl::DATA_ERROR;
		}
		/* Read CRC from backup copy of flash nv */
		crc_add = m_flash_config->mirror_start_address + m_real_data_size;
		nv_status = m_real_nv_ctrl->Read( reinterpret_cast<uint8_t*>( &block_crc ), crc_add, FLASH_NV_CHECKSUM_LEN,
										  false );
		/* Verify backup copy of flash nv */
		nv_status = m_real_nv_ctrl->Read( m_plain_text_buff, m_flash_config->mirror_start_address, m_real_data_size,
										  false );
		if ( BF_Lib::CRC16::Calc_On_String( m_plain_text_buff, m_real_data_size, MEM_CHECK_CRC_INIT ) != block_crc )
		{
			nv_status = NV_Ctrl::DATA_ERROR;
		}
	}

	if ( nv_status == NV_Ctrl::SUCCESS )
	{
		/* As backup copy is good, Let's restore active copy */
		nv_status = m_real_nv_ctrl->Write( m_plain_text_buff, m_flash_config->start_address,
										   m_real_data_size, false );

		block_crc =
			BF_Lib::CRC16::Calc_On_String( m_plain_text_buff, m_real_data_size, MEM_CHECK_CRC_INIT );

		NV_CTRL_ADDRESS_TD crc_add = m_flash_config->start_address + m_real_data_size;
		nv_status = m_real_nv_ctrl->Write( reinterpret_cast<uint8_t*>( &block_crc ), crc_add,
										   FLASH_NV_CHECKSUM_LEN, false );
		nv_status = m_real_nv_ctrl->Write( reinterpret_cast<uint8_t*>( &length ), length_add,
										   FLASH_NV_LENGTH, false );
	}


	if ( ( m_cb_func != nullptr ) && ( NV_Ctrl::SUCCESS != nv_status ) )
	{
		// ToDo:As both flash nv copy found corrupted. Now lets rely on DCI module for reloading default values.
		m_cb_func( nullptr, NV_Ctrl::DATA_ERROR );
	}

	m_cr_task = new CR_Tasker( &NV_Flash_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
							   CR_TASKER_IDLE_PRIORITY, "Flash_NV_Update_Task" );
	m_cr_task->Suspend();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Flash_NV::~Flash_NV( void )
{
	delete m_cr_task;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Flash_NV::Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									 NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	bool success = false;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		success = Read_String( data, address, length );

		if ( success == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to Read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to Read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Flash_NV::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											  NV_CTRL_LENGTH_TD length )
{
	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( checksum_dest ),
						 reinterpret_cast<uint8_t const*>( m_plain_text_buff + address + length ),
						 FLASH_NV_CHECKSUM_LEN );
	return ( NV_Ctrl::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Flash_NV::Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									  NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;

	if ( STATE_WRITE != m_state )
	{
		m_wait_state_timeout_counter = 0U;
		return_status = Write_Now( data, address, length );
		if ( NV_Ctrl::SUCCESS == return_status )
		{
			m_state = STATE_WAIT;
			m_cr_task->Resume();
		}
		else
		{
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to write from Address: %X, Length: %u, Protection: %d, CryptoState: %d",
					   address, length, use_protection, m_state );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Flash_NV::Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
										  NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	bool success = false;
	uint8_t write_count = 0U;


	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		do
		{
			write_count++;

			success = Write_String( data, address, length );
		}while ( ( !success ) && ( write_count < MAX_NUMBER_OF_RAM_NV_WRITES ) );

		if ( success == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Flash_NV::Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									  uint8_t erase_data, bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::SUCCESS;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		if ( STATE_WRITE != m_state )
		{
			BF_Lib::Copy_Val_To_String( ( m_plain_text_buff + address ), erase_data, length );
		}
		else
		{
			return_status = NV_Ctrl::BUSY_ERROR;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
	}

	if ( NV_Ctrl::SUCCESS != return_status )
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to erase from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
					   address, length, protected_data, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Flash_NV::Erase_All( void )
{
	return ( Erase( 0U, m_nv_block_size, uC_FLASH_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Flash_NV::Read_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							NV_CTRL_LENGTH_TD length )
{
	uint16_t read_checksum_value = 0U;
	uint16_t calculated_checksum_value = 0U;
	bool good_data = false;

	if ( m_checksum_enabled == true )
	{
		calculated_checksum_value = BF_Lib::Copy_String_Ret_Checksum( data, ( m_plain_text_buff + address ), length );
		Read_Checksum( &read_checksum_value, address, length );
		if ( read_checksum_value == calculated_checksum_value )
		{
			good_data = true;
		}
		else
		{

			good_data = false;

		}

	}
	else
	{
		BF_Lib::Copy_String( data, ( m_plain_text_buff + address ), length );
		good_data = true;
	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Flash_NV::Write_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							 NV_CTRL_LENGTH_TD length )
{
	uint16_t read_checksum_value = 0U;
	uint16_t calculated_checksum_value = 0U;
	bool good_data = false;

	if ( m_checksum_enabled == true )
	{
		calculated_checksum_value = BF_Lib::Copy_String_Ret_Checksum( ( m_plain_text_buff + address ), data, length );

		BF_Lib::Copy_String( ( m_plain_text_buff + address + length ),
							 reinterpret_cast<uint8_t const*>( &calculated_checksum_value ), FLASH_NV_CHECKSUM_LEN );
		Read_Checksum( &read_checksum_value, address, length );
		if ( read_checksum_value == calculated_checksum_value )
		{
			good_data = true;
		}
		else
		{
			good_data = false;
		}
	}
	else
	{
		BF_Lib::Copy_String( ( m_plain_text_buff + address ), data, length );
		good_data = true;

	}

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Flash_NV::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
							bool protection ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;
	NV_CTRL_ADDRESS_TD hw_addr = m_flash_config->start_address + address;

	end_address = m_flash_config->end_address;

	if ( protection == true )
	{
		length += FLASH_NV_CHECKSUM_LEN;
		if ( MIRROR_SUPPORT == true )
		{
			end_address = m_flash_config->mirror_start_address;
		}
	}

	if ( ( hw_addr >= m_flash_config->start_address ) && ( ( hw_addr + length ) <= end_address ) )
	{
		range_good = true;
	}

	return ( range_good );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Flash_NV::Reset_CallBack( BF::System_Reset::reset_select_t reset_req )
{
	/*  */
	bool status = false;
	/* Skipping first iteration will allow m_state to change from IDLE to ENCRYPT */
	static bool first_run_done = false;

	if ( ( m_state == STATE_IDLE ) && ( first_run_done == true ) )
	{
		status = true;
		first_run_done = false;
	}

	first_run_done = true;
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Flash_NV::Flash_NV_Task( CR_Tasker* cr_task )
{
	uint16_t block_crc = 0;
	NV_CTRL_ADDRESS_TD crc_add = 0;

	CR_Tasker_Begin( cr_task );
	while ( 1 )
	{
		switch ( m_state )
		{
			case STATE_IDLE:
				break;

			case STATE_WAIT:
				m_wait_state_timeout_counter++;
				if ( m_wait_state_timeout_counter >= ( m_wait_state_timeout / CR_TASK_DELAY_MS ) )
				{
					m_state = STATE_WRITE;
					m_wait_state_timeout_counter = 0U;
				}
				break;

			case STATE_WRITE:
				/* Another WRITE request only gets accepted when engine in IDLE state */
				if ( MIRROR_SUPPORT == true )
				{
					block_crc =
						BF_Lib::CRC16::Calc_On_String( m_plain_text_buff, m_real_data_size, MEM_CHECK_CRC_INIT );

					/* Fill data into active flash nv area */
					m_real_nv_ctrl->Erase( m_flash_config->start_address, m_real_data_size );
					m_real_nv_ctrl->Write( m_plain_text_buff, m_flash_config->start_address, m_real_data_size, false );

					/* Update CRC on active flash nv area */
					crc_add = m_flash_config->start_address + m_real_data_size;
					m_real_nv_ctrl->Write( reinterpret_cast<uint8_t*>( &block_crc ), crc_add, FLASH_NV_CHECKSUM_LEN,
										   false );

					/* Fill data into backup flash nv area */
					m_real_nv_ctrl->Erase( m_flash_config->mirror_start_address, m_real_data_size );
					m_real_nv_ctrl->Write( m_plain_text_buff, m_flash_config->mirror_start_address, m_real_data_size,
										   false );

					/* Update CRC on backup flash nv area */
					crc_add = m_flash_config->mirror_start_address + m_real_data_size;
					m_real_nv_ctrl->Write( reinterpret_cast<uint8_t*>( &block_crc ), crc_add, FLASH_NV_CHECKSUM_LEN,
										   false );
				}
				else
				{
					block_crc =
						BF_Lib::CRC16::Calc_On_String( m_plain_text_buff, m_real_data_size, MEM_CHECK_CRC_INIT );

					/* Fill data into active flash nv area */
					m_real_nv_ctrl->Erase( m_flash_config->start_address, m_real_data_size );
					m_real_nv_ctrl->Write( m_plain_text_buff, m_flash_config->start_address, m_real_data_size, false );

					/* Update CRC on active flash nv area */
					crc_add = m_flash_config->start_address + m_real_data_size;
					m_real_nv_ctrl->Write( reinterpret_cast<uint8_t*>( &block_crc ), crc_add, FLASH_NV_CHECKSUM_LEN,
										   false );
				}
				m_state = STATE_IDLE;
				m_cr_task->Suspend();
				break;

			default:
				break;
		}
		CR_Tasker_Delay( cr_task, CR_TASK_DELAY_MS );
	}
	CR_Tasker_End();
}

}	// namespace NV_Ctrls end
