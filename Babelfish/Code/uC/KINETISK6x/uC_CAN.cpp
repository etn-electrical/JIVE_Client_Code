/**
 *****************************************************************************************
 *	@file uC_CAN.cpp
 *	@details See header file for module overview.
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "includes.h"
#include "Defines.h"
#include "uC_Base.h"
#include "uC_CAN.h"
#include "CAN_Config.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_CAN::uC_CAN( UINT8 CAN_Select, uC_BASE_CAN_IO_PORT_STRUCT const* io_ctrl )
{
	m_config = ( uC_Base::Self() )->Get_CAN_Ctrl( CAN_Select );

	BF_ASSERT( m_config != NULL );
	m_can_reg = m_config->reg_ctrl;

	/* System clocks configuration */
	uC_Base::Reset_Periph( &m_config->periph_def );
	uC_Base::Enable_Periph_Clock( &m_config->periph_def );

	if ( io_ctrl == NULL )
	{
		io_ctrl = m_config->default_port_io;
	}

	/* GPIO configuration : RX pin Pull up input AND Tx pin as Alternate push pull*/
	uC_IO_Config::Enable_Periph_Input_Pin( io_ctrl->rx_pio );
	uC_IO_Config::Enable_Periph_Output_Pin( io_ctrl->tx_pio, TRUE );

	reset_CANError_register();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Set_CAN_Comm_Baud_Rate( CAN_BaudRate_Def CANBaudRate )
{
	/* Mask all required bit time parameters */
	m_can_reg->BTR &= ~( ( ( 0x03 ) << 24 ) | ( ( 0x07 ) << 20 ) | ( ( 0x0F ) << 16 ) | ( 0x1FF ) );

	/* Set the bit timing register */
	m_can_reg->BTR |= ( UINT32 )( ( ( ( ( UINT32 )CANBitTimeParam[CANBaudRate].CAN_SJW - 1 ) & 0x03 ) << 24 )
								  | ( ( ( ( UINT32 )CANBitTimeParam[CANBaudRate].CAN_TSEG2 - 1 ) & 0x07 ) << 20 )
								  | ( ( ( ( UINT32 )CANBitTimeParam[CANBaudRate].CAN_TSEG1 - 1 ) & 0x0F ) << 16 )
								  | ( ( ( UINT32 )CANBitTimeParam[CANBaudRate].CAN_BPR - 1 ) & 0x1FF ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Configure_CAN_Function_State( CAN_Function_Mode CANFunc, FunctionalState funcState )
{
	const UINT16 CANfuncTriggerArr[CAN_MAX_FUNC] =
	{
		CAN_MCR_TTCM,
		CAN_MCR_ABOM,
		CAN_MCR_AWUM,
		CAN_MCR_NART,
		CAN_MCR_RFLM,
		CAN_MCR_TXFP
	};

	if ( funcState == ENABLE )
	{
		m_can_reg->MCR |= CANfuncTriggerArr[CANFunc];
	}
	else
	{
		m_can_reg->MCR &= ~CANfuncTriggerArr[CANFunc];
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Initialize_CAN_Bus( void )
{
	/* Exit CAN bus from sleep mode */
	m_can_reg->MCR &= ~CAN_MCR_SLEEP;

	/* Request CAN Bus Initialisation */
	m_can_reg->MCR |= CAN_MCR_INRQ;

	/* Wait the acknowledge by CAN HW */
	while ( ( m_can_reg->MSR & CAN_MSR_INAK ) != CAN_MSR_INAK )
	{}

	/* Enable Bus Off Recovery automatically by Hardware */
	m_can_reg->MCR |= CAN_MCR_ABOM;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Enable_CAN( void )
{
	/* Enable Bus Off Recovery automatically by Hardware */
	m_can_reg->MCR |= CAN_MCR_ABOM;

	/* Request leave initialisation and jump into Normal mode*/
	m_can_reg->MCR &= ~CAN_MCR_INRQ;

	/* Wait the acknowledge by CAN HW */
	while ( ( m_can_reg->MSR & CAN_MSR_INAK ) == CAN_MSR_INAK )
	{}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::InitMailboxRegisters( UINT8 CAN_Filter_Index, UINT32 mesg_id, UINT8 format, UINT8 isRTR )
{
	UINT32 CAN_Message_ID1 = 0;
	UINT32 filter_number_bit_pos = 0;

	if ( CAN_Filter_Index >= 14 )
	{												// check if Filter Memory is full
		return;
	}

	// Setup identifier information
	if ( format == STANDARD_FORMAT )
	{
		// Standard ID
		CAN_Message_ID1 |= ( UINT32 )( mesg_id << 21 ) | CAN_ID_STD;
	}
	else
	{
		// Extended ID
		CAN_Message_ID1 |= ( UINT32 )( mesg_id << 3 ) | CAN_ID_EXT;
	}

	// Setup identifier information
	if ( isRTR == REMOTE_FRAME )
	{
		// Standard ID
		CAN_Message_ID1 |= ( UINT32 )( mesg_id << 21 ) | CAN_ID_STD | CAN_RTR_REMOTE;
	}

	filter_number_bit_pos = ( ( uint32_t )0x00000001 ) << CAN_Filter_Index;

	m_can_reg->FMR |= CAN_FMR_FINIT;

	/* set Initialisation mode for filter banks */
	m_can_reg->FA1R &= ~( UINT32 )filter_number_bit_pos;						// deactivate filter
																				// initialize filter
	// m_can_reg->FS1R &= ~(UINT32)filter_number_bit_pos;                         // set 16-bit scale configuration
	m_can_reg->FS1R |= filter_number_bit_pos;									// set 32-bit scale configuration

	m_can_reg->FM1R |= ( UINT32 )filter_number_bit_pos;							// set 2 32-bit identifier list mode
	// m_can_reg->FM1R &= ~(UINT32)filter_number_bit_pos;                         // set 2 32-bit identifier/mask mode

	/* 32-bit identifier or First 32-bit identifier */
	m_can_reg->sFilterRegister[CAN_Filter_Index].FR1 = ( UINT32 )CAN_Message_ID1;			// 32-bit identifier

	/* 32-bit mask or Second 32-bit identifier */
	m_can_reg->sFilterRegister[CAN_Filter_Index].FR2 = ( UINT32 )CAN_Message_ID1;	// CAN_Message_ID2  //  32-bit
																					// identifier

	m_can_reg->FFA1R &= ~( UINT32 )filter_number_bit_pos;						// assign filter to FIFO 0
	m_can_reg->FA1R |= filter_number_bit_pos;									// activate filter

	m_can_reg->FMR &= ~CAN_FMR_FINIT;											// reset Initialisation mode for filter
																				// banks
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Configure_CAN_filter( CAN_FilterConfTypeDef* CAN_FilterInitStruct )
{
	uint32_t filter_number_bit_pos = 0;

	filter_number_bit_pos = ( ( uint32_t )0x00000001 ) << CAN_FilterInitStruct->FilterNumber;

	/* Initialisation mode for the filter */
	m_can_reg->FMR |= CAN_FMR_FINIT;

	/* Filter Deactivation */
	m_can_reg->FA1R &= ~( uint32_t )filter_number_bit_pos;

	/* Filter Scale */
	if ( CAN_FilterInitStruct->FilterScale == CAN_FILTERSCALE_16BIT )
	{
		/* 16-bit scale for the filter */
		m_can_reg->FS1R &= ~( uint32_t )filter_number_bit_pos;

		/* First 16-bit identifier and First 16-bit mask
		   Or First 16-bit identifier and Second 16-bit identifier*/
		m_can_reg->sFilterRegister[CAN_FilterInitStruct->FilterNumber].FR1 =
			( ( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterMaskIdLow ) << 16 ) |
			( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterIdLow );

		/* Second 16-bit identifier and Second 16-bit mask
		   Or Third 16-bit identifier and Fourth 16-bit identifier*/
		m_can_reg->sFilterRegister[CAN_FilterInitStruct->FilterNumber].FR2 =
			( ( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterMaskIdHigh ) << 16 ) |
			( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterIdHigh );
	}

	if ( CAN_FilterInitStruct->FilterScale == CAN_FILTERSCALE_32BIT )
	{
		/* 32-bit scale for the filter */
		m_can_reg->FS1R |= filter_number_bit_pos;
		/* 32-bit identifier or First 32-bit identifier */
		m_can_reg->sFilterRegister[CAN_FilterInitStruct->FilterNumber].FR1 =
			( ( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterIdHigh ) << 16 ) |
			( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterIdLow );
		/* 32-bit mask or Second 32-bit identifier */
		m_can_reg->sFilterRegister[CAN_FilterInitStruct->FilterNumber].FR2 =
			( ( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterMaskIdHigh ) << 16 ) |
			( 0x0000FFFF & ( uint32_t )CAN_FilterInitStruct->FilterMaskIdLow );
	}

	/* Filter Mode */
	if ( CAN_FilterInitStruct->FilterMode == CAN_FILTERMODE_IDMASK )
	{
		/*Id/Mask mode for the filter*/
		m_can_reg->FM1R &= ~( uint32_t )filter_number_bit_pos;
	}
	else/* CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdList */
	{
		/*Identifier list mode for the filter*/
		m_can_reg->FM1R |= ( uint32_t )filter_number_bit_pos;
	}

	/* Filter FIFO assignment */
	if ( CAN_FilterInitStruct->FilterFIFOAssignment == CAN_FILTER_FIFO0 )
	{
		/* FIFO 0 assignation for the filter */
		m_can_reg->FFA1R &= ~( uint32_t )filter_number_bit_pos;
	}

	if ( CAN_FilterInitStruct->FilterFIFOAssignment == CAN_FILTER_FIFO1 )
	{
		/* FIFO 1 assignation for the filter */
		m_can_reg->FFA1R |= ( uint32_t )filter_number_bit_pos;
	}

	/* Filter activation */
	if ( CAN_FilterInitStruct->FilterActivation == ENABLE )
	{
		m_can_reg->FA1R |= filter_number_bit_pos;
	}

	/* Leave the initialisation mode for the filter */
	m_can_reg->FMR &= ~CAN_FMR_FINIT;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::Configure_CAN_Interrupt( UINT32 CAN_IT, FunctionalState NewState )
{
	if ( NewState != DISABLE )
	{
		/* Enable the selected CAN interrupt */
		m_can_reg->IER |= CAN_IT;
	}
	else
	{
		/* Disable the selected CAN interrupt */
		m_can_reg->IER &= ~CAN_IT;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_CAN::CAN_Register_callback( INT_CALLBACK_FUNC* int_handler, UINT8 priority,
									UINT8 mbxNo, UINT8 port_requested )
{
	/* Set interrupt for any Receive FIFO */
	uC_Interrupt::Set_Vector( int_handler, m_config->rx_irq_num, priority );

	/* Set interrupt for any other status change */
	uC_Interrupt::Set_Vector( int_handler, m_config->tx_irq_num, priority );

	/* Enable Interrupt */
	uC_Interrupt::Enable_Int( m_config->rx_irq_num );
}

/**
 * @brief  Returns the number of pending messages.
 * @param  FIFONumber: Receive FIFO number, CAN_FIFO0 or CAN_FIFO1.
 * @retval Nnumber of pending message.
 */
UINT8 uC_CAN::Get_No_Of_Pending_CAN_Mesgs( UINT8 FIFONumber )
{
	UINT8 message_pending = 0;

	if ( FIFONumber == CAN_FIFO0 )
	{
		message_pending = ( UINT8 )( m_can_reg->RF0R & ( uint32_t )CAN_RF0R_FMP0 );
	}
	else if ( FIFONumber == CAN_FIFO1 )
	{
		message_pending = ( UINT8 )( m_can_reg->RF1R & ( uint32_t )CAN_RF1R_FMP1 );
	}
	else
	{
		message_pending = 0;
	}

	return ( message_pending );
}

/**
 * @brief  Initiates the transmission of a message.
 * @param  TxMessage: pointer to a structure which contains CAN Id, CAN
 *   DLC and CAN datas.
 * @retval The number of the mailbox that is used for transmission
 *   or CAN_NO_MB if there is no empty mailbox.
 */
UINT8 uC_CAN::CAN_Data_Transmit( CAN_MessageBuf* TxMessage )
{
	UINT8 transmit_mailbox = 0;

	/* Select one empty transmit mailbox */
	if ( ( m_can_reg->TSR & CAN_TSR_TME0 ) == CAN_TSR_TME0 )
	{
		transmit_mailbox = 0;
	}
	else if ( ( m_can_reg->TSR & CAN_TSR_TME1 ) == CAN_TSR_TME1 )
	{
		transmit_mailbox = 1;
	}
	else if ( ( m_can_reg->TSR & CAN_TSR_TME2 ) == CAN_TSR_TME2 )
	{
		transmit_mailbox = 2;
	}
	else
	{
		transmit_mailbox = 0x04;
	}

	if ( transmit_mailbox != 0x04 )
	{
		/* Set up the Id */
		m_can_reg->sTxMailBox[transmit_mailbox].TIR &= CAN_TI0R_TXRQ;
		if ( TxMessage->isIdExtended == CAN_ID_STD )
		{
			m_can_reg->sTxMailBox[transmit_mailbox].TIR |= ( ( TxMessage->id << 21 ) | TxMessage->frameType );
		}
		else
		{
			m_can_reg->sTxMailBox[transmit_mailbox].TIR |= ( ( TxMessage->id << 3 ) | TxMessage->isIdExtended << 2U |
															 TxMessage->frameType << 1 );
		}

		/* Set up the DLC */
		TxMessage->DLC &= ( uint8_t )0x0000000F;
		m_can_reg->sTxMailBox[transmit_mailbox].TDTR &= ( uint32_t )0xFFFFFFF0;
		m_can_reg->sTxMailBox[transmit_mailbox].TDTR |= TxMessage->DLC;

		/* Set up the data field */
		m_can_reg->sTxMailBox[transmit_mailbox].TDLR = ( ( ( uint32_t )TxMessage->data[3] << 24 ) |
														 ( ( uint32_t )TxMessage->data[2] << 16 ) |
														 ( ( uint32_t )TxMessage->data[1] << 8 ) |
														 ( ( uint32_t )TxMessage->data[0] ) );
		m_can_reg->sTxMailBox[transmit_mailbox].TDHR = ( ( ( uint32_t )TxMessage->data[7] << 24 ) |
														 ( ( uint32_t )TxMessage->data[6] << 16 ) |
														 ( ( uint32_t )TxMessage->data[5] << 8 ) |
														 ( ( uint32_t )TxMessage->data[4] ) );
		/* Request transmission */
		m_can_reg->sTxMailBox[transmit_mailbox].TIR |= CAN_TI0R_TXRQ;
	}
	return ( transmit_mailbox );
}

/**
 * @brief  Receives a message.
 * @param  RxMessage: pointer to a structure receive message which
 *   contains CAN Id, CAN DLC, CAN datas and FMI number.
 * @retval None.
 */
void uC_CAN::CAN_Receive_Data( CAN_MessageBuf* RxMessage )
{
	if ( ( m_can_reg->sFIFOMailBox[0].RIR & CAN_ID_EXT ) == 0 )
	{	/* Standard ID */
		RxMessage->isIdExtended = FALSE;
		RxMessage->id = ( UINT32 )0x000007FF & ( m_can_reg->sFIFOMailBox[0].RIR >> 21 );
	}
	else
	{	/* Extended ID */
		RxMessage->isIdExtended = TRUE;
		RxMessage->id = ( UINT32 )0x1FFFFFFF & ( m_can_reg->sFIFOMailBox[0].RIR >> 3 );
	}
	/* Read type information */
	if ( ( m_can_reg->sFIFOMailBox[0].RIR & CAN_RTR_REMOTE ) == 0 )
	{
		RxMessage->frameType = DATA_FRAME;
	}
	else
	{
		RxMessage->frameType = REMOTE_FRAME;
	}
	/* Read length (number of received bytes) */
	RxMessage->DLC = ( UINT8 )0x0000000F & ( m_can_reg->sFIFOMailBox[0].RDTR );

	/* Read the FMI field */
	RxMessage->filterMatchIndex = ( UINT8 )0xFF & ( m_can_reg->sFIFOMailBox[0].RDTR >> 8 );

	/* Read data bytes */
	RxMessage->data[0] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDLR );
	RxMessage->data[1] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDLR >> 8 );
	RxMessage->data[2] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDLR >> 16 );
	RxMessage->data[3] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDLR >> 24 );

	RxMessage->data[4] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDHR );
	RxMessage->data[5] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDHR >> 8 );
	RxMessage->data[6] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDHR >> 16 );
	RxMessage->data[7] = ( UINT8 )0x000000FF & ( m_can_reg->sFIFOMailBox[0].RDHR >> 24 );

	/* Release the FIFO 0 */
	m_can_reg->RF0R |= CAN_RF0R_RFOM0;

#if 0
	/* Get the Id */
	RxMessage->IDE = ( uint8_t )0x04 & CANx->sFIFOMailBox[FIFONumber].RIR;
	if ( RxMessage->IDE == CAN_ID_STD )
	{
		RxMessage->id = ( uint32_t )0x000007FF & ( CANx->sFIFOMailBox[FIFONumber].RIR >> 21 );
	}
	else
	{
		RxMessage->id = ( uint32_t )0x1FFFFFFF & ( CANx->sFIFOMailBox[FIFONumber].RIR >> 3 );
	}

	RxMessage->RTR = ( uint8_t )0x02 & CANx->sFIFOMailBox[FIFONumber].RIR;
	/* Get the DLC */
	RxMessage->DLC = ( uint8_t )0x0F & CANx->sFIFOMailBox[FIFONumber].RDTR;
	/* Get the FMI */
	RxMessage->FMI = ( uint8_t )0xFF & ( CANx->sFIFOMailBox[FIFONumber].RDTR >> 8 );
	/* Get the data field */
	RxMessage->Data[0] = ( uint8_t )0xFF & CANx->sFIFOMailBox[FIFONumber].RDLR;
	RxMessage->Data[1] = ( uint8_t )0xFF & ( CANx->sFIFOMailBox[FIFONumber].RDLR >> 8 );
	RxMessage->Data[2] = ( uint8_t )0xFF & ( CANx->sFIFOMailBox[FIFONumber].RDLR >> 16 );
	RxMessage->Data[3] = ( uint8_t )0xFF & ( CANx->sFIFOMailBox[FIFONumber].RDLR >> 24 );
	RxMessage->Data[4] = ( uint8_t )0xFF & CANx->sFIFOMailBox[FIFONumber].RDHR;
	RxMessage->Data[5] = ( uint8_t )0xFF & ( CANx->sFIFOMailBox[FIFONumber].RDHR >> 8 );
	RxMessage->Data[6] = ( uint8_t )0xFF & ( CANx->sFIFOMailBox[FIFONumber].RDHR >> 16 );
	RxMessage->Data[7] = ( uint8_t )0xFF & ( CANx->sFIFOMailBox[FIFONumber].RDHR >> 24 );
	/* Release the FIFO */
	CAN_FIFORelease( CANx, FIFONumber );
#endif
}

/**
 * @brief  check a CAN error.
 * @param  None
 * @retval True or False.
 */
UINT8 uC_CAN::check_CANError( void )
{
	UINT8 isCANInError = 0;

	/* Last Error Couter and Bus-Off bits */
	if ( ( m_can_reg->ESR & ( CAN_ESR_LEC | CAN_ESR_EWGF | CAN_ESR_EPVF | CAN_ESR_BOFF ) ) ||
		 ( m_can_reg->RF0R & CAN_RF0R_FOVR0 ) )
	{
		isCANInError = 1;
		reset_CANError_register();
	}
	else
	{
		isCANInError = 0;
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
	/*Reset LEC flags, other errors flags and error counters */
	m_can_reg->ESR = CAN_ESR_LEC;
}

/**
 * @brief  check a CAN Buss off State.
 * @param  None
 * @retval True or False.
 */
UINT8 uC_CAN::check_CANBusOff( void )
{
	UINT8 isCANBusOffStateReached = 0;

	/* Bus-Off bits */
	if ( ( m_can_reg->ESR & CAN_ESR_BOFF ) )
	{
		isCANBusOffStateReached = 1;
	}
	else
	{
		isCANBusOffStateReached = 0;
	}
	return ( isCANBusOffStateReached );
}

/**
 * @brief  check a CAN Error counter reached to Passive State.
 * @param  None
 * @retval True or False.
 */
UINT8 uC_CAN::check_CANErrCntrPassiveState( void )
{
	UINT8 isCANErrCntrPassiveStateReached = 0;

	/* Error cntr Passive state bits */
	if ( ( m_can_reg->ESR & CAN_ESR_EPVF ) )
	{
		isCANErrCntrPassiveStateReached = 1;
	}
	else
	{
		isCANErrCntrPassiveStateReached = 0;
	}
	return ( isCANErrCntrPassiveStateReached );
}

/**
 * @brief  check a CAN Error counter reached to Warn State.
 * @param  None
 * @retval True or False.
 */
UINT8 uC_CAN::check_CANErrCntrWarnState( void )
{
	UINT8 isCANErrCntrWarnStateReached = 0;

	/* Error cntr Warn state bits */
	if ( ( m_can_reg->ESR & CAN_ESR_EWGF ) )
	{
		isCANErrCntrWarnStateReached = 1;
	}
	else
	{
		isCANErrCntrWarnStateReached = 0;
	}
	return ( isCANErrCntrWarnStateReached );
}
