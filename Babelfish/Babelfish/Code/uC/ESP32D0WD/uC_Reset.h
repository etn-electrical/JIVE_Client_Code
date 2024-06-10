/**
 *****************************************************************************************
 *	@file		uC_Reset.h
 *
 *	@brief		The file shall wrap all the functions which are required for the
 *	@n			resetting the micro controller
 *
 *	@copyright	2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef uC_RESET_H
#define uC_RESET_H

#include "uC_Base.h"
#include "esp32/rom/rtc.h"
#include "esp_system.h"

/**
 ****************************************************************************************
 * @brief	ESP32 Reset trigger details
 *
 ****************************************************************************************
 */
enum
{
	RESET_POWER_UP_TRIGRD,				///< Power up Trigger
	RESET_WAKEUP_TRIGRD,				///< Wakeup Trigger
	RESET_WATCHDOG_TRIGRD,				///< Watchdog Trigger
	RESET_SOFTWARE_TRIGRD,				///< Software Trigger
	RESET_USER_PIN_TRIGRD,				///< External Pin Trigger
	RESET_BROWN_OUT_TRIGRD,				///< Brown Out Trigger
	RESET_PANIC,						///< Software reset due to exception/panic
	RESET_TASK_WDT,						///< Reset due to task watchdog
	RESET_WDT,							///< Reset due to other watchdogs
	RESET_SDIO,							///< Reset over SDIO
	RESET_UNDEFINED						///< Undefined Maximum

};

/**
 ****************************************************************************************
 * @brief	Typedef for cause of the last reset
 *
 ****************************************************************************************
 */
typedef uint32_t uC_RESET_CAUSE_TD;

/**
 * @brief					uC_Reset Class Declaration. The class will include all the
 * @n						features of the micro controller Reset functionality
 */
class uC_Reset
{
	public:
		/**
		 * @brief						Constructor to create an instance of uC_Reset class.
		 * @n							When the object is created using this constructor, the constructor
		 * @n							finds out the cause of last micro controller reset by reading the
		 * @n							micro controller status register and update the reset cause in
		 * @n							global variable "m_reset_id"
		 */
		uC_Reset( void );

		/**
		 * @brief						Destructor to delete the uC_Reset instance when it goes out of scope
		 * @return						None
		 */
		~uC_Reset( void ){}

		/**
		 * @brief						The function puts the controller in soft reset condition.
		 * @n							Integrator should use this function when he wants to perform soft reset
		 * @return						None
		 */
		static void Soft_Reset( void );

		/**
		 * @brief						This function invokes watchdog reset
		 * @return						None
		 */
		static void Watchdog_Reset( void );

		/**
		 * @brief							Function returns the cause of the last reset
		 * @return[out] uC_RESET_CAUSE_TD	Returns the reset cause information
		 */
		static uC_RESET_CAUSE_TD Get_Reset_Trigger( void );

		/**
		 * @brief							This function invokes watchdog reset
		 * @n								It put the system/micro in exception state and triggers watchdog reset
		 * @n								Don't use this if you don't want to put micro controller in exception state
		 * @return							None
		 */
		static void Periph_Reset( void );

		/**
		 * @brief							This function enables the software reset
		 * @return Void						None
		 */
		static void Enable_SW_Reset();

		/**
		 * @brief							This function get the particular bit of reset details
		 * @param[in] var_val				Variable to be read
		 * @param[in] val					Bit Value details
		 * @return bool						True or false
		 */
		static bool Get_Bit( uint32_t var_val, uint32_t bit_val );

	private:
		/*
		 *****************************************************************************************
		 *		Constants
		 *****************************************************************************************
		 */
		static const uint8_t ENABLE_VALUE = 1U;			///< Enable value
		static const uint8_t DISABLE_VALUE = 0U;		///< Disable value
		static const uint8_t C1S_SHIFT_VALUE = 0x21U;	///< C1S left shift value
		static const uint8_t C0S_SHIFT_VALUE = 2U;		///< C0S left shift value

		/**
		 * @brief							This function sets the particular bit of reset details
		 * @param[in] val					Value to be set
		 * @return							None
		 */
		static void Set_Bit_Reset( uint8_t val );

		/**
		 * @brief							This function get the particular bit of reset details
		 * @return Void						None
		 */
		static esp_reset_reason_t ESP32_Get_Reset_Reason();

		static uC_RESET_CAUSE_TD m_reset_id;			///< The variable which hold the status of last reset
};

#endif

#ifdef __cplusplus
}
#endif
