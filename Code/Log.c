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
/****************************************************************************
 *                              Global variables
 ****************************************************************************/

/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/

/**
 * @brief      This function writes the metrology/AD calibration to NVS
 * @param[in]   none
 * @retval     none
 */
void nvs_write_metro_calib(metro_calib_struct calib_factors)
{
	printf("Write Metro Calibration\n");

	WriteDataToFlash(EEOFFSET_METRO_CALIB_LABEL, (char *)&calib_factors, sizeof(calib_factors));
}



/**
 * @brief      This function reads the metrology/AD calibration to NVS
 * @param[in]   none
 * @retval     none
 */

size_t nvs_read_metro_calib(metro_calib_struct *calib_factors)
{
	size_t length;

	length = ReadDataFromFlash(EEOFFSET_METRO_CALIB_LABEL, (char *)calib_factors);

	if(length <= 0)
	{
		printf("Re-writting default values\n");
		Metro_Load_Default_Calibration();
		length = ReadDataFromFlash(EEOFFSET_METRO_CALIB_LABEL, (char *)calib_factors);
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
