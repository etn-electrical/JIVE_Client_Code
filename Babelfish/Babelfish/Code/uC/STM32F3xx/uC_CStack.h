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
#ifndef uC_CSTACK_H
   #define uC_CSTACK_H

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

void uC_CSTACK_Init( void );

/**
 * @brief Returns the currently used percentage of the CSTACK.  This is achieved by filling the
 * stack on powerup with a known value and then scanning to find the peak.  The peak adapts
 * so that it is not scanning too much or too often.
 *
 * @return Returns the percent used.
 */
uint8_t uC_CSTACK_Used( void );


#endif
