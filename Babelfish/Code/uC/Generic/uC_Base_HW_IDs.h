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
#ifndef uC_BASE_HW_IDS_H
   #define uC_BASE_HW_IDS_H

// Please reference the appropriate H file for the hardware you intend to use.
// The include file needs to be placed below to include the appropriate hardware identifications.

#ifdef uC_STM32F030_USAGE
	#include "uC_Base_HW_IDs_STM32F030.h"
#endif

#ifdef uC_STM32F100_USAGE
	#include "uC_Base_HW_IDs_STM32F100.h"
#endif

#ifdef uC_STM32F101_USAGE
	#include "uC_Base_HW_IDs_STM32F101.h"
#endif

#ifdef uC_STM32F105_USAGE
	#include "uC_Base_HW_IDs_STM32F105.h"
#endif

#ifdef uC_STM32F107_USAGE
	#include "uC_Base_HW_IDs_STM32F107.h"
#endif

#ifdef uC_STM32F407_USAGE
	#include "uC_Base_HW_IDs_STM32F407.h"
#endif

#ifdef uC_STM32F411_USAGE
	#include "uC_Base_HW_IDs_STM32F411.h"
#endif

#ifdef uC_STM32F427_USAGE
	#include "uC_Base_HW_IDs_STM32F427.h"
#endif

#ifdef uC_STM32F437_USAGE
	#include "uC_Base_HW_IDs_STM32F437.h"
#endif

#ifdef uC_STM32F302_USAGE
	#include "uC_Base_HW_IDs_STM32F302.h"
#endif

#ifdef uC_STM32F207_USAGE
	#include "uC_Base_HW_IDs_STM32F207.h"
#endif

#ifdef uC_STM32F205_USAGE
	#include "uC_Base_HW_IDs_STM32F205.h"
#endif

#ifdef uC_STM32F373_USAGE
	#include "uC_Base_HW_IDs_STM32F373.h"
#endif

#ifdef uC_STM32F767_USAGE
	#include "uC_Base_HW_IDs_STM32F767.h"
#endif

#ifdef uC_STM32H743_USAGE
	#include "uC_Base_HW_IDs_STM32H743.h"
#endif

#ifdef uC_KINETISK66x_USAGE
	#include "uC_Base_HW_IDs_K66F18.h"
#endif

#ifdef uC_KINETISK60x_USAGE
	#include "uC_Base_HW_IDs_K60F12.h"
#endif

#ifdef uC_KINETISK60DNx_USAGE
	#include "uC_Base_HW_IDs_K60D10.h"
#endif

#ifdef uC_STM32L451_USAGE
	#include "uC_Base_HW_IDs_STM32L451.h"
#endif

#ifdef uC_STM32WB55_USAGE
	#include "uC_Base_HW_IDs_STM32WB55R.h"
#endif

#ifdef uC_STM32MP157_USAGE
	#include "uC_Base_HW_IDs_STM32MP157.h"
#endif
#endif
