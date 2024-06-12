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
#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "Modbus_Display.h"
#include "Modbus_Defines.h"
#include "Modbus_Serial.h"
#include "OS_Tasker.h"
#include "OS_Semaphore.h"
#include "DCI_Owner.h"
#include "Timers_Lib.h"

// lwIP includes.
#include "lwip\api.h"
#include "lwip\tcpip.h"
#include "lwip\memp.h"
#include "lwip\stats.h"
#include "lwip\tcp.h"
#include "lwip\Sys.h"

/*
 ********************************************************************************
 *		Constants
 ********************************************************************************
 */
// Deprecate this define and use the static constant
#define MODBUS_TCP_DEFAULT_PORT     Modbus_TCP::DEFAULT_PORT

/*
 *****************************************************************************************
 *		typedefs
 *****************************************************************************************
 */
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Modbus_TCP : public Modbus_Serial
{
	public:
		static const uint16_t DEFAULT_PORT = 502U;

		Modbus_TCP( BF_Mbus::Modbus_Display* modbus_display_ctrl = nullptr,
					uint16_t modbus_port = DEFAULT_PORT,
					DCI_Owner* session_timeout_owner = nullptr );

		virtual ~Modbus_TCP( void ) {}

		// "use_long_holdoff = true" means 1.75ms is used as the min msg holdoff.
		void Enable_Port( MB_RX_PROC_CBACK rx_cback,
						  MB_RX_PROC_CBACK_HANDLE rx_cback_handle,
						  tx_mode_t modbus_tx_mode, parity_t parity_select = MODBUS_PARITY_EVEN,
						  uint32_t baud_rate = 19200U, bool use_long_holdoff = true,
						  stop_bits_t stop_bits = MODBUS_1_STOP_BIT );

		void Disable_Port( void );

		bool Port_Enabled( void );

		bool Idle( void );

		void Send_Frame( MB_FRAME_STRUCT* tx_struct );

		/**
		 * @brief These events are passed back to the function which asked to be notified.
		 */
		enum event_t
		{
			CONNECTION_OPENED,
			CONNECTION_CLOSED
		};

		typedef void* mb_event_cback_info_t;	///< Will eventually support additional info.
		typedef void* mb_event_cback_handle_t;	///< Typically contains the this pointer.

		/**
		 * @brief Defines the event callback type.  The function that is called
		 * by the events listed above must meet the following type.
		 * @param handle: Typically the this pointer but can be any pointer.
		 * @param event: The enumeration listed above for what the event was.
		 * @param info: Potentially a pointer to a structure with event relevant information.
		 */
		typedef void (* mb_event_cback_t)( mb_event_cback_handle_t handle, event_t event,
										   mb_event_cback_info_t info );

		/**
		 * @brief Provides the method to attach an event callback to Modbus.  This will work per
		 * Modbus TCP instance.
		 * This callback functionality is provided to help adopters get a notification when a
		 * connection is opened or
		 * closed to track number of open connections and general Modbus TCP activity.  Typically
		 * the
		 * Com Dog is used to monitor that traffic is going but not necessarily the connections
		 * themselves..
		 * @param cback: The callback function which needs to be called on event.
		 * @param handle: Typically the this pointer to the class which has the callback.
		 */
		void Attach_Event_Callback( mb_event_cback_t cback, mb_event_cback_handle_t handle )
		{
			m_event_cback = cback;
			m_event_handle = handle;
		}

		/**
		 * @brief Defines the Modbus TCP header.
		 * These are private defines made public for an external LWIP interface.
		 */
		static const uint8_t MODBUS_TCP_MBAP_TRANSACTION_ID_HIGH = 0U;	//!<
		// MODBUS_TCP_MBAP_TRANSACTION_ID_HIGH
		static const uint8_t MODBUS_TCP_MBAP_TRANSACTION_ID_LOW = 1U;	//!<
		// MODBUS_TCP_MBAP_TRANSACTION_ID_LOW
		static const uint8_t MODBUS_TCP_MBAP_PROTOCOL_ID_HIGH = 2U;	//!<
		// MODBUS_TCP_MBAP_PROTOCOL_ID_HIGH
		static const uint8_t MODBUS_TCP_MBAP_PROTOCOL_ID_LOW = 3U;	//!<
		// MODBUS_TCP_MBAP_PROTOCOL_ID_LOW
		static const uint8_t MODBUS_TCP_MBAP_LENGTH_HIGH = 4U;	//!< MODBUS_TCP_MBAP_LENGTH_HIGH
		static const uint8_t MODBUS_TCP_MBAP_LENGTH_LOW = 5U;	//!< MODBUS_TCP_MBAP_LENGTH_LOW
		static const uint8_t MODBUS_TCP_MBAP_UNIT_ID = 6U;	//!< MODBUS_TCP_MBAP_UNIT_ID
		static const uint8_t MODBUS_TCP_MBAP_HEADER_LENGTH = 7U;//!< MODBUS_TCP_MBAP_HEADER_LENGTH

		/// Defines the maximum Modbus Serial Frame outside of the TCP header.
		static const uint16_t MAX_MODBUS_SERIAL_PDU = 256U;	// or should it be 253?

		/**
		 * @brief The mb_app_struct is used to define the buffer for a Modbus TCP message.
		 *
		 */
		struct mb_app_struct
		{
			uint16_t rx_buff_index;			///< Index into the buffer.
			uint8_t rx_buff[MAX_MODBUS_SERIAL_PDU + MODBUS_TCP_MBAP_HEADER_LENGTH];
			uint8_t timeout_num;	///< This indicates which timeout slot we have.
			Modbus_TCP* handle;
		};

	private:
		/**
		 * @brief Private Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Modbus_TCP( const Modbus_TCP& object );
		Modbus_TCP & operator =( const Modbus_TCP& object );


		/**
		 * @Brief This is the amount of time between checks of the Modbus TCP connections
		 * for timeout in this way we can make sure that the check is not happening too
		 * often.  Especially since it is dependent on a millisecond tick.  At the
		 * fastest we could go is using a 1ms check time.  Frankly when combined with
		 * latency on the wire and intermessage time any sample time less than 100ms is
		 * probably too fast.  We used a prime number because it is good to break these
		 * things up so that they do not get on a tick.
		 */
		static const BF_Lib::Timers::TIMERS_TIME_TD CONNECTION_TIMEOUT_CHECK_TIME = 11U;

		// Structure added to implement Modbus TCP socket cleanup on timeout on 19072016- Shekhar
		struct mb_timeout_struct_t
		{
			struct tcp_pcb* pcb;
			BF_Lib::Timers::TIMERS_TIME_TD time_sample;	///< Time sample to check against.
			struct mb_app_struct* pcb_mb_app;
		};

		bool TCP_Init( void );

		bool PCB_Init( struct tcp_pcb** pcb, uint16_t port );

		err_t TCP_Accept( void* arg, struct tcp_pcb* pcb, err_t error );

		static err_t TCP_Accept_Static( void* arg, struct tcp_pcb* pcb, err_t error )
		{
			return ( ( reinterpret_cast<Modbus_TCP*>( arg ) )->TCP_Accept( arg, pcb, error ) );
		}

		void TCP_Error( void* arg, err_t xErr );

		static void TCP_Error_Static( void* arg, err_t error )
		{
			( reinterpret_cast<Modbus_TCP*>( ( static_cast<struct mb_app_struct*>( arg ) )->handle ) )->TCP_Error( arg,
																												   error );
		}

		err_t TCP_Receive( void* arg, struct tcp_pcb* pcb, struct pbuf* p_buffer, err_t error );

		static err_t TCP_Receive_Static( void* arg, struct tcp_pcb* pcb, struct pbuf* p_buffer,
										 err_t error )
		{
			return ( ( reinterpret_cast<Modbus_TCP*>( ( static_cast<struct mb_app_struct*>( arg ) )->handle ) )->
					 TCP_Receive( arg, pcb, p_buffer, error ) );
		}

		bool MBTCP_Send( struct mb_app_struct* pcb_mb_app, uint8_t* mbus_frame, uint16_t frame_length );

		void Release_Client( struct mb_app_struct* pcb_mb_app, struct tcp_pcb* pcb );

		// Functions added to implement Modbus TCP socket cleanup on timeout on 19072016- Shekhar
		// Resets the timeout for the TCP connection for a session when there is activity
		void Session_Timeout_Checker( void );

		static void Session_Timeout_Checker_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
		{ ( reinterpret_cast<Modbus_TCP*>( param ) )->Session_Timeout_Checker(); }

		void Reset_Session_Timeout( struct mb_app_struct* pcb_mb_app, struct tcp_pcb* pcb );

		bool m_port_enabled;

		uint16_t m_modbus_port;

		MB_FRAME_STRUCT m_tx_struct;
		uint8_t m_tx_state;

		BF_Mbus::Modbus_Display* m_modbus_display;

		MB_RX_PROC_CBACK m_rx_cback;
		MB_RX_PROC_CBACK_HANDLE m_rx_cback_handle;

		mb_event_cback_t m_event_cback;
		mb_event_cback_handle_t m_event_handle;

		OS_Queue* m_event_que;

		bool m_msg_ready_to_tx;

		struct tcp_pcb* m_pcb_listen1;
		// support an optional port in addition to 502
		struct tcp_pcb* m_pcb_listen2;

		struct tcp_pcb* m_curr_pcb_client;

		uint8_t m_tx_string[MAX_MODBUS_SERIAL_PDU + MODBUS_TCP_MBAP_HEADER_LENGTH];

		// Members added to implement Modbus TCP socket cleanup on timeout on 19072016- Shekhar
		// Array of structure to hold active TCP connection (Max 5 as of today)
		mb_timeout_struct_t m_session_timeout_list[MEMP_NUM_TCP_PCB];
		DCI_Owner* m_session_timeout_owner;

		// #define IS_TICK_GREATER( tick1, tick2 )  ( ( tick1 < 0xffff && tick2 > 0xffff0000 ) ?
		// TRUE :
		// ( ( tick2 < 0xffff && tick1 > 0xffff0000 ) ?
		// FALSE :
		// ( tick1 > tick2 ) ) )
		inline static bool Is_Tick_Greater( uint32_t tick1, uint32_t tick2 )
		{
			bool greater;

			if ( ( tick1 < 0xffff ) && ( tick2 > 0xffff0000 ) )
			{
				greater = true;
			}
			else if ( ( tick2 < 0xffff ) && ( tick1 > 0xffff0000 ) )
			{
				greater = false;
			}
			else
			{
				greater = ( tick1 > tick2 );
			}

			return ( greater );
		}

		static const BF_Lib::Timers::TIMERS_TIME_TD SESSION_TIMEOUT_DEFAULT = 600000U;		// 10 Minutes

		/**
		 * @brief All PCBs are set with default TCP_PRIO_NORMAL priority unless set explicitly.
		 * MODBUS_TCP_PCB_PRIO gives existing modbus tcp connection priority over any
		 * new requests coming in for example http/s. In this case modbus tcp conn. will survive.
		 */
		// static const uint8_t MODBUS_TCP_PCB_PRIO = TCP_PRIO_NORMAL + 1U; //ltk-3362
};

#endif
