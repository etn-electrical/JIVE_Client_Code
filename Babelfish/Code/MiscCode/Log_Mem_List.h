/**
 **********************************************************************************************
 *	@file            Log_Mem_List.h
 *
 *	@brief           Log memory (RAM/NV) instances Handler
 *	@details		 The file shall include the function declarations and variables necessary
 *                   for handling list of log memory (RAM/NV) instances required for general
 *                   logging on RAM and NV Memory.
 *	@copyright       2020 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef LOG_MEM_LIST_H
#define LOG_MEM_LIST_H

#include "Log_Config.h"

namespace BF_Misc
{

class Log_Mem_List
{
	public:

		/**
		 * @brief Constructor
		 * @details Initialize the log mem list and append log memory instance in a list
		 * @param[in] log_index: Log type
		 * @param[in] this_ptr: Instance of the Log memory (RAM/NV) class which is created
		 * for given log type (log_index)
		 */
		Log_Mem_List( log_id_t log_index, Log_Mem* this_ptr );

		/**
		 * @brief Get log memory (RAM/NV) instance based on Log Id
		 * @param[in] log_index: Log type
		 * @return Pointer to log memory instance
		 */
		static Log_Mem* Get_Instance( log_id_t log_index );

		/**
		 * @brief Get a list of log memory (RAM/NV) instances created for all enabled log types
		 * @param None
		 * @return Pointer to the list of log memory instances
		 */
		static Log_Mem** Get_List( void );

		/**
		 * @brief Get total number of log types
		 * @param None
		 * @return Total number of log types
		 */
		static inline log_id_t Total_Logs( void )
		{
			log_id_t total_logs;

			#ifndef LOG_MEM_TOTAL_LOGS
			total_logs = TOTAL_LOG_ID;
			#else
			total_logs = LOG_MEM_TOTAL_LOGS;
			#endif

			return ( total_logs );
		}

		/**
		 * @brief Update list of log memory instances, should be used only when
		 * default log memory list constructed is need to be overwritten
		 * @param None
		 * @return Total number of log types
		 */
		static bool Update_List( log_id_t log_index, Log_Mem* log_mem );

	private:
		/**
		 * @brief Pointer to the list of log memory (RAM/NV) instances created for all enabled log types
		 */
		static Log_Mem** m_log_mem_list;

};

}	/* End namespace BF_Misc for this module */

#endif	// LOG_MEM_LIST_H