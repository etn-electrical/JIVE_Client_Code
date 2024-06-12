/**
 **************************************************************************************************
 *  @file
 *  @brief DSI_Pub.h This file provides APIs to access Data Stream APIs
 *
 *	@details Responsible for the publishing of the data.  Doesn't really care too much about how many are at the end.
 * Just rifles through the stream handles and loads the data into each stream independently.
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef DSI_PUB_H
#define DSI_PUB_H

#include "Includes.h"
#include "DSI_Core.h"


class DSI_Pub
{
	public:
		/**
		 * @brief Constructor of DSI publisher
		 * @param[in] dsid: DSID number
		 * @return DSI publisher handle
		 */
		DSI_Pub( dsi_id_t dsid );

		/**
		 * @brief Data gets Published out using this api
		 * A couple notes: We may add a wait here and pass that in so it would be "ticks_t wait_time = 0 )"  For now we
		 * are going to march through without waiting.
		 * what this means though is that a low priority queue could hold up a higher priority queue unless we went
		 * through and ordered them appropriately in the sub list by priority.
		 * This may be a bit dicey.  We also need to have a method of saying that something went wrong on the receiving
		 * side if one of the stream buffers fails.  The publisher really can't do anything about it
		 * So I have left it out for now.
		 * @param[in] data: Data which needs to be published.
		 * @return Operation status
		 */
		dsi_pub_resp_t Pub( dsi_data_t data );

		/**
		 * @brief Data gets Published out using this interrupt safe api
		 * A couple notes:
		 * We will need to be able to publish from an interrupt.  We could hide all this but it may as well be a special
		 * function call instead.  Again here we may need more options to track some other details.
		 * Like did the task wake anything up.  More to this function to be worked through.  We will need to document
		 * that failure is a possibility.
		 * Please list out the possible errors and what could be done about them for both pub and int_pub.  We can come
		 * up with some solutions.  We may need to be able to send a control character across or something to the
		 * subscriber.
		 * or else the subscriber has an event handler callback which can be called to notify that something went wrong.
		 * @param[in] data: Data which needs to be published.
		 * @param[in] context_switch_req: If context_switch_req was set to TRUE then task that has a priority above the
		 * priority of the currently executing task was unblocked and a context switch should be performed to ensure
		 * the ISR returns to the unblocked task.
		 * @return Operation status

		 */
		dsi_pub_resp_t Pub_Int( dsi_data_t data, bool* context_switch_req );

		/**
		 * @brief Get number of subscribers of a dsid for which this publisher got constructed.
		 * @return Number of subscribers
		 */
		uint8_t Get_Num_Subscribers( void )
		{
			return ( DSI_Core::Sub_Count( m_dsid ) );
		}

	private:
		/**
		 * Since this class is pure static we disallow the Constructor, Destructor,
		 * Copy Constructor and Copy Assignment Operator definitions.
		 */
		DSI_Pub( const DSI_Pub& rhs );
		DSI_Pub & operator =( const DSI_Pub& object );

		dsi_id_t m_dsid;
};


#endif	// DSI_PUB_H
