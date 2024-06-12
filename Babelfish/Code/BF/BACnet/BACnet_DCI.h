/*
 *****************************************************************************************
 *		$Workfile:
 *
 *		$Author:
 *		$Date:
 *		Copyright
 *		Creator:
 *
 *		Description:
 *
 *
 *		Code Inspection Date:
 *
 *		$Header:
 *****************************************************************************************
 */
#ifndef BACNET_DCI_H
#define BACNET_DCI_H

// #include "Modbus_DCI_Defines.h"
#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "vsbhp.h"
#include "Base_DCI_BACnet_Data.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Global variable
 *****************************************************************************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class BACnet_DCI
{
	public:
		BACnet_DCI();
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~BACnet_DCI( void ){}

		int32_t Write_Param( DCI_ID_TD dcid, uint8_t* src_data, DCI_ACCESS_CMD_TD cmd );

		int32_t Read_Param( DCI_ID_TD dcid, uint8_t* dest_data, DCI_ACCESS_CMD_TD cmd );

		/**
		 * Provides the data length associated with a DCID.
		 */
		DCI_LENGTH_TD Get_Length( DCI_ID_TD dcid ) const;

	private:
		DCI_Patron* m_dci_access;
		DCI_SOURCE_ID_TD m_source_id;

		int32_t Interpret_DCI_BACNET_Error( DCI_ERROR_TD dci_error ) const;

};

#endif
