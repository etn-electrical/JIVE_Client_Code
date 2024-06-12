/**
 **********************************************************************************************
 * @file            uC_AtoD.h Header File for ADC Driver implementation.
 *
 * @brief           The file contains uC_AtoD Class required for communication
 *                  over ADC channels on STM32F20x.
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef uC_ATOD_H
#define uC_ATOD_H

#include "uC_Interrupt.h"
#include "uC_DMA.h"

/**
 * @brief Various ADC resolution options
 */
typedef enum uC_ATOD_RESOLUTION_ENUM
{
	/** @brief AtoD conversion with 6 bit resolution */
	uC_ATOD_6BIT_RESOLUTION,
	/** @brief AtoD conversion with 8 bit resolution */
	uC_ATOD_8BIT_RESOLUTION,
	/** @brief AtoD conversion with 10 bit resolution */
	uC_ATOD_10BIT_RESOLUTION,
	/** @brief AtoD conversion with 12 bit resolution */
	uC_ATOD_12BIT_RESOLUTION,
	uC_ATOD_MAX_RESOLUTION
} uC_ATOD_RESOLUTION_ENUM;

#define ADC_STABILIZATION_TIME_uS       5U

/**
 **********************************************************************************************
 * @brief        AtoD Class Declaration. The class will handle all the
 *               functionalities related to A2D conversion.
 **********************************************************************************************
 */
class uC_AtoD
{

	public:
		/**
		 * @brief                             Constructor to create instance of uC_AtoD class.
		 * @param[in] resolution              Resolution of the AtoD conversion. Possible values are mentioned below :
		 * @n                                 uC_ATOD_8BIT_RESOLUTION : Selects 8 Bit Resolution
		 * @n                                 uC_ATOD_16BIT_RESOLUTION : Selects 16 Bit Resolution
		 * @param[in] active_channels_mask    Specifies ADC channels used in application. STM32F207 features total 19
		 *ADC channels (channel 0 to channel 18).
		 * @n                                 For each active channel, corresponding bit should be set.
		 * @n                                 If channel 0 and 2 are to be used, value of active_channels_mask should be
		 *0x00000005.
		 * @param[in] time_reqd_to_scan_all_channels_us    The amount of time in microseconds required to scan all of
		 *                                                 the channels so that one good data sample is acquired before
		 *                                                 the constructor exits.  Value must be non-zero or will
		 *                                                 go to HEL.
		 * @param[in] atod_block              Analog to Digital Channel control structure
		 * @return
		 * @n
		 */
		uC_AtoD( uC_ATOD_RESOLUTION_ENUM resolution,
				 uint32_t active_channels_mask,
				 uC_BASE_ATOD_CTRL_ENUM atod_block = uC_BASE_ATOD_CTRL_1,
				 uint16_t time_reqd_to_scan_all_channels_us = ADC_STABILIZATION_TIME_uS );

		/**
		 * @brief                   Destructor to delete the uc_AtoD instance when it goes out of scope
		 * @param[in] void          none
		 * @return
		 */
		~uC_AtoD() {}

		/**
		 * @brief                   This function turns on AtoD converter and starts conversion
		 * @param[in] void          none
		 * @return
		 */
		void Start_Scan( void ) const;

		/**
		 * @brief                   This function checks if AtoD conversion is finished
		 * @param[in] void          none
		 * @return bool             TRUE: AtoD conversion is finished
		 * @n                       FALSE: AtoD conversion is not finished
		 */
		bool Scan_Done( void ) const;

		/**
		 * @brief                   This function enables continuous AtoD conversion
		 * @param[in] void          none
		 * @return
		 */
		void Start_Repeat_Scan( void ) const;

		/**
		 * @brief                   This function gives AtoD value from a particular ADC channel
		 * @param[in] channel       ADC channel
		 * @return uint16_t         AtoD value read from ADC channel (Always returns a 16bit value no matter the
		 *resolution)
		 * @n                       The 16bit value is always left justified. A 10bit ADC will left justify
		 * @n                       the 10bit result to create a 16bit result with the lower 6bits set to 0.
		 */
		uint16_t Get_Channel( uint8_t channel ) const;

	private:

		/**
		 * @brief                   This function dumps ADC regular data register value into local variable
		 * @param[in] void          none
		 * @return
		 */
		void Dump_AtoD_Data( void ) const;

		/**
		 * @brief AtoD Control Definition Structure
		 */
		uC_BASE_ATOD_IO_STRUCT const* m_adc;
		/**
		 * @brief ***
		 */
		uint8_t m_resolution;
		/**
		 * @brief ***
		 */
		uint16_t* m_channel_buffer;
		/**
		 * @brief ***
		 */
		uC_DMA* m_dma_ctrl;
		/**
		 * @brief ***
		 */
		uint8_t* m_channel_lookup;
};

#endif


