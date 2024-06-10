/*
 **********************************************************************************************
 * @file            String_Sanitize_Whitelists.h
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the different white-list used by Babelfish code to sanitize ACII(string)
 * parameters.
 *                  the white-list lookup can be easily created using RTK_Example\Tools\Sanitizer Bit list Creator.xlsx.
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "String_Sanitizer.hpp"
#include "Sntp_Config_ESP32.h"

/*
 **************************************************************************************************
 * this bit list is use for sanitizing user input parameter like IOT username, IOT password,
 * app user name
 * The whitelist_str_Sanitizer allows following set of characters
 * A - Z
 * a - z
 * 0 - 9
 * '-' , '.' , '_' , NULL, space, '"'
 **************************************************************************************************
 */
const BF_Lib::String_Sanitizer::str_sanitize_word_t
	whitelist_str_sanitize_lookup[BF_Lib::String_Sanitizer::STR_SANTIZE_LOOKUP_WORD_LEN] =
{
	0x1, 0x0, 0x0, 0x0, 0x5, 0x60, 0xFF, 0x3, 0xFE, 0xFF, 0xFF, 0x87, 0xFE, 0xFF, 0xFF, 0x7 };

/*
 **************************************************************************************************
 * The device_str_sanitizer (Device UUID) allows following set of characters
 * A - Z
 * a - z
 * 0 - 9
 * '-' , NULL, space, '"'
 **************************************************************************************************
 */
const BF_Lib::String_Sanitizer::str_sanitize_word_t
	uuid_sanitize_lookup[BF_Lib::String_Sanitizer::STR_SANTIZE_LOOKUP_WORD_LEN] =
{
	0x1, 0x0, 0x0, 0x0, 0x5, 0x20, 0xFF, 0x3, 0xFE, 0xFF, 0xFF, 0x7, 0xFE, 0xFF, 0xFF, 0x7 };

/*
 **************************************************************************************************
 * The conn_str_sanitizer allows following set of characters
 * A - Z
 * a - z
 * 0 - 9
 * + '-' . / ; = , NULL, '"'
 **************************************************************************************************
 */
const BF_Lib::String_Sanitizer::str_sanitize_word_t
	conn_str_sanitize_lookup[BF_Lib::String_Sanitizer::STR_SANTIZE_LOOKUP_WORD_LEN] =
{
	0x1, 0x0, 0x0, 0x0, 0x4, 0xE8, 0xFF, 0x2B, 0xFE, 0xFF, 0xFF, 0x7, 0xFE, 0xFF, 0xFF, 0x7 };

#ifdef SNTP_DOMAIN_NAME_SUPPORT
/*
 **************************************************************************************************
 * this bit list is use for sanitizing user input parameter like Time Server 1/2/3
 * The whitelist_domain_name_Sanitizer allows following set of characters
 * as per the standards mentioned in the https://en.wikipedia.org/wiki/Hostname
 * A - Z
 * a - z
 * 0 - 9
 * '-' , '.' , '_', NULL
 * However RFC-952 standard disallowed labels starts with hyphen character
 **************************************************************************************************
 */
const BF_Lib::String_Sanitizer::str_sanitize_word_t
	sntp_dns_sanitize_lookup[BF_Lib::String_Sanitizer::STR_SANTIZE_LOOKUP_WORD_LEN] =
{
	0x1, 0x0, 0x0, 0x0, 0x0, 0x60, 0xFF, 0x3, 0xFE, 0xFF, 0xFF, 0x87, 0xFE, 0xFF, 0xFF, 0x7 };

#endif
/*
 **************************************************************************************************
 *  @brief It is designed to work as basic ASCII character sanitizer for string parameters.
 *  An object of String_Sanitizer Class is created by by passing (constant) Bit-addressable ASCII white-list,
 *  thereafter String_Sanitizer::Sanitize function can be used  to validate the input string against the associated
 *  white-list provided.
 *  @n @b Usage: An example if you have input string parameters and you want to sanitize the input
 *  values before it gets saved into NV.This can be achieved by creating a (constant and compile time only) object.
 *  RTK_Example\Tools\Sanitizer Bit list Creator.xlsx is used to create the bit list. Adopter need
 *  to specify the allowed character in the excel by putting 1 in the (allow? set to 1)column of
 *  excel sheet.Based on the allowed characters specified the const sanitizer bit list will be
 *  created.
 *  If white-list requirement doesn't match with any of the lookup tables provided then the Adopter
 *  is required to modify the constant whitelist arrays as per requirement. This header can be extended for additional
 *  white-list look-ups specific to product parameters. as per Babelfish requirements the (constant)objects
 *  are created in String_Sanitizer at compile time so that RAM consumption is nil.
 **************************************************************************************************
 */

