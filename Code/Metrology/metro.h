/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : metro.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/

#ifndef METRO_H
#define METRO_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/

#include "esp_system.h"
#include "esp_log.h"
#include "esp_crc.h"
#include "../DeviceGPIO.h"
#include "freertos/queue.h"
#include "metro_utilities.h"
/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/
#define CRC_POLYNOM  0x1021		//0x8408
#define CRC_PRESET   0xFFFF

#define ESP_INTR_FLAG_DEFAULT 0

#define METRO_MAX_QUEUE_SIZE 5

#define MAX_WAVEFORM_SAMPLES 512
#define WAVEFORM_BUF_START_INDEX 2

#define SAMPLE_BYTES 2
#define METRO_TASK_STACK_SIZE 2048
#define POSITIVE 0
#define METRO_MAX_AVG_SAMPLES	20

#define MAX_CONSECUTIVE_SPI_ERROR 3

//#define DEBUG_WAVEFORM_DATA
//#define METRO_ENERGY_ACC_DEBUG
//#define CLOUD_DATA_DEBUG_PRINTS
/*******************************************************************************
* TYPES:
*******************************************************************************/
/**
 * @brief METROLOGY EVENT definition
 *
 */
typedef enum {
	METRO_IRQ_READ_EVENT,
	WAVEFORM_INIT_FAIL_EVENT,
	METRO_SET_CALIBRATION,
	METRO_GET_CALIBRATION,
	METRO_SET_SAMPLE_READ_TIME,
	METRO_DEFAULT_EVENT,
	METRO_GET_EVENT_STATUS,
} MetroEventType;


/**
 * @brief IRQ EVENT definition
 *
 */
typedef enum {
	WAVEFORM_FULL_EVENT = 0x00800000,
	CH1_EGY_SIGN_CHANGE_EVENT = 0x00000002,
	CH2_EGY_SIGN_CHANGE_EVENT = 0x00000004,
	ENERGY_READ_EVENT  = 0x00000001,
} MetroIRQEventType;


/**
 * @brief METROLOGY flag definition
 *
 */
typedef struct
{
	bool read_data;
	bool waveform_enable;
    bool update_trending_data;
	bool waveform_cap_status;
	bool data_average_status;
}metro_flag_t;


/**
   * @brief waveform data store in buffer
   *
   */
 typedef enum
 {
 	METRO_LOAD_IA,
 	METRO_LOAD_IB,
 	METRO_LOAD_VA,
 	METRO_LOAD_VB,
 }metro_wfb_t;



/**
  * @brief waveform data collection
  *
  */
typedef union
{
	uint32_t data_16;
	uint8_t data[2];
}waveform_sample_t;


/**
  * @brief metrology data structure definition
  *
  */
typedef struct
{
	//Line1, Line2 RMS voltage
	float rmsvoltage[2];

	//Line1, Line2 RMS Current
	float rmscurrent[2];

	//Line1, Line2 Frequency
	float linefrequency[2];

	//Line1, Line2 Active Power
	float powerActive[2];

	//Line1, Line2 Reactive Power
	float powerReactive[2];

	//Line1, Line2 Apparent Power
	float powerApparent[2];

	//Line1, Line2 Power Factor
	float powerfactor[2];

	//Line1, Line2 Active energy
	double energyActive[2];

	//Line1, Line2 Reactive energy
	double energyReactive[2];

	//Line1, Line2 Apparent energy
	double energyApparent[2];

	//Line1, Line2 Rev Active energy
	double energyRevActive[2];

	//Line1, Line2 Rev Reactive energy
	double energyRevReactive[2];

	//Line1, Line2 Rev Apparent energy
	double energyRevApparent[2];

	//Line1, Line2 Energy sign
	uint8_t energySign[2];

	//total breaker current
	float totalCurrent;

	// ST temperature
	float st_temperature;
} metroData_t;



/**
  * @brief calibration structure
  *
  */
typedef struct
{
	uint32_t avGain;
	uint32_t aiGain;
	uint32_t aiRmsOs;
	uint32_t apGain;
	uint32_t aphCal_0;


	uint32_t bvGain;
	uint32_t biGain;
	uint32_t biRmsOs;
	uint32_t bpGain;
	uint32_t bphCal_0;
}metro_calib_struct;

/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void Metro_Task(void* arg);
void Metro_Update_Measures(void);
void Metro_Waveform_Channel_array_fill(void);
void Metro_Read_Accumulated_Energy(METRO_Channel_t in_Metro_Channel);
void Metro_Data_Reset(void);
void Metro_Task_Stop_Execution(void);
void Metro_ReInit(metro_err_t ret);
void Metro_Task_Delete(void);
void Metro_Load_Default_Calibration(void);
void Metro_Set_Sample_Read_time(void);
void Metro_Set_Trending_Data_Status(bool trending_data_ready);
void Metro_Update_Energy(void);
void Metro_Update_Energy_data(Metro_Energy*);
void Metro_Update_Cloud_data(void);
void Metro_Get_Event_Status(void);

bool Metro_Get_Trending_Data_Status(void);

metro_err_t Metro_Init(void);
metro_err_t Metro_GPIO_IRQ_Init(void);
metro_err_t Metro_Event_Execution(void);
metro_err_t Metro_Waveform_Cap_Init(void);
metro_err_t Metro_Wfb_Read(void);
metro_err_t Metro_Data_Read(void);
metro_err_t Metro_Calculate_Average_data(void);
metro_err_t Metro_Error_Check(metro_err_t ret);
metro_err_t Metro_Get_Energy(void);
metro_err_t Metro_IRQ_Event_Execution(void);



#ifdef __cplusplus
extern "C"
{
#endif
void Metro_Get_Percentage_Load(float *percentLoad);
#ifdef __cplusplus
}
#endif

#endif /* METRO_H */
