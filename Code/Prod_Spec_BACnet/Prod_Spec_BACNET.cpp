/**
 **********************************************************************************************
 * @file            Prod_Spec_BACNET.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to BACNET
 * component.
 *
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "Prod_Spec_BACNET.h"
#include "Prod_Spec_LTK_ESP32.h"
// #include "uC_USART_Soft_DMA.h"
#include "vsbhp.h"
#include "BACnetServer.h"
#include "BACnetDCIInterface.h"
#include "BACnet_DCI.h"
#include "BACnetUARTInterface.h"
#include "Shadow_BACnet.h"
#include "Prod_Spec_Services.h"
#include "Output_Null.h"
#include "Prod_Spec_Debug.h"


/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
/* Extern First */

DCI_Owner* bacnet_ip_enable_owner = nullptr;

DCI_Owner* IPAddConflictFaultProtect_Owner = nullptr;
DCI_Owner* MbsRtu_LastFaultCode_owner = nullptr;
DCI_Owner* MbsTcp_LastFaultCode_Owner = nullptr;

DCI_Owner* bacnet_ip_enabled_owner = nullptr;
DCI_Owner* bacnet_ip_instance_number_owner = nullptr;
DCI_Owner* bacnet_ip_fault_code_owner = nullptr;

DCI_Owner* bac_baud_rate_owner = nullptr;
DCI_Owner* bac_dev_addr_owner = nullptr;

uint8_t ubRS485Comm = 0;
BACnet_UART* bacnet_uart_for_485 = nullptr;
BACnet_DCI* BACnetDCIPtr = nullptr;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
void Init_Bacnet_Application_Parameters( void );

void Init_Bacnet_Uart_Port( void );

uint8_t Update_Our_Station( void );

/**
 **********************************************************************************************
 * @brief                     Initialize the BACnet instance number and serial number
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_BACNET_Init( void )
{
	DCI_Owner* bac_instance_number_owner = nullptr;
	UINT32 bacnet_instance_number = 0;
	UINT32 date = 0;
	UINT32 serial_seq_number = 0;

	// coverity[leaked_storage]
	new DCI_Owner( DCI_MAX_MASTER_DCID );	// DH1-131 made Writable

	bac_dev_addr_owner = new DCI_Owner( DCI_BACNET_DEVICE_ADDR_DCID );

	bac_instance_number_owner = new DCI_Owner( DCI_BACNET_MSTP_INSTANCE_NUM_DCID );

	bacnet_ip_instance_number_owner = new DCI_Owner( DCI_BACNET_IP_INSTANCE_NUM_DCID );

	BACnetDCIPtr = new BACnet_DCI();

	GetProductSerialNumber( &bacnet_instance_number );
	serial_seq_number = bacnet_instance_number & 0xFFF;			// get serial_seq_number
	date = bacnet_instance_number & 0x7FC0000;	// get date
	bacnet_instance_number = date;	// Copy date
	bacnet_instance_number = ( bacnet_instance_number >> 6 ) | serial_seq_number;// Shift it
	bacnet_instance_number |= serial_seq_number;// copy serial_seq_number
	bac_instance_number_owner->Check_In_Default( ( DCI_RAM_DATA_TD* )&bacnet_instance_number );
	bacnet_ip_instance_number_owner->Check_In_Default( ( DCI_RAM_DATA_TD* )&bacnet_instance_number );
	/*
	   For the bacnet instance number to change according to serial number following steps are to be
	      executed:
	   Step 1: Manufacture unlock the device
	   Step 2: Change the serial number.
	   Step 3: Perform power cycle
	   Step 4: Perform factory reset
	   After these steps default value of bacnet instance number is derived from the serial number
	      and
	   will be updated which can be seen on the bacbeat
	 */
	// SABus_instance_number_owner->Check_In_Default((DCI_RAM_DATA_TD*)&bacnet_instance_number);
	Init_Bacnet_Application_Parameters();
	new Shadow_BACnet();

#ifdef BACNET_MSTP_SETUP
	Init_Bacnet_Uart_Port();
#endif

	new BACnetServer( &bacnet_dci_data_target_info, Is_Bacnet_Ip_Enabled() );
	// coverity[leaked_storage]
}

/**
 **********************************************************************************************
 * @brief                     To check BACnet ip is enable or not
 * @param[in] void            none
 * @return[out] void          bacnet_ip_enable
 * @n
 **********************************************************************************************
 */
bool_t Is_Bacnet_Ip_Enabled( void )
{
	bool_t bacnet_ip_enable = false;

#ifdef BACNET_IP_SETUP
	bacnet_ip_enable_owner = new DCI_Owner( DCI_BACNET_IP_ENABLE_DCID );
	bacnet_ip_enable_owner->Check_Out( bacnet_ip_enable );	/*Application can read the DCI here*/
#endif
	return ( bacnet_ip_enable );
}

/**
 **********************************************************************************************
 * @brief                     Initialize the BACnet uart port
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void Init_Bacnet_Uart_Port( void )
{
#ifdef BACNET_MSTP_SETUP
	UINT32 bacnet_baud_rate = 38400;
	bac_baud_rate_owner = new DCI_Owner( DCI_BACNET_MSTP_BAUD_DCID );

	bac_baud_rate_owner->Check_Out( bacnet_baud_rate );
	uC_USART_Soft_DMA* bacnet_rs485_port = new uC_USART_Soft_DMA( BACNET_SERIAL_PORT,
																  BACNET_SERIAL_TIMER_CTRL, &BACNET_SERIAL_PIO );

	// BF_Lib::Output_Pos *bacnet_rs485_tx_enable = new BF_Lib::Output_Pos( &RS485_SLAVE_TXEN_PIO );
	BF_Lib::Output* bacnet_rs485_tx_enable = new BF_Lib::Output_Null();

	bacnet_uart_for_485 = new BACnet_UART( bacnet_rs485_port );	// , bacnet_rs485_tx_enable);

	bacnet_uart_for_485->Enable_Port( BACNET_MSTP_TX_MODE, BACNET_PARITY_NONE, bacnet_baud_rate, BACNET_1_STOP_BIT );
	frStartup( portMSTP );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "BACnet/MSTP initialised, Buad rate: %d and Parity: %s", bacnet_baud_rate,
						   "None" );

#endif
}

/**
 **********************************************************************************************
 * @brief                     Initialize the BACnet Application parameter and create the DCI owner
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void Init_Bacnet_Application_Parameters( void )
{
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ANALOG_VALUE_0_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ANALOG_VALUE_1_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_AV_ANY_PARAM_NUM_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_AV_ANY_PARAM_VAL_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ANALOG_INPUT_0_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_ANALOG_INPUT_1_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_DIGITAL_INPUT_WORD_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_DIGITAL_OUTPUT_WORD_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_DIGITAL_INPUT_BIT_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_DIGITAL_OUTPUT_BIT_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_MULTI_STATE_0_DCID );
	// coverity[leaked_storage]
	new DCI_Owner( DCI_MULTI_STATE_1_DCID );

}

/**
 **********************************************************************************************
 * @brief                     To Update the station
 * @param[in] void            none
 * @return[out] void          device address
 * @n
 **********************************************************************************************
 */
uint8_t Update_Our_Station( void )
{
	uint8_t device_addr = 0U;

	bac_dev_addr_owner->Check_Out( device_addr );
	return ( device_addr );
}
