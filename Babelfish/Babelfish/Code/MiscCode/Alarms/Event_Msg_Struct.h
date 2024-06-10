/**
 *****************************************************************************************
 *  @file

 *	@brief event message structures.
 *
 *	@details This file will provide data structures required for event operation.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 **/

#ifndef _EVENT_MSG_STRUCT_H_
#define _EVENT_MSG_STRUCT_H_

#include "Includes.h"


/*
 *****************************************************************************************
 *		Message Passing Structures
 *****************************************************************************************
 */

namespace BF_Event
{

typedef uint8_t status_bitmask_t;
typedef uint8_t rule_t;
typedef rule_t active_event_t;


/**
   @brief Structure for event data generate from event Manger
 */
struct new_event_t
{
	uint8_t rule_id;
	status_bitmask_t event_status;
};

/**
   @brief callback function param for raised event
 */
typedef void ( * event_interface_cb_t )( new_event_t new_event_cb );

}

#endif	/* _EVENT_MSG_STRUCT_H_ */
