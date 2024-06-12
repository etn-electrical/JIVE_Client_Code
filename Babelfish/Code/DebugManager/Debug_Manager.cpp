/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <string>
#include <stdarg.h>
#include "Fault_Catcher.h"
#include "Babelfish_Assert.h"
#include "Debug_Manager.h"
#include "Ram.h"
#ifndef ESP32_SETUP
#include "Timers_uSec.h"
#endif
#include "Debug_Manager_Config.h"

/*
 * Static const variables initialization for circular buffer management
 */
bool Debug_Manager::m_protect_buf_flag = false;
uint8_t* Debug_Manager::m_raw_msg = {0U};
uint8_t* Debug_Manager::m_buffer_start = nullptr;
uint8_t* Debug_Manager::m_buffer_end = nullptr;
uint8_t* Debug_Manager::m_enque = nullptr;
uint8_t* Debug_Manager::m_deque = nullptr;

uint16_t Debug_Manager::m_msg_buf_max_length = 0U;
uint_fast16_t Debug_Manager::m_msg_buf_free_size = 0U;
uint_fast32_t Debug_Manager::m_missed_packet_counter = 0U;
uint_fast32_t Debug_Manager::m_missed_multi_thread_packet = 0U;
uint16_t Debug_Manager::m_msg_buf_free_size_water_mark = 0U;

/*
 * Static variable to hold debug interface pointer used to print assert debug message
 */
Debug_Interface* Debug_Manager::m_dbg_inter_assert = nullptr;

/*
 * Static const buffers initialization
 */
const char_t Debug_Manager::DIAG_STR_BUF_USAGE[] = {"\nDebug_Manager: Debug buffer usage percent: "};
const char_t Debug_Manager::DIAG_STR_MISSED_PACKET_COUNT[] = {"\nDebug_Manager: Debug buffer full...missed count: "};
const char_t Debug_Manager::DIAG_STR_MULTI_THREAD_MISSED_COUNT[] =
{"\n Debug_Manager: Missed Packet due to multi task access count: "};

/*
 * Static const structure initialization
 */
const uint8_t msg_type_index[17] = {0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4};
const Debug_Manager::dbg_msg_type_info_t Debug_Manager::DBG_MSG_TYPE_INFO[5] =
{
	{( char_t* )" 4 ", ( char_t* )" DBG ",   sizeof( " DBG " )          },
	{( char_t* )" 3 ", ( char_t* )" INFO ",  sizeof( " INFO " )         },
	{( char_t* )" 2 ", ( char_t* )" CRIT ",  sizeof( " CRIT " )         },
	{( char_t* )" 1 ", ( char_t* )" ERR ",   sizeof( " ERR " )          },
	{( char_t* )" 0 ", ( char_t* )" EMERG ", sizeof( " EMERG" )         },
};

/*
 *  Constant string used to print Assert debug message
 */
const char* assert_str = {"\n!!!! ASSERT triggered at "};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Debug_Manager::Debug_Manager( Debug_Interface* dm_base_ptr,
							  uint8_t task_priority,
							  uint16_t time_slice,
							  uint16_t dbg_msg_buf_size,
							  Fault_Catcher* fault_catch_handle ) :
	m_diag_buf{ 0 },
	m_msg_buf_threshold_size( 0U ),
	m_time_slice( time_slice ),
	m_dbg_if_ptr( dm_base_ptr ),
	m_fault_catch_handle( fault_catch_handle )
{
	m_patron = new DCI_Patron( true );
	m_source_id = DCI_SOURCE_IDS_Get();
	BF_ASSERT( m_patron );

	// Assign  interface pointer to a static variable
	m_dbg_inter_assert = dm_base_ptr;

	// Allocate RAM to debug message circular buffer
	m_buffer_start = reinterpret_cast<uint8_t*>( Ram::Allocate( dbg_msg_buf_size ) );
	BF_ASSERT( m_buffer_start );

	// Allocate RAM to store debug message packet at the time of packet creation
	m_raw_msg = reinterpret_cast<uint8_t*>( Ram::Allocate( DBG_MSG_LEN ) );
	BF_ASSERT( m_raw_msg );

	// Initializing buffer management variables
	m_buffer_end = m_buffer_start + dbg_msg_buf_size;
	m_enque = m_buffer_start;
	m_deque = m_buffer_start;

	m_msg_buf_free_size = dbg_msg_buf_size;
	m_msg_buf_max_length = dbg_msg_buf_size;
	m_msg_buf_free_size_water_mark = dbg_msg_buf_size;

	// Calculate threshold size for circular buffer
	// Circular buffer threshold is 70% of the buffer max length, calculating threshold size
	// If buffer free size reduces below threshold size, buffer usage will be printed
	m_msg_buf_threshold_size = ( m_msg_buf_max_length * ( 100U - DBG_BUF_USAGE_PERCENT_THRESHOLD ) ) / 100U;

	// Initialize previous buffer water mark with threshold
	m_previous_buf_water_mark = m_msg_buf_threshold_size;

	// Create CR task
	// coverity[leaked_storage]
	new CR_Tasker( PrintDebugMessage_Handler_Static,
				   ( CR_TASKER_PARAM )this,
				   task_priority,
				   "Debug Print Task" );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Debug_Manager::~Debug_Manager()
{
	// RAM deallocation
	Ram::De_Allocate( m_buffer_start );
	Ram::De_Allocate( m_raw_msg );
	delete ( m_patron );
}

/*
 ******************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager::Enque( uint8_t* src_buf, uint16_t data_length )
{
	if ( data_length <= m_msg_buf_free_size )
	{
		// Check if buffer free size is less than the previous water mark
		if ( m_msg_buf_free_size < m_msg_buf_free_size_water_mark )
		{
			m_msg_buf_free_size_water_mark = m_msg_buf_free_size;
		}

		m_msg_buf_free_size -= data_length;
		uint8_t* end = m_enque + data_length;

		if ( end > m_buffer_end )
		{
			while ( m_enque < m_buffer_end )
			{
				*m_enque = *src_buf;
				m_enque++;
				src_buf++;
			}

			m_enque = m_buffer_start;
			end = ( uint8_t* ) m_enque + ( end - m_buffer_end );
		}

		while ( m_enque < end )
		{
			*m_enque = *src_buf;
			m_enque++;
			src_buf++;
		}
	}
	else
	{
		m_missed_packet_counter++;
	}
}

/*
 ******************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager::Deque( void )
{
	uint_fast16_t data_len_to_print = 0U;
	uint_fast16_t print_len = 0U;

	// Deque data if buffer free size is less than max length supported
	if ( m_msg_buf_free_size < m_msg_buf_max_length )
	{
		data_len_to_print = m_msg_buf_max_length - m_msg_buf_free_size;
		uint8_t* end = m_deque + data_len_to_print;
		print_len = data_len_to_print;

		// Check if end address crosses circular buffer end address
		if ( end > m_buffer_end )
		{
			print_len = m_buffer_end - m_deque;
			m_dbg_if_ptr->Send( m_deque, print_len );
			m_deque = m_buffer_start;
			m_msg_buf_free_size += print_len;
			print_len = data_len_to_print - print_len;
		}

		// Print the remaining bytes of data
		m_dbg_if_ptr->Send( m_deque, print_len );
		m_deque += print_len;
		m_msg_buf_free_size += print_len;
	}
}

/*
 ******************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager::Debug_Msg_Print( uint16_t enable_mask, uint16_t msg_type_val, const char_t* func_name,
									 const char_t* format, ... )
{
	va_list args;
	uint_fast16_t bytes_to_copy = 0U;

	if ( false == m_protect_buf_flag )
	{
		// Set protection flag
		m_protect_buf_flag = true;

		// Add data only when debug manager buffer is initialized
		if ( ( nullptr != m_buffer_start ) && ( nullptr != m_raw_msg ) )
		{
			m_raw_msg[bytes_to_copy++] = '\n';

	#if BF_DBG_PRINT_TIME
			// Get usec time
			BF_Lib::Timers_uSec::TIME_TD micro_sec_time = BF_Lib::Timers_uSec::Get_Time();
			bytes_to_copy += snprintf( ( char_t* )&m_raw_msg[bytes_to_copy],
									   ( DBG_MSG_LEN - bytes_to_copy ),
									   " %u",
									   micro_sec_time );
	#endif

			// Add Message type code in message packet
			msg_type_val = msg_type_index[msg_type_val];
			bytes_to_copy += snprintf( ( char_t* )&m_raw_msg[bytes_to_copy], ( DBG_MSG_LEN - bytes_to_copy ),
									   DBG_MSG_TYPE_INFO[msg_type_val].msg_code );

	#if BF_DBG_PRINT_FUNC_NAME
			for ( uint32_t itr = 0U;
				  ( func_name[itr] != '\0' ) && ( bytes_to_copy < ( DBG_MSG_LEN - bytes_to_copy - 1 ) );
				  itr++ )
			{
				m_raw_msg[bytes_to_copy++] = func_name[itr];
			}
			m_raw_msg[bytes_to_copy++] = ' ';
	#endif
			// Add debug message in transmit buffer
			va_start( args, format );
			bytes_to_copy +=
				vsnprintf( reinterpret_cast<char_t*>( &m_raw_msg[bytes_to_copy] ), ( DBG_MSG_LEN - bytes_to_copy ),
						   format,
						   args );
			va_end( args );

			// Add data in debug buffer
			Enque( m_raw_msg, bytes_to_copy );
		}

		// Reset protection flag
		m_protect_buf_flag = false;
	}
	else
	{
		m_missed_multi_thread_packet++;

		/* TODO: To avoid packet loss in case of multi thread operation i.e if packet formating is
		   in process
		 * and a high priority task takes the control and sends the debug message packet, we can
		 * capture that
		 * message here by creating a run time dynamic buffer and formatting the same with our
		 * packet format
		 * Note: Enque for this packet should be done in next cycle along with the next cycle debug
		 * message
		 *
		 */
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager::Print_Debug_Message_Handler( CR_Tasker* cr_task )
{
	CR_Tasker_Begin( cr_task );

	Print_Hard_Fault_Data();

	while ( true )
	{
		// Dequeue debug message and print on selected interfaces
		Deque();

		// Print diagnostics logs directly on interface avoiding circular buffer
		Debug_Manager_Diagnostics();

		// Sleep task
		CR_Tasker_Delay( cr_task, m_time_slice );
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager::Debug_Manager_Diagnostics( void )
{
	uint8_t buf_usage_percent = 0U;
	uint32_t written_bytes_count = 0U;
	static uint32_t prev_count = 0U;
	static uint32_t prev_multi_thread_miss_count = 0U;

	// Check for missed packet counter value
	if ( prev_count != m_missed_packet_counter )
	{
		m_dbg_if_ptr->Send( ( uint8_t* )DIAG_STR_MISSED_PACKET_COUNT, ( sizeof( DIAG_STR_MISSED_PACKET_COUNT ) - 1 ) );
		written_bytes_count = snprintf( m_diag_buf, DIAG_BUF_LEN, "%u", m_missed_packet_counter );
		m_dbg_if_ptr->Send( ( uint8_t* ) m_diag_buf, written_bytes_count );
		prev_count = m_missed_packet_counter;
	}

	// Check for multi thread packet loss
	if ( prev_multi_thread_miss_count != m_missed_multi_thread_packet )
	{
		m_dbg_if_ptr->Send( ( uint8_t* )DIAG_STR_MULTI_THREAD_MISSED_COUNT,
							( sizeof( DIAG_STR_MULTI_THREAD_MISSED_COUNT ) - 1 ) );
		written_bytes_count = snprintf( m_diag_buf, DIAG_BUF_LEN, "%u", m_missed_multi_thread_packet );
		m_dbg_if_ptr->Send( ( uint8_t* ) m_diag_buf, written_bytes_count );
		prev_multi_thread_miss_count = m_missed_multi_thread_packet;
	}


	// Check if buffer free size is less than the threshold value
	if ( m_msg_buf_free_size_water_mark < m_previous_buf_water_mark )
	{
		buf_usage_percent = ( ( m_msg_buf_max_length - m_msg_buf_free_size_water_mark ) * 100 ) / m_msg_buf_max_length;
		m_dbg_if_ptr->Send( ( uint8_t* )DIAG_STR_BUF_USAGE, ( sizeof( DIAG_STR_BUF_USAGE ) - 1 ) );
		written_bytes_count = snprintf( m_diag_buf, DIAG_BUF_LEN, "%u", buf_usage_percent );
		m_dbg_if_ptr->Send( ( uint8_t* ) m_diag_buf, written_bytes_count );
		m_previous_buf_water_mark = m_msg_buf_free_size_water_mark;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager::Print_Hard_Fault_Data( void )
{
#ifndef ESP32_SETUP
	Fault_Catcher* fault_catch_handle = reinterpret_cast<Fault_Catcher*>( m_fault_catch_handle );
	if ( nullptr != fault_catch_handle )
	{
		fault_catch_handle->Print_Hard_Fault_Data( m_dbg_if_ptr );
	}
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Debug_Manager::Get_DCID_Value( DCI_ID_TD dcid, uint8_t* data )
{
	DCI_ACCESS_ST_TD access_struct;

	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = m_source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	access_struct.data_access.offset = 0U;

	DCI_ERROR_TD return_status = m_patron->Data_Access( &access_struct );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager::Debug_Assert( const uint32_t line_num, const char_t* func_name )
{
	uint8_t bytes_to_copy = 0U;

	// Check if interface is initialized or not
	if ( m_dbg_inter_assert != nullptr )
	{
		for ( uint32_t itr = 0U;
			  ( func_name[itr] != '\0' ) && ( bytes_to_copy < ( DBG_MSG_LEN - bytes_to_copy - 1 ) );
			  itr++ )
		{
			m_raw_msg[bytes_to_copy++] = func_name[itr];
		}
		m_raw_msg[bytes_to_copy++] = ' ';
		bytes_to_copy += snprintf( ( char_t* )&m_raw_msg[bytes_to_copy], DBG_MSG_LEN, "%u", line_num );

		m_dbg_inter_assert->Send_Now( ( uint8_t* ) assert_str, strlen( assert_str ) );
		m_dbg_inter_assert->Send_Now( ( uint8_t* ) m_raw_msg, bytes_to_copy );

		// Adding Delay of 1sec to allow assert message transmission before resetting the system
		uC_Delay( 1000U );
	}
#ifndef ESP32_SETUP
	__asm( "BKPT #0\n" );
#endif
	// coverity[no_escape]
	while ( 1 )
	{}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Debug_Manager_Assert( unsigned int line_num, const char* func_name )
{
	Debug_Manager::Debug_Assert( line_num, func_name );
}
