/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Fram.h"
#include "Exception.h"
#include "NV_Ctrl_Debug.h"

namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define FRAM_WRITE_STATUS_REGISTER_OPCODE       0x01U
#define FRAM_WRITE_MEMORY_ADD_DATA_OPCODE       0x02U
#define FRAM_READ_MEMORY_ADD_DATA_OPCODE        0x03U
#define FRAM_WRITE_DISABLE_OPCODE               0x04U
#define FRAM_READ_STATUS_REGISTER_OPCODE        0x05U
#define FRAM_SET_WRITE_ENABLE_LATCH_OPCODE      0x06U
#define FRAM_READ_DEVICE_ID_OPCODE              0x9FU

// #define FRAM_STATUS_BLOCK_PROTECT_0_TO_7FF		0x0CU
// #define FRAM_STATUS_BLOCK_PROTECT_400_TO_7FF	0x08U
// #define FRAM_STATUS_BLOCK_PROTECT_600_TO_7FF	0x04U
// #define FRAM_STATUS_BLOCK_PROTECT_600_TO_7FF	0x00U
#define FRAM_STATUS_WRITE_PROTECT_ENABLE_BIT    7U
#define FRAM_STATUS_BLOCK0_PROTECT_ENABLE_BIT   3U
#define FRAM_STATUS_BLOCK1_PROTECT_ENABLE_BIT   2U
#define FRAM_STATUS_WRITES_ENABLED_LATCH_BIT    1U

#define FRAM_STATUS_WRITE_PROTECT_ENABLE_BITMASK    ( 1U << FRAM_STATUS_WRITE_PROTECT_ENABLE_BIT )
#define FRAM_STATUS_BLOCK0_PROTECT_ENABLE_BITMASK   ( 1U << FRAM_STATUS_BLOCK0_PROTECT_ENABLE_BIT )
#define FRAM_STATUS_BLOCK1_PROTECT_ENABLE_BITMASK   ( 1U << FRAM_STATUS_BLOCK1_PROTECT_ENABLE_BIT )
#define FRAM_STATUS_WRITES_ENABLED_LATCH_BITMASK    ( 1U << FRAM_STATUS_WRITES_ENABLED_LATCH_BIT )

// The number of bits that the upper bits need to be shifted to get it into the
// right space in the write opcode.  This is only for the single byte address type.
#define UPPER_ADDRESS_BIT_SHIFT                 3U

#define MAX_NUMBER_OF_FRAM_WRITES       2U
#define MAX_NUMBER_OF_FRAM_READS        2U

#define Delay_Chip_Select_Change()      uC_Delay( 1U )

#define FRAM_NV_CHECKSUM_LEN            2U
#define CONTINUATION_CODE         0x7FU

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
FRAM::FRAM( uC_SPI* spi_ctrl, uint8_t chip_select, FRAM_CHIP_CONFIG_TD const* chip_config ) :
	NV_Ctrl(),
	m_spi_ctrl( spi_ctrl ),
	m_chip_select( chip_select ),
	m_chip_cfg( chip_config )
{
	uint8_t status = 0U;

	m_spi_ctrl->Configure_Chip_Select( static_cast<uint8_t>( uC_SPI_MODE_0 ), chip_select,
									   chip_config->max_clock_freq,
									   static_cast<uint8_t>( uC_SPI_BIT_COUNT_8 ), true );

	Read_Status( &status );

	if ( status != 0U )
	{
		status = 0U;
		Write_Status( status );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FRAM::Execute_Op_Code( uint8_t op_code, uint8_t* rx_data, uint16_t rx_data_len )
{
	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( op_code );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}
	for ( uint16_t i = 0; i < rx_data_len; i++ )
	{
		rx_data[i] = m_spi_ctrl->RX_Byte();
	}
	m_spi_ctrl->De_Select_Chip( m_chip_select );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FRAM::~FRAM( void )
{
	m_spi_ctrl = reinterpret_cast<uC_SPI*>( nullptr );
	m_chip_cfg = reinterpret_cast<FRAM_CHIP_CONFIG_TD const*>( nullptr );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t FRAM::Read( uint8_t* data, uint32_t address, uint32_t length,
								 bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;
	uint_fast8_t read_count;

	if ( Check_Range( address, length, use_protection ) && ( length != 0U ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( FRAM_SPI_SEMAPHORE_TIMEOUT ) )
		{
			read_count = 0U;
			do
			{
				read_count++;
				good_data = Read_Now( data, address, length, use_protection );
			}while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_FRAM_READS ) );

			// if the data didn't read right, go check the backup location to see if
			// our good data is there.
			if ( ( MIRROR_SUPPORT == true ) && ( !good_data ) && ( use_protection == true ) )
			{
				good_data = true;

				read_count = 0U;
				do
				{
					read_count++;
					good_data = Read_Now( data, ( address + m_chip_cfg->mirror_start_address ),
										  length, use_protection );
				}while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_FRAM_READS ) );

				if ( good_data == true )
				{
					good_data = Write_Now( data, address, length, use_protection );
				}
			}

			if ( good_data == true )
			{
				return_status = NV_Ctrl::SUCCESS;
			}
			else
			{
				return_status = NV_Ctrl::DATA_ERROR;
				NV_CTRL_PRINT( DBG_MSG_ERROR,
							   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
							   address, length, use_protection, return_status );
			}

			m_spi_ctrl->Release_Semaphore();
		}
		else
		{
			return_status = NV_Ctrl::BUSY_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t FRAM::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
										  NV_CTRL_LENGTH_TD length )
{
	return ( Read( reinterpret_cast<uint8_t*>( checksum_dest ), address + length,
				   FRAM_NV_CHECKSUM_LEN, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t FRAM::Write( uint8_t* data, uint32_t address, uint32_t length,
								  bool use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool good_data;
	uint_fast8_t write_count;

	if ( Check_Range( address, length, use_protection ) && ( length != 0U ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( FRAM_SPI_SEMAPHORE_TIMEOUT ) )
		{
			write_count = 0U;
			do
			{
				write_count++;

				good_data = Write_Now( data, address, length, use_protection );
				if ( ( use_protection == true ) && good_data )
				{
					good_data = Check_Data( data, address, length );
				}
			}while ( ( !good_data ) && ( write_count < MAX_NUMBER_OF_FRAM_WRITES ) );

			if ( ( MIRROR_SUPPORT == true ) && ( good_data == true ) && ( use_protection == true ) )
			{
				write_count = 0U;
				do
				{
					write_count++;

					good_data = Write_Now( data, ( address + m_chip_cfg->mirror_start_address ),
										   length, use_protection );
					if ( good_data )
					{
						good_data = Check_Data( data,
												( address + m_chip_cfg->mirror_start_address ), length );
					}
				}while ( ( !good_data ) && ( write_count < MAX_NUMBER_OF_FRAM_WRITES ) );
			}

			if ( good_data == true )
			{
				return_status = NV_Ctrl::SUCCESS;
			}
			else
			{
				return_status = NV_Ctrl::DATA_ERROR;
				NV_CTRL_PRINT( DBG_MSG_ERROR,
							   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
							   address, length, use_protection, return_status );
			}

			m_spi_ctrl->Release_Semaphore();
		}
		else
		{
			return_status = NV_Ctrl::BUSY_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
						   address, length, use_protection, return_status );
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %u",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t FRAM::Erase( uint32_t address, uint32_t length, uint8_t erase_data,
								  bool protected_data )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	BF_Lib::SPLIT_UINT16 temp_address;
	uint8_t block_count;
	uint8_t max_block_count;

	if ( MIRROR_SUPPORT == true )
	{
		max_block_count = 2U;
	}
	else
	{
		max_block_count = 1U;
	}

	if ( Check_Range( address, length, protected_data ) )
	{
		if ( m_spi_ctrl->Get_Semaphore( FRAM_SPI_SEMAPHORE_TIMEOUT ) )
		{
			temp_address.u16 = static_cast<uint16_t>( address );// FRAM_FLUSH_START_ADDRESS_NVADD;

			block_count = 0U;
			do
			{
				m_spi_ctrl->Select_Chip( m_chip_select );	// SPI_PERIPH_Enable_CS( FRAM_DEVICE_ID );

				m_spi_ctrl->TX_Byte( FRAM_SET_WRITE_ENABLE_LATCH_OPCODE );
				while ( !m_spi_ctrl->TX_Shift_Empty() )
				{}

				m_spi_ctrl->De_Select_Chip( m_chip_select );// SPI_PERIPH_Disable_CS( FRAM_DEVICE_ID );

				Delay_Chip_Select_Change();

				m_spi_ctrl->Select_Chip( m_chip_select );	// SPI_PERIPH_Enable_CS( FRAM_DEVICE_ID );

				Send_Data_Command( FRAM_WRITE_MEMORY_ADD_DATA_OPCODE, temp_address.u16 );

				if ( true == protected_data )
				{
					length += FRAM_NV_CHECKSUM_LEN;
				}
				for ( uint_fast16_t i = 0U; i < length; i++ )
				{
					m_spi_ctrl->TX_Byte( erase_data );
					while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
					{}
				}

				if ( MIRROR_SUPPORT == true )
				{
					temp_address.u16 = static_cast<uint16_t>
						( m_chip_cfg->mirror_start_address + address );
				}

				while ( !m_spi_ctrl->TX_Shift_Empty() )
				{}
				m_spi_ctrl->De_Select_Chip( m_chip_select );

				block_count++;
			}while ( ( block_count < max_block_count ) && ( protected_data == true ) );	// If this erase was not a
																						// protected value loc exit out.

			return_status = NV_Ctrl::SUCCESS;

			m_spi_ctrl->Release_Semaphore();
		}
	}
	else
	{
		return_status = NV_Ctrl::INVALID_ADDRESS;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to Erase from Address: %X, Length: %u, ProtectedData: %d, ErrorStatus: %u",
					   address, length, protected_data, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t FRAM::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					m_chip_cfg->end_address, 0U, false ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FRAM::Read_Now( uint8_t* data, uint32_t address, uint32_t length, bool use_protection )
{
	bool good_data = false;
	uint_fast16_t rx_checksum_low;
	uint_fast16_t rx_checksum_high;
	uint_fast16_t calc_checksum;

	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( FRAM_READ_MEMORY_ADD_DATA_OPCODE, address );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	calc_checksum = 0U;
	while ( length > 0U )
	{
		length--;
		*data = m_spi_ctrl->RX_Byte();
		calc_checksum += *data;
		data++;
	}
	calc_checksum &= 0xFFFFU;

	if ( use_protection == true )
	{
		rx_checksum_low = m_spi_ctrl->RX_Byte();
		rx_checksum_high = m_spi_ctrl->RX_Byte();
		rx_checksum_low |= rx_checksum_high << 8;

		if ( rx_checksum_low == calc_checksum )
		{
			good_data = true;
		}
	}
	else
	{
		good_data = true;
	}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FRAM::Write_Now( uint8_t* data, uint32_t address, uint32_t length, bool use_protection )
{
	uint_fast16_t checksum;

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->TX_Byte( FRAM_SET_WRITE_ENABLE_LATCH_OPCODE );
	// while ( !m_spi_ctrl->TX_Data_Reg_Empty() );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	// An edge has to be sent to the Chip select pin on a write instruction.
	// Nops are there to give at least a 5Mhz pulse width.
	Delay_Chip_Select_Change();

	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( FRAM_WRITE_MEMORY_ADD_DATA_OPCODE, address );

	checksum = 0U;
	for ( uint_fast16_t i = 0U; i < length; i++ )
	{
		m_spi_ctrl->TX_Byte( *data );
		checksum += *data;
		data++;
		while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
		{}
	}
	if ( use_protection == true )
	{
		m_spi_ctrl->TX_Byte( static_cast<uint8_t>( checksum & 0xFFU ) );
		while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
		{}
		checksum = checksum >> 8;
		m_spi_ctrl->TX_Byte( static_cast<uint8_t>( checksum & 0xFFU ) );
		// while ( !m_spi_ctrl->TX_Data_Reg_Empty() );
	}
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FRAM::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length )
{
	bool good_data = true;
	uint8_t read_data;
	BF_Lib::SPLIT_UINT16 checksum;

	m_spi_ctrl->Select_Chip( m_chip_select );

	Send_Data_Command( FRAM_READ_MEMORY_ADD_DATA_OPCODE, address );
	while ( !m_spi_ctrl->TX_Shift_Empty() )
	{}

	checksum.u16 = 0U;
	while ( length > 0U )
	{
		length--;
		read_data = m_spi_ctrl->RX_Byte();

		if ( *check_data != read_data )
		{
			good_data = false;
		}
		check_data++;
		checksum.u16 = read_data + checksum.u16;
	}

	if ( good_data == true )
	{
		read_data = m_spi_ctrl->RX_Byte();
		if ( read_data != checksum.u8[0] )
		{
			good_data = false;
		}

		read_data = m_spi_ctrl->RX_Byte();
		if ( read_data != checksum.u8[1] )
		{
			good_data = false;
		}
	}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( good_data );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void FRAM::Send_Data_Command( uint8_t fram_command, uint32_t address )
{
	BF_Lib::SPLIT_UINT32 temp_address;

	temp_address.u32 = address;

	switch ( m_chip_cfg->address_length )
	{
		case 1:
			m_spi_ctrl->TX_Byte(
				fram_command
				| static_cast<uint8_t>( temp_address.u8[1] << UPPER_ADDRESS_BIT_SHIFT ) );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			m_spi_ctrl->TX_Byte( temp_address.u8[0] );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			break;

		default:
			m_spi_ctrl->TX_Byte( fram_command );
			while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
			{}
			for ( uint_fast8_t i = m_chip_cfg->address_length; i > 0U; i-- )
			{
				m_spi_ctrl->TX_Byte( temp_address.u8[i - 1U] );
				while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
				{}
			}
			break;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FRAM::Read_Status( uint8_t* status )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( FRAM_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( FRAM_READ_STATUS_REGISTER_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		*status = m_spi_ctrl->RX_Byte();

		success = true;

		m_spi_ctrl->De_Select_Chip( m_chip_select );
		m_spi_ctrl->Release_Semaphore();
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FRAM::Write_Status( uint8_t status )
{
	bool success = false;

	if ( m_spi_ctrl->Get_Semaphore( FRAM_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( FRAM_SET_WRITE_ENABLE_LATCH_OPCODE );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		m_spi_ctrl->De_Select_Chip( m_chip_select );

		// An edge has to be sent to the Chip select pin on a write instruction.
		// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
		Delay_Chip_Select_Change();

		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte( FRAM_WRITE_STATUS_REGISTER_OPCODE );
		while ( !m_spi_ctrl->TX_Data_Reg_Empty() )
		{}
		m_spi_ctrl->TX_Byte( status );
		while ( !m_spi_ctrl->TX_Shift_Empty() )
		{}

		success = true;

		m_spi_ctrl->De_Select_Chip( m_chip_select );
		m_spi_ctrl->Release_Semaphore();
	}

	return ( success );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FRAM::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						bool protection ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	end_address = m_chip_cfg->end_address;
	if ( protection == true )
	{
		length += FRAM_NV_CHECKSUM_LEN;
		if ( MIRROR_SUPPORT == true )
		{
			end_address = m_chip_cfg->mirror_start_address;
		}
	}

	if ( ( address >= m_chip_cfg->start_address ) && ( ( address + length ) <= end_address ) )
	{
		range_good = true;
	}

	return ( range_good );
}

}
