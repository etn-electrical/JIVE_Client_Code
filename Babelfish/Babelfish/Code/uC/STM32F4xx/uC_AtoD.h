/**
 **********************************************************************************************
 * @file            uC_AtoD.h Header File for ADC Driver implementation.
 *
 * @brief           The file contains uC_AtoD Class required for communication
 *                  over ADC channels on STM32F40x.
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
		 *  @brief Definitions for the different callback types.  int_cback is the
		 *  void argument function type.  The cback typedefs are associated with the
		 *  param argument passing function type.
		 */
		typedef void (* int_cback_t)( void );
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );
		/**
		 * @brief                             Constructor to create instance of uC_AtoD class.
		 * @param[in] resolution              Resolution of the AtoD conversion. Possible values are mentioned below :
		 * @n                                 uC_ATOD_8BIT_RESOLUTION : Selects 8 Bit Resolution
		 * @n                                 uC_ATOD_16BIT_RESOLUTION : Selects 16 Bit Resolution
		 * @param[in] active_channels_mask    Specifies ADC channels used in application. STM32F407 features total 19
		 * ADC channels (channel 0 to channel 18).
		 * @n                                 For each active channel, corresponding bit should be set.
		 * @n                                 If channel 0 and 2 are to be used, value of active_channels_mask should be
		 * 0x00000005.
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
		 * resolution)
		 * @n                       The 16bit value is always left justified. A 10bit ADC will left justify
		 * @n                       the 10bit result to create a 16bit result with the lower 6bits set to 0.
		 */
		uint16_t Get_Channel( uint8_t channel ) const;

		/**
		 * @brief                   This function attaches the interrupt callback details and enables the interrupt
		 * @param[in] func_callback 		 The function to be attached to the callback
		 * @param[in] func_param    		 The callback function parameter
		 * @param[in] int_priority			 The interrupt priority
		 * @return
		 */
		void Attach_Callback( cback_func_t func_callback, cback_param_t func_param,
							  uint8_t int_priority );

		/**
		 *  @brief      Configure the ADC interrupt priority.
		 *  @n          Declare and pass the call back function and Call back parameter
		 *  @param[in]  int_callback => Call back function to be used
		 *  @param[in]  priority => priority of the ADC interrrupt
		 *  @param[out] None
		 *  @return
		 */
		void Set_Callback( INT_CALLBACK_FUNC* int_callback, uint8_t int_priority ) const;

		/**
		 * @brief                   This function enables the ADC interrupt
		 * @param[in] void          none
		 * @return
		 */
		inline void Enable_Int( void )          { uC_Interrupt::Enable_Int( m_adc->irq_num ); }

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

		void Int_Handler( void );

		/**
		 * @brief                   This function sets the callback for the interrupt
		 * @param[in] void          Callback parameter
		 * @return
		 */
		static void Int_Handler_Static( Advanced_Vect::cback_param_t param )
		{ ( ( uC_AtoD* )param )->Int_Handler(); }

		Advanced_Vect* m_advanced_vect;
		cback_param_t m_cback_param;
		cback_func_t m_cback_func;
};

#endif


