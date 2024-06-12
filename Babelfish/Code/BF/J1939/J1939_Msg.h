/**
 *****************************************************************************************
 *	@file
 *
 *	@brief J1939 message structure
 *
 *	@details This module will provide basic J1939 message structure (Which is derived from Simma stack)
 *			If Simma stack is used then don't include this file. Include j1939.h file inside J1939_DCI_Interface.h file
 *			The message structure is present inside j1939.h.
 *
 *	@copyright 2017 Eaton Corporation. All Rights Reserved.
 *
 ***/

#ifndef J1939_MSG_H
	#define J1939_MSG_H

/*			This module will provide basic J1939 message structure (Which is derived from Simma stack)
 *			If Simma stack is used then don't include this file. Include j1939.h file inside J1939_DCI_Interface.h file
 *			The message structure is present inside j1939.h.
 */

typedef struct
{

	uint32_t pgn;	/* parameter group number */
	uint8_t* buf;	/* pointer to data */
	uint16_t buf_len;	/* size of data */
	uint8_t dst;	/* destination of message */
	uint8_t src;	/* source of message */
	uint8_t pri;	/* priority of message */
	uint8_t port;	/* CAN port of message */

} j1939_t;

#endif
