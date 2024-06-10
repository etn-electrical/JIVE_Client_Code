/*
 *****************************************************************************************
 *		$Workfile: uC_CAN.h
 *
 *		$Author: Sandip Bhagwat
 *		$Date: 11/07/2013
 *		Copyright © Eaton Corporation. All Rights Reserved.
 *		Creator:
 *
 *		Description:
 *
 *
 *		Code Inspection Date:
 *
 *		$Header: uC_CAN.h
 *****************************************************************************************
 */
#ifndef uC_CAN_H
   #define uC_CAN_H

#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_OS_Interrupt.h"
#include "stm32h7xx_hal_can.h"
#include "stm32h7xx_hal_rcc.h"
/*
 *****************************************************************************************
 *	Variables and Constants
 *****************************************************************************************
 */
#define STANDARD_FORMAT  0
#define EXTENDED_FORMAT  1
#define DATA_FRAME       0
#define REMOTE_FRAME     1
#define CAN_TDTxR_DLC         ( ( unsigned long )0x0000000F )

#define RCC_APB2Periph_GPIO_CAN     RCC_APB2Periph_GPIOB
#define GPIO_Remap_CAN              GPIO_Remap1_CAN1
#define GPIO_CAN                    GPIOB
#define GPIO_Pin_CAN_RX             GPIO_Pin_8
#define GPIO_Pin_CAN_TX             GPIO_Pin_9
#define MAX_CAN_DATA_LENGTH         8

typedef enum
{
	CAN_BAUD_RATE_1000KBPS,
	CAN_BAUD_RATE_800KBPS,
	CAN_BAUD_RATE_500KBPS,
	CAN_BAUD_RATE_250KBPS,
	CAN_BAUD_RATE_125KBPS,
	CAN_BAUD_RATE_100KBPS,
	CAN_BAUD_RATE_50KBPS,
	CAN_BAUD_RATE_20KBPS,
	CAN_BAUD_RATE_10KBPS,
	CAN_MAX_NO_BAUD_RATE
} CAN_BaudRate_Def;


typedef struct CAN_Bit_Time_Param
{
	UINT16 CAN_BPR;		// Specifies the length of a time quantum. It ranges from 1 to 1024. */
	UINT8 CAN_TSEG1;	// Specifies the number of time quanta in Bit Segment 1.
	UINT8 CAN_TSEG2;	// Specifies the number of time quanta in Bit Segment 2.
	UINT8 CAN_SJW;		// Specifies the maximum number of time quanta the CAN hardware
	// is allowed to lengthen or shorten a bit to perform resynchronization.
} CAN_Bit_Time_Param;

typedef struct
{
	UINT32 id;					// 29 bit identifier
	bool isIdExtended;			// 0- Standard, 1- Extended
	UINT8 frameType;			// 0- DATA FRAME, 1- REMOTE FRAME
	UINT8 DLC;					// data length byte
	UINT8 commMode;				// 0-Receive, 1- transmit...
	UINT8 filterMatchIndex;		// Specifies the index of the filter the message stored in the mailbox passes through
	UINT8 data[8];				// data field array
} CAN_MessageBuf;

typedef enum
{
	CAN_TTCM,	/* Time triggered communication mode.*/
	CAN_ABOM,	/* Automatic bus-off management.*/
	CAN_AWUM,	/* Automatic wake-up mode. */
	CAN_NART,	/* no-automatic retransmission mode.*/
	CAN_RFLM,	/* Receive FIFO Locked mode.*/
	CAN_TXFP,	/* Transmit FIFO priority.*/
	CAN_MAX_FUNC
} CAN_Function_Mode;

/*
 **************************************************************************************************
 *	 Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_CAN
{
	public:
		uC_CAN( UINT8 CAN_Select, uC_BASE_CAN_IO_PORT_STRUCT const* io_ctrl );
		~uC_CAN();

		/* Enable or Disable requeste CAN functional mode */
		void Configure_CAN_Function_State( CAN_Function_Mode CANFunc, FunctionalState funcState );

		void Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def CANBaudRate );

		void CAN_Register_callback( INT_CALLBACK_FUNC* int_handler,
									UINT8 priority, UINT8 mbxNo = 0,
									UINT8 port_requested = uC_BASE_CAN_CTRL_1 );

		void Initialize_CAN_Bus( void );

		void Enable_CAN( void );

		void Configure_CAN_filter( CAN_FilterConfTypeDef* CAN_FilterInitStruct );

		void Configure_CAN_Interrupt( UINT32 CAN_IT, FunctionalState NewState );

		UINT8 CAN_Data_Transmit( CAN_MessageBuf* TxMessage );

		void CAN_Receive_Data( CAN_MessageBuf* RxMessage );

		UINT8 Get_No_Of_Pending_CAN_Mesgs( UINT8 FIFONumber );

		UINT8 check_CANError( void );

		void reset_CANError_register( void );

		UINT8 check_CANBusOff( void );

		UINT8 check_CANErrCntrPassiveState( void );

		UINT8 check_CANErrCntrWarnState( void );

		void InitMailboxRegisters( UINT8 CAN_Filter_Index, UINT32 mesg_id, UINT8 format, UINT8 isRTR );

	/*
	   void CAN_Init(void);
	   void Poll_For_CAN_Rx_Tx(void);
	   uint8_t	getCANdata(void);
	 */

	private:

		/*   void RCC_Configuration(void);
		   void GPIO_Configuration(void);
		   void CAN_Config(void);
		   void Init_RxMes(CanRxMsg *RxMessage);
		   void Delay(void);

		   CAN_InitTypeDef         CAN_InitStructure;
		   CAN_FilterConfTypeDef   CAN_FilterInitStructure;
		   RCC_ClkInitTypeDef       RCC_Clocks;
		   CanTxMsg                TxMessage;
		   uint8_t	                CANData[MAX_CAN_DATA_LENGTH];
		 */
		void Configure_CAN_Communication( void );

		uC_BASE_CAN_IO_STRUCT const* m_config;
		// static const CAN_Bit_Time_Param CANBitTimeParam[CAN_MAX_NO_BAUD_RATE];
		CAN_TypeDef* m_can_reg;

};

#endif