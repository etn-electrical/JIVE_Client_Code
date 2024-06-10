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
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
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

/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
/**
 * @brief      This function read energy registers as per channels
 * @param[in]  METRO_Channel_t in_Metro_Channel
 * @retval     metro_err_t err type
*/
metro_err_t Metro_Read_Energy_reg(METRO_Channel_t in_Metro_Channel)
{
	metro_err_t ret = METRO_SUCCESS;

	if (Metro_HAL_read_Energy_reg(in_Metro_Channel) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}
	return ret;
}


/**
 * @brief      This function clear the interrupt status on IRQ pin for next inetrrupt
 * @param[in]  uint32_t value
 * @retval     metro_err_t err type
*/
metro_err_t Metro_Clear_Interrupt_Status(uint32_t value)
{
	metro_err_t ret = METRO_SUCCESS;

	if ((ret = Metro_HAL_set_Status_reg(value)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}

	return ret;
}


/**
 * @brief      This function read required registers for power monitoring routine
 * @param[in]  none
 * @retval     metro_err_t err type
*/
metro_err_t Metro_Get_Measures(void)
{
	metro_err_t ret = METRO_SUCCESS;

		//Read Vrms, Irms, Power, Energy, Frequency, PF registers

	if (Metro_HAL_read_required_reg() != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}

	return ret;
}


/**
 * @brief      This function reads the interrupt status
 * @param[in]  uint32_t * status0_val : stores status0 register value
 * @retval     metro_err_t error type
*/
metro_err_t Metro_get_Interrupt_Status(uint32_t *status0_val)
{
	metro_err_t ret = METRO_SUCCESS;

	if (Metro_HAL_get_Status_reg(status0_val) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}

	return ret;
}


/**
 * @brief      This function is designed to initialize ADE9039 chipset
 * @param[in]  None
 * @retval     metro_err_t error type
*/
metro_err_t Metro_AD_Init(void)
{
	metro_err_t ret = METRO_SUCCESS;

	if (Metro_HAL_Redirect_Channels() != METRO_SUCCESS)		//Redirect channels
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}
	else
	{

		if (Metro_HAL_set_Channel_Gain() != METRO_SUCCESS)		//Set channel gain for Channel1 and Channel2
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
		else
		{
			if (Metro_HAL_set_Config_regs() != METRO_SUCCESS)		//Write Configuration registers
			{
				ret = METRO_SPI_ERR;	//SPI ERROR
			}
			else
			{
				if ((ret = Metro_HAL_set_Energy_Threshold()) != METRO_SUCCESS)
				{
					ret = METRO_SPI_ERR;
				}
				else
				{
					if (Metro_HAL_Write_Calibration_regs() != METRO_SUCCESS)		//Configure calibration factors for VA, IA, VB, IB
					{
						ret = METRO_SPI_ERR;	//SPI ERROR
					}
					else
					{
						ret = Metro_HAL_Start_SPI_Com();		//Start SPI Communication
					}
				}
			}
		}
	}

	return ret;
}


/**
 * @brief      This function reset the ADE9039 chipset
 * @param[in]  None
 * @retval     None
*/
void Metro_AD_Reset(void)
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
 * @param[in]  None
 * @retval     None
*/
void Metro_AD_Stop(void)
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
 * @retval     metro_err_t error type
*/
metro_err_t Metro_SPI_Comm_Init(void)
{
	return Merto_HAL_SPI_Init();
}


/**
 * @brief      This function Read power factor for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2, float *cal_pf: to store pf value
 * @retval     None
*/
void Metro_Read_Power_Factor(METRO_Channel_t in_Metro_Channel, float *cal_pf)
{
	Metro_HAL_read_Power_Factor(in_Metro_Channel, cal_pf);		//Get Power factor according to channel
}



/**
 * @brief  :   Read frequency for the selected channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2, float *raw_freq: to store freq value
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
			//Active Power

		case (W_ACTIVE):
		{
			Metro_HAL_read_Active_Power(in_Metro_Channel, calc_RMS_Power);		//Get active power according to channel
			break;
		}

			//Reactive Power

		case (REACTIVE):
		{
			Metro_HAL_read_Reactive_Power(in_Metro_Channel, calc_RMS_Power);		//Get reactive power according to channel
			break;
		}

			//Apparent Power

		case (APPARENT):
		{
			Metro_HAL_read_Apparent_Power(in_Metro_Channel, calc_RMS_Power);		//Get apparent power according to channel
			break;
		}

		default:
			break;

	 }		//end switch

	*calc_RMS_Power = *calc_RMS_Power / MILLI_DIV_FACTOR;
}



/**
 * @brief      This function read RMS values of both voltage and current for the selected channel
 * @param[in]   in_Metro_Channel : CHANNEL1 or CHANNEL2, float * out_Metro_RMS_voltage, float * out_Metro_RMS_current
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
 * @param[in]  in_Metro_Power_Selection : W_ACTIVE, REACTIVE, APPARENT, calculated energy variable, METRO_Channel_t in_Metro_Channel, double *calc_energy
 * @retval	   None
 */
void Metro_Read_Energy(METRO_Power_selection_t in_Metro_Power_Selection, METRO_Channel_t in_Metro_Channel, double *calc_energy)
{
	uint32_t raw_energy = 0;

	switch (in_Metro_Power_Selection)
	{
			//Active Energy

		case (W_ACTIVE):
		{
			Metro_HAL_read_Active_Energy(in_Metro_Channel, &raw_energy);	//Get active energy
			break;
		}

			//Reactive Energy

		case (REACTIVE):
		{
			Metro_HAL_read_Reactive_Energy(in_Metro_Channel, &raw_energy);	//Get Reactive energy
			break;
		}

			//Apparent Energy

		case (APPARENT):
		{
			Metro_HAL_read_Apparent_Energy(in_Metro_Channel, &raw_energy);	//Get apparent energy
			break;
		}

		default:
			break;
	 }		//end switch


		//mWh to KWh conversion

	*calc_energy = (double)raw_energy * ENERGY_CONV_FACT;
	*calc_energy = *calc_energy / MILLI_DIV_FACTOR;		//calculate energy in mWh
	*calc_energy = *calc_energy / MILLI_DIV_FACTOR;		//calculate energy in Wh
	*calc_energy = *calc_energy / MILLI_DIV_FACTOR;		//calculate energy in kWh
}



/**
 * @brief      This function Read energy according to the selected type for the given channel
 * @param[in]  METRO_Channel_t in_Metro_Channel, calculated energy sign
 * @retval	   metro_err_t error type
 */
metro_err_t Metro_Get_Energy_Sign(METRO_Channel_t in_Metro_Channel, uint16_t *phase_sign_reg)
{
	return Metro_HAL_read_Phase_Sign(in_Metro_Channel, phase_sign_reg);;
}


#ifdef AD_WFB_EN

/**
 * @brief      This function reads waveform data
 * @param[in]  None
 * @retval     metro_err_t err type
*/
metro_err_t Metro_read_Waveform_data(void)
{
	return Metro_HAL_read_Waveform_burst_data();	//Read waveform buffer
}


/**
 * @brief      This function initialize waveform capture
 * @param[in]  None
 * @retval     metro_err_t error type
*/
metro_err_t Metro_Start_Waveform(void)
{
	metro_err_t ret = METRO_SUCCESS;

		//To enable waveform capture, disable waveform and then enable it

	if (Metro_HAL_Waveform_Cap_Dis() != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}
	else
	{
		ret = Metro_HAL_Waveform_Cap_En();
	}

	return ret;
}
#endif


/**
 * @brief      This function update calibration into NVS
 * @param[in]  None
 * @retval     None
*/
metro_err_t Metro_Update_Calib_Fact(void)
{
	printf("\nUpdating cal factors\n");

	return Metro_HAL_Write_Calibration_regs();		//Configure calibration factors for VA, IA, VB, IB
}



/**
 * @brief      This function set energy read time
 * @param[in]   uint16_t timer_count
 * @retval     metro_err_t err type
 */
metro_err_t Metro_set_Energy_Ready_time(uint16_t timer_count)
{
	return Metro_HAL_set_EGY_TIME_reg(timer_count);
}
