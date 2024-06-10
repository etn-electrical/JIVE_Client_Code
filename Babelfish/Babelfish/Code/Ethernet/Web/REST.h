/**
 *****************************************************************************************
 *	@file        REST header file for the webservices.
 *
 *	@brief       This file shall wrap the functions required for webservices using REST protocol.
 *
 *	@details     The LTK REST services are designed primarily for use from the adopting  product web front end for
 * browser based data access. However, they provide an open and standard method for data access from any supporting REST
 * client
 *

 *      @copyright   2015 Eaton Corporation. All Rights Reserved.
 *
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed here in.  This technical
 * information should not be reproduced or used without direct written permission from Eaton Corporation.
 *
 *
 *
 *
 *
 *
 *****************************************************************************************
 */

#ifndef __REST_H__
#define __REST_H__

#include "hams.h"
/** External user defined functions required for REST services to function.
 * @brief       This function is used to write the XML tag name and REST parameter id.
 * @param[in]parser   Pointer to parser structure pDCI     Pointer to DCID lookup structure consisting of REST parameter
 * ids.
 *           param    Parameter
 * @return[out] bool  Returns either true or false
 */
extern bool getXMLParamSingleItem( hams_parser* parser, const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
								   uint32_t param, uint32_t verbose );

/**
 * @brief       This function is to set a numerical value of the parameter according to parameter datatype.
 * @param[in]dest     Pointer to destination/target.
 *          datatype  Datatype of particular parameter
 *
 * @return[out] int   Returns a numeric value for the parameter.
 */
extern int set_param_numeric( ReusedRecvBuf* dest, const struct yxml_put_xml_state* src,
							  uint32_t datatype );

/**
 * @brief       This function is used to write a value for a particular parameter.
 * @param[in]pTarget   Pointer to target/destination pDCI      Pointer to DCID lookup structure consisting of REST
 * parameter ids.
 *          data_block Pointer to datablock structure.
 *          realWrite  Boolean True or false value depending on whether parameter can be written or not.
 *         param_type  Parameter Type:Whether Read Only or R/W(Can be written).
 * @return[out] uint_16    Returns a REST error code depending on whether write is supporetd or not.
 */

extern uint16_t putParamValue( const DCI_REST_TO_DCID_LKUP_ST_TD* pDCI,
							   ReusedRecvBuf* pTarget, const DCI_DATA_BLOCK_TD* data_block,
							   bool realWrite, uint32_t param_type );

/**
 * @brief              This function is used to find log of a number to base 10.
 * @param[in]number    The number whose log is to be found
 * @return[out] int    Returns  a numerical value of the log to the base 10.
 */

extern uint16_t Find_Base_Log_Ten( uint16_t number );


extern void Initialize_pTarget_Buffer( ReusedRecvBuf* pTarget );

extern void Clone_pTarget_Present_Data( uint8_t* present_dest, const uint8_t* present_src, uint16_t num_of_bytes );

extern DCI_Patron* httpd_dci_access;


#define MAX_QUOTE_IN_CRED_STRING    2U
#define IS_EXTRA_SLASH( data ) ( *( uint16_t* )data == 0x2F2F ? true : false )
#define ENDS_WITH_SLASH( data ) ( *( uint16_t* )data == 0x202F ? true : false )
#define IS_SINGLE_EXTRA_SLASH( data ) ( *( uint8_t* )data == 0x2F ? true : false )

static const uint8_t REST_BIT_LIST_WORD_MULT_FCTR = sizeof( present_t ) * 8U;
static const uint8_t REST_BIT_LIST_SINGLE_WORD_BIT_MASK = REST_BIT_LIST_WORD_MULT_FCTR - 1U;
static const uint8_t REST_BIT_LIST_WORD_SHFT_FCTR = ( sizeof( present_t ) / 2U ) + 3U;

const uint16_t REST_PATH_MAX_LEN = 256;
const uint8_t* const s_rest_paths[COUNT_REST] =
{
	reinterpret_cast<const uint8_t*>( "/" ),
	reinterpret_cast<const uint8_t*>( "/assy" ),
	reinterpret_cast<const uint8_t*>( "/device" ),
	reinterpret_cast<const uint8_t*>( "/param" ),
	reinterpret_cast<const uint8_t*>( "/struct" ),
	reinterpret_cast<const uint8_t*>( "/fw" ),
	reinterpret_cast<const uint8_t*>( "/users" ),
	reinterpret_cast<const uint8_t*>( "/log" ),
	reinterpret_cast<const uint8_t*>( "/certstore" )
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline void PTARGET_CLEAR_DATA_PRESENT( present_t* present_bitlist, uint16_t offset )
{
	present_bitlist[offset >> REST_BIT_LIST_WORD_SHFT_FCTR] &= ~( 1U
																  << ( offset & REST_BIT_LIST_SINGLE_WORD_BIT_MASK ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline void PTARGET_SET_DATA_PRESENT( present_t* present_bitlist, uint16_t offset )
{
	present_bitlist[offset >> REST_BIT_LIST_WORD_SHFT_FCTR] |= 1U
		<< ( offset & REST_BIT_LIST_SINGLE_WORD_BIT_MASK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline bool PTARGET_DATA_PRESENT( present_t* present_bitlist, uint16_t offset )
{
	return ( ( ( present_bitlist[offset >> REST_BIT_LIST_WORD_SHFT_FCTR] )
			   & ( 1U << ( offset & REST_BIT_LIST_SINGLE_WORD_BIT_MASK ) ) ) != 0U );
}

/**
 * @brief Callback for /rs request
 * @details This function is called on /rs request request, For "GET /rs" request it returns all available REST paths in
 * response
 * @param[in] parser : Pointer to the parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return Success or failure of requested operation.
 */
uint32_t s_rest_root_hdrs_end( hams_parser* parser, const uint8_t* data, uint32_t length );

#endif
