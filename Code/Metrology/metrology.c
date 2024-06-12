/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : metrology.c
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
#include <stdint.h>
#include <math.h>
#include "Log.h"
#include "metrology_hal.h"
#include "metrology.h"

/****************************************************************************
 *                              Global variables
 ****************************************************************************/
static const char* TAG = "Metro data";

/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
/**
 * @brief      This function read energy register values
 * @retval     uint8_t error type
*/
uint8_t Metro_Get_Energy()
{
	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_Read_Energy_reg(CHANNEL_1)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		if ((ret = Metro_Read_Energy_reg(CHANNEL_2)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}

uint8_t Metro_Read_Energy_reg(METRO_Channel_t in_Metro_Channel)
{
	uint8_t ret = METRO_SUCCESS;

	if ((Metro_HAL_read_Energy_reg(in_Metro_Channel)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function clear the interrupt status on IRQ pin for next inetrrupt
 * @param[in]  None
 * @retval     uint8_t err type
*/
uint8_t Metro_Clear_Interrupt_Status()
{
	uint8_t ret =  METRO_SUCCESS;

	if ((ret = Metro_HAL_set_Status_reg()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function read required registers for power monitoring routine
 * @param[in]  none
 * @retval     uint8_t err type
*/
uint8_t Metro_read_data_regs(void)
{
	uint8_t ret = METRO_SUCCESS;

		//Read Vrms, Irms, Power, Energy, Frequency, PF registers

	if ((ret = Metro_HAL_read_required_reg()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function reads the interrupt status
 * @param[in]  uint32_t * status0_val : stores status0 register value
 * @retval     uint8_t error type
*/
uint8_t Metro_get_Interrupt_Status(uint32_t *status0_val)
{
	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_HAL_get_Status_reg(status0_val)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	return ret;
}


/**
 * @brief      This function is designed to initialize ADE9039 chipset
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_AD_Init()
{
#ifdef METRO_DEBUG_PRINTS
	ESP_LOGI(TAG,"AD Init\n");
#endif

	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_HAL_Redirect_Channels()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	else
	{
		if ((ret = Metro_HAL_set_Channel_Gain()) != METRO_SUCCESS)		//Set channel gain for Channel1 and Channel2
		{
			ret = METRO_SPI_ERR;
		}
		else
		{
			if ((ret = Metro_HAL_set_Config_regs()) != METRO_SUCCESS)		//Write Configuration registers
			{
				ret = METRO_SPI_ERR;
			}
			else
			{
				if ((ret = Metro_HAL_Write_Calibration_regs()) != METRO_SUCCESS)		//Configure calibration factors for VA, IA, VB, IB
				{
					ret = METRO_SPI_ERR;
				}
				else
				{
					if ((ret = Metro_HAL_Start_SPI_Com()) != METRO_SUCCESS)		//Start SPI Communication
					{
						ret = METRO_SPI_ERR;
					}
				}
			}	//if ends
		}	//if ends
	}//if ends

	return ret;
}


/**
 * @brief      This function reset the ADE9039 chipset
 * @retval     None
*/
void Metro_AD_Reset()
{
#if 0
		//RST logic for ESP kit

	gpio_set_level(BOARD_PIN_RST,0);
	gpio_set_level(BOARD_PIN_RST,1);
#endif

		//RST logic for P0 board

	gpio_set_level(BOARD_PIN_RST, 1);	//10ms pulse required to reset ADE9039
	vTaskDelay(10);
	gpio_set_level(BOARD_PIN_RST, 0);
	vTaskDelay(50);	//50ms delay required for Power on sequence
}


/**
 * @brief      This function stop the functionality of ADE9039 chipset
 * @retval     None
*/
void Metro_AD_Stop()
{
#if 0
		//RST logic for ESP kit

	gpio_set_level(BOARD_PIN_RST,0);
	gpio_set_level(BOARD_PIN_RST,1);
#endif

		//RST logic for P0 board

	gpio_set_level(BOARD_PIN_RST, 1);	//10ms pulse required to reset ADE9039
	vTaskDelay(10);
	gpio_set_level(BOARD_PIN_RST, 0);
}


/**
 * @brief      This function initialize SPI communication
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_SPI_Comm_Init()
{
	uint8_t ret = METRO_SUCCESS;

	if ((Merto_HAL_SPI_Init()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function Read power factor for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     None
*/
void Metro_Read_Power_Factor(METRO_Channel_t in_Metro_Channel, float *cal_pf)
{
#ifdef METRO_DEBUG_PRINTS
	ESP_LOGI(TAG,"read period\n");
#endif

	Metro_HAL_read_Power_Factor(in_Metro_Channel, cal_pf);		//Get Power factor according to channel
}



/**
 * @brief  :   Read frequency for the selected channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     None
 */
void Metro_Read_Frequency(METRO_Channel_t in_Metro_Channel, float *raw_freq)
{
#ifdef METRO_DEBUG_PRINTS
	ESP_LOGI(TAG,"read frequency\n");
#endif

	uint32_t raw_period;

		//Get period according to channel

	Metro_HAL_read_Period(in_Metro_Channel, &raw_period);

	if (raw_period != 0)
	{
		*raw_freq = (4000 * pow(2,16)) / ((float)raw_period + 1);
	}
}



/**
 * @brief      This function Read Power according to the selected type for the given channel
 * @param[in]  in_Metro_Power_Selection : W_ACTIVE, REACTIVE, APPARENT, METRO_Channel_t in_Metro_Channel, *calc_RMS_Power
 * @retval     None
 */
void Metro_Read_Power(METRO_Power_selection_t in_Metro_Power_Selection, METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power)
{
	switch (in_Metro_Power_Selection)
	{
		case (W_ACTIVE):
		{
			Metro_HAL_read_Active_Power(in_Metro_Channel, calc_RMS_Power);		//Get active power according to channel
			break;
		}

		case (REACTIVE):
		{
			Metro_HAL_read_Reactive_Power(in_Metro_Channel, calc_RMS_Power);		//Get reactive power according to channel
			break;
		}

		case (APPARENT):
		{
			Metro_HAL_read_Apparent_Power(in_Metro_Channel, calc_RMS_Power);		//Get apparent power according to channel
			break;
		}

		default:
			break;

	 }		//end switch

	*calc_RMS_Power = *calc_RMS_Power / MILLI_DIV_FACTOR;

#ifdef METRO_DEBUG_PRINTS
	ESP_LOGI(TAG,"calc power: %f\n",calc_RMS_Power);
#endif
}



/**
 * @brief      This function read RMS values of both voltage and current for the selected channel
 * @param[in]   in_Metro_Channel : CHANNEL1 or CHANNEL2
 * @param[out]  out_Metro_RMS_voltage , out_Metro_RMS_current
 * @retval     None
 * */
void Metro_Read_RMS(METRO_Channel_t in_Metro_Channel, float * out_Metro_RMS_voltage, float * out_Metro_RMS_current)
{
	float calc_RMS_Voltage;
	float calc_RMS_Current;

	Metro_HAL_read_RMS_Voltage(in_Metro_Channel, &calc_RMS_Voltage);	//Get RMS voltage
	Metro_HAL_read_RMS_Current(in_Metro_Channel, &calc_RMS_Current);	//Get RMS current-

	calc_RMS_Voltage = calc_RMS_Voltage / MICRO_DIV_FACTOR;		//Converting from uV to V
	calc_RMS_Current = calc_RMS_Current / MICRO_DIV_FACTOR;		//Converting from uA to A

	*out_Metro_RMS_voltage = calc_RMS_Voltage;
	*out_Metro_RMS_current = calc_RMS_Current;
}



/**
 * @brief      This function Read energy according to the selected type for the given channel
 * @param[in]  in_Metro_Power_Selection : W_ACTIVE, REACTIVE, APPARENT, calculated energy variable
 * @retval	   None
 */
void Metro_Read_Energy(METRO_Power_selection_t in_Metro_Power_Selection, METRO_Channel_t in_Metro_Channel, double *calc_energy)
{
	uint32_t raw_energy = 0;

	switch (in_Metro_Power_Selection)
	{
		case (W_ACTIVE):
		{
			Metro_HAL_read_Active_Energy(in_Metro_Channel, &raw_energy);	//Get active energy
			break;
		}

		case (REACTIVE):
		{
			Metro_HAL_read_Reactive_Energy(in_Metro_Channel, &raw_energy);	//Get Reactive energy
			break;
		}

		case (APPARENT):
		{
			Metro_HAL_read_Apparent_Energy(in_Metro_Channel, &raw_energy);	//Get apparent energy
			break;
		}

		default:
			break;
	 }		//end switch

	*calc_energy = (double)raw_energy * ENERGY_CONV_FACT;
	*calc_energy = *calc_energy / MILLI_DIV_FACTOR;		//calculate energy in mWh
	*calc_energy = *calc_energy / MILLI_DIV_FACTOR;		//calculate energy in Wh
	*calc_energy = *calc_energy / MILLI_DIV_FACTOR;		//calculate energy in kWh
}

/**
 * @brief      This function Read energy according to the selected type for the given channel
 * @param[in]  METRO_Channel_t in_Metro_Channel, calculated energy variable
 * @retval	   None
 */
void Metro_Get_Energy_Sign(METRO_Channel_t in_Metro_Channel, uint16_t *phase_sign_reg)
{
	Metro_HAL_read_Phase_Sign(in_Metro_Channel, phase_sign_reg);
}



/**
 * @brief      This function writes waveform config regs
 * @retval     None
*/
#ifdef AD_WFB_EN

/**
 * @brief      This function reads waveform data
 * @param[in]  None
 * @retval     None
*/
uint8_t Metro_read_Waveform_data()
{
	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_HAL_read_Waveform_burst_data()) != METRO_SUCCESS)		//Read waveform buffer
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function initialize waveform capture
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_Start_Waveform()
{
	uint8_t ret = METRO_SUCCESS;

	if ((ret = Metro_HAL_Waveform_Cap_Dis()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		if ((ret = Metro_HAL_Waveform_Cap_En()) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}
#endif

uint8_t Metro_Write_EOL_Calibration_Params(BreakerEOLCalibration_st *pBreakerEOLCalibration)
{
	uint8_t ret = METRO_SUCCESS;
	if(pBreakerEOLCalibration != NULL)
	{
		printf("set calibration values\n");
		if((ret = Metro_HAL_Write_EOL_Calibration_Params(pBreakerEOLCalibration)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}
	return ret;
}


/**
 * @brief      This function update calibration into NVS
 * @retval     None
*/
uint8_t Metro_Update_Calib_Fact()
{
	uint8_t ret = METRO_SUCCESS;

//	metro_calib_struct metro_update_calib = {
//		.avGain = METRO_CALB_AV_GAIN,
//		.aiGain = METRO_CALB_AI_GAIN,
//		.aiRmsOs = METRO_CALB_AI_OS_GAIN,
//		.apGain = METRO_CALB_AP_GAIN,
//		.aphCal_0 = METRO_CALB_APHCAL0_GAIN,
//
//		.bvGain = METRO_CALB_BV_GAIN,
//		.biGain = METRO_CALB_BI_GAIN,
//		.biRmsOs = METRO_CALB_BI_OS_GAIN,
//		.bpGain = METRO_CALB_BP_GAIN,
//		.bphCal_0 = METRO_CALB_BPHCAL0_GAIN,
//	};

	printf("\nUpdating cal factors\n");

//	nvs_write_metro_calib(metro_update_calib);

	if ((ret = Metro_HAL_Write_Calibration_regs()) != METRO_SUCCESS)		//Configure calibration factors for VA, IA, VB, IB
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}
