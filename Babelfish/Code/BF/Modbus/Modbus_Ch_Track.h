/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Interface used to create the Change_Tracker for Modbus Registers
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_CH_TRACK_H
#define MODBUS_CH_TRACK_H

#include "Change_Tracker.h"
#include "DCI.h"
#include "DCI_Owner.h"
#include "Bit_List.h"
#include "Modbus_DCI.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief This is a Modbus_Ch_Tracker class
 * @details It provides an interface used to create the Change_Tracker for Modbus Registers
 * @n @b Usage:
 * The modbus change tracker uses a single register.  The single register provides both the
 * watch register entry and the resulting queue.
 * The queue control register returns the register number that has changed.
 *
 * A set of this register with the register number that you want to watch
 * will tell the change tracker to monitor that register value for change.
 * If the value changes that values register will be added to the change queue.  To Remove
 * the register number from the head of the queue you send a 0xFFFF.  If the queue is empty
 * a 0xFFFF will be returned.
 * The steps work out like this:
 * 1) Write to the change_register the register numbers you want to watch.
 * 2) Poll the change_register.
 * 		if the change_register is 0xFFFF then no watched values have changed.
 * 		if the change_register is not 0xFFFF then go retrieve the register number provided by the change_register.
 * 3) Write a 0xFFFF to dequeue the value from the change_register.  Goto 2.

 *
 ****************************************************************************************
 */
class Modbus_Ch_Track
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus Change Track
		 *  @param[in] modbus_dev_profile Defines the Modbus target,
		 *  This is a container for the entire identity of a Modbus Interface
		 *  @param[in] change_que_owner is the owner of DCI store to be tracked
		 *  @return None
		 */
		Modbus_Ch_Track( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
						 DCI_Owner* change_que_owner );

		/**
		 *  @brief destructor
		 *  @details It will be invoked when object of Modbus_Ch_Track goes out of scope
		 *  @return none
		 */
		~Modbus_Ch_Track( void )
		{}

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_Ch_Track( const Modbus_Ch_Track& object );
		Modbus_Ch_Track & operator =( const Modbus_Ch_Track& object );

		/// No value has changed since the last request.  There is no value to check.
		static const uint16_t MB_CH_NO_REG_CHANGE = 0xFFFFU;

		static const uint16_t MB_CH_NO_DCID_VAL = 0xFFFFU;

		void Change_Track_Handler( DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Change_Track_Handler_Static( Change_Tracker::cback_param_t param,
												 DCI_ID_TD dci_id, Change_Tracker::attrib_mask_t attribute_mask )
		{
			( reinterpret_cast<Modbus_Ch_Track*>( param ) )->Change_Track_Handler( dci_id,
																				   attribute_mask );
		}

		DCI_CB_RET_TD Change_Que_Reg_Callback( DCI_ACCESS_ST_TD* access_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Change_Que_Reg_Callback_Static( DCI_CBACK_PARAM_TD handle,
															 DCI_ACCESS_ST_TD* access_struct )
		{
			return ( ( ( Modbus_Ch_Track* )handle )->Change_Que_Reg_Callback( access_struct ) );
		}

		uint16_t Find_Modbus_Reg( DCI_ID_TD dcid ) const;

		DCI_ID_TD Find_DCID( uint16_t modbus_reg ) const;

		Change_Tracker* m_change_tracker;
		DCI_ID_TD m_current_dcid;

		bool m_change_shadow;
		DCI_Owner* m_change_owner;
		BF_Lib::Bit_List* m_change_que;

		const MODBUS_TARGET_INFO_ST_TD* m_dev_profile;
};

}

#endif	/*MODBUS_CH_TRACK_H_*/
