/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "NV_Crypto.h"
#include "Ram.h"
#include "StdLib_MV.h"
#include "System_Reset.h"
#include "NV_Ctrl_Debug.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Crypto::NV_Crypto( NV_Ctrl* nv_ctrl, Crypt* crypto_obj, nv_crypto_config_t const* crypto_config,
					  cback_func_t cb_func ) :
	NV_Ctrl(),
	m_real_nv_ctrl( nv_ctrl ),
	m_crypto_config( crypto_config ),
	m_state( STATE_IDLE ),
	m_encrypt_text_buff( nullptr ),
	m_plain_text_buff( nullptr ),
	m_crypto_obj( crypto_obj ),
	m_cb_func( cb_func )
{
	NV_Ctrl::nv_status_t read_encry_block_status = NV_Ctrl::BUSY_ERROR;
	Crypt::crypt_status_t crypt_status = Crypt::BUSY_ERROR;

	m_real_data_size = m_crypto_config->block_size;
	m_meta_data_size = m_crypto_config->meta_data_size;

	m_encrypt_text_buff = reinterpret_cast<uint8_t*>
		( Ram::Allocate( m_real_data_size + m_meta_data_size ) );

	m_plain_text_buff = reinterpret_cast<uint8_t*>
		( Ram::Allocate( m_real_data_size ) );

	new BF::System_Reset( &Reset_CallBack_Static,
						  reinterpret_cast<BF::System_Reset::cback_param_t>( this ) );

	/* Stop here if unable to allocate space for operation */
	BF_ASSERT( ( m_plain_text_buff != nullptr ) && ( m_encrypt_text_buff != nullptr ) );

	/* Read encrypted CRYPTO_NV_BLOCK */
	read_encry_block_status = m_real_nv_ctrl->Read( m_encrypt_text_buff,
													m_crypto_config->offset,
													m_real_data_size + m_meta_data_size, true );
	if ( read_encry_block_status == SUCCESS )
	{
		/* Decrypt CRYPTO_NV_BLOCK into plain text */
		uint32_t plain_text_size = m_real_data_size;
		crypt_status = m_crypto_obj->Decrypt( m_encrypt_text_buff, m_real_data_size + m_meta_data_size,
											  m_plain_text_buff, &plain_text_size );
	}

	if ( ( m_cb_func != nullptr ) && ( ( NV_Ctrl::SUCCESS != read_encry_block_status ) ||
									   ( Crypt::SUCCESS != crypt_status ) ) )
	{
		m_cb_func( nullptr, NV_Ctrl::DATA_ERROR );
	}

	m_cr_task = new CR_Tasker( &NV_Crypto_Task_Static, reinterpret_cast<CR_TASKER_PARAM>( this ),
							   CR_TASKER_IDLE_PRIORITY, "NV_Crypto_Routine" );
	m_cr_task->Suspend();

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Crypto::~NV_Crypto( void )
{
	// Todo
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t NV_Crypto::Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									  NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	bool success;

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
NV_Ctrl::nv_status_t NV_Crypto::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											   NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	uint8_t* data_ptr;
	uint16_t temp_checksum;

	if ( Check_Range( address, length ) )
	{

		data_ptr = &m_plain_text_buff[address];
		temp_checksum = 0U;
		for ( NV_CTRL_LENGTH_TD i = length; i > 0U; i-- )
		{
			temp_checksum += *data_ptr;
			data_ptr++;
		}
		*checksum_dest = temp_checksum;

		return_status = NV_Ctrl::SUCCESS;

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
NV_Ctrl::nv_status_t NV_Crypto::Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									   NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;

	if ( STATE_WRITE != m_state )
	{
		return_status = Write_Now( data, address, length );

		if ( NV_Ctrl::SUCCESS == return_status )
		{
			m_state = STATE_START_ENCRYPT;
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
NV_Ctrl::nv_status_t NV_Crypto::Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
										   NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::DATA_ERROR;
	bool success;
	uint8_t write_count;


	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		write_count = 0U;
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
NV_Ctrl::nv_status_t NV_Crypto::Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									   uint8_t erase_data, bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	if ( STATE_WRITE != m_state )
	{
		return_status = Erase_Now( address, length, erase_data );

		if ( NV_Ctrl::SUCCESS == return_status )
		{
			m_state = STATE_ENCRYPT;
		}
		else
		{
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to erase from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
						   address, length, protected_data, return_status );
		}
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t NV_Crypto::Erase_Now( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
										   uint8_t erase_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::SUCCESS;



	if ( Check_Range( address, length ) )
	{
		for ( uint32_t i = address; i < ( address + length ); i++ )
		{
			m_plain_text_buff[i] = erase_data;
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
NV_Ctrl::nv_status_t NV_Crypto::Erase_All( void )
{
	return ( Erase( m_crypto_config->start_address, m_real_data_size, 0U, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Crypto::Read_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							 NV_CTRL_LENGTH_TD length )
{

	BF_Lib::Copy_String_Ret_Checksum( data, ( m_plain_text_buff + address ), length );


	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Crypto::Write_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							  NV_CTRL_LENGTH_TD length )
{

	BF_Lib::Copy_String_Ret_Checksum( ( m_plain_text_buff + address ), data, length );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Crypto::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool range_good = false;

	if ( ( address + length ) <= m_real_data_size )
	{
		range_good = true;
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed range check Address: %X, Length: %u",
					   address, length );
		// If you hit this point then you have run outside of
		// the allocated nv_ram.  You likely need to check your address or increase the
		// nv_ram ram size.
		// This is used primarily for debug purposes.
		BF_ASSERT( false );
	}

	return ( range_good );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Crypto::Reset_CallBack( BF::System_Reset::reset_select_t reset_req )
{
	/*  */
	bool status = false;
	/* Skiping first interation will allow m_state to change from IDLE to ENCRYPT */
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
void NV_Crypto::NV_Crypto_Task( CR_Tasker* cr_task )
{
	uint32_t output_length = m_real_data_size + m_meta_data_size;

	switch ( m_state )
	{
		case STATE_IDLE:
			break;

		case STATE_START_ENCRYPT:
			m_state = STATE_ENCRYPT;
			break;

		case STATE_ENCRYPT:
			/* Encrypt the plain text data */
			output_length = m_real_data_size + m_meta_data_size;
			m_crypto_obj->Encrypt( m_plain_text_buff, m_real_data_size,
								   m_encrypt_text_buff, &output_length );
			if ( m_state == STATE_ENCRYPT )
			{
				/* This check added to protect from interrupt context switching. If another WRITE request comes in
				   interrupt context then engine state will change from STATE_ENCRYPT to STATE_START_ENCRYPT.
				   This check will detect this change and restart encryption */
				m_state = STATE_WRITE;
			}
			break;

		case STATE_WRITE:
			/* Write encrypted data block. In this state we are blocking all WRITE request.
			   Another WRITE request only gets accepted when engine in IDLE state */
			m_real_nv_ctrl->Write( m_encrypt_text_buff, m_crypto_config->offset,
								   m_real_data_size + m_meta_data_size, true );
			m_state = STATE_IDLE;
			m_cr_task->Suspend();
			break;

		default:
			break;
	}
}

}
