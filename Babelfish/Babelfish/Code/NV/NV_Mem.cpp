/**
   @file			NV_Mem.cpp implementation file for Direct Memory Access RX and TX functionality implementation
 *					with USART as main peripheral.
 *
 *	@brief			This file shall wrap all the functions required for NV memory interface.
 *
 *	@details        The basic purpose of having Non volatile Memory is to keep the data stored into the memory,
 *	                e.g. some data related to project which is going to be constant.
 *
 *  @copyright      2014 Eaton Corporation. All Rights Reserved.
 *
 *  @note           Eaton Corporation claims proprietary rights to the material disclosed
 *                  here in.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 */

#include "Includes.h"
#include "NV_Mem.h"
#include "NV_Default.h"
#include "NV_Address.h"
#include "uC_Code_Range.h"			// Should we include this here TBD. Need to refine to accomodate flash


namespace NV
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
NV_Ctrl** NV_Mem::m_nv_ctrl_list = reinterpret_cast<NV_Ctrl**>( nullptr );
uint_fast8_t NV_Mem::m_nv_ctrl_list_size = 0U;
uint8_t const* NV_Mem::m_nv_flush_key_list = reinterpret_cast<uint8_t const*>( nullptr );
NV_Mem::nv_error_cback_t NV_Mem::m_nv_error_cback =
	reinterpret_cast<NV_Mem::nv_error_cback_t>( nullptr );
NV_Mem::nv_error_cback_param_t NV_Mem::m_nv_error_cback_param =
	reinterpret_cast<NV_Mem::nv_error_cback_param_t>( nullptr );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void NV_Mem::Init( NV_Ctrl** nv_ctrl_list, uint8_t nv_ctrl_list_size,
				   uint8_t const* nv_flush_key_list, nv_error_cback_t nv_error_cback,
				   nv_error_cback_param_t nv_error_cback_param )
{
	uint8_t nv_flush_key_read;
	NV_Ctrl::nv_status_t nv_status;
	uint8_t nv_flush_key;

	m_nv_ctrl_list = nv_ctrl_list;
	m_nv_ctrl_list_size = nv_ctrl_list_size;
	m_nv_flush_key_list = nv_flush_key_list;

	m_nv_error_cback = nv_error_cback;
	m_nv_error_cback_param = nv_error_cback_param;

	for ( uint8_t nv_ctrl_id = 0U; nv_ctrl_id < nv_ctrl_list_size; nv_ctrl_id++ )
	{
		nv_status = nv_ctrl_list[nv_ctrl_id]->Read( &nv_flush_key_read, NV_CTRL_MEM_INIT_DONE_NVADD,
													1U, true );

		if ( nv_flush_key_list != NULL )
		{
			nv_flush_key = nv_flush_key_list[nv_ctrl_id];
		}
		else
		{
			nv_flush_key = NV_MEM_FLUSH_COMPLETE;
		}
		if ( ( nv_status == NV_Ctrl::DATA_ERROR ) || ( nv_flush_key_read != nv_flush_key ) )
		{
			if ( !Erase_All_Single_Id( nv_ctrl_id ) )
			{
				nv_status = NV_Ctrl::DATA_ERROR;
			}
		}
		if ( nv_status != NV_Ctrl::SUCCESS )
		{
			if ( m_nv_error_cback != NULL )
			{
				( *m_nv_error_cback )( m_nv_error_cback_param, nv_status,
									   INITIALIZATION, nv_ctrl_id, NV_CTRL_MEM_INIT_DONE_NVADD );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Mem::Read( uint8_t* data, uint32_t address, uint32_t length, bool use_protection )
{
	NV_Ctrl::nv_status_t nv_status = NV_Ctrl::INVALID_ADDRESS;
	uint8_t nv_ctrl_id;

	nv_ctrl_id = static_cast<uint8_t>( address >> NV_ADDRESS_CTRL_SELECT_SHIFT );

	if ( ( nv_ctrl_id < m_nv_ctrl_list_size ) && ( m_nv_ctrl_list[nv_ctrl_id] != NULL ) )
	{
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-2-7, 5-2-8, 5-2-9, cast from enum to unsigned short
		 *@n Justification: PC-lint false alarm.  5-2-7, 5-2-8, and 5-2-9 involve casting pointers, not enums.
		 */
		/*lint -e{930} # MISRA Deviation */
		nv_status = static_cast<NV_Ctrl::nv_status_t>( m_nv_ctrl_list[nv_ctrl_id]->Read( data,
																						 ( address &
																						   NV_ADDRESS_CTRL_ADDRESS_MASK ),
																						 length, use_protection ) );
	}

	if ( nv_status != NV_Ctrl::SUCCESS )
	{
		if ( m_nv_error_cback != NULL )
		{
			( *m_nv_error_cback )( m_nv_error_cback_param, nv_status, READ_OP, nv_ctrl_id, address );
		}
	}

	return ( nv_status == NV_Ctrl::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Mem::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
							NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t nv_status = NV_Ctrl::INVALID_ADDRESS;
	uint8_t nv_ctrl_id;

	nv_ctrl_id = static_cast<uint8_t>( address >> NV_ADDRESS_CTRL_SELECT_SHIFT );

	if ( ( nv_ctrl_id < m_nv_ctrl_list_size ) && ( m_nv_ctrl_list[nv_ctrl_id] != NULL ) )
	{
		nv_status = m_nv_ctrl_list[nv_ctrl_id]->Read_Checksum( checksum_dest,
															   ( address & NV_ADDRESS_CTRL_ADDRESS_MASK ), length );
	}

	if ( nv_status != NV_Ctrl::SUCCESS )
	{
		if ( m_nv_error_cback != NULL )
		{
			( *m_nv_error_cback )( m_nv_error_cback_param, nv_status, READ_CHECKSUM_OP,
								   nv_ctrl_id, address );
		}
	}

	return ( nv_status == NV_Ctrl::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Mem::Write( uint8_t* data, uint32_t address, uint32_t length, bool use_protection )
{
	NV_Ctrl::nv_status_t nv_status = NV_Ctrl::INVALID_ADDRESS;
	uint8_t nv_ctrl_id;

	nv_ctrl_id = static_cast<uint8_t>( address >> NV_ADDRESS_CTRL_SELECT_SHIFT );

	if ( ( nv_ctrl_id < m_nv_ctrl_list_size ) && ( m_nv_ctrl_list[nv_ctrl_id] != NULL ) )
	{
		nv_status = m_nv_ctrl_list[nv_ctrl_id]->Write( data,
													   ( address & NV_ADDRESS_CTRL_ADDRESS_MASK ), length,
													   use_protection );
	}

	if ( nv_status != NV_Ctrl::SUCCESS )
	{
		if ( m_nv_error_cback != NULL )
		{
			( *m_nv_error_cback )( m_nv_error_cback_param, nv_status, WRITE_OP, nv_ctrl_id, address );
		}
	}

	return ( nv_status == NV_Ctrl::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Mem::Erase( uint32_t address, uint32_t length, uint8_t erase_data, bool protected_data )
{
	NV_Ctrl::nv_status_t nv_status = NV_Ctrl::INVALID_ADDRESS;
	uint8_t nv_ctrl_id;

	nv_ctrl_id = static_cast<uint8_t>( address >> NV_ADDRESS_CTRL_SELECT_SHIFT );

	if ( ( nv_ctrl_id < m_nv_ctrl_list_size ) && ( m_nv_ctrl_list[nv_ctrl_id] != NULL ) )
	{
		nv_status = m_nv_ctrl_list[nv_ctrl_id]->Erase(
			( address & NV_ADDRESS_CTRL_ADDRESS_MASK ),
			length, erase_data, protected_data );
	}

	if ( nv_status != NV_Ctrl::SUCCESS )
	{
		if ( m_nv_error_cback != NULL )
		{
			( *m_nv_error_cback )( m_nv_error_cback_param, nv_status, ERASE_OP, nv_ctrl_id, address );
		}
	}

	return ( nv_status == NV_Ctrl::SUCCESS );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Mem::Erase_All( void )
{
	bool erase_success = true;

	for ( uint_fast8_t i = 0U; i < m_nv_ctrl_list_size; i++ )
	{
		if ( !Erase_All_Single_Id( i ) )
		{
			erase_success = false;
		}
	}

	return ( erase_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool NV_Mem::Erase_All_Single_Id( uint8_t nv_ctrl_id )
{
	NV_Ctrl::nv_status_t nv_status;
	bool erase_success = false;
	uint8_t nv_flush_key;

	nv_status = m_nv_ctrl_list[nv_ctrl_id]->Erase_All();
	if ( nv_status == NV_Ctrl::SUCCESS )
	{
		Load_Defaults( nv_ctrl_id );

		if ( m_nv_flush_key_list != NULL )
		{
			nv_flush_key = m_nv_flush_key_list[nv_ctrl_id];
		}
		else
		{
			nv_flush_key = NV_MEM_FLUSH_COMPLETE;
		}

		if ( m_nv_ctrl_list[nv_ctrl_id]->Write( &nv_flush_key, NV_CTRL_MEM_INIT_DONE_NVADD,
												sizeof ( nv_flush_key ), true ) == NV_Ctrl::SUCCESS )
		{
			erase_success = true;
		}
	}
	else
	{
		erase_success = false;
		if ( m_nv_error_cback != NULL )
		{
			( *m_nv_error_cback )( m_nv_error_cback_param, nv_status, ERASE_ALL_OP, nv_ctrl_id,
								   NV_CTRL_MEM_INIT_DONE_NVADD );
		}
	}

	return ( erase_success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void NV_Mem::Load_Defaults( uint8_t nv_ctrl_id )
{
#if NV_DEFAULT_LIST_ITEM_COUNT != 0
	NV_DEFAULT_LIST_ITEM_TD const* nv_def;
	bool write_success;

	for ( uint32_t def_counter = 0U; def_counter < NV_DEFAULT_LIST_ITEM_COUNT; def_counter++ )
	{
		nv_def = &nv_def_list[def_counter];
		if ( ( nv_def->address >> NV_ADDRESS_CTRL_SELECT_SHIFT ) == nv_ctrl_id )
		{
			write_success = Write( ( uint8_t* )nv_def->data, nv_def->address, nv_def->length, true );
		}
	}
#endif
	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'nv_ctrl_id' not referenced
	 * @n PCLint:
	 * @n Justification: In some cases we are not using a variable.  Rather than create
	 * confusing code where we assign the param to itself which could imply an error or
	 * peppering the code with lint exceptions we mark a variable as being unused using this
	 * template which clearly indicates our intention.
	 */
	/*lint -e{715}*/
}

}
