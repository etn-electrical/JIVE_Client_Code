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
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/

#ifndef __METROLOGY_H
#define __METROLOGY_H

#include "metro_utilities.h"
#include "SBLCP.h"

/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/
//#define METRO_ENERGY_ACC_DEBUG
#define MICRO_DIV_FACTOR 1000000

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
void Metro_AD_Stop(void);
void Metro_AD_Reset(void);
void Metro_Read_Power(METRO_Power_selection_t in_Metro_Power_Selection, METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power);
void Metro_Read_RMS(METRO_Channel_t in_Metro_Channel, float * out_Metro_RMS_voltage, float * out_Metro_RMS_current);
void Metro_Read_Frequency(METRO_Channel_t in_Metro_Channel, float *raw_freq);
void Metro_Read_Power_Factor(METRO_Channel_t in_Metro_Channel, float *cal_pf);
void Metro_Read_Energy(METRO_Power_selection_t in_Metro_Power_Selection, METRO_Channel_t in_Metro_Channel, double *calc_energy);

metro_err_t Metro_SPI_Comm_Init(void);
metro_err_t Metro_Get_Measures(void);
metro_err_t Metro_get_Interrupt_Status(uint32_t *status0_val);
metro_err_t Metro_Clear_Interrupt_Status(uint32_t value);
metro_err_t Metro_Start_Waveform(void);
metro_err_t Metro_read_Waveform_data(void);
metro_err_t Metro_AD_Init(void);
metro_err_t Metro_Get_Energy_Sign(METRO_Channel_t in_Metro_Channel, uint16_t *phase_sign_reg);
metro_err_t Metro_Read_Energy_reg(METRO_Channel_t in_Metro_Channel);
metro_err_t Metro_Update_Calib_Fact(void);
metro_err_t Metro_set_Energy_Ready_time(uint16_t timer_count);

#endif /* __METROLOGY_H */

