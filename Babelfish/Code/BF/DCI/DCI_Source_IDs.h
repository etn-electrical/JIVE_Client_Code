/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_SOURCE_IDS_H
#define DCI_SOURCE_IDS_H

/*
 *********************************************************
 *		Source ID List
 *********************************************************
 */
static const DCI_SOURCE_ID_TD DCI_UNKNOWN_SOURCE_ID =
	static_cast<DCI_SOURCE_ID_TD>( ~static_cast<DCI_SOURCE_ID_TD>( 0U ) );
static const DCI_SOURCE_ID_TD DCI_OWNER_SOURCE_ID = 1U;

/// The following DNET source ID shall be removed when we switch DNET stacks.
static const DCI_SOURCE_ID_TD DNET_DCI_SOURCE_ID = 2U;

/**
 * Source id where we start dynamically allocating IDs.  The number is quite irrelevant
 * except to oneself.  It can be used when using a callback in your own code for example.
 */
static const DCI_SOURCE_ID_TD REST_DCI_SOURCE_ID = 9U;
static const DCI_SOURCE_ID_TD DCI_SOURCE_ID_BEGIN_DYNAMIC = 10U;
static const DCI_SOURCE_ID_TD DCI_SOURCE_ID_BEGIN_PRIVILEGED_DYNAMIC = 0x80U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline DCI_SOURCE_ID_TD DCI_SOURCE_IDS_Get( void )
{
	static uint8_t source_id_ctr = DCI_SOURCE_ID_BEGIN_DYNAMIC;

	return ( source_id_ctr++ );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline DCI_SOURCE_ID_TD DCI_SOURCE_IDS_Get_Privileged( void )
{
	static uint8_t source_id_ctr = DCI_SOURCE_ID_BEGIN_PRIVILEGED_DYNAMIC;

	return ( source_id_ctr++ );
}

#endif
