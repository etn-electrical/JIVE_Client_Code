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
#ifndef UC_USB_H_
#define UC_USB_H_

#include "Includes.h"
#include "uC_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "Exception.h"

class uC_USB
{
	public:
		uC_USB( uint8_t usb_channel );
		~uC_USB();

	private:
		uC_BASE_USB_IO_STRUCT const* m_io_ctrl;
};

#endif	/* UC_USB_H_ */
