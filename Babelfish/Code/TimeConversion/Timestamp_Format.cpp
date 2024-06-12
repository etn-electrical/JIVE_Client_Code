/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Timestamp_Format.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Timestamp_Format::Timestamp_Format()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Timestamp_Format::Get_Timestamp_In_ISO_8601_Format( char timestamp_iso_8601[], time_t now )
{
	struct tm* ts;

	ts = localtime( &now );
	BF_ASSERT( ts );
	strftime( timestamp_iso_8601, TIMESTAMP_ISO_8601_SIZE, "%Y-%m-%dT%H:%M:%S.0Z", ts );
}
