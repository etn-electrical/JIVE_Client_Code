/**
 *****************************************************************************************
 *	@file		uC_I2C.h
 *
 *	@brief		The file shall wrap all the functions which are required for communication
 *	@n			between master and slave
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __uC_I2C_h__
#define     __uC_I2C_h__

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "driver/i2c.h"
#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "Etn_Types.h"
#include "string.h"
#include "freertos/FreeRTOS.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

static const uint8_t ACK_CHECK_EN = 0x1;		///< I2C master will check ack from slave

/**
 **********************************************************************************************
 * @brief			I2C Class Declaration. The class will handle all the functionalities
 * @n				related to I2C communication.
 **********************************************************************************************
 */
class uC_I2C
{
	public:

		/**
		 * @brief		I2C communication error return types
		 */
		enum return_type
		{
			NO_ERROR = 0U,	///< NO Error in I2C communication
			I2C_ERROR		///< Error in I2C communication
		};

		/**
		 * @brief		I2C Addressing modes
		 */
		enum address_type_t
		{
			ADDRESS_7_BIT,	///< 7 bit addressing mode
			ADDRESS_10_BIT	///< 10 bit addressing mode
		};

		/**
		 * @brief		I2C channel states
		 */
		enum channel_state_t
		{
			CHANNEL_FREE = 0U,	///< I2C channel is free to initiate new transfer
			CHANNEL_BUSY		///< I2C channel is busy
		};

		/**
		 * @brief		I2C duty cycle types
		 */
		enum duty_type_t
		{
			FASTMODE_DUTYCYCLE_2,	///< I2C fast mode Tlow/Thigh = 2
			FASTMODE_DUTYCYCLE_16_9	///< I2C fast mode Tlow/Thigh = 16/9
		};

		/**
		 * @brief		I2C communication mode type
		 */
		enum i2c_mode_t
		{
			I2C_STANDARD_MODE,	///< Standard Mode communication @100KHz
			I2C_FAST_MODE		///< Standard Mode communication @400KHz
		};

		/**
		 * @brief		I2C communication status type
		 */
		enum i2c_status_t
		{
			I2C_STATUS_READ,		///< read status for current master command
			I2C_STATUS_WRITE,		///< write status for current master command
			I2C_STATUS_IDLE,		///< idle status for current master command
			I2C_STATUS_ACK_ERROR,	///< ack error status for current master command
			I2C_STATUS_DONE,		///< I2C command done
			I2C_STATUS_TIMEOUT,		///< I2C bus status error, and operation timeout
			NO_ERROR_INIT			///< No error initialization state
		};

		/**
		 * @brief		type defines
		 */
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief						Constructor to create instance of uc_I2C class.
		 * @param[in] channel			The I2C channel of microcontroller which is to be used for
		 * @n							communication
		 * @param[in] device_address	The device address of chip either 7 bit of 10 bit
		 * @param[in] speed_mode		Speed of I2C bus for this channel not used for ESP32 I2C implementation .
		 * @parm[in] addr_mode			Addressing mode either 10 bit addressing or 7 bit addressing.
		 * @n							Possible values are
		 * @n							0 - 7 bit addressing
		 * @n							1 - 10 bit addressing
		 * @param[in] synchronus_trnasfer	The user can use this option to select if the transfer
		 * @n							will be synchronous or asynchronous.
		 * @param[in] enable_dma		If I2C will use DMA for data transfer.
		 * @n							true -DMA enabled for I2C data transfer
		 * @n							false -DMA not used for I2C data transfer
		 * @n							For ESP32 I2C over DMA is not supported
		 * @param[in] fastmode_duty_cycle	Duty cycle selection for fast mode not used for ESP32 I2C implementation
		 */
		uC_I2C( uint8_t channel, uint16_t device_address, i2c_mode_t speed_mode =
				I2C_STANDARD_MODE, address_type_t addr_mode = ADDRESS_7_BIT,
				bool synchronus_trnasfer = false, bool enable_dma = false,
				duty_type_t fastmode_duty_cycle = FASTMODE_DUTYCYCLE_16_9,
				uC_BASE_I2C_IO_PORT_STRUCT const* io_port = static_cast<uC_BASE_I2C_IO_PORT_STRUCT const*>
				( nullptr ) );

		/**
		 * @brief						Destructor to delete an instance of uc_I2C class
		 * @return
		 */
		~uC_I2C( void )
		{}

		/**
		 * @brief					Establishes communication with addresses slave and reads data over
		 * @n						an I2C bus from the slave.
		 * @param[in] address		Address of slave from which data to be read
		 * @param[in] length		Amount of data to be read(number of bytes) from slave
		 * @param[in] datasource	Pointer/RAM location where the read data will be put.
		 * @param[in] read_status	This parameter is valid only in case of synchronous transfer.
		 * @n						not used for ESP32 I2C implementation
		 * @param[in] cmd			i2c_cmd_handle_t structure reference
		 * @note					The user shall use this API to read the data from I2C slave.
		 * @return return_type		Returns I2C error or no error
		 */
		return_type Read( uint16_t address, uint16_t length, uint8_t* datasource,
						  i2c_status_t* read_status = NULL, i2c_cmd_handle_t cmd = nullptr );

		/**
		 * @brief					Establishes communication with addresses slave and Write data over
		 * @n						an I2C bus to the slave.
		 * @param[in] address		Address of slave to which data to be transmitted
		 * @param[in] length		Amount of data to be transmitted(number of bytes) to slave
		 * @param[in] datasource	Pointer/RAM location where the read data will be put.
		 * @note					The user shall use this API to read the data from I2C slave.
		 * @param[in] write_status	This parameter is valid only in case of synchronous transfer.
		 * @n						not used for ESP32 I2C implementation
		 * @param[in] cmd			i2c_cmd_handle_t structure reference
		 * @note					The user shall use this API to write the data to I2C slave.
		 * @return return_type		Returns I2C error or no error
		 */
		return_type Write( uint16_t address, uint16_t length, uint8_t* datasource,
						   i2c_status_t* write_status = NULL, i2c_cmd_handle_t cmd = nullptr );

		/**
		 * @brief					Application layer can enquire if the transfer of data transfer
		 * @n						over I2C is finished or not
		 * @return i2c_status_t		It returns the status of I2C transfer
		 * @note					The user shall use this API to read the data from I2C slave.
		 * @return void				None
		 */
		i2c_status_t I2C_Transfer_Status( void );

		/**
		 * @brief					Attaching callback function for notifying that last transaction was successful.
		 * @param[in] cback_func	The callback function which will be called when data transfer will be completed.
		 * @note					The function is valid only in case of asynchronous data transfer.
		 * @return void				None
		 */
		void Attach_Callback( cback_func_t cback_func = NULL, cback_param_t param = NULL );

		/**
		 * @brief					I2C interrupt details
		 * @param[in] arg			Interrupt service routine function pointer
		 * @return void				None
		 */
		static void I2C_ISR( void* arg );

		/**
		 * @brief					A static class call back redirector to pass control to the
		 * @n						appropriate object-specific I2C interrupt handler.
		 * @param [in] object		pointer object call back is itended for.
		 * @param [in] i2c_num		I2C number
		 * @return void				None
		 */
		static void I2C_ISR_Static( void* object = nullptr, int32_t i2c_num = I2C_NUM_MAX );

		/**
		 * @brief						A static class call back redirector to pass control to the
		 * @n							appropriate object-specific I2C interrupt handler.
		 * @param [in] slv_rx_buf_len	pointer object call back is itended for.
		 * @param [in] i2c_num			I2C number
		 * @return	esp_err_t			ESP error type
		 */
		esp_err_t I2C_Driver_Install( i2c_port_t i2c_num, size_t slv_rx_buf_len, size_t slv_tx_buf_len,
									  int32_t intr_alloc_flags );

	private:

		/**
		 * @brief							Initialization of an I2C channel
		 * @param [in] i2c_channel			I2C Channel number
		 * @param [in] i2c_mode				I2C mode type (Standard mode or fast mode)
		 * @param [in] fmode_duty_cycle		Duty cycle details
		 * @return void						None
		 */
		void Init_I2C( uint8_t i2c_channel, i2c_mode_t i2c_mode,
					   duty_type_t fmode_duty_cycle );

		/**
		 * @brief						Function which enables the I2C peripheral of microcontroller
		 * @param [in] i2c_num			I2C number
		 * @return void					None
		 */
		inline void Enable_I2C( i2c_port_t i2c_num );

		/**
		 * @brief							Function for generating start condition
		 * @param [in] cmd_handle			i2c_cmd_handle_t structure reference
		 * @return return_type				It returns the I2C error type
		 */
		return_type Generate_Start( i2c_cmd_handle_t cmd_handle );

		/**
		 * @brief							Function for sending the data over an I2C bus
		 * @param [in] data					I2C Data
		 * @param [in] ack_en				Ack enable or not(True or False)
		 * @param [in] cmd_handle			i2c_cmd_handle_t structure reference
		 * @return return_type				It returns the I2C error type
		 */
		return_type Send_Data( uint8_t data, bool ack_en = ACK_CHECK_EN, i2c_cmd_handle_t cmd_handle = nullptr );

		/**
		 * @brief							Function for sending the data over an I2C bus
		 * @param [in] cmd_handle			i2c_cmd_handle_t structure reference
		 * @return return_type				It returns the I2C error type
		 */
		return_type Set_Stop( i2c_cmd_handle_t cmd_handle );

		/**
		 * @brief							Receives data from an I2C slave
		 * @param [in] cmd_handle			i2c_cmd_handle_t structure reference
		 * @param [in] data					I2C Data pointer
		 * @param [in] ack					ACK type
		 * @return return_type				It returns the I2C error type
		 */
		return_type Receive_Data( uint8_t* data, i2c_ack_type_t ack = I2C_MASTER_ACK_MAX,
								  i2c_cmd_handle_t cmd_handle = nullptr ) const;

		/**
		 * @brief							Receives data from an I2C slave
		 * @param [in] error				I2C error type
		 * @param [in] current_state		Present I2C bus state
		 * @param [in] next_state			I2C Bus next state
		 * @param [in] state_specific_error	Specific state error details
		 * @return i2c_status_t				It returns the I2C bus status
		 */
		i2c_status_t Set_State( return_type error, i2c_status_t* current_state,
								i2c_status_t next_state, i2c_status_t state_specific_error ) const;

		/**
		 * @brief							Sends the device address of the slave
		 * @param [in] adr					Device address
		 * @param [in] dir					Read or write direction
		 * @return return_type				It returns the I2C error type
		 */
		return_type Send_Device_Address( uint16_t adr, uint8_t dir );

		/**
		 * @brief							Function to check the I2C
		 * @param[in] validation_result		true if the validation is correct or else false will be sent
		 * @param[in] str					GPIO string used for displaying the error message
		 * @param[in] error_type			esp Error type
		 * @return esp_err_t				esp error type
		 */
		static esp_err_t Check_I2C( bool validation_result, const char* str, esp_err_t error_type );

		/**
		 * @brief							This function set I2C pins
		 * @param[in] i2c_num				I2C number
		 * @return esp_err_t				esp error type
		 */
		esp_err_t Set_I2C_Pins( i2c_port_t i2c_num );

		/**
		 * @brief							Function which enables the I2C peripheral of microcontroller
		 * @param[in] i2c_num				I2C number
		 */
		inline void Disable_I2C( i2c_port_t i2c_num );

		uint16_t m_chip_address;				///< Device Address of an I2C slave
		address_type_t m_addressing_mode;		///< addressing mode of the device
		bool_t m_dma_transfer;					///< if the data transfer between I2C devices is using DMA or not
		bool_t m_synchronous_type;				///< Synchrouns type that is Synchrouns or Asynchronous communication
		cback_func_t m_call_back_ptr;			///< Coroutine functionality with a function pointer
		cback_param_t m_cback_param;			///< It for applications where you planning for coroutine functionality
		uC_BASE_I2C_IO_STRUCT const* m_config;	///< I2C configuration structure.
		IRQn_Type m_i2c_vector_number;			///< I2C interrupt event vector number
		uC_BASE_I2C_IO_PORT_STRUCT const* m_io_ctrl;	///< I2C I/O configuration
		duty_type_t m_f_duty_cycle;				///< fast mode duty cycle
		i2c_mode_t m_i2c_speed_type;			///< I2C speed type standard mode/fast mode
		volatile i2c_status_t m_transfer_status;///< Holds the status of I2C data transmisson/reception
		uint8_t m_channel_number;				///< channel number associated with an I2C object

		/**
		 * @brief	Static constants
		 */
		static const uint32_t I2C_ClockSpeed_100K = static_cast<uint32_t>( 100000U );		///< I2C speed 100kbps
		static const uint32_t I2C_ClockSpeed_400K = static_cast<uint32_t>( 400000U );		///< I2C speed 400kbps
		static const uint8_t I2C_READ = 0x01U;	///< I2C Read bit
		static const uint8_t I2C_WRITE = 0x00U;	///< I2C Write bit
		static const uint8_t ADDR_HEADER_10_BIT = ( 0xF0U );	///< I2C 10bit header upper byte
		// I2C bus worst case Timeout in uSec considering ACK time and setup and hold time
		static const uint8_t I2C_TIMEOUT = ( 16U );
		static const uint16_t CPAL_I2C_STATUS_ERR_MASK = static_cast<uint16_t>( 0x0F00U );		///< I2C error mask byte
		static const uint8_t MAX_END_ADDR_7_BIT = static_cast<uint8_t>( 0x7FU );		///< Maximum 7 Bit end address
		static const uint16_t MAX_END_ADDR_10_BIT = static_cast<uint16_t>( 0x3FFU );	///< Maximum 10 Bit end address
};

#endif

#ifdef __cplusplus
}
#endif
