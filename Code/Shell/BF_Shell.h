/**
 *****************************************************************************************
 * @file
 *
 * @brief	Class declaration for Shell::BF_Shell::
 *
 * @details BF_Shell:: is derived from Shell_Base:: for the purpose of specializing to the
 * 			Babelfish environment. It makes use of Babelfish co-routines and USART.
 *
 * @copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef BF_Shell_H_
#define BF_Shell_H_

#include "CR_Queue.h"
#include "CR_Tasker.h"
#include "uC_USART_DMA.h"
#include "uC_USART_Buff.h"
#include "uC_USART_HW.h"

namespace Shell
{

/**
 ****************************************************************************************
 * @brief Shell for BabelFish testing.
 *
 * @details BF_Shell:: is derived from Shell_Base:: for the purpose of specializing to the
 * 			Babelfish environment. It makes use of Babelfish co-routines and USART.
 *
 ****************************************************************************************
 */

class BF_Shell : public Shell_Base
{
	public:
		/**
		 * @brief Constructs an instance of the class.
		 * @param shell_config_ptr: Configuration structure.
		 * @param cle: CLE command line editor (use nullptr, provided only for unit testability).
		 * @param cli: CLI command line interpreter (use nullptr, provided only for unit testability).
		 */
		BF_Shell( uC_USART_Buff* usart_ptr, shell_config_t const* shell_config_ptr, Command_Line_Editor* cle = nullptr,
				  Command_Line_Interpreter* cli = nullptr );

		/**
		 * @brief Destroys an instance of the class.
		 */
		virtual ~BF_Shell();

		/**
		 * @brief Puts a character out to the host environment.
		 * @note This is a blocking call.
		 * @param out_char: The given character.
		 */
		void Put_Char( shell_char_t out_char );

		/**
		 * @brief Puts a string out to the host environment.
		 * @note This is a blocking call.
		 * @param out_string: The given string.
		 */
		void Put_String( shell_char_t const* out_string );

		/**
		 * @brief Puts a string out to the host environment.
		 * @param out_string: The given string.
		 * @param bytes_to_send: Number of bytes to transmit
		 */
		void Put_String( shell_char_t const* out_string, uint32_t bytes_to_send );

		/**
		 * @brief Enters critical section.
		 */
		void Enter_Critical( void );

		/**
		 * @brief Exits critical section.
		 */
		void Exit_Critical( void );

		/**
		 * @brief Gets a new command buffer.
		 * @return Pointer to new command buffer.
		 */
		shell_char_t* Get_New_Command_Buffer( void );

		/**
		 * @brief Releases command buffer.
		 * @return Pointer to new command buffer.
		 */
		void Release_Command_Buffer( shell_char_t* buffer );

		/**
		 * @brief Gets currently executing test.
		 * @return  Currently executing test, or nullptr if none.
		 */
		Test_Base* Get_Current_Test( void );

		/**
		 * @brief Sets currently executing test.
		 * @param test: Currently executing test.
		 */
		void Set_Current_Test( Test_Base* test );

		/**
		 * @brief Clears currently executing test pointer.
		 */
		void Clear_Current_Test( void );

		/**
		 * @brief Get status of currently executing test.
		 */
		test_status_t Get_Test_Status( void );

		void Set_Tested_Function_Name( const shell_char_t* name );

		void Set_Test_Criteria( const shell_char_t* name );

		shell_char_t* Get_Tested_Function_Name();

		shell_char_t* Get_Test_Criteria();

		/*
		 * @brief: Function to flush shell buffer data directly
		 * @details: BF shell uses CR tasker to flush all data on serial terminal
		 * 			 This function will be used to transmit data on serial independent of
		 * 			 CR tasker. Hence the usage of this function should me minimum
		 * 			 and only on places where it is required
		 */
		void Direct_Transmit_Buffer_Flush( void );

		/*
		 * @brief: Function to flush shell buffer data directly
		 * @details: BF shell uses CR tasker to flush all data on serial terminal
		 * 			 This function will be used to transmit data on serial independent of
		 * 			 CR tasker. Hence the usage of this function should me minimum
		 * 			 and only on places where it is required
		 */
		void Put_String_Now( shell_char_t const* out_string, uint32_t bytes_to_send );

	private:
		/** Allocated size of data array in BF_Shell::m_rx_buffer{} and BF_Shell::event_t.m_data[]
		 */
		static const uint32_t RX_CHAR_MAX = 64U;

		/** Allocated size of data array in BF_Shell::m_tx_buffer[] */
		static const uint32_t TX_CHAR_MAX = 1024U;

		/** Allocated length of queue. */
		static const uint16_t QUEUE_LENGTH = 8U;

		/** Receive timeout, microseconds. */
		static const uint32_t RX_TIMEOUT = 10000U;

		/** Transmit timeout, microseconds. */
		static const uint32_t TX_TIMEOUT = 10000U;

		/**
		 * @brief List of event types.
		 */
		enum event_type_t
		{
			RECIEVE_DATA,
			TRANSMIT_COMPLETE,
			ERROR
		};

		/**
		 * @brief Structure for shell event queue items.
		 */
		struct event_t
		{
			/**  Event type. */
			event_type_t m_type;

			// ?TODO event status?

			/** event data */
			shell_char_t m_data[BF_Shell::RX_CHAR_MAX];

			/** event data size */
			uint16_t m_size;
		};


		/**
		 * @brief Pointer to event queue.
		 */
		CR_Queue* m_queue_ptr;

		/**
		 * @brief Pointer to our USART.
		 */
		uC_USART_Buff* m_usart_ptr;

		/**
		 * @brief Pointer to our coroutine task.
		 */
		CR_Tasker* m_task_ptr;

		/**
		 * @brief Receive character buffer.
		 */
		uint8_t m_rx_buffer[RX_CHAR_MAX];

		/**
		 * @brief Receive character buffer.
		 */
		shell_char_t* m_cmd_buffer_ptr;

		/**
		 * @brief Transmit character buffer.
		 */
		uint8_t m_tx_buffer[TX_CHAR_MAX];

		/**
		 * @brief Head pointer within transmit buffer.
		 */
		uint8_t* m_tx_head_ptr;

		/**
		 * @brief Tail pointer within transmit buffer.
		 */
		uint8_t* m_tx_tail_ptr;

		/**
		 * @brief Next tail pointer within transmit buffer.
		 */
		uint8_t* m_tx_next_tail_ptr;

		/**
		 * @brief Pointer to end of transmit buffer.
		 */
		uint8_t* m_tx_end_ptr;

		/**
		 * @brief Flag to indicate transmitter is busy.
		 */
		bool_t m_tx_busy;

		/**
		 * @brief Pointer to currently executing test (or nullptr if none).
		 */
		Test_Base* m_current_test;

		/**
		 * @brief Dispatches events from USART callback.
		 */
		void Event_Dispatcher( event_t* event_ptr );

		/**
		 * @brief Handles USART receive events.
		 */
		void Receive_Handler( event_t* event_ptr );

		/**
		 * @brief Handles USART transmit complete events.
		 */
		void Transmit_Handler( event_t* event_ptr );

		/**
		 * @brief Handles USART error events.
		 */
		void Error_Handler( event_t* event_ptr ) const;

		/**
		 * @brief Initializes transmit buffer pointers.
		 */
		void Transmit_Buffer_Initialize( void );

		/**
		 * @brief Puts a character into the transmit buffer.
		 */
		void Transmit_Buffer_Put_Char( uint8_t out_char );

		/**
		 * @brief Flushes the transmit buffer.
		 */
		void Transmit_Buffer_Flush( void );

		/**
		 * @brief		Coroutine callback function.
		 */
		void Coroutine_Callback( void );

		/**
		 * @brief 		Static coroutine callback function.
		 *
		 * @details 	This is called by the OS.
		 */
		static void Coroutine_Callback_Static( CR_Tasker* tasker, CR_TASKER_PARAM param )
		{ reinterpret_cast<BF_Shell*>( param )->Coroutine_Callback(); }

		/**
		 * @brief		Callback function for UART.
		 */
		void UART_Callback( uC_USART_Buff::cback_status_t status_bits );

		/**
		 * @brief 		Static callback function for UART.
		 *
		 * @details 	This is called back by the UART driver upon specified events.
		 */
		static void UART_Callback_Static( uC_USART_Buff::cback_param_t param,
										  uC_USART_Buff::cback_status_t status_bits )
		{ reinterpret_cast<BF_Shell*>( param )->UART_Callback( status_bits ); }

		/**
		 * @brief Internal initialization.
		 */
		void Internal_Initialize( void );

		/**
		 * @brief Privatized Copy Constructor.
		 */
		BF_Shell( const BF_Shell& object );

		/**
		 * @brief Privatized Assignment Operator.
		 */
		BF_Shell & operator =( const BF_Shell& object );

		/**
		 * variable for storing the class and function tested in the test case
		 * gives more information in the automated Test reports
		 */
		shell_char_t* m_function_tested;

		/**
		 * variable for storing the test criteria based on which the test is marked as failed
		 * gives more information in the automated Test reports
		 */
		shell_char_t* m_test_criteria;

};

}	// namespace

#endif	/* BF_Shell_H_ */
