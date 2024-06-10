/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_INTERRUPT_H
#define uC_INTERRUPT_H

#include "uC_Base.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

// Interrupt Priority Selection
static const uint8_t uC_INT_HW_PRIORITY_0 = 0U;		// Highest priority
static const uint8_t uC_INT_HW_PRIORITY_1 = 1U;
static const uint8_t uC_INT_HW_PRIORITY_2 = 2U;
static const uint8_t uC_INT_HW_PRIORITY_3 = 3U;
static const uint8_t uC_INT_HW_PRIORITY_4 = 4U;
static const uint8_t uC_INT_HW_PRIORITY_5 = 5U;
static const uint8_t uC_INT_HW_PRIORITY_6 = 6U;
static const uint8_t uC_INT_HW_PRIORITY_7 = 7U;
static const uint8_t uC_INT_HW_PRIORITY_8 = 8U;
static const uint8_t uC_INT_HW_PRIORITY_9 = 9U;
static const uint8_t uC_INT_HW_PRIORITY_10 = 10U;
static const uint8_t uC_INT_HW_PRIORITY_11 = 11U;
static const uint8_t uC_INT_HW_PRIORITY_12 = 12U;
static const uint8_t uC_INT_HW_PRIORITY_13 = 13U;
static const uint8_t uC_INT_HW_PRIORITY_14 = 14U;
static const uint8_t uC_INT_HW_PRIORITY_15 = 15U;		// Lowest Priority
static const uint8_t uC_INT_HW_MAX_PRIORITY = 16U;

static const uint8_t uC_INT_HW_PRIORITY_LOWEST = ( uC_INT_HW_MAX_PRIORITY - 1U );
static const uint8_t uC_INT_HW_PRIORITY_HIGHEST = uC_INT_HW_PRIORITY_0;

#define INT_VECT_N14    ( NonMaskableInt_IRQn + 0x10 )
#define INT_VECT_N13    ( HardFault_IRQn + 0x10 )
#define INT_VECT_N5     ( SVC_IRQn + 0x10 )
#define INT_VECT_N2     ( PendSV_IRQn + 0x10 )
#define INT_VECT_N1     ( SysTick_IRQn + 0x10 )
#define INT_VECT_0      ( WWDG_IRQn + 0x10 )
#define INT_VECT_2      ( RTC_IRQn + 0x10 )
#define INT_VECT_3      ( FLASH_IRQn + 0x10 )
#define INT_VECT_4      ( RCC_IRQn + 0x10 )
#define INT_VECT_5      ( EXTI0_1_IRQn + 0x10 )
#define INT_VECT_6      ( EXTI2_3_IRQn + 0x10 )
#define INT_VECT_7      ( EXTI4_15_IRQn + 0x10 )
#define INT_VECT_9      ( DMA1_Channel1_IRQn + 0x10 )
#define INT_VECT_10     ( DMA1_Channel2_3_IRQn + 0x10 )
#define INT_VECT_11     ( DMA1_Channel4_5_IRQn + 0x10 )
#define INT_VECT_12     ( ADC1_IRQn + 0x10 )
#define INT_VECT_13     ( TIM1_BRK_UP_TRG_COM_IRQn + 0x10 )
#define INT_VECT_14     ( TIM1_CC_IRQn + 0x10 )
#define INT_VECT_16     ( TIM3_IRQn + 0x10 )
#define INT_VECT_17     ( TIM6_IRQn + 0x10 )
#define INT_VECT_18     ( TIM7_IRQn + 0x10 )
#define INT_VECT_19     ( TIM14_IRQn + 0x10 )
#define INT_VECT_20     ( TIM15_IRQn + 0x10 )
#define INT_VECT_21     ( TIM16_IRQn + 0x10 )
#define INT_VECT_22     ( TIM17_IRQn + 0x10 )
#define INT_VECT_23     ( I2C1_IRQn + 0x10 )
#define INT_VECT_24     ( I2C2_IRQn + 0x10 )
#define INT_VECT_25     ( SPI1_IRQn + 0x10 )
#define INT_VECT_26     ( SPI2_IRQn + 0x10 )
#define INT_VECT_27     ( USART1_IRQn + 0x10 )
#define INT_VECT_28     ( USART2_IRQn + 0x10 )
#define INT_VECT_29     ( USART3_6_IRQn + 0x10 )

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef void INT_CALLBACK_FUNC ( void );

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Interrupt
{
	public:
		uC_Interrupt( void );
		~uC_Interrupt( void );

		static bool Set_Vector( INT_CALLBACK_FUNC* vector_callback, IRQn_Type vector_number,
								uint16_t priority );

		static void Clear_Vector( IRQn_Type vector_number );

		static bool Is_Vector_Empty( IRQn_Type vector_number );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Enable_Int( IRQn_Type vector_number )
		{
			NVIC_EnableIRQ( vector_number );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Disable_Int( IRQn_Type vector_number )
		{
			NVIC_DisableIRQ( vector_number );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static bool Is_Enabled( IRQn_Type vector_number )
		{
			return ( NVIC->ISER[0U]
					 & ( ( uint32_t )( 1UL << ( ( ( uint32_t )( int32_t )vector_number ) ) ) ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Clear_Int( IRQn_Type vector_number )
		{
			NVIC_ClearPendingIRQ( vector_number );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Set_Int( IRQn_Type vector_number )
		{
			NVIC_SetPendingIRQ( vector_number );
		}

	private:
};

#endif
