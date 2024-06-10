/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This module is responsible for data coversion fron DCI raw data to formated data and vice versa.
 *
 *	@details DCI module will contain raw data which has minimum memory foor print. Other module will covert
 *	 raw data to formatted data  or formatted data to raw data using this module.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef PROD_SPEC_FORMAT_HANDLER_H
#define PROD_SPEC_FORMAT_HANDLER_H

#include "includes.h"
#include "DCI_Defines.h"
#include "Format_Handler.h"
#include "Rest_Config.h"

/*
 *****************************************************************************************
 *		Formatter Structure Prototypes
 *****************************************************************************************
 */

struct scale_const_dec_pt_formatter_config_t
{
	int8_t decimal_pt;
};

struct scale_const_int_formatter_config_t
{
	int8_t decimal_pt;
};

struct scale_dci_div_formatter_config_t
{
	DCI_ID_TD dci_reference;
};

struct scale_dci_mult_formatter_config_t
{
	DCI_ID_TD dci_reference;
};


/*
 *****************************************************************************************
 *		Function Prototype (all format handler should be in lower case)
 *****************************************************************************************
 */

/**
 * @brief Handler for SCALE_CONST_INT( x )
 * @param[in] data: pointer to raw data
 * @param[in] string: formatted data
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_SCALE_CONST_INT( void* data, const uint16_t DATA_BUF_SIZE, uint8_t* string,
										 const uint16_t STR_BUF_SIZE, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for SCALE_CONST_DEC_PT( x )
 * @param[in] data: pointer to raw data
 * @param[in] string: formatted data
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_SCALE_CONST_DEC_PT( void* data, const uint16_t DATA_BUF_SIZE, uint8_t* string,
											const uint16_t STR_BUF_SIZE, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for SCALE_DCI_MULT( <DCI_SCALAR_DCID> )
 * @param[in] data: pointer to raw data
 * @param[in] string: formatted data
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_SCALE_DCI_MULT( void* data, const uint16_t DATA_BUF_SIZE, uint8_t* string,
										const uint16_t STR_BUF_SIZE, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for SCALE_DCI_DIV( <DCI_SCALAR_DCID> )
 * @param[in] data: pointer to raw data
 * @param[in] string: formatted data
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_SCALE_DCI_DIV( void* data, const uint16_t DATA_BUF_SIZE, uint8_t* string,
									   const uint16_t STR_BUF_SIZE, conv_type_t dir, void* config = nullptr );

/**
 * @brief Handler for SCALE_HTML_LINK( )
 * @param[in] data: pointer to raw data
 * @param[in] string: formatted data
 * @param[in] dir:RAW_TO_STRING = raw input (data) to formatted (string) output coversion required.
 * STRING_TO_RAW = formatted input (string) to raw output (data) coversion required.
 * @param[in] config: Configuration parameters required for conversion.
 * @return 	Zero 		: Validation failed
 *			Non Zero	: Number of bytes parsed in successful validation.
 */
uint16_t FORMAT_HANDLER_HTML_LINK( void* data, const uint16_t data_buf_size, uint8_t* string,
								   const uint16_t str_buf_size, conv_type_t dir, void* config = nullptr );

#endif	// PROD_SPEC_FORMAT_HANDLER_H
