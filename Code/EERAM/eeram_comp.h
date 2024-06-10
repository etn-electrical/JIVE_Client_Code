/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : eeram_comp.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/

#ifndef EERAM_H
#define EERAM_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include "esp_system.h"
#include "esp_log.h"
#include "esp_crc.h"
#include "../DeviceGPIO.h"
#include "freertos/queue.h"
#include "Metrology/metro_utilities.h"
#include "driver/i2c.h"
/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/
#define EERAM_TASK_STACK_SIZE 2048
#define MAX_CONSECUTIVE_I2C_ERROR 3
#define EERAM_MAX_QUEUE_SIZE 5
#define MAX_EERAM_PACKET_DATA_SIZE 128

#define STARTUP_OPEN 0
#define STARTUP_LAST 1
/*******************************************************************************
* TYPES:
*******************************************************************************/
/**
 * @brief EERAM ADDRESS definition
 *
 */
typedef enum {

	EERAM_PHA_FORWARD_ACT_EGY_ADD = 0x0000,		 //8byte
	EERAM_PHB_FORWARD_ACT_EGY_ADD =	0x0008,		 //8byte

	EERAM_PHA_FORWARD_REACT_EGY_ADD = 0x0010,	//8byte
	EERAM_PHB_FORWARD_REACT_EGY_ADD	= 0x0018,	//8byte

	EERAM_PHA_FORWARD_APP_EGY_ADD = 0x0020,		 //8byte
	EERAM_PHB_FORWARD_APP_EGY_ADD =	0x0028,		 //8byte

	EERAM_PHA_REVERSE_ACT_EGY_ADD =	0x0030,		 //8byte
	EERAM_PHB_REVERSE_ACT_EGY_ADD = 0x0038,		 //8byte

	EERAM_PHA_REVERSE_REACT_EGY_ADD	= 0x0040,	//8byte
	EERAM_PHB_REVERSE_REACT_EGY_ADD = 0x0048,	//8byte

	EERAM_PHA_REVERSE_APP_EGY_ADD =	0x0050,		 //8byte
	EERAM_PHB_REVERSE_APP_EGY_ADD = 0x0058, 	//8byte

	EERAM_UNICAST_PRIMARY_KEY_TTL_ADD =	0x00C1, 	//4 bytes
	EERAM_BROADCAST_PRIMARY_KEY_TTL_ADD = 0x00C7,		//4 byte

	EERAM_UNICAST_PRIMARY_UDP_KEY_STATUS_ADD =	0x00CB, 	//12 bytes
	EERAM_BROADCAST_PRIMARY_UDP_KEY_STAUS_ADD = 0x00D7,		//12 byte

	EERAM_UNICAST_SECONDARY_KEY_TTL_ADD =	0x00E3, 	//4 bytes
	EERAM_BROADCAST_SECONDARY_KEY_TTL_ADD = 0x00E7,		//4 byte

	EERAM_UNICAST_SECONDARY_UDP_KEY_STATUS_ADD =	0x00EB, 	//12 bytes
	EERAM_BROADCAST_SECONDARY_UDP_KEY_STAUS_ADD = 0x00F7,		//12 byte

	EERAM_SBLCP_FACTORY_MODE_ADD	= 0x0103,	//1 byte

	EERAM_STARTUP_CONFIG_ADD = 0x0104, //1 bytes
	EERAM_LAST_CONTACT_STATE_ADD = 0x0105 //1 byte
} MetroEnergyAddrType;


/**
  * @brief EERAM  ERROR definition
  *
  */
typedef enum {
    EERAM_SUCCESS = 0,
    EERAM_I2C_ERR = 20,
} eeram_err_t;


/**
 * @brief EERAM EVENT definition
 *
 */
typedef enum {
	EERAM_WRITE_DATA,
	EERAM_DEFAULT_EVENT,
} EERAMEventType;


/**
 * @brief EERAM Message Packet definition
 *
 */
typedef struct __attribute__((__packed__))
{
	EERAMEventType event;
    uint16_t address;
    uint8_t data_array[MAX_EERAM_PACKET_DATA_SIZE];
    uint8_t data_size;
    uint8_t read_write_status; //for future use
}EERAM_Message_Packet;


/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void EERAM_Task(void* arg);
void EERAM_ReInit(eeram_err_t ret);
void EERAM_Task_Stop_Execution(void);
void EERAM_Task_Delete(void);
void EERAM_Update_trending_data_Timer(void);
void EERAM_Send_Message_Packet(EERAM_Message_Packet *eeram_message_packet);
void EERAM_Create_Message_Packet(EERAM_Message_Packet *eeram_message_packet, void* data_ptr, uint16_t address, uint16_t data_size);


eeram_err_t EERAM_Init(void);
eeram_err_t EERAM_Read_data(uint16_t address, void* dataPtr, size_t dataSize);
eeram_err_t EERAM_Enable_Auto_Store_Mode(void);
eeram_err_t EERAM_Error_Check(eeram_err_t ret);
eeram_err_t EERAM_Read_Metro_data(void);
eeram_err_t EERAM_Reset_Energy(void);
eeram_err_t EERAM_Task_Init(void);
eeram_err_t EERAM_Write_data(EERAM_Message_Packet *eeram_message_packet);
TaskHandle_t EERAM_Get_Task_Handle();

eeram_err_t EERAM_Init_Startup_Config(void);
eeram_err_t EERAM_Set_Startup_Config(uint8_t startup_config);
eeram_err_t EERAM_Set_Last_State(uint8_t last_state);

#endif /* EERAM_H */
