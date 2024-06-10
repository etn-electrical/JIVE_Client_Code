/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_USER_BLOCK_H
#define MODBUS_USER_BLOCK_H

#include "Change_Tracker.h"
#include "DCI.h"
#include "DCI_Owner.h"
#include "Bit_List.h"
#include "Modbus_DCI.h"

/**
 * @brief Handles a user specified assembly of Modbus registers.  Creates a contiguous list of
 * registers to read in a block.
 *
 * @details In some cases the user may want to construct their own register block which can be
 * read in one access.  In order to provide this ability this module will control two
 * parameters: Data and List.  The user defines the data section by putting the register numbers
 * in the list section.  The register data will then appear in the same data register offset as the
 * list register location.
 *
 * @note Pay attention to the following details.
 * @li It is a terrible idea to access this through anything but Modbus.  It counts on the
 * alignment being tied up to the register.
 * @li Any other access of the data_block data (through a patron) may lead to issues.
 * @li The Data block DCID must only support Ram values and not init values.
 * @li The list can contain (and probably should) INIT, RAM and default values.
 * @li The default value should be 0.
 * @li All registers entered into the list are user level register numbers.  For example:
 * if the user wants to read what they know as register 100 they will enter 100 in the list.
 * We internally subtract 1 and submit it as the register requested.
 */
class Modbus_User_Block
{
	public:

		Modbus_User_Block( DCI_ID_TD list_dcid, DCI_ID_TD data_block_dcid,
						   Modbus_DCI* modbus_dci_ctrl, bool writable = true );
		~Modbus_User_Block( void );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Modbus_User_Block( const Modbus_User_Block& object );
		Modbus_User_Block & operator =( const Modbus_User_Block& object );

		DCI_CB_RET_TD Data_Callback( DCI_ACCESS_ST_TD* access_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Data_Callback_Static( DCI_CBACK_PARAM_TD handle,
												   DCI_ACCESS_ST_TD* access_struct )
		{ return ( ( ( Modbus_User_Block* )handle )->Data_Callback( access_struct ) ); }

		static uint8_t m_dummy_owner_data;
		static const DCI_CB_MSK_TD CBACK_MASK_DEFINITION;
		static const uint16_t EMPTY_LIST_REGISTER = 0U;

		DCI_Owner* m_list_owner;
		DCI_Owner* m_data_owner;
		bool m_writable;
		Modbus_DCI* m_modbus_dci_ctrl;
};

#endif	/*MODBUS_CH_TRACK_H_*/
