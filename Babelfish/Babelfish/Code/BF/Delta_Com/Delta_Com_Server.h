/**
 *****************************************************************************************
 *	@Delta_Com.h
 *
 *	@brief This class will be used to communicate changes to the DCI based on a
 *	parameter ID
 *
 *	@details By monitoring the DCI and allowing for users to set which values are of
 *	import, this class will send and receive those values which have changed as well
 *	as any other values that are requested.
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DELTA_COM_SERVER_H_
#define DELTA_COM_SERVER_H_

#include "Includes.h"
#include "DCI.h"
#include "Delta_Com.h"
#include "Delta_Com_Defines.h"
#include "Timers_uSec.h"
#include "Timers_Lib.h"
#include "Delta_Com_Display.h"
#include "DCI_Worker.h"
#include "DCI_Workers.h"
#include "Delta_Data.h"
#include "Delta_Com_Identity.h"

/**
 ****************************************************************************************
 * @brief This Class includes all methods and variables required for the server
 * implementation of the Delta Com.
 *
 ****************************************************************************************
 */
class Delta_Com_Server
{
	public:
		enum server_state_t
		{
			NO_COMMUNICATION,
			COMMUNICATION_LOSS,
			INITIALIZING,
			INITIALIZED
		};

		typedef server_state_t cback_status_t;
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param, server_state_t state );
		typedef dcom_identity_error (* cback_client_validate_func_t)( cback_param_t param,
																	  delta_com_identity_t* identity );

		/**
		 *  @brief Constructs a communication specific Delta Com.
		 *  @param delta_com_def: The server definition block.  This contains all the mapping
		 *  between the MPL and DCIDs.
		 *  @param display: The delta com display handler.  If null then the device will map the
		 *  display to a null display.
		 *  @param callback: The status callback for when the delta com server changes states
		 *  (eg: online, offline, communication loss timeout).
		 *  @param parameter: Parameter for the callback.
		 *  @param identity_def: An identity definition control.
		 *
		 *  @return this.
		 */
		Delta_Com_Server( Delta_Data::data_def_t const* delta_com_def,
						  delta_com_identity_source_t const* identity_def = nullptr,
						  Delta_Com_Display* display = nullptr,
						  cback_func_t state_change_callback = nullptr,
						  cback_client_validate_func_t client_validate_callback = nullptr,
						  cback_param_t parameter = nullptr );

		/**
		 *  @brief Destructor
		 *  @note currently unused due to non-dynamic memory allocation
		 */
		~Delta_Com_Server( void );

		bool Process_Delta_Com_Message( uint8_t* rx_data, uint16_t rx_data_len,
										uint8_t* tx_data, uint16_t* tx_data_len );

		void Set_Maximum_Message_Size( uint16_t max_size );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Delta_Com_Server( const Delta_Com_Server& object );
		Delta_Com_Server & operator =( const Delta_Com_Server& object );

		static DCI_ACCESS_CMD_TD Translate_Attribute_To_DCI_Get_Command( delta_com_attrib_t attribute );

		static DCI_ACCESS_CMD_TD Translate_Attribute_To_DCI_Set_Command( delta_com_attrib_t attribute );

		static const BF_Lib::Timers::TIMERS_TIME_TD DEFAULT_COMM_TIMEOUT = 300U;

		void Communication_Loss_Handler( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Comm_Loss_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{ reinterpret_cast<Delta_Com_Server*>( handle )->Communication_Loss_Handler(); }

		delta_com_error_t Exchange_Delta_Parameters( uint8_t* rx_data, uint16_t rx_data_len,
													 uint8_t* tx_data, uint16_t* tx_data_len );

		delta_com_error_t Manage_Subscriptions( uint8_t* rx_data, uint16_t rx_data_len,
												uint8_t* tx_data, uint16_t* tx_data_len );

		delta_com_error_t Exchange_Identity( uint8_t* rx_data, uint16_t rx_data_len,
											 uint8_t* tx_data, uint16_t* tx_data_len );

		void Set_Error_Code( uint8_t* output_frame, uint16_t* frame_length,
							 delta_com_error_t error_code );

		void Initialize_Sub_Pub( void );

		static const uint32_t MAX_TIMEOUT = 0x0000FFFFU;
		static const BF_Lib::Timers::TIMERS_TIME_TD COMMUNICATION_TIMEOUT_POLL_MULTIPLIER = 30U;

		server_state_t m_state;
		Delta_Com_Display* m_display;
		BF_Lib::Timers* m_timer;
		uint16_t m_max_message_size;

		uint8_t m_deque_key;
		uint8_t m_prev_deque_key;


		cback_func_t m_state_change_cback;
		cback_client_validate_func_t m_client_validate_cback;
		cback_param_t m_cback_param;

		BF_Lib::Timers::TIMERS_TIME_TD m_communication_timeout;

		Delta_Data* m_delta_data;

		delta_com_identity_source_t const* m_identity_def;
};

#endif
