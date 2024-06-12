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
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/18/2022
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
#include "ADE9000.h"
/****************************************************************************
 *                              Global variables
 ****************************************************************************/

//Rahul:- following calibration and configuration structure are temporary
// need to replace with final calibration and configuration structure once finalised
metro_config_st g_stMetro_config;

extern DRAM_ATTR Device_Info_str DeviceInfo;
extern sblcpStatusInfo_st replyStatusInfo;

//Stores 8 cycle data
uint16_t VA_Waveform[MAX_WAVEFORM_SAMPLES];
uint16_t VB_Waveform[MAX_WAVEFORM_SAMPLES];
uint16_t IA_Waveform[MAX_WAVEFORM_SAMPLES];
uint16_t IB_Waveform[MAX_WAVEFORM_SAMPLES];

metroData_t g_admetroData;
metroData_t g_cloudTrendingData;
metro_flag_t g_metroFlag = {false, false, false, false, false, false, false, false};

xQueueHandle Metro_evt_queue = NULL;
TaskHandle_t Metro_task_handle;

static const char* TAG = "Metro tag";
extern metro_waveform_burst_data g_metroWaveformData;

/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
/**
 * @brief      This function is designed to initialize the metrology components
 * @param[in]  None
 * @retval     int8_t error type
*/
uint8_t Metro_Init()
{
	uint8_t ret = METRO_SUCCESS;

		//initialize SPI bus

	if ((ret = Metro_SPI_Comm_Init()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
			//Reset ADE9039

		Metro_AD_Reset();

			//Initialze ADE9039

		if ((ret = Metro_AD_Init()) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
			printf("AD\n");
		}
	}

	return ret;
}



/**
 * @brief      This function run the state machine with stated state and update the state
 * @retval     None
*/
void Metro_Task(void* arg)
{
	uint8_t event;
	uint8_t ret = METRO_SUCCESS;
	static bool metro_first_read = false;

	while(1)
	{
		if (xQueueReceive(Metro_evt_queue, &event, portMAX_DELAY))		//Receive Metro event queue
		{
			switch(event)
			{
					//Executes Waveform Init fail event

				case WAVEFORM_INIT_FAIL_EVENT:
				{
					ret = METRO_SPI_ERR;
					break;
				}

					//Executes IRQ events

				case METRO_IRQ_READ_EVENT:
				{
					if ((ret = Metro_Event_Execution()) != METRO_SUCCESS)
					{
						ret = METRO_SPI_ERR;
					}
					break;
				}

					//Executes Metrology read event every sec

				case METRO_READ_EVENT:
				{
					if (!g_metroFlag.waveform_cap_status)
					{
						if ((ret = Metro_Data_Read()) != METRO_SUCCESS)
						{
							ret = METRO_SPI_ERR;
						}
						else
						{
							if (!metro_first_read)
							{
								memcpy(&g_cloudTrendingData, &g_admetroData, sizeof(g_admetroData));
								g_metroFlag.update_trending_data = true;
								metro_first_read = true;
							}
						}
					}

					break;
				}

				case METRO_SET_CALIBRATION:
				{
					if ((ret = Metro_Update_Calib_Fact()) != METRO_SUCCESS)
					{
						ret = METRO_SPI_ERR;
					}
					else
					{
						replyStatusInfo.status_updated = true;
					}

					break;
				}

				default:
				{
					ret = METRO_SPI_ERR;
					break;
				}
			}

			if (ret != METRO_SUCCESS)
			{
				printf("metro task fail\n");
			   	Metro_Task_Stop_Execution();
			   	Metro_Task_ReInit();
			   	Metro_Task_Delete();
			}
		}
	}

	Metro_Task_Delete();
}


/**
 * @brief      This function delete metro task
 * @param[in]  None
 * @retval     None
*/
void Metro_Task_Delete()
{
	if (Metro_task_handle != NULL)
	{
		ets_printf("Metro task delete\n");
		vTaskDelete(Metro_task_handle); 	//delete metro task
		Metro_task_handle = NULL;
	}
}

/**
 * @brief      This function stop the state machine, metro task
 * @retval     None
*/
void Metro_Task_Stop_Execution()
{
	ErrLog(LOG_METRO_SPI_ERROR);	//On spi err, log err to nvm

	Metro_AD_Stop();	//Stop ADE9039

	if (Metro_task_handle != NULL)
	{
		gpio_intr_disable(BOARD_PIN_IRQ1);	//Disable ADE interrupts
		gpio_intr_disable(BOARD_PIN_CF3_ZA);	//Disable zero crossing interrupt

		gpio_isr_handler_remove(BOARD_PIN_IRQ1);
		gpio_isr_handler_remove(BOARD_PIN_CF3_ZA);

		if (Metro_evt_queue != NULL)
		{
			vQueueDelete(Metro_evt_queue);	//delete queue
			Metro_evt_queue = NULL;		//queue = NULL
		}
	}
}


/**
 * @brief      This function reinit the metro task
 * @retval     None
*/
void Metro_Task_ReInit()
{
	uint8_t ret;

	ret = ReadByteFromFlash(EEOFFSET_METRO_RESET_LABEL);

	if (ret ==  METRO_RESET_CLEAR)
	{
		WriteByteToFlash(EEOFFSET_METRO_RESET_LABEL, METRO_RESET);

		ets_printf("calling device reset\n");
		ResetDevice(DEVICE_RESET, true);
	}
	else
	{
			//We need to clear it but not to reset

		WriteByteToFlash(EEOFFSET_METRO_RESET_LABEL, METRO_RESET_CLEAR);
	}
}


/**
 * @brief      This function is designed to read the registers and store in the structure
 * @param[in]  None
 * @retval     uint8_t err type
*/
uint8_t Metro_Get_Measures()
{
	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_read_data_regs()) != METRO_SUCCESS)		//Read required metro data registers
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function read and update the metro data
 * @retval     int8_t err type
*/
uint8_t Metro_Data_Read()
{
#ifdef METRO_DEBUG_PRINTS
	//	ESP_LOGI(TAG,"metro\n");
#endif
	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_Get_Measures()) != METRO_SUCCESS)		//Read metrology data registers
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		Metro_Update_Measures();		//Calculate and update the metro data
	}
	if (g_metroFlag.read_Energy)
	{
		if ((ret = Metro_Get_Energy()) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
		else
		{
			Metro_Update_Energy();		//Calculate and update the energy
		}
	}

	if (g_metroFlag.data_average_status)
	{
		if ((ret = Metro_Calculate_Average_data()) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}

	}

	return ret;
}


/**
 * @brief      This function is designed to update the metro register data into global structure
 * @param[in]  None
 * @retval     None
*/
void Metro_Update_Measures()
{
	METRO_Channel_t in_Metro_Channel;

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

		Metro_Read_Frequency(in_Metro_Channel, &g_admetroData.linefrequency[in_Metro_Channel]);	//Calculate and update frequency

		Metro_Read_Power(W_ACTIVE, in_Metro_Channel, &g_admetroData.powerActive[in_Metro_Channel]);	//Calculate and update Active Power(WATT)
		Metro_Read_Power(REACTIVE, in_Metro_Channel, &g_admetroData.powerReactive[in_Metro_Channel]);	//Calculate and update Reactive Power(VAR)
		Metro_Read_Power(APPARENT, in_Metro_Channel, &g_admetroData.powerApparent[in_Metro_Channel]);	//Calculate and update Apparent Power(VA)

		Metro_Read_RMS(in_Metro_Channel, &g_admetroData.rmsvoltage[in_Metro_Channel], &g_admetroData.rmscurrent[in_Metro_Channel]);	//Calculate and update RMS current(A) and voltage(Volts)

		Metro_Read_Power_Factor(in_Metro_Channel, &g_admetroData.powerfactor[in_Metro_Channel]);	//Calculate and update Power Factor

#ifdef METRO_DEBUG_PRINTS

		ESP_LOGI("==================\nCloud data : CH = %d, VRMS = %f V, IRMS = %f A, "
	   "Frequency = %f Hz, PowerFactor = %f, Pact = %f W, Preact = %f VAR, Papp = %f VA \n",
		in_Metro_Channel + 1, g_admetroData.rmsvoltage[in_Metro_Channel], g_admetroData.rmscurrent[in_Metro_Channel],
		g_admetroData.linefrequency[in_Metro_Channel],g_admetroData.powerfactor[in_Metro_Channel],
		g_admetroData.powerActive[in_Metro_Channel], g_admetroData.powerReactive[in_Metro_Channel],
		g_admetroData.powerApparent[in_Metro_Channel]);

#endif

	}	//end for
}


/**
 * @brief      This function is designed to update the Energy into global structure, NVS and cloud structure
 * @param[in]  None
 * @retval     None
*/
void Metro_Update_Energy()
{
	uint8_t ret = METRO_SUCCESS;
	METRO_Channel_t in_Metro_Channel;

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

			//Read energy every 5 sec

		if (g_metroFlag.read_Energy)
		{
			Metro_Read_Accumulated_Energy(in_Metro_Channel);	//Read energy registers

			if (in_Metro_Channel == CHANNEL_2)
			{
				g_metroFlag.read_Energy = false;	//Reset energy interrupt flag
			}
		}

			//Write energies into NVS

		if ((g_metroFlag.energy_nvs_write) && (in_Metro_Channel == CHANNEL_2))
		{
				//TBD: Rahul need to add control action for error
			ets_printf("Metro NVS write\n");
			Metro_Energy_NVS_Write();	//Write energies into NVS
			g_metroFlag.energy_nvs_write = false;	//Reset energy update flag
		}

			//Update metrology data to cloud every 5 mins

		if ((g_metroFlag.trending_data_timer) && (in_Metro_Channel == CHANNEL_2))
		{
			g_metroFlag.trending_data_timer = false;	//Reset metro trending data flag
			g_metroFlag.update_trending_data = true;	//Set update cloud trending data flag
			memcpy(&g_cloudTrendingData, &g_admetroData, sizeof(g_admetroData));	//Load g_cloudTrendingData structure
		}
	}
}


/**
 * @brief      This function initialize the interrupts on GPIO pins
 * @retval     None
*/
uint8_t Metro_GPIO_IRQ_Init(void)
{
#ifdef METRO_DEBUG_PRINTS
	ESP_LOGI(TAG,"IRQ unit\n");
#endif
	uint8_t ret = METRO_SUCCESS;
	BaseType_t xReturned = pdFAIL;

		//Create a queue to handle gpio event from Zero Crossing isr

	if ((Metro_evt_queue = xQueueCreate(METRO_MAX_QUEUE_SIZE, sizeof(uint8_t))) != NULL)
	{

		if ((xReturned = xTaskCreate(Metro_Task, "Metro_Task", METRO_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &Metro_task_handle)) == pdPASS)	//Create Metro task
		{
				//Enable GPIO interrupts

			gpio_intr_enable(BOARD_PIN_IRQ1);
			gpio_intr_enable(BOARD_PIN_CF3_ZA);

			gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);


				//Hook isr handler for BOARD_PIN_ZCD gpio pin

			gpio_isr_handler_add(BOARD_PIN_IRQ1, EXTI_IRQ0_Handler, BOARD_PIN_IRQ1);
			gpio_isr_handler_add(BOARD_PIN_CF3_ZA, EXTI_CF3_ZA_Handler, BOARD_PIN_CF3_ZA);

				//Clear status register

			if ((ret = Metro_Clear_Interrupt_Status()) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;
			}
		}
		else
		{
			ret = METRO_SPI_ERR;
		}
	}
	else
	{
		ret = METRO_SPI_ERR;
	}
//	ret = METRO_SUCCESS;
	return ret;
}


/**
 * @brief      This function is designed to identify interrupts on IRQ0 pin and execute the events
 * @param[in]  None
 * @retval     int8_t error type
*/
uint8_t Metro_Event_Execution()
{
	uint32_t status0_val = 0;
	METRO_Channel_t in_Metro_Channel;
	uint8_t ret = METRO_SUCCESS;

		//Read status register

	if ((ret = Metro_get_Interrupt_Status(&status0_val)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	else
	{
#ifdef AD_WFB_EN

			//Identify and execute interrupts

		if ((status0_val & WAVEFORM_FULL_EVENT) && (g_metroFlag.waveform_enable))	//Waveform page full event
		{
			g_metroFlag.waveform_cap_status = true;

			if ((ret = Metro_Wfb_Read()) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;
			}

			g_metroFlag.waveform_cap_status = false;
		}
#endif
		if ((!g_metroFlag.waveform_cap_status) && (ret == METRO_SUCCESS))
		{

			if (status0_val & CH1_EGY_SIGN_CHANGE_EVENT)		//Channel 1 energy sign change event
			{
#ifdef METRO_DEBUG_PRINTS
				ESP_LOGI(TAG,"energy change event for channel 1\n");
#endif
				in_Metro_Channel = CHANNEL_1;

				if ((ret = Metro_Read_Energy_reg(in_Metro_Channel)) != METRO_SUCCESS)
				{
					ret = METRO_SPI_ERR;
				}

				else
				{
					Metro_Read_Accumulated_Energy(in_Metro_Channel);		//Calculate and accumulate energy
				}
			}		//if ends

			if ((status0_val & CH2_EGY_SIGN_CHANGE_EVENT) && (ret == METRO_SUCCESS))	//Channel 2 energy sign change event
			{

#ifdef METRO_DEBUG_PRINTS
				ESP_LOGI(TAG,"energy change event for channel 2\n");
#endif

				in_Metro_Channel = CHANNEL_2;

				if ((ret = Metro_Read_Energy_reg(in_Metro_Channel)) != METRO_SUCCESS)
				{
					ret = METRO_SPI_ERR;
				}
				else
				{
					Metro_Read_Accumulated_Energy(in_Metro_Channel);		//Calculate and accumulate energy
				}		//if ends
			}		//if ends
		}

		if ((ret = Metro_Clear_Interrupt_Status()) != METRO_SUCCESS)	//Calculate and accumulate energy
		{
			ret = METRO_SPI_ERR;
		}
	}		//if ends

	return ret;
}

#ifdef AD_WFB_EN
/**
 * @brief      This function init the waveform capture mode
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_Waveform_Cap_Init()
{
	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_Start_Waveform()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function read the waveform data
 * @param[in]  None
 * @retval     int8_t error type
*/
uint8_t Metro_Wfb_Read()
{

	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_read_Waveform_data()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
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
/**
 * @brief      This function loads Waveform buffer data into channel array
 * @param[in]  None
 * @retval     None
*/
void Metro_Waveform_Channel_array_fill()
{
	uint16_t waveform_buf_index = WAVEFORM_BUF_START_INDEX;	//Start index of waveform buffer
	uint16_t wfb_sample_arr_index = 0;	//Sample array index to store 512 samples
	uint16_t wfb_sample = 0;
	uint8_t ret = METRO_SUCCESS;

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

				IA_Waveform[wfb_sample_arr_index] = wfb_sample;
				wavefomChannel = METRO_LOAD_VA;
				break;
			}

			case METRO_LOAD_VA:
			{
					//Channel 1 Voltage waveform

				VA_Waveform[wfb_sample_arr_index] = wfb_sample;
				wavefomChannel = METRO_LOAD_IB;
				break;
			}

			case METRO_LOAD_IB:
			{
				//Channel 2 Current waveform

				IB_Waveform[wfb_sample_arr_index] = wfb_sample;
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

				VA_Waveform[wfb_sample_arr_index] = wfb_sample;
				wavefomChannel = METRO_LOAD_IA;
				waveform_buf_index = waveform_buf_index + 4;	//To skip reading IC and VC
				wfb_sample_arr_index ++;
				break;
			}

			default:
				break;

		} //end switch

	}// end while

	waveform_buf_index = WAVEFORM_BUF_START_INDEX;

		//reset waveform data array index to restart filling from initial index

	if (wfb_sample_arr_index == MAX_WAVEFORM_SAMPLES)
	{
		wfb_sample_arr_index = 0;
	}
}
#endif


/**
 * @brief      This function Read active, reactive and apparent energy according to the selected type for the given channel
 * @param[in]   uint8_t channel
 * @retval      uint8_t error type
 */
void Metro_Read_Accumulated_Energy(METRO_Channel_t in_Metro_Channel)
{
	double act_cal_energy = 0;
	double react_cal_energy = 0;
	double app_cal_energy = 0;
	uint16_t phase_sign = 0;

	Metro_Get_Energy_Sign(in_Metro_Channel, &phase_sign);

	Metro_Read_Energy(W_ACTIVE, in_Metro_Channel, &act_cal_energy);	//active energy
	Metro_Read_Energy(REACTIVE, in_Metro_Channel, &react_cal_energy);	//reactive energy
	Metro_Read_Energy(APPARENT, in_Metro_Channel, &app_cal_energy);	//Apparent energy

	g_admetroData.energySign[in_Metro_Channel] = phase_sign;

		//Accumulate energy

	if (g_admetroData.energySign[in_Metro_Channel] == METRO_EGY_SIGN_POSITIVE)
	{
		g_admetroData.energyActive[in_Metro_Channel] = g_admetroData.energyActive[in_Metro_Channel] + act_cal_energy;
		g_admetroData.energyReactive[in_Metro_Channel] = g_admetroData.energyReactive[in_Metro_Channel] + react_cal_energy;
		g_admetroData.energyApparent[in_Metro_Channel] = g_admetroData.energyApparent[in_Metro_Channel] + app_cal_energy;
	}
	else
	{
		g_admetroData.energyRevActive[in_Metro_Channel] = g_admetroData.energyRevActive[in_Metro_Channel] + act_cal_energy;
		g_admetroData.energyRevReactive[in_Metro_Channel] = g_admetroData.energyRevReactive[in_Metro_Channel] + react_cal_energy;
		g_admetroData.energyRevApparent[in_Metro_Channel] = g_admetroData.energyRevApparent[in_Metro_Channel] + app_cal_energy;
	}		//if ends

#ifdef Metro_Bi_Metro_EGY_ACC_DEBUG
	if (in_Metro_Channel == CHANNEL_2)
	{
		ESP_LOGI(" FW energy-- Aact: %f, Areact: %f, Aapp: %f, Bact: %f, Breact: %f, Bapp: %f \n"
				"RW energy-- Act: %f, Areact: %f, Aapp: %f, Bact: %f, Breact: %f, Bapp: %f \n==========================\n",
				g_admetroData.energyActive[0], g_admetroData.energyReactive[0], g_admetroData.energyApparent[0],
				g_admetroData.energyActive[1], g_admetroData.energyReactive[1], g_admetroData.energyApparent[1],
				g_admetroData.energyRevActive[0], g_admetroData.energyRevReactive[0], g_admetroData.energyRevApparent[0],
				g_admetroData.energyRevActive[1], g_admetroData.energyRevReactive[1], g_admetroData.energyRevApparent[1]);

	}
#endif

}

void getMetroData(meterTelemetryData_st *metroData)
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
void Metro_startAvgMetroDataRead()
{
	//set flag to start reading meteorology data every 20msec
	g_metroFlag.data_average_status = true;
}
//--------------------------------------------------------------------------------------------------
//            START OF FUNCTION        Metro_setBreakerCalibration()
//--------------------------------------------------------------------------------------------------
/**
 * @brief		Function sets set an event for metro task to set new calibration from ADE chip
 * 				received over SBLCP
 *
 * @param[in] 	1. calibration parameters received over SBLCP
 * @retval     	none
*/
//--------------------------------------------------------------------------------------------------
void Metro_setBreakerCalibration(uint8_t *rxBuff)
{

	uint8_t MetroEvent;
	metro_calib_struct calib_param;

	memcpy(&calib_param, rxBuff, sizeof(metro_calib_struct));
	nvs_write_metro_calib(calib_param);
	MetroEvent = METRO_SET_CALIBRATION;
	xQueueSend(Metro_evt_queue, &MetroEvent, NULL);
}

/**
 * @brief      This function write energy into NVS for channel 1 and 2 as per sign of energy
 * @param[in]   none
 * @retval     none
 */
void Metro_Energy_NVS_Write(void)
{

	write_PhA_Frwrd_energy();
	write_PhA_reverse_energy();

	write_PhB_frwrd_energy();
	write_PhB_reverse_energy();
}


/**
 * @brief      This function reset metro_data structure
 * @retval     None
*/
void Metro_Data_Reset()
{
    memset (&g_admetroData, 0x00, sizeof (g_admetroData));
    memset (&g_cloudTrendingData, 0x00, sizeof (g_cloudTrendingData));
}


/**
 * @brief      This function loads default calibration into NVS
 * @retval     None
*/
void Metro_Load_Default_Calibration()
{
	metro_calib_struct metro_default_calib = {
		.avGain = ADE9000_AVGAIN,
		.aiGain = ADE9000_AIGAIN,
		.aiRmsOs = ADE9000_AIRMSOS,
		.apGain = ADE9000_APGAIN,
		.aphCal_0 = ADE9000_APHCAL0,

		.bvGain = ADE9000_BVGAIN,
		.biGain = ADE9000_BIGAIN,
		.biRmsOs = ADE9000_BIRMSOS,
		.bpGain = ADE9000_BPGAIN,
		.bphCal_0 = ADE9000_BPHCAL0,
	};

	nvs_write_metro_calib(metro_default_calib);
}


/**
 * @brief      This function is designed to read metro data and calculate average
 * @param[in]  None
 * @retval     uint8_t err_type
*/
uint8_t Metro_Calculate_Average_data()
{
	METRO_Channel_t in_Metro_Channel;
	uint8_t ret = METRO_SUCCESS;
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
					if ((ret = Metro_Get_Energy()) != METRO_SUCCESS)
					{
						ret = METRO_SPI_ERR;
					}
					else
					{
						Metro_Update_Energy();		//Calculate and update the energy

#ifdef METRO_DATA_AVERAGE_DEBUG
						printf("==================\n\nnaverage : CH = %d, VRMS = %f V, IRMS = %f A, Frequency = %f Hz, PowerFactor = %f, "
								"Pact = %f W, Preact = %f VAR, Papp = %f VA \naverage : CH = %d, VRMS = %f V, IRMS = %f A, Frequency = %f Hz, PowerFactor = %f, "
								"Pact = %f W, Preact = %f VAR, Papp = %f VA \n\n",
						CHANNEL_1 + 1, g_admetroData.rmsvoltage[CHANNEL_1], g_admetroData.rmscurrent[CHANNEL_1], g_admetroData.linefrequency[CHANNEL_1],
						g_admetroData.powerfactor[CHANNEL_1], g_admetroData.powerActive[CHANNEL_1], g_admetroData.powerReactive[CHANNEL_1],
						g_admetroData.powerApparent[CHANNEL_1], CHANNEL_2 + 1, g_admetroData.rmsvoltage[CHANNEL_2], g_admetroData.rmscurrent[CHANNEL_2],
						g_admetroData.linefrequency[CHANNEL_2], g_admetroData.powerfactor[CHANNEL_2], g_admetroData.powerActive[CHANNEL_2],
						g_admetroData.powerReactive[CHANNEL_2], g_admetroData.powerApparent[CHANNEL_2]);
#endif
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
		SBLCP_telemetry_data.phase_A_active_energy = average_data.energyActive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reactive_energy = average_data.energyReactive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_app_energy = average_data.energyApparent[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reverse_active_energy = average_data.energyRevActive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reverse_reactive_energy = average_data.energyRevReactive[CHANNEL_1];
		SBLCP_telemetry_data.phase_A_reverse_app_energy = average_data.energyRevApparent[CHANNEL_1];

		SBLCP_telemetry_data.phase_B_line_frequency = average_data.linefrequency[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_rms_voltage = average_data.rmsvoltage[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_rms_current = average_data.rmscurrent[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_power_factor = average_data.powerfactor[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_active_power = average_data.powerActive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reactive_power = average_data.powerReactive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_app_power = average_data.powerApparent[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_active_energy = average_data.energyActive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reactive_energy = average_data.energyReactive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_app_energy = average_data.energyApparent[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reverse_active_energy = average_data.energyRevActive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reverse_reactive_energy = average_data.energyRevReactive[CHANNEL_2];
		SBLCP_telemetry_data.phase_B_reverse_app_energy = average_data.energyRevApparent[CHANNEL_2];

		SBLCP_getSetAvgTeleData(&SBLCP_telemetry_data);
	}
	return ret;
}


/**
 * @brief      This function is designed to send metro read event to metro task
 * @param[in]  None
 * @retval     uint8_t err_type
*/
void Metro_Read_Event_Set(void)
{
    uint8_t MetroEvent;
    MetroEvent = METRO_READ_EVENT;

    if (Metro_evt_queue != NULL)
	{
    	xQueueSendToFront(Metro_evt_queue, &MetroEvent, NULL);
	}
}
