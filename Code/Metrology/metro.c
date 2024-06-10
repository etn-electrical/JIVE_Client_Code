/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : metro.c
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
#include "metro.h"
#include "sblcp.h"
#include "Log.h"
#include "esp_log.h"
#include "ErrorLog.h"
#include "interrupt_comp.h"
#include "metrology.h"
#include <float.h>
#include "ADE9039.h"
#include "uart_events.h"
/****************************************************************************
 *                              Global variables
 ****************************************************************************/
extern DRAM_ATTR Device_Info_str DeviceInfo;

//Stores 8 cycle data
uint16_t g_VA_Waveform[MAX_WAVEFORM_SAMPLES];
uint16_t g_VB_Waveform[MAX_WAVEFORM_SAMPLES];
uint16_t g_IA_Waveform[MAX_WAVEFORM_SAMPLES];
uint16_t g_IB_Waveform[MAX_WAVEFORM_SAMPLES];

metroData_t g_admetroData = {0};
metroData_t g_cloudTrendingData;
metro_flag_t g_metroFlag = {false};

xQueueHandle g_metro_evt_queue = NULL;
TaskHandle_t g_metro_task_handle;

extern metro_waveform_burst_data g_metroWaveformData;
extern float st_temperature;
/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
/**
 * @brief      This function initialize the metrology components
 * @param[in]  None
 * @retval     metro_err_t error type
*/
metro_err_t Metro_Init(void)
{
	metro_err_t ret = METRO_SUCCESS;

    	//initialize SPI bus

	if (Metro_SPI_Comm_Init() != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}
	else
	{
			//Reset ADE9039

		Metro_AD_Reset();

			//Initialize ADE9039

		ret = Metro_AD_Init();
	}

	return ret;
}



/**
 * @brief      Metro task to read data from ADE9039, update it to cloud
 * @param[in]  None
 * @retval     None
*/
void Metro_Task(void* arg)
{
	#define CAL_EGY_TIME 0x0005
	metro_err_t ret = METRO_SUCCESS;
	MetroEventType event = METRO_DEFAULT_EVENT;

	while(1)
	{

		if (xQueueReceive(g_metro_evt_queue, &event, pdMS_TO_TICKS(5000))) // Wait for 5 seconds for an event
		{
			switch(event)
			{
					//Executes Waveform Init fail event

				case WAVEFORM_INIT_FAIL_EVENT:
				{
					ret = METRO_SPI_ERR;	//SPI ERROR
					break;
				}

					//Executes IRQ0 events: Energy read event

				case METRO_IRQ_READ_EVENT:
				{
					ret = Metro_IRQ_Event_Execution();
					break;
				}

					//Executes Waveform full and Sign change events

				case METRO_GET_EVENT_STATUS:
				{
					ret = Metro_Event_Execution();
					break;
				}

					//Update Calibration Factors

				case METRO_SET_CALIBRATION:
				{
					if ((ret = Metro_Update_Calib_Fact()) != METRO_SUCCESS)
					{
						ret = METRO_SPI_ERR;	//SPI ERROR
					}
					else
					{
						sblcp_updateReplyMsgStatus(true);
					}
					break;
				}

					//Set metrology sample read time

				case METRO_SET_SAMPLE_READ_TIME:
				{
					if(get_sblcpManufacturingModeOnStatus())
					{
						//load count with 50ms

						if ((ret = Metro_set_Energy_Ready_time(CAL_EGY_TIME)) != METRO_SUCCESS)
						{
							ret = METRO_SPI_ERR;	//SPI ERROR
						}
					}
					else
					{
						//load count with 1sec

						if ((ret = Metro_set_Energy_Ready_time(ADE9039_EGY_TIME)) != METRO_SUCCESS)
						{
							ret = METRO_SPI_ERR;	//SPI ERROR
						}
					}

					break;
				}

				default:
				{
					break;
				}
			}

			//Checks consecutive SPI errors during run time

			ret = Metro_Error_Check(ret);
		}
        else
        {
            // No event received within 5 seconds

            printf("No metro event received within 5 seconds\n");
            ret = METRO_SPI_ERR;	//SPI ERROR
        }

		if (ret != METRO_SUCCESS)
		{
			printf("metro task fail\n");
		   	Metro_Task_Stop_Execution();
		   	Metro_ReInit(ret);
		   	Metro_Task_Delete();
		}
	}

	Metro_Task_Delete();
}


/**
 * @brief      This function delete metro task
 * @param[in]  None
 * @retval     None
*/
void Metro_Task_Delete(void)
{
	if (g_metro_task_handle != NULL)
	{
		ets_printf("Metro task delete\n");
		vTaskDelete(g_metro_task_handle); 	//delete metro task
		g_metro_task_handle = NULL;
	}
}



/**
 * @brief      This function stop the state machine, metro task
 * @param[in]  None
 * @retval     None
*/
void Metro_Task_Stop_Execution(void)
{
	Metro_AD_Stop();	//Stop ADE9039

	if (g_metro_task_handle != NULL)
	{
		gpio_intr_disable(BOARD_PIN_IRQ1);	//Disable ADE interrupts
		gpio_intr_disable(BOARD_PIN_CF3_ZA);	//Disable zero crossing interrupt

		gpio_isr_handler_remove(BOARD_PIN_IRQ1);
		gpio_isr_handler_remove(BOARD_PIN_CF3_ZA);

		if (g_metro_evt_queue != NULL)
		{
			vQueueDelete(g_metro_evt_queue);	//delete queue
			g_metro_evt_queue = NULL;		//queue = NULL
		}
	}
}



/**
 * @brief      This function reinit the metro module
 * @inval	   metro_err_t error_type
 * @retval     None
*/
void Metro_ReInit(metro_err_t ret)
{
	uint8_t reset_status = 0;

	reset_status = ReadByteFromFlash(EEOFFSET_METRO_RESET_LABEL);

	if (reset_status ==  METRO_RESET_CLEAR)
	{
		if (ret == METRO_SPI_ERR)
		{
			ErrLog(LOG_SPI_COMM_FAIL);	//On temp SPI err, log err to nvm
		}

		WriteByteToFlash(EEOFFSET_METRO_RESET_LABEL, METRO_RESET);

		if (DeviceInfo.StartReProvisioning != RE_START_PROVISIONING)
		{
			ets_printf("calling device reset\n");
			ResetDevice(DEVICE_RESET, true);
		}
	}
	else
	{
			//We need to clear it but not to reset

		if (ret == METRO_SPI_ERR)
		{
			ErrLog(LOG_SPI_COMM_ERR);	//On SPI err, log err to nvm
		}

		WriteByteToFlash(EEOFFSET_METRO_RESET_LABEL, METRO_RESET_CLEAR);
	}
}


/**
 * @brief      This function read and update the metro data
  * @param[in]  None
 * @retval     metro_err_t err type
*/
metro_err_t Metro_Data_Read(void)
{
	metro_err_t ret = METRO_SUCCESS;
	if (Metro_Get_Measures() != METRO_SUCCESS)		//Read metrology data registers
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}
	else
	{
		Metro_Update_Measures();		//Calculate and update the metro data

		if (Metro_Get_Energy() != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
		else
		{
			Metro_Update_Energy();		//Calculate and update the energy

			if (g_metroFlag.data_average_status)
			{
				ret = Metro_Calculate_Average_data();
			}
		}
	}


	return ret;
}


/**
 * @brief      This function updates the metro register data into global structure
 * @param[in]  None
 * @retval     None
*/
void Metro_Update_Measures(void)
{
	METRO_Channel_t in_Metro_Channel;
	float data = 0;
	float current_data = 0;

	for (uint8_t channel = 0; channel < METRO_MAX_CURRENT_CH; channel++)
	{
		if (channel == CHANNEL_1)
		{
			in_Metro_Channel = CHANNEL_1;
		}
		else
		{
			in_Metro_Channel = CHANNEL_2;
		}

		Metro_Read_Frequency(in_Metro_Channel, &data);	//Calculate and update frequency
		g_admetroData.linefrequency[in_Metro_Channel] = data;

		Metro_Read_Power(W_ACTIVE, in_Metro_Channel, &data);	//Calculate and update Active Power(WATT)
		g_admetroData.powerActive[in_Metro_Channel] = data;

		Metro_Read_Power(REACTIVE, in_Metro_Channel, &data);	//Calculate and update Reactive Power(VAR)
		g_admetroData.powerReactive[in_Metro_Channel] = data;

		Metro_Read_Power(APPARENT, in_Metro_Channel, &data);	//Calculate and update Apparent Power(VA)
		g_admetroData.powerApparent[in_Metro_Channel] = data;

		Metro_Read_RMS(in_Metro_Channel, &data, &current_data);	//Calculate and update RMS current(A) and voltage(Volts)
		g_admetroData.rmsvoltage[in_Metro_Channel] = data;
		g_admetroData.rmscurrent[in_Metro_Channel] = current_data;

		Metro_Read_Power_Factor(in_Metro_Channel, &data);	//Calculate and update Power Factor
		g_admetroData.powerfactor[in_Metro_Channel] = data;

		// get temperature here
		g_admetroData.st_temperature = st_temperature;


#ifdef METRO_DEBUG_PRINTS

		ESP_LOGI(TAG, "==================\nCH = %d, VRMS = %f V, IRMS = %f A, "
	   "Frequency = %f Hz, PowerFactor = %f, Pact = %f W, Preact = %f VAR, Papp = %f VA \n",
		in_Metro_Channel + 1, g_admetroData.rmsvoltage[in_Metro_Channel], g_admetroData.rmscurrent[in_Metro_Channel],
		g_admetroData.linefrequency[in_Metro_Channel],g_admetroData.powerfactor[in_Metro_Channel],
		g_admetroData.powerActive[in_Metro_Channel], g_admetroData.powerReactive[in_Metro_Channel],
		g_admetroData.powerApparent[in_Metro_Channel]);

#endif
	}	//end for


	g_admetroData.totalCurrent = g_admetroData.rmscurrent[CHANNEL_1] + g_admetroData.rmscurrent[CHANNEL_2];
}

/**
 * @brief      This function updates the Energy into global structure, EERAM and cloud structure
 * @param[in]  None
 * @retval     eeram_err_t err type
*/
void Metro_Update_Energy(void)
{
	Metro_Energy metro_energy;
	Metro_Energy* metro_energy_ptr = &metro_energy;
	EERAM_Message_Packet dataPacket;

	memset(&metro_energy, 0, sizeof(metro_energy));

	for (uint8_t channel = 0; channel < METRO_MAX_CURRENT_CH; channel++)
	{
		Metro_Read_Accumulated_Energy((METRO_Channel_t)channel);	//Read energy register values

			//Write Line energies into EERAM as per channel selection

		metro_energy.active[channel] = g_admetroData.energyActive[channel];
		metro_energy.reactive[channel] = g_admetroData.energyReactive[channel];
		metro_energy.apparent[channel] = g_admetroData.energyApparent[channel];

		metro_energy.revActive[channel] = g_admetroData.energyRevActive[channel];
		metro_energy.revReactive[channel] = g_admetroData.energyRevReactive[channel];
		metro_energy.revApparent[channel] = g_admetroData.energyRevApparent[channel];
	}

	//Create Message packet

	EERAM_Create_Message_Packet(&dataPacket, metro_energy_ptr, EERAM_PHA_FORWARD_ACT_EGY_ADD, sizeof(metro_energy));

	//Send Packet

	EERAM_Send_Message_Packet(&dataPacket);
}


/**
 * @brief      This function read energy register values
 * @param[in]  None
 * @retval     metro_err_t error type
*/
metro_err_t Metro_Get_Energy(void)
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t phase_sign = 0;

	for (uint8_t channel = 0; channel < METRO_MAX_CURRENT_CH; channel++)
	{
			//Read Energy sign

		if (Metro_Get_Energy_Sign((METRO_Channel_t)channel, &phase_sign) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
		else
		{
			g_admetroData.energySign[channel] = phase_sign;

				//Read energy reg

			if (Metro_Read_Energy_reg((METRO_Channel_t)channel) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;	//SPI ERROR
			}
		}
	}
	return ret;
}


/**
 * @brief      This function update flag and copies metrology data for cloud trending data
 * @param[in]  None
 * @retval     None
*/
void Metro_Update_Cloud_data(void)
{
	memcpy(&g_cloudTrendingData, &g_admetroData, sizeof(g_admetroData));	//Load g_cloudTrendingData structure
	Metro_Set_Trending_Data_Status(true);

#ifdef CLOUD_DATA_DEBUG_PRINTS
	printf("CH = %d, VRMS1 = %f V, IRMS1 = %f A, Frequency1 = %f Hz, PowerFactor1 = %f, Pact1 = %f W, Preact1 = %f VAR, Papp1 = %f VA, "
			"ActEnergy1 = %f kwh, ReactEnergy1 = %f kvarh, AppEnergy1 = %f kvah,"
			"RevActEnergy1 = %f kwh, RevReactEnergy1 = %f kvarh, RevAppEnergy1 = %f kvah\n",
		CHANNEL_1 + 1, g_cloudTrendingData.rmsvoltage[CHANNEL_1], g_cloudTrendingData.rmscurrent[CHANNEL_1],
		g_cloudTrendingData.linefrequency[CHANNEL_1],g_cloudTrendingData.powerfactor[CHANNEL_1],
		g_cloudTrendingData.powerActive[CHANNEL_1], g_cloudTrendingData.powerReactive[CHANNEL_1],
		g_cloudTrendingData.powerApparent[CHANNEL_1], g_cloudTrendingData.energyActive[CHANNEL_1],
		g_cloudTrendingData.energyReactive[CHANNEL_1], g_cloudTrendingData.energyApparent[CHANNEL_1],
		g_cloudTrendingData.energyRevActive[CHANNEL_1], g_cloudTrendingData.energyRevReactive[CHANNEL_1],
		g_cloudTrendingData.energyRevApparent[CHANNEL_1]);

	printf("CH = %d, VRMS2 = %f V, IRMS2 = %f A, Frequency2 = %f Hz, PowerFactor2 = %f, Pact2 = %f W, Preact2 = %f VAR, Papp2 = %f VA, "
			"ActEnergy2 = %f kwh, ReactEnergy2 = %f kvarh, AppEnergy2 = %f kvah,"
			"RevActEnergy2 = %f kwh, RevReactEnergy2 = %f kvarh, RevAppEnergy2 = %f kvah\n=================\n",
		CHANNEL_2 + 1, g_cloudTrendingData.rmsvoltage[CHANNEL_2], g_cloudTrendingData.rmscurrent[CHANNEL_2],
		g_cloudTrendingData.linefrequency[CHANNEL_2],g_cloudTrendingData.powerfactor[CHANNEL_2],
		g_cloudTrendingData.powerActive[CHANNEL_2], g_cloudTrendingData.powerReactive[CHANNEL_2],
		g_cloudTrendingData.powerApparent[CHANNEL_2], g_cloudTrendingData.energyActive[CHANNEL_2],
		g_cloudTrendingData.energyReactive[CHANNEL_2], g_cloudTrendingData.energyApparent[CHANNEL_2],
		g_cloudTrendingData.energyRevActive[CHANNEL_2], g_cloudTrendingData.energyRevReactive[CHANNEL_2],
		g_cloudTrendingData.energyRevApparent[CHANNEL_2]);
#endif
}



/**
 * @brief      This function initialize the interrupts on GPIO pins
 * @param[in]  None
 * @retval     metro_err_t return type
*/
metro_err_t Metro_GPIO_IRQ_Init(void)
{
	metro_err_t ret = METRO_SUCCESS;
	uint32_t status_reg_value = 0xffffffff;

		//Create a queue to handle gpio event from Zero Crossing isr

	if ((g_metro_evt_queue = xQueueCreate(METRO_MAX_QUEUE_SIZE, sizeof(uint8_t))) != NULL)
	{
			//Create Metro task

		if (xTaskCreate(Metro_Task, "Metro_Task", METRO_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &g_metro_task_handle) == pdPASS)
		{
				//Enable GPIO interrupts

			gpio_intr_enable(BOARD_PIN_IRQ1);
			gpio_intr_enable(BOARD_PIN_CF3_ZA);

			gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);


				//Hook isr handler for BOARD_PIN_ZCD gpio pin

			gpio_isr_handler_add(BOARD_PIN_IRQ1, EXTI_IRQ0_Handler, BOARD_PIN_IRQ1);
			gpio_isr_handler_add(BOARD_PIN_CF3_ZA, EXTI_CF3_ZA_Handler, BOARD_PIN_CF3_ZA);

				//Clear status register

			ret = Metro_Clear_Interrupt_Status(status_reg_value);
		}
		else
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}
	else
	{
		 ret = METRO_SPI_ERR;	//SPI ERROR
	}

	return ret;
}


/**
 * @brief      This function identify interrupts on IRQ0 pin and execute the events
 * @param[in]  None
 * @retval     metro_err_t error type
*/
metro_err_t Metro_IRQ_Event_Execution(void)
{
	uint32_t status0_val = 0;
	metro_err_t ret = METRO_SUCCESS;

		//Read status register and Identify, execute interrupts

	if (Metro_get_Interrupt_Status(&status0_val) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}

	if (status0_val & ENERGY_READ_EVENT)
	{
		ret = Metro_Data_Read();

		if (Metro_Clear_Interrupt_Status(ENERGY_READ_EVENT) != METRO_SUCCESS)	//Calculate and accumulate energy
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}

	return ret;
}


#ifdef AD_WFB_EN
/**
 * @brief      This function init the waveform capture mode
 * @param[in]  None
 * @retval     metro_err_t error type
*/
metro_err_t Metro_Waveform_Cap_Init(void)
{
	return Metro_Start_Waveform();
}


/**
 * @brief      This function read the waveform data
 * @param[in]  None
 * @retval     metro_err_t error type
*/
metro_err_t Metro_Wfb_Read(void)
{
	metro_err_t ret = METRO_SUCCESS;

	if (Metro_read_Waveform_data() != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}
	else
	{
		Metro_Waveform_Channel_array_fill();
	}

	return ret;
}


/**
 * @brief      This function loads Waveform buffer data into channel array
 * @param[in]  None
 * @retval     None
*/
void Metro_Waveform_Channel_array_fill(void)
{
	uint16_t waveform_buf_index = WAVEFORM_BUF_START_INDEX;	//Start index of waveform buffer
	static uint16_t wfb_sample_arr_index = 0;	//Sample array index to store 512 samples
	uint16_t wfb_sample = 0;


#ifdef DEBUG_WAVEFORM_DATA
	#define TEST_ARR_SIZE 9600
	static uint16_t test_arr_count = 0;
	static uint16_t test_arr[TEST_ARR_SIZE];
#endif

	metro_wfb_t wavefomChannel = METRO_LOAD_IA;
	waveform_sample_t waveform_sample;

	while (waveform_buf_index < MAX_BURST_READ_DATA_BYTES)
	{
			//To process waveform data as byte by byte

		for(uint8_t byte_index = 0; byte_index < SAMPLE_BYTES ; byte_index++)
		{
			waveform_sample.data[byte_index] = g_metroWaveformData.spi_read_data[waveform_buf_index];
			waveform_buf_index++;
		}

		wfb_sample = Metro_Reverse_bytes_16(waveform_sample.data_16);		//To access waveform sample data as word

			//loading into different individual waveform data array

		switch(wavefomChannel)
		{
			case METRO_LOAD_IA:
			{
					//Channel 1 Current waveform

				g_IA_Waveform[wfb_sample_arr_index] = wfb_sample;
				wavefomChannel = METRO_LOAD_VA;

				break;
			}

			case METRO_LOAD_VA:
			{
					//Channel 1 Voltage waveform

				g_VA_Waveform[wfb_sample_arr_index] = wfb_sample;
				wavefomChannel = METRO_LOAD_IB;

				break;
			}

			case METRO_LOAD_IB:
			{
				//Channel 2 Current waveform

				g_IB_Waveform[wfb_sample_arr_index] = wfb_sample;
				wavefomChannel = METRO_LOAD_VB;

#ifdef DEBUG_WAVEFORM_DATA
				test_arr[test_arr_count] = wfb_sample;
				test_arr_count ++;

				if (test_arr_count == TEST_ARR_SIZE)
				{
					for(test_arr_count = 0; test_arr_count < TEST_ARR_SIZE; test_arr_count++)
					{
						if((test_arr[test_arr_count] >> 15) & 0x0001)
						{
							test_arr[test_arr_count] = (~test_arr[test_arr_count]);
							test_arr[test_arr_count] = test_arr[test_arr_count] + 1;
							printf("  -%d, ",test_arr[test_arr_count]);
						}
						else
							printf("  %d, ",test_arr[test_arr_count]);
					}
					ets_printf("\n\n");
					test_arr_count = 0;
				}
#endif
				break;
			}

			case METRO_LOAD_VB:
			{
					//Channel 2 Voltage waveform

				g_VB_Waveform[wfb_sample_arr_index] = wfb_sample;
				wavefomChannel = METRO_LOAD_IA;
				waveform_buf_index = waveform_buf_index + 4;	//To skip reading IC and VC
				wfb_sample_arr_index ++;
				break;
			}

			default:
				break;

		} //end switch

		if (wfb_sample_arr_index == MAX_WAVEFORM_SAMPLES)
		{
			wfb_sample_arr_index = 0;
		}

	}// end while
}
#endif



/**
 * @brief      This function Read active, reactive and apparent energy according to the selected type for the given channel
 * @param[in]   uint8_t channel
 * @retval      None
 */
void Metro_Read_Accumulated_Energy(METRO_Channel_t in_Metro_Channel)
{
	double act_cal_energy = 0;
	double react_cal_energy = 0;
	double app_cal_energy = 0;

	Metro_Read_Energy(W_ACTIVE, in_Metro_Channel, &act_cal_energy);	//active energy
	Metro_Read_Energy(REACTIVE, in_Metro_Channel, &react_cal_energy);	//reactive energy
	Metro_Read_Energy(APPARENT, in_Metro_Channel, &app_cal_energy);	//Apparent energy

		//Accumulate energy

	if (g_admetroData.energySign[in_Metro_Channel] == POSITIVE)
	{
			//Reset Active Energy to zero if it reaches to max value

		if ((g_admetroData.energyActive[in_Metro_Channel] + act_cal_energy) > DBL_MAX)
		{
			ets_printf("Resetting CH %d Act Energy\n",in_Metro_Channel);
			g_admetroData.energyActive[in_Metro_Channel] = 0;
		}

			//Reset Reactive Energy to zero if it reaches to max value

		if ((g_admetroData.energyReactive[in_Metro_Channel] + react_cal_energy) > DBL_MAX)
		{
			ets_printf("Resetting CH %d Reactive Energy\n",in_Metro_Channel);
			g_admetroData.energyReactive[in_Metro_Channel] = 0;
		}

			//Reset Apparent Energy to zero if it reaches to max value

		if ((g_admetroData.energyApparent[in_Metro_Channel] + app_cal_energy) > DBL_MAX)
		{
			ets_printf("Resetting CH %d App Energy\n",in_Metro_Channel);
			g_admetroData.energyApparent[in_Metro_Channel] = 0;
		}

		g_admetroData.energyActive[in_Metro_Channel] = g_admetroData.energyActive[in_Metro_Channel] + act_cal_energy;
		g_admetroData.energyReactive[in_Metro_Channel] = g_admetroData.energyReactive[in_Metro_Channel] + react_cal_energy;
		g_admetroData.energyApparent[in_Metro_Channel] = g_admetroData.energyApparent[in_Metro_Channel] + app_cal_energy;

	}
	else
	{
			//Reset Rev Active Energy to zero if it reaches to max value

		if ((g_admetroData.energyRevActive[in_Metro_Channel] + act_cal_energy) > DBL_MAX)
		{
			ets_printf("Resetting CH %d Rev Act Energy\n",in_Metro_Channel);
			g_admetroData.energyRevActive[in_Metro_Channel] = 0;
		}

			//Reset Rev Reactive Energy to zero if it reaches to max value

		if ((g_admetroData.energyRevReactive[in_Metro_Channel] + react_cal_energy) > DBL_MAX)
		{
			ets_printf("Resetting CH %d Rev Reactive Energy\n",in_Metro_Channel);
			g_admetroData.energyRevReactive[in_Metro_Channel] = 0;
		}

			//Reset Rev Apparent Energy to zero if it reaches to max value

		if ((g_admetroData.energyRevApparent[in_Metro_Channel] + app_cal_energy) > DBL_MAX)
		{
			ets_printf("Resetting CH %d Rev App Energy\n",in_Metro_Channel);
			g_admetroData.energyRevApparent[in_Metro_Channel] = 0;
		}

		g_admetroData.energyRevActive[in_Metro_Channel] = g_admetroData.energyRevActive[in_Metro_Channel] + act_cal_energy;
		g_admetroData.energyRevReactive[in_Metro_Channel] = g_admetroData.energyRevReactive[in_Metro_Channel] + react_cal_energy;
		g_admetroData.energyRevApparent[in_Metro_Channel] = g_admetroData.energyRevApparent[in_Metro_Channel] + app_cal_energy;
	}		//if ends

#ifdef METRO_ENERGY_ACC_DEBUG
	if (in_Metro_Channel == CHANNEL_2)
	{
		ESP_LOGI(TAG, " FW energy-- Aact: %f, Areact: %f, Aapp: %f, Bact: %f, Breact: %f, Bapp: %f \n"
				"RW energy-- Act: %f, Areact: %f, Aapp: %f, Bact: %f, Breact: %f, Bapp: %f \n==========================\n",
				g_admetroData.energyActive[0], g_admetroData.energyReactive[0], g_admetroData.energyApparent[0],
				g_admetroData.energyActive[1], g_admetroData.energyReactive[1], g_admetroData.energyApparent[1],
				g_admetroData.energyRevActive[0], g_admetroData.energyRevReactive[0], g_admetroData.energyRevApparent[0],
				g_admetroData.energyRevActive[1], g_admetroData.energyRevReactive[1], g_admetroData.energyRevApparent[1]);

	}
#endif
}



/**
* @brief      This function copies the telemetry data for SBLCP. it is called from SBLCP
* @param[in]   [meterTelemetryData_st *metroData] pointer of structure to which telemetry data is copied
* @retval      none
*/
void Metro_Get_Telemetry_data(meterTelemetryData_st *metroData)
{
	if(metroData != NULL)
	{
		memset(metroData, 0x00, sizeof(meterTelemetryData_st));
		metroData->phase_A_line_frequency = g_admetroData.linefrequency[CHANNEL_1];
		metroData->phase_A_rms_voltage = g_admetroData.rmsvoltage[CHANNEL_1];
		metroData->phase_A_rms_current = g_admetroData.rmscurrent[CHANNEL_1];
		metroData->phase_A_power_factor = g_admetroData.powerfactor[CHANNEL_1];
		metroData->phase_A_active_power = g_admetroData.powerActive[CHANNEL_1];
		metroData->phase_A_reactive_power = g_admetroData.powerReactive[CHANNEL_1];
		metroData->phase_A_app_power = g_admetroData.powerApparent[CHANNEL_1];
		metroData->phase_A_active_energy = g_admetroData.energyActive[CHANNEL_1];
		metroData->phase_A_reactive_energy = g_admetroData.energyReactive[CHANNEL_1];
		metroData->phase_A_app_energy = g_admetroData.energyApparent[CHANNEL_1];
		metroData->phase_A_reverse_active_energy = g_admetroData.energyRevActive[CHANNEL_1];
		metroData->phase_A_reverse_reactive_energy = g_admetroData.energyRevReactive[CHANNEL_1];
		metroData->phase_A_reverse_app_energy = g_admetroData.energyRevApparent[CHANNEL_1];

		metroData->phase_B_line_frequency = g_admetroData.linefrequency[CHANNEL_2];
		metroData->phase_B_rms_voltage = g_admetroData.rmsvoltage[CHANNEL_2];
		metroData->phase_B_rms_current = g_admetroData.rmscurrent[CHANNEL_2];
		metroData->phase_B_power_factor = g_admetroData.powerfactor[CHANNEL_2];
		metroData->phase_B_active_power = g_admetroData.powerActive[CHANNEL_2];
		metroData->phase_B_reactive_power = g_admetroData.powerReactive[CHANNEL_2];
		metroData->phase_B_app_power = g_admetroData.powerApparent[CHANNEL_2];
		metroData->phase_B_active_energy = g_admetroData.energyActive[CHANNEL_2];
		metroData->phase_B_reactive_energy = g_admetroData.energyReactive[CHANNEL_2];
		metroData->phase_B_app_energy = g_admetroData.energyApparent[CHANNEL_2];
		metroData->phase_B_reverse_active_energy = g_admetroData.energyRevActive[CHANNEL_2];
		metroData->phase_B_reverse_reactive_energy = g_admetroData.energyRevReactive[CHANNEL_2];
		metroData->phase_B_reverse_app_energy = g_admetroData.energyRevApparent[CHANNEL_2];
	}
}



/**
* @brief      This function send event to set the flag to for metro module
* 			  to start averaging of telemetry data.This function called from SBLCP,
* @param[in]   none
* @retval      none
*/
void Metro_start_data_average(void)
{
		//set flag to start reading meteorology data every	50 msec

	g_metroFlag.data_average_status = true;
	uint8_t MetroEvent = METRO_IRQ_READ_EVENT;

	if (g_metro_evt_queue != NULL)
	{
		xQueueSend(g_metro_evt_queue, &MetroEvent, portMAX_DELAY);
	}
}


/**
* @brief      Function sets set an event for metro task to set new calibration from ADE chip
*                 received over SBLCP
* @param[in]   1. calibration parameters received over SBLCP
* @retval      none
*/
void Metro_Set_Breaker_Calibration_Factors(uint8_t *rxBuff)
{
	uint8_t MetroEvent = METRO_SET_CALIBRATION;
	metro_calib_struct calib_param;

	memcpy(&calib_param, rxBuff, sizeof(metro_calib_struct));
	nvs_write_metro_calib(calib_param);

	if (g_metro_evt_queue != NULL)
	{
		xQueueSend(g_metro_evt_queue, &MetroEvent, portMAX_DELAY);
	}
}


/**
 * @brief      This function reset metro_data structure
 * @param[in]  None
 * @retval     None
*/
void Metro_Data_Reset(void)
{
		//Reset metro strcuture

    memset (&g_admetroData, 0x00, sizeof (g_admetroData));

    	//Reset cloud trending data structure

    memset (&g_cloudTrendingData, 0x00, sizeof (g_cloudTrendingData));
}


/**
 * @brief      This function loads default calibration into NVS
 * @param[in]  None
 * @retval     None
*/
void Metro_Load_Default_Calibration(void)
{
	metro_calib_struct metro_default_calib = {
		.avGain = ADE9039_AVGAIN,
		.aiGain = ADE9039_AIGAIN,
		.aiRmsOs = ADE9039_AIRMSOS,
		.apGain = ADE9039_APGAIN,
		.aphCal_0 = ADE9039_APHCAL0,

		.bvGain = ADE9039_BVGAIN,
		.biGain = ADE9039_BIGAIN,
		.biRmsOs = ADE9039_BIRMSOS,
		.bpGain = ADE9039_BPGAIN,
		.bphCal_0 = ADE9039_BPHCAL0,
	};

	nvs_write_metro_calib(metro_default_calib);
}


/**
 * @brief      This function is designed to read metro data and calculate average
 * @param[in]  None
 * @retval     metro_err_t err_type
*/
metro_err_t Metro_Calculate_Average_data(void)
{
	metro_err_t ret = METRO_SUCCESS;
	METRO_Channel_t in_Metro_Channel;
	static metroData_t average_data;
	static uint8_t sample_count = 0;

	if (sample_count == 0)
	{
		memset(&average_data, 0x0000, sizeof(average_data));
	}
	if (sample_count < METRO_MAX_AVG_SAMPLES)
	{
		sample_count++;

		for (uint8_t channel = 0; channel < METRO_MAX_CURRENT_CH; channel++)
		{
			if (channel == CHANNEL_1)
			{
				in_Metro_Channel = CHANNEL_1;
			}
			else
			{
				in_Metro_Channel = CHANNEL_2;
			}

			average_data.linefrequency[in_Metro_Channel] += g_admetroData.linefrequency[in_Metro_Channel];
			average_data.rmsvoltage[in_Metro_Channel] += g_admetroData.rmsvoltage[in_Metro_Channel];
			average_data.rmscurrent[in_Metro_Channel] += g_admetroData.rmscurrent[in_Metro_Channel];
			average_data.powerfactor[in_Metro_Channel] += g_admetroData.powerfactor[in_Metro_Channel];
			average_data.powerActive[in_Metro_Channel] += g_admetroData.powerActive[in_Metro_Channel];
			average_data.powerReactive[in_Metro_Channel] += g_admetroData.powerReactive[in_Metro_Channel];
			average_data.powerApparent[in_Metro_Channel] += g_admetroData.powerApparent[in_Metro_Channel];


			if (sample_count == METRO_MAX_AVG_SAMPLES)
			{
				average_data.linefrequency[in_Metro_Channel] = average_data.linefrequency[in_Metro_Channel] / METRO_MAX_AVG_SAMPLES;
				average_data.rmsvoltage[in_Metro_Channel] = average_data.rmsvoltage[in_Metro_Channel] / METRO_MAX_AVG_SAMPLES;
				average_data.rmscurrent[in_Metro_Channel] = average_data.rmscurrent[in_Metro_Channel] / METRO_MAX_AVG_SAMPLES;
				average_data.powerfactor[in_Metro_Channel] = average_data.powerfactor[in_Metro_Channel] / METRO_MAX_AVG_SAMPLES;
				average_data.powerActive[in_Metro_Channel] = average_data.powerActive[in_Metro_Channel] / METRO_MAX_AVG_SAMPLES;
				average_data.powerReactive[in_Metro_Channel] = average_data.powerReactive[in_Metro_Channel] / METRO_MAX_AVG_SAMPLES;
				average_data.powerApparent[in_Metro_Channel] = average_data.powerApparent[in_Metro_Channel] / METRO_MAX_AVG_SAMPLES;


				if (in_Metro_Channel == CHANNEL_2)
				{
					if (Metro_Get_Energy() != METRO_SUCCESS)
					{
						ret = METRO_SPI_ERR;	//SPI ERROR
					}
					else
					{
						Metro_Update_Energy();		//Calculate and update the energy
					}

					sample_count = 0;
					g_metroFlag.data_average_status = false;
				}
			}
		}
	}

	if(!g_metroFlag.data_average_status)
	{
		meterTelemetryData_st SBLCP_telemetry_data;

		memset(&SBLCP_telemetry_data, 0x00, sizeof(meterTelemetryData_st));
		SBLCP_telemetry_data.phase_A_line_frequency = average_data.linefrequency[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_rms_voltage = average_data.rmsvoltage[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_rms_current = average_data.rmscurrent[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_power_factor = average_data.powerfactor[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_active_power = average_data.powerActive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reactive_power = average_data.powerReactive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_app_power = average_data.powerApparent[CHANNEL_1];

			//Copy energies for phase A

		SBLCP_telemetry_data.phase_A_active_energy = g_admetroData.energyActive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reactive_energy = g_admetroData.energyReactive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_app_energy = g_admetroData.energyApparent[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reverse_active_energy = g_admetroData.energyRevActive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reverse_reactive_energy = g_admetroData.energyRevReactive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reverse_app_energy = g_admetroData.energyRevApparent[CHANNEL_1];

		SBLCP_telemetry_data.phase_B_line_frequency = average_data.linefrequency[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_rms_voltage = average_data.rmsvoltage[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_rms_current = average_data.rmscurrent[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_power_factor = average_data.powerfactor[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_active_power = average_data.powerActive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reactive_power = average_data.powerReactive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_app_power = average_data.powerApparent[CHANNEL_2];

			//Copy energies for Phase B

		SBLCP_telemetry_data.phase_B_active_energy = g_admetroData.energyActive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reactive_energy = g_admetroData.energyReactive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_app_energy = g_admetroData.energyApparent[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reverse_active_energy = g_admetroData.energyRevActive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reverse_reactive_energy = g_admetroData.energyRevReactive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reverse_app_energy = g_admetroData.energyRevApparent[CHANNEL_2];

		SBLCP_getSetAvgTeleData(&SBLCP_telemetry_data);
	}

	return ret;
}


/**
 * @brief      This function is designed to check consecutive SPI error
 * @param[in]  metro_err_t return type
 * @retval     metro_err_t err_type
*/
metro_err_t Metro_Error_Check(metro_err_t ret)
{
	static uint8_t spi_error_counter = 0;

		//Checks consecutive errors and maintain error counter

	if (ret != METRO_SUCCESS)
	{
		printf("Communication fail...\n");

		if (ret == METRO_SPI_ERR)
		{
			spi_error_counter++;
		}
	}
	else
	{
		spi_error_counter = 0;
	}

		//If error counter reaches to MAX error count it returns SPI error

	if (spi_error_counter == MAX_CONSECUTIVE_SPI_ERROR)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}
	else
	{
		ret = METRO_SUCCESS;
	}

	return ret;
}



/**
 * @brief      This function update forward and reverse energies which read from EERAM for channel 1 and 2 into global structure
 * @param[in]  struct energy *metro_energy_ptr
 * @retval     eeram_err_t err type
 */
void Metro_Update_Energy_data(Metro_Energy *metro_energy_ptr)
{
	for (uint8_t channel = 0; channel < METRO_MAX_CURRENT_CH; channel++)
	{
		g_admetroData.energyActive[channel] = metro_energy_ptr->active[channel];
		g_admetroData.energyReactive[channel] = metro_energy_ptr->reactive[channel];
		g_admetroData.energyApparent[channel] = metro_energy_ptr->apparent[channel];
		g_admetroData.energyRevActive[channel] = metro_energy_ptr->revActive[channel];
		g_admetroData.energyRevReactive[channel] = metro_energy_ptr->revReactive[channel];
		g_admetroData.energyRevApparent[channel] = metro_energy_ptr->revApparent[channel];

		printf("\n-------CH%d-------\nFW Act: %f\nFW react: %f\nFW App: %f\nRW Act: %f\nRW react: %f\nRW App: %f\n",
				channel + 1, g_admetroData.energyActive[channel], g_admetroData.energyReactive[channel],
				g_admetroData.energyApparent[channel], g_admetroData.energyRevActive[channel],
				g_admetroData.energyRevReactive[channel], g_admetroData.energyRevApparent[channel]);
	}
}


/**
* @brief      Function sets sample read time for metro data
* @param[in]   None
* @retval      none
*/
void Metro_Set_Sample_Read_time(void)
{
	uint8_t MetroEvent;

	MetroEvent = METRO_SET_SAMPLE_READ_TIME;

	if (g_metro_evt_queue != NULL)
	{
		xQueueSendToBack(g_metro_evt_queue, &MetroEvent, portMAX_DELAY);
	}
}



/**
* @brief      Function get the trending data status if it is ready or not
* @param[in]   None
* @retval      bool true/false
*/
bool Metro_Get_Trending_Data_Status(void)
{
	if (g_metroFlag.update_trending_data)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/**
* @brief      Function set/change the trending data status after read
* @param[in]   bool true/false
* @retval     None
*/
void Metro_Set_Trending_Data_Status(bool trending_data_ready)
{
	if (!trending_data_ready)
	{
		g_metroFlag.update_trending_data = false;
	}
	else
	{
		g_metroFlag.update_trending_data = true;
	}
}

/**
* @brief      calculates the percentage load
* @param[in]  none
* @retval     None
*/
void Metro_Get_Percentage_Load(float *percentLoad)
{
	float percent_load;
	uint8_t current_rating;
	current_rating = DeviceInfo.current_rating;

	g_admetroData.totalCurrent = g_admetroData.rmscurrent[CHANNEL_1] + g_admetroData.rmscurrent[CHANNEL_2];

	//calculates the percentage load on total current
	percent_load =  (g_admetroData.totalCurrent / (float)(METRO_MAX_CURRENT_CH * current_rating)) * 100;
	memcpy(percentLoad, &percent_load, sizeof(float));
}
/**
* @brief      This function execute metro events based on SW polling
* @param[in]   None
* @retval     metro_err_t err type
*/
metro_err_t Metro_Event_Execution()
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t phase_sign = 0;
	uint32_t status0_val = 0;

		//Read status register

	if (Metro_get_Interrupt_Status(&status0_val) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}

	//--------------------------------------------------------------------------------------------------

		//Waveform full event

	if (status0_val & WAVEFORM_FULL_EVENT)
	{
			//Waveform page full event

		if (g_metroFlag.waveform_enable)
		{
			if ((ret = Metro_Wfb_Read()) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;	//SPI ERROR
			}
		}
		if (Metro_Clear_Interrupt_Status(WAVEFORM_FULL_EVENT) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}

	//--------------------------------------------------------------------------------------------------

		//Channel 1 energy sign change event

	if (status0_val & CH1_EGY_SIGN_CHANGE_EVENT)
	{
		if (Metro_Read_Energy_reg(CHANNEL_1) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
		else
		{
			Metro_Read_Accumulated_Energy(CHANNEL_1);		//Calculate and accumulate energy

			if ((ret = Metro_Get_Energy_Sign(CHANNEL_1, &phase_sign)) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;	//SPI ERROR
			}
			else
			{
				g_admetroData.energySign[CHANNEL_1] = phase_sign;
			}
		}

		if (Metro_Clear_Interrupt_Status(CH1_EGY_SIGN_CHANGE_EVENT) != METRO_SUCCESS)	//Calculate and accumulate energy
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}		//if ends

	//-------------------------------------------------------------------------------------------------------------

		//Channel 2 energy sign change event

	if (status0_val & CH2_EGY_SIGN_CHANGE_EVENT)
	{
		if (Metro_Read_Energy_reg(CHANNEL_2) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
		else
		{
			Metro_Read_Accumulated_Energy(CHANNEL_2);		//Calculate and accumulate energy

			if (Metro_Get_Energy_Sign(CHANNEL_2, &phase_sign) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;	//SPI ERROR
			}
			else
			{
				g_admetroData.energySign[CHANNEL_2] = phase_sign;
			}
		}		//if ends

		if (Metro_Clear_Interrupt_Status(CH2_EGY_SIGN_CHANGE_EVENT) != METRO_SUCCESS)	//Calculate and accumulate energy
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}		//if ends

	//--------------------------------------------------------------------------------------------------

		//Energy read event

	if (status0_val & ENERGY_READ_EVENT)
	{
		ret = Metro_Data_Read();

		if (Metro_Clear_Interrupt_Status(ENERGY_READ_EVENT) != METRO_SUCCESS)	//Calculate and accumulate energy
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}

	return ret;
}



/**
* @brief      This function is designed to get status of events by reading status register
* @param[in]  None
* @retval     None
*/
void Metro_Get_Event_Status()
{
	MetroEventType MetroEvent = METRO_GET_EVENT_STATUS;

	if (g_metro_evt_queue != NULL)
	{
		xQueueSend(g_metro_evt_queue, &MetroEvent, portMAX_DELAY);
	}
}
