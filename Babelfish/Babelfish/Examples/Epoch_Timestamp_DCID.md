//\example  	Epoch_Timestamp_DCID
//\brief    	The sample code to read POSIX epoch seconds
//\				tested for STM32WB55 with RTC
//\
//\				iPPE Project uses STM32WB55 and WB55 has no
//\				scope of time sync. 
//\				Hence epoch counting starts when controller boots up
//\				w.r.t 1970 Jan 1 00:00:00 as epoch count 0
//\

#include "Includes.h"
#include "DCI_Patron.h"


/**
 ****************************************************************************************
 * @brief	Function to get data from DCI variable
 * @param[in] dcid: DCID value
 * @param[in] data: pointer to destination buffer
 ****************************************************************************************
 */
DCI_ERROR_TD Get_DCID_Value( DCI_ID_TD dcid, uint8_t* data )
{
	DCI_ERROR_TD return_status = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	DCI_Patron* m_patron = new DCI_Patron( false );

	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = NULL;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}


DCI_ERROR_TD Example_RTC_timestamp_epoch_sec( uint32_t &timestamp_epoch_sec )
{

	DCI_ERROR_TD return_status = Get_DCID_Value( DCI_UNIX_EPOCH_TIME_DCID,
												( uint8_t* )&timestamp_epoch_sec );
	
	return return_status;
	
}

