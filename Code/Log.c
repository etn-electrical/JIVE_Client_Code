/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : Breaker
 *
 * File name        : Log.C
 *
 * Author           : Rahul Sakhare
 *
 * Last Changed By  : $Author: Rahul Sakhare$
 * Revision         : $Revision: 0.1 $
 * Last Changed     : $Date: 01/30/23
 *
 ****************************************************************************/
 
/****************************************************************************
 *                              INCLUDE FILES                               
 ****************************************************************************/
 
#include "nvs_flash.h"
#include "nvs.h"
#include "Flash.h"
#include "Breaker.h"
#include "Log.h"

/****************************************************************************
 *                              Macro
 ****************************************************************************/
#define BLOCK_ONE										1
#define BLOCK_TWO										2
/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
extern DRAM_ATTR Device_Info_str DeviceInfo;
extern metroData_t g_admetroData;
//extern metro_calib_st g_Metro_calibration;
extern metro_config_st g_stMetro_config;
/****************************************************************************
 *                              Global variables
 ****************************************************************************/

/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
/**
 * @brief      This function write Phase A forward energy into two different memory locations alternatively in NVS
 * @param[in]   none
 * @retval     none
 */
void write_PhA_Frwrd_energy()
{
	static uint8_t	mem_block_no = BLOCK_ONE;
	nvm_energy_st nvsEnergy;

	nvsEnergy.active_energy = g_admetroData.energyActive[0];
	nvsEnergy.reactive_energy = g_admetroData.energyReactive[0];
	nvsEnergy.apparent_energy = g_admetroData.energyApparent[0];

	if(mem_block_no == BLOCK_ONE)
	{
		WriteDataToFlash(EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_TWO;

	}
	else
	{
		WriteDataToFlash(EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_ONE;
	}
}
/**
 * @brief      This function write Phase A reverse energy into two different memory locations alternatively in NVS
 * @param[in]   none
 * @retval     none
 */
void write_PhA_reverse_energy()
{
	static uint8_t	mem_block_no = BLOCK_ONE;
	nvm_energy_st nvsEnergy;

	nvsEnergy.active_energy = g_admetroData.energyRevActive[0];
	nvsEnergy.reactive_energy = g_admetroData.energyRevReactive[0];
	nvsEnergy.apparent_energy = g_admetroData.energyRevApparent[0];

	if(mem_block_no == BLOCK_ONE)
	{
		WriteDataToFlash(EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_TWO;
	}
	else
	{
		WriteDataToFlash(EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_ONE;
	}
}
/**
 * @brief      This function write Phase B forward energy into two different memory locations alternatively in NVS
 * @param[in]   none
 * @retval     none
 */
void write_PhB_frwrd_energy()
{
	static uint8_t	mem_block_no = BLOCK_ONE;
	nvm_energy_st nvsEnergy;

	nvsEnergy.active_energy = g_admetroData.energyActive[1];
	nvsEnergy.reactive_energy = g_admetroData.energyReactive[1];
	nvsEnergy.apparent_energy = g_admetroData.energyApparent[1];

	if(mem_block_no == BLOCK_ONE)
	{
		WriteDataToFlash(EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_TWO;
	}
	else
	{
		WriteDataToFlash(EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_ONE;
	}

}
/**
 * @brief      This function write Phase B reverse energy into two different memory locations alternatively in NVS
 * @param[in]   none
 * @retval     none
 */
void write_PhB_reverse_energy()
{
	static uint8_t	mem_block_no = BLOCK_ONE;
	nvm_energy_st nvsEnergy;

	nvsEnergy.active_energy = g_admetroData.energyRevActive[1];
	nvsEnergy.reactive_energy = g_admetroData.energyRevReactive[1];
	nvsEnergy.apparent_energy = g_admetroData.energyRevApparent[1];

	if(mem_block_no == BLOCK_ONE)
	{
		WriteDataToFlash(EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_TWO;
	}
	else
	{
		WriteDataToFlash(EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
		mem_block_no = BLOCK_ONE;
	}

}
/**
 * @brief      This function resets all the energies to zero, and writes it to nVS
 * @param[in]   none
 * @retval     ESP_OK / ESP_FAIL
 */
int8_t reset_energy()
{
	int8_t err;
	nvm_energy_st nvsEnergy;
#if NVS_TEST
	printf("Reseting Energy \n" );
#endif
	g_admetroData.energyActive[0] = 0;
	g_admetroData.energyReactive[0] = 0;
	g_admetroData.energyApparent[0] = 0;

	g_admetroData.energyActive[1] = 0;
	g_admetroData.energyReactive[1] = 0;
	g_admetroData.energyApparent[1] = 0;

	g_admetroData.energyRevActive[0] = 0;
	g_admetroData.energyRevReactive[0] = 0;
	g_admetroData.energyRevApparent[0] = 0;

	g_admetroData.energyRevActive[1] = 0;
	g_admetroData.energyRevReactive[1] = 0;
	g_admetroData.energyRevApparent[1] = 0;


	nvsEnergy.active_energy = 0;
	nvsEnergy.reactive_energy = 0;
	nvsEnergy.apparent_energy = 0;

	err = WriteDataToFlash(EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}
	err = WriteDataToFlash(EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}
	err = WriteDataToFlash(EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}
	err = WriteDataToFlash(EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}

	err = WriteDataToFlash(EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}
	err = WriteDataToFlash(EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}

	err = WriteDataToFlash(EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_1_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}
	err = WriteDataToFlash(EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_2_LABEL, &nvsEnergy, sizeof(nvsEnergy));
	if(err!= ESP_OK)
	{
		return err;
	}
	return err;
}
/**
 * @brief      This function resets all the non re-settable energies to zero, and writes it to NVS
 * @param[in]   none
 * @retval     ESP_OK / ESP_FAIL
 */
int8_t reset_nonResettable_energy()
{
	printf("Resetting Non-Resettable Energy\n");
	return -1;
}
/**
 * @brief      This function reads Phase A forward energy from NVS
 * @param[in]   none
 * @retval     none
 */
void read_PhA_frwrd_energy()
{
    size_t length;

    nvm_energy_st energy_one;
    nvm_energy_st energy_two;

/*************************** Read Phase A Energy********************************************************************/
	length = ReadDataFromFlash(EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_1_LABEL, &energy_one);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_one.active_energy = 0;
		energy_one.apparent_energy = 0;
		energy_one.reactive_energy = 0;
	}

	length = ReadDataFromFlash(EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_2_LABEL, &energy_two);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_two.active_energy = 0;
		energy_two.apparent_energy = 0;
		energy_two.reactive_energy = 0;
	}
	if(energy_one.active_energy >= energy_two.active_energy)
	{
		g_admetroData.energyActive[0] = energy_one.active_energy;
	}
	else
	{
		g_admetroData.energyActive[0] = energy_two.active_energy;
	}

	if(energy_one.reactive_energy >= energy_two.reactive_energy)
	{
		g_admetroData.energyReactive[0] = energy_one.reactive_energy;
	}
	else
	{
		g_admetroData.energyReactive[0] = energy_two.reactive_energy;
	}

	if(energy_one.apparent_energy >= energy_two.apparent_energy)
	{
		g_admetroData.energyApparent[0] = energy_one.apparent_energy;
	}
	else
	{
		g_admetroData.energyApparent[0] = energy_two.apparent_energy;
	}
#if NVS_TEST
	printf("PhA Active Total Energy %lf\n", g_admetroData.energyActive[0]);
	printf("PhA ReActive Total Energy %lf\n", g_admetroData.energyReactive[0]);
	printf("PhA apparent Total Energy %lf\n", g_admetroData.energyApparent[0]);
#endif
}
/**
 * @brief      This function reads Phase A reverse energy from NVS
 * @param[in]   none
 * @retval     none
 */
void read_PhA_reverse_energy()
{
    size_t length;

    nvm_energy_st energy_one;
    nvm_energy_st energy_two;
/*************************** Read Phase A reverse Active Energy********************************************************************/
	length = ReadDataFromFlash(EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_1_LABEL, &energy_one);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_one.active_energy = 0;
		energy_one.apparent_energy = 0;
		energy_one.reactive_energy = 0;
	}
	length = ReadDataFromFlash(EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_2_LABEL, &energy_two);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_two.active_energy = 0;
		energy_two.apparent_energy = 0;
		energy_two.reactive_energy = 0;
	}
	if(energy_one.active_energy >= energy_two.active_energy)
	{
		g_admetroData.energyRevActive[0] = energy_one.active_energy;
	}
	else
	{
		g_admetroData.energyRevActive[0] = energy_two.active_energy;
	}

	if(energy_one.reactive_energy >= energy_two.reactive_energy)
	{
		g_admetroData.energyRevReactive[0] = energy_one.reactive_energy;
	}
	else
	{
		g_admetroData.energyRevReactive[0] = energy_two.reactive_energy;
	}

	if(energy_one.apparent_energy >= energy_two.apparent_energy)
	{
		g_admetroData.energyRevApparent[0] = energy_one.apparent_energy;
	}
	else
	{
		g_admetroData.energyRevApparent[0] = energy_two.apparent_energy;
	}

#if NVS_TEST
	printf("PhA reverse Active Total Energy %lf\n", g_admetroData.energyRevActive[0]);
	printf("PhA reverse ReActive Total Energy %lf\n", g_admetroData.energyRevReactive[0]);
	printf("PhA reverse apparent Total Energy %lf\n", g_admetroData.energyRevApparent[0]);
#endif
}
/**
 * @brief      This function reads Phase B forward energy from NVS
 * @param[in]   none
 * @retval     none
 */
void read_PhB_frwrd_energy()
{
	size_t length;
    nvm_energy_st energy_one;
    nvm_energy_st energy_two;

/*************************** Read Phase B Active Energy********************************************************************/
	length = ReadDataFromFlash(EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_1_LABEL, &energy_one);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_one.active_energy = 0;
		energy_one.apparent_energy = 0;
		energy_one.reactive_energy = 0;
	}
	length = ReadDataFromFlash(EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_2_LABEL, &energy_two);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_two.active_energy = 0;
		energy_two.apparent_energy = 0;
		energy_two.reactive_energy = 0;
	}
	if(energy_one.active_energy >= energy_two.active_energy)
	{
		g_admetroData.energyActive[1] = energy_one.active_energy;
	}
	else
	{
		g_admetroData.energyActive[1] = energy_two.active_energy;
	}

	if(energy_one.reactive_energy >= energy_two.reactive_energy)
	{
		g_admetroData.energyReactive[1] = energy_one.reactive_energy;
	}
	else
	{
		g_admetroData.energyReactive[1] = energy_two.reactive_energy;
	}

	if(energy_one.apparent_energy >= energy_two.apparent_energy)
	{
		g_admetroData.energyApparent[1] = energy_one.apparent_energy;
	}
	else
	{
		g_admetroData.energyApparent[1] = energy_two.apparent_energy;
	}
#if NVS_TEST
	printf("PhB Active Total Energy %lf\n", g_admetroData.energyActive[1]);
	printf("PhB ReActive Total Energy %lf\n", g_admetroData.energyReactive[1]);
	printf("PhB apparent Total Energy %lf\n", g_admetroData.energyApparent[1]);
#endif
}
/**
 * @brief      This function reads Phase B reverse energy from NVS
 * @param[in]   none
 * @retval     none
 */
void read_PhB_reverse_energy()
{
	size_t length;
    nvm_energy_st energy_one;
    nvm_energy_st energy_two;

/*************************** Read Phase B reverse Active Energy********************************************************************/
	length = ReadDataFromFlash(EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_1_LABEL, &energy_one);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_one.active_energy = 0;
		energy_one.apparent_energy = 0;
		energy_one.reactive_energy = 0;
	}
	length = ReadDataFromFlash(EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_2_LABEL, &energy_two);
	if(length == ESP_ERR_NVS_NOT_FOUND)
	{
		ets_printf("Nothing is saved yet\n");
		energy_two.active_energy = 0;
		energy_two.apparent_energy = 0;
		energy_two.reactive_energy = 0;
	}
	if(energy_one.active_energy >= energy_two.active_energy)
	{
		g_admetroData.energyRevActive[1] = energy_one.active_energy;
	}
	else
	{
		g_admetroData.energyRevActive[1] = energy_two.active_energy;
	}

	if(energy_one.reactive_energy >= energy_two.reactive_energy)
	{
		g_admetroData.energyRevReactive[1] = energy_one.reactive_energy;
	}
	else
	{
		g_admetroData.energyRevReactive[1] = energy_two.reactive_energy;
	}

	if(energy_one.apparent_energy >= energy_two.apparent_energy)
	{
		g_admetroData.energyRevApparent[1] = energy_one.apparent_energy;
	}
	else
	{
		g_admetroData.energyRevApparent[1] = energy_two.apparent_energy;
	}

#if NVS_TEST
	printf("PhB reverse Active Total Energy %lf\n", g_admetroData.energyRevActive[1]);
	printf("PhB reverse ReActive Total Energy %lf\n", g_admetroData.energyRevReactive[1]);
	printf("PhB reverse apparent Total Energy %lf\n", g_admetroData.energyRevApparent[1]);
#endif
}
/**
 * @brief      This function writes the metrology/AD calibration to NVS
 * @param[in]   none
 * @retval     none
 */
void nvs_write_metro_calib(metro_calib_struct calib_factors)
{
	printf("Write Metro Calibration\n");

	WriteDataToFlash(EEOFFSET_METRO_CALIB_LABEL, &calib_factors, sizeof(calib_factors));
}
/**
 * @brief      This function writes the metrology/AD configuration to NVS
 * @param[in]   none
 * @retval     none
 */
void write_metro_config()
{
	printf("Write Metro configuration\n");
	WriteDataToFlash(EEOFFSET_METRO_CONFIG_LABEL, &g_stMetro_config, sizeof(g_stMetro_config));
}
/**
 * @brief      This function reads the metrology/AD calibration to NVS
 * @param[in]   none
 * @retval     none
 */

size_t nvs_read_metro_calib(metro_calib_struct *calib_factors)
{
	size_t length;

	length = ReadDataFromFlash(EEOFFSET_METRO_CALIB_LABEL, calib_factors);

	if(length <= 0)
	{
		printf("Re-writting default values\n");
		Metro_Load_Default_Calibration();
		length = ReadDataFromFlash(EEOFFSET_METRO_CALIB_LABEL, &calib_factors);
	}

	printf("AIGAIN: 0x%x\n", calib_factors-> aiGain);
	printf("AVGAIN: 0x%x\n", calib_factors-> avGain);
	printf("AIRMSOS: 0x%x\n", calib_factors-> aiRmsOs);
	printf("APGAIN: 0x%x\n", calib_factors-> apGain);
	printf("APHCAL0: 0x%x\n", calib_factors-> aphCal_0);

	printf("BIGAIN: 0x%x\n", calib_factors-> biGain);
	printf("BVGAIN: 0x%x\n", calib_factors-> bvGain);
	printf("AIRMSOS: 0x%x\n", calib_factors-> biRmsOs);
	printf("BPGAIN: 0x%x\n", calib_factors-> bpGain);
	printf("BPHCAL0: 0x%x\n", calib_factors-> bphCal_0);

	return length;
}
/**
 * @brief      This function writes the metrology/AD configuration to NVS
 * @param[in]   none
 * @retval     none
 */
void read_metro_config()
{
	size_t length;

	length = ReadDataFromFlash(EEOFFSET_METRO_CONFIG_LABEL, &g_stMetro_config);

	if(length == 0)
	{
		printf("Nothing is saved yet\n");
	}
//#if NVS_TEST
//	printf("accmode %d\n", g_stMetro_config.accmode);
//	printf("config_0 %d\n", g_stMetro_config.config_0);
//	printf("config_1 %d\n", g_stMetro_config.config_1);
//	printf("config_2 %d\n", g_stMetro_config.config_2);
//	printf("config_3 %d\n", g_stMetro_config.config_3);
//	printf("egy_time %d\n", g_stMetro_config.egy_time);
//	printf("ep_cfg %d\n", g_stMetro_config.ep_cfg);
//	printf("mask_0 %d\n", g_stMetro_config.mask_0);
//	printf("mask_1 %d\n", g_stMetro_config.mask_1);
//	printf("pga_gain %d\n", g_stMetro_config.pga_gain);
//	printf("zx_lp_sel %d\n", g_stMetro_config.zx_lp_sel);
//	printf("zxthresh %d\n", g_stMetro_config.zxthresh);
//	printf("\n\n");
//#endif
}
/**
 * @brief      This function writes the default metrology configuration and calibration to NVS
 * @param[in]   none
 * @retval     none
 */
void setDefaultMetroCalibConfig()
{
//	write_metro_calib();
	//Need to call updated function
	//No need of this function

	write_metro_config();
}
