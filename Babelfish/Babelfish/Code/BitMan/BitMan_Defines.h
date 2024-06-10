/**
 *****************************************************************************************
 * @file
 *
 * @brief This file contains the definitions of the data structures used by the BitMan
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BITMAN_DEFINES_H_
#define BITMAN_DEFINES_H_

#include "Includes.h"
// Babelfish Includes
#include "DCI_Defines.h"

/**
 *****************************************************************************************
 * @brief The "Logic" namespace contains all code related to logic functions such as
 * Bitman and Logic Engine in the future
 *
 *****************************************************************************************
 */
namespace Logic
{

typedef uint16_t BITMAN_TD;

/**
 * @brief Source & Sink parameters Buffers
 * @details Commandments:
 *
 */
typedef struct BitMan_Base_Structure_t
{
	BITMAN_TD m_sink_dcid_list_len;
	DCI_ID_TD const* m_sink_dcid_list;

	BITMAN_TD m_source_dcid_list_len;
	DCI_ID_TD const* m_source_dcid_list;
} BitMan_Base_Structure;

typedef struct BitMan_Profile_Map_t
{
	BITMAN_TD m_source_dcid_bitfield;
	BITMAN_TD m_sink_dcid_bitfield;
} BitMan_Profile_Map;

typedef struct BitMan_Profile_t
{
	BitMan_Profile_Map const* m_profile_map;
	BITMAN_TD m_map_length;
} BitMan_Profile;

typedef enum
{
	NO_ERRORS = 0U,
	SINK_REMAP_ERROR,
	SINK_DCI_ACCESS_VIOLATION,
	SOURCE_DCI_ACCESS_VIOLATION,
} Bitman_Error_t;

}

#endif	/* BITMAN_DEFINES_H_ */
