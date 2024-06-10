/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "RTC_NV.h"
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
namespace NV_Ctrls
{

RTC_NV* RTC_NV::m_rtc_nv_instance = nullptr;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
RTC_NV::RTC_NV( uC_RTC_Backup* rtc_bkp_handle, RTC_NV_CONFIG_TD const* rtc_nv_config ) :
	NV_Ctrl(),
	m_RTC_backup_handle( rtc_bkp_handle ),
	m_nv_cfg( rtc_nv_config )
{
	m_rtc_nv_instance = this;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
RTC_NV::~RTC_NV( void )
{
	// TODO
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t RTC_NV::Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t status = SUCCESS;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		if ( true != Read_Now( data, address, length ) )
		{
			status = DATA_ERROR;
		}
	}
	else
	{
		status = INVALID_ADDRESS;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool RTC_NV::Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool status = true;
	bool loop_break = false;
	uint32_t read_val = 0U;
	uint8_t* read_val_ptr = reinterpret_cast<uint8_t*>( &read_val );
	const uint8_t BKP_REG_SIZE = uC_RTC_Backup::RTC_BKP_REG_BYTE_SIZE;
	uint8_t bytes_to_copy = BKP_REG_SIZE;

	uint8_t bkp_reg_num = address / BKP_REG_SIZE;

	while ( loop_break == false )
	{
		read_val = Read_Backup_Register( bkp_reg_num );
		BF_Lib::Copy_Data( data, BKP_REG_SIZE, read_val_ptr, bytes_to_copy );
		if ( length > BKP_REG_SIZE )
		{
			bytes_to_copy = BKP_REG_SIZE;
			length -= BKP_REG_SIZE;
			bkp_reg_num++;
			data += BKP_REG_SIZE;
		}
		else
		{
			bytes_to_copy = length;
			loop_break = true;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t RTC_NV::Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool use_protection )
{
	NV_Ctrl::nv_status_t status = SUCCESS;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		if ( true != Write_Now( data, address, length ) )
		{
			status = DATA_ERROR;
		}
	}
	else
	{
		status = INVALID_ADDRESS;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool RTC_NV::Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	bool status = true;
	bool loop_break = false;
	uint32_t write_val = 0U;
	uint32_t old_val = 0U;
	uint8_t* write_val_ptr = reinterpret_cast<uint8_t*>( &write_val );
	const uint8_t BKP_REG_SIZE = uC_RTC_Backup::RTC_BKP_REG_BYTE_SIZE;
	uint8_t bytes_to_copy = ( length > BKP_REG_SIZE ) ? ( BKP_REG_SIZE ) : ( length );

	uint8_t bkp_reg_num = address / BKP_REG_SIZE;

	while ( loop_break == false )
	{
		BF_Lib::Copy_Data( write_val_ptr, BKP_REG_SIZE, data, bytes_to_copy );

		old_val = Read_Backup_Register( bkp_reg_num );

		// As one backup register holds 4 bytes of information.
		// We should protect remaining bytes when bytes_to_copy is less than 4.
		write_val = Final_Write_Value( old_val, write_val, bytes_to_copy );

		status = Write_Backup_Register( bkp_reg_num, write_val );

		if ( true == status )
		{
			if ( length > BKP_REG_SIZE )
			{
				bytes_to_copy = BKP_REG_SIZE;
				length -= BKP_REG_SIZE;
				bkp_reg_num++;
				data += BKP_REG_SIZE;
			}
			else
			{
				bytes_to_copy = length;
				loop_break = true;
			}
		}
		else
		{
			loop_break = true;
		}
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t RTC_NV::Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data, bool protected_data )
{
	return ( OPERATION_NOT_SUPPORTED );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t RTC_NV::Erase_All( void )
{
	return ( OPERATION_NOT_SUPPORTED );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool RTC_NV::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool range_good = false;
	NV_CTRL_ADDRESS_TD end_address;

	end_address = m_nv_cfg->end_address;

	if ( ( address >= m_nv_cfg->start_address ) && ( ( address + length ) <= ( end_address + 1U ) ) )
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
uint32_t RTC_NV::Read_Backup_Register( uint8_t BackupRegister ) const
{
	uint32_t return_value = 0U;

	return_value = m_RTC_backup_handle->Backup_Register_Read( BackupRegister );

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool RTC_NV::Write_Backup_Register( uint8_t BackupRegister,
									uint32_t Data )
{
	bool return_value = false;

	return_value = m_RTC_backup_handle->Backup_Register_Write( BackupRegister, Data );

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t RTC_NV::Final_Write_Value( uint32_t old_bkp_val, uint32_t new_bkp_val, uint8_t bytes_to_copy )
{
	uint32_t ret_val = new_bkp_val;
	uint32_t word_mask = 0;

	if ( bytes_to_copy < uC_RTC_Backup::RTC_BKP_REG_BYTE_SIZE )
	{
		switch ( bytes_to_copy )
		{
			case 1U:
				word_mask = 0xFF;
				break;

			case 2U:
				word_mask = 0xFFFF;
				break;

			case 3U:
				word_mask = 0xFFFFFF;
				break;

			default:
				break;

		}
		ret_val = ( old_bkp_val & ~word_mask ) | ( new_bkp_val & word_mask );
	}

	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
RTC_NV* RTC_NV::Get_RTC_NV_instance()
{
	return ( m_rtc_nv_instance );
}

}	// namespace NV_Ctrls end
