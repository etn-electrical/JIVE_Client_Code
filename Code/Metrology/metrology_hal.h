/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : metrology_hal.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/19/2022
 *
 ****************************************************************************/

#ifndef __METROLOGY_HAL_H
#define __METROLOGY_HAL_H

/*******************************************************************************
* INCLUDE FILES:
*******************************************************************************/
#include "metro_utilities.h"
#include "SBLCP.h"
/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/
#define MILLI_DIV_FACTOR 		(double)1000.00
#define MAX_WAVEFORM_CYCLES 8

#define CURRENT_CONV_FACT 21.36473467
#define VOLTAGE_CONV_FACT 18.3026904
#define POWER_CONV_FACT 52.48344288
#define ENERGY_CONV_FACT 29.8572475

#define MAX_CONFIG_REG_16 9
#define MAX_CONFIG_REG_32 2

#define MAX_REQUIRED_DATA_REG 14
#define MAX_ENERGY_REG_CH1 3
#define MAX_ENERGY_REG 6
#define MAX_CALIB_REG 10

#define ZERO_ENERGY 0

/*******************************************************************************
* TYPES:
*******************************************************************************/
typedef struct
{
//	32 bit RW reg

  uint32_t CONFIG0;                     /*!< Configuration Register 0,	                    	Address offset: 0x060 */
  uint32_t AIRMS;                       /*!< Phase A current rms,	           					Address offset: 0x20C */
  uint32_t AVRMS;                       /*!< Phase A voltage rms,	            				Address offset: 0x20D */
  uint32_t AWATT;                       /*!< Phase A total active power,	                    Address offset: 0x210 */
  uint32_t AFVAR;                       /*!<  Phase A total reactive power,	                    Address offset: 0x214 */
  uint32_t AVA;                       	/*!< Phase A total apparent power,	                    Address offset: 0x212 */
  uint32_t APF;                       	/*!< Phase A power factor,	                        	Address offset: 0x21E */
  uint32_t BIRMS;                       /*!< Phase B current rms,	           					Address offset: 0x22C */
  uint32_t BVRMS;                       /*!< Phase B voltage rms,	            				Address offset: 0x22D */
  uint32_t BWATT;                       /*!< Phase B total active power,	                    Address offset: 0x230 */
  uint32_t BFVAR;                       /*!<  Phase B total reactive power,	                    Address offset: 0x234 */
  uint32_t BVA;                       	/*!< Phase B total apparent power,	                    Address offset: 0x232 */
  uint32_t BPF;                       	/*!< Phase B power factor,	                        	Address offset: 0x23E */
  uint32_t AWATTHR_L0;					/*!< Phase A active energy lower 16 bits,	            Address offset: 0x2E6 */
  uint32_t AWATTHR_HI;					/*!< Phase A active energy higher 32 bits,	            Address offset: 0x2E7 */
  uint32_t AVAHR_L0;					/*!< Phase A apparent energy lower 16 bits,	            Address offset: 0x2FA */
  uint32_t AVAHR_HI;					/*!< Phase A apparent energy higher 32 bits,	        Address offset: 0x2FB2 */
  uint32_t AFVARHR_L0;					/*!< Phase A reactive energy lower 16 bits,	            Address offset: 0x30E */
  uint32_t AFVARHR_HI;					/*!< Phase A reactive energy higher 32 bits,	        Address offset: 0x30F */
  uint32_t BWATTHR_L0;					/*!< Phase B active energy lower 16 bits,	            Address offset: 0x322 */
  uint32_t BWATTHR_HI;					/*!< Phase B active energy higher 32 bits,	            Address offset: 0x323 */
  uint32_t BVAHR_L0;					/*!< Phase B apparent energy lower 16 bits,	            Address offset: 0x336 */
  uint32_t BVAHR_HI;					/*!< Phase B apparent energy higher 32 bits,	        Address offset: 0x337 */
  uint32_t BFVARHR_L0;					/*!< Phase B reactive energy lower 16 bits,	            Address offset: 0x34A */
  uint32_t BFVARHR_HI;					/*!< Phase B reactive energy higher 32 bits,	        Address offset: 0x30B */
  uint32_t IPEAK;                       /*!< Current peak register,	                        	Address offset: 0x400 */
  uint32_t VPEAK;                       /*!< Voltage peak register.,	                        Address offset: 0x401 */
  uint32_t STATUS0;                     /*!< Status Register 0,	                				Address offset: 0x402 */
  uint32_t STATUS1;                     /*!< Status Register 1,	                				Address offset: 0x403 */
  uint32_t EVENT_STATUS;                /*!< Event Status Register,	                			Address offset: 0x404 */
  uint32_t MASK0;                       /*!< Interrupt Enable Register 0,	                    Address offset: 0x405 */
  uint32_t MASK1;                       /*!< Interrupt Enable Register 1,	                    Address offset: 0x406 */
  uint32_t EVENT_MASK;                  /*!< Event enable register,	                            Address offset: 0x407 */
  uint32_t USER_PERIOD;                 /*!< User configured line period,	                    Address offset: 0x40E */
  uint32_t APERIOD;                     /*!< Line period on Phase A voltage,	            	Address offset: 0x044 */
  uint32_t BPERIOD;                     /*!< Line period on Phase B voltage,	                Address offset: 0x048 */
  uint32_t COM_PERIOD;                  /*!< Line period measurement on Phase A,B	        	Address offset: 0x04C */

//  16 bit reg

  uint16_t RUN;                      	/*!< start the measurements ,	                        Address offset: 0x480 */
  uint16_t CONFIG1;                    	/*!< Configuration Register 1,	                        Address offset: 0x481 */
  uint16_t CFMODE;                      /*!< CFx configuration register,	                    Address offset: 0x490 */
  uint16_t COMPMODE;                    /*!< Computation mode register.,                        Address offset: 0x491 */
  uint16_t CONFIG3;                     /*!<Configuration Register 3,                           Address offset: 0x493 */
  uint16_t ZXTOUT;                     	/*!< Zero-crossing timeout config register,             Address offset: 0x498 */
  uint16_t ZXTHRSH;                    	/*!< Voltage channel zero-crossing threshold register,  Address offset: 0x499 */
  uint16_t ZX_LP_SEL;                   /*!< zero crossing & line period,                   	Address offset: 0x49A */
  uint16_t PHSIGN;                      /*!< Power sign register,                              	Address offset: 0x49D */
  uint16_t WFB_CFG;                     /*!< Waveform buffer configuration register,          	Address offset: 0x4A0 */
  uint16_t CONFIG5;                     /*!< Configuration Register 5,                        	Address offset: 0x4A4 */
  uint16_t CRC_RSLT;                    /*!< CRC of configuration registers,                 	Address offset: 0x4A8*/
  uint16_t CRC_SPI;                     /*!< 16-bit CRC of the data sent out,                  	Address offset: 0x4A9*/
  uint16_t LAST_DATA_16;                /*!<16-bit transaction on the SPI port,           		Address offset: 0x4AC*/
  uint16_t LAST_CMD;                    /*!< Hold read/write operation last cmd,              	Address offset: 0x4AE*/
  uint16_t CONFIG2;                     /*!< Configuration Register 2,                         	Address offset: 0x4AF*/
  uint16_t PWR_TIME;                    /*!< Power update time configuration,                 	Address offset: 0x4B1*/
  uint16_t EGY_TIME;                    /*!< Energy update time configuration,                	Address offset: 0x4B2*/
  uint16_t CRC_FORCE;                   /*!< forces an update of the CRC,                    	Address offset: 0x4B4*/
  uint16_t CRC_OPTEN;                   /*!< Optionally included registers,                  	Address offset: 0x4B5 */
  uint16_t Version;                     /*!< chip version of the ADE9039 IC,                   	Address offset: 0x4FE */

//	32 bit SPI Burst read accessible reg

  uint32_t AIRMS_1;                      /*!< SPI burst read accessible,	           		    Address offset: 0x607 */
  uint32_t AVRMS_1;                      /*!< SPI burst read accessible,	            		Address offset: 0x60A */
  uint32_t AWATT_1;                      /*!< SPI burst read accessible,	                    Address offset: 0x60E */
  uint32_t AFVAR_1;                      /*!< SPI burst read accessible,	                    Address offset: 0x617 */
  uint32_t AVA_1;                        /*!< SPI burst read accessible,	                    Address offset: 0x611 */
  uint32_t APF_1;                        /*!< SPI burst read accessible,	                    Address offset: 0x61A */
  uint32_t BIRMS_1;                      /*!< SPI burst read accessible,	           			Address offset: 0x608 */
  uint32_t BVRMS_1;                      /*!< SPI burst read accessible,	            		Address offset: 0x60B */
  uint32_t BWATT_1;                      /*!< SPI burst read accessible,	                    Address offset: 0x60F */
  uint32_t BFVAR_1;                      /*!< SPI burst read accessible,	                    Address offset: 0x618 */
  uint32_t BVA_1;                        /*!< SPI burst read accessible,	                    Address offset: 0x612 */
  uint32_t BPF_1;                        /*!< SPI burst read accessible,	                    Address offset: 0x61B */
 }METRO_AD_reg;

/*******************************************************************************
 * FUNCTIONS:
 *******************************************************************************/
void Metro_HAL_read_RMS_Voltage(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Voltage);
void Metro_HAL_read_RMS_Current(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Current);
void Metro_HAL_read_Active_Power(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power);
void Metro_HAL_read_Reactive_Power(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power);
void Metro_HAL_read_Apparent_Power(METRO_Channel_t in_Metro_Channel, float *calc_RMS_Power);
void Metro_HAL_read_Power_Factor(METRO_Channel_t in_Metro_Channel, float *cal_pf);
void Metro_HAL_read_Period(METRO_Channel_t in_Metro_Channel, uint32_t *raw_period);
void Metro_HAL_read_Active_Energy(METRO_Channel_t in_Metro_Channel, uint32_t *raw_energy);
void Metro_HAL_read_Reactive_Energy(METRO_Channel_t in_Metro_Channel, uint32_t *raw_energy);
void Metro_HAL_read_Apparent_Energy(METRO_Channel_t in_Metro_Channel, uint32_t *raw_energy);
uint8_t Metro_HAL_set_Channel_Gain(void);
uint8_t Metro_HAL_set_Config_regs(void);
uint8_t Metro_HAL_Write_Calibration_regs(void);
uint8_t Metro_HAL_Start_SPI_Com(void);
uint8_t Metro_HAL_get_Status_reg(uint32_t *status0_val);
uint8_t Metro_HAL_read_Energy_reg(METRO_Channel_t in_Metro_Channel);
uint8_t Metro_HAL_set_Status_reg(void);
uint8_t Merto_HAL_SPI_Init(void);
uint8_t Metro_HAL_read_required_reg(void);
uint8_t Metro_HAL_Waveform_Cap_En(void);
uint8_t Metro_HAL_Waveform_Cap_Dis(void);
uint8_t Metro_HAL_read_Waveform_burst_data(void);
void Metro_HAL_read_Phase_Sign(METRO_Channel_t in_Metro_Channel, uint16_t *phase_sign);
uint8_t Metro_HAL_Redirect_Channels(void);
uint8_t Metro_HAL_Write_EOL_Calibration_Params(BreakerEOLCalibration_st *pBreakerEOLCalibration);
#endif /* __METROLOGY_HAL_H */

