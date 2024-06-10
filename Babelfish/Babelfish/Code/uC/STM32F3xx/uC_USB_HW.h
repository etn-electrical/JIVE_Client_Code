/**
 *****************************************************************************************
 *	@file
 *
 *	@brief		Manages the USB peripheral of the STM32F2xx microcontroller.
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_USB_H_
#define UC_USB_H_

#include "Includes.h"
#include "Advanced_Vect.h"
#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "Exception.h"

/**
 * @brief Class represents a USB (Universal Serial Bus) communication peripheral.
 */
class uC_USB_HW
{
	public:
		/**
		 * @brief Type definition of interrupt callback function pointer.
		 */
		typedef void (* int_cback_t)( void );

		/**
		 * @brief Type definition of parameter for callback function.
		 */
		typedef void* cback_param_t;

		/**
		 * @brief Type definition of callback function pointer.
		 */
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief Constructs an instance of the class.
		 * @param usb_port_id: USB port number e.g. uC_BASE_HS_FS_USB_PORT.
		 * @param int_handler: Interrupt callback function e.g. HAL_PCD_IRQHandler.
		 * @param param: Parameter to callback, e.g. PCD_HandleTypeDef hpcd.
		 * @param priority: Interrupt priority, e.g. uC_INT_HW_PRIORITY_15
		 */
		uC_USB_HW( uint8_t usb_port_id,
				   cback_func_t int_handler,
				   cback_param_t param,
				   uint16_t priority );

		/*
		 * @brief Destroys an instance of the class.
		 */
		~uC_USB_HW();

		/*
		 * @brief Returns the state of the Vbus pin (indicates whether USB power
		 * is present or not).
		 * @return Returns true if USB power is present else returns false.
		 * @note To access this function, include uC_USB_HW.h, add "extern
		 * uC_USB_HW* g_uC_USB", and then call g_uC_USB->USB_Power_Is_Present().
		 */
		bool USB_Power_Is_Present( void ) const;

	private:
		/*
		 * @brief Privatized copy constructor.
		 */
		uC_USB_HW( const uC_USB_HW& object );

		/*
		 * @brief Privatized assignment operator.
		 */
		uC_USB_HW & operator =( const uC_USB_HW& object );

		/*
		 * @brief Points to the configuration structure.
		 */
		uC_BASE_USB_IO_STRUCT const* m_io_ctrl;

		/**
		 * @brief Points to the interrupt vector object.
		 */
		Advanced_Vect* m_int_vect_cback;
};

#endif	/* UC_USB_H_ */
