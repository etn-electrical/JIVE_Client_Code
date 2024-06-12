/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "defines.h"
#include "Timers_Lib.h"
#include "VPC3_platform.h"
#include "ProfiBus_DCI.h"
#include "ProfiBus_Prod_Cons.h"
#include "VPC3_dp_if.h"


/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define         MAX_LENGTH          8

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
static uint8_t prod_data_array[MAX_PROD_SIZE];

/*
 *****************************************************************************************
 * --Function Header ---------------------------------------------------------------------
 * --Function Name: ProfiBus_Prod_Cons()--------------------------------------------------
 * ---Description: Constructor -----------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
ProfiBus_Prod_Cons::ProfiBus_Prod_Cons( DCI_ID_TD prod_list_id, DCI_ID_TD cons_list_id,
										DCI_ID_TD prod_data_id, ProfiBus_DCI* profibus_dci_ctrl )
{
	m_prod_data_ptr = &prod_data_array[0];

	m_prod_timer = new Timers( Refresh_Prod_Static, ( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD )this,
							   TIMERS_DEFAULT_CR_PRIORITY, "PROFIBUS Prod Cons" );

	m_prod_list_owner = new DCI_Owner( prod_list_id );
	m_cons_list_owner = new DCI_Owner( cons_list_id );
	m_prod_data_owner = new DCI_Owner( prod_data_id, m_prod_data_ptr );
	m_empty_pad_byte_owner = new DCI_Owner( DCI_EMPTY_PAD_BYTE_DCID );


	m_prod_data_change = FALSE;
	m_profibus_dci_ctrl = profibus_dci_ctrl;
}

/*
 *****************************************************************************************
 * --Function Header ---------------------------------------------------------------------
 * --Function Name: Build_IO_Assemblies () -----------------------------------------------
 * ---Description: Builds Prod & Cons assembly structures from configuration buffer. -----
 * ---------Function expects configuration data to be in special format ------------------
 * ---------DCI Owner checks in Prod & Cons Lists along with Prod & Cons sizes -----------
 * ---------Prod & Cons Lists are populated with Profi IDs. ------------------------------
 * ---------Function returns an error if the assemblies could not be set------------------
 * ---Inputs: ----------------------------------------------------------------------------
 * ----arg#1: uint8_t* cfg_ptr - Pointer to start of ProfiBus configuration buffer in VPC3--
 * ----arg#2: uint8_t cfg_len - Length of data in the configuration buffer------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs: ---------------------------------------------------------------------------
 * -----return: uint8_t error - ------------------------------------------------------------
 * -------- CFG_OK                           << No error ---------------------------------
 * -------- CFG_NOT_SUPPORTED_ERROR          << Module in CFG data not supported ---------
 * -------- CFG_IO_ASSEMBLY_SET_LENGTH_ERROR << Invalid number of IO modules checked in---
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t ProfiBus_Prod_Cons::Build_IO_Assemblies( uint8_t* cfg_ptr, uint8_t cfg_len )
{
	uint8_t error = CFG_OK;
	DCI_ID_TD prod_list[MAX_PROD_MODULES];
	DCI_ID_TD cons_list[MAX_CONS_MODULES];
	DCI_ID_TD* id_ptr;
	uint8_t count = 0;
	MODULE_STRUCT io_module;
	DCI_LENGTH_TD length = 0;
	DCI_LENGTH_TD data_length = 0;
	uint16_t prod_data_size = 0;
	uint16_t cons_data_size = 0;

	m_total_prod_modules = 0;
	m_total_cons_modules = 0;

	for (; ( cfg_len > 0 ) && ( error == CFG_OK ); cfg_len -= SPECIAL_FORMAT_CFG_LENGTH )
	{
		error = Get_Module_Data( ( cfg_ptr + count ), &io_module );
		if ( error == CFG_OK )
		{
			switch ( io_module.mod_type )
			{
				case INPUT:
					prod_list[m_total_prod_modules] = io_module.profi_id;
					prod_data_size += io_module.data_size;
					m_total_prod_modules++;
					break;

				case OUTPUT:
					cons_list[m_total_cons_modules] = io_module.profi_id;
					cons_data_size += io_module.data_size;
					m_total_cons_modules++;
					break;

				default:
					error = CFG_NOT_SUPPORTED_ERROR;
					break;
			}
		}
		count += SPECIAL_FORMAT_CFG_LENGTH;
	}
	if ( error == CFG_OK )
	{
		data_length = prod_data_size;
		length = ( m_total_prod_modules * 2 );
		if ( m_prod_list_owner->Check_In_Length( &length ) )
		{
			id_ptr = &prod_list[0];
			m_prod_list_owner->Check_In( id_ptr );
			m_prod_data_owner->Check_In_Length( &data_length );
		}
		else
		{
			error = CFG_IO_ASSEMBLY_SET_LENGTH_ERROR;
		}
	}
	if ( error == CFG_OK )
	{
		data_length = cons_data_size;
		length = ( m_total_cons_modules * 2 );
		if ( m_cons_list_owner->Check_In_Length( &length ) )
		{
			id_ptr = &cons_list[0];
			m_cons_list_owner->Check_In( id_ptr );
		}
		else
		{
			error = CFG_IO_ASSEMBLY_SET_LENGTH_ERROR;
		}
	}
	return ( error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ----Function Name: Get_Module_Data()---------------------------------------------------
 * -----Description: Copies the profiBus module ID and data length from the configuration-
 * --------data in the VPC3 configuration buffer.  Grabs one IO module at a time ---------
 * --------and copies the data to the MODULE_STRUCT* module_ptr---------------------------
 * --------Function expects configuration data to be in 4 byte special format-------------
 * --------and each single module can be either an input or an output, not both-----------
 * ---Input: -----------------------------------------------------------------------------
 * -----arg#1: uint8_t* cfg_ptr - Pointer to start of ProfiBus configuration buffer in VPC3-
 * -----arg#2: MODULE_STRUCT* module_ptr - Pointer to MODULE_STRUCT variable where -------
 * ------------the IO module data will be copied------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---Outputs: ---------------------------------------------------------------------------
 * ------Copies IO module data - Profi_ID & data length to MODULE_STRUCT------------------
 * ------return: uint8_t error--------------------------------------------------------------
 * -------- CFG_OK                           << No error ---------------------------------
 * -------- CFG_NOT_SUPPORTED_ERROR          << Module in CFG data not supported ---------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t ProfiBus_Prod_Cons::Get_Module_Data( uint8_t* cfg_ptr, MODULE_STRUCT* module_ptr )
{
	uint8_t specific_data_length = 0;
	uint8_t error = CFG_OK;


	if ( ( ( cfg_ptr[SF_HEADER_BYTE] & VPC3_CFG_IS_BYTE_FORMAT ) == 0 ) &&
		 ( ( cfg_ptr[SF_LENGTH_BYTE] & CONSISTENCY_WHOLE_LENGTH ) == 0 ) )
	{
		switch ( cfg_ptr[SF_HEADER_BYTE] & VPC3_CFG_SF_IO_EXIST )
		{
			case VPC3_CFG_SF_OUTP_EXIST:
				// next VPC3_UBYTE contains the length of outp_data
				module_ptr->data_size = ( VPC3_UBYTE ) ( ( cfg_ptr[SF_LENGTH_BYTE] & VPC3_CFG_SF_LENGTH ) + 1 );
				if ( cfg_ptr[SF_LENGTH_BYTE] & VPC3_CFG_LENGTH_IS_WORD_FORMAT )
				{
					module_ptr->data_size = ( 2 * module_ptr->data_size );
				}
				module_ptr->mod_type = OUTPUT;
				break;

			case VPC3_CFG_SF_INP_EXIST:
				// next VPC3_UBYTE contains the length of inp_data
				module_ptr->data_size = ( VPC3_UBYTE ) ( ( cfg_ptr[SF_LENGTH_BYTE] & VPC3_CFG_SF_LENGTH ) + 1 );
				if ( cfg_ptr[SF_LENGTH_BYTE] & VPC3_CFG_LENGTH_IS_WORD_FORMAT )
				{
					module_ptr->data_size = ( 2 * module_ptr->data_size );
				}
				module_ptr->mod_type = INPUT;
				break;

			case VPC3_CFG_SF_IO_EXIST:
				error = CFG_NOT_SUPPORTED_ERROR;
				break;

			default:
				error = CFG_NOT_SUPPORTED_ERROR;
				break;
		}
		specific_data_length = ( cfg_ptr[SF_HEADER_BYTE] & VPC3_CFG_BF_LENGTH );
		if ( ( specific_data_length == 2 ) && ( error == CFG_OK ) )
		{
			module_ptr->profi_id = ( cfg_ptr[SF_MSB_PROFI_ID_BYTE] << 8 );	// Shift MSB of ID into prm_id
			module_ptr->profi_id |= cfg_ptr[SF_LSB_PROFI_ID_BYTE];			// LSB of ID
		}
		else
		{
			error = CFG_NOT_SUPPORTED_ERROR;
		}
	}
	else
	{
		error = CFG_NOT_SUPPORTED_ERROR;
	}
	return ( error );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ----Function Name: Check_Module_Data_Length()------------------------------------------
 * -----Description: Checks the data length of a ProfiBus ID with the length specified ---
 * --------in the MODULE_STRUCT ----------------------------------------------------------
 * ----Inputs: ---------------------------------------------------------------------------
 * ------arg#1: MODULE_STRUCT* module_ptr - Pointer to beginning of MODULE_STRUCT---------
 * -------------Structure contains IO module data parsed from configuration buffer--------
 * ---------------------------------------------------------------------------------------
 * ----Outputs:---------------------------------------------------------------------------
 * ------return: BOOL length_match - -----------------------------------------------------
 * --------TRUE      << Length specified in cfg_data matches that of DCI parameter--------
 * --------FALSE     << Length specified in cfg_data does not match that of DCI parameter-
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool ProfiBus_Prod_Cons::Check_Module_Data_Length( MODULE_STRUCT* module_ptr )
{
	DCI_LENGTH_TD dci_data_length = 0;
	bool length_match = FALSE;


	dci_data_length = m_profibus_dci_ctrl->Get_Length( module_ptr->profi_id );
	if ( dci_data_length == module_ptr->data_size )
	{
		length_match = TRUE;
	}
	return ( length_match );
}

/*
 *****************************************************************************************
 * --- Function Header -------------------------------------------------------------------
 * ----Function Name: Refresh_Prod()------------------------------------------------------
 * -----Description: Refreshes the data in the PROFIBUS_PROD_DATA DCI parameter ----------
 * -------Function gets the data format and prod list from the DCI then updates the data--
 * -------in the PROFIBUS_PROD_DATA parameter by copying each individual parameter's -----
 * -------data.  The PROFIBUS_PROD_DATA parameter is locked during this transaction-------
 * ---------------------------------------------------------------------------------------
 * -----Inputs: - None -------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * -----Outputs: Data in PROFIBUS_PROD_DATA has been refreshed ---------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */


void ProfiBus_Prod_Cons::Refresh_Prod()
{
	DCI_ERROR_TD error = PROFI_ERR_NO_ERROR;
	DCI_LENGTH_TD data_length = 0;
	DCI_LENGTH_TD total_length = 0;
	DCI_LENGTH_TD index;
	PROFI_ID_TD prod_list_mod;


	m_prod_data_owner->Lock();

	for ( index = 0; ( index < m_total_prod_modules ) && ( error == DCI_ERR_NO_ERROR ); index++ )
	{
		m_prod_list_owner->Check_Out_Index( &prod_list_mod, index );

		error = m_profibus_dci_ctrl->Get_Profi_Data( prod_list_mod, &data_length,
													 ( m_prod_data_ptr + total_length ) );
		m_prod_list_owner->Check_In_Index( &prod_list_mod, index );
		total_length += data_length;
	}

	if ( error == PROFI_ERR_NO_ERROR )
	{
		m_prod_data_owner->Check_In();
		m_prod_data_owner->Unlock();
		m_prod_data_change = TRUE;
	}
}

/*
 *****************************************************************************************
 * ----Function Header -------------------------------------------------------------------
 * ----Function Name: Copy_Prod_Data()----------------------------------------------------
 * ----Description: Copies the data from the PROFIBUS_PROD_DATA parameter in the DCI to---
 * --------------the destination buffer pointed to by dest_data if the data has changed---
 * ---------------------------------------------------------------------------------------
 * ----Inputs: ---------------------------------------------------------------------------
 * ------arg#1: uint8_t* dest_data - Pointer to start of buffer where data should be copied-
 * ------arg#2: DCI_LENGTH_TD max_buffer_length - Length of the available buffer----------
 * --------------Used to ensure the dest buffer is not overrun----------------------------
 * ------m_prod_data_change: indicates the PROFIBUS_PROD_DATA parameter has been updated--
 * ----Outputs: --------------------------------------------------------------------------
 * ------Data from PROFIBUS_PROD_DATA parameter is copied to dest buffer------------------
 * ------return: DCI_ERROR_TD error - ----------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
DCI_ERROR_TD ProfiBus_Prod_Cons::Copy_Prod_Data( uint8_t* dest_data, DCI_LENGTH_TD max_buffer_length )
{
	DCI_ERROR_TD error = PROFI_ERR_NO_ERROR;
	DCI_LENGTH_TD length = 0;
	DCI_LENGTH_TD data_length = 0;


	if ( m_prod_data_change == TRUE )
	{
		length = m_profibus_dci_ctrl->Get_Length( DCI_PROFIBUS_PROD_DATA_PROFIBUS_MOD );
		if ( max_buffer_length >= length )
		{
			error = m_profibus_dci_ctrl->Get_Profi_Data( DCI_PROFIBUS_PROD_DATA_PROFIBUS_MOD, &data_length,
														 dest_data );
			m_prod_data_change = FALSE;
		}
		else
		{
			error = PROFI_ERR_INVALID_DATA_LENGTH;
		}
	}
	return ( error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void ProfiBus_Prod_Cons::Start_Prod_Scan( BF_Lib::Timers::TIMERS_TIME_TD refresh_rate )
{
	if ( refresh_rate > 0 )
	{
		m_prod_timer->Start( refresh_rate, TRUE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void ProfiBus_Prod_Cons::Stop_Prod_Scan()
{
	m_prod_timer->Stop();
}

/*
 *****************************************************************************************
 * ----Function Header -------------------------------------------------------------------
 * ----Function Name: Copy_Cons_Data()----------------------------------------------------
 * ----Description: Copies the data from the src buffer to the DCI parameters listed in---
 * -------the PROFIBUS_CONS_LIST parameter in the DCI. Function gets PROFIBUS_CONS_LIST---
 * -------and copies the data in the buffer to each individual parameter in the DCI-------
 * ----Inputs: ---------------------------------------------------------------------------
 * ------arg#1: uint8_t* src_data - Pointer to begining of Consuption data buffer-----------
 * ---------------------------------------------------------------------------------------
 * ----Outputs: --------------------------------------------------------------------------
 * -------Data in src_buffer is copied to each individual CONS_LIST parameter in DCI------
 * -------return: DCI_ERROR_TD error - ---------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

DCI_ERROR_TD ProfiBus_Prod_Cons::Copy_Cons_Data( uint8_t* src_data )
{
	DCI_ERROR_TD error = PROFI_ERR_NO_ERROR;
	DCI_LENGTH_TD data_len = 0;
	DCI_LENGTH_TD total_data_len = 0;
	PROFI_ID_TD cons_list_mod;


	for ( uint8_t index = 0; index < m_total_cons_modules; index++ )
	{
		m_cons_list_owner->Check_Out_Index( &cons_list_mod, index );
		data_len = m_profibus_dci_ctrl->Get_Length( cons_list_mod );
		error = m_profibus_dci_ctrl->Set_Profi_IO_Data( cons_list_mod, data_len, ( src_data + total_data_len ) );
		m_cons_list_owner->Check_In_Index( &cons_list_mod, index );
		total_data_len += data_len;
	}
	return ( error );
}

/*
 *****************************************************************************************
 * ----Function Header -------------------------------------------------------------------
 * ----Function Name: Clear_Cons_Data()---------------------------------------------------
 * ----Description: Zeros the data in the DCI parameters listed in------------------------
 * -------the PROFIBUS_CONS_LIST parameter in the DCI. Function gets PROFIBUS_CONS_LIST---
 * -------and zeros the data for each individual parameter in the DCI---------------------
 * ----Inputs: ---------------------------------------------------------------------------
 * ------arg#1: uint8_t zero_data - zero data byte------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ----Outputs: --------------------------------------------------------------------------
 * -------Data in src_buffer is copied to each individual CONS_LIST parameter in DCI------
 * -------return: DCI_ERROR_TD error - ---------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */

void ProfiBus_Prod_Cons::Clear_MI_Run( void )
{
	BF_Lib::SPLIT_UINT16 relay_off_command;

	relay_off_command.u16 = 0xDD00;

	m_profibus_dci_ctrl->Set_Profi_IO_Data( DCI_COMMAND_REGISTER_PROFIBUS_MOD, 2, &relay_off_command.u8[0] );


	return;
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---       Check_Config_Data( uint8_t* cfg_buffer_ptr, uint8_t* rd_cfg_buffer_ptr,      ----
 * ---                       uint8_t cfg_data_len, uint8_t rd_cfg_data_len )              ----
 * ---                                                                                ----
 * Function: Verifies the contents from the configuration buffer                      ----
 * -Compares the cfg_buffer contents against the rd_cfg_buffer                        ----
 * -If different, function checks to see if all modules are supported for IO exchange ----
 * -----------Returns VPC3_CFG_OK (0) if cfg_buffer & rd_cfg_buffer match             ----
 *------------Returns VPC3_CFG_NOT_OK (1)  if a module is not supported               ----
 *------------Returns VPC3_CFG_UPDATE (2)  if rd_cfg_buffer needs to be updated       ----
 *****************************************************************************************
 */

uint8_t ProfiBus_Prod_Cons::Check_New_Config_Data( uint8_t* cfg_buffer_ptr, uint8_t* rd_cfg_buffer_ptr,
												   uint8_t cfg_data_len, uint8_t rd_cfg_data_len )
{
	MODULE_STRUCT io_module;
	uint8_t error = CFG_OK;
	uint8_t cfg_status = VPC3_CFG_OK;
	DCI_LENGTH_TD prod_data_size = 0;
	DCI_LENGTH_TD cons_data_size = 0;
	uint8_t prod_modules = 0;
	uint8_t cons_modules = 0;
	bool cfg_match = FALSE;

	DCI_ID_TD* id_ptr;
	DCI_LENGTH_TD length = 0;
	DCI_LENGTH_TD data_length = 0;
	DCI_ID_TD prod_list[MAX_PROD_MODULES];
	DCI_ID_TD cons_list[MAX_CONS_MODULES];


	// Check for proper cfg_len
	if ( ( cfg_data_len % SPECIAL_FORMAT_CFG_LENGTH == 0 ) && ( cfg_data_len <= MAX_CK_CFG_LEN ) )
	{
		// Check sent configuration against real configuration stored in the ProfiChip
		// if they match - CFG_OK
		if ( cfg_data_len == rd_cfg_data_len )
		{
			for ( uint_fast8_t index = 0; ( index < cfg_data_len ) && ( cfg_match == TRUE ); index++ )
			{
				if ( cfg_buffer_ptr[index] == rd_cfg_buffer_ptr[index] )
				{
					cfg_match = TRUE;
				}
				else
				{
					cfg_match = FALSE;
				}
			}
		}

		// If the sent configuration does not match the real configuration stored in the ProfiChip
		// Attempt to build a new configuration from the sent configuration data.
		// Parses each individual IO module from the cfg data.
		// Verifys each IO module is available in the DCI and
		// Compares the DCI datalength against the length reported in the cfg data
		// If no errors, the IO module is OK and available for IO

		if ( cfg_match == FALSE )
		{
			for ( uint_fast8_t index = 0;
				  ( index < cfg_data_len ) && ( error == CFG_OK );
				  index += SPECIAL_FORMAT_CFG_LENGTH )
			{
				error = Get_Module_Data( ( cfg_buffer_ptr + index ), &io_module );
				if ( ( io_module.profi_id >= IO_MOD_START_ID ) && ( io_module.profi_id <= IO_MOD_END_ID ) )
				{
					if ( ( error == CFG_OK ) && ( Check_Module_Data_Length( &io_module ) ) )
					{
						switch ( io_module.mod_type )
						{
							case INPUT:
								if ( prod_modules < MAX_PROD_MODULES )
								{
									prod_list[prod_modules] = io_module.profi_id;
									prod_data_size += io_module.data_size;
									prod_modules++;
								}
								else
								{
									error = CFG_MAX_IO_MODULE_ERROR;
									cfg_status = VPC3_CFG_FAULT;
								}
								break;

							case OUTPUT:
								if ( cons_modules < MAX_CONS_MODULES )
								{
									cons_list[cons_modules] = io_module.profi_id;
									cons_data_size += io_module.data_size;
									cons_modules++;
								}
								else
								{
									error = CFG_MAX_IO_MODULE_ERROR;
									cfg_status = VPC3_CFG_FAULT;
								}
								break;

							default:
								error = CFG_NOT_SUPPORTED_ERROR;
								break;
						}
					}
					else
					{
						error = CFG_NOT_SUPPORTED_ERROR;
						cfg_status = VPC3_CFG_FAULT;
					}
				}
				else
				{
					error = CFG_NOT_SUPPORTED_ERROR;
					cfg_status = VPC3_CFG_FAULT;
				}
			}

			if ( ( prod_data_size > MAX_PROD_SIZE ) || ( cons_data_size > MAX_CONS_SIZE ) )
			{
				error = CFG_MAX_IO_SIZE_ERROR;
				cfg_status = VPC3_CFG_FAULT;
			}
			if ( ( prod_modules > MAX_PROD_MODULES ) || ( cons_modules > MAX_CONS_MODULES ) )
			{
				error = CFG_MAX_IO_MODULE_ERROR;
				cfg_status = VPC3_CFG_FAULT;
			}
			if ( error == CFG_OK )
			{
				data_length = prod_data_size;
				length = ( prod_modules * 2 );
				if ( m_prod_list_owner->Check_In_Length( &length ) )
				{
					id_ptr = &prod_list[0];
					m_prod_list_owner->Check_In( id_ptr );
					m_prod_data_owner->Check_In_Length( &data_length );
				}
				else
				{
					error = CFG_IO_ASSEMBLY_SET_LENGTH_ERROR;
				}
			}
			if ( error == CFG_OK )
			{
				data_length = cons_data_size;
				length = ( cons_modules * 2 );
				if ( m_cons_list_owner->Check_In_Length( &length ) )
				{
					id_ptr = &cons_list[0];
					m_cons_list_owner->Check_In( id_ptr );
				}
				else
				{
					error = CFG_IO_ASSEMBLY_SET_LENGTH_ERROR;
				}
			}
			if ( error == CFG_OK )
			{
				m_total_prod_modules = prod_modules;
				m_total_cons_modules = cons_modules;
				cfg_status = VPC3_CFG_UPDATE;
			}

		}
		else
		{
			cfg_status = VPC3_CFG_OK;
		}
	}
	else
	{
		cfg_status = VPC3_CFG_FAULT;
	}

	return ( cfg_status );
}
