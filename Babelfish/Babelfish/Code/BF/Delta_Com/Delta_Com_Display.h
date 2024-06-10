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
#ifndef DELTA_COM_DISPLAY_H_
#define DELTA_COM_DISPLAY_H_

#include "Includes.h"
#include "Bi_Color_LED.h"

class Delta_Com_Display
{
	public:
		/**
		 * @brief Constructor for the Delta Com Display class.
		 * @param bi_led:  This Bi Color LED control shall have Green as color one and
		 * Red as color two.
		 */
		Delta_Com_Display( BF_Misc::Bi_Color_LED* bi_led );

		/**
		 * @brief This is a null constructor.  It will initialize a null Bi Led Control
		 */
		Delta_Com_Display( void );

		/**
		 * @brief Destructor.
		 */
		virtual ~Delta_Com_Display( void );

		/**
		 * @brief Sets display to idle mode.
		 */
		virtual void Idle( void );

		/**
		 * @brief Sets display to discovery mode.
		 */
		virtual void Discovery( void );

		/**
		 * @brief Sets display to active mode.
		 */
		virtual void Active( void );

		/**
		 * @brief Sets display to faulted mode.
		 */
		virtual void Faulted( void );

	protected:
		enum current_status_t
		{
			IDLE, DISCOVERY, ACTIVE, FAULTED, INITIAL
		};

		static const BF_Lib::Timers::TIMERS_TIME_TD ONLINE_ON_FLASH_TIME = 1450U;
		static const BF_Lib::Timers::TIMERS_TIME_TD ONLINE_OFF_FLASH_TIME = 50U;

		static const BF_Lib::Timers::TIMERS_TIME_TD OFFLINE_ON_FLASH_TIME = 50U;
		static const BF_Lib::Timers::TIMERS_TIME_TD OFFLINE_OFF_FLASH_TIME = 1450U;

		static const BF_Lib::Timers::TIMERS_TIME_TD DISCOVERY_FLASH_TIME = 50U;

		static const BF_Lib::Timers::TIMERS_TIME_TD MAJOR_FAULT_FLASH_TIME = 1000U;

		current_status_t m_current_status;

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Delta_Com_Display( const Delta_Com_Display& object );
		Delta_Com_Display & operator =( const Delta_Com_Display& object );

		static const BF_Misc::Bi_Color_LED::bi_color_state_t GREEN_LED =
			BF_Misc::Bi_Color_LED::COLOR_ONE;
		static const BF_Misc::Bi_Color_LED::bi_color_state_t RED_LED =
			BF_Misc::Bi_Color_LED::COLOR_TWO;
		static const BF_Misc::Bi_Color_LED::bi_color_state_t COMBINED =
			BF_Misc::Bi_Color_LED::COMBINED_COLOR;

		BF_Misc::Bi_Color_LED* m_bi_led;
};

class Delta_Com_Display_Null : public Delta_Com_Display
{
	public:
		/**
		 * @brief Constructor for essentially an empty DCOM Display Control.
		 */
		Delta_Com_Display_Null( void ) :
			Delta_Com_Display( reinterpret_cast<BF_Misc::Bi_Color_LED*>( nullptr ) ) {}

		/**
		 * @brief Destructor.
		 */
		~Delta_Com_Display_Null( void ) {}

		/**
		 * @brief Sets display to idle mode.
		 */
		void Idle( void ) {}

		/**
		 * @brief Sets display to discovery mode.
		 */
		void Discovery( void ) {}

		/**
		 * @brief Sets display to active mode.
		 */
		void Active( void ) {}

		/**
		 * @brief Sets display to faulted mode.
		 */
		void Faulted( void ) {}

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Delta_Com_Display_Null( const Delta_Com_Display_Null& object );
		Delta_Com_Display_Null & operator =( const Delta_Com_Display_Null& object );

};


#endif

