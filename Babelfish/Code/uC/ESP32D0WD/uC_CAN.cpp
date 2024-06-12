/**
 *****************************************************************************************
 *	@file		uC_CAN.cpp
 *	@details	See header file for module overview.
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_Base.h"
#include "uC_CAN.h"
#include "uC_IO_Define_ESP32.h"
#include "soc/gpio_sig_map.h"
#include "freertos/task.h"
#include "uC_IO_Config.h"
#include "freertos/queue.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const char* CAN_TAG = "CAN";
// static const uint8_t STANDARD_FORMAT = 0;
// static const uint8_t EXTENDED_FORMAT = 1;
static const uint8_t ISRTR = 0x0;						///< ISTR is not in use with ESP32.
static const uint8_t CAN_FILTER_INDEX = 0x0;			///< CAN_FILTER_INDEX is not in use with ESP32.

// Control flags
static const uint32_t CTRL_FLAG_STOPPED = 0x001;		///< CAN peripheral in stopped state
static const uint32_t CTRL_FLAG_RECOVERING = 0x002;		///< Bus is undergoing bus recovery
static const uint32_t CTRL_FLAG_ERR_WARN = 0x004;		///< TEC or REC is >= error warning limit
static const uint32_t CTRL_FLAG_ERR_PASSIVE = 0x008;	///< TEC or REC is >= 128
static const uint32_t CTRL_FLAG_BUS_OFF = 0x010;		///< Bus-off due to TEC >= 256
static const uint32_t CTRL_FLAG_TX_BUFF_OCCUPIED = 0x020;///< Transmit buffer is occupied

// Command Bit Masks
static const uint8_t CMD_TX_REQ = 0x01;					///< Transmission Request
static const uint8_t CMD_SELF_RX_REQ = 0x10;			///< Self Reception Request
static const uint8_t CMD_TX_SINGLE_SHOT = 0x03;			///< Single Shot Transmission
static const uint8_t CMD_SELF_RX_SINGLE_SHOT = 0x12;	///< Single Shot Self Reception

/*
 *****************************************************************************************
 *		Static variables
 *****************************************************************************************
 */
static can_obj_t* p_can_obj_temp = NULL;
static portMUX_TYPE can_spinlock = portMUX_INITIALIZER_UNLOCKED;

/*
 *****************************************************************************************
 *		Defines
 *****************************************************************************************
 */
#define CAN_ENTER_CRITICAL()  portENTER_CRITICAL( &can_spinlock )
#define CAN_EXIT_CRITICAL()   portEXIT_CRITICAL( &can_spinlock )

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_CAN::uC_CAN( uint8_t CAN_Select, uC_BASE_CAN_IO_PORT_STRUCT const* io_ctrl )
{
	uC_PERIPH_IO_STRUCT temp_PERIPH_IO_STRUCT;

	m_config = &uC_Base::m_can_io[CAN_Select];
	if ( m_config != NULL )
	{
		// uC_PERIPH_IO_STRUCT of m_config is used for CAN Rx and Tx
		uC_IO_INPUT_CFG_EN input_cfg = uC_IO_IN_CFG_HIZ;

		// Set TX pin
		temp_PERIPH_IO_STRUCT = m_config->default_port_io.tx_pio;
		uC_IO_Config::Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, input_cfg );
		gpio_matrix_out( temp_PERIPH_IO_STRUCT.io_num, CAN_TX_IDX, false, false );
		gpio_pad_select_gpio( temp_PERIPH_IO_STRUCT.io_num );

		// Set RX pin
		temp_PERIPH_IO_STRUCT = m_config->default_port_io.rx_pio;
		uC_IO_Config::Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, input_cfg );
		gpio_matrix_in( temp_PERIPH_IO_STRUCT.io_num, CAN_RX_IDX, false );
		gpio_pad_select_gpio( temp_PERIPH_IO_STRUCT.io_num );
		uC_IO_Config::Set_GPIO_Direction( temp_PERIPH_IO_STRUCT.io_num, GPIO_MODE_INPUT );

		// Configure output clock pin (Optional)
		temp_PERIPH_IO_STRUCT = m_config->clock_port.tx_pio;
		if ( ( temp_PERIPH_IO_STRUCT.io_num >= 0 ) && ( temp_PERIPH_IO_STRUCT.io_num < GPIO_NUM_MAX ) )
		{
			uC_IO_Config::Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, input_cfg );
			gpio_matrix_out( temp_PERIPH_IO_STRUCT.io_num, CAN_CLKOUT_IDX, false, false );
			gpio_pad_select_gpio( temp_PERIPH_IO_STRUCT.io_num );
		}

		// Configure bus status pin (Optional)
		temp_PERIPH_IO_STRUCT = m_config->clock_port.rx_pio;
		if ( ( temp_PERIPH_IO_STRUCT.io_num >= 0 ) && ( temp_PERIPH_IO_STRUCT.io_num < GPIO_NUM_MAX ) )
		{
			uC_IO_Config::Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, input_cfg );
			gpio_matrix_out( temp_PERIPH_IO_STRUCT.io_num, CAN_BUS_OFF_ON_IDX, false, false );
			gpio_pad_select_gpio( temp_PERIPH_IO_STRUCT.io_num );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Initialize_CAN_Bus( can_timing_config_t* timing_config )
{
	can_general_config_t g_config;
	can_filter_config_t f_config;
	esp_err_t ret;

	// Check arguments
	Check_CAN( m_config != NULL, ESP_ERR_INVALID_ARG );
	Check_CAN( m_config->rx_queue_len > 0, ESP_ERR_INVALID_ARG );
	Check_CAN( m_config->default_port_io.tx_pio.io_num >= 0 &&
			   m_config->default_port_io.tx_pio.io_num < GPIO_NUM_MAX, ESP_ERR_INVALID_ARG );
	Check_CAN( m_config->default_port_io.rx_pio.io_num >= 0 &&
			   m_config->default_port_io.rx_pio.io_num < GPIO_NUM_MAX, ESP_ERR_INVALID_ARG );

	g_config.mode = can_mode_t( m_config->mode );
	g_config.tx_io = m_config->default_port_io.tx_pio.io_num;
	g_config.rx_io = m_config->default_port_io.rx_pio.io_num;
	g_config.clkout_io = m_config->clock_port.tx_pio.io_num;
	g_config.bus_off_io = m_config->clock_port.rx_pio.io_num;
	g_config.tx_queue_len = m_config->tx_queue_len;
	g_config.rx_queue_len = m_config->rx_queue_len;
	g_config.alerts_enabled = m_config->alerts_enabled;
	g_config.clkout_divider = m_config->clkout_divider;

	f_config.acceptance_code = m_config->filter_config.acceptance_code;
	f_config.acceptance_mask = m_config->filter_config.acceptance_mask;
	f_config.single_filter = m_config->filter_config.single_filter;

	ret = can_driver_install( &g_config, timing_config, &f_config );
	if ( ret != ESP_OK )
	{
		ESP_LOGI( CAN_TAG, "Driver not installed successfully " );
	}
	else
	{
		ESP_LOGI( CAN_TAG, "Driver installed successfully " );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def CANBaudRate, can_timing_config_t* Timing_Config )
{
	switch ( CANBaudRate )
	{
		case CAN_BAUD_RATE_1000KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_4;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_15;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_4;
			break;

		case CAN_BAUD_RATE_800KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_4;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_16;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_8;
			break;

		case CAN_BAUD_RATE_500KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_8;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_15;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_4;
			break;

		case CAN_BAUD_RATE_250KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_16;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_15;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_4;
			break;

		case CAN_BAUD_RATE_125KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_32;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_15;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_4;
			break;

		case CAN_BAUD_RATE_100KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_40;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_15;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_4;
			break;

		case CAN_BAUD_RATE_50KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_80;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_15;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_4;
			break;

		case CAN_BAUD_RATE_25KBPS:
			Timing_Config->brp = TIMING_CONFIG_VALUE_128;
			Timing_Config->tseg_1 = TIMING_CONFIG_VALUE_16;
			Timing_Config->tseg_2 = TIMING_CONFIG_VALUE_8;
			break;

		case CAN_MAX_NO_BAUD_RATE:
		default:
			break;
	}
	Timing_Config->sjw = TIMING_CONFIG_VALUE_3;
	Timing_Config->triple_sampling = false;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Configure_CAN_Function_State( CAN_Function_Mode CANFunc, FunctionalState funcState )
{
	// Configure CAN mode of operation
	can_mode_reg_t mode_reg;

	mode_reg.val = CAN.mode_reg.val;				///< Get current value of mode register
	if ( CANFunc == CAN_NO_ACK )
	{
		// No ACknowledgement mode
		mode_reg.self_test = ENABLE_VALUE;
		mode_reg.listen_only = DISABLE_VALUE;
	}
	else if ( CANFunc == CAN_LISTEN_ONLY )
	{
		// Listen only mode
		mode_reg.self_test = DISABLE_VALUE;
		mode_reg.listen_only = ENABLE_VALUE;
	}
	else
	{
		// Default to normal operating mode
		mode_reg.self_test = DISABLE_VALUE;
		mode_reg.listen_only = DISABLE_VALUE;
	}
	CAN.mode_reg.val = mode_reg.val;				///< Write back modified value to register
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Enable_CAN( void )
{
	esp_err_t ret;

	// Can enter critical
	CAN_ENTER_CRITICAL();
	ret = can_start();
	if ( ret != ESP_OK )
	{
		ESP_LOGI( CAN_TAG, "Module start failed " );
	}
	else
	{
		ESP_LOGI( CAN_TAG, "Module started " );
	}
	CAN_Reset_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_STOPPED );
	// Can exit critical
	CAN_EXIT_CRITICAL();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::InitMailboxRegisters( uint8_t CAN_Filter_Index, uint32_t mesg_id, uint8_t format, uint8_t isRTR,
								   const uint8_t* data, uint32_t flags, CAN_FrameBuf_t* tx_frame )
{
	int32_t id_len;
	int32_t i;
	uint8_t* data_buffer;
	uint32_t id_temp;
	uint8_t* id_buffer;

	// Set frame information
	tx_frame->dlc = format;
	tx_frame->rtr = ( flags & CAN_MSG_FLAG_RTR ) ? ENABLE_VALUE : DISABLE_VALUE;
	tx_frame->frame_format = ( flags & CAN_MSG_FLAG_EXTD ) ? ENABLE_VALUE : DISABLE_VALUE;
	tx_frame->self_reception = ( flags & CAN_MSG_FLAG_SELF ) ? ENABLE_VALUE : DISABLE_VALUE;
	tx_frame->single_shot = ( flags & CAN_MSG_FLAG_SS ) ? ENABLE_VALUE : DISABLE_VALUE;

	// Set ID
	id_len = ( flags & CAN_MSG_FLAG_EXTD ) ? FRAME_EXTD_ID_LEN : FRAME_STD_ID_LEN;
	id_buffer = ( flags & CAN_MSG_FLAG_EXTD ) ? tx_frame->extended_format.id : tx_frame->standard_format.id;
	// Split ID into 4 or 2 bytes, and turn into big-endian with left alignment (<< 3 or 5)
	id_temp = ( flags & CAN_MSG_FLAG_EXTD ) ? __builtin_bswap32( ( mesg_id & CAN_EXTD_ID_MASK ) << 3 ) :
		__builtin_bswap16( ( mesg_id & CAN_STD_ID_MASK ) << 5 );
	for ( i = 0; i < id_len; i++ )
	{
		id_buffer[i] = ( id_temp >> ( FILTER_INDEX_VALUE * i ) ) & 0xFF;					///< Copy big-endian ID
		// byte by byte
	}

	// Set Data.
	data_buffer = ( flags & CAN_MSG_FLAG_EXTD ) ? tx_frame->extended_format.data : tx_frame->standard_format.data;
	for ( i = 0; ( i < format ) && ( i < FRAME_MAX_DATA_LEN ); i++ )		///< Handle case where format is > 8
	{
		data_buffer[i] = data[i];
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Configure_CAN_filter( CAN_FilterConfTypeDef* CAN_FilterInitStruct )
{
	int32_t index;

	CAN.mode_reg.acceptance_filter =
		( CAN_FilterInitStruct->filter_config.single_filter ) ? ENABLE_VALUE : DISABLE_VALUE;

	// Swap code and mask to match big endian registers
	uint32_t code_swapped = __builtin_bswap32( CAN_FilterInitStruct->filter_config.acceptance_code );
	uint32_t mask_swapped = __builtin_bswap32( CAN_FilterInitStruct->filter_config.acceptance_mask );

	for ( index = 0; index < MAX_FILTER_VALUE; index++ )
	{
		CAN.acceptance_filter.code_reg[index].byte = ( ( code_swapped >> ( index * FILTER_INDEX_VALUE ) ) & 0xFF );
		CAN.acceptance_filter.mask_reg[index].byte = ( ( mask_swapped >> ( index * FILTER_INDEX_VALUE ) ) & 0xFF );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Configure_CAN_Interrupt( uint32_t CAN_IT, FunctionalState NewState )
{
	BaseType_t task_woken = pdFALSE;
	int32_t alert_req = 0;
	can_status_reg_t status;
	can_intr_reg_t intr_reason;

	CAN_ENTER_CRITICAL();
	status.val = CAN.status_reg.val;
	// In case interrupt occurs whilst driver is being un installed
	intr_reason.val = ( p_can_obj_temp != NULL ) ? CAN.interrupt_reg.val : DISABLE_VALUE;

	// Handle error counter related interrupts
	if ( intr_reason.err_warn )
	{
		// Triggers when Bus-Status or Error-status bits change
		CAN_Intr_Handler_err_warn( &status, &alert_req );
	}
	if ( intr_reason.err_passive )
	{
		// Triggers when entering/returning error passive/active state
		CAN_Intr_Handler_err_passive( &alert_req );
	}

	// Handle other error interrupts
	if ( intr_reason.bus_err )
	{
		// Triggers when an error (Bit, Stuff, CRC, Form, ACK) occurs on the CAN bus
		CAN_Intr_Handler_err( &alert_req, BUS_ERROR );
	}
	if ( intr_reason.arb_lost )
	{
		// Triggers when arbitration is lost
		CAN_Intr_Handler_err( &alert_req, ARB_ERROR );
	}
	CAN_EXIT_CRITICAL();

	if ( ( p_can_obj_temp->alert_semphr != NULL ) && alert_req )
	{
		// Give semaphore if alerts were triggered
		xSemaphoreGiveFromISR( p_can_obj_temp->alert_semphr, &task_woken );
	}
	if ( task_woken == pdTRUE )
	{
		portYIELD_FROM_ISR();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Data_Transmit( const CAN_MessageBuf* TxMessage )
{
	esp_err_t ret;
	CAN_FrameBuf_t tx_frame;
	uint32_t Index;
	can_message_t Temp_TX_msg;

	Temp_TX_msg.flags = TxMessage->flags;
	Temp_TX_msg.identifier = TxMessage->identifier;
	Temp_TX_msg.data_length_code = TxMessage->data_length_code;
	for ( Index = 0; Index < CAN_MAX_DATA_LEN; Index++ )
	{
		Temp_TX_msg.data[Index] = TxMessage->data[Index];
	}

	TickType_t PortDelay = portMAX_DELAY;

	// Check arguments
	Check_CAN( ( TxMessage->data_length_code <= FRAME_MAX_DATA_LEN ) ||
			   ( TxMessage->flags & CAN_MSG_FLAG_DLC_NON_COMP ), ESP_ERR_INVALID_ARG );

	CAN_ENTER_CRITICAL();
	// Format frame

	InitMailboxRegisters( CAN_FILTER_INDEX, TxMessage->identifier,
						  TxMessage->data_length_code, ISRTR, TxMessage->data,
						  TxMessage->flags, &tx_frame );
	ret = can_transmit( &Temp_TX_msg, PortDelay );
	if ( ret != ESP_OK )
	{
		ESP_LOGI( CAN_TAG, " Transmission of data is not successfull" );
	}
	CAN_EXIT_CRITICAL();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_CAN::CAN_Receive_Data( CAN_MessageBuf* RxMessage )
{
	can_message_t Temp_RX_msg;
	uint32_t Index;
	bool return_val = false;

	if ( can_receive( &Temp_RX_msg, pdMS_TO_TICKS( 0 ) ) == ESP_OK )
	{
		RxMessage->flags = Temp_RX_msg.flags;
		RxMessage->identifier = Temp_RX_msg.identifier;
		RxMessage->data_length_code = Temp_RX_msg.data_length_code;
		for ( Index = 0; Index < Temp_RX_msg.data_length_code; Index++ )
		{
			RxMessage->data[Index] = Temp_RX_msg.data[Index];
		}
		return_val = true;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Register_callback( INT_CALLBACK_FUNC int_handler, uint8_t priority,
									uint8_t mbxNo, uint8_t port_requested )
{
	interrupt_config_t* interrupt_config = reinterpret_cast<interrupt_config_t*>( int_handler );

	interrupt_config->priority = priority;
	interrupt_config->arg = NULL;
	interrupt_config->ret_handle = &p_can_obj_temp->isr_handle;
	interrupt_config->source = ETS_CAN_INTR_SOURCE;
	interrupt_config->intrstatusmask = DISABLE_VALUE;
	interrupt_config->intrstatusreg = DISABLE_VALUE;
	uC_Interrupt::Set_Vector( reinterpret_cast<INT_CALLBACK_FUNC*>( interrupt_config ), MAX_IRQTYPE, priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_CAN::check_CANError( void )
{

	uint8_t isCANInError = 0;
	uint32_t tec, rec;

	CAN_Get_Error_Counters( &tec, &rec );
	if ( ( tec != 0 ) || ( rec != 0 ) )
	{
		isCANInError = ENABLE_VALUE;
	}
	else
	{
		isCANInError = DISABLE_VALUE;
	}
	return ( isCANInError );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::reset_CANError_register( void )
{
	esp_err_t ret;

	ret = can_initiate_recovery();
	if ( ret != ESP_OK )
	{
		ESP_LOGI( CAN_TAG, " CAN driver is not in the bus-off state, or is not installed " );
	}
	else
	{
		ESP_LOGI( CAN_TAG, " Bus recovery started " );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_CAN::check_CANBusOff( void )
{
	uint8_t isCANBusOffStateReached = DISABLE_VALUE;
	can_status_info_t Bus_Error_Status_Info;

	can_get_status_info( &Bus_Error_Status_Info );
	// Bus-Off bits
	if ( Bus_Error_Status_Info.bus_error_count != DISABLE_VALUE )
	{
		isCANBusOffStateReached = ENABLE_VALUE;
	}
	else
	{
		isCANBusOffStateReached = DISABLE_VALUE;
	}
	return ( isCANBusOffStateReached );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_CAN::check_CANErrCntrPassiveState( void )
{
	uint32_t tec, rec;
	uint8_t isCANErrCntrPassiveStateReached;

	CAN_Get_Error_Counters( &tec, &rec );
	if ( ( tec >= DRIVER_DEFAULT_ERR_PASS_CNT ) || ( rec >= DRIVER_DEFAULT_ERR_PASS_CNT ) )
	{
		// Entered error passive
		isCANErrCntrPassiveStateReached = ENABLE_VALUE;
	}
	else
	{
		// Returned to error active
		isCANErrCntrPassiveStateReached = DISABLE_VALUE;
	}
	return ( isCANErrCntrPassiveStateReached );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t uC_CAN::check_CANErrCntrWarnState( can_status_reg_t* status )
{
	uint8_t isCANErrCntrWarnStateReached = 0;

	if ( ( status->bus != 0 ) && ( status->error != 0 ) )
	{
		// Bus-Off condition. TEC should set and held at 127, REC should be 0, reset mode entered
		CAN_Set_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_BUS_OFF );
		isCANErrCntrWarnStateReached = ENABLE_VALUE;
	}
	else if ( status->error )
	{
		// TEC or REC surpassed error warning limit
		CAN_Set_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_ERR_WARN );
		isCANErrCntrWarnStateReached = ENABLE_VALUE;
	}
	else
	{
		isCANErrCntrWarnStateReached = DISABLE_VALUE;
	}
	return ( isCANErrCntrWarnStateReached );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_CAN::Check_CAN( bool validation_result, esp_err_t error_type )
{
	if ( !( validation_result ) )
	{
		return ( error_type );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_CAN::CAN_Reset_Mode( uint8_t Type )
{
	if ( Type == 1 )
	{
		/* Enter reset mode (required to write to configuration registers). Reset mode
		   also prevents all CAN activity on the current module and is automatically
		   set upon entering a BUS-OFF condition. */
		CAN.mode_reg.reset = ENTER_MODE;		///< Set reset mode bit
		Check_CAN( CAN.mode_reg.reset == 1, ESP_ERR_INVALID_STATE );	// Check bit was set
	}
	else
	{
		/* Exiting reset mode will return the CAN module to operating mode. Reset mode
		   must also be exited in order to trigger BUS-OFF recovery sequence. */
		CAN.mode_reg.reset = EXIT_MODE;	///< Exit reset mode
		Check_CAN( CAN.mode_reg.reset == 0, ESP_ERR_INVALID_STATE );	// Check bit was reset
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Config_clk_out( uint32_t divider )
{
	can_clk_div_reg_t clock_divider_reg;

	clock_divider_reg.val = CAN.clock_divider_reg.val;
	if ( ( divider >= MIN_CLK_DIVIDER_VALUE ) && ( divider <= MAX_CLK_DIVIDER_VALUE ) )
	{
		clock_divider_reg.clock_off = DISABLE_VALUE;
		clock_divider_reg.clock_divider = ( divider / MIN_CLK_DIVIDER_VALUE ) - 1;
	}
	else if ( divider == 1 )
	{
		clock_divider_reg.clock_off = DISABLE_VALUE;
		clock_divider_reg.clock_divider = DIVIDER_VALUE;
	}
	else
	{
		clock_divider_reg.clock_off = ENABLE_VALUE;
		clock_divider_reg.clock_divider = DISABLE_VALUE;
	}
	CAN.clock_divider_reg.val = clock_divider_reg.val;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Alert_Handler( uint32_t alert_code, int32_t* alert_req )
{
	if ( ( p_can_obj_temp->alerts_enabled & alert_code ) != 0 )
	{
		// Signify alert has occurred
		CAN_Set_Flag( p_can_obj_temp->alerts_triggered, alert_code );
		*alert_req = 1;
		if ( p_can_obj_temp->alerts_enabled & CAN_ALERT_AND_LOG )
		{
			if ( alert_code >= CAN_ALERT_TX_FAILED )
			{
				ESP_EARLY_LOGE( CAN_TAG, "Alert %d", alert_code );
			}
			else if ( alert_code >= CAN_ALERT_ARB_LOST )
			{
				ESP_EARLY_LOGW( CAN_TAG, "Alert %d", alert_code );
			}
			else
			{
				ESP_EARLY_LOGI( CAN_TAG, "Alert %d", alert_code );
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Intr_Handler_err_warn( can_status_reg_t* status, int32_t* alert_req )
{
	if ( status->bus )
	{
		if ( status->error )
		{
			// Bus-Off condition. TEC should set and held at 127, REC should be 0, reset mode entered
			CAN_Set_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_BUS_OFF );

			/* Note: REC is still allowed to increase during bus-off. REC > err_warn
			   can prevent "bus recovery complete" interrupt from occurring. Set to
			   listen only mode to freeze REC. */
			Configure_CAN_Function_State( CAN_LISTEN_ONLY );
			CAN_Alert_Handler( CAN_ALERT_BUS_OFF, alert_req );
		}
		else
		{
			// Bus-recovery in progress. TEC has dropped below error warning limit
			CAN_Alert_Handler( CAN_ALERT_RECOVERY_IN_PROGRESS, alert_req );
		}
	}
	else
	{
		if ( status->error )
		{
			// TEC or REC surpassed error warning limit
			CAN_Set_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_ERR_WARN );
			CAN_Alert_Handler( CAN_ALERT_ABOVE_ERR_WARN, alert_req );
		}
		else if ( ( p_can_obj_temp->control_flags & CTRL_FLAG_RECOVERING ) != 0 )
		{
			// Bus recovery complete.
			CAN_Reset_Mode( ENTER_MODE );
			// Reset and set flags to the equivalent of the stopped state
			CAN_Reset_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_RECOVERING | CTRL_FLAG_ERR_WARN |
							CTRL_FLAG_ERR_PASSIVE | CTRL_FLAG_BUS_OFF |
							CTRL_FLAG_TX_BUFF_OCCUPIED );
			CAN_Set_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_STOPPED );
			CAN_Alert_Handler( CAN_ALERT_BUS_RECOVERED, alert_req );
		}
		else
		{
			// TEC and REC are both below error warning
			CAN_Reset_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_ERR_WARN );
			CAN_Alert_Handler( CAN_ALERT_BELOW_ERR_WARN, alert_req );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Intr_Handler_err_passive( int32_t* alert_req )
{
	uint32_t tec, rec;

	CAN_Get_Error_Counters( &tec, &rec );
	if ( ( tec >= DRIVER_DEFAULT_ERR_PASS_CNT ) || ( rec >= DRIVER_DEFAULT_ERR_PASS_CNT ) )
	{
		// Entered error passive
		CAN_Set_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_ERR_PASSIVE );
		CAN_Alert_Handler( CAN_ALERT_ERR_PASS, alert_req );
	}
	else
	{
		// Returned to error active
		CAN_Reset_Flag( p_can_obj_temp->control_flags, CTRL_FLAG_ERR_PASSIVE );
		CAN_Alert_Handler( CAN_ALERT_ERR_ACTIVE, alert_req );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Get_Error_Counters( uint32_t* tx_error_cnt, uint32_t* rx_error_cnt )
{
	if ( tx_error_cnt != NULL )
	{
		*tx_error_cnt = CAN.tx_error_counter_reg.byte;
	}
	if ( rx_error_cnt != NULL )
	{
		*rx_error_cnt = CAN.rx_error_counter_reg.byte;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Intr_Handler_err( int32_t* alert_req, uint8_t err_type )
{
	// ECC register is read to re-arm bus error interrupt. ECC is not used
	if ( err_type == BUS_ERROR )
	{
		// ECC register is read to re-arm bus error interrupt. ECC is not used
		( void ) CAN.error_code_capture_reg.val;
		p_can_obj_temp->bus_error_count++;
		CAN_Alert_Handler( CAN_ALERT_BUS_ERROR, alert_req );
	}
	else
	{
		( void ) CAN.arbitration_lost_captue_reg.val;
		p_can_obj_temp->arb_lost_count++;
		CAN_Alert_Handler( CAN_ALERT_ARB_LOST, alert_req );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Set_TX_Buffer_And_Transmit( CAN_FrameBuf_t* frame )
{
	uint8_t command;
	int32_t i;

	// Copy frame structure into TX buffer registers
	for ( i = 0; i < FRAME_MAX_LEN; i++ )
	{
		CAN.tx_rx_buffer[i].val = frame->bytes[i];
	}

	// Set correct transmit command
	if ( frame->self_reception )
	{
		command = ( frame->single_shot ) ? CMD_SELF_RX_SINGLE_SHOT : CMD_SELF_RX_REQ;
	}
	else
	{
		command = ( frame->single_shot ) ? CMD_TX_SINGLE_SHOT : CMD_TX_REQ;
	}
	CAN.command_reg.val = command;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Stop( void )
{
	esp_err_t ret;

	ret = can_stop();
	if ( ret != ESP_OK )
	{
		ESP_LOGI( CAN_TAG, "Module stop failed " );
	}
	else
	{
		ESP_LOGI( CAN_TAG, "Module stop success " );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_CAN::CAN_Check_From_CRIT( bool validation_result, esp_err_t error_type )
{
	if ( !( validation_result ) )
	{
		CAN_EXIT_CRITICAL();
		return ( error_type );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_CAN::CAN_Driver_Uninstall( void )
{
	esp_err_t ret;

	ret = can_driver_uninstall();

	if ( ret != ESP_OK )
	{
		ESP_LOGI( CAN_TAG, " Driver un-installed failed " );
	}
	else
	{
		ESP_LOGI( CAN_TAG, " Driver un-installed successfully " );
	}
	return ( ret );
}
