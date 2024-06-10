/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "ProfiBus_DCI.h"
#include "DCI_Defines.h"
// SSN This file was not present in the directory structure PROFI\Code\BFData.
// So i have picked it up from C440 Motorinsight from accurev.To work with this file I have commented or modified some
// variables in this project
// Mark will soon forward me his "DCI_ProfiBus_Data.h" and "DCI_ProfiBus_Data.cpp" file
#include "DCI_ProfiBus_Data.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define     DCID_NOT_FOUND              ( 0xFFFFU )
#define     PROFI_ID_NOT_FOUND          ( 0xFFFFU )




/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * --Function Name: ProfiBus_DCI()  ------------------------------------------------------
 * --Description: Class Constructor ------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
ProfiBus_DCI::ProfiBus_DCI( DCI_SOURCE_ID_TD unique_id ) :
	m_source_id( unique_id ),
	m_dci_access( static_cast<DCI_Patron*>( nullptr ) )
{
	m_source_id = unique_id;
	m_dci_access = new DCI_Patron( TRUE );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * --Function Name: Get_Profi_Data()  ----------------------------------------------------
 * --Description: Function retrieves the ram data from the DCI for a single PROFIBUS ID --
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * ------arg#1:PROFI_ID_TD profi_id - Profibus id for which data will be retrieved from --
 * --------- the DCI ---------------------------------------------------------------------
 * ------arg#2: DCI_LENGTH_TD* length_ptr - Pointer to location where the data length-----
 * --------- will be written. Used outside function to index data block for multiple gets-
 * ------arg#3: uint8_t* dest_data_ptr - Pointer to starting location where data will be ---
 * ------------placed---------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs: ---------------------------------------------------------------------------
 * ------ DCI data is placed staring at the location pointed to by dest_data_ptr      ----
 * ------ Length of the data is stored starting at the location pointed to by length_ptr -
 * -------return: DCI_ERROR_TD error - ---------------------------------------------------
 * -------- PROFI_ERR_NO_ERROR,                                                       ----
 * -------- PROFI_ERR_INVALID_DATA_ID,    < Data ID does not exist.                   ----
 * -------- PROFI_ERR_EXCEEDS_RANGE,	    < The set value resulted in a range error.   ----
 * -------- PROFI_ERR_RAM_READ_ONLY,	    < This value is read only.                   ----
 * -------- PROFI_ERR_INVALID_DATA_LENGTH,  < The data length is invalid.             ----
 * -------- PROFI_ERR_VALUE_LOCKED	      < The data value has been locked           ----
 *****************************************************************************************
 */
DCI_ERROR_TD ProfiBus_DCI::Get_Profi_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD* length_ptr,
										   uint8_t* dest_data_ptr )
{
	DCI_ID_TD dcid;
	DCI_ERROR_TD dci_error = PROFI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	uint8_t data_type_len;


	dcid = Find_Profi_Mod_DCID( profi_id );
	if ( dcid != DCID_NOT_FOUND )
	{
		access_struct.data_id = dcid;
		access_struct.source_id = m_source_id;
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;
		access_struct.data_access.data = dest_data_ptr;
		access_struct.data_access.length = Get_Length( profi_id );
		access_struct.data_access.offset = 0U;

		dci_error = m_dci_access->Data_Access( &access_struct );
		*length_ptr = access_struct.data_access.length;

		if ( dci_error == PROFI_ERR_NO_ERROR )
		{
			m_dci_access->Get_Datatype_Length( dcid, &data_type_len );
			// Swap_Data_Bytes( data_type_len, access_struct.data_access.length, dest_data_ptr );
		}
	}
	else
	{
		dci_error = PROFI_ERR_INVALID_PROFI_ID;
	}
	return ( dci_error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * --Function Name: Get_DCI_Data()  ----------------------------------------------------
 * --Description: Function retrieves the ram data from the DCI for a single PROFIBUS ID --
 * ---------------No Swapping of bytes----------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * ------arg#1:PROFI_ID_TD profi_id - Profibus id for which data will be retrieved from --
 * --------- the DCI ---------------------------------------------------------------------
 * ------arg#2: DCI_LENGTH_TD* length_ptr - Pointer to location where the data length-----
 * --------- will be written. Used outside function to index data block for multiple gets-
 * ------arg#3: uint8_t* dest_data_ptr - Pointer to starting location where data will be ---
 * ------------placed---------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs: ---------------------------------------------------------------------------
 * ------ DCI data is placed staring at the location pointed to by dest_data_ptr      ----
 * ------ Length of the data is stored starting at the location pointed to by length_ptr -
 * -------return: DCI_ERROR_TD error - ---------------------------------------------------
 * -------- PROFI_ERR_NO_ERROR,                                                       ----
 * -------- PROFI_ERR_INVALID_DATA_ID,    < Data ID does not exist.                   ----
 * -------- PROFI_ERR_EXCEEDS_RANGE,	    < The set value resulted in a range error.   ----
 * -------- PROFI_ERR_RAM_READ_ONLY,	    < This value is read only.                   ----
 * -------- PROFI_ERR_INVALID_DATA_LENGTH,  < The data length is invalid.             ----
 * -------- PROFI_ERR_VALUE_LOCKED	      < The data value has been locked           ----
 *****************************************************************************************
 */
DCI_ERROR_TD ProfiBus_DCI::Get_DCI_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD* length_ptr,
										 uint8_t* dest_data_ptr )
{
	DCI_ID_TD dcid;
	DCI_ERROR_TD dci_error = PROFI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;



	dcid = Find_Profi_Mod_DCID( profi_id );
	if ( dcid != DCID_NOT_FOUND )
	{
		access_struct.data_id = dcid;
		access_struct.source_id = m_source_id;
		access_struct.command = DCI_ACCESS_GET_RAM_CMD;
		access_struct.data_access.data = dest_data_ptr;
		access_struct.data_access.length = Get_Length( profi_id );
		access_struct.data_access.offset = 0U;

		dci_error = m_dci_access->Data_Access( &access_struct );
		*length_ptr = access_struct.data_access.length;
	}
	else
	{
		dci_error = PROFI_ERR_INVALID_PROFI_ID;
	}
	return ( dci_error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---Function Name: Set_Profi_IO_Data()  ------------------------------------------------
 * ---Description: Function writes ram data to DCI for a single PROFIBUS ID --------------
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * ------arg#1:PROFI_ID_TD profi_id - This is the parameter for which data will be -------
 * --------- written to in the DCI -------------------------------------------------------
 * ------arg#2: DCI_LENGTH_TD length - Data length of data (0) = Write all data-----------
 * ---------------------------------------------------------------------------------------
 * ------arg#3: uint8_t* src_data_ptr - Pointer to starting location where data to be ------
 * ------------written is located --------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs: ---------------------------------------------------------------------------
 * ------ return: DCI_ERROR_TD error - ---------------------------------------------------
 * -------- PROFI_ERR_NO_ERROR,                                                       ----
 * -------- PROFI_ERR_INVALID_DATA_ID,    < Data ID does not exist.                   ----
 * -------- PROFI_ERR_EXCEEDS_RANGE,	    < The set value resulted in a range error.   ----
 * -------- PROFI_ERR_RAM_READ_ONLY,	    < This value is read only.                   ----
 * -------- PROFI_ERR_INVALID_DATA_LENGTH,  < The data length is invalid.             ----
 * -------- PROFI_ERR_VALUE_LOCKED	      < The data value has been locked           ----
 *****************************************************************************************
 */
DCI_ERROR_TD ProfiBus_DCI::Set_Profi_IO_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD length,
											  uint8_t* src_data_ptr )
{
	DCI_ID_TD dcid;
	DCI_ERROR_TD dci_error = PROFI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	uint8_t data_type_len = 0U;
	DCI_ERROR_TD dci_access_error = PROFI_ERR_NO_ERROR;

	dcid = Find_Profi_Mod_DCID( profi_id );
	if ( dcid != DCID_NOT_FOUND )
	{

		m_dci_access->Get_Datatype_Length( dcid, &data_type_len );
		Swap_Data_Bytes( data_type_len, length, src_data_ptr );

		access_struct.data_id = dcid;
		access_struct.source_id = m_source_id;
		access_struct.command = DCI_ACCESS_SET_RAM_CMD;
		access_struct.data_access.data = src_data_ptr;
		access_struct.data_access.length = length;
		access_struct.data_access.offset = 0U;
		dci_access_error = m_dci_access->Error_Check_Access( &access_struct );
		if ( PROFI_ERR_NO_ERROR == dci_access_error )	/* Value shall not be checked in into DCID if it does not
														   fullfill basic criteria */
		{
			dci_error = m_dci_access->Data_Access( &access_struct );
		}
		else
		{
			dci_error = dci_access_error;
		}
	}
	else
	{
		dci_error = PROFI_ERR_INVALID_PROFI_ID;
	}

	return ( dci_error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---Function Name: Set_Profi_Prm_Data()  -----------------------------------------------
 * ---Description: Function writes Ram & Initial values to DCI ---------------------------
 * -------------------  for a single PROFIBUS ID -----------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * ------arg#1:PROFI_ID_TD profi_id - This is the parameter for which data will be -------
 * --------- written to in the DCI -------------------------------------------------------
 * ------arg#2: DCI_LENGTH_TD length - Data length  (0) = Write all data------------------
 * ---------------------------------------------------------------------------------------
 * ------arg#3: uint8_t* src_data_ptr - Pointer to starting location where data to be ------
 * ------------written is located --------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs: ---------------------------------------------------------------------------
 * ------ return: DCI_ERROR_TD error - ---------------------------------------------------
 * -------- PROFI_ERR_NO_ERROR,                                                       ----
 * -------- PROFI_ERR_INVALID_DATA_ID,    < Data ID does not exist.                   ----
 * -------- PROFI_ERR_EXCEEDS_RANGE,	    < The set value resulted in a range error.   ----
 * -------- PROFI_ERR_RAM_READ_ONLY,	    < This value is read only.                   ----
 * -------- PROFI_ERR_INVALID_DATA_LENGTH,  < The data length is invalid.             ----
 * -------- PROFI_ERR_VALUE_LOCKED	      < The data value has been locked           ----
 *****************************************************************************************
 */
DCI_ERROR_TD ProfiBus_DCI::Set_Profi_Prm_Data( PROFI_ID_TD profi_id, DCI_LENGTH_TD length,
											   uint8_t* src_data_ptr )
{
	DCI_ID_TD dcid;
	DCI_ERROR_TD dci_error = PROFI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;
	DCI_DATATYPE_SIZE_TD data_type_len = 0U;
	DCI_DATA_BLOCK_TD const* data_block;


	dcid = Find_Profi_Mod_DCID( profi_id );
	if ( dcid != DCID_NOT_FOUND )
	{
		data_block = &dci_data_blocks[dcid];
		m_dci_access->Get_Datatype_Length( dcid, &data_type_len );
		Swap_Data_Bytes( data_type_len, length, src_data_ptr );

		access_struct.data_id = dcid;
		access_struct.source_id = m_source_id;
		access_struct.command = DCI_ACCESS_SET_RAM_CMD;
		access_struct.data_access.data = src_data_ptr;
		access_struct.data_access.length = length;
		access_struct.data_access.offset = 0U;

		dci_error = m_dci_access->Data_Access( &access_struct );

		if ( dci_error == PROFI_ERR_NO_ERROR )
		{
			// If parameter does not have an initial value do not write to it
			if ( Test_Bit( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_DATA ) )
			{
				if ( Test_Bit( data_block->patron_attrib_info, DCI_PATRON_ATTRIB_NVMEM_WR_DATA ) )
				{
					access_struct.command = DCI_ACCESS_SET_INIT_CMD;
					dci_error = m_dci_access->Data_Access( &access_struct );
				}
			}
		}
	}
	else
	{
		dci_error = PROFI_ERR_INVALID_PROFI_ID;
	}

	return ( dci_error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---Function Name: Get_Length()  -------------------------------------------------------
 * ---Description: Function returns the data length for a single PROFIBUS ID -------------
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * -----arg#1: PROFI_ID_TD profi_id - Parameter whose length should be returned-----------
 * ---------------------------------------------------------------------------------------
 * ---Outputs:----------------------------------------------------------------------------
 * -----return: return_length - data length of parameter----------------------------------
 *****************************************************************************************
 */
DCI_LENGTH_TD ProfiBus_DCI::Get_Length( PROFI_ID_TD profi_id ) const
{
	DCI_ID_TD dcid;
	DCI_LENGTH_TD return_length = 0U;

	dcid = Find_Profi_Mod_DCID( profi_id );

	m_dci_access->Get_Length( dcid, &return_length );

	return ( return_length );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---Function Name: Get_Datatype_Length()  ----------------------------------------------
 * ---Description: Function returns the length of the datatype for a single PROFIBUS ID --
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * -----arg#1: PROFI_ID_TD profi_id - Parameter whose datatype length should be returned--
 * ---------------------------------------------------------------------------------------
 * ---Outputs:----------------------------------------------------------------------------
 * -----return: return_length - length of datatype for DCI parameter ---------------------
 *****************************************************************************************
 */
DCI_LENGTH_TD ProfiBus_DCI::Get_Datatype_Length( PROFI_ID_TD profi_id ) const
{
	DCI_ID_TD dcid;
	DCI_DATATYPE_SIZE_TD return_length = 0U;


	dcid = Find_Profi_Mod_DCID( profi_id );

	m_dci_access->Get_Datatype_Length( dcid, &return_length );

	return ( return_length );
}

/*
 *****************************************************************************************
 * ---Function Name: Find_Profi_Mod_DCID() -----------------------------------------------
 * ---Description: Returns the DCI id# for the provided PROFIBUS id# ---------------------
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * -----arg#1: PROFI_ID_TD profibus_id - -------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs:----------------------------------------------------------------------------
 * -----return: dcid - DCI id# for the supplied Profibus id#------------------------------
 *****************************************************************************************
 */

DCI_ID_TD ProfiBus_DCI::Find_Profi_Mod_DCID( PROFI_ID_TD profibus_id )
{
	bool end_loop = false;
	uint16_t begin = 0U;
	uint16_t end;
	uint16_t middle;
	DCI_ID_TD dcid = DCID_NOT_FOUND;

	end = DCI_PROFIBUS_DATA_TOTAL_MODULES - 1U;
	while ( ( begin <= end ) && ( end_loop == FALSE ) )
	{
		middle = ( begin + end ) >> 1;
		if ( dci_profibus_lkup[middle].profibus_id < profibus_id )
		{
			begin = middle + 1U;
		}
		else if ( dci_profibus_lkup[middle].profibus_id > profibus_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				end_loop = TRUE;
			}
		}
		else
		{
			end_loop = TRUE;
			dcid = dci_profibus_lkup[middle].dcid;
		}

	}
	return ( dcid );
}

/*
 *****************************************************************************************
 * ---Function Name: Find_Profi_Mod_ID() -------------------------------------------------
 * ---Description: Returns the PROFI_MOD id# for the provided DCI id#---------------------
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * -----arg#1: DCI_ID_TD dci_id- ---------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs:----------------------------------------------------------------------------
 * -----return: PROFI_ID_TD  profibus_id - Profibus id# for the supplied DCI id#----------
 *****************************************************************************************
 */
PROFI_ID_TD ProfiBus_DCI::Find_Profi_Mod_ID( DCI_ID_TD dci_id )
{
	bool end_loop = false;
	uint16_t begin = 0U;
	uint16_t end;
	uint16_t middle;
	PROFI_ID_TD profi_id = PROFI_ID_NOT_FOUND;

	end = DCI_PROFIBUS_DATA_TOTAL_MODULES - 1U;
	while ( ( begin <= end ) && ( end_loop == FALSE ) )
	{
		middle = ( begin + end ) >> 1U;
		if ( dci_id_lkup[middle].dci_id < dci_id )
		{
			begin = middle + 1U;
		}
		else if ( dci_id_lkup[middle].dci_id > dci_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				end_loop = TRUE;
			}
		}
		else
		{
			end_loop = TRUE;
			profi_id = dci_id_lkup[middle].profi_id;
		}

	}
	return ( profi_id );
}

/*
 *****************************************************************************************
 * ---Function Name: Swap_Data_Bytes() ---------------------------------------------------
 * ---Description: Swaps bytes of data depending upon the datatype provided --------------
 * ---------------------------------------------------------------------------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * -----arg#1: uint8_t data_type_len - Length of the data type for a DCI parameter----------
 * --------Used to determine which bytes need to be swapped ------------------------------
 * -----arg#2: DCI_LENGTH_TD data_len - length of the overall data to be swapped----------
 * --------Used to determine how many bytes need to be swapped ---------------------------
 * -----arg#3: uint8_t* data - Pointer to starting location of data to be swapped-----------
 * ---------------------------------------------------------------------------------------
 * ---Outputs: ---------------------------------------------------------------------------
 * ------Swapped data --------------------------------------------------------------------
 * -----ONE_BYTE----------TWO_BYTES-----------FOUR_BYTES-------------EIGHT_BYTES----------
 * --before   after    before   after       before   after          before   after   -----
 * ----No Swap         Byte1    Byte2       Byte1    Byte4          Byte1    Byte8   -----
 * -----------------   Byte2    Byte1       Byte2    Byte3          Byte2    Byte7   -----
 * ---------------------------------------- Byte3    Byte2          Byte3    Byte6   -----
 * ---------------------------------------- Byte4    Byte1          Byte4    Byte5   -----
 * ---------------------------------------------------------------- Byte5    Byte4   -----
 * ---------------------------------------------------------------- Byte6    Byte3   -----
 * ---------------------------------------------------------------- Byte7    Byte2   -----
 * ---------------------------------------------------------------- Byte8    Byte1   -----
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void ProfiBus_DCI::Swap_Data_Bytes( uint8_t data_type_len, DCI_LENGTH_TD data_len, uint8_t* data ) const
{
	uint16_t high_index;
	uint16_t low_index;
	uint16_t data_index = 0U;
	uint16_t temp_data = 0U;


	if ( data_type_len > 1U )
	{
		if ( ( data_len % 2 ) != 0U )
		{
			data_index++;
		}

		while ( data_index < data_len )
		{
			high_index = data_type_len - 1U;
			low_index = 0U;
			while ( low_index < high_index )
			{
				temp_data = data[data_index + low_index];
				data[data_index + low_index] = data[data_index + high_index];
				data[data_index + high_index] = temp_data;
				low_index++;
				high_index--;
			}
			data_index += data_type_len;
		}
	}
}
