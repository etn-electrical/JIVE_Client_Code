/**
 *****************************************************************************************
 *	@file
 *
 *	@brief this file contains the NV OTP class which can be used for both NV OTP memory
 *         and the same behavior can be simulated for RAM.
 *	@details The OTP memory provides a software control to lock the OTP block.
 *			Lock API is by default enable which will lock the block once a write is
 *			performed on OTP block.Adoptor can comment the Lock API in write call if
 *			they dont want a block to be locked.
 *			On enabling OTP_RAM_SIMULATION the OTP class can be simulated to use on RAM
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_OTP_H
#define UC_OTP_H
// #define OTP_RAM_SIMULATION
#include "NV_Ctrl.h"
#include "uC_Watchdog.h"
#include "CR_Tasker.h"
namespace NV_Ctrls
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t OTP_PGERR = ( FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR
									| FLASH_SR_WRPERR );

enum uC_OTP_STATUS_EN
{
	uC_OTP_BUSY,
	uC_OTP_ERROR_PG,
	uC_OTP_ERROR_WRP,
	uC_OTP_COMPLETE,
	uC_OTP_TIMEOUT
};

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_OTP : public NV_Ctrl
{
	public:

		struct uc_otp_config_t
		{
			uint32_t otp_start_address;
			uint16_t size;
		};

#ifdef OTP_RAM_SIMULATION
		/* this will serve as block to lock RAM OTP area common to all instances of RAM OTP */
		static uint8_t* m_otp_lock_block;
#endif
		/**
		 * @brief constructor to initialized uC_OTP class object .
		 * @param[in] otp_config: A pointer to the structure used to configure OTP .
		 * @param[in] flash_ctrl: A pointer to the kind of flash handler.
		 */
		uC_OTP( uc_otp_config_t const* otp_config, FLASH_TypeDef* flash_ctrl );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_OTP( void )
		{}

		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read
		 * operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] cback: A pointer to the function to be called when the operation is
		 * complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Read( uint8_t* data, uint32_t address, uint32_t length,
								  cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data: A pointer to the buffer used to hold the data for the write
		 * operation.
		 * @param[in] address: The address of the first byte to be written.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] cback: A pointer to the function to be called when the operation is
		 * complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Write( const uint8_t* data, uint32_t address, uint32_t length,
								   cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Erases a block of memory in the NV_Ctrl device. This may involve a range
		 * larger than specified in the address and length arguments as some devices have
		 * minimum block sizes that can be erased.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] cback: A pointer to the function to be called when the operation is
		 * complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase( uint32_t address, uint32_t length,
								   cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Erases the entire NV_Ctrl device.
		 * @param[in] cback: A pointer to the function to be called when the operation is
		 * complete.
		 * @param[in] param: A void pointer to an object passed as an argument to the cback
		 * function.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( cback_func_t cback, cback_param_t param )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual void Continue( void )
		{}

		/**
		 * @brief Read the content from the OTP memory area.
		 * @param[in] data: pointer to hold the buffer to store the read content
		 * @param[in] address: address of the data to be read
		 * @param[in] length: length of the data read
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
								   bool use_protection = false );

		/**
		 * @brief write the content to OTP memory area.
		 * @param[in] data: pointer to the data to be copied to the OTP memory area
		 * @param[in] address: address of the data to be written
		 * @param[in] length: length of the data to be written
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									bool use_protection = false );

		static const uint8_t OTP_UNLOCK_VALUE = 0xFF;
		static const uint8_t OTP_LOCK_VALUE = 0x00;
		static const uint8_t OTP_MIN_BLK_SIZE = 32U;

	private:
		bool_t Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length );

		bool_t Read_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief whether the address and data length satisfy the OTP area limit
		 * @param[in] param: address of OTP area where data needs to be written
		 * @param[in] param: length of the data to be written to OTP area
		 * @return status whether the data is within range of OTP area.
		 */
		bool_t Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		/**
		 * @brief checks whether source and destination data are same after write is done
		 * @param[in] check_data: pointer to source
		 * @param[in] address: pointer to destination
		 * @param[in] length: length of data to check
		 * @return status of requested operation.
		 */
		bool_t Check_Data( uint8_t* check_data, NV_CTRL_ADDRESS_TD address,
						   NV_CTRL_LENGTH_TD length ) const;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Write_u8( uint32_t adr,
							  uint8_t dat )
		{
			( *( reinterpret_cast<volatile uint8_t*>( adr ) ) ) =
				dat;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Write_u16( uint32_t adr,
							   uint16_t dat )
		{
			( *( reinterpret_cast<volatile uint16_t*>( adr ) ) ) = dat;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Write_u32( uint32_t adr,
							   uint32_t dat ) { ( *( reinterpret_cast<volatile uint32_t*>( adr ) ) ) = dat; }

		inline void Write_u64( uint32_t adr,
							   uint64_t dat ) { ( *( reinterpret_cast<volatile uint64_t*>( adr ) ) ) = dat;
		}

		/**
		 * @brief locks entire block to prevent future programming.when you lock your block you
		 * are not able to program it anymore, you can't unlock it back
		 * @param[in] uint8_t block_number:
		 * @return status of requested operation.
		 */
		nv_status_t Lock_Block( uint8_t block_number );

		uC_OTP_STATUS_EN Wait_For_Complete( void ) const;

		/**
		 * @brief Get the block index using data block address and size so that lock block
		 * index can be known
		 * @return the block number.
		 */
		uint8_t Get_Num_Blocks( void );

		/**
		 * @brief locks the particular block,once it is locked it cannot be unlocked use
		 * cautiously
		 * @param[in] param: void
		 * @return status whether the block lock is successful or not
		 */
		nv_status_t Lock( void );

		/**
		 * @brief checks whether the given block number is block or not
		 * @param[in] void:
		 * @return status whether the block is lock or not
		 */
		bool_t Is_Locked( void );

		/**
		 * @brief returns the address of the lock block index
		 * @param[in] void:
		 * @return address of the lock block index
		 */
		uint32_t Get_Lock_Start_Index_Address( void );

		uC_OTP_STATUS_EN Get_Status( void ) const;

		void Unlock_Flash( void );

		void Lock_Flash( void );

		uc_otp_config_t const* m_otp_cfg;

		FLASH_TypeDef* m_flash_ctrl;
};
}
#endif
