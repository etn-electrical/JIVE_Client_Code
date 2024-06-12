/*
 ********************************************************************************
 *		$Workfile: Input_Deb.h$
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 9/26/2007 2:57:42 PM$
 *		Copyright © Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *		Code Inspection Date: 5/20/2004
 *
 *		$Header: Input_Deb.h, 2, 9/26/2007 2:57:42 PM, Mark A Verheyen$
 ********************************************************************************
 */
#ifndef SHADOW_H
   #define SHADOW_H

#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Change_Tracker.h"

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */

/*
 ********************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------
 */
class Shadow
{
	public:
		Shadow( DCI_Patron* patron_ctrl, DCI_SOURCE_ID_TD source_id );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Shadow( void )
		{
			delete m_patron;
		}

		DCI_ERROR_TD Get_DCID_Ram_Data( DCI_ID_TD dcid,
										DCI_DATA_PASS_TD* data,
										DCI_LENGTH_TD length = 0U,
										DCI_LENGTH_TD offset = 0U );

		DCI_ERROR_TD Set_DCID_Ram_Data( DCI_ID_TD dcid,
										DCI_DATA_PASS_TD* data,
										DCI_LENGTH_TD length,
										DCI_LENGTH_TD offset = 0U );

		DCI_ERROR_TD Set_DCID_Init_Data( DCI_ID_TD dcid,
										 DCI_DATA_PASS_TD* data,
										 DCI_LENGTH_TD length,
										 DCI_LENGTH_TD offset = 0U );

		DCI_ERROR_TD Get_DCID_Init_Data( DCI_ID_TD dcid,
										 DCI_DATA_PASS_TD* data,
										 DCI_LENGTH_TD length,
										 DCI_LENGTH_TD offset = 0U );

		// Returns the Inputs and outputs in a single bitfield.
		void Get_Packed_IO_Data( UINT8* dest_data );

		void Set_Relays( UINT8 relay_state );

		bool_t Target_Device_Is_Present( void );

		bool_t Relay_Ctrl_Voltage_Low( void );

	private:

		DCI_SOURCE_ID_TD m_source_id;
		DCI_Patron* m_patron;
};

#endif
