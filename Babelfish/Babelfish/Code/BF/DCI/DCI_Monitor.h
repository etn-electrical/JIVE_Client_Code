/**
 *****************************************************************************************
 *	@brief This file contains the DCI Monitor Class.  This class displays parameters by default
 *	on the terminal window of IAR.  Custom behavior can be attached if need be.
 *
 *	@details
 *
 *	@copyright 2015 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_MONITOR_H
	#define DCI_MONITOR_H

#include "Includes.h"
#include "DCI_Workers.h"
#include "Change_Tracker.h"
#include "StdLib_MV.h"

/**
 ****************************************************************************************
 * @brief This class is used to monitor DCIDs for change and print out the value.  By default the
 * data will be printed to the debugger terminal.
 *
 * @details This class will allow a developer to monitor DCIDs during debug time.  To provide some
 * monitoring behavior this class must be constructed with a structure defining the DCIDs to watch
 * (see example struct below).  If you use your own callback function you must pass in a callback
 * function to call.  This function will then be called at a change tracker context (try to be
 * quick if you can).  The function will receive the DCID which changed,
 * the attribute accessed and the most recent data value.  This callback happens through the use
 * of a Change tracker and therefore does not monitor the value on an instantaneous level.
 * It will typically catch any value change you may need to monitor.
 *
 *
 * @code
    //Example struct below:
    static const DCI_ID_TD monitor_dcids[] =
        { DCI_VALUE_0_DCID, DCI_VALUE_1_DCID, DCI_VALUE_2_DCID, DCI_VALUE_3_DCID };
    static const DCI_Monitor::monitor_list_t monitor_struct =
        {
            monitor_dcids,
            sizeof( monitor_dcids ) / sizeof( DCI_ID_TD )
        };
 * @endcode
 *
 * If this class is called without a callback function then the internal default one
 * will take over.  This default callback will term print the changes.  It will print out the
 * following data (in CSV format):
 * "DCI_Mon:, Sample Counter, Sample Time(in ms and rolls over), DCID, Access Type, Value(s)"
 *
 ****************************************************************************************
 */
class DCI_Monitor
{
	public:
		struct monitor_list_t
		{
			DCI_ID_TD const* list;
			DCI_ID_TD length;
		};

		/**
		 * @brief Defines the callback function and parameter to be passed back.
		 * @details The parameter is typically the this pointer.  The callback is defaulted
		 * to the internal handler which will term print the results.  If an alternate is
		 * desired then make sure it matches the below typedef.
		 */
		typedef void* cback_param_t;

		/**
		 * @brief Callback function.
		 * @param param: Typically the this pointer.
		 * @param dci_id: The DCID which has changed.
		 * @param data: A pointer to a temporary image of the present value of the DCID attribute.
		 * @param data_length: The length of data available.
		 * @param attribute: The attribute which has changed.  (See Change Tracker for a list.
		 */
		typedef void (* cback_func_t)( cback_param_t param, DCI_ID_TD dci_id,
									   DCI_DATA_PASS_TD const* data, DCI_LENGTH_TD data_length,
									   Change_Tracker::attrib_t attribute );

		/**
		 *  @brief Builds a DCI monitor and requires a list of parameters to watch.
		 *  @details Pass in the list of parameters to watch and the function to call on change.
		 *  Remember that this class works at the change tracker priority level and therefore
		 *  will run only when the system is up and running (CR Tasker is up).
		 *
		 *  @param[in] dci_list: A structure with a list of DCIDs to watch and the length.
		 *  @param[in] param: Callback parameter for the function if desired
		 *  (typically the this pointer).
		 *  @param[in] func: A callback function to call.  If left defaulted it will call the
		 *  internal term print behavior.
		 */
		DCI_Monitor( monitor_list_t const* dci_list,
					 cback_param_t param = reinterpret_cast<cback_param_t>( nullptr ),
					 cback_func_t func = & Change_Print_Static );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object goes out of scope or deleted.
		 *  @return None
		 */
		~DCI_Monitor( void );

	private:
		/**
		 * Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		DCI_Monitor( const DCI_Monitor& rhs );
		DCI_Monitor & operator =( const DCI_Monitor& object );

		/**
		 * @brief
		 * @param dci_id
		 * @param attribute_mask
		 */
		void Change_Handler( DCI_ID_TD dci_id,
							 Change_Tracker::attrib_mask_t attribute_mask );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Change_Handler_Static( Change_Tracker::cback_param_t param,
										   DCI_ID_TD dci_id,
										   Change_Tracker::attrib_mask_t attribute_mask )
		{
			reinterpret_cast<DCI_Monitor*>( param )->Change_Handler( dci_id,
																	 attribute_mask );
		}

		/**
		 * @brief Does the printing.
		 * @param dci_id: The changed DCID.
		 * @param data: pointer to a array of data (present value of attribute).
		 * @param data_length: Length of the data.  Just in case you don't know already.
		 * @param attribute_mask: The mask of the attribute which changed.  This should only be
		 * one attribute.
		 */
		void Change_Print( DCI_ID_TD dci_id, DCI_DATA_PASS_TD const* data,
						   DCI_LENGTH_TD data_length, Change_Tracker::attrib_t attribute ) const;

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Change_Print_Static( cback_param_t param, DCI_ID_TD dci_id,
										 DCI_DATA_PASS_TD const* data, DCI_LENGTH_TD data_length,
										 Change_Tracker::attrib_t attribute )
		{
			reinterpret_cast<DCI_Monitor*>( param )->Change_Print( dci_id, data, data_length,
																   attribute );
		}

		cback_param_t m_cback_param;
		cback_func_t m_cback;

		/**
		 * @brief Pointer to Change Tracker
		 */
		Change_Tracker* m_change_tracker;

		/**
		 * @brief Pointer to worker class which retrieves the current value.
		 */
		DCI_Workers* m_worker;

		uint8_t* m_temp_data;

		DCI_SOURCE_ID_TD m_source_id;
};

#endif
