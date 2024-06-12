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
#ifndef MODBUS_PROD_CONS_H
#define MODBUS_PROD_CONS_H

#include "Change_Tracker.h"
#include "DCI.h"
#include "DCI_Owner.h"
#include "Bit_List.h"
#include "Modbus_DCI.h"

/**
 *
 */
class Modbus_Prod_Cons
{
	public:

		Modbus_Prod_Cons( DCI_ID_TD prod_list_id, DCI_ID_TD prod_data_id,
						  DCI_ID_TD cons_list_id, DCI_ID_TD cons_data_id,
						  Modbus_DCI* modbus_dci_ctrl );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Modbus_Prod_Cons( void )
		{}

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Modbus_Prod_Cons( const Modbus_Prod_Cons& object );
		Modbus_Prod_Cons & operator =( const Modbus_Prod_Cons& object );

		DCI_CB_RET_TD Data_Callback( DCI_ACCESS_ST_TD* access_struct, bool prod_data );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Cons_Data_Callback_Static( DCI_CBACK_PARAM_TD handle,
														DCI_ACCESS_ST_TD* access_struct )
		{
			return ( ( ( Modbus_Prod_Cons* )handle )->Data_Callback( access_struct, false ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD Prod_Data_Callback_Static( DCI_CBACK_PARAM_TD handle,
														DCI_ACCESS_ST_TD* access_struct )
		{
			return ( ( ( Modbus_Prod_Cons* )handle )->Data_Callback( access_struct, true ) );
		}

		DCI_Owner* m_prod_list_owner;
		DCI_Owner* m_prod_data_owner;
		DCI_Owner* m_cons_list_owner;
		DCI_Owner* m_cons_data_owner;
		uint8_t m_dummy_owner_data;

		Modbus_DCI* m_modbus_dci_ctrl;
};

#endif	/*MODBUS_CH_TRACK_H_*/
