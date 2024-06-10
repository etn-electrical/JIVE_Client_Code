/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : app_conf.h Description        : Application configuration file for STM32WPAN Middleware.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics. All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license SLA0044, the "License"; You may not use this
 * file except in compliance with the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_CONF_H
#define APP_CONF_H

#include "shci.h"
#include "hw.h"
#include "ble_std.h"
#include "uC_IO_Define_STM32WB55R.h"
/*Configuration in app_conf.h file can be overridden by the user from below file*/
#include "BLE_STM32_WPAN_Config.h"

/******************************************************************************
* Application Config
******************************************************************************/

/**
 * Define Tx Power
 */
#ifndef CFG_TX_POWER
#define CFG_TX_POWER                      ( 0x18 )	/**< 0dbm */
#endif
/**
 * Define Advertising parameters
 */
#ifndef CFG_ADV_BD_ADDRESS
#define CFG_ADV_BD_ADDRESS                ( 0x7257acd87a6c )
#endif

#ifndef CFG_CONN_ADV_INTERVAL_MIN
#define CFG_CONN_ADV_INTERVAL_MIN    ( 0x80 )	/**< 80ms */
#endif

#ifndef CFG_CONN_ADV_INTERVAL_MAX
#define CFG_CONN_ADV_INTERVAL_MAX    ( 0xa0 )	/**< 100ms */
#endif

/**
 * Define IO Authentication
 */
#ifndef CFG_BONDING_MODE
#define CFG_BONDING_MODE                  ( 1 )
#endif

#ifndef CFG_FIXED_PIN
#define CFG_FIXED_PIN                     ( 111111 )
#endif

#define CFG_USED_FIXED_PIN                ( 0 )
#define CFG_ENCRYPTION_KEY_SIZE_MAX       ( 16 )
#define CFG_ENCRYPTION_KEY_SIZE_MIN       ( 8 )

/**
 * Define IO capabilities
 */
#define CFG_IO_CAPABILITY_DISPLAY_ONLY       ( 0x00 )
#define CFG_IO_CAPABILITY_DISPLAY_YES_NO     ( 0x01 )
#define CFG_IO_CAPABILITY_KEYBOARD_ONLY      ( 0x02 )
#define CFG_IO_CAPABILITY_NO_INPUT_NO_OUTPUT ( 0x03 )
#define CFG_IO_CAPABILITY_KEYBOARD_DISPLAY   ( 0x04 )

#ifndef CFG_IO_CAPABILITY
#define CFG_IO_CAPABILITY             CFG_IO_CAPABILITY_NO_INPUT_NO_OUTPUT
#endif

/**
 * Define MITM modes
 */
#define CFG_MITM_PROTECTION_NOT_REQUIRED      ( 0x00 )
#define CFG_MITM_PROTECTION_REQUIRED          ( 0x01 )

#ifndef CFG_MITM_PROTECTION
#define CFG_MITM_PROTECTION             CFG_MITM_PROTECTION_NOT_REQUIRED
#endif

/**
 * Define GATT characteristics permission
 */
#ifndef ATTR_PERMISSION
#define ATTR_PERMISSION                 ATTR_PERMISSION_NONE
#endif

/**
 * Define PHY
 */
#define ALL_PHYS_PREFERENCE                             0x00
#define RX_2M_PREFERRED                                 0x02
#define TX_2M_PREFERRED                                 0x02
#define TX_1M                                           0x01
#define TX_2M                                           0x02
#define RX_1M                                           0x01
#define RX_2M                                           0x02

/**
 *   Identity root key used to derive LTK and CSRK
 */
#ifndef CFG_BLE_IRK
#define CFG_BLE_IRK     {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0}
#endif

/**
 * Encryption root key used to derive LTK and CSRK
 */
#ifndef CFG_BLE_ERK
#define CFG_BLE_ERK     {0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21, 0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21}
#endif

/**
 * Define Secure Connections Support
 */
#define CFG_SECURE_NOT_SUPPORTED       ( 0x00 )
#define CFG_SECURE_OPTIONAL            ( 0x01 )
#define CFG_SECURE_MANDATORY           ( 0x02 )

#ifndef CFG_SC_SUPPORT
#define CFG_SC_SUPPORT                 CFG_SECURE_MANDATORY
#endif

/**
 * Define Keypress Notification Support
 */
#define CFG_KEYPRESS_NOT_SUPPORTED      ( 0x00 )
#define CFG_KEYPRESS_SUPPORTED          ( 0x01 )

#ifndef CFG_KEYPRESS_NOTIFICATION_SUPPORT
#define CFG_KEYPRESS_NOTIFICATION_SUPPORT             CFG_KEYPRESS_NOT_SUPPORTED
#endif

/**
 * Numeric Comparison Answers
 */
#define YES ( 0x01 )
#define NO  ( 0x00 )




/******************************************************************************
* BLE Stack
******************************************************************************/
/**
 * Maximum number of simultaneous connections that the device will support. Valid values are from 1 to 8
 */
#ifndef CFG_BLE_NUM_LINK
#define CFG_BLE_NUM_LINK            4
#endif

/**
 * Maximum number of Services that can be stored in the GATT database. Note that the GAP and GATT services are
 * automatically added so this parameter should be 2 plus the number of user services
 */
// TODO: Get input from BLE_DCI_Data.cpp
#ifndef CFG_BLE_NUM_GATT_SERVICES
#define CFG_BLE_NUM_GATT_SERVICES   7
#endif

/**
 * Maximum number of Attributes (i.e. the number of characteristic + the number of characteristic values + the number of
 * descriptors, excluding the services) that can be stored in the GATT database. Note that certain characteristics and
 * relative descriptors are added automatically during device initialization so this parameters should be 9 plus the
 * number of user Attributes (attribute means char. UUID + value + descriptors UUID + descriptors value)
 */
#ifndef CFG_BLE_NUM_GATT_ATTRIBUTES
#define CFG_BLE_NUM_GATT_ATTRIBUTES ( DCI_BLE_DATA_TOTAL_CHAR_ID * 3 + 9 )
#endif

/**
 * Maximum supported ATT_MTU size
 */
#ifndef CFG_BLE_MAX_ATT_MTU
#define CFG_BLE_MAX_ATT_MTU             ( 156U )
#endif

/**
 * Size of the storage area for Attribute values This value depends on the number of attributes used by application. In
 * particular the sum of the following quantities (in octets) should be made for each attribute:
 *  - attribute value length
 *  - 5, if UUID is 16 bit; 19, if UUID is 128 bit
 *  - 2, if server configuration descriptor is used
 *  - 2*DTM_NUM_LINK, if client configuration descriptor is used
 *  - 2, if extended properties is used The total amount of memory needed is the sum of the above quantities for each
 * attribute.
 */

// #define CFG_BLE_ATT_VALUE_ARRAY_SIZE    (1344)
#ifndef CFG_BLE_ATT_VALUE_ARRAY_SIZE
#define CFG_BLE_ATT_VALUE_ARRAY_SIZE    ( ( 19 + 2 ) * DCI_BLE_DATA_TOTAL_CHAR_ID + BLE_TOTAL_DCID_RAM_SIZE )
#endif

/**
 * Prepare Write List size in terms of number of packet with ATT_MTU=23 bytes
 */
#ifndef CFG_BLE_PREPARE_WRITE_LIST_SIZE
#define CFG_BLE_PREPARE_WRITE_LIST_SIZE         ( 0x3A )
#endif

/**
 * Number of allocated memory blocks
 */
#ifndef CFG_BLE_MBLOCK_COUNT
#define CFG_BLE_MBLOCK_COUNT            ( 0x79 )
#endif

/**
 * Enable or disable the Extended Packet length feature. Valid values are 0 or 1.
 */
#ifndef CFG_BLE_DATA_LENGTH_EXTENSION
#define CFG_BLE_DATA_LENGTH_EXTENSION   1
#endif

/**
 * Sleep clock accuracy in Slave mode (ppm value)
 */
#ifndef CFG_BLE_SLAVE_SCA
#define CFG_BLE_SLAVE_SCA   500
#endif

/**
 * Sleep clock accuracy in Master mode 0 : 251 ppm to 500 ppm 1 : 151 ppm to 250 ppm 2 : 101 ppm to 150 ppm 3 : 76 ppm
 * to 100 ppm 4 : 51 ppm to 75 ppm 5 : 31 ppm to 50 ppm 6 : 21 ppm to 30 ppm 7 : 0 ppm to 20 ppm
 */
#ifndef CFG_BLE_MASTER_SCA
#define CFG_BLE_MASTER_SCA   0
#endif

/**
 *  Source for the 32 kHz slow speed clock 1 : internal RO 0 : external crystal ( no calibration )
 */
#ifndef CFG_BLE_LSE_SOURCE
#define CFG_BLE_LSE_SOURCE  1
#endif

/**
 * Start up time of the high speed (16 or 32 MHz) crystal oscillator in units of 625/256 us (~2.44 us)
 */
#ifndef CFG_BLE_HSE_STARTUP_TIME
#define CFG_BLE_HSE_STARTUP_TIME  0x148
#endif

/**
 * Maximum duration of the connection event when the device is in Slave mode in units of 625/256 us (~2.44 us)
 */
#ifndef CFG_BLE_MAX_CONN_EVENT_LENGTH
#define CFG_BLE_MAX_CONN_EVENT_LENGTH  ( 0xFFFFFFFF )
#endif

/**
 * Viterbi Mode 1 : enabled 0 : disabled
 */
#ifndef CFG_BLE_VITERBI_MODE
#define CFG_BLE_VITERBI_MODE  1
#endif

/**
 *  LL Only Mode 1 : LL Only 0 : LL + Host
 */
#ifndef CFG_BLE_LL_ONLY
#define CFG_BLE_LL_ONLY  0
#endif

#ifndef MAX_BUFFER
static const uint16_t MAX_BUFFER = CFG_BLE_MAX_ATT_MTU - ( HCI_COMMAND_MAX_PARAM_LEN - HCI_ACLDATA_MAX_DATA_LEN );
#endif


/* *< specific parameters
 ****************************************************/

/******************************************************************************
* Transport Layer
******************************************************************************/
/**
 * Queue length of BLE Event This parameter defines the number of asynchronous events that can be stored in the HCI
 * layer before being reported to the application. When a command is sent to the BLE core coprocessor, the HCI layer is
 * waiting for the event with the Num_HCI_Command_Packets set to 1. The receive queue shall be large enough to store all
 * asynchronous events received in between. When CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE is set to 27, this allow to store
 * three 255 bytes long asynchronous events between the HCI command and its event. This parameter depends on the value
 * given to CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE. When the queue size is to small, the system may hang if the queue is full
 * with asynchronous events and the HCI layer is still waiting for a CC/CS event, In that case, the notification
 * TL_BLE_HCI_ToNot() is called to indicate to the application a HCI command did not receive its command event within
 * 30s (Default HCI Timeout).
 */
#ifndef CFG_TLBLE_EVT_QUEUE_LENGTH
#define CFG_TLBLE_EVT_QUEUE_LENGTH 5
#endif

/**
 * This parameter should be set to fit most events received by the HCI layer. It defines the buffer size of each element
 * allocated in the queue of received events and can be used to optimize the amount of RAM allocated by the Memory
 * Manager. It should not exceed 255 which is the maximum HCI packet payload size (a greater value is a lost of memory
 * as it will never be used) It shall be at least 4 to receive the command status event in one frame. The default value
 * is set to 27 to allow receiving an event of MTU size in a single buffer. This value maybe reduced further depending
 * on the application.
 *
 */
#ifndef CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE
#define CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE 255	/**< Set to 255 with the memory manager and the mailbox */
#endif

#ifndef TL_BLE_EVENT_FRAME_SIZE
#define TL_BLE_EVENT_FRAME_SIZE ( TL_EVT_HDR_SIZE + CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE )
#endif

/******************************************************************************
* OTP manager
******************************************************************************/
#ifndef CFG_OTP_BASE_ADDRESS
#define CFG_OTP_BASE_ADDRESS    OTP_AREA_BASE
#endif

#ifndef CFG_OTP_END_ADRESS
#define CFG_OTP_END_ADRESS      OTP_AREA_END_ADDR
#endif


// Hardware sempahore ID to use for HCI_CMD_RESP
#ifndef HCI_CMD_RESP_HSEM
#define HCI_CMD_RESP_HSEM       ( 31U )
#endif

// Hardware sempahore ID to use for SHCI_CMD_RESP
#ifndef SHCI_CMD_RESP_HSEM
#define SHCI_CMD_RESP_HSEM      ( 30U )
#endif

// Hardware sempahore ID to use for SHCI_CMD_RESP
#ifndef RCC_CRRCR_CCIPR_HSEM
#define RCC_CRRCR_CCIPR_HSEM        ( 5U )
#endif

// Hardware semaphore ID to use for SHCI_CMD_RESP
#ifndef RNG_HSEM
#define RNG_HSEM                 ( 0U )
#endif

#endif  /*APP_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
