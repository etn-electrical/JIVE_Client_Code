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
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DELTA_SERVICE_H
#define DELTA_SERVICE_H

#include "DCI_Owner.h"
#include "Services.h"

/**
 ****************************************************************************************
 * @brief Delta Service provides a method to send and receive services between two points using
 * a common list of services across the devices.
 *
 * @details Multiple components will communicate to each other and must share services back and
 * forth.  These services must be acknowledged to verify that the two components have processed
 * and/or executed the service.  Some examples follow.
 *
    \msc
     arcgradient = 8;

     a [label="Led Test"],b [label="Services"],
     c [label="Delta Services Param"], d [label="Delta Services Param"];

     a=>b [label="Send Data"];
     a-xb [label="data2"];
     a=>b [label="data3"];
     a<=b [label="ack1, nack2"];
     a=>b [label="data2", arcskip="1"];
 |||;
     a<=b [label="ack3"];
 |||;
    \endmsc
 *
 ****************************************************************************************
 */
class Delta_Service
{
	public:
		struct service_def_t
		{
			DCI_ID_TD service_tx_dcid;
			DCI_ID_TD service_rx_dcid;

			uint8_t total_services;
			uint8_t* service_id_list;
			uint8_t* shared_id_list;
		};

		Delta_Service( service_def_t const* service_def );
		~Delta_Service( void );

		void Send_Service( uint8_t service );

		bool Idle( void );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		Delta_Service( const Delta_Service& rhs );
		Delta_Service & operator =( const Delta_Service& object );

		DCI_CB_RET_TD RX_Change( DCI_ACCESS_ST_TD* access_struct );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static DCI_CB_RET_TD RX_Change_Static( DCI_CBACK_PARAM_TD param,
											   DCI_ACCESS_ST_TD* access_struct )
		{ return ( reinterpret_cast<Delta_Service*>( param )->RX_Change( access_struct ) ); }

		uint8_t Convert_DS_To_Service( uint8_t delta_service );

		uint8_t Convert_Service_To_DS( uint8_t service );

		DCI_Owner* m_tx;
		DCI_Owner* m_rx;
};

#endif
