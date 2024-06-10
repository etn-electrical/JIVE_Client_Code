/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_RTC_Backup.h"
#include "stm32h7xx_hal_rtc.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_RTC_Backup::uC_RTC_Backup( uC_RTC* uC_RTC_handle )
{
	m_uC_RTC_handle = uC_RTC_handle;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_RTC_Backup::Backup_Register_Write( uint8_t BackupRegister, uint32_t Data )
{
	bool return_value = false;

	// Check if RTC is initialised
	if ( m_uC_RTC_handle != nullptr )
	{
		// perform the write
		m_uC_RTC_handle->Backup_Register_Write( BackupRegister, Data );
	}

	// Verify if the data written correctly
	if ( m_uC_RTC_handle->Backup_Register_Read( BackupRegister ) == Data )
	{
		return_value = true;
	}
	else
	{
		return_value = false;
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_RTC_Backup::Backup_Register_Read( uint8_t BackupRegister )
{
	uint32_t tmp = 0U;

	// Check if RTC is initialised
	if ( m_uC_RTC_handle != nullptr )
	{
		// read the value
		tmp = m_uC_RTC_handle->Backup_Register_Read( BackupRegister );
	}
	else
	{
		// TODO
		// tmp = 0xFFFF; //some way to show error
	}

	return ( tmp );
}
