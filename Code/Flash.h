/***************************************************************************
*
* Copyright (c) 2019
* Eaton Cooper wiring devices.
* All Rights Reserved
*
*---------------------------------------------------------------------------
*
*
* Author: Ahmed El-Gayyar
*
* Created   May 3, 2021
****************************************************************************/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/


#ifndef __FLASH__H
#define __FLASH__H

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
/****************************************************************************
 *                              General definitions                               
 ****************************************************************************/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
//#include "nvs_flash.h"
//#include "nvs.h"


#include "Breaker.h"

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/



#define EEOFFSET_MAGIC_LABEL                            "MAGIC"

#define EEOFFSET_PROVISION_LABEL                        "Provision"

#define EEOFFSET_DEVICE_RESET_MODE_LABEL                "DeviceReset"

#define EEOFFSET_DEVICE_NAME_LABEL                      "DeviceName" 

#define EEOFFSET_DEVICE_LOCKED_LABEL                    "DeviceLocked" 

#define EEOFFSET_BUILD_TYPE_LABEL                       "BuildType"

#define EEOFFSET_BUILD_TYPE_SAVED_LABEL                 "BuildTypeSaved"

#define EEOFFSET_PROGRAMED_NEW_PART_NO_LABEL            "NewPartNo"

#define EEOFFSET_PROGRAMED_NEW_PART_NO_SAVED_LABEL      "NewPartSaved"

#define EEOFFSET_RE_PROVISION_STARTED_LABEL              "ReProvStart"

#define EEOFFSET_INIT_ERROR_LOG_LABEL        "InitErrorLog"
#define EEOFFSET_ERROR_LOG_LABEL              "ErrorLog"

#define EEOFFSET_METRO_CONFIG_LABEL								"AD_config"
#define EEOFFSET_METRO_CALIB_LABEL								"AD_Calibration"


#define EEOFFSET_PH_A_REVRS_REACT_ENERGY_LOCATION_1_LABEL	"PhAReactRev_M1"
#define EEOFFSET_PH_A_REVRS_REACT_ENERGY_LOCATION_2_LABEL	"PhAReactRev_M2"

#define EEOFFSET_PH_B_REVRS_REACT_ENERGY_LOCATION_1_LABEL	"PhBReactRev_M1"
#define EEOFFSET_PH_B_REVRS_REACT_ENERGY_LOCATION_2_LABEL	"PhBReactRev_M2"

#define EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_1_LABEL			"PhAFwdEgy_M1"
#define EEOFFSET_PH_A_FRWRD_ENERGY_LOCATION_2_LABEL			"PhAFwdEgy_M2"

#define EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_1_LABEL			"PhARevEgy_M1"
#define EEOFFSET_PH_A_REVRS_ENERGY_LOCATION_2_LABEL			"PhARevEgy_M2"

#define EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_1_LABEL			"PhBFwdEgy_M1"
#define EEOFFSET_PH_B_FRWRD_ENERGY_LOCATION_2_LABEL			"PhBFwdEgy_M2"

#define EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_1_LABEL			"PhBRevEgy_M1"
#define EEOFFSET_PH_B_REVRS_ENERGY_LOCATION_2_LABEL			"PhBRevEgy_M2"

#define EEOFFSET_EERAM_INIT_LABEL					"EERAM_Ready"

#define EEOFFSET_METRO_RESET_LABEL					"Reset_Metro"

#define EEOFFSET_EERAM_RESET_LABEL					"Reset_EERAM"

#define EEOFFSET_CALIBRATION_INIT_STATUS_LABEL		"Calibration"

#define EEOFFSET_SET_PRODUCTION_DETAILS				"Prod_details"
#define EEOFFSET_SET_PCB_DETAILS					"PCB_details"
#define EEOFFSET_SBLCP_FACTORY_MODE					"FACT_MODE"
#define EEOFFSET_PROD_DATA_NVS_INIT				"Init_Prod_NVS"


esp_err_t   FlashInit();
#ifdef __cplusplus
extern "C"
{
#endif
    void     WriteByteToFlash(const char *offeset, uint8_t value );
    uint8_t  ReadByteFromFlash(const char *offeset);

    void     WriteIntToFlash(const char *offeset, uint16_t value );
    uint16_t ReadIntFromFlash(const char *offeset);

    void     WriteWordToFlash(const char *offeset, uint32_t value );
    uint32_t  ReadWordFromFlash(const char *offeset);

    esp_err_t     WriteDataToFlash(const char *offeset, const char * String, size_t length );
    size_t   ReadDataFromFlash(const char *offeset, char *out_value);

    size_t ReadArrayFromFlash(const char *offeset, char *out_value, size_t length);
    void WriteArrayToFlash(const char *offeset, const char * String, size_t length );
#ifdef __cplusplus
}
#endif

void FlashErease(void);
void FlashEnableWriting(void);
void FlashDisableWriting(void);
bool IsWritingToFlashEnabled(void);

void Flash10msTimer(void);


#endif //__FLASH__H

