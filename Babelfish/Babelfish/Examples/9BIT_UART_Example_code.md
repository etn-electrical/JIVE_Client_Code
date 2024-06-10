@example  uC_USART_9BIT_DMA
@brief    The sample code to communicate the UART over 9 bit using onchip DMA peripheral and soft DMA.

@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run the sample code successfully.
@n        1. STM32F407 Card is fitted on the board.


@par Pin Configurations 
@details Not applicable

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#include "uC_USART_9BIT_Soft_DMA.h"
#include "uC_USART_9BIT_DMA.h"

/*
    If you want to use onchip hardware DMA peripheral for UART communication, define HARD_DMA.
    It will use uC_UART_9BIT_DMA.cpp file.

    If DMA channels are not available, define SOFT_DMA. It use use a pseudo DMA buffer to store the receive 
    and transmit buffers.

    Note: if DMA channel is available it is recommended to use the hardware DMA peripheral.
*/
//#define SOFT_DMA  1
#define HARD_DMA  1

static void Uart_9Bit_test_Thread_Static( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param );

/*Define Tx Mode*/
#define REMOTE_MODBUS_TX_MODE  							MODBUS_RTU_TX_MODE
#define REMOTE_MODBUS_PARITY_MODE  						MODBUS_PARITY_EVEN
#define REMOTE_MODBUS_BAUD_RATE  						19200	


/*
    There is a test code available for testing the the onboard modbus UART communication in Master/Slave configuration.
    the same UARTs are used to verify the 9 bit UART communiation.
*/
/*Define Slave Port*/
#define MODBUS_SLAVE_USART_PORT                                                 uC_BASE_USART_PORT_6                   
#define MODBUS_SLAVE_TIMER                                                      uC_BASE_TIMER_CTRL_3   

/*Define Master Port*/
#define MODBUS_MASTER_USART_PORT                                                uC_BASE_USART_PORT_3
#define MODBUS_MASTER_TIMER                                                     uC_BASE_TIMER_CTRL_5


const uC_BASE_USART_IO_PORT_STRUCT	uC_BASE_USART_PORT_3_PIO = { &USART3_TX_PIO_PD8 , &USART3_RX_PIO_PD9 };
const uC_BASE_USART_IO_PORT_STRUCT	uC_BASE_USART_PORT_6_PIO = { &USART6_TX_PIO_PC6 , &USART6_RX_PIO_PC7 };


void UART_9BIT_Cback( uC_USART_9BIT_Buff::cback_param_t param,
		uC_USART_9BIT_Buff::cback_status_t status );
void    UART_9BIT_Cback_Main( uC_USART_9BIT_Buff::cback_status_t status );

static const uint32_t MAX_RX_BUFFER_SIZE  = 512U;
static const uint32_t MAX_TX_BUFFER_SIZE  = 512U;
static const uint32_t BAUD_RATE = 19200U;

uint8_t RxDataBuff[MAX_RX_BUFFER_SIZE];
uint8_t TxDataBuff[MAX_TX_BUFFER_SIZE] = {1,0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,8,0};

#if SOFT_DMA == 1
uC_USART_9BIT_Soft_DMA* usart_ctrl_tx;
uC_USART_9BIT_Soft_DMA* usart_ctrl_rx;
#elif HARD_DMA == 1
uC_USART_9BIT_DMA* usart_ctrl_tx;
uC_USART_9BIT_DMA* usart_ctrl_rx;
#endif

uC_USART_9BIT_Buff* usart_ptr_tx;
DCI_Owner* modbus_test4_owner;
DCI_Owner* modbus_test5_owner;

/*
 * brief        Example Code for establishing 9 bit UART communication between 2 PatB boards on STM32F407 processor.
 * note         The below function does the following
 *              1. Initialization of 9bit UART classes
 *              2. Enable first transmisstion on a trigger event when user writes a 5555 value in Test param 4 register.
 * 			       (To be viewed on MODBUS TCP port Default IP address 192.168.1.254 )	
 *              3. Receive the frame and increament second word count by one and then retransmitt the frame.
 *			    4. Same code is to be downloaded on two boards. Only one board will require the one time trigger for first transmission.
 * 			    5. The two boards will continue receive and transmitt cycle. The incremented count is stored in Modbus Test Reg5 register.	
 * 			       You will observe "Test Param5" value continuously incrementing from 0 to 511.
 *			    6. It is also possible to communicate this program with one PatB board and the PC.
 *			    7. Congigure the PCs communication tool in parity mode. Select parity as MARK(1) or SPACE(0).
 * 			    8. Send finite hex data from PC, PatB board will receive the frame and retransmitt the frame with 2nd word incremented by one.
 *			    9. Call Uart_9Bit_Test() function from PROD_SPEC_Target_Main_Init() after Test_Init() function.
 */
void Uart_9Bit_Test();

void Uart_9Bit_Test()
{

	uC_USART_9BIT_Buff::cback_status_t usart_dma_req_bits;
	uint32_t rx_done_time = 10000U;     //MicroSeconds
	uint32_t tx_holdoff_time = 10000U;  //MicroSeconds
    
    modbus_test4_owner = new DCI_Owner(DCI_MODBUS_TEST4_DCID);
    modbus_test5_owner = new DCI_Owner(DCI_MODBUS_TEST5_DCID);

#if SOFT_DMA == 1
	usart_ctrl_tx = new uC_USART_9BIT_Soft_DMA( MODBUS_MASTER_USART_PORT,MODBUS_MASTER_TIMER, 
                                               &uC_BASE_USART_PORT_3_PIO );
	usart_ctrl_rx = new uC_USART_9BIT_Soft_DMA( MODBUS_SLAVE_USART_PORT,MODBUS_SLAVE_TIMER, 
                                               &uC_BASE_USART_PORT_6_PIO );
#elif HARD_DMA == 1
    /*  The uC_USART_9BIT_HW() class will initilize the uC_DMA class such that the DMA will treat the 
        peripheral as a 16 bit device */
	usart_ctrl_tx = new uC_USART_9BIT_DMA( MODBUS_MASTER_USART_PORT,MODBUS_MASTER_TIMER, &uC_BASE_USART_PORT_3_PIO );
	usart_ctrl_rx = new uC_USART_9BIT_DMA( MODBUS_SLAVE_USART_PORT,MODBUS_SLAVE_TIMER, &uC_BASE_USART_PORT_6_PIO );
#endif
	usart_dma_req_bits = 0U;
    
    /*  Configure callback for below events */
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BYTE_TIMEOUT );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::RX_BUFF_FULL );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TX_COMPLETE );
	BF_Lib::Bit::Set( usart_dma_req_bits, uC_USART_Buff::TXRX_HOLDOFF_COMPLETE );
    
	usart_ctrl_tx->Config_Callback( UART_9BIT_Cback, NULL, usart_dma_req_bits,
		rx_done_time, tx_holdoff_time );

    /*  Select baud char size as 9 bit for 9 bit communication */
    /*  Parity must be NONE for 9 bit communication on STM32F407 and 439 processor */
    
	usart_ctrl_tx->Config_Port( BAUD_RATE, uC_USART_9BIT_Buff::CHAR_SIZE_9BIT, uC_USART_9BIT_Buff::PARITY_NONE, 
                               uC_USART_9BIT_Buff::STOP_BIT_1 );
    	usart_ptr_tx = usart_ctrl_tx;
	new CR_Tasker( Uart_9Bit_test_Thread_Static, NULL );
    /*  Allow the uart in receive mode from start. */
    	usart_ptr_tx->Start_RX(RxDataBuff,MAX_RX_BUFFER_SIZE);

}

/*
 * brief          Example Code for establishing 9 bit UART communication between 2 PatB boards on STM32F407 processor.
 * note           The below function does the following
 *                1. Triggers the first transmisstion of Tx_buffer if Test Param 4 value is changed to 5555 from MODBUS TCP.
 */
static void Uart_9Bit_test_Thread_Static( CR_Tasker* cr_task, CR_TASKER_PARAM cr_param )
{
    uint16_t test_param4;


    modbus_test4_owner->Check_Out( test_param4 );

    /*  In case of interboard communication check for the value of ModbusTestReg4 register, if user writes value as 5555, transmit the TxDataBuff */
    if (  test_param4 == 5555 )
    {
        test_param4 = 0;
    /*  Reset the value of ModbusTestReg4 to 0 to avoid continuous retranmission */        
        modbus_test4_owner->Check_In( test_param4 );
        usart_ptr_tx->Start_TX( TxDataBuff, 10 );
    }
}

/*
 * brief        Example Code for establishing 9 bit UART communication between 2 PatB boards on STM32F407 processor.
 * note         The below function does the following
 *              1. This callback function is called when any event related to transmission and reception as specified by usart_dma_req_bits.
 *			    2. Details related to interboard PatB communication and PatB to PC communication is alread covered in the first function
 * 			       descrition.
 */
void UART_9BIT_Cback( uC_USART_9BIT_Buff::cback_param_t param,
		uC_USART_9BIT_Buff::cback_status_t status )
{
    uint16_t i;
    uint16_t ModbusData;
    uint16_t Rx_length;

    /* If UART interrupt is triggered, check if it is receive interrupt */
	if ( ( BF_Lib::Bit::Test( status, static_cast<uint_fast8_t>(uC_USART_9BIT_Buff::RX_BYTE_TIMEOUT) ) ) ||
         ( BF_Lib::Bit::Test( status, static_cast<uint_fast8_t>(uC_USART_9BIT_Buff::RX_BUFF_FULL) ) ) )
    {
    	/* This test program reads second word from the received RxData buffer, store it into ModbusTestReg5 register and then increments it by one. */
        ModbusData = RxDataBuff[3];
        ModbusData = ( ModbusData << 8 ) | RxDataBuff[2];
        
        modbus_test5_owner->Check_In( ModbusData );
        
        if ( ModbusData++ >= MAX_RX_BUFFER_SIZE )
        ModbusData = 0;
        /*The incremented value is also copied into RxDataBuff */
        RxDataBuff[2] = ModbusData;
        RxDataBuff[3] = ModbusData >> 8;
    	Rx_length = usart_ptr_tx->Get_RX_Length();
        /*Modifed Received buffer is copied to TxDataBuff */    	
        for (i = 0 ; i < Rx_length ; i++ )
        {
            TxDataBuff[i] = RxDataBuff[i];
        }
        /*Start transmitting the TxDataBuff. */
        usart_ptr_tx->Start_TX(TxDataBuff,Rx_length);
    }
    /* If UART interrupt is triggered, check if it is transmission complete interrupt */
	if ( BF_Lib::Bit::Test( status, static_cast<uint_fast8_t>(uC_USART_9BIT_Buff::TX_COMPLETE) ) )
    {
        /* Set UART in receive mode */
	usart_ptr_tx->Start_RX(RxDataBuff,MAX_RX_BUFFER_SIZE);
    }
}
~~~~