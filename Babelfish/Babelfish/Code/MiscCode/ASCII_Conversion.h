/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "DCI_Defines.h"

/**
 * @brief Constants used for float and double precision.
 * @details The float and double precisions are needed for converting float/double datatype
 * value to a string datatype.
 */
static const uint8_t FLOAT_PREC = 7U;
static const uint8_t DOUBLE_PREC = 15U;
static const uint8_t EXTRA_BYTES = 7U;			///< decimal+scientific notation+exponent = 7U

/**
 * @brief Callback function to get log details.
 * @details This function converts any data type to string and returns number of characters in the
 * string
 * @param[in] data: location of the source data to be converted
 * @param[in] dest_data: location to store the converted data
 * @param[in] datatype: datatype of the data sent to be converted.
 * @param[in] offset: log data offset where the data is located.
 * @param[in] output_format: for decimal pass TRUE for hex pass FALSE .
 * @return number of characters in the string after conversion.
 */
uint16_t Convert_To_String( uint8_t* data, uint8_t* dest_data, DCI_DATATYPE_TD datatype,
							uint16_t offset, bool output_format );
