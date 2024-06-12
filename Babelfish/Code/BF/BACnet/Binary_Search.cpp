/*
 *****************************************************************************************
 *
 *		$Author:$
 *		$Date$
 *		Copyright @Eaton Corporation. All Rights Reserved.
 *		Creator: Alvin Zhao
 *
 *		Description:
 *		$Header:$
 *
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Binary_Search.h"
#include "Modbus_DCI_Data.h"

/*****************************************************************************************
* Find DCID from Modbus id.
*****************************************************************************************/
DCI_ID_TD Find_DCID_From_Modbus_ID( uint16_t modbus_reg )
{
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* search_struct;
	BF_Lib::MUINT16 begin = 0;
	BF_Lib::MUINT16 end;
	BF_Lib::MUINT16 middle;

	search_struct = modbus_dci_data_target_info.mbus_to_dcid;
	end = modbus_dci_data_target_info.total_registers - 1;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1;
		if ( search_struct[middle].modbus_reg < modbus_reg )
		{
			begin = middle + 1;
		}
		else if ( search_struct[middle].modbus_reg > modbus_reg )
		{
			if ( middle > 0 )
			{
				end = middle - 1;
			}
			else
			{
				return ( DCI_ID_UNDEFINED );
			}
		}
		else
		{
			return ( search_struct[middle].dcid );
		}
	}

	return ( DCI_ID_UNDEFINED );// There is a return above here.  It just looks cleaner with the return above.
}

/*****************************************************************************************
* Find Modbus ID from DCID
*****************************************************************************************/
uint16_t Find_Modbus_ID_From_DCID( DCI_ID_TD dcid )
{
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* search_struct;
	BF_Lib::MUINT16 begin = 0;
	BF_Lib::MUINT16 end;
	BF_Lib::MUINT16 middle;

	search_struct = modbus_dci_data_target_info.dcid_to_mbus;
	end = modbus_dci_data_target_info.total_dcids - 1;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1;
		if ( search_struct[middle].dcid < dcid )
		{
			begin = middle + 1;
		}
		else if ( search_struct[middle].dcid > dcid )
		{
			if ( middle > 0 )
			{
				end = middle - 1;
			}
			else
			{
				return ( 0xFFFF );
			}
		}
		else
		{
			return ( search_struct[middle].modbus_reg );
		}
	}

	return ( 0xFFFF );
}

/*****************************************************************************************
* Function Head
*****************************************************************************************/
const DCI_MBUS_TO_DCID_LKUP_ST_TD* Find_From_Modbus_ID( uint16_t modbus_reg )
{
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* search_struct = NULL;
	BF_Lib::MUINT16 begin = 0;
	BF_Lib::MUINT16 end;
	BF_Lib::MUINT16 middle;

	search_struct = modbus_dci_data_target_info.mbus_to_dcid;
	end = modbus_dci_data_target_info.total_registers - 1;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1;
		if ( search_struct[middle].modbus_reg < modbus_reg )
		{
			begin = middle + 1;
		}
		else if ( search_struct[middle].modbus_reg > modbus_reg )
		{
			if ( middle > 0 )
			{
				end = middle - 1;
			}
			else
			{
				return ( NULL );
			}
		}
		else
		{
			return ( &search_struct[middle] );
		}
	}
	return ( NULL );
}

/*****************************************************************************************
* Function Head
*****************************************************************************************/
const DCI_MBUS_FROM_DCID_LKUP_ST_TD* Find_From_DCID( DCI_ID_TD dci_id )
{
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* search_struct = NULL;
	BF_Lib::MUINT16 begin = 0;
	BF_Lib::MUINT16 end;
	BF_Lib::MUINT16 middle;

	search_struct = modbus_dci_data_target_info.dcid_to_mbus;
	end = modbus_dci_data_target_info.total_dcids - 1;
	while ( begin <= end )
	{
		middle = ( begin + end ) >> 1;
		if ( search_struct[middle].dcid < dci_id )
		{
			begin = middle + 1;
		}
		else if ( search_struct[middle].dcid > dci_id )
		{
			if ( middle > 0 )
			{
				end = middle - 1;
			}
			else
			{
				return ( NULL );
			}
		}
		else
		{
			return ( &search_struct[middle] );
		}
	}

	return ( NULL );
}
