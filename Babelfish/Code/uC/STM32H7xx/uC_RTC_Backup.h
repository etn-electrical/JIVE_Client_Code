/**
 **********************************************************************************************
 * @file            uC_RTC_Backup.h Header File for RTC Backup Register Driver implementation.
 *
 * @brief           The file contains uC_RTC_Backup Class required for accessing RTC Backup registers
 * @details
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed here on.  This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef UC_RTC_BACKUP_H
#define UC_RTC_BACKUP_H

#include "Includes.h"
#include "uC_RTC.h"

/**
 ****************************************************************************************
 * @brief Class to access RTC Backup registers.
 *
 * @details RTC Backup class is used for Reading from and Writing to RTC Backup registers. User must not create multiple
 * instances of uC_RTC_Backup.
 ****************************************************************************************
 */
class uC_RTC_Backup
{
	public:

		/**
		 * @brief Represents RTC peripheral index number
		 */
		enum RTC_backup_register_t
		{
			RTC_BKP_REG_0,
			RTC_BKP_REG_1,
			RTC_BKP_REG_2,
			RTC_BKP_REG_3,
			RTC_BKP_REG_4,
			RTC_BKP_REG_5,
			RTC_BKP_REG_6,
			RTC_BKP_REG_7,
			RTC_BKP_REG_8,
			RTC_BKP_REG_9,
			RTC_BKP_REG_10,
			RTC_BKP_REG_11,
			RTC_BKP_REG_12,
			RTC_BKP_REG_13,
			RTC_BKP_REG_14,
			RTC_BKP_REG_15,
			RTC_BKP_REG_16,
			RTC_BKP_REG_17,
			RTC_BKP_REG_18,
			RTC_BKP_REG_19,
			RTC_BKP_REG_20,
			RTC_BKP_REG_21,
			RTC_BKP_REG_22,
			RTC_BKP_REG_23,
			RTC_BKP_REG_24,
			RTC_BKP_REG_25,
			RTC_BKP_REG_26,
			RTC_BKP_REG_27,
			RTC_BKP_REG_28,
			RTC_BKP_REG_29,
			RTC_BKP_REG_30,
			RTC_BKP_REG_31,
			RTC_BKP_REG_MAX
		};

		/**
		 * @brief Byte addressing for backup registers. Size of each backup register is 4 bytes. RTC_BKP_REG_0 is used
		 * by system. So we are using backup registers from RTC_BKP_REG_1 to RTC_BKP_REG_31.
		 */

		static const uint8_t RTC_BKP_FC_START_ADDR = RTC_BKP_REG_1 * 4U;

		static const uint8_t RTC_BKP_START_ADDR = RTC_BKP_REG_1 * 4U;
		static const uint8_t RTC_BKP_END_ADDR = RTC_BKP_REG_MAX * 4U;
		static const uint8_t RTC_BKP_REG_BYTE_SIZE = 4U;

		/**
		 * @brief 	Constructor updating the uC_RTC handler to use it RTC register specific operations
		 * @param1 uC_RTC_handle	uC_RTC handle will be passed from outside
		 *
		 */
		uC_RTC_Backup( uC_RTC* uC_RTC_handle );

		/**
		 * @brief The destructor.
		 */
		~uC_RTC_Backup( void );

		/**
		 * @brief  Write a data in a specified RTC Backup data register.
		 * @param1 BackupRegister RTC Backup data Register number. This parameter can be from 1 to 31 to specify the
		 * register.
		 * @param2 Data Data to be written in the specified RTC Backup data register.
		 * @retval true if write operation is succesful else false
		 */
		bool Backup_Register_Write( uint8_t BackupRegister, uint32_t Data );

		/**
		 * @brief  Reads data from the specified RTC Backup data Register.
		 * @param BackupRegister RTC Backup data Register number. This parameter can be from 1 to 31 to specify the
		 * register.
		 * @retval Read value
		 */
		uint32_t Backup_Register_Read( uint8_t BackupRegister );

	private:

		uC_RTC* m_uC_RTC_handle;

};

#endif
