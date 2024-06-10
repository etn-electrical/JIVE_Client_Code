/**
 *****************************************************************************************
 * @file Language_Config.h
 *
 * @brief
 * @details : This file contains variables and Macros for language configuration
 *
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LANGUAGE_CONFIG_H
#define LANGUAGE_CONFIG_H

#include "Includes.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/* Count of supported REST URI extentions */
const uint16_t PROD_SPEC_REST_ROOT = 0U;
const uint16_t PROD_SPEC_REST_LANG = 1U;
const uint16_t PROD_SPEC_COUNT_REST = 2U;

/* Maximum value of supported language specification's version */
static const uint32_t LG_MAX_SPEC_VER = 1U;

/* List of Supported REST Paths in Prod Spec
   While adding new rest path, add the same in XML_RS_ROOT in httpd_xml.h
   XML_RS_ROOT has the list of all rest paths, the list is sent as a response for "GET /rs" URI.
   rest path length depends on REST_PATH_MAX_LEN.
 */
const uint8_t* const prod_spec_rest_paths[PROD_SPEC_COUNT_REST] =
{
	( uint8_t* ) "/",
	( uint8_t* ) "/lang"
};

/* Max length of language code */
const uint16_t LG_PATH_MAX_LEN = 10U;

const uint8_t LG_CODE_MAX_BYTES = 8;///< Maximum bytes for language code
const uint8_t LG_NAME_MAX_BYTES = 32;	///< Maximum bytes for language name

enum lang_enum_t
{
	LG_ENGLISH = 0U,
	LG_FRENCH = 1U,
	LG_GERMAN = 2U,
	LG_SPANISH = 3U,
	LG_CHINESE = 4U,
	LG_ARABIC = 5U,
	LG_HEBREW = 6U,
	LG_MAX
};

/* enum for compression methods */
enum compress_enum_t
{
	NO_COMPRESSION = 0U,
	GZIP = 1U,
	TOTAL_COMPRESS_METHOD
};

/* Supported Languages Paths (language codes) */
const uint8_t* const rest_lang_paths[LG_MAX] =
{
	( uint8_t* ) "/en",	// English
	( uint8_t* ) "/fr",	// French
	( uint8_t* ) "/de",	// German
	( uint8_t* ) "/es",	// Spanish
	( uint8_t* ) "/zh",	// Chinese
	( uint8_t* ) "/ar",	// Arabic
	( uint8_t* ) "/he"	// Hebrew
};

#endif	// LANGUAGE_CONFIG_H
