/**
 *****************************************************************************************
 *	@file AT25DFx.h
 *
 *	@brief AT25DF NV_Ctrl class capable of supporting queued SPI operations
 *
 *	@details It is designed for use in a wide variety of applications in which program
 *  code is shadowed from Flash memory into embedded or external RAM for execution.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef AT25DFx_H
#define AT25DFx_H

#include "NV_Ctrl.h"
#include "uC_Queued_SPI.h"
#include "Timers_Lib.h"

namespace NV_Ctrls
{

/**
 ****************************************************************************************
 * @brief AT25DFx NV_Ctrl class
 *
 * @details Realizes portions of the NV_Ctrl interface that support asynchronous
 * operations
 *
 ****************************************************************************************
 */
class AT25DFx : public NV_Ctrl
{
	public:

		/**
		 * @brief AT25DFx operation codes
		 */
		static const uint8_t FAST_READ_OPCODE = 0x1BU;
		static const uint8_t MEDIUM_READ_OPCODE = 0x0BU;
		static const uint8_t SLOW_READ_OPCODE = 0x03U;
		static const uint8_t BLOCK_ERASE_4K_OPCODE = 0x20U;
		static const uint8_t BLOCK_ERASE_32K_OPCODE = 0x52U;
		static const uint8_t BLOCK_ERASE_64K_OPCODE = 0xD8U;
		static const uint8_t CHIP_ERASE_OPCODE = 0x60U;
		static const uint8_t PAGE_PROGRAM_OPCODE = 0x02U;
		static const uint8_t WRITE_ENABLE_OPCODE = 0x06U;
		static const uint8_t WRITE_DISABLE_OPCODE = 0x04U;
		static const uint8_t READ_STATUS_OPCODE = 0x05U;
		static const uint8_t WRITE_STATUS_OPCODE = 0x01U;

		/**
		 * @brief AT25DFx status register bit codes
		 */

		// AT25DFx Status Read Resister Byte 1
		static const uint8_t STATUS_SPRL = ( 1U << 7U );
		static const uint8_t STATUS_RES = ( 1U << 6U );
		static const uint8_t STATUS_EPE = ( 1U << 5U );
		static const uint8_t STATUS_WPP = ( 1U << 4U );
		static const uint8_t STATUS_SWP1 = ( 1U << 3U );
		static const uint8_t STATUS_SWP0 = ( 1U << 2U );
		static const uint8_t STATUS_WEL = ( 1U << 1U );

		// AT25DFx Status Read Resister Byte 2
		static const uint8_t STATUS_RSTE = ( 1U << 4U );
		static const uint8_t STATUS_SLE = ( 1U << 3U );
		static const uint8_t STATUS_PS = ( 1U << 2U );
		static const uint8_t STATUS_ES = ( 1U << 1U );

		// AT25DFx Status Read Resister Bytes 1 & 2
		static const uint8_t STATUS_BSY = ( 1U << 0U );

		/**
		 * @brief AT25DFx class states
		 */
		static const uint8_t OP_IN_PROGRESS_FLAG = 0x80U;
		static const uint8_t WRITE_IN_PROGRESS_FLAG = 0x40U;
		static const uint8_t IDLE = 0U;
		static const uint8_t PAGE_PROGRAM = 1U;
		static const uint8_t PAGE_PROGRAM_WAIT = OP_IN_PROGRESS_FLAG | PAGE_PROGRAM;
		static const uint8_t ERASE_BLOCK = 2U;
		static const uint8_t ERASE_BLOCK_WAIT = OP_IN_PROGRESS_FLAG | ERASE_BLOCK;
		static const uint8_t ERASE_ALL = 3U;
		static const uint8_t ERASE_ALL_WAIT = OP_IN_PROGRESS_FLAG | ERASE_ALL;
		static const uint8_t WRITE_STATUS = 4U;
		static const uint8_t WRITE_BLOCK_ERASE = WRITE_IN_PROGRESS_FLAG | 0U;
		static const uint8_t WRITE_BLOCK_ERASE_WAIT = OP_IN_PROGRESS_FLAG | WRITE_BLOCK_ERASE;
		static const uint8_t WRITE_BLOCK_WRITE = WRITE_IN_PROGRESS_FLAG | 1U;
		static const uint8_t WRITE_BLOCK_WRITE_WAIT = OP_IN_PROGRESS_FLAG | WRITE_BLOCK_WRITE;

		/**
		 * @brief AT25DFx(AT25DF041A) chip specific delays
		 */
		static const uint16_t PAGE_PROGRAM_TYP_DELAY = 2U;
		static const uint16_t BLOCK_ERASE_4K_TYP_DELAY = 50U;
		static const uint16_t BLOCK_ERASE_32K_TYP_DELAY = 250U;
		static const uint16_t BLOCK_ERASE_64K_TYP_DELAY = 400U;
		static const uint16_t CHIP_ERASE_TYP_DELAY = 3000U;

		/**
		 * @brief AT25DFx(AT25DF041A) chip specific step delays after typical wait time is over and status is still busy
		 */
		static const uint16_t PAGE_PROGRAM_MAX_DELAY = 5U;
		static const uint16_t BLOCK_ERASE_4K_MAX_DELAY = 200U;
		static const uint16_t BLOCK_ERASE_32K_MAX_DELAY = 600U;
		static const uint16_t BLOCK_ERASE_64K_MAX_DELAY = 950U;
		static const uint16_t CHIP_ERASE_MAX_DELAY = 7000U;

		/**
		 * @brief AT25DFx(AT25DF041A) Number of delay steps(polls)
		 */
		static const uint16_t NUMBER_OF_DELAY_POLLS = 3U;

		/**
		 * @brief AT25DFx(AT25DF041A) delay step for each flash operation, used when typical wait time is over
		 */
		static const uint16_t PAGE_PROGRAM_DELAY_STEP = ( PAGE_PROGRAM_MAX_DELAY
														  - PAGE_PROGRAM_TYP_DELAY ) / NUMBER_OF_DELAY_POLLS;
		static const uint16_t BLOCK_ERASE_4K_DELAY_STEP = ( BLOCK_ERASE_4K_MAX_DELAY
															- BLOCK_ERASE_4K_TYP_DELAY ) / NUMBER_OF_DELAY_POLLS;
		static const uint16_t BLOCK_ERASE_32K_DELAY_STEP = ( BLOCK_ERASE_32K_MAX_DELAY
															 - BLOCK_ERASE_32K_TYP_DELAY ) / NUMBER_OF_DELAY_POLLS;
		static const uint16_t BLOCK_ERASE_64K_DELAY_STEP = ( BLOCK_ERASE_64K_MAX_DELAY
															 - BLOCK_ERASE_64K_TYP_DELAY ) / NUMBER_OF_DELAY_POLLS;
		static const uint16_t CHIP_ERASE_DELAY_STEP =
			( CHIP_ERASE_MAX_DELAY - CHIP_ERASE_TYP_DELAY ) / NUMBER_OF_DELAY_POLLS;

		/**
		 * @brief Structure used by the constructor to specify memory attributes for the
		 * base class and the read op code to be used by the instance of the class.
		 */
		struct AT25DFx_attribute_struct_t
		{
			mem_attribute_struct_t base;
			uint8_t read_opcode;
		};

		/**
		 * @brief Constructs a AT25DFx NV_Ctrl object.
		 * @param[in] attrib - A pointer to the attribute structure used to define attributes
		 * for this instance of the class.
		 * @param[in] spi_ctrl - A pointer to the uC_Queued_SPI object used by this class
		 * to communicate with the physical memory device.
		 * @param[in] select - The select id used to indicate to the uC_Queued_SPI object
		 * which chip select to use for operations requested by this AT25DFx instance.
		 */
		AT25DFx( const AT25DFx_attribute_struct_t* attrib, uC_Queued_SPI* spi_ctrl,
				 uint8_t select );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object of AT25DFx goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~AT25DFx( void )
		{}

		/**
		 * @brief Reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data - A pointer to the buffer used to rececive the data from the read operation.
		 * @param[in] address - The address of the first byte to be read.
		 * @param[in] length - The number of bytes to be read.
		 * @param[in][in] cback - A pointer to the function to be called when the operation is complete.
		 * @param[in] param - a void pointer to an object passed as an arguement to the cback function.
		 * @return status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Read( uint8_t* data, uint32_t address, uint32_t length,
										   cback_func_t cback, cback_param_t param );

		/**
		 * @brief Writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data - A pointer to the buffer used to hold the data for the write operation.
		 * @param[in] address - The address of the first byte to be written.
		 * @param[in] length - The number of bytes to be written.
		 * @param[in] cback - A pointer to the function to be called when the operation is complete.
		 * @param[in] param - a void pointer to an object passed as an argument to the cback function.
		 * @return status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Write( const uint8_t* data, uint32_t address, uint32_t length,
											cback_func_t cback, cback_param_t param );

		/**
		 * @brief Erases a block of memory in the AT25DFx device. This may involve a range
		 * larger than specified in the address and length arguments as only blocks of fixed
		 * sizes can be erased.
		 * @param[in] address - The address of the first byte to be erased.
		 * @param[in] length - The number of bytes to be erased.
		 * @param[in] cback - A pointer to the function to be called when the operation is complete.
		 * @param[in] param - a void pointer to an object passed as an arguement to the cback function.
		 * @return status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Erase( uint32_t address, uint32_t length,
											cback_func_t cback, cback_param_t param );

		/**
		 * @brief Erases the entire AT25DFx device.
		 * @param[in] cback - A pointer to the function to be called when the operation is complete.
		 * @param[in] param - a void pointer to an object passed as an arguement to the cback function.
		 * @return status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Erase_All( cback_func_t cback, cback_param_t param );

		/**
		 * @brief Provides the status of the operation/AT25DFx device. Calling this operation also
		 * runs the internal state machine required for operations involving the exchange of multiple
		 * SPI exchanges and/or require time for the device to perform.
		 * @param[in] cback - A pointer to the function to be called when the operation is complete.
		 * @param[in] param - a void pointer to an object passed as an arguement to the cback function.
		 * @return status of requested operation.
		 */
		virtual NV_Ctrl::nv_status_t Ready( cback_func_t cback, cback_param_t param );

		/**
		 * @brief Writes a value to the AT25DFx device's status register.
		 * @param[in] data - A pointer to the byte the status register's value to be written.
		 * @param[in] cback - A pointer to the function to be called when the operation is complete.
		 * @param[in] param - a void pointer to an object passed as an arguement to the cback function.
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Set_Status( const uint8_t* data, cback_func_t cback,
										 cback_param_t param );

		static const uint32_t PAGE_SIZE = 256;

		/**
		 * @brief Synchronously reads a specified number of bytes from a given address into a buffer.
		 * @param[in] data: A pointer to the buffer used to receive the data from the read operation.
		 * @param[in] address: The address of the first byte to be read.
		 * @param[in] length: The number of bytes to be read.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Read( UINT8* data, NV_CTRL_ADDRESS_TD address,
								  NV_CTRL_LENGTH_TD length, bool use_protection = true )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		virtual nv_status_t Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
										   NV_CTRL_LENGTH_TD length )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Synchronously writes a specified number of bytes to a given address from a buffer.
		 * @param[in] data: A pointer to the buffer used to hold the data for the write operation.
		 * @param[in] address: The address of the first byte to be written.
		 * @param[in] length: The number of bytes to be written.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Write( UINT8* data, UINT32 address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = true )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Synchronously erases a block of memory in the NV_Ctrl device. This may involve a range
		 * larger than specified in the address and length arguments as some devices have
		 * minimum block sizes that can be erased.
		 * @param[in] address: The address of the first byte to be erased.
		 * @param[in] length: The number of bytes to be erased.
		 * @param[in] use_protection: A flag to indicate fault detection/correction is used for this operation.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, UINT8 erase_data = 0, bool protected_data = true )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

		/**
		 * @brief Synchronously erases the entire NV_Ctrl device.
		 * @return status of requested operation.
		 */
		virtual nv_status_t Erase_All( void )
		{
			return ( OPERATION_NOT_SUPPORTED );
		}

	protected:
		/**
		 * @brief Checks the validity of the adress and length arguements used by some operations.
		 * @param[in] address - The start address of a requested memory operation
		 * @param[in] length - The length of a requested memory operation
		 * @return status of requested operation.
		 */
		NV_Ctrl::nv_status_t Check_Address( uint32_t address, uint32_t length );

		/**
		 * @brief Handles post call back processing for multi-step operations.
		 */
		void Process_Call_Back( void );

		/**
		 * @brief Provides a common method for initiating an exchange with the
		 * uC_Queued_SPI object.
		 */
		void Initiate_Exchange( void );

		/**
		 * @brief Initiates an SPI echange that enables write and erase operations.
		 * @param[in] initial_exchange - Indicates if this is the first exchange of the operation.
		 */
		void Enable_Writes( bool initial_exchange );

		/**
		 * @brief Handles the call back from the queued SPI object.
		 */
		virtual void Continue( void );

		uC_Queued_SPI* m_spi_ctrl;
		uC_Queued_SPI::exchange_struct_t m_exchange;
		uC_Queued_SPI::exchange_segment_t m_command_seg;
		uC_Queued_SPI::exchange_segment_t m_data_seg;
		NV_Ctrl::cback_func_t m_cback;
		NV_Ctrl::cback_param_t m_cback_param;
		uint32_t m_address;
		uint32_t m_length;
		uint32_t m_max_address;
		uint8_t m_command[6];
		uint8_t m_write_buffer[4096];
		uint32_t m_write_address;
		uint32_t m_write_length;
		const uint8_t* m_write_data;
		uint8_t m_block_erase_opcode;
		uint8_t m_read_opcode;
		uint8_t m_read_command_length;
		uint8_t m_state;

		volatile NV_Ctrl::nv_status_t m_delayed_result;

	private:
		/**
		 * @brief A protected default constructor to inhibit the instantiation of the class without
		 * the appropriate arguments.
		 */
		AT25DFx( void )
		{}

		/**
		 * @brief Provides the value of the AT25DFx device's status register.
		 */
		void Get_Status( void );

		static BF_Lib::Timers* m_wait_timer;	// CR timer instance
		static bool m_callback_pending;
		static uint16_t m_delay_step;

	public:
		/**
		 * @brief Returns a pointer to the class SPI write buffer for test purposes.
		 * @return A pointer to the class SPI write buffer.
		 */
		const uint8_t* Get_Write_Buffer( void )
		{
			return ( &m_write_buffer[0] );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Wait_Time_Expired_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{
			( ( AT25DFx* )handle )->Wait_Time_Expired();
		}

		void Wait_Time_Expired();

};

}

#endif
