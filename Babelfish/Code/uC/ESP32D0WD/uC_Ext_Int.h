/**
 *****************************************************************************************
 *	@file     uC_Ext_Int.h
 *
 *	@brief    This file provides the prototypes for external interrupt configuration for
 *            enable, disable and clear interrupt
 *
 *	@copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_EXT_INT_H
#define uC_EXT_INT_H

#include "uC_Base.h"
#include "uC_IO_Config.h"

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef void INT_CALLBACK_FUNC ( void );

/*
 **************************************************************************************************
 *  Class Declaration
 **************************************************************************************************
 */
class uC_Ext_Int
{
	public:
		/**
		 * @brief                   Constructor to create instance of uC_Ext_Int class.
		 * @param[in] pio_ctrl      Structure pointer for configure IO as an external interrupt.
		 * @return                  None
		 */
		uC_Ext_Int( uC_USER_IO_STRUCT const* pio_ctrl );

		/**
		 * @brief                   Destructor to delete an instance of uC_Ext_Int class
		 * @return                  None
		 */
		~uC_Ext_Int() {}

		/**
		 * @brief                      Allocates interrupt and attaches callback
		 * @param[in] ext_int_callback Interrupt config structure pointer to allocate interrupt
		 *                             and handler
		 * @param[in] int_type         Sets interrupt triggering method
		 * @param[in] input_cfg        Not used
		 * @param[in] priority         Sets interrupt priority
		 * @return                     None
		 */
		void Attach_IRQ( INT_CALLBACK_FUNC* ext_int_callback, uint8_t int_type,
						 uC_IO_INPUT_CFG_EN input_cfg, uint8_t priority ) const;

		/**
		 * @brief         Enable interrupt
		 * @return        None
		 */
		void Enable_Int( void ) const;

		/**
		 * @brief         Disable interrupt
		 * @return        None
		 */
		void Disable_Int( void ) const;

		/**
		 * @brief         Clear interrupt
		 * @return        None
		 */
		void Clear_Int( void ) const;

	private:
		/**
		 * @brief         Pointer to GPIO user structure.
		 */
		uC_USER_IO_STRUCT const* m_ext_pio_ctrl;
};


#endif



