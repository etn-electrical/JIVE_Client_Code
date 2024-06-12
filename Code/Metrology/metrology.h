/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : metrology.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/19/2022
 *
 ****************************************************************************/

#ifndef __METROLOGY_H
#define __METROLOGY_H

#include "metro_utilities.h"
#include "SBLCP.h"

/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/
//#define Metro_Bi_Metro_EGY_ACC_DEBUG
#define MICRO_DIV_FACTOR 1000000

//Need to remove after SBLCP integration
#define METRO_CALB_AV_GAIN	0x1C3292C
#define METRO_CALB_AI_GAIN	0xFDF27AD5
#define METRO_CALB_AI_OS_GAIN	0xFFFFFFEA
#define METRO_CALB_AP_GAIN	0x2CFD
#define METRO_CALB_APHCAL0_GAIN	0xC9B69

#define METRO_CALB_BV_GAIN	0x1CC7E8E
#define METRO_CALB_BI_GAIN	0xF2070D9E
#define METRO_CALB_BI_OS_GAIN	0xFFFFFFF6
#define METRO_CALB_BP_GAIN	0xFFFE172E
#define METRO_CALB_BPHCAL0_GAIN	0xC9D4B

/*******************************************************************************
* TYPES:
*******************************************************************************/
 /**
  * @brief METROLOGY Power definition
  *
  */
typedef enum
{
  W_ACTIVE = 1,
  REACTIVE,
  APPARENT,
}METRO_Power_selection_t;



/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
void Metro_Read_Power(METRO_Power_selection_t in_Metro_Power_Selection, METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power);
void Metro_Read_RMS(METRO_Channel_t in_Metro_Channel, float * out_Metro_RMS_voltage, float * out_Metro_RMS_current);
void Metro_Read_Frequency(METRO_Channel_t in_Metro_Channel, float *raw_freq);
void Metro_Read_Power_Factor(METRO_Channel_t in_Metro_Channel, float *cal_pf);
void Metro_Read_Energy(METRO_Power_selection_t in_Metro_Power_Selection, METRO_Channel_t in_Metro_Channel, double *calc_energy);
uint8_t Metro_Get_Energy(void);
uint8_t Metro_SPI_Comm_Init(void);
void Metro_AD_Reset(void);
uint8_t Metro_read_data_regs(void);
uint8_t Metro_get_Interrupt_Status(uint32_t *status0_val);
uint8_t Metro_Clear_Interrupt_Status(void);
uint8_t Metro_Start_Waveform(void);
uint8_t Metro_read_Waveform_data(void);
uint8_t Metro_AD_Init(void);
uint8_t METRO_AD_Init(void);
void Metro_Get_Energy_Sign(METRO_Channel_t in_Metro_Channel, uint16_t *phase_sign_reg);
uint8_t Metro_Read_Energy_reg(METRO_Channel_t in_Metro_Channel);
void Metro_AD_Stop(void);
uint8_t Metro_Write_EOL_Calibration_Params(BreakerEOLCalibration_st *pBreakerEOLCalibration);
uint8_t Metro_Update_Calib_Fact();

#endif /* __METROLOGY_H */

