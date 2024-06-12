/**
 * This an example application that demonstrates Bluetooth connectivity
 * using BGLIB C function definitions. The example enables Bluetooth advertisements
 * and connections.
 *
 * Most of the functionality in BGAPI uses a request-response-event pattern
 * where the module responds to a command with a command response indicating
 * it has processed the request and then later sending an event indicating
 * the requested operation has been completed. */

/* BG stack headers */
#include "gecko_bglib.h"
#include "bg_types.h"

/* hardware specific headers */
#include "uart.h"
#include "Ble_UART.h"
#include "Ble_DCI.h"
#include "BLE_Debug.h"

/***************************************************************************************************
 * Local Macros and Definitions
 **************************************************************************************************/
BGLIB_DEFINE();
const int32_t BLE_DEFAULT_PASSKEY = 11111U;
const uint16_t BLE_MIN_VAL_CONN_INTERVAL = 16U;
const uint16_t BLE_MAX_VAL_CONN_INTERVAL = 32U;
const uint16_t BLE_SLAVE_LATENCY = 4U;
const uint16_t BLE_SUPERVISION_TIMEOUT = 200U;

/***************************************************************************************************
 Local Variables
 **************************************************************************************************/
static Ble_UART* ble_uart_port;
static Ble_DCI* ble_dci_ref;

/***************************************************************************************************
 Global Variables
 **************************************************************************************************/
int32_t DCID_passkey;
uint16_t DCID_min_conn_interval,DCID_max_conn_interval,DCID_slave_latency,
				DCID_supervision_timeout;
extern Ble_UART* Get_Ble_Serial_Obj(void);
extern Ble_DCI* Get_Ble_Dci_Obj(void);

/***************************************************************************************************
 * Static Function Declarations
 **************************************************************************************************/
static void on_message_send(uint32_t msg_len, uint8_t* msg_data);

/***************************************************************************************************
 * Public Function Declarations
 **************************************************************************************************/
extern void appHandleEvents(struct gecko_cmd_packet *evt);
void sampleapp(void);
void bglib_init(void);
void get_passkey(void);
void get_connection_parameters(void);

/***************************************************************************************************
 * Public Function Definitions
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  The bglib init function.
 *  \param[in] None.
 *  \return  None
 **************************************************************************************************/
void bglib_init(void)
{
	/* Initialize BGLIB with our output function for sending messages. */
	BGLIB_INITIALIZE_NONBLOCK(on_message_send, uartRx, uartRxPeek);
	BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Starting up...\nResetting NCP target...\n");
	/* Reset NCP to ensure it gets into a defined state.
	 * Once the chip successfully boots, gecko_evt_system_boot_id event should be received. */
	gecko_cmd_system_reset(0);

	/* Read the Default passkey from the DCID workbook*/
	get_passkey();
	
	/* Read the Default connection parameters from the DCID workbook*/
	get_connection_parameters();
}

/***********************************************************************************************//**
 *  \brief  Get connection parameters function is to retrive the default connection parameter
	 from the DCI Workbook.
 *  \param[in] None.
 *  \return  None
 **************************************************************************************************/
void get_connection_parameters(void)
{
	DCI_ERROR_TD status;
	uint16_t length;

    /* Get the Ble Dci Object */
    ble_dci_ref = Get_Ble_Dci_Obj();
    
	status = ble_dci_ref->Get_Dcid_Data(DCI_BLE_MIN_VAL_CONN_INTERVAL_DCID,
	reinterpret_cast<DCI_LENGTH_TD*>(&length),reinterpret_cast<uint8_t*>(&DCID_min_conn_interval));
	
	if( status == DCI_ERR_NO_ERROR)
	{
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "BLE_min_conn_interval : %d\n",DCID_min_conn_interval);
	}
	else
	{
	    DCID_min_conn_interval = BLE_MIN_VAL_CONN_INTERVAL;
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Ble Minimum Connection Interval Read Error\n");
	}
	
	status = ble_dci_ref->Get_Dcid_Data(DCI_BLE_MAX_VAL_CONN_INTERVAL_DCID,
	reinterpret_cast<DCI_LENGTH_TD*>(&length),reinterpret_cast<uint8_t*>(&DCID_max_conn_interval));
	
	if( status == DCI_ERR_NO_ERROR)
	{
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "BLE_max_conn_interval : %d\n",DCID_max_conn_interval);
	}
	else
	{
	    DCID_max_conn_interval = BLE_MAX_VAL_CONN_INTERVAL;
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Ble_Maximum_Connection_Interval Read Error\n");
	}  
	
	status = ble_dci_ref->Get_Dcid_Data(DCI_BLE_SLAVE_LATENCY_DCID,
	reinterpret_cast<DCI_LENGTH_TD*>(&length),reinterpret_cast<uint8_t*>(&DCID_slave_latency));
	
	if( status == DCI_ERR_NO_ERROR)
	{
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "BLE_slave_latency : %d\n",DCID_slave_latency);
	}
	else
	{
	    DCID_slave_latency = BLE_SLAVE_LATENCY;
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Ble_slave_latency Read Error\n");
	}
	
	status = ble_dci_ref->Get_Dcid_Data(DCI_BLE_SUPERVISION_TIMEOUT_DCID,
	reinterpret_cast<DCI_LENGTH_TD*>(&length),reinterpret_cast<uint8_t*>(&DCID_supervision_timeout));
	
	if( status == DCI_ERR_NO_ERROR)
	{
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "BLE_supervision_timeout : %d\n",DCID_supervision_timeout);
	}
	else
	{
	    DCID_supervision_timeout = BLE_SUPERVISION_TIMEOUT;
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Ble_supervision_timeout Read Error\n");
	}
}

/***********************************************************************************************//**
 *  \brief  Get passkey function is to retrive the default passkey from the DCI Workbook.
 *  \param[in] None.
 *  \return  None
 **************************************************************************************************/
void get_passkey(void)
{
	DCI_ERROR_TD status;
	uint16_t length;
    
    /* Get the Ble Dci Object */
    ble_dci_ref = Get_Ble_Dci_Obj();    

	status = ble_dci_ref->Get_Dcid_Data(DCI_BLE_DEVICE_PASSKEY_DCID,
			reinterpret_cast<DCI_LENGTH_TD*>(&length),reinterpret_cast<uint8_t*>(&DCID_passkey));
	
	if( status == DCI_ERR_NO_ERROR)
	{
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Passkey : %d\n",DCID_passkey);
	}
	else
	{
	    DCID_passkey = BLE_DEFAULT_PASSKEY;
		BLE_DEBUG_PRINT(BF_DBG_MSG_DEBUG, "Passkey Read Error\n");
	}
}
 
/***********************************************************************************************//**
 *  \brief  The main program.
 *  \param[in] None.
 *  \return  None.
 **************************************************************************************************/
void sampleapp(void)
{
	struct gecko_cmd_packet* evt;
	/* Check for stack event. */
	evt = gecko_peek_event();
	/* Run application and event handler. */
	appHandleEvents(evt);
}

/***************************************************************************************************
 * Static Function Definitions
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Function called when a message needs to be written to the serial port.
 *  \param[in] msg_len Length of the message.
 *  \param[in] msg_data Message data, including the header.
 **************************************************************************************************/
static void on_message_send(uint32_t msg_len, uint8_t* msg_data)
{
	/** Variable for storing function return values. */
	uartTx(msg_len, msg_data);
}

/***********************************************************************************************//**
 *  \brief  The uart receive function for BGLIB
 *  \param[in] dataLength of the packet to be received.
 *  \param[in] data receieved from the UART.
 *  \return  1 for success 0 for failure.
 **************************************************************************************************/
int32_t uartRx(uint32_t dataLength, uint8_t* data)
{
	int32_t retval=0;    
    /* Get the Ble Uart Port Object */
    ble_uart_port = Get_Ble_Serial_Obj();
    
	retval = ble_uart_port->Bg_Lib_Read( dataLength,data );
	return retval;
}

/***********************************************************************************************//**
 *  \brief  The uart transmit function for BGLIB
 *  \param[in] dataLength of the packet to be sent.
 *  \param[in] data to be sent to the UART.
 *  \return  1 for success 0 for failure.
 **************************************************************************************************/
int32_t uartTx(uint32_t dataLength, uint8_t* data)
{
    /* Get the Ble Uart Port Object */
    ble_uart_port = Get_Ble_Serial_Obj();
    
    ble_uart_port->Bg_Lib_Write( dataLength,data);
	return 1;
}

/***********************************************************************************************//**
 *  \brief  The Receive peek function to check whether any data received in the UART
 *  \param[in] None.
 *  \return  1 for some data packet received in the UART , 0 for queue is empty.
 **************************************************************************************************/
int32_t uartRxPeek(void)
{
    /* Get the Ble Uart Port Object */
    ble_uart_port = Get_Ble_Serial_Obj();
    
    if(ble_uart_port->Is_Queue_Empty())
	{
		return (0);
	}
	else
	{
		return (1);
	}
}