/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Ram_NV.h"
#include "Ram.h"
#include "StdLib_MV.h"
#include "NV_Ctrl_Debug.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MAX_NUMBER_OF_RAM_NV_WRITES     2U
#define RAM_NV_CHECKSUM_LEN             2U

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
Ram_NV::Ram_NV( uint32_t nv_ram_block_size, bool include_checksum ) :
	m_nv_memory( reinterpret_cast<uint8_t*>( Ram::Allocate( nv_ram_block_size ) ) ),
	m_nv_mem_size( nv_ram_block_size ),
	m_checksum_exists( include_checksum ),
	NV_Ctrl()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Ram_NV::~Ram_NV( void )
{
	BF_ASSERT( false );	// this class is not supposed to be destructed
	delete m_nv_memory;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Ram_NV::Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status;
	bool success;

	if ( m_checksum_exists == false )
	{
		use_protection = false;
	}

	if ( Check_Range( address, length, use_protection ) && ( length != 0U ) )
	{
		success = Read_String( data, address, length, use_protection );

		if ( success == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Ram_NV::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t return_status;
	uint8_t* data_ptr;
	uint16_t temp_checksum;

	if ( Check_Range( address, length, false ) )
	{
		// if the checksum doesn't exist then we need to calculate it.
		if ( m_checksum_exists == false )
		{
			data_ptr = &m_nv_memory[address];
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
			return_status = Read( reinterpret_cast<uint8_t*>( checksum_dest ), address + length,
								  RAM_NV_CHECKSUM_LEN, false );
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed read checksum Address: %X, Length: %u, ErrorStatus: %d",
					   address, length, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Ram_NV::Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t return_status;
	bool success;
	uint8_t write_count;

	if ( m_checksum_exists == false )
	{
		use_protection = false;
	}

	if ( Check_Range( address, length, use_protection ) && ( length != 0U ) )
	{
		write_count = 0U;
		do
		{
			write_count++;

			success = Write_String( data, address, length, use_protection );
		}while ( ( !success ) && ( write_count < MAX_NUMBER_OF_RAM_NV_WRITES ) );

		if ( success == true )
		{
			return_status = NV_Ctrl::SUCCESS;
		}
		else
		{
			return_status = NV_Ctrl::DATA_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ram_NV::Read_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
						  NV_CTRL_LENGTH_TD length, bool use_protection )
{
	bool success = false;
	BF_Lib::SPLIT_UINT16 checksum;

	checksum.u16 = BF_Lib::Copy_String_Ret_Checksum( data, ( m_nv_memory + address ), length );

	if ( ( use_protection != true ) ||
		 ( ( m_nv_memory[address + length] == checksum.u8[0] ) &&
		   ( m_nv_memory[address + length + 1U] == checksum.u8[1] ) ) )
	{
		success = true;
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ram_NV::Write_String( uint8_t* data, NV_CTRL_ADDRESS_TD address,
						   NV_CTRL_LENGTH_TD length, bool use_protection )
{
	bool success = true;
	BF_Lib::SPLIT_UINT16 checksum;

	checksum.u16 = BF_Lib::Copy_String_Ret_Checksum( ( m_nv_memory + address ), data, length );
	if ( use_protection == true )
	{
		m_nv_memory[address + length] = checksum.u8[0];
		m_nv_memory[address + length + 1U] = checksum.u8[1];
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Ram_NV::Erase( NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, uint8_t erase_data, bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::SUCCESS;

	if ( m_checksum_exists == false )
	{
		protected_data = false;
	}

	if ( true == protected_data )
	{
		length += RAM_NV_CHECKSUM_LEN;	// Checksum length is 2.
	}

	if ( Check_Range( address, length, protected_data ) )
	{
		for ( uint32_t i = address; i < ( address + length ); i++ )
		{
			m_nv_memory[i] = erase_data;
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to erase from Address: %X, Length: %u, ProtectedData: %d, ErrorStatus: %d",
					   address, length, protected_data, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t Ram_NV::Erase_All( void )
{
	return ( Erase( 0U, m_nv_mem_size, 0U, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Ram_NV::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						  bool protection ) const
{
	bool range_good = false;

	if ( protection == true )
	{
		length += RAM_NV_CHECKSUM_LEN;
	}

	if ( ( address + length ) <= m_nv_mem_size )
	{
		range_good = true;
	}
	else
	{
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed range check for Address: %X, Length: %u, Protection: %d",
					   address, length, protection );

		// If you hit this point then you have run outside of
		// the allocated nv_ram.  You likely need to check your address or increase the
		// nv_ram ram size.
		// This is used primarily for debug purposes.
		BF_ASSERT( false );
	}

	return ( range_good );
}

}

