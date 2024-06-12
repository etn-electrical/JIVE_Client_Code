/**
 *****************************************************************************************
 *  @file
 *	@brief Event defines used under Event Manager
 *
 *	@details This file provides macros used for Event operations.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef _EVENT_DEFINE_H_
#define _EVENT_DEFINE_H_

namespace BF_Event
{

enum event_op_status_t
{
	EVENT_NO_ERROR,
	EVENT_ACK_ERROR,
	EVENT_RESET_ERROR,
	NO_EVENT_TO_RESET,
	EVENT_ID_NOT_FOUND,
	EVENT_ALREADY_ACK,
	EVENT_ALREADY_RESET,
	EVENT_RULE_ID_NOT_FOUND,
	EVENT_RULE_OUT_OF_RANGE,
	EVENT_RULE_DISABLE,
	EVENT_NOT_CLEARED,
	EVENT_ACK_LEVEL_MISMATCH
};


}


#endif	/* _EVENT_DEFINE_H_ */
