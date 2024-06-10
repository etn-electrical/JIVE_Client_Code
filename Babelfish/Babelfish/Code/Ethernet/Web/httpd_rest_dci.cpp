/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "httpd_common.h"
#include "httpd_rest_dci.h"

#include "INT_ASCII_Conversion.h"

#include "Includes.h"

#include "httpd_xml.h"

REST_TARGET_INFO_ST_TD const* httpd_rest_dci::m_rest_struct =
	static_cast<REST_TARGET_INFO_ST_TD const*>( NULL );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void httpd_rest_dci::init_httpd_rest_dci()
{
	new DCI_Owner( DCI_VALID_BITS_ASSY_0_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_1_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_2_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_3_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_4_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_5_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_6_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_7_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_8_DCID );
	new DCI_Owner( DCI_VALID_BITS_ASSY_9_DCID );

	new DCI_Owner( DCI_MEMBERS_ASSY_0_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_1_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_2_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_3_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_4_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_5_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_6_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_7_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_8_DCID );
	new DCI_Owner( DCI_MEMBERS_ASSY_9_DCID );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const DCI_REST_TO_DCID_LKUP_ST_TD* httpd_rest_dci::findIndexREST( uint16_t param_id )
{
	const DCI_REST_TO_DCID_LKUP_ST_TD* search_struct = m_rest_struct->param_list;
	uint16_t begin = 0U;
	uint16_t end = m_rest_struct->number_of_parameters - 1U;
	uint16_t middle;
	const DCI_REST_TO_DCID_LKUP_ST_TD* return_val =
		static_cast<const DCI_REST_TO_DCID_LKUP_ST_TD*>( NULL );
	bool_t loop_break = false;

	while ( ( begin <= end ) && ( false == loop_break ) )
	{
		middle = ( begin + end ) >> 1;
		if ( search_struct[middle].param_id < param_id )
		{
			begin = middle + 1U;
		}
		else if ( search_struct[middle].param_id > param_id )
		{
			if ( middle > 0U )
			{
				end = middle - 1U;
			}
			else
			{
				return_val = static_cast<const DCI_REST_TO_DCID_LKUP_ST_TD*>( NULL );
				loop_break = true;
			}
		}
		else
		{
			return_val = ( &search_struct[middle] );
			loop_break = true;
		}
	}

	return ( return_val );	// There is a return above here.  It just looks cleaner with the return above.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t httpd_rest_dci::getParamAllSize( uint8_t req_access )
{
	uint32_t size = STRLEN( XML_PARAM_ALL_PREFIX ) + STRLEN( XML_PARAM_ALL_SUFFIX );

	for ( uint16_t i = 0U; i < m_rest_struct->number_of_parameters; i++ )
	{
		if ( includedInParamAll( i, req_access ) )
		{
			size += XML_PARAM_ALL_LINE_SIZE_BARE;
#ifndef REMOVE_REST_PARAM_NAME_TEXT
			size += m_rest_struct->param_list[i].lenName;
#endif
			size +=
				static_cast<uint32_t>( ( static_cast<uint8_t>( INT_ASCII_Conversion::UINT16_Number_Digits(
																   static_cast<uint8_t>( m_rest_struct->param_list[i].
																						 param_id ) ) ) ) << 1U );	// pid
																													// appears
																													// twice
		}
	}

	return ( size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t httpd_rest_dci::getAssyAllSize( uint8_t req_access )
{
	uint32_t size = static_cast<uint32_t>( STRLEN( XML_ASSY_ALL_PREFIX ) );

	size += STRLEN( XML_ASSY_ALL_ASSY_SLOTS_TOTAL_PREFIX );
	size += INT_ASCII_Conversion::UINT16_Number_Digits(
		m_rest_struct->number_of_predefined_assemblies
		+ m_rest_struct->number_of_custom_assemblies );
	size += STRLEN( XML_ASSY_ALL_ASSY_SLOTS_TOTAL_SUFFIX );

	size += STRLEN( XML_ASSY_ALL_ASSY_SLOTS_LEFT_PREFIX );
	size += STRLEN( "0" );	// only "0" or "1" allowed
	size += STRLEN( XML_ASSY_ALL_ASSY_SLOTS_LEFT_SUFFIX );

	for ( uint16_t i = 0U; i < m_rest_struct->number_of_predefined_assemblies; i++ )
	{
		if ( includedInPredefinedAssyAll( i, req_access ) )
		{
			size += XML_ASSY_ALL_LINE_SIZE_BARE;
			size +=
				static_cast<uint32_t>( static_cast<uint8_t>( ( INT_ASCII_Conversion::UINT16_Number_Digits(
																   m_rest_struct->predefined_assembly_list[i].assy_id ) )
															 << 1U ) );
			size += m_rest_struct->predefined_assembly_list[i].assy_name_len;
			size += STRLEN( "0" );
		}
	}

	for ( uint16_t i = 0U; i < m_rest_struct->number_of_custom_assemblies; i++ )
	{
		if ( includedInCustomAssyAll( i, req_access ) )
		{
			size += XML_ASSY_ALL_LINE_SIZE_BARE;
			size +=
				static_cast<uint32_t>( static_cast<uint8_t>( static_cast<uint8_t>( ( INT_ASCII_Conversion::
																					 UINT16_Number_Digits(
																						 m_rest_struct->
																						 custom_assembly_list[i].assy_id ) )
																				   << 1U ) ) );
			size += m_rest_struct->custom_assembly_list[i].assy_name_len;
			size += STRLEN( "0" );
		}
	}

	size += STRLEN( XML_ASSY_ALL_SUFFIX );
	return ( size );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t httpd_rest_dci::includedInParamAll( uint16_t index, uint8_t req_access )
{
	bool_t return_val = true;

	// if req_access (maximum of m_auth_level and m_exempt_level)
	// smaller than the parameter's visible access level
	// it should not be included
	if (
#ifdef REST_SERVER_FORMAT_HANDLING
		( m_rest_struct->param_list[index].pFormatData == nullptr ) ||
#endif
		( req_access < m_rest_struct->param_list[index].access_r ) )
	{
		return_val = false;
	}
	// password/username/exact match are all "level" related parameters
	// and should not be included in the list
#ifdef REST_SERVER_FORMAT_HANDLING
	else if ( ( 0 ==
				strncmp( "$CRED", m_rest_struct->param_list[index].pFormatData->format_name,
						 strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) ) &&
			  ( STRLEN( "$CRED" ) == strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) )
	{
		return_val = false;
	}
	else if ( ( 0 ==
				strncmp( "WAIVER", m_rest_struct->param_list[index].pFormatData->format_name,
						 strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) ) &&
			  ( STRLEN( "WAIVER" ) == strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) )
	{
		return_val = false;
	}
	else if ( ( 0 ==
				strncmp( "REQUEST_LEVEL", m_rest_struct->param_list[index].pFormatData->format_name,
						 strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) ) &&
			  ( STRLEN( "REQUEST_LEVEL" ) == strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) )
	{
		return_val = false;
	}
	else if ( ( 0 ==
				strncmp( "$EXACT", m_rest_struct->param_list[index].pFormatData->format_name,
						 strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) ) &&
			  ( STRLEN( "$EXACT" ) == strlen( m_rest_struct->param_list[index].pFormatData->format_name ) ) )
	{
		return_val = false;
	}
#endif
	else
	{
		// MISRA Suppress
	}

	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t httpd_rest_dci::getParamExtraSize( uint16_t index )
{
#ifndef REMOVE_REST_PARAM_NAME_TEXT
	return ( static_cast<uint8_t>( ( INT_ASCII_Conversion::UINT16_Number_Digits(
										 m_rest_struct->param_list[index].param_id ) << 1U ) )
			 + m_rest_struct->param_list[index].lenName );

#else
	return ( static_cast<uint8_t>( ( INT_ASCII_Conversion::UINT16_Number_Digits( m_rest_struct->param_list[index].
																				 param_id ) << 1U ) ) );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
const DCI_REST_TO_DCID_LKUP_ST_TD* httpd_rest_dci::Find_Index_Rest_From_Dcid( DCI_ID_TD dcid )
{
	const DCI_REST_TO_DCID_LKUP_ST_TD* search_struct = m_rest_struct->param_list;
	uint16_t index_found = 0U;
	bool_t loop_break = false;
	uint16_t end = m_rest_struct->number_of_parameters;
	const DCI_REST_TO_DCID_LKUP_ST_TD* return_ptr = nullptr;

	while ( ( index_found < end ) && ( loop_break == false ) )
	{
		if ( search_struct[index_found].dcid == dcid )
		{
			return_ptr = ( &search_struct[index_found] );
			loop_break = true;
		}
		index_found++;
	}
	return ( return_ptr );
}
