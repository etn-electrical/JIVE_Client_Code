/**
 *****************************************************************************************
 *	@file RTC_NV.h
 *
 *	@brief  RTC_NV is a interface through which we can access data stored in Backup Registers its used only on STM32WB55
 * configuration
 *
 *	@details
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef RTC_NV_H
#define RTC_NV_H

#include "NV_Ctrl.h"
#include "uC_RTC_Backup.h"
#include "NV_Address.h"

namespace NV_Ctrls
{

struct RTC_NV_CONFIG_TD
{
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;
};

/**
 ****************************************************************************************
 * @brief The RTC_NV class derived from the NV_Ctrl class contains implementations required for the access of the Backup
 * Registers memory.
 *
 * @details It provides functionalities to read and write the backup registers.
 *
 ****************************************************************************************
 */
class RTC_NV : public NV_Ctrl
{
	public:
		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members
		 * @param[in] rtc_bkp_handle	Handle of uC_RTC_Backup
		 * @param[in] rtc_nv_config		Defines address range of RTC_NV object
		 * @return None
		 */
		RTC_NV( uC_RTC_Backup* rtc_bkp_handle, RTC_NV_CONFIG_TD const* rtc_nv_config );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Flash_NV goes out of scope or deleted.
		 *  @return None
		 */
		~RTC_NV( void );


		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @details In case of the protection being enabled, if there is a checksum match, then the data is considered
		 * good.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: virtual address of rtc backup register. One backup register covers 4 byte address.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: Flash_NV does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								  NV_CTRL_LENGTH_TD length, bool use_protection = false );

		/**
		 * @brief Writes a specified number of bytes from a given address into a buffer.
		 * @details This initially checks the range of the address and the length to be basically between the start and
		 * end addresses;
		 * the end address being appropriately selected based upon the protection bit enabled or disabled.
		 * @param[in] data: A pointer to the buffer used to write the data.
		 * @param[in] address: virtual address of rtc backup register. One backup register covers 4 byte address.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] use_protection: Flash_NV does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual nv_status_t Write( uint8_t* data, uint32_t address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = false );

		/**	TODO	Erase selected backup registers
		 * @brief Performs the erase operation.
		 * @details A appropriate erase value is written to erase the length of the flash memory.
		 * @param[in] BackupRegister: The address of the register to be erased.
		 * @param[in]
		 * @param[in] erase_data: The erase value.
		 * @param[in]
		 * @return Status of requested operation.
		 */
		virtual nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, uint8_t erase_data = 0, bool protected_data = false );

		/**	TODO	Erase all the backup registers
		 * @brief Performs the erase operation over the complete address range.
		 * @details A appropriate erase value is written to erase the length of the AT25DF memory.
		 * @return Status of requested operation.
		 */
		virtual nv_status_t Erase_All( void );

		// Not implemented the pure virtual function from base class
		virtual void Continue( void )
		{}

		/**
		 * @brief  	Returns the the pointer to RTC_NV, Basically returning 'this' pointer
		 *
		 * @retval 	pointer
		 */
		static RTC_NV* Get_RTC_NV_instance();

		/**
		 * Gets updated after RTC NV initialization is complete
		 */
		static RTC_NV* m_rtc_nv_instance;

	private:

		/**
		 * @brief                       This function can be used to read the backup RTC register
		 * @param[in] BackupRegister    Backup register id.
		 * @return backup register value.
		 */
		uint32_t Read_Backup_Register( uint8_t BackupRegister ) const;

		/**
		 * @brief                       This function can be used to write data into the backup RTC register
		 * @param[in] BackupRegister    Backup register id.
		 * @param[in] Data    			Backup register Value.
		 * @return status of the operation.
		 */
		bool Write_Backup_Register( uint8_t BackupRegister, uint32_t Data );

		/**
		 * @brief Supportive function for range check
		 * @param[in] address: virtual address of rtc backup register. One backup register covers 4 byte address.
		 * @param[in] length: The number of bytes to be written.
		 * @return Status of requested operation.
		 */
		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief Supportive function for write operation
		 * @param[in] data: A pointer to the buffer used to write the data.
		 * @param[in] address: virtual address of rtc backup register. One backup register covers 4 byte address.
		 * @param[in] length: The number of bytes to be written.
		 * @return Status of requested operation.
		 */
		bool Write_Now( uint8_t* data, uint32_t dest_address,
						NV_CTRL_LENGTH_TD length );

		/**
		 * @brief Supportive function for read operation
		 * @param[in] data: A pointer to the buffer used to write the data.
		 * @param[in] address: virtual address of rtc backup register. One backup register covers 4 byte address.
		 * @param[in] length: The number of bytes to be written.
		 * @return Status of requested operation.
		 */
		bool Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address,
					   NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief This function will return a new 4 byte backup register value based on number of bytes which needs to
		 * be copied.
		 * @param[in] old_bkp_val		Current value of backup register
		 * @param[in] new_bkp_val		New value which needs to be written
		 * @param[in] bytes_to_copy		Number of bytes which needs to be copied from new value
		 * @return backup register value.
		 */
		uint32_t Final_Write_Value( uint32_t old_bkp_val, uint32_t new_bkp_val, uint8_t bytes_to_copy );


		/**
		 * @brief Stores uC_RTC_Backup handle which is passed from outside
		 */
		uC_RTC_Backup* m_RTC_backup_handle;

		/**
		 * @brief Defines RTC NV address range
		 */
		RTC_NV_CONFIG_TD const* m_nv_cfg;
};

}
#endif
