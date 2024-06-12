/**
 *****************************************************************************************
 *	@file Flash_NV.h
 *
 *	@brief  Flash_NV is a interface through which we can access data stored in
 *      Flash memory. Dynamically allocated RAM buffer holds the continous value and
 *      Flash NV region will be updated with latest values present in buffer every wait state interval.
 *      Values gets read from flash NV to RAM buffer every code startup.
 *
 *	@details
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FLASH_NV_H
#define FLASH_NV_H

#include "NV_Ctrl.h"
#include "CR_Tasker.h"
#include "System_Reset.h"
#include "uC_Flash.h"
#include "NV_Address.h"

namespace NV_Ctrls
{

/**
 ****************************************************************************************
 * @brief The Flash_NV class derived from the NV_Ctrl class contains implementations required for the access of the
 * Flash_NV memory.
 *
 * @details It provides functionalities to read, write, erase the Flash_NV memory.
 *
 ****************************************************************************************
 */
class Flash_NV : public NV_Ctrl
{
	public:
		/**
		 * @brief Flash_NV engine states
		 */
		enum nv_flash_state_t
		{
			STATE_IDLE,				///< Flash_NV idle
			STATE_WAIT,				///< Flash_NV wait
			STATE_WRITE				///< Flash_NV write
		};

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members
		 * @param[in] nv_ctrl: Critical data stored in this nv_ctrl.
		 * @param[in] flash_config: The chip configuration for Flash_NV.
		 * @param[in] waitstate_timeout_ms: wait state of write function default: Go to WRITE state after 10mSec
		 * inactivity
		 * @param[in] cback_func_t: Callback to handle error event
		 * @return None
		 */
		Flash_NV( NV_Ctrl* nv_ctrl, NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const* flash_config,
				  uint32_t nv_data_memory_size, uint32_t waitstate_timeout_ms = 10U, bool_t checksum_enabled = false,
				  cback_func_t cb_func = nullptr );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Flash_NV goes out of scope or deleted.
		 *  @return None
		 */
		~Flash_NV( void );


		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @details In case of the protection being enabled, if there is a checksum match, then the data is
		 * considered good.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: Flash_NV does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
										   NV_CTRL_LENGTH_TD length, bool use_protection = true );

		/**
		 * @brief Writes a specified number of bytes from a given address into a buffer.
		 * @details This initially checks the range of the address and the length to be basically between the start and
		 * end addresses;
		 * the end address being appropriately selected based upon the protection bit enabled or disabled.
		 * @param[in] data: A pointer to the buffer used to write the data.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] use_protection: Flash_NV does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length, bool use_protection = true );

		/**
		 * @brief Performs the erase operation.
		 * @details A appropriate erase value is written to erase the length of the flash memory.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased. The address of the first byte to be read.
		 * @param[in] erase_data: The erase value.
		 * @param[in] protected_data: Flash_NV does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length, uint8_t erase_data = uC_FLASH_DEFAULT_ERASE_VAL,
											bool protected_data = true );

		/**
		 * @brief Performs the erase operation over the complete address range.
		 * @details A appropriate erase value is written to erase the length of the memory.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Erase_All( void );

		/**
		 * @brief Performs the checksum read operation.
		 * @param[in] checksum_dest: A pointer to the address used to write the checksum.
		 * @param[in] address: It contains the checksum.
		 * @param[in] length: The length to be read.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest,
													NV_CTRL_ADDRESS_TD address,
													NV_CTRL_LENGTH_TD length );

		// TODO
		virtual void Continue( void )
		{}

	private:

		/**
		 * @brief			Type of function pointer to coroutine call back function.
		 * @param[in] cr_task		Pointer to coroutine task
		 * @param[in] param		Coroutine task parameter
		 */
		static void NV_Flash_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
		{
			reinterpret_cast<Flash_NV*>( param )->Flash_NV_Task( cr_task );
		}

		/**
		 * @brief NV_Flash_Task_Static will call this function.
		 * @param[in] cr_task		Pointer to coroutine task
		 * @return 					void
		 */
		void Flash_NV_Task( CR_Tasker* cr_task );

		/**
		 * @brief Static callback function for System_Reset object.
		 * @param[in] handle: The callback parameter (our 'this' pointer).
		 * @param[in] reset_req Reset request type (not used).
		 */
		static bool Reset_CallBack_Static( BF::System_Reset::cback_param_t handle,
										   BF::System_Reset::reset_select_t reset_req )
		{
			return ( reinterpret_cast<Flash_NV*>( handle )->Reset_CallBack( reset_req ) );
		}

		/* Reset_CallBack_Static will call this member function */
		bool Reset_CallBack( BF::System_Reset::reset_select_t reset_req );

		NV_Ctrl::nv_status_t Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		NV_Ctrl::nv_status_t Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		bool Write_String( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		bool Read_String( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						  bool protection = true ) const;

		/* Actual encrypted data stored in this NV_Ctrl */
		NV_Ctrl* m_real_nv_ctrl;
		uC_FLASH_CHIP_CONFIG_TD const* m_flash_config;
		nv_flash_state_t m_state;
		uint8_t* m_plain_text_buff;
		cback_func_t m_cb_func;
		uint16_t m_wait_state_timeout_counter;
		uint32_t m_wait_state_timeout;
		NV_CTRL_LENGTH_TD m_real_data_size;
		NV_CTRL_LENGTH_TD m_nv_block_size;
		CR_Tasker* m_cr_task;
		bool_t m_checksum_enabled;

		static const uint8_t MAX_NUMBER_OF_RAM_NV_WRITES = 2U;
		static const uint16_t CR_TASK_DELAY_MS = 2U;// Routine periodicity set to 2 msec
		static const uint16_t WAIT_TIME_MS = 10U;// WAIT state time set to 10 msec
		static const uint16_t MEM_CHECK_CRC_INIT = 0U;

		#ifndef DCI_FLASH_NV_MIRROR_SUPPORT_DISABLE
			#define FLASH_NV_MIRROR_SUPPORT true
		#else
			#define FLASH_NV_MIRROR_SUPPORT false
		#endif
		/* Enable or disable the MIRROR support for FLASH NV */
		static const bool MIRROR_SUPPORT = FLASH_NV_MIRROR_SUPPORT;

};

}
#endif
