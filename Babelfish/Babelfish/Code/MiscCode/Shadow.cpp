/*
 ********************************************************************************
 *		$Workfile: Input_Deb.cpp$
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 10/10/2008 12:18:38 PM$
 *		Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *		$Header: Input_Deb.cpp, 10, 10/10/2008 12:18:38 PM, Mark A Verheyen$
 ********************************************************************************
 */
#include "includes.h"
#include "Shadow.h"
#include "Stdlib_MV.h"
// #include "Intercom_App.h"

/*
 ********************************************************************************
 *		Macros
 ********************************************************************************
 */



/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */


/**-----------------------------------------------------------------------------
**  Function Name:   Shadow::Shadow
**  Created By:      Mark Verheyen
**  Date Created:    12-19-2011
**
**  Description:
**                  Constructor
**  Inputs:
**                  pointer to DCI_Patron object, DCI_SourceID
**  Outputs:
**                  None
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
Shadow::Shadow( DCI_Patron* patron_ctrl, DCI_SOURCE_ID_TD source_id ) :
	m_source_id( 0U ),
	m_patron( reinterpret_cast<DCI_Patron*>( NULL ) )
{
	m_patron = patron_ctrl;
	m_source_id = source_id;
}

/**-----------------------------------------------------------------------------
**  Function Name:   Shadow::Get_DCID_Ram_Data
**  Created By:      Mark Verheyen
**  Date Created:    12-19-2011
**
**  Description:
**                  Read value of Parameter
**  Inputs:
**                  Parameter: Want to Read,
**                  Pointer to store the Read value,
**                  length of parameter,
**                  offset from where want to read value
**  Outputs:
**                  Read Status: Sucess/fail
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
DCI_ERROR_TD Shadow::Get_DCID_Ram_Data(
	DCI_ID_TD dcid,
	DCI_DATA_PASS_TD* data,
	DCI_LENGTH_TD length,
	DCI_LENGTH_TD offset )
{
	DCI_ERROR_TD return_status;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.source_id = m_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = length;
	access_struct.data_access.offset = offset;
	access_struct.data_id = dcid;
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Shadow::Set_DCID_Ram_Data
**  Created By:      Mark Verheyen
**  Date Created:    12-19-2011
**
**  Description:
**                  Write a value to Parameter
**  Inputs:
**                  Parameter: Want to Write,
**                  Pointer to Write value,
**                  length of parameter,
**                  offset from where want to write value
**  Outputs:
**                  Read Status: Sucess/fail
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
DCI_ERROR_TD Shadow::Set_DCID_Ram_Data(
	DCI_ID_TD dcid,
	DCI_DATA_PASS_TD* data,
	DCI_LENGTH_TD length,
	DCI_LENGTH_TD offset )
{
	DCI_ERROR_TD return_status;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = length;
	access_struct.data_access.offset = offset;
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Shadow::Set_DCID_Init_Data
**  Created By:      Mark Verheyen
**  Date Created:    12-19-2011
**
**  Description:
**                  Write a value into allocated NV0/NV1 memory of Parameter
**  Inputs:
**                  Parameter: Want to Write,
**                  Pointer to Write value,
**                  length of parameter,
**                  offset from where want to write value
**  Outputs:
**                  Read Status: Sucess/fail
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/
DCI_ERROR_TD Shadow::Set_DCID_Init_Data( DCI_ID_TD dcid,
										 DCI_DATA_PASS_TD* data,
										 DCI_LENGTH_TD length,
										 DCI_LENGTH_TD offset )
{
	DCI_ERROR_TD return_status;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = length;
	access_struct.data_access.offset = offset;
	access_struct.command = DCI_ACCESS_SET_INIT_CMD;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}

/**-----------------------------------------------------------------------------
**  Function Name:   Shadow::Get_DCID_Init_Data
**  Created By:      Pranav Kodre
**  Date Created:    4-21-2014
**
**  Description:
**                  Read a value from allocated NV0/NV1 memory of Parameter
**  Inputs:
**                  Parameter: Want to read,
**                  Pointer to store the Read value,
**                  length of parameter,
**                  offset from where want to write value
**  Outputs:
**                  Read Status: Sucess/fail
**  Notes:
**                  None
**
**----------------------------------------------------------------------------*/

DCI_ERROR_TD Shadow::Get_DCID_Init_Data( DCI_ID_TD dcid,
										 DCI_DATA_PASS_TD* data,
										 DCI_LENGTH_TD length,
										 DCI_LENGTH_TD offset )
{
	DCI_ERROR_TD return_status;
	DCI_ACCESS_ST_TD access_struct;

	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = length;
	access_struct.data_access.offset = offset;
	access_struct.command = DCI_ACCESS_GET_INIT_CMD;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}
