/**
 *****************************************************************************************
 *	@file		uC_CAN.h
 *
 *	@brief		The file shall wrap all the functions Which are required for CAN communication
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef uC_CAN_H
   #define uC_CAN_H

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "driver/twai.h"
#include "freertos/portmacro.h"
#include "soc/twai_struct.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint8_t FRAME_MAX_LEN = 13;			///< EFF with 8 bytes of data
static const uint8_t FRAME_MAX_DATA_LEN = 1;		///< Max data bytes allowed in CAN2.0
static const uint8_t FRAME_EXTD_ID_LEN = 1;			///< EFF ID requires 4 bytes (29bit)
static const uint8_t FRAME_STD_ID_LEN = 1;			///< SFF ID requires 2 bytes (11bit)

/**
 ****************************************************************************************
 * @brief ESP32 Can frame format union
 *
 ****************************************************************************************
 */
union CAN_FrameBuf_t
{
	struct
	{
		struct
		{
			uint8_t dlc : 4;						///< Data length code (0 to 8) of the frame
			uint8_t self_reception : 1;				///< This frame should be transmitted using self reception command
			uint8_t single_shot : 1;				///< This frame should be transmitted using single shot command
			uint8_t rtr : 1;						///< This frame is a remote transmission request
			uint8_t frame_format : 1;				///< Format of the frame (1 = extended, 0 = standard)
		};

		union
		{
			struct
			{
				uint8_t id[FRAME_STD_ID_LEN];		///< 11 bit standard frame identifier
				uint8_t data[FRAME_MAX_DATA_LEN];	///< Data bytes (0 to 8)
				uint8_t reserved8[2];
			} standard_format;
			struct
			{
				uint8_t id[FRAME_EXTD_ID_LEN];		///< 29 bit extended frame identifier
				uint8_t data[FRAME_MAX_DATA_LEN];	///< Data bytes (0 to 8)
			} extended_format;
		};

	};

	uint8_t bytes[FRAME_MAX_LEN];
};

/**
 ****************************************************************************************
 * @brief ESP32 Message buffer structure
 *
 ****************************************************************************************
 */
struct CAN_MessageBuf
{
	uint32_t flags;					///< Bit field of message flags indicates frame/transmission type
	uint32_t identifier;			///< 11 or 29 bit identifier
	uint8_t data_length_code;		///< Data length code
	uint8_t data[FRAME_MAX_DATA_LEN];	///< Data bytes (not relevant in RTR frame)
};

/**
 **********************************************************************************************
 * @brief			uC_CAN Class Declaration. The class will handle all the functionalities
 * @n				related to CAN communication.
 **********************************************************************************************
 */
class uC_CAN
{
	public:
		/**
		 * @brief									Constructor to create instance of uC_CAN class.
		 * @param[in] CAN_Select					The CAN to select
		 * @param[in] uC_BASE_CAN_IO_PORT_STRUCT	CAN IO structure pointer
		 */
		uC_CAN( uint8_t CAN_Select = 0, uC_BASE_CAN_IO_PORT_STRUCT const* io_ctrl = nullptr );

		/**
		 * @brief									Destructor to delete an instance of uC_CAN class
		 * @return
		 */
		~uC_CAN( void ){}

		/* Enable or Disable requeste CAN functional mode */
		/**
		 * @brief							Enable or Disable request CAN functional mode
		 * @param[in] CANFunc				CAN function mode.
		 * @param[in] funcState				Enable or disable details
		 * @return void						None
		 */
		void Configure_CAN_Function_State( CAN_Function_Mode CANFunc, FunctionalState funcState = DISABLE );

		/**
		 * @brief							Function sets the CAN communication Baud rate
		 * @param[in] CANBaudRate			Baud rate to be set
		 * @param[in] Timing_Config			Timing configuration structure pointer
		 * @return void						None
		 */
		void Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def CANBaudRate, can_timing_config_t* Timing_Config = nullptr );

		/**
		 * @brief							Callback function for CAN communication.
		 * @param[in] int_handler			ISR function pointer
		 * @param[in] priority				Interrupt priority
		 * @param[in] mbxNo					Mail box number
		 * @param[in] port_requested		Requested port details
		 * @return void						None
		 */
		void CAN_Register_callback( INT_CALLBACK_FUNC int_handler,
									uint8_t priority, uint8_t mbxNo = 0,
									uint8_t port_requested = 1 );

		/**
		 * @brief							Function which Initialize CAN driver
		 * @param[in] timing_config			Timing configuration structure pointer
		 * @return void						None
		 */
		void Initialize_CAN_Bus( can_timing_config_t* timing_config );

		/**
		 * @brief							Function to enable CAN communication
		 * @return void						None
		 */
		void Enable_CAN( void );

		/**
		 * @brief							To configure CAN filter
		 * @param[in] CAN_FilterInitStruct	CAN filter initial configuration structure pointer
		 * @return void						None
		 */
		void Configure_CAN_filter( CAN_FilterConfTypeDef* CAN_FilterInitStruct );

		/**
		 * @brief							To configure CAN interrupt
		 * @param[in] CAN_IT				CAN interrupt
		 * @param[in] NewState				Enable or disable state details
		 * @return void						None
		 */
		void Configure_CAN_Interrupt( uint32_t CAN_IT = 0, FunctionalState NewState = DISABLE );

		/**
		 * @brief							Initiates the transmission of a message.
		 * @param  TxMessage:				pointer to a structure which contains CAN Id, CAN
		 * @n								DLC and CAN data's.
		 * @return void						None
		 */
		void CAN_Data_Transmit( const CAN_MessageBuf* TxMessage );

		/**
		 * @brief							Receives a message.
		 * @param  RxMessage:				pointer to a structure receive message which
		 * @n								Contains CAN Id, CAN DLC, CAN data's and FMI number.
		 * @return							None.
		 */
		bool CAN_Receive_Data( CAN_MessageBuf* RxMessage );

		/**
		 * @brief							Get number of pending messages
		 * @param  FIFONumber				FIFO number
		 * @return uint8_t					Pending message number
		 */
		uint8_t Get_No_Of_Pending_CAN_Mesgs( uint8_t FIFONumber ){return ( 1 );}

		/**
		 * @brief							Check a CAN error.
		 * @return bool						True or False.
		 */
		uint8_t check_CANError( void );

		/**
		 * @brief							Resets CAN error registers
		 * @return Void						None
		 */
		void reset_CANError_register( void );

		/**
		 * @brief							Check a CAN Buss off State.
		 * @return Void						None
		 */
		uint8_t check_CANBusOff( void );

		/**
		 * @brief					Check a CAN Error counter reached to Passive State.
		 * @return uint8_t			If count is non zero then one will be the output else zero will be the output
		 */
		uint8_t check_CANErrCntrPassiveState( void );

		/**
		 * @brief					Check a CAN Error counter reached to Warn State.
		 * @param  status			Can status register pointer
		 * @return uint8_t			If count is non zero then one will be the output else zero will be the output
		 */
		uint8_t check_CANErrCntrWarnState( can_status_reg_t* status = nullptr );

		/**
		 * @brief					This function encodes a message into a frame structure. The frame structure has
		 * @n						an identical layout to the TX buffer, allowing the frame structure to be directly
		 * @n						copied into TX buffer
		 * @param  CAN_Filter_Index	CAN filter index
		 * @param  mesg_id			Message ID
		 * @param  format			Message format
		 * @param  isRTR			Is RTR
		 * @param  data				Data pointer
		 * @param  flags			Flag details
		 * @param  tx_frame			Transmit frame buffer pointer
		 * @return void				None
		 */
		void InitMailboxRegisters( uint8_t CAN_Filter_Index, uint32_t mesg_id, uint8_t format, uint8_t isRTR,
								   const uint8_t* data = nullptr, uint32_t flags = 0,
								   CAN_FrameBuf_t* tx_frame = nullptr );

		/**
		 * @brief					Configure CLKOUT. CLKOUT is a pre-scaled version of APB CLK. Divider can be
		 * @n						1, or any even number from 2 to 14. Set to out of range value (0) to disable CLKOUT
		 * @param  divider			Clock divider value
		 * @return void				None
		 */
		void CAN_Config_clk_out( uint32_t divider );

		/**
		 * @brief					Configure CLKOUT. CLKOUT is a pre-scaled version of APB CLK. Divider can be
		 * @n						1, or any even number from 2 to 14. Set to out of range value (0) to disable CLKOUT
		 * @param  divider			Clock divider value
		 * @return void				None
		 */
		void CAN_Intr_Handler_err_warn( can_status_reg_t* status, int32_t* alert_req );

		/**
		 * @brief					CAN alert handler
		 * @param  alert_code		Alert code details
		 * @param  alert_req		Pointer for alert req
		 * @return void				None
		 */
		void CAN_Alert_Handler( uint32_t alert_code, int32_t* alert_req );

		/**
		 * @brief					CAN Passive error handler
		 * @param  alert_req		Pointer for alert req
		 * @return void				None
		 */
		void CAN_Intr_Handler_err_passive( int32_t* alert_req );

		/**
		 * @brief					CAN interrupt error handler
		 * @param  alert_req		Pointer for alert req
		 * @param  err_type			Error type
		 * @return void				None
		 */
		void CAN_Intr_Handler_err( int32_t* alert_req, uint8_t err_type );

		/**
		 * @brief					Sets transmit buffer and triggers transmission of CAN data
		 * @param frame				Pointer to CAN frame buffer
		 * @return void				None
		 */
		void CAN_Set_TX_Buffer_And_Transmit( CAN_FrameBuf_t* frame );

		/**
		 * @brief					Gets the error counter value
		 * @param tx_error_cnt		Pointer to Transmit error counter value
		 * @param rx_error_cnt		Pointer to Receive error counter value
		 * @return void				None
		 */
		void CAN_Get_Error_Counters( uint32_t* tx_error_cnt, uint32_t* rx_error_cnt );

		/**
		 * @brief					Stops the CAN driver module
		 * @return void				None
		 */
		void CAN_Stop( void );

		/**
		 * @brief					Un Install the CAN driver
		 * @return esp_err_t		ESP32 error type
		 */
		esp_err_t CAN_Driver_Uninstall( void );

		/*
		 *****************************************************************************************
		 *		Constants
		 *****************************************************************************************
		 */

		// Error details
		static const uint8_t BUS_ERROR = 0;								///< Bus error
		static const uint8_t ARB_ERROR = 1;								///< Arbitration error
		static const uint8_t ENTER_MODE = 1;							///< Enter to Rest mode
		static const uint8_t EXIT_MODE = 0;								///< Exit rest mode

		// Timing configuration
		static const uint8_t TIMING_CONFIG_VALUE_3 = 3;
		static const uint8_t TIMING_CONFIG_VALUE_4 = 4;
		static const uint8_t TIMING_CONFIG_VALUE_8 = 8;
		static const uint8_t TIMING_CONFIG_VALUE_15 = 15;
		static const uint8_t TIMING_CONFIG_VALUE_16 = 16;
		static const uint8_t TIMING_CONFIG_VALUE_32 = 32;
		static const uint8_t TIMING_CONFIG_VALUE_40 = 40;
		static const uint8_t TIMING_CONFIG_VALUE_80 = 80;
		static const uint8_t TIMING_CONFIG_VALUE_128 = 128;

		static const uint8_t ENABLE_VALUE = 1;
		static const uint8_t DISABLE_VALUE = 0;
		static const uint8_t MAX_FILTER_VALUE = 4;
		static const int32_t FILTER_INDEX_VALUE = 8;

		static const uint32_t DRIVER_DEFAULT_ERR_PASS_CNT = 128;		///< Error counter threshold for error passive
		static const uint8_t DRIVER_DEFAULT_INTERRUPTS = 0xE7;			///< Exclude data overrun
		static const uint8_t DRIVER_DEFAULT_EWL = 96;					///< Default Error Warning Limit value
		static const uint8_t DRIVER_DEFAULT_TEC = 0;					///< TX Error Counter starting value
		static const uint8_t DRIVER_DEFAULT_REC = 0;					///< RX Error Counter starting value

		// Clock configuration constatnts
		static const uint8_t MIN_CLK_DIVIDER_VALUE = 2;
		static const uint8_t MAX_CLK_DIVIDER_VALUE = 14;
		static const uint8_t DIVIDER_VALUE = 7;

	private:

		/**
		 * @brief					Configures CAN communication
		 * @return void				None
		 */
		void Configure_CAN_Communication( void );

		/**
		 * @brief					Base CAN IO structure
		 */
		uC_BASE_CAN_IO_STRUCT const* m_config;

		/**
		 * @brief							Function to check the CAN
		 * @param[in] validation_result		true if the validation is correct or else false will be sent
		 * @param[in] error_type			esp Error type
		 * @return esp_err_t				esp error type
		 */
		static esp_err_t Check_CAN( bool validation_result, esp_err_t error_type );

		/**
		 * @brief							Function to check the CAN from CRIT
		 * @param[in] validation_result		true if the validation is correct or else false will be sent
		 * @param[in] error_type			esp Error type
		 * @return esp_err_t				esp error type
		 */
		static esp_err_t CAN_Check_From_CRIT( bool validation_result, esp_err_t error_type );

		esp_err_t CAN_Reset_Mode( uint8_t Type );

		/**
		 * @brief							Function set the CAN flag
		 * @param[in] flag					Flag to be set
		 * @param[in] mask					Mask input
		 * @return  void					None
		 */
		static inline void CAN_Set_Flag( uint32_t flag, uint32_t mask )
		{  ( ( flag ) |= ( mask ) ); }

		/**
		 * @brief							Function Reset the CAN flag
		 * @param[in] flag					Flag to be set
		 * @param[in] mask					Mask input
		 * @return  void					None
		 */
		static inline void CAN_Reset_Flag( uint32_t flag, uint32_t mask )
		{  ( ( flag ) &= ~( mask ) ); }

};

#endif

#ifdef __cplusplus
}
#endif
