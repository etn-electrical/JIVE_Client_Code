/*
 *****************************************************************************************
 *		$Workfile: IS25.h$
 *
 *		$Author: Zoe$
 *		Copyright ?Eaton Corporation. All Rights Reserved.
 *
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************************
 */
#ifndef IS25_H
#define IS25_H

#include "Includes.h"
#include "uC_QSPI.h"
#include "NV_Ctrl.h"
#include "Timers_Lib.h"
#include "CR_Queue.h"
#include "CR_Tasker.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define IS25_DEFAULT_ERASE_VAL      0xFF

typedef OS_TICK_TD TIMERS_TIME_TD;

typedef enum
{
	IS25_EPAGE,
	IS25_WPAGE
} IS25_PAGE_TYPE_EN;

typedef enum
{
	IS25_PREP_TO_WRITE,
	IS25_WRITTING_PAGE,
	IS25_WRITTING_PROTECTION,
	IS25_PREP_SAVE_DEST_PAGE,
	IS25_SAVING_DEST_PAGE,
	IS25_ERASING_PAGE,
	IS25_RECOVERYING_DEST_PAGE,
	IS25_CHECKING_DATA,
	IS25_FINISH
} IS25_WRITE_STATUS_EN;

typedef enum
{
	IS25_OPERATE_CMD,
	IS25_WAITTING
} IS25_LONG_OPERATE_STATUS_EN;

typedef enum
{
	SAME_VAL,
	ERASE_VAL,
	NO_MEANING_VAL
} DATA_ANALYSE_RESULT_EN;

typedef enum
{
	IS25_FALSE,
	IS25_PENDING,
	IS25_TRUE
} IS25_RESULT_EN;

typedef struct
{
	IS25_RESULT_EN result;
	IS25_WRITE_STATUS_EN status;
} RESULT_STATE_ST;


/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */

typedef struct IS25_CHIP_CONFIG_TD
{
	uint8_t address_length;
	uint32_t erase_page_size;
	uint32_t write_page_size;
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;
	uint32_t max_clock_freq;

	uint32_t byte_write_time;		// (in microseconds)
	uint32_t page_write_time;
	uint32_t page_erase_time;
} IS25_CHIP_CONFIG_TD;


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class IS25 : public NV_Ctrl
{
	public:
		IS25( uC_QSPI* spi_ctrl, uint8_t chip_select, IS25_CHIP_CONFIG_TD const* chip_config,
			  uint16_t buffer_size = 256 );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~IS25( void ){}

		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool_t use_protection = false );

		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool_t use_protection = false );

		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
									uint8_t erase_data = IS25_DEFAULT_ERASE_VAL, bool_t protected_data = false );

		NV_Ctrl::nv_status_t Erase_All( void );

		NV_Ctrl::nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual void Continue( void )
		{}

		/**
		 * @brief Calculates time required for write operation.
		 * @param[in] data: Used to analyze number of bytes which has value FLASH_ERASED_VALUE. Does not require
		 * write operation for such bytes.
		 * @param[in] address: Address to write data to flash
		 * @param[in] length: length of data to write to flash
		 * @param[in] include_erase_time:  Flag to include erase time
		 * function.
		 * @return Operation time in milli second.
		 */
		uint32_t Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							 NV_CTRL_LENGTH_TD length, bool include_erase_time = true ) const;

		/**
		 * @brief Calculates time required for read operation. Not supported for this controller and need to define it
		 * properly. BF_ASSERT will be trigered by calling this API.
		 * @param[in] data: Not used inside read operation.
		 * @param[in] address: Address to read data from flash
		 * @param[in] length: length of data to read from flash
		 * function.
		 * @return Operation time in milli second.
		 */
		uint32_t Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address,
							NV_CTRL_LENGTH_TD length ) const
		{
			BF_ASSERT( false );
			return ( 0 );
		}

		/**
		 * @brief Calculates time required for erase operation
		 * @param[in] address: Start address
		 * @param[in] length: length of data
		 * @return Operation time in milli second.
		 */
		uint32_t Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
		{
			return ( Write_Time( NULL, address, length ) );
		}

		bool_t Write_Status( uint8_t status );

		bool_t Read_Status( uint8_t* status );

		bool_t Change_Mode( uint8_t mode );

		void Set_Fault( void );

		void Clr_Fault( void );

		static bool_t Is_Ready( void );

		static void Use_OS( bool_t value ){m_use_os = value;}

		NV_Ctrl::nv_status_t Erase_Operation( uint8_t erase_cmd, uint32_t address );

		NV_Ctrl::nv_status_t Erase_External_Serial_Flash( uint32_t start_address, uint32_t length );

	private:
		typedef struct BUFF_STRUCT_TD
		{
			BUFF_STRUCT_TD* next;
			uint8_t* data;
			NV_CTRL_LENGTH_TD length;
		} BUFF_STRUCT_TD;

		void IS25_CR_Callback( CR_Tasker* cr_tasker );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void IS25_CR_Callback_Static( CR_Tasker* cr_tasker, CR_TASKER_PARAM param )
		{
			( ( IS25* )param )->IS25_CR_Callback( cr_tasker );
		}

		/*
		 * The differences from Write method is don't need to check range and get semaphore again.
		 */
		bool_t Internal_Write( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length, bool_t use_checksum,
							   bool_t use_protection );

		/**
		 * Maintain a state machine to do the actual write work.
		 */
		RESULT_STATE_ST Write_Now( void );

		DATA_ANALYSE_RESULT_EN Analyze_Data( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		uint32_t Get_Recovery_Address( uint32_t dest_address, bool_t use_protection );

		NV_CTRL_LENGTH_TD Get_Deal_Length( uint32_t address, NV_CTRL_LENGTH_TD total_length,
										   NV_CTRL_LENGTH_TD done_length, IS25_PAGE_TYPE_EN page_type );

		uint8_t* Get_Deal_Data( NV_CTRL_LENGTH_TD done_length );

		void Condition_Copy( uint8_t* dest_address, uint8_t* source_address, NV_CTRL_LENGTH_TD length );

		RESULT_STATE_ST Save_Page( uint32_t dest_page, uint32_t source_page, bool_t use_ram_val );

		static bool_t Compare_Data( uint8_t* source_data, uint8_t* dest_data, NV_CTRL_LENGTH_TD length );

		static bool_t Compare_Data( uint8_t* source_data, uint8_t value, NV_CTRL_LENGTH_TD length );

		IS25_RESULT_EN Check_Chip_Status( TIMERS_TIME_TD duration );

		IS25_RESULT_EN Writting( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		bool_t Reading( uint8_t* data, NV_CTRL_ADDRESS_TD address,
						NV_CTRL_LENGTH_TD length, bool_t use_checksum );

		bool_t Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						   NV_CTRL_LENGTH_TD length, bool_t use_checksum );

		void Send_Data_Command( uint8_t command, NV_CTRL_ADDRESS_TD address );

		bool_t Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, bool_t use_checksum = false,
							bool_t protection = false );

		void Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		IS25_RESULT_EN Erase_Page( uint32_t address );

		void Deal_IS25_Result( IS25_RESULT_EN result );

		uC_QSPI* m_spi_ctrl;
		uint8_t m_chip_select;
		IS25_CHIP_CONFIG_TD const* m_chip_cfg;
		uint8_t* m_wpage_buff;
		uint8_t* m_shadow_wpage_buff;

		TIMERS_TIME_TD m_start_time;

		uint8_t* m_data_buffer;
		uint16_t m_buffer_size;

		IS25_WRITE_STATUS_EN m_write_status;
		NV_CTRL_LENGTH_TD m_done_length;
		uint8_t* m_deal_data;
		uint32_t m_deal_address;
		NV_CTRL_LENGTH_TD m_deal_length;

		IS25_WRITE_STATUS_EN m_sub_status;
		NV_CTRL_LENGTH_TD m_sub_done_length;

		IS25_LONG_OPERATE_STATUS_EN m_long_operate_status;

		CR_Queue* m_cr_queue;
		uint8_t m_retry_times;

		uint8_t* m_data_ptr;
		uint32_t m_dest_address;
		uint32_t m_data_length;
		uint32_t m_total_length;
		bool_t m_use_checksum;
		uint16_t m_check_sum;
		bool_t m_use_protection;
		uint32_t m_recovery_address;
		static bool_t m_use_os;
		uint8_t m_erase_val;
		static bool_t isBusy;

		bool_t Get_Semaphore( void );

		void Release_Semaphore( void );

};

}
#endif
