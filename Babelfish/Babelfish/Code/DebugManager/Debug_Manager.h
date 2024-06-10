/**
 **********************************************************************************************
 * @file            Debug Manager class deceleration for debug message communication
 *
 * @brief           This file contains Debug Manager class implementation
 *
 * @details			This class will be used to transmit debug messages on selected communication
 * 					interface.
 *
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 *
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include "Includes.h"
#include "CR_Tasker.h"
#include "Debug_Interface.h"
#include "DCI_Patron.h"

/**
 **************************************************************************************************
 * @brief  : This class is responsible for printing debug log messages on any selected device
 * interface
 * @details: Data will be first added in circular buffer and later using CR Task data is pulled out
 * from
 * 			 out from circular buffer and is written on selected debug print interface
 **************************************************************************************************
 */
class Fault_Catcher;
class Debug_Manager
{
	public:
		/**
		 * @brief  : Constructor to create an instance of Debug_Manager.
		 * @details: This constructor initializes debug manager related parameters.
		 * @param1 [in] dm_base_ptr : Pointer to Debug Manager interface. This pointer will be used
		 * to print debug messages
		 * @param2 [in] task_priority: Debug Manager task priority (0 - 6). This is user
		 * configurable.
		 * @param3 [in] time_slice: Debug Manager task time slice (periodic time). This is user
		 * configurable.
		 * @param4 [in] dbg_msg_buf_size: debug message buffer size
		 * debug configuration
		 * 						file Babelfish_Debug.h
		 * @param5 [in] fault_catch_handle: fault catch handle to call the hard fault print job
		 * 				Actual hardfault messages are getting filled and printed in Fault_Catcher file
		 * @return this.
		 */
		Debug_Manager( Debug_Interface* dm_base_ptr,
					   uint8_t task_priority,
					   uint16_t time_slice,
					   uint16_t dbg_msg_buf_size,
					   Fault_Catcher* fault_catch_handle = nullptr );
		/**
		 * @brief  : Destructor to delete an instance of Debug_Manager.
		 * @details: This Destructor will delete debug manager related parameters.
		 * @return this.
		 */
		~Debug_Manager();

		/**
		 * @brief  : Function add debug message in circular buffer
		 * @details: This buffer is a circular buffer
		 * @param1 [in] enable_mask : Mask value to verify Debug On/OFF status, MessageType to print
		 * and time prefix status
		 * @param2 [in] msg_type: Type of message received
		 * @param3 [in] func_name: Function name from where debug message is received
		 * @param4 [in] format: Format for Variadic macro
		 * @return void.
		 */
		static void Debug_Msg_Print( uint16_t enable_mask,
									 uint16_t msg_type,
									 const char_t* func_name,
									 const char_t* format, ... );

		/**
		 * @brief  : Function to print assert message
		 * @details:
		 * @param1 [in] line_num : Line number from where assert is triggered
		 * @param2 [in] func_name: Function name from where assert is triggered
		 * @return void.
		 */
		static void Debug_Assert( const uint32_t line_num,
								  const char_t* func_name );

	private:
		struct dbg_msg_type_info_t
		{
			char_t* msg_code;
			char_t* msg_type_name;
			uint8_t length;
		};

		/**
		 * @brief   : Constant for limiting maximum length for a single debug message packet.
		 * @details : This constant has been used to limit the byte usage for creating a complete
		 * 			  debug message packet.
		 */
		static const uint16_t DBG_MSG_LEN = 256U;

		/**
		 * @brief   : Constant map for holding message type strings and there codes.
		 * @details : This structure constant has been used as a lookup table for adding message
		 * 			  type string / message type code in debug message packet.
		 */
		static const dbg_msg_type_info_t DBG_MSG_TYPE_INFO[];

		/**
		 * @brief   : Constant for limiting maximum length for Diagnostics buffer.
		 * @details : This constant buffer length has been used to limit the message size used for
		 * 			  printing diagnostics information of Debug Manager
		 */
		static const uint8_t DIAG_BUF_LEN = 5U;

		/**
		 * @brief   : Constant for limiting maximum length for Diagnostics buffer.
		 * @details : This constant buffer length has been used to limit the message size used for
		 * 			  printing diagnostics information of Debug Manager
		 */
		static const uint8_t DBG_BUF_USAGE_PERCENT_THRESHOLD = 70U;

		/*
		 * @brief: Variables used to store const strings used for printing Debug Manager diagnostics
		 * @details: These are fixed strings used for printing diagnostics information
		 */
		static const char DIAG_STR_BUF_USAGE[];
		static const char DIAG_STR_MISSED_PACKET_COUNT[];
		static const char DIAG_STR_MULTI_THREAD_MISSED_COUNT[];

		/**
		 * @brief   : Static buffer to store one debug message packet
		 * @details : This buffer is used to create and store debug message packet till message
		 * 			  is copied in circular buffer. This buffer has been limited to 256 bytes to
		 * 			  limit the debug message length.
		 */
		static uint8_t* m_raw_msg;

		/**
		 * @brief   : Flag to protect circular buffer usage from single multi thread operation
		 */
		static bool m_protect_buf_flag;

		/**
		 * @brief   : Pointer to circular buffer and its associated variables
		 * @details : This is pointer to circular buffer that is used to store multiple debug
		 *			  message packets. Message packet will be added in this buffer using head
		 *			  index and will be read by Debug Manager task using tail index.
		 */
		static uint8_t* m_buffer_start;
		static uint8_t* m_buffer_end;
		static uint8_t* m_enque;
		static uint8_t* m_deque;
		static uint16_t m_msg_buf_max_length;
		static uint_fast16_t m_msg_buf_free_size;
		static uint16_t m_msg_buf_free_size_water_mark;
		static uint_fast32_t m_missed_packet_counter;
		static uint_fast32_t m_missed_multi_thread_packet;

		/*
		 * @brief: Debug Interface static pointer
		 * @details: This pointer is used to access debug interface to print assert message directly
		 * on
		 * 		   	 selected communication interface from assert static function
		 */
		static Debug_Interface* m_dbg_inter_assert;

		/**
		 * @brief   : Pointer to circular buffer
		 * @details : This is pointer to circular buffer that is used to store multiple debug
		 * message packets.
		 * 			  Message packet will be added in this buffer using head index and will be read
		 * by Debug Manager
		 * 			  task using tail index.
		 */
		char m_diag_buf[DIAG_BUF_LEN];

		/*
		 * @brief   : Variables to hold circular buffer threshold size
		 * @details : This variable will hold the minimum threshold value for free bytes in circular
		 * buffer
		 */
		uint16_t m_msg_buf_threshold_size;

		/*
		 * @brief   : Variables to hold previous circular buffer water mark
		 * @details : This variable is used to hold the previous circular buffer water mark value
		 *			  This value will be compared with current buffer free size to calculate buffer
		 * usage percentage
		 */
		uint16_t m_previous_buf_water_mark;

		/**
		 * @brief   : Variable to store Debug Manager time slice
		 * @details : This variable will be updated in Debug Manager constructor and is user
		 * configurable.
		 */
		uint32_t m_time_slice;

		/**
		 * @brief   : Pointer to Debug Manager base interface class
		 * @details : This pointer is used to print debug messages on interface.
		 */
		Debug_Interface* m_dbg_if_ptr;

		/*
		 * @brief: Fault Catch handle
		 * @details: This pointer is used to call the fault catch to print fault catch data
		 * on selected communication interface from fault catch print function
		 */
		Fault_Catcher* m_fault_catch_handle;

		/*
		 * @brief   : Pointer to DCI Patron
		 * @details : This pointer is used to get the hard fault data from DCI variables
		 */
		DCI_Patron* m_patron;

		/**
		 * @brief DCI Patron Source ID
		 */
		DCI_SOURCE_ID_TD m_source_id;

		/**
		 * @brief  : Task to print debug message on UART interface periodically
		 * @details: This function will read formated debug message from circular buffer and will
		 * print the same on UART interface.
		 * @param1 [out] cr_task : Pointer to CR Task
		 * @return void.
		 */
		void Print_Debug_Message_Handler( CR_Tasker* cr_task );

		/**
		 * @brief  : Task function handler.
		 * @details: This handler function will call task function.
		 * @param1 [in] cr_task: Pointer to CR Task.
		 * @param2 [in] handle: Handle to call task handler function.
		 * @return void.
		 */
		static void PrintDebugMessage_Handler_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			( ( Debug_Manager* )handle )->Print_Debug_Message_Handler( cr_task );
		}

		/**
		 * @brief  : Function to print debug message on interface selected in Babelfish_Debug.h.
		 * @details: This function will be used to create a Debug message packet and storing the
		 * same in circular buffer for further printing on interface.
		 * @param1 [in] msg_buf : Pointer to circular message buffer.
		 * @return void.
		 */
		void Print_Debug_Message( const char* msg_buf );

		/**
		 * @brief  : Static function to add formated debug message in circular buffer.
		 * @details: This function will be used add formated debug message in circular buffer using
		 *			 head index.
		 * @param1 [in] src_buf : Pointer to source formated debug message packet.
		 * @param2 [in] data_length: Number of bytes to be added in circular buffer.
		 * @return void.
		 */
		static void Enque( uint8_t* src_buf, uint16_t data_length );

		/**
		 * @brief  : Function to read data from circular buffer.
		 * @details: This function will read all data newly added in circular buffer and will
		 * process
		 * 			 the same for printing on selected interface.
		 * @return void.
		 */
		void Deque( void );

		/**
		 * @brief  : Function to find the percent usage of debug manager circular buffer.
		 * @details: This function will print the percent usage of buffer once it crosses its
		 *			 threshold and missed packet counts.
		 * @return void.
		 */
		void Debug_Manager_Diagnostics( void );

		/*
		 * @brief  : Function to print Hard fault capture data.
		 * @details: This function get hard fault data from DCI variables and will print
		 * 			 the same on Debug interface.
		 * @return void.
		 */
		void Print_Hard_Fault_Data( void );

		/*
		 * @brief  : Function to get hard fault data from DCI variables
		 * @details: This function will get the DCID value and fill the same in data pointer
		 * @param1 [in] dcid : DCID value
		 * @param2 [in] data : pointer to data buffer
		 */
		DCI_ERROR_TD Get_DCID_Value( DCI_ID_TD dcid, uint8_t* data );

};

/**
 * @brief  : Non-class function which will be redirected to Debug_Manager::Debug_Assert() to print assert message
 * @details: Need this (eg, for FreeRTOS configAssert) where its too troublesome to include the full Debug_Manager
 * class.
 * @param line_num : Line number from where assert is triggered
 * @param func_name: Function name from where assert is triggered
 */
#ifdef __cplusplus
extern "C" {
#endif
void Debug_Manager_Assert( unsigned int line_num, const char* func_name );

#ifdef __cplusplus
}
#endif

#endif	// #ifndef DEBUG_MANAGER_H
