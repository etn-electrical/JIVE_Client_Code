/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : eeram_comp.c
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/

/****************************************************************************
 *                           	Include files
 ****************************************************************************/
#include <stdio.h>
#include "esp_log.h"
#include "i2c_comp.h"
#include <math.h>
#include "ErrorLog.h"
#include "Log.h"
#include "eeram_comp.h"
#include "EventManager.h"
/****************************************************************************
 *                              Global variables
 ****************************************************************************/
xQueueHandle g_eeram_evt_queue = NULL;
TaskHandle_t g_eeram_task_handle;

extern DRAM_ATTR Device_Info_str DeviceInfo;
extern eeram_err_t SBLCP_resetUdpKeys();
extern eeram_err_t EERAM_Read_UDP_Keys();
/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
/**
 * @brief      This function is designed to initialize the I2C components and Enable Auto store mode
 * @param[in]  None
 * @retval     eeram_err_t error type
*/
eeram_err_t EERAM_Init(void)
{
	eeram_err_t ret = EERAM_SUCCESS;
	uint8_t readValue = 0;
	//Initialize I2C bus
	if ((I2C_Master_Init()) != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;	//I2C ERROR
	}
	else
	{
			//Enable Auto store mode
		if (EERAM_Enable_Auto_Store_Mode() != ESP_OK)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
		else
		{
			readValue = ReadByteFromFlash(EEOFFSET_EERAM_INIT_LABEL);
				//Read Energies from EERAM on Power ON
			if(readValue == EERAM_READY)
			{
				if(EERAM_Read_Metro_data() == EERAM_SUCCESS)
				{
					if(EERAM_Read_UDP_Keys() == EERAM_SUCCESS)
					{
						if(EERAM_Init_Startup_Config() != EERAM_SUCCESS)
						{
							//NOTE: I'm not sure why we dont just use the return from the functions to set ret.
							ret = EERAM_I2C_ERR;	//I2C ERROR
						}
					}
					else
					{
						ret = EERAM_I2C_ERR;	//I2C ERROR
					}
				}
				else
				{
					ret = EERAM_I2C_ERR;	//I2C ERROR
				}
			}
			else
			{
				if (EERAM_Reset_Energy() == EERAM_SUCCESS)
				{
					if(SBLCP_resetUdpKeys() == EERAM_SUCCESS)
					{
						if(EERAM_Set_Startup_Config(STARTUP_LAST) == EERAM_SUCCESS)
						{
							if(EERAM_Set_Last_State((uint8_t)SS_UNKNOWN) == EERAM_SUCCESS)
							{
								//Update the label
								WriteByteToFlash(EEOFFSET_EERAM_INIT_LABEL, EERAM_READY);
							}
							else
							{
								ret = EERAM_I2C_ERR;	//I2C ERROR
							}
						}
						else
						{
							ret = EERAM_I2C_ERR;	//I2C ERROR
						}
					}
					else
					{
						ret = EERAM_I2C_ERR;	//I2C ERROR
					}
				}
				else
				{
						//Update the label
					//NOTE: Why are we marking it ready if it fails to reset the energy?
					WriteByteToFlash(EEOFFSET_EERAM_INIT_LABEL, EERAM_READY);
				}
			}
				//Initialize EERAM task
			if (ret == EERAM_SUCCESS)
			{
				if (EERAM_Task_Init() != EERAM_SUCCESS)
				{
					ret = EERAM_I2C_ERR;	//I2C ERROR
				}
			}
		}
	}
	return ret;
}

/**
 * @brief      EERAM task stores and retrieve data from external RAM memory i.e. EERAM
 * @param[in]  None
 * @retval     eeram_err_t error type
*/
void EERAM_Task(void* arg)
{
	eeram_err_t ret = EERAM_SUCCESS;

	EERAM_Message_Packet eeram_message_packet = {
			.address = 0x0000,
			.data_size = 0,
			.read_write_status = 0,
			.data_array = {0},
			.event = EERAM_DEFAULT_EVENT,
	};

	while(1)
	{
		if (xQueueReceive(g_eeram_evt_queue, &eeram_message_packet, portMAX_DELAY)) // Wait for an event
		{
			switch(eeram_message_packet.event)
			{
				//Executes event to write data into EERAM

				case EERAM_WRITE_DATA:
				{

					if (EERAM_Write_data(&eeram_message_packet) != EERAM_SUCCESS)
					{
						ret = EERAM_I2C_ERR; //I2C ERROR
					}
					else
					{

						//If data is metro data -- > update trending data timer counter for 5 min data cloud reporting

						if (eeram_message_packet.address == EERAM_PHA_FORWARD_ACT_EGY_ADD)
						{
							EERAM_Update_trending_data_Timer();
						}
					}
					break;
				}

				default:
				{
					break;
				}
			}

				//Check consecutive I2C errors

			ret = EERAM_Error_Check(ret);
		}

		if (ret != EERAM_SUCCESS)
		{
			printf("EERAM task fail\n");

			EERAM_Task_Stop_Execution();	//Stop EERAM task execution, STop queue
			EERAM_ReInit(ret);		//Reinit EERAM task
			EERAM_Task_Delete();	//Delete EERAM task
		}
	}	//while loop ends

	EERAM_Task_Delete();
}


/**
 * @brief      This function is designed to enable Auto store and auto recall mode
 * @param[in]  None
 * @retval     eeram_err_t i2c error
*/
eeram_err_t EERAM_Enable_Auto_Store_Mode(void)
{
	eeram_err_t ret = EERAM_SUCCESS;
	uint16_t address = I2C_STATUS_REG_ADD;

	uint8_t highAddress = (address >> 8) & 0xFF;
	uint8_t lowAddress = address & 0xFF;

	uint8_t reg_data = I2C_STATUS_REG_VAL;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();	//create i2c link for communication

	if ((i2c_master_start(cmd)) != ESP_OK)	//Generate the start condition on the I2C bus
	{
		ret = EERAM_I2C_ERR; 	//I2C ERROR
	}
	else
	{
		i2c_master_write_byte(cmd, (DEVICE_ADDRESS_CONTROL_REG << 1) | I2C_MASTER_WRITE, true);	//load write control byte

		i2c_master_write_byte(cmd, highAddress, true);	//write 9 bit address

		i2c_master_write_byte(cmd, lowAddress, true);

		i2c_master_write_byte(cmd, reg_data, true);

		i2c_master_stop(cmd);

			//Executes sequence of I2C commands that are stored in an i2c_cmd_handle_t command sequence
			//i2c_master_cmd_begin function implicitly verifies the ACK/NACK status during the execution of the command sequence.
			//If an ACK/NACK error occurs, it can be detected and handled through the return value of the function.
			//A non-zero return value indicates an error during the I2C transaction.

		if ((i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS((TickType_t)1000))) != ESP_OK)
		{
			ret = EERAM_I2C_ERR;	 //I2C ERROR
		}
	}

	i2c_cmd_link_delete(cmd);

	return ret;
}


/**
 * @brief      This function is designed to write data into EERAM
 * @param[in]  message packet
 * @retval     eeram_err_t i2c error type
*/
eeram_err_t EERAM_Write_data(EERAM_Message_Packet *eeram_message_packet)
{
	#define MAX_I2C_RETRIES 2
	eeram_err_t ret = EERAM_SUCCESS;
	const uint8_t *byteData = eeram_message_packet->data_array;
	i2c_cmd_handle_t cmd = NULL;
	//separate higher and lower data bytes

	uint8_t highAddress = (eeram_message_packet->address >> 8) & 0xFF;
	uint8_t lowAddress = eeram_message_packet->address & 0xFF;

	for (uint8_t retry = 0; retry < MAX_I2C_RETRIES; retry++)
	{
		if ((eeram_message_packet->address + eeram_message_packet->data_size) < I2C_MAX_EERAM_BYTE_SIZE)
		{
			cmd = i2c_cmd_link_create();
			if(cmd != NULL)
			{
				if ((i2c_master_start(cmd)) != ESP_OK)
				{
					ret = EERAM_I2C_ERR;	 //I2C ERROR
				}
				else
				{
					i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);

					i2c_master_write_byte(cmd, highAddress, true);	//Load higher address byte
					i2c_master_write_byte(cmd, lowAddress, true);	//Load lower address byte

					for (size_t i = 0; i < eeram_message_packet->data_size; i++)
					{
						i2c_master_write_byte(cmd, byteData[i], true);
					}

					i2c_master_stop(cmd);

					if (i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000)) != ESP_OK)
					{
						ret = EERAM_I2C_ERR;	 //I2C ERROR
					}
				}

				i2c_cmd_link_delete(cmd);

				if (ret == ESP_OK)
				{
					break;
				}
			}
			else
			{
				ret = EERAM_I2C_ERR;
			}
		}
		else
		{
			ret = EERAM_I2C_ERR;	 //I2C ERROR
		}
	}

	return ret;
}



/**
 * @brief      This function read data from EERAM
 * @param[in]  uint16_t address, const void* dataPtr, size_t length
 * @retval     eeram_err_t i2c error type
*/
eeram_err_t EERAM_Read_data(uint16_t address, void* dataPtr, size_t length)
{
	#define MAX_I2C_RETRIES 2
	eeram_err_t ret = EERAM_SUCCESS;

    uint8_t highAddress = (address >> 8) & 0x01;
    uint8_t lowAddress = address & 0xFF;

    uint8_t* byteData = (uint8_t*)dataPtr;

    if (dataPtr == NULL)
	{
    	ret = EERAM_I2C_ERR;	//I2C ERROR
	}
    	//If I2C successfully read at first attempt loop will execute once.
    	//If error occurs, I2C retry to read the EERAM chip again.

	for (uint8_t retry = 0; retry < MAX_I2C_RETRIES; retry++)
	{
			//Check memory boundary condition

		if ((address + length) < I2C_MAX_EERAM_BYTE_SIZE)
		{
			i2c_cmd_handle_t cmd = i2c_cmd_link_create();	//create i2c link for communication

			if ((i2c_master_start(cmd)) != ESP_OK)	//Generate the start condition on the I2C bus
			{
				ret = EERAM_I2C_ERR;	//I2C ERROR
			}
			else
			{

				i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_WRITE, true);	//load write control byte

				i2c_master_write_byte(cmd, highAddress, true);	//write 9 bit address

				i2c_master_write_byte(cmd, lowAddress, true);

				if ((i2c_master_start(cmd)) != ESP_OK)	//Generate the start condition on the I2C bus
				{
					ret = EERAM_I2C_ERR;	//I2C ERROR
				}
				else
				{
					i2c_master_write_byte(cmd, (I2C_DEVICE_ADDRESS << 1) | I2C_MASTER_READ, true);	//load read control byte

						//read data bytes from EERAM

					for (size_t i = 0; i < (length - 1); i++)
					{
						i2c_master_read_byte(cmd, &byteData[i], I2C_MASTER_ACK);
					}

					i2c_master_read_byte(cmd, &byteData[length - 1], I2C_MASTER_NACK);

					i2c_master_stop(cmd);

						//Executes sequence of I2C commands that are stored in an i2c_cmd_handle_t command sequence
						//i2c_master_cmd_begin function implicitly verifies the ACK/NACK status during the execution of the command sequence.
						//If an ACK/NACK error occurs, it can be detected and handled through the return value of the function.
						//A non-zero return value indicates an error during the I2C transaction.

					if (i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000)) != ESP_OK)
					{
						ret = EERAM_I2C_ERR;	//I2C ERROR
					}
				}
			}

			i2c_cmd_link_delete(cmd);

			if (ret == EERAM_SUCCESS)
			{
				break;
			}
		}
		else
		{
			ret = EERAM_I2C_ERR;	 //I2C ERROR
		}
	}

    return ret;
}


/**
 * @brief      This function check I2C consecutive error
 * @param[in]  eeram_err_t err_type
 * @retval     eeram_err_t err_type
*/
eeram_err_t EERAM_Error_Check(eeram_err_t ret)
{
	static uint8_t i2c_error_counter = 0;

	if (ret != EERAM_SUCCESS)
	{
		printf("Communication fail...\n");

		if (ret == EERAM_I2C_ERR)
		{
			i2c_error_counter++;
		}
	}
	else
	{
		i2c_error_counter = 0;
	}

	if (i2c_error_counter == MAX_CONSECUTIVE_I2C_ERROR)
	{
		ret = EERAM_I2C_ERR;	//I2C ERROR
	}
	else
	{
		ret = EERAM_SUCCESS;
	}

	return ret;
}


/**
 * @brief      This function delete EERAM task
 * @param[in]  None
 * @retval     None
*/
void EERAM_Task_Delete(void)
{
	if (g_eeram_task_handle != NULL)
	{
		ets_printf("EERAM task delete\n");
		vTaskDelete(g_eeram_task_handle); 	//delete EERAM task
		g_eeram_task_handle = NULL;
	}
}



/**
 * @brief      This function stop the EERAM task
 * @param[in]  None
 * @retval     None
*/
void EERAM_Task_Stop_Execution(void)
{
	if (g_eeram_task_handle != NULL)
	{
		if (g_eeram_evt_queue != NULL)
		{
			printf("EERAM delete queue\n");
			vQueueDelete(g_eeram_evt_queue);	//delete queue
			g_eeram_evt_queue = NULL;		//queue = NULL
		}
	}
}



/**
 * @brief     This function re-init the EERAM module
 * @inval	   eeram_err_t error_type
 * @retval     None
*/
void EERAM_ReInit(eeram_err_t ret)
{
	printf("EERAM_ReInit\n");
	uint8_t reset_status = 0;
	reset_status = ReadByteFromFlash(EEOFFSET_EERAM_RESET_LABEL);

	if (reset_status ==  EERAM_RESET_CLEAR)
	{
		if (ret == EERAM_I2C_ERR)
		{
			ErrLog(LOG_I2C_COMM_FAIL);	//On temp I2C err, log err to nvm
		}

		WriteByteToFlash(EEOFFSET_EERAM_RESET_LABEL, EERAM_RESET);

		if (DeviceInfo.StartReProvisioning != RE_START_PROVISIONING)
		{
			ets_printf("calling device reset\n");
			ResetDevice(DEVICE_RESET, true);
		}
	}
	else
	{
			//We need to clear it but not to reset

		if (ret == EERAM_I2C_ERR)
		{
			ErrLog(LOG_I2C_COMM_ERR);	//On I2C err, log err to nvm
		}

		WriteByteToFlash(EEOFFSET_EERAM_RESET_LABEL, EERAM_RESET_CLEAR);
	}
}

/**
 * @brief      This function read forward and reverse energies from EERAM for channel 1 and 2
 * @param[in]  None
 * @retval     eeram_err_t err type
 */
eeram_err_t EERAM_Read_Metro_data(void)
{
	eeram_err_t ret = EERAM_SUCCESS;
	Metro_Energy metro_energy;
	uint16_t init_addr = 0;

	init_addr = EERAM_PHA_FORWARD_ACT_EGY_ADD;

	memset(&metro_energy, 0, sizeof(metro_energy));

	if (EERAM_Read_data(init_addr, &metro_energy, sizeof(metro_energy)) != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;		//I2C ERROR
	}
	else
	{
		//Update the energy data into global structure

		Metro_Update_Energy_data(&metro_energy);
	}

	return ret;
}



/**
 * @brief      This function resets all the energies to zero, and writes it to EERAM locations
 * @param[in]   none
 * @retval     eeram_err_t
 */
eeram_err_t EERAM_Reset_Energy(void)
{
	eeram_err_t ret = EERAM_SUCCESS;
	Metro_Energy metro_energy;
	EERAM_Message_Packet dataPacket;
	EERAM_Message_Packet *ptr_packet;

	Metro_Data_Reset(); 	//reset metrology data and cloud data structure

	memset(&metro_energy, 0, sizeof(metro_energy));

	//Create packet

	EERAM_Create_Message_Packet(&dataPacket, &metro_energy, EERAM_PHA_FORWARD_ACT_EGY_ADD, sizeof(metro_energy));

	ptr_packet = &dataPacket;

	if (g_eeram_task_handle != NULL)
	{
		//Send Packet

		EERAM_Send_Message_Packet(ptr_packet);
	}
	else
	{
		if (EERAM_Write_data(ptr_packet) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}

	return ret;
}

/**
 * @brief      This function reads the startup configuration from EERAM and
 * 			   sends an update to the protection micro.
 * @param[in]  None
 * @retval     eeram_err_t err type
 *
 * @note:      Due to this function operating the secondary solenoid to re-establish
 *             its original state, this is only intended to be read on startup.
 */
eeram_err_t EERAM_Init_Startup_Config(void)
{
	//const char *TX_EE_TASK_TAG = "TX_BYTES";
	eeram_err_t ret = EERAM_SUCCESS;
	uint8_t startup_config = 0;
	uint8_t last_state = 0;
	uint16_t init_addr = 0;

	init_addr = EERAM_STARTUP_CONFIG_ADD;

	if (EERAM_Read_data(init_addr, &startup_config, sizeof(startup_config)) != EERAM_SUCCESS)
	{
		ret = EERAM_I2C_ERR;		//I2C ERROR
	}
	else
	{
		//memset(&gm2m_uart_comm, 0x00, sizeof(gm2m_uart_comm));
		//prepare_uart_command(&gm2m_uart_comm, SET_STARTUP_CONFIG, startup_config, NO_PAYLOAD_2, NO_PAYLOAD_2);
		//send_packet_to_m2m_uart(TX_EE_TASK_TAG, (const char*)&gm2m_uart_comm, sizeof(M2M_UART_COMMN));

		updateStartupConfig(startup_config);

		init_addr = EERAM_LAST_CONTACT_STATE_ADD;
		if (EERAM_Read_data(init_addr, &last_state, sizeof(last_state)) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR;		//I2C ERROR
		}
		else
		{
			if(startup_config == STARTUP_LAST)
			{
				if(    ((ss_status_t)last_state == SS_CLOSED)
					|| ((ss_status_t)last_state == SS_CLOSING))
				{
					TurnBreakerOn();
					//memset(&gm2m_uart_comm_ss_onoff, 0x00, sizeof(gm2m_uart_comm_ss_onoff));
					//prepare_uart_command(&gm2m_uart_comm_ss_onoff, BREAKER_OPEN_CLOSE, SECONDARY_CONTACTS_CLOSE, NO_PAYLOAD_2, NO_PAYLOAD_2);
				}
			}
		}
	}

	return ret;
}



/**
 * @brief      This function sets the startup configuration, and writes it to EERAM locations
 * @param[in]   none
 * @retval     eeram_err_t
 */
eeram_err_t EERAM_Set_Startup_Config(uint8_t startup_config)
{
	eeram_err_t ret = EERAM_SUCCESS;
	EERAM_Message_Packet dataPacket;
	EERAM_Message_Packet *ptr_packet;

	//Create packet

	EERAM_Create_Message_Packet(&dataPacket, &startup_config, EERAM_STARTUP_CONFIG_ADD, sizeof(startup_config));

	ptr_packet = &dataPacket;

	if (g_eeram_task_handle != NULL)
	{
		//Send Packet

		EERAM_Send_Message_Packet(ptr_packet);
	}
	else
	{
		if (EERAM_Write_data(ptr_packet) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}

	return ret;
}

/**
 * @brief      This function sets the last state, and writes it to EERAM locations
 * @param[in]   none
 * @retval     eeram_err_t
 */
eeram_err_t EERAM_Set_Last_State(uint8_t last_state)
{
	eeram_err_t ret = EERAM_SUCCESS;
	EERAM_Message_Packet dataPacket;
	EERAM_Message_Packet *ptr_packet;

	//Create packet

	EERAM_Create_Message_Packet(&dataPacket, &last_state, EERAM_LAST_CONTACT_STATE_ADD, sizeof(last_state));

	ptr_packet = &dataPacket;

	if (g_eeram_task_handle != NULL)
	{
		//Send Packet

		EERAM_Send_Message_Packet(ptr_packet);
	}
	else
	{
		if (EERAM_Write_data(ptr_packet) != EERAM_SUCCESS)
		{
			ret = EERAM_I2C_ERR; //I2C ERROR
		}
	}

	return ret;
}


/**
 * @brief      This function Initiate EERAM task
 * @param[in]   none
 * @retval     eeram_err_t err type
 */
eeram_err_t EERAM_Task_Init(void)
{
	eeram_err_t ret = EERAM_SUCCESS;

		//Create EERAM message queue

	if ((g_eeram_evt_queue = xQueueCreate(EERAM_MAX_QUEUE_SIZE, sizeof(EERAM_Message_Packet))) != NULL)
	{
			//Create EERAM task

		if (xTaskCreate(EERAM_Task, "EERAM_Task", EERAM_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &g_eeram_task_handle) != pdPASS)
		{
			ret = EERAM_I2C_ERR;	//I2C ERROR
		}
	}
	else
	{
		ret = EERAM_I2C_ERR;	//I2C ERROR
	}

	return ret;
}


/**
* @brief      This function Updates trending data timer for 5 min time calculation
* @param[in]  None
* @retval     None
*/
void EERAM_Update_trending_data_Timer(void)
{
	#define TRENDING_DATA_TIMER 300 //means every 5 mins
	static uint16_t TrendingDataTimer = TRENDING_DATA_TIMER;
	static bool first_read = true;

	if (TrendingDataTimer != 0)
	{
		TrendingDataTimer--;
	}

		//Skip first read to provide 1sec stabilization time for AD

	if((TrendingDataTimer == 0) || ((first_read) && (TrendingDataTimer == (TRENDING_DATA_TIMER - 2))))
	{
		Metro_Update_Cloud_data();

		if (!first_read)
		{
			TrendingDataTimer = TRENDING_DATA_TIMER;
		}
		else
		{
			first_read = false;
		}
	}
}


/**
* @brief      This function creates packet for EERAM write operation
* @param[in]  EERAM_Message_Packet *dataPacket,uint8_t* data_ptr, uint16_t address, uint16_t data_size
* @retval     None
*/
void EERAM_Create_Message_Packet(EERAM_Message_Packet *eeram_message_packet, void* dataPtr, uint16_t address, uint16_t data_size)
{
	uint8_t* byteData = (uint8_t*)dataPtr;

	//Load address

	eeram_message_packet->address = address;

	//Load data

	memcpy(&eeram_message_packet->data_array, byteData, data_size);

	//Load data size

	eeram_message_packet->data_size = data_size;

	//Load event type

	eeram_message_packet->event = EERAM_WRITE_DATA;

	//Read write status

	eeram_message_packet->read_write_status = 1;

	return;
}


/**
* @brief      This function sends packet to EERAM task for EERAM write operation
* @param[in]  struct eeram_message_packet *dataPacket pointer
* @retval     None
*/
void EERAM_Send_Message_Packet(EERAM_Message_Packet *eeram_message_packet)
{
	eTaskState task_state = 0;

	if (g_eeram_task_handle != NULL)	//Check EERAM task handle
	{
		task_state = Get_Task_State(g_eeram_task_handle);	//Get EERAM task current state

		if ((task_state != eSuspended) && (task_state != eDeleted))
		{
			//Sends queue to EERAM task

			if (g_eeram_evt_queue != NULL)
			{
				xQueueSend(g_eeram_evt_queue, eeram_message_packet, 0);
			}
		}
	}



	return;
}
/**
* @brief      This function returns the EERAM task handle status
* @param[in]  none
* @retval     task handle status
*/
TaskHandle_t EERAM_Get_Task_Handle()
{
	return g_eeram_task_handle;
}
