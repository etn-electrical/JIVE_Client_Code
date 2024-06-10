/**
 *****************************************************************************************
 *	@file NV_Crypto.h
 *
 *	@brief  NV_Crypto is a interface through which we can access encrypted data stored in
 *  any non-volatile random-access memory.
 *	same functionality as flash memory.
 *
 *	@details
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef NV_CRYPTO_H
#define NV_CRYPTO_H

#include "NV_Ctrl.h"
#include "CR_Tasker.h"
#include "Crypt.h"
#include "System_Reset.h"

namespace NV_Ctrls
{

/**
 ****************************************************************************************
 * @brief The NV_Crypto class derived from the NV_Ctrl class contains implementations required for the access of the
 *NV_Crypto memory.
 *
 * @details It provides functionalities to read, write, erase the NV_Crypto memory.
 *
 ****************************************************************************************
 */
class NV_Crypto : public NV_Ctrl
{
	public:
		/**
		 * @brief NV_Crypto engine states
		 */
		enum nv_crypto_state_t
		{
			STATE_IDLE,				///< Nv_Crypto idle
			STATE_START_ENCRYPT,	///< Nv_Crypto start encrypt
			STATE_ENCRYPT,			///< Nv_Crypto encrypt
			STATE_WRITE				///< Nv_Crypto write
		};

		/**
		 * @brief NV_Crypto configuration parameters
		 */
		struct nv_crypto_config_t
		{
			uint32_t start_address;		///< Start address of CRYPTO_NV_BLOCK.
			uint32_t block_size;		///< Size of CRYPTO_NV_BLOCK.
			uint32_t meta_data_size;	///< Extra bytes added to validate decrypted data
			uint32_t offset;			///< Offset required to find address of CRYPTO_NV_BLOCK
										///< from actual nv_ctrl.
		};

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members
		 * @param[in] nv_ctrl: Critical data stored in this nv_ctrl.
		 * @param[in] crypto_obj: This object will help in encryption and decryption.
		 * @param[in] crypto_config: The chip configuration for NV_Crypto.
		 * @param[in] cback_func_t: Callback to handle error event
		 * @return None
		 */
		NV_Crypto( NV_Ctrl* nv_ctrl, Crypt* crypto_obj, nv_crypto_config_t const* crypto_config,
				   cback_func_t cb_func = nullptr );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of NV_Crypto goes out of scope or deleted.
		 *  @return None
		 */
		~NV_Crypto( void );


		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @details This does the read operation over the SPI. The data received is written into the address pointed by
		 *the
		 * variable data. In case of the protection being enabled, if there is a checksum match, then the data is
		 *considered good.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: NV_Crypto does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
										   NV_CTRL_LENGTH_TD length, bool use_protection = false );

		/**
		 * @brief Writes a specified number of bytes from a given address into a buffer.
		 * @details This initially checks the range of the address and the length to be basically between the start and
		 *end addresses;
		 * the end address being appropriately selected based upon the protection bit enabled or disabled.
		 * @param[in] data: A pointer to the buffer used to write the data.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] use_protection: NV_Crypto does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length, bool use_protection = false );

		/**
		 * @brief Performs the erase operation.
		 * @details A appropriate erase value is written to erase the length of the AT25DF memory.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased. The address of the first byte to be read.
		 * @param[in] erase_data: The erase value.
		 * @param[in] protected_data: NV_Crypto does not support checksum protection.
		 * @return Status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
											NV_CTRL_LENGTH_TD length, uint8_t erase_data = 0,
											bool protected_data = false );

		/**
		 * @brief Performs the erase operation over the complete address range.
		 * @details A appropriate erase value is written to erase the length of the AT25DF memory.
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
		 * @brief					Type of function pointer to coroutine call back function.
		 * @param[in] cr_task		Pointer to coroutine task
		 * @param[in] param			Coroutine task parameter
		 */
		static void NV_Crypto_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
		{
			reinterpret_cast<NV_Crypto*>( param )->NV_Crypto_Task( cr_task );
		}

		/**
		 * @brief NV_Crypto_Task_Static will call this function.
		 * @param[in] cr_task		Pointer to coroutine task
		 * @return 					void
		 */
		void NV_Crypto_Task( CR_Tasker* cr_task );

		/**
		 * @brief Static callback function for System_Reset object.
		 * @param[in] handle: The callback parameter (our 'this' pointer).
		 * @param[in] reset_req Reset request type (not used).
		 */
		static bool Reset_CallBack_Static( BF::System_Reset::cback_param_t handle,
										   BF::System_Reset::reset_select_t reset_req )
		{
			return ( reinterpret_cast<NV_Crypto*>( handle )->Reset_CallBack( reset_req ) );
		}

		/* Reset_CallBack_Static will call this member function */
		bool Reset_CallBack( BF::System_Reset::reset_select_t reset_req );

		NV_Ctrl::nv_status_t Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		NV_Ctrl::nv_status_t Write_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		NV_Ctrl::nv_status_t Erase_Now( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, uint8_t erase_data );

		bool Write_String( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		bool Read_String( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length );

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const;

		/* Actual encrypted data stored in this NV_Ctrl */
		NV_Ctrl* m_real_nv_ctrl;
		nv_crypto_config_t const* m_crypto_config;
		NV_CTRL_LENGTH_TD m_real_data_size;
		NV_CTRL_LENGTH_TD m_meta_data_size;
		nv_crypto_state_t m_state;
		uint8_t* m_encrypt_text_buff;
		uint8_t* m_plain_text_buff;
		Crypt* m_crypto_obj;
		cback_func_t m_cb_func;
		CR_Tasker* m_cr_task;

		static const uint8_t MAX_NUMBER_OF_RAM_NV_WRITES = 2U;

};

}
#endif
