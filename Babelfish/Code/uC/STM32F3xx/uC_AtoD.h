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
#ifndef uC_ATOD_H
   #define uC_ATOD_H

#include "uC_Interrupt.h"
#include "uC_DMA.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
typedef enum uC_ATOD_RESOLUTION_ENUM
{
	uC_ATOD_6BIT_RESOLUTION,
	uC_ATOD_8BIT_RESOLUTION,
	uC_ATOD_10BIT_RESOLUTION,
	uC_ATOD_12BIT_RESOLUTION,
	uC_ATOD_MAX_RESOLUTION
} uC_ATOD_RESOLUTION_ENUM;


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_AtoD
{
	public:
		uC_AtoD( uC_ATOD_RESOLUTION_ENUM resolution,
				 uint32_t active_channels_mask,
				 uC_BASE_ATOD_CTRL_ENUM atod_block = uC_BASE_ATOD_CTRL_1 );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_AtoD() {}

		/// Do one single scan.
		void Start_Scan( void ) const;

		bool Scan_Done( void ) const;

		void Start_Repeat_Scan( void ) const;

		uint16_t Get_Channel( uint8_t channel ) const;				// Always returns a 16bit value no matter the
																	// resolution.

	// static void Start_Scan_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
	// { ((uC_AtoD*)handle)->Start_Scan(); }
	// void 		Start_Interval_Scan( UINT32 interval_time );


	//
	//
	//
	// BOOL		Channel_Idle( uint8_t channel );
	//
	// void 		Assign_Scan_Done_Int( INT_CALLBACK_FUNC* int_callback_func, uint8_t priority );
	// void		Enable_Int( void )		{ uC_Interrupt::Enable_Int( m_pid ); }
	// void		Disable_Int( void )		{ uC_Interrupt::Disable_Int( m_pid ); }

	private:
		void Dump_AtoD_Data( void ) const;

		uC_BASE_ATOD_IO_STRUCT const* m_adc;
		uint8_t m_resolution;
		uint16_t* m_channel_buffer;
		uC_DMA* m_dma_ctrl;
		uint8_t* m_channel_lookup;
		// Timers*			m_scan_timer;
};



#endif
