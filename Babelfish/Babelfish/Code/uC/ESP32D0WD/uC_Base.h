/**
 *****************************************************************************************
 *	@file		uC_Base.cpp
 *
 *	@brief		uC_bace class for interfacing common defines and functions of HAL layer
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_BASE_H
	#define uC_BASE_H

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/timer.h"
#include "driver/uart.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_intr_alloc.h"
#include "esp_intr_alloc.h"
#include "soc/uhci_struct.h"
#include "soc/uhci_reg.h"
#include "soc/periph_defs.h"
#include "soc/dport_reg.h"
#include "soc/dport_access.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "freertos/ringbuf.h"

#include "uC_IO_Define_ESP32.h"
#include "uC_Base_HW_IDs_ESP32.h"
#include "driver/adc.h"
#include "driver/i2c.h"
#include "driver/twai.h"


/*
 *****************************************************************************************
 *	Constant Variables
 *****************************************************************************************
 */
static const uint16_t RINGBUF_SIZE = 2048U;
static const uint16_t RX_BUF_SIZE = 2048U;
static const uint32_t PERIPH_DEFAULT_MASK = 0U;
static const uint8_t MAX_GPIO_OUTPUT = 34;

/**
 **********************************************
 * @brief   UHCI numbers,Total 2 UHCI numbers.
 **********************************************
 */
enum uhci_num_t
{
	UHCI_NUM_0 = 0x0,
	UHCI_NUM_1 = 0x1,
	UHCI_NUM_MAX,
};

/**
 ***************************************
 * @brief   UHCI links output and input
 ***************************************
 */
enum uhci_link_type_t
{
	UHCI_LINK_IN = 0x0,
	UHCI_LINK_OUT = 0x1,
	UHCI_LINK_MAX,
};

/**
 ***************************************
 * @brief   UHCI interrupts type
 ***************************************
 */
enum uhci_intr_type_t
{
	UHCI_INTR_DIS = ( 0x0 ),				///< disable all uhci related interrupts
	UHCI_RX_START = ( 0X1 << 0 ),			///< rx_start interrupt mask
	UHCI_TX_START = ( 0X1 << 1 ),			///< tx_start interrupt mask
	UHCI_RX_HUNG = ( 0X1 << 2 ),			///< rx_hung interrupt mask
	UHCI_TX_HUNG = ( 0X1 << 3 ),			///< tx_hung interrupt mask
	UHCI_IN_DONE = ( 0X1 << 4 ),			///< in description link completed
	UHCI_IN_SUC_EOF = ( 0X1 << 5 ),
	UHCI_IN_ERR_EOF = ( 0X1 << 6 ),
	UHCI_OUT_DONE = ( 0X1 << 7 ),
	UHCI_OUT_EOF = ( 0X1 << 8 ),
	UHCI_IN_DSCR_ERR = ( 0X1 << 9 ),
	UHCI_OUT_DSCR_ERR = ( 0X1 << 10 ),
	UHCI_IN_DSCR_EMPTY = ( 0X1 << 11 ),
	UHCI_OUTLINK_EOF_ERR = ( 0X1 << 12 ),
	UHCI_OUT_TOTAL_EOF = ( 0X1 << 13 ),
	UHCI_SEND_S_REG_Q = ( 0X1 << 14 ),
	UHCI_SEND_A_REG_Q = ( 0X1 << 15 ),
	UHCI_DMA_IN_FIFO_FULL = ( 0X1 << 16 ),
	UHCI_INTR_MAX,
};

/**
 ***************************************
 * @brief   UHCI DMA link structure
 ***************************************
 */
struct uhci_dma_link_t
{
	struct
	{
		uint32_t size :     12;				///< the size of buf, must be able to be divisible by 4 in link
		uint32_t length :   12;				///< in link length
		uint32_t reversed : 6;				///< reversed
		uint32_t eof :      1;				///< if this dma link is the last one, you should set this bit 1.
		uint32_t owner :    1;				///< the owner of buf, bit 1 : DMA, bit 0 : CPU.
	} des;
	uint8_t* buf;							///< the pointer of buf
	struct uhci_dma_link* pnext;			///< point to the next dma link, if this link is the last one, set it NULL.
};

typedef intr_handle_t uhci_isr_handle_t;

/**
 ***************************************
 * @brief   UHCI DMA Event types
 ***************************************
 */
enum uhci_event_type_t
{
	UNDEFINED = 0,							///< Undefined event type
	RX_START,								///< Rx start event type
	TX_START,								///< Tx start event type
	RX_HUNG,								///< Rx Hang event type
	TX_HUNG,								///< TX Hang event type
	OUT_DONE,								///< Out done event type
	OUT_EOF,								///< Out EOF event type
	IN_DONE,								///< In event type
};

/**
 ***************************************
 * @brief   UHCI Event types
 ***************************************
 */
struct uhci_event_t
{
	uhci_event_type_t event_type;
	uint32_t data_len;
	uint32_t send_sta;

};

/*
 ****************************************
 * @brief	Peripheral UCHI Definition Structure
 ****************************************
 */
struct uC_BASE_UHCI_STRUCT
{
	uhci_num_t uhci_num;					///< UHCI number
	int32_t queue_size;						///< Queue size
	QueueHandle_t xQueueUhci;				///< UHCI Queue
	SemaphoreHandle_t tx_sem;				///< Transmit semaphore
	uhci_isr_handle_t uhci_isr_handle;		///< ISR handle
	RingbufHandle_t rx_ring_buf;			///< Receive Ring buffer
	uint32_t rx_data_len;					///< Data length
	uhci_dma_link_t rx_link;				///< Out description link
	uhci_dma_link_t tx_link;				///< In description link
	uint8_t rx_buf[RX_BUF_SIZE];			///< Receive Buffer
};

/*
 ********************************
 * @brief	USART Definition Structure
 ********************************
 */
struct uC_BASE_USART_IO_PORT_STRUCT
{
	uC_PERIPH_IO_STRUCT const* tx_port;		///< IO configuration of TX port
	uC_PERIPH_IO_STRUCT const* rx_port;		///< IO configuration of RX port
};

/*
 ********************************
 * @brief	USART IO Definition Structure
 ********************************
 */

struct uC_BASE_USART_IO_STRUCT
{
	int8_t usart_num;										///< USART number
	uC_BASE_USART_IO_PORT_STRUCT const* default_port_io;	///< USART RX & TX ports structure
};

/*
 **************************************************
 * @brief PIO Controller Definition Structure
 **************************************************
 */
struct uC_BASE_PIO_CTRL_STRUCT
{
	uint64_t pin_bit_mask;					///< GPIO pin: set with bit mask, each bit maps to a GPIO.
	gpio_mode_t mode;						///< GPIO mode: set input/output mode.
	gpio_pullup_t pull_up_en;				///< GPIO pull-up.
	gpio_pulldown_t pull_down_en;			///< GPIO pull-down.
	GPIO_INT_TYPE intr_type;				///< GPIO interrupt type.
};

/*
 **************************************************
 * @brief Decides whether timer is on or paused
 **************************************************
 */
struct uC_BASE_TIMER_IO_STRUCT
{
	bool alarm_en;							///< Timer alarm enable
	bool counter_en;						///< Counter enable
	timer_intr_mode_t intr_type;			///< Interrupt mode
	timer_count_dir_t counter_dir;			///< Counter direction
	timer_autoreload_t auto_reload;			///< Timer auto-reload
	uint32_t divider;						///< Counter clock divider. The divider's range is from from 2 to 65536.
	timer_group_t group_num;				///< Selects a Timer-Group out of 2 available groups
	int32_t timer_idx;						///< Select a hardware timer from timer groups
	uint64_t load_val;						///< Load value
	uint64_t alarm_value;					///< Alarm Value
};

/**
 ****************************************************************************************
 * @brief ESP32 Interrupt Definition Structure
 *
 ****************************************************************************************
 */
struct interrupt_config_t
{
	int32_t priority;
	int32_t source;
	uint32_t intrstatusreg;
	uint32_t intrstatusmask;
	intr_handler_t handler;
	void* arg;
	timer_isr_handle_t* ret_handle;
};

/**
 * @brief UART configuration parameters for uart_param_config function
 */
struct uC_BASE_UART_CONFIG_STRUCT
{
	uart_port_t uart_num;					///< UART Number
	int32_t baud_rate;						///< UART baud rate
	uart_word_length_t data_bits;			///< UART byte size
	uart_parity_t parity;					///< UART parity mode
	uart_stop_bits_t stop_bits;				///< UART stop bits
	uart_hw_flowcontrol_t flow_ctrl;		///< UART HW flow control mode (cts/rts)
	uint8_t rx_flow_ctrl_thresh;			///< UART HW RTS threshold
	bool use_ref_tick;						///< Set to true if UART should be clocked from REF_TICK
};

/**
 ****************************************************************************************
 * @brief ESP32 AtoD Control Definition Structure
 *
 ****************************************************************************************
 */
struct uC_BASE_ATOD_IO_STRUCT
{
	uC_BASE_ATOD_CTRL_ENUM adc_block;
	uint8_t max_channels;
	adc_atten_t atten;
	uint32_t ref_voltage;
};

/**
 ****************************************************************************************
 * @brief ESP32 I2C command Structure
 *
 ****************************************************************************************
 */
struct i2c_cmd_t
{
	uint8_t byte_num;		///< cmd byte number
	uint8_t ack_en;			///< ack check enable
	uint8_t ack_exp;		///< expected ack level to get
	uint8_t ack_val;		///< ack value to send
	uint8_t* data;			///< data address
	uint8_t byte_cmd;		///< to save cmd for one byte command mode
	i2c_opmode_t op_code;	///< haredware cmd type
};

/**
 ****************************************************************************************
 * @brief I2C IO configuration  structure
 *
 ****************************************************************************************
 */
struct uC_BASE_I2C_IO_STRUCT
{
	i2c_mode_t mode;				///< I2C mode
	gpio_num_t sda_io_num;			///< GPIO number for I2C sda signal
	gpio_pullup_t sda_pullup_en;	///< Internal GPIO pull mode for I2C sda signal
	gpio_num_t scl_io_num;			///< GPIO number for I2C scl signal
	gpio_pullup_t scl_pullup_en;	///< Internal GPIO pull mode for I2C scl signal
	uint32_t clk_speed;				///< I2C clock frequency for master mode, (no higher than 1MHz for now)
};

/**
 ****************************************************************************************
 * @brief I2C Control Definition Structure
 *
 ****************************************************************************************
 */
struct uC_BASE_I2C_IO_PORT_STRUCT
{
	const uC_PERIPH_IO_STRUCT* scl_port;
	const uC_PERIPH_IO_STRUCT* sda_port;
};

/**
 ****************************************************************************************
 * @brief ESP32 I2C command link structure
 *
 ****************************************************************************************
 */
struct i2c_cmd_link_t
{
	i2c_cmd_t cmd;					///< command in current cmd link
	struct i2c_cmd_link_t* next;	///< next cmd link
};

/**
 ****************************************************************************************
 * @brief ESP32 I2C command structure
 *
 ****************************************************************************************
 */
struct i2c_cmd_desc_t
{
	i2c_cmd_link_t* head;		///< head of the command link
	i2c_cmd_link_t* cur;		///< last node of the command link
	i2c_cmd_link_t* free;		///< the first node to free of the command link
};

/**
 ****************************************************************************************
 * @brief ESP32 I2C object structure
 *
 ****************************************************************************************
 */
struct i2c_obj_t
{
	int32_t i2c_num;					///< I2C port number
	int32_t mode;						///< I2C mode, master or slave
	intr_handle_t intr_handle;			///< I2C interrupt handle
	int32_t cmd_idx;					///< record current command index, for master mode
	int32_t status;						///< record current command status, for master mode
	int32_t rx_cnt;						///< record current read index, for master mode
	uint8_t data_buf[SOC_I2C_FIFO_LEN];		///< a buffer to store i2c fifo data
	i2c_cmd_desc_t cmd_link;			///< I2C command link
	QueueHandle_t cmd_evt_queue;		///< I2C command event queue
#if CONFIG_SPIRAM_USE_MALLOC
	uint8_t* evt_queue_storage;			///< The buffer that will hold the items in the queue
	int32_t intr_alloc_flags;			///< Used to allocate the interrupt
	StaticQueue_t evt_queue_buffer;		///< The buffer that will hold the queue structure
#endif
	xSemaphoreHandle cmd_mux;			///< semaphore to lock command process
	size_t tx_fifo_remain;				///< tx fifo remain length, for master mode
	size_t rx_fifo_remain;				///< rx fifo remain length, for master mode

	xSemaphoreHandle slv_rx_mux;		///< slave rx buffer mux
	xSemaphoreHandle slv_tx_mux;		///< slave tx buffer mux
	size_t rx_buf_length;				///< rx buffer length
	RingbufHandle_t rx_ring_buf;		///< rx ringbuffer handler of slave mode
	size_t tx_buf_length;				///< tx buffer length
	RingbufHandle_t tx_ring_buf;		///< tx ringbuffer handler of slave mode
};

/**
 ****************************************************************************************
 * @brief CAN IO defination structure
 *
 ****************************************************************************************
 */
struct uC_BASE_CAN_IO_PORT_STRUCT
{
	uC_PERIPH_IO_STRUCT tx_pio;			///< TX IO peripheral structure
	uC_PERIPH_IO_STRUCT rx_pio;			///< RX IO peripheral structure
};

/**
 ****************************************************************************************
 * @brief CAN Function mode enum
 *
 ****************************************************************************************
 */
enum CAN_Function_Mode
{
	CAN_NORMAL,					///< Normal operating mode where CAN controller can send/receive/acknowledge messages
	CAN_NO_ACK,					///< Transmission does not require acknowledgment. Use this mode for self testing
	CAN_LISTEN_ONLY				///< The CAN controller will not influence the bus (No transmissions or acknowledgments)
};

/**
 ****************************************************************************************
 * @brief CAN Functional baud rate details
 *
 ****************************************************************************************
 */
enum CAN_BaudRate_Def
{
	CAN_BAUD_RATE_1000KBPS,		///< 1MBPS baud rate selection
	CAN_BAUD_RATE_800KBPS,		///< 800KBPS baud rate selection
	CAN_BAUD_RATE_500KBPS,		///< 500KBPS baud rate selection
	CAN_BAUD_RATE_250KBPS,		///< 250KBPS baud rate selection
	CAN_BAUD_RATE_125KBPS,		///< 125KBPS baud rate selection
	CAN_BAUD_RATE_100KBPS,		///< 100KBPS baud rate selection
	CAN_BAUD_RATE_50KBPS,		///< 50KBPS baud rate selection
	CAN_BAUD_RATE_25KBPS,		///< 25KBPS baud rate selection
	CAN_MAX_NO_BAUD_RATE
};

/**
 ****************************************************************************************
 * @brief ESP32 CAN Filter configuration structure
 *
 ****************************************************************************************
 */
struct CAN_Filter_Config_t
{
	uint32_t acceptance_code;			///< 32-bit acceptance code
	uint32_t acceptance_mask;			///< 32-bit acceptance mask
	bool single_filter;					///< Use Single Filter Mode (see documentation)
};

/**
 ****************************************************************************************
 * @brief CAN Base IO structure
 *
 ****************************************************************************************
 */
struct uC_BASE_CAN_IO_STRUCT
{
	CAN_Function_Mode mode;				///< Mode of CAN controller
	uint32_t tx_queue_len;				///< Number of messages TX queue can hold (set to 0 to disable TX Queue)
	uint32_t rx_queue_len;				///< Number of messages RX queue can hold
	uint32_t alerts_enabled;			///< Bit field of alerts to enable (see documentation)
	uint32_t clkout_divider;			///< CLKOUT divider. Can be 1 or any even number from 2 to 14
	CAN_BaudRate_Def baud_Rate;			///< Baud rate details
	CAN_Filter_Config_t filter_config;	///< Filter configuration structure
	uC_BASE_CAN_IO_PORT_STRUCT const default_port_io;	///< Default PORT IO structure(TX and RX)
	uC_BASE_CAN_IO_PORT_STRUCT const clock_port;		///< Clock and Bus state PORT IO structure
};

/**
 ****************************************************************************************
 * @brief Control structure for CAN driver
 *
 ****************************************************************************************
 */
struct can_obj_t
{
	uint32_t control_flags;				///< Control flag
	uint32_t rx_missed_count;			///< Receive missed count
	uint32_t tx_failed_count;			///< Transmit fail counter
	uint32_t arb_lost_count;			///< Arbitration loss counter
	uint32_t bus_error_count;			///< Bus error counter
	intr_handle_t isr_handle;			///< ISR pointer
	QueueHandle_t tx_queue;				///< Transmit queue
	QueueHandle_t rx_queue;				///< Receive Queue
	int32_t tx_msg_count;				///< Transmit message count
	int32_t rx_msg_count;				///< Receive message count
	SemaphoreHandle_t alert_semphr;		///< Alert handler
	uint32_t alerts_enabled;			///< Alert enabling or disabling detail
	uint32_t alerts_triggered;			///< Alter triggering details
#ifdef CONFIG_PM_ENABLE
	// Power Management
	esp_pm_lock_handle_t pm_lock;
#endif
};

/**
 ****************************************************************************************
 * @brief CAN filter configuration structure
 *
 ****************************************************************************************
 */
struct CAN_FilterConfTypeDef
{
	CAN_Filter_Config_t filter_config;	///< ESP32 filter configuration structure
};

/**
 ****************************************************************************************
 * @brief CAN Functional states details
 *
 ****************************************************************************************
 */
enum FunctionalState
{
	DISABLE = 0,
	ENABLE = !DISABLE
};


/*
 *************************************************************************************
 *  Peripherals  Definition Structure
 *************************************************************************************
 */
struct PERIPH_DEF_ST
{
	uint8_t pclock_reg_index;
	uint32_t pid;
};

/*
 *************************************************************************************
 *  Watchdog Controller Definition Structure
 *************************************************************************************
 */
struct uC_BASE_WDOG_CTRL_STRUCT
{
	PERIPH_DEF_ST periph;
};

/*
 *************************************************************************************
 *  PWM Definition Structure
 *************************************************************************************
 */
struct uC_BASE_PWM_STRUCT
{
	uint8_t pwm_ctrl_block;
	uint8_t pwm_channel;
	uint8_t pwm_timer_no;
	uint8_t pwm_op;
	uC_PERIPH_IO_STRUCT const* pwm_pio[6];
};

/**
 **********************************************************************************************
 * @brief		uC_Base Class Declaration.
 **********************************************************************************************
 */

class uC_Base
{
	protected:
		/**
		 * @brief	Constructs an instance of the uC_Base class.
		 * @return Void						None
		 */
		uC_Base( void );

		/**
		 * @brief	Destructor (for MISRA purposes only).
		 * @return Void						None
		 */
		~uC_Base( void ) {}

	public:

		/**
		 * @brief	variable to store the object pointer
		 */
		static uC_Base* m_myself;

		/**
		 * @brief		Function to return the pointer of the object
		 * @return uC_Base					uC_Base class object
		 */
		static uC_Base* Self( void )
		{
			return ( m_myself );
		}

		/**
		 * @brief									Function returns GPIO configuration structure
		 * @param[in] io_block						Input output block number
		 * @return uC_BASE_PIO_CTRL_STRUCT			Base GPIO structure
		 */
		uC_BASE_PIO_CTRL_STRUCT const* Get_PIO_Ctrl( uint32_t io_block );

		/**
		 * @brief GPIO constant used for the uC_Base
		 */
		static const uC_BASE_PIO_CTRL_STRUCT m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS];

		/**
		 * @brief									Function returns Timer configuration structure
		 * @param[in] timer							Timer number
		 * @return uC_BASE_PIO_CTRL_STRUCT			Base Timer IO structure
		 */
		uC_BASE_TIMER_IO_STRUCT const* Get_Timer_Ctrl( uint32_t timer );

		/**
		 * @brief Timer constant used for the uC_Base
		 */
		static const uC_BASE_TIMER_IO_STRUCT m_timer_io[uC_BASE_MAX_TIMER_CTRLS];

		/**
		 * @brief							Function to Enable Peripheral module
		 * @param[in] periph				Peripheral Module number to enable
		 * @return esp_err_t				esp error type
		 */
		static esp_err_t Periph_Module_Enable( periph_module_t periph );

		/**
		 * @brief							Function to Get clock mask
		 * @param[in] periph				Peripheral module number to enable
		 * @return uint32_t					Clock enable mask
		 */
		static uint32_t Get_Clk_En_Mask( periph_module_t periph );

		/**
		 * @brief							Function to Get reset mask
		 * @param[in] periph				Peripheral module number to enable
		 * @return uint32_t					Clock enable mask
		 */
		static uint32_t Get_Rst_En_Mask( periph_module_t periph );

		/**
		 * @brief							Function to Get clock enable register
		 * @param[in] periph				Peripheral module number to enable
		 * @return uint32_t					Clock enable register
		 */
		static uint32_t Get_Clk_En_Reg( periph_module_t periph );

		/**
		 * @brief							Function to reset register
		 * @param[in] periph				Peripheral module number to enable
		 * @return uint32_t					Clock enable register value
		 */
		static uint32_t Get_Rst_En_Reg( periph_module_t periph );

		/**
		 * @brief A2D IO structure used for the uC_Base
		 */
		static const uC_BASE_ATOD_IO_STRUCT m_atod_io[uC_BASE_ATOD_MAX_CTRLS];

		/**
		 * @brief I2C IO structure used for the uC_Base
		 */
		static const uC_BASE_I2C_IO_STRUCT m_i2c_io[uC_BASE_I2C_MAX_CTRLS];

		/**
		 * @brief CAN IO structure used for the uC_Base
		 */
		static const uC_BASE_CAN_IO_STRUCT m_can_io[uC_BASE_CAN_MAX_CTRLS];

		/**
		 * @brief PWM structure used for the uC_Base
		 */
		static uC_BASE_PWM_STRUCT m_pwm_ctrls[uC_BASE_PWM_MAX_CTRLS];

		/**
		 * @brief Use adc2_vref_to_gpio() to obtain a better estimate
		 */
		static const uint16_t DEFAULT_VREF = 1100U;
};

#endif
