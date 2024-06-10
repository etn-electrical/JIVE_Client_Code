/**
 *****************************************************************************************
 *	@file		uC_Base_HW_IDs.h
 *
 *	@brief		Contains the header file of uC_Base_HW_ID's
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_BASE_HW_IDS_H
   #define uC_BASE_HW_IDS_H
/**
 * @brief Please reference the appropriate H file for the hardware you intend to use.
 * The include file needs to be placed below to include the appropriate hardware identifications.
 */

#ifdef uC_ESP32_USAGE
	#include "uC_Base_HW_IDs_ESP32.h"
#endif

#endif
