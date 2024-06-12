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
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/19/2022
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

#define METRO_MAX_QUEUE_SIZE 4
#define MAX_WAVEFORM_SAMPLES 512
#define WAVEFORM_BUF_START_INDEX 2

#define WAVEFORM_FULL_EVENT 0x00800000
#define CH1_EGY_SIGN_CHANGE_EVENT 0x00000002
#define CH2_EGY_SIGN_CHANGE_EVENT 0x00000004

//#define METRO_CLOUD_CONNECTION_DEBUG
#define METRO_EGY_SIGN_POSITIVE 0
#define SAMPLE_BYTES 2
#define METRO_TASK_STACK_SIZE 2048

//#define DEBUG_WAVEFORM_DATA
//#define Metro_Bi_Metro_EGY_ACC_DEBUG
//#define METRO_DATA_AVERAGE_DEBUG

#define METRO_READ_EVENT    	1
#define METRO_IRQ_READ_EVENT  	2
#define WAVEFORM_INIT_FAIL_EVENT 3
#define METRO_SET_CALIBRATION 	4
#define METRO_GET_CALIBRATION 	5

#define METRO_MAX_AVG_SAMPLES	50

/*******************************************************************************
* TYPES:
*******************************************************************************/
/**
  * @brief metrology state machine states
  *
  */
enum METRO_State_t
{
	METRO_GET_MEASURES,
	METRO_UPDATE_MEASURES,
};


/**
 * @brief METROLOGY flag definition
 *
 */
typedef struct
{
	bool read_data;
	bool waveform_enable;
	bool energy_nvs_write;
    bool update_trending_data;
    bool read_Energy;
	bool trending_data_timer;
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
} metroData_t;



// Rahul:- following is the dummy structure for NVS code implementation
// replace this with final calibration structure
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


// Rahul:- following is the dummy structure for NVS code implementation
// replace this with final configuration structure
typedef struct
{
	uint16_t pga_gain;
	uint16_t config_1;
	uint16_t config_2;
	uint16_t config_3;
	uint16_t accmode;
	uint16_t zx_lp_sel;
	uint16_t ep_cfg;
	uint16_t egy_time;
	uint16_t zxthresh;
	uint32_t mask_0;
	uint32_t mask_1;
	uint32_t config_0;
	uint8_t no_cofig_saved;
	uint16_t crc;
}metro_config_st;

/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void Metro_Task(void* arg);
uint8_t Metro_Init(void);
void Metro_Update_Measures(void);
void Metro_Update_Energy(void);
uint8_t Metro_GPIO_IRQ_Init(void);
uint8_t Metro_Event_Execution(void);
uint8_t Metro_Waveform_Cap_Init(void);
void Metro_Waveform_Channel_array_fill(void);
uint8_t Metro_Wfb_Read(void);
uint8_t Metro_Data_Read(void);
void Metro_Read_Accumulated_Energy(METRO_Channel_t in_Metro_Channel);
void Metro_Energy_NVS_Write(void);
void Metro_Task_Stop_Execution(void);
void Metro_Task_ReInit(void);
void Metro_Task_Delete(void);
void Metro_Data_Reset(void);
uint8_t Metro_Calculate_Average_data(void);
void Metro_Load_Default_Calibration(void);
void Metro_Read_Event_Set(void);

#endif /* METRO_H */
