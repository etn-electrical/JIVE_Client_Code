/*
 *****************************************************************************************
 *		$Workfile: BACnet_DCI.cpp$
 *
 *		$Author:
 *		$Date:
 *		Copyright.
 *
 *		$Header:
 *****************************************************************************************
 */
#include "includes.h"
#include "BACnet_DCI.h"
#include "NV_Mem.h"
#include "BACnetDCIInterface.h"
#include "vsbhp.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

BACnet_DCI::BACnet_DCI() :
	m_dci_access( reinterpret_cast<DCI_Patron*>( nullptr ) ),
	m_source_id( 0U )
{
	m_dci_access = new DCI_Patron( TRUE );

	m_source_id = DCI_SOURCE_IDS_Get();
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
// DCI_ACCESS_GET_RAM_CMD     // to read the RAM value
// DCI_ACCESS_GET_DEFAULT_CMD //to read the default value
int32_t BACnet_DCI::Read_Param( DCI_ID_TD dcid,
								uint8_t* dest_data, DCI_ACCESS_CMD_TD cmd )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.command = cmd;
	access_struct.data_access.data = dest_data;
	access_struct.source_id = m_source_id;
	access_struct.data_access.length = 0U;

	// if ( dcid != NULL )
	// {
	m_dci_access->Get_Length( dcid, &access_struct.data_access.length );
	access_struct.data_access.offset = 0U;
	access_struct.data_id = dcid;
	dci_error = m_dci_access->Data_Access( &access_struct );
	// }
	// else
	// {
	// dci_error = DCI_ERR_INVALID_DATA_ID;
	// }

	return ( Interpret_DCI_BACNET_Error( dci_error ) );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
int32_t BACnet_DCI::Write_Param( DCI_ID_TD dcid, uint8_t* src_data, DCI_ACCESS_CMD_TD cmd )
{
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_access.data = src_data;
	access_struct.data_access.length = 0U;

	// if ( dcid != NULL )
	// {
	m_dci_access->Get_Length( dcid, &access_struct.data_access.length );

	access_struct.command = cmd;
	access_struct.data_id = dcid;
	access_struct.data_access.offset = 0U;
	dci_error = m_dci_access->Data_Access( &access_struct );

	if ( dci_error == DCI_ERR_NO_ERROR )
	{
		access_struct.command = DCI_ACCESS_SET_INIT_CMD;
		dci_error = m_dci_access->Data_Access( &access_struct );

		if ( ( dci_error == DCI_ERR_NV_NOT_AVAILABLE ) ||
			 ( dci_error == DCI_ERR_NV_READ_ONLY ) )
		{
			dci_error = DCI_ERR_NO_ERROR;
		}
	}
	// }
	// else
	// {
	// dci_error = DCI_ERR_INVALID_DATA_ID;
	// }

	return ( Interpret_DCI_BACNET_Error( dci_error ) );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
DCI_LENGTH_TD BACnet_DCI::Get_Length( DCI_ID_TD dcid ) const
{
	DCI_LENGTH_TD return_length = 0U;

	m_dci_access->Get_Length( dcid, &return_length );

	return ( return_length );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
int32_t BACnet_DCI::Interpret_DCI_BACNET_Error( DCI_ERROR_TD dci_error ) const
{
	int32_t return_status;

	switch ( dci_error )
	{
		case DCI_ERR_NO_ERROR:
			return_status = vsbe_noerror;
			break;

		case DCI_ERR_INVALID_COMMAND:
			return_status = vsbe_inconsistentparameters;
			break;

		case DCI_ERR_INVALID_DATA_ID:
			return_status = vsbe_inconsistentparameters;
			break;

		case DCI_ERR_INVALID_DATA_LENGTH:
			return_status = vsbe_inconsistentparameters;
			break;

		case DCI_ERR_EXCEEDS_RANGE:
			return_status = vsbe_valueoutofrange;
			break;

		case DCI_ERR_RAM_READ_ONLY:
			return_status = vsbe_writeaccessdenied;
			break;

		case DCI_ERR_NV_READ_ONLY:
			return_status = vsbe_writeaccessdenied;
			break;

		case DCI_ERR_NV_ACCESS_FAILURE:
		case DCI_ERR_ACCESS_VIOLATION:
		default:
			return_status = propertyerror;
			break;
	}

	return ( return_status );
}
