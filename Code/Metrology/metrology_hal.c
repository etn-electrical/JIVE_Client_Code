/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : metrology_hal.c
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
#include "spi_comp.h"
#include "metro_utilities.h"
#include <math.h>
#include "metrology_hal.h"
#include "Log.h"

/****************************************************************************
 *                              Global variables
 ****************************************************************************/
static const char* TAG = "Metro_HAL";
METRO_AD_reg g_metroADreg;

/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
#ifdef AD_WFB_EN
/**
 * @brief      This function disables the waveform capture
 * @param[in]  None
 * @retval     uint8_t err type
*/
uint8_t Metro_HAL_Waveform_Cap_Dis()
{
	uint8_t ret = METRO_SUCCESS;
	uint16_t spi_read_data = 0;

		//Waveform Capture disabled

	if ((ret = Write_SPI_ADE9039_Reg_16(ADDR_WFB_CFG, ADE9000_WFB_CFG_CAP_DIS)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

		//Verify SPI read value to ensure SPI is working

	if ((ret = Read_SPI_ADE9039_Reg_16(ADDR_WFB_CFG, &spi_read_data)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	else
	{
		if (spi_read_data != ADE9000_WFB_CFG_CAP_DIS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}


/**
 * @brief      This function enables the waveform capture
 * @param[in]  None
 * @retval     uint8_t err type
*/

uint8_t Metro_HAL_Waveform_Cap_En()
{
	uint8_t ret = METRO_SUCCESS;
	uint16_t spi_read_data = 0;

		//Waveform Capture Enabled

	if ((ret = Write_SPI_ADE9039_Reg_16(ADDR_WFB_CFG, ADE9000_WFB_CFG_CAP_EN)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

		//Verify SPI read value to ensure SPI is working

	if ((ret = Read_SPI_ADE9039_Reg_16(ADDR_WFB_CFG, &spi_read_data)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	else
	{
		if (spi_read_data != ADE9000_WFB_CFG_CAP_EN)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}
#endif

/**
 * @brief      This function reads required registers
 * @param[in]  None
 * @retval     uint8_t err type
*/
uint8_t Metro_HAL_read_required_reg()
{
	uint8_t reg_count = 0;
	uint8_t ret = METRO_SUCCESS;
	uint32_t spi_read_data = 0;

		//Array of register address

	uint16_t address_array[] = {ADDR_AIRMS, ADDR_BIRMS, ADDR_AVRMS, ADDR_BVRMS, ADDR_AWATT, ADDR_BWATT,
			ADDR_AFVAR, ADDR_BFVAR, ADDR_AVA, ADDR_BVA, ADDR_APF, ADDR_BPF, ADDR_APERIOD, ADDR_BPERIOD};

		//Read Vrms, Irms, PF, Active Power, Reactive Power, Apparent Power, Frequency, Line period for PH1 and PH2

	for (reg_count = 0; reg_count < MAX_REQUIRED_DATA_REG; reg_count++)
	{
		if ((ret = Read_SPI_ADE9039_Reg_32(address_array[reg_count], &spi_read_data)) != METRO_SUCCESS)		//Read register
		{
			ret = METRO_SPI_ERR;
			break;
		}
		else
		{
				// Update g_metroADreg structure with the received value

			switch (reg_count)
			{
				case 0:
				{
					g_metroADreg.AIRMS = spi_read_data;
					break;
				}

				case 1:
				{
					g_metroADreg.BIRMS = spi_read_data;
					break;
				}

				case 2:
				{
					g_metroADreg.AVRMS = spi_read_data;
					break;
				}

				case 3:
				{
					g_metroADreg.BVRMS = spi_read_data;
					break;
				}

				case 4:
				{
					g_metroADreg.AWATT = spi_read_data;
					break;
				}

				case 5:
				{
					g_metroADreg.BWATT = spi_read_data;
					break;
				}

				case 6:
				{
					g_metroADreg.AFVAR = spi_read_data;
					break;
				}

				case 7:
				{
					g_metroADreg.BFVAR = spi_read_data;
					break;
				}

				case 8:
				{
					g_metroADreg.AVA = spi_read_data;
					break;
				}

				case 9:
				{
					g_metroADreg.BVA = spi_read_data;
					break;
				}

				case 10:
				{
					g_metroADreg.APF = spi_read_data;
					break;
				}

				case 11:
				{
					g_metroADreg.BPF = spi_read_data;
					break;
				}

				case 12:
				{
					g_metroADreg.APERIOD = spi_read_data;
					break;
				}

				case 13:
				{
					g_metroADreg.BPERIOD = spi_read_data;
					break;
				}

				default:
					break;
			}
		}
	}

		//Verify SPI reg read value

	if (spi_read_data == 0Xffffffff)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}


/**
 * @brief      This function sets the Status register
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_set_Status_reg()
{
	uint32_t set = 0xffffffff;
	uint8_t ret =  METRO_SUCCESS;

		//Write Status registers

	if ((ret = Write_SPI_ADE9039_Reg_32(ADDR_STATUS1, set)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	else
	{
		if ((ret = Write_SPI_ADE9039_Reg_32(ADDR_STATUS0, set)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}



/**
 * @brief      This function read waveform burst data of VA, IA, VB, IB for 1 cycle
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_read_Waveform_burst_data(void)
{
	uint16_t waveform_page_num = 0;
	uint16_t page_init_addr;
	uint8_t ret = METRO_SUCCESS;
	uint32_t spi_read_data = 0;

		//waveform cycle data start address array

	uint16_t wfb_init_addr[MAX_WAVEFORM_CYCLES] = {0x800, 0x900, 0xA00, 0xB00, 0xC00, 0xD00, 0xE00, 0xF00};

		//Read resampled last cycle status register

	if ((ret = Read_SPI_ADE9039_Reg_32(ADDR_RESAMPLE_LAST_CYCLE, &spi_read_data)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	else
	{
		waveform_page_num = spi_read_data;

			//Check page number which recently read

		if(waveform_page_num < MAX_WAVEFORM_CYCLES)
		{
			page_init_addr = wfb_init_addr[waveform_page_num];

			if ((ret = Burst_Read_SPI_ADE9039(page_init_addr, MAX_BURST_READ_DATA_BYTES)) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;
			}
		}
		else
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}

/**
 * @brief      This function reads energy registers
 * @param[in]  METRO_Channel_t in_Metro_Channel: Channel1 or Channel 2
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_read_Energy_reg(METRO_Channel_t in_Metro_Channel)
{
	uint8_t reg_count = 0;
	uint8_t ret = METRO_SUCCESS;
	uint16_t phase_sign_reg = 0;
	uint32_t spi_read_data = 0;
	uint16_t spi_read_data_16 = 0;

	uint16_t address_array[] = {ADDR_AWATTHR_HI, ADDR_AFVARHR_HI, ADDR_AVAHR_HI,
			ADDR_BWATTHR_HI, ADDR_BFVARHR_HI, ADDR_BVAHR_HI};

	if ((ret = Read_SPI_ADE9039_Reg_16(ADDR_PHSIGN, &spi_read_data_16)) != METRO_SUCCESS)	//read sign of the energy
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		g_metroADreg.PHSIGN = spi_read_data_16;
	}

	switch(in_Metro_Channel)
	{
		case CHANNEL_1:
		{
				//Read Channel 1 energy registers

			for(reg_count = 0; reg_count < MAX_ENERGY_REG_CH1; reg_count++)	//#define
			{
				if ((ret = Read_SPI_ADE9039_Reg_32(address_array[reg_count], &spi_read_data)) != METRO_SUCCESS)
				{
					ret = METRO_SPI_ERR;
					break;
				}
				else
				{
						//Load register value into variable

					switch(reg_count)
					{
						case 0:
						{
							g_metroADreg.AWATTHR_HI = spi_read_data;
							break;
						}

						case 1:
						{
							g_metroADreg.AFVARHR_HI = spi_read_data;
							break;
						}

						case 2:
						{
							g_metroADreg.AVAHR_HI = spi_read_data;
							break;
						}

						default:
							break;
					}
				}
			}

			break;
		}

		case CHANNEL_2:
		{
				//Read Channel 2 energy registers

			for (reg_count = 3; reg_count < MAX_ENERGY_REG; reg_count++)
			{
				if ((ret = Read_SPI_ADE9039_Reg_32(address_array[reg_count], &spi_read_data)) != METRO_SUCCESS)
				{
					ret = METRO_SPI_ERR;
					break;
				}

				else
				{
						//Load register value into variable

					switch(reg_count)
					{
						case 3:
						{
							g_metroADreg.BWATTHR_HI = spi_read_data;
							break;
						}

						case 4:
						{
							g_metroADreg.BFVARHR_HI = spi_read_data;
							break;
						}

						case 5:
						{
							g_metroADreg.BVAHR_HI = spi_read_data;
							break;
						}

						default:
							break;
					}
				}
			}
			break;
		}

		default:
			break;

	}	//switch ends

	if (spi_read_data == 0xffffffff)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}



/**
 * @brief      This function reads the status register
 * @param[in]  uint32_t *status0_val: Stores status register value
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_get_Status_reg(uint32_t *status0_val)
{
	uint8_t ret = METRO_SUCCESS;
	uint32_t spi_read_data = 0;

		//Read Status0 register

	if ((ret = Read_SPI_ADE9039_Reg_32(ADDR_STATUS0, &spi_read_data)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		*status0_val = spi_read_data;
	}

	return ret;
}



/**
 * @brief      This function start SPI communication
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_Start_SPI_Com()
{
	uint8_t ret = METRO_SUCCESS;
	uint16_t spi_read_data = 0;

		//Write RUN_ON register

	if ((ret = Write_SPI_ADE9039_Reg_16(ADDR_RUN, ADE9000_RUN_ON)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	if ((ret = Read_SPI_ADE9039_Reg_16(ADDR_RUN, &spi_read_data))!= METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		if (spi_read_data != ADE9000_RUN_ON)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}




///**
// * @brief      This function writes calibration registers
// * @param[in]  None
// * @retval     uint8_t error type
//*/
//uint8_t Metro_HAL_Write_Calibration_regs()
//{
//	uint8_t ret = METRO_SUCCESS;
//	uint8_t reg_count;
//	size_t length;
//	metro_calib_struct calib_factors;
//	uint32_t spi_read_data = 0;
//
//	length = nvs_read_metro_calib(&calib_factors);
//
//	if (length > 0)
//	{
//
//		uint16_t address_arr_32[] = {ADDR_AIGAIN, ADDR_AVGAIN, ADDR_AIRMSOS, ADDR_APGAIN, ADDR_APHCAL0,
//				ADDR_BIGAIN, ADDR_BVGAIN, ADDR_BIRMSOS, ADDR_BPGAIN, ADDR_BPHCAL0};		//Array of 32 bit register address
//
//		uint32_t reg_data_arr_32[MAX_CALIB_REG];
//
//		reg_data_arr_32[0] = calib_factors.aiGain;
//		reg_data_arr_32[1] = calib_factors.avGain;
//		reg_data_arr_32[2] = calib_factors.aiRmsOs;
//		reg_data_arr_32[3] = calib_factors.apGain;
//		reg_data_arr_32[4] = calib_factors.aphCal_0;
//		reg_data_arr_32[5] = calib_factors.biGain;
//		reg_data_arr_32[6] = calib_factors.bvGain;
//		reg_data_arr_32[7] = calib_factors.biRmsOs;
//		reg_data_arr_32[8] = calib_factors.bpGain;
//		reg_data_arr_32[9] = calib_factors.bphCal_0;
//
//
//#if 0
//	ESP_LOGI("AIGAIN: 0x%x\n", reg_data_arr_32[0]);
//	ESP_LOGI("AVGAIN: 0x%x\n", reg_data_arr_32[1]);
//	ESP_LOGI("AIRMSOS: 0x%x\n", reg_data_arr_32[2]);
//	ESP_LOGI("APGAIN: 0x%x\n", reg_data_arr_32[3]);
//	ESP_LOGI("APHCAL0: 0x%x\n", reg_data_arr_32[4]);
//
//	ESP_LOGI("BIGAIN: 0x%x\n", reg_data_arr_32[5]);
//	ESP_LOGI("BVGAIN: 0x%x\n", reg_data_arr_32[6]);
//	ESP_LOGI("AIRMSOS: 0x%x\n", reg_data_arr_32[7]);
//	ESP_LOGI("BPGAIN: 0x%x\n", reg_data_arr_32[8]);
//	ESP_LOGI("BPHCAL0: 0x%x\n", reg_data_arr_32[9]);
//#endif
//
//			//Write voltage, Current, Current offset, Phase calibration factors for channel 1 and 2
//
//		for (reg_count = 0; reg_count < MAX_CALIB_REG; reg_count++)
//		{
//			if ((ret = Write_SPI_ADE9039_Reg_32(address_arr_32[reg_count], reg_data_arr_32[reg_count])) != METRO_SUCCESS)
//			{
//				ret = METRO_SPI_ERR;
//				break;
//			}
//		}
//
//		if ((ret = Read_SPI_ADE9039_Reg_32(ADDR_BPHCAL0, &spi_read_data)) != METRO_SUCCESS)
//		{
//			ret = METRO_SPI_ERR;
//		}
//		else
//		{
//				//Verify SPI read value to ensure SPI is working
//
//			if (spi_read_data != ADE9000_BPHCAL0)
//			{
//				ret = METRO_SPI_ERR;
//			}
//		}
//	}
//	else
//	{
//		ret = METRO_SPI_ERR;
//	}
//
//	return ret;
//}

/**
* @brief      This function writes calibration registers
* @param[in]  None
* @retval     uint8_t error type
*/
uint8_t Metro_HAL_Write_Calibration_regs()
{
    uint8_t ret = METRO_SUCCESS;
    uint8_t reg_count;
    size_t length;
    metro_calib_struct calib_factors;
    uint32_t spi_read_data = 0;



    length = nvs_read_metro_calib(&calib_factors);



    if (length > 0)
    {



        uint16_t address_arr_32[] = {ADDR_AIGAIN, ADDR_AVGAIN, ADDR_AIRMSOS, ADDR_APGAIN, ADDR_APHCAL0,
                ADDR_BIGAIN, ADDR_BVGAIN, ADDR_BIRMSOS, ADDR_BPGAIN, ADDR_BPHCAL0};        //Array of 32 bit register address



        uint32_t reg_data_arr_32[MAX_CALIB_REG];



        reg_data_arr_32[0] = calib_factors.aiGain;
        reg_data_arr_32[1] = calib_factors.avGain;
        reg_data_arr_32[2] = calib_factors.aiRmsOs;
        reg_data_arr_32[3] = calib_factors.apGain;
        reg_data_arr_32[4] = calib_factors.aphCal_0;
        reg_data_arr_32[5] = calib_factors.biGain;
        reg_data_arr_32[6] = calib_factors.bvGain;
        reg_data_arr_32[7] = calib_factors.biRmsOs;
        reg_data_arr_32[8] = calib_factors.bpGain;
        reg_data_arr_32[9] = calib_factors.bphCal_0;




#if 0
    ESP_LOGI("AIGAIN: 0x%x\n", reg_data_arr_32[0]);
    ESP_LOGI("AVGAIN: 0x%x\n", reg_data_arr_32[1]);
    ESP_LOGI("AIRMSOS: 0x%x\n", reg_data_arr_32[2]);
    ESP_LOGI("APGAIN: 0x%x\n", reg_data_arr_32[3]);
    ESP_LOGI("APHCAL0: 0x%x\n", reg_data_arr_32[4]);



    ESP_LOGI("BIGAIN: 0x%x\n", reg_data_arr_32[5]);
    ESP_LOGI("BVGAIN: 0x%x\n", reg_data_arr_32[6]);
    ESP_LOGI("AIRMSOS: 0x%x\n", reg_data_arr_32[7]);
    ESP_LOGI("BPGAIN: 0x%x\n", reg_data_arr_32[8]);
    ESP_LOGI("BPHCAL0: 0x%x\n", reg_data_arr_32[9]);
#endif



            //Write voltage, Current, Current offset, Phase calibration factors for channel 1 and 2



        for (reg_count = 0; reg_count < MAX_CALIB_REG; reg_count++)
        {
            if ((ret = Write_SPI_ADE9039_Reg_32(address_arr_32[reg_count], reg_data_arr_32[reg_count])) != METRO_SUCCESS)
            {
                ret = METRO_SPI_ERR;
                break;
            }
        }



        if ((ret = Read_SPI_ADE9039_Reg_32(ADDR_BPHCAL0, &spi_read_data)) != METRO_SUCCESS)
        {
            ret = METRO_SPI_ERR;
        }
        else
        {
                //Verify SPI read value to ensure SPI is working



            if (spi_read_data != calib_factors.bphCal_0)
            {
                ret = METRO_SPI_ERR;
            }
        }
    }
    else
    {
        ret = METRO_SPI_ERR;
    }

    return ret;
}

/**
 * @brief      This function sets the Configuration
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_set_Config_regs()
{
	uint8_t ret = METRO_SUCCESS;
	uint8_t reg_counter;
	uint16_t spi_read_data = 0;

	uint16_t address_arr_16[] = {ADDR_CONFIG2, ADDR_CONFIG3, ADDR_CONFIG1, ADDR_ACCMODE,
			ADDR_EGY_TIME, ADDR_EP_CFG, ADDR_ZX_LP_SEL, ADDR_ZXTHRSH, ADDR_CRC_FORCE};		//Array of 16 bit register address

	uint16_t address_arr_32[] = {ADDR_CONFIG0, ADDR_MASK0};		//Array of 32 bit register address

	uint16_t reg_data_arr_16[] = {ADE9000_CONFIG2, ADE9000_CONFIG3, ADE9000_CONFIG1, ADE9000_ACCMODE,
			ADE9000_EGY_TIME, ADE9000_EP_CFG, ADE9000_ZX_LP_SEL, ADE9000_ZXTHRSH, ADE9000_CRC_FORCE};		//Array of 16 bit register data

	uint32_t reg_data_arr_32[] = {ADE9000_CONFIG0, ADE9000_MASK0};		//Array of 32 bit register data

		//Enable energy Acc, Select zero crossing channel, Enables CRC calculations, Configure Zero crossing threshold

	for (reg_counter = 0; reg_counter < MAX_CONFIG_REG_16; reg_counter++)
	{
		if ((ret = Write_SPI_ADE9039_Reg_16(address_arr_16[reg_counter], reg_data_arr_16[reg_counter])) != METRO_SUCCESS)
		{
				ret = METRO_SPI_ERR;
				break;
		}
	}

		//Verify SPI read data

	if ((ret = Read_SPI_ADE9039_Reg_16(ADDR_CRC_FORCE, &spi_read_data)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		if (spi_read_data != ADE9000_CRC_FORCE)
		{
			ret = METRO_SPI_ERR;
		}
		else
		{
				//Set configuration register and Enable Interrupts

			for (reg_counter = 0; reg_counter < MAX_CONFIG_REG_32; reg_counter++)
			{
				if ((ret = Write_SPI_ADE9039_Reg_32(address_arr_32[reg_counter], reg_data_arr_32[reg_counter])) != METRO_SUCCESS)
				{
					ret = METRO_SPI_ERR;
					break;
				}
			}
		}
	}

	return ret;
}


/**
 * @brief      This function redirect metro channels using Mux
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_Redirect_Channels()
{
	uint8_t ret = METRO_SUCCESS;
	uint32_t spi_read_data = 0;

	if ((ret = Write_SPI_ADE9039_Reg_32(ADDR_ADC_REDIRECT, ADE9000_ADC_REDIRECT)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

		//Verify SPI read value to ensure SPI is working

	if ((ret = Read_SPI_ADE9039_Reg_32(ADDR_ADC_REDIRECT, &spi_read_data)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		if (spi_read_data != ADE9000_ADC_REDIRECT)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}



/**
 * @brief      This function sets the Channel gain
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Metro_HAL_set_Channel_Gain()
{
	uint8_t ret = METRO_SUCCESS;
	uint16_t spi_read_data = 0;

		//Configure PGA gain for channels

	if ((ret = Write_SPI_ADE9039_Reg_16(ADDR_PGA_GAIN, ADE9000_PGA_GAIN)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

		//Verify SPI read value to ensure SPI is working

	if ((ret = Read_SPI_ADE9039_Reg_16(ADDR_PGA_GAIN, &spi_read_data)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}
	else
	{
		if (spi_read_data != ADE9000_PGA_GAIN)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}

/**
 * @brief      This function initialize SPI communication
 * @param[in]  None
 * @retval     uint8_t error type
*/
uint8_t Merto_HAL_SPI_Init()
{
	uint8_t ret = METRO_SUCCESS;

	if ((ret = SPI_Init()) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}



/**
 * @brief      Read raw rms voltage for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     uint8_t err type
*/
void Metro_HAL_read_RMS_Voltage(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Voltage)
{
	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			*calc_RMS_Voltage = (float)g_metroADreg.AVRMS * VOLTAGE_CONV_FACT;	//Calculate raw voltage for Channel 1
			break;
		}

		case (CHANNEL_2):
		{
			*calc_RMS_Voltage = (float)g_metroADreg.BVRMS * VOLTAGE_CONV_FACT;	//Calculate raw voltage for Channel 2
			break;
		}

		default:
			break;

	}		//end switch

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw vol : 0x%x \n", *calc_RMS_Voltage);
#endif
}



/**
 * @brief      This function read raw rms current for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2, *calc_RMS_Current
 * @retval     None
*/
void Metro_HAL_read_RMS_Current(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Current)
{
	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			*calc_RMS_Current = (float)g_metroADreg.AIRMS * CURRENT_CONV_FACT;		//Calculate raw Current for Channel 1
			break;
		}

		case (CHANNEL_2):
		{
			*calc_RMS_Current = (float)g_metroADreg.BIRMS * CURRENT_CONV_FACT;		//Calculate raw Current for Channel 2
			break;
		}

		default:
			break;

	}		//end switch

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw current : 0x%x \n", *calc_RMS_Current);
#endif
}


/**
 * @brief      This function read active power current for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2, *calc_RMS_Power
 * @retval     None
*/
void Metro_HAL_read_Active_Power(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power)
{
	int32_t raw_power = 0;

	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			raw_power = g_metroADreg.AWATT;		//Calculate Active Power for Channel 1
			break;
		}

		case (CHANNEL_2):
		{
			raw_power = g_metroADreg.BWATT;		//Calculate Active Power for Channel 2
			break;
		}

		default:
			break;
	}		//end switch

	*calc_RMS_Power = (float)raw_power * POWER_CONV_FACT;

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw power : %f \n", *calc_RMS_Power);
#endif
}



/**
 * @brief      This function read reactive power current for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2, *calc_RMS_Power
 * @retval     None
*/
void Metro_HAL_read_Reactive_Power(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power)
{
	int32_t raw_power = 0;

	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			raw_power = g_metroADreg.AFVAR;		//Calculate raw Reactive Power for Channel 1
			break;
		}

		case (CHANNEL_2):
		{
			raw_power = g_metroADreg.BFVAR;		//Calculate raw Reactive Power for Channel 2
			break;
		}

		default:
			break;

	}		//end switch

	*calc_RMS_Power = (float)raw_power * POWER_CONV_FACT;

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw power : %f \n", *calc_RMS_Power);
#endif

}


/**
 * @brief      This function read apparent power current for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2, *calc_RMS_Power
 * @retval     None
*/
void Metro_HAL_read_Apparent_Power(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power)
{
	int32_t raw_power = 0;

	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			raw_power = g_metroADreg.AVA;	//Calculate raw Apparent Power for Channel 1
			break;
		}

		case (CHANNEL_2):
		{
			raw_power = g_metroADreg.BVA;	//Calculate raw Apparent Power for Channel 2
			break;
		}

		default:
			break;
	}		//end switch

	*calc_RMS_Power = (float)raw_power * POWER_CONV_FACT;

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw power : %f \n", *calc_RMS_Power);
#endif

}


/**
 * @brief      This function read power factor for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     None
*/
void Metro_HAL_read_Power_Factor(METRO_Channel_t in_Metro_Channel, float *cal_pf)
{
	int32_t raw_pf = 0;

	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			raw_pf = g_metroADreg.APF;		//Calculate raw Power Factor for Channel 1
			break;
		}

		case (CHANNEL_2):
		{
			raw_pf = g_metroADreg.BPF;		//Calculate raw Power Factor for Channel 2
			break;
		}

		default:
			break;

	}		//end switch

	*cal_pf = (float)raw_pf * (pow(2,-27));

#ifdef METRO_RAW_DATA_DEBUG
		ESP_LOGI(TAG,"raw_pf : %f \n", *cal_pf);
#endif
}



/**
 * @brief      This function read period for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     None
*/
void Metro_HAL_read_Period(METRO_Channel_t in_Metro_Channel, uint32_t *raw_period)
{
	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			*raw_period = g_metroADreg.APERIOD;		//Calculate raw period for Channel 1
			break;
		}

		case (CHANNEL_2):
		{
			*raw_period = g_metroADreg.BPERIOD;		//Calculate raw period for Channel 2
			break;
		}

		default:
			break;

	}		//end switch
}


/**
 * @brief      This function read phase sign
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2, uint16_t *phase_sign
 * @retval     None
*/
void Metro_HAL_read_Phase_Sign(METRO_Channel_t in_Metro_Channel, uint16_t *phase_sign)
{
	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			if (g_metroADreg.PHSIGN & 0x0001)
			{
				*phase_sign = 1;
			}
			else
			{
				*phase_sign = 0;
			}
			break;
		}

		case (CHANNEL_2):
		{
			if (g_metroADreg.PHSIGN & 0x0004)
			{
				*phase_sign = 1;
			}
			else
			{
				*phase_sign = 0;
			}
			break;
		}

		default:
			break;

	}		//end switch

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw: energy 0x%x \n", *calc_energy);
#endif
}


/**
 * @brief      This function read active energy for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     None
*/
void Metro_HAL_read_Active_Energy(METRO_Channel_t in_Metro_Channel, uint32_t *raw_energy)
{
	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			if (g_metroADreg.AWATTHR_HI != ZERO_ENERGY)
			{
				*raw_energy = g_metroADreg.AWATTHR_HI;		//Calculate Active Energy for Channel 1
			}
			else
			{
				*raw_energy = ZERO_ENERGY;
			}
			break;
		}

		case (CHANNEL_2):
		{
			if (g_metroADreg.BWATTHR_HI != ZERO_ENERGY)
			{
				*raw_energy = g_metroADreg.BWATTHR_HI;		//Calculate Active Energy for Channel 2
			}
			else
			{
				*raw_energy = ZERO_ENERGY;
			}
			break;
		}

		default:
			break;

	}		//end switch

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw: energy 0x%x \n", *calc_energy);
#endif
}


/**
 * @brief      This function read reactive energy for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     None
*/
void Metro_HAL_read_Reactive_Energy(METRO_Channel_t in_Metro_Channel, uint32_t *raw_energy)
{
	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			if (g_metroADreg.AFVARHR_HI != ZERO_ENERGY)
			{
				*raw_energy = g_metroADreg.AFVARHR_HI;		//Calculate Reactive Energy for Channel 1
			}
			else
			{
				*raw_energy = ZERO_ENERGY;
			}
			break;
		}

		case (CHANNEL_2):
		{
			if (g_metroADreg.BFVARHR_HI != ZERO_ENERGY)
			{
				*raw_energy = g_metroADreg.BFVARHR_HI;		//Calculate Reactive Energy for Channel 2
			}
			else
			{
				*raw_energy = ZERO_ENERGY;
			}
			break;
		}

		default:
			break;

	}		//end switch

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw reactive energy: 0x%x \n", *calc_energy);
#endif
}



/**
 * @brief      This function read apparent energy for channel
 * @param[in]  METRO_Channel_t in_Metro_Channel :Channel1 or Channel2
 * @retval     None
*/
void Metro_HAL_read_Apparent_Energy(METRO_Channel_t in_Metro_Channel, uint32_t *raw_energy)
{
	switch (in_Metro_Channel)
	{
		case (CHANNEL_1):
		{
			if (g_metroADreg.AVAHR_HI != ZERO_ENERGY)
			{
				*raw_energy = g_metroADreg.AVAHR_HI;		//Calculate Apparent Energy for Channel 1
			}
			else
			{
				*raw_energy = ZERO_ENERGY;
			}
			break;
		}

		case (CHANNEL_2):
		{
			if (g_metroADreg.BVAHR_HI != ZERO_ENERGY)
			{
				*raw_energy = g_metroADreg.BVAHR_HI;		//Calculate Apparent Energy for Channel 2
			}
			else
			{
				*raw_energy = ZERO_ENERGY;
			}
			break;
		}

		default:
			break;

	}		//end switch

#ifdef METRO_RAW_DATA_DEBUG
	ESP_LOGI(TAG,"raw apparent energy: 0x%x \n", *calc_energy);
#endif
}
