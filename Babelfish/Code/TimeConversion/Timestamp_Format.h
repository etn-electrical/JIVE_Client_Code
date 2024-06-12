/**
 *****************************************************************************************
 *	@file Timestamp_Format.h
 *
 *	@brief This file includes functions related to timestamp formatting
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef TIMESTAMP_FORMAT_H
#define TIMESTAMP_FORMAT_H

#include <time.h>
#include <stdint.h>

/**
 * @brief This is Timestamp_Format class.
 * @details The Timestamp_Format class provides public methods for converting time to various
 * formats(eg. ISO 8601 format)
 */
class Timestamp_Format
{
	public:
		/**
		 * @brief Convert time to ISO 8601 format
		 * @details This converts the current time(unix epoch in seconds) to ISO 8601
		 * format.
		 * @param[out] timestamp_iso_8601: ISO 8601 formated timestamp
		 * @param[in] now: Unix epoch timestamp that has to be formatted
		 * @return None
		 */
		static void Get_Timestamp_In_ISO_8601_Format( char timestamp_iso_8601[], time_t now );

		/**
		 * Maximum size of timestamp represented in ISO 8601 format. For eg, 2021-05-14T11:24:24.000Z */
		static const uint8_t TIMESTAMP_ISO_8601_SIZE = 25;

	private:

		Timestamp_Format( void );	// do not instantiate
		~Timestamp_Format( void );	// do not destroy

};

#endif	// TIMESTAMP_FORMAT
