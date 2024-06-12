/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef CIEC61850_SAV_INTERFACE_H
   #define CIEC61850_SAV_INTERFACE_H

#include "includes.h"
#include "IEC61850_Config.h"
#include "IEC61850_Defines.h"
#include "Iec61850_Debug.h"
#include <stdio.h>
#include "sys.h"
#include "tmwtarg.h"
#include "vendor.h"
#include "ber.h"
#include "mms_erro.h"
#include "eth1.h"
#include "sav.h"

#include "uC_Timers.h"
#include "uC_Timers_HW.h"
#include "uC_Single_Timer.h"

/*
 *****************************************************************************************
 *		prototypes - IEC61850 stack dependencies
 *****************************************************************************************
 */
MMSd_length TMW_SAV_EmitTagAndLength( uint8_t tag,
									  int length,
									  MMSd_descriptor* E );

MMSd_length TMW_SAV_EmitOctets( uint8_t* s,
								MMSd_length len,
								MMSd_descriptor* E );

/*
 *****************************************************************************************
 *		Defines
 *****************************************************************************************
 */

class CIEC61850_SAV_Interface
{
	public:

		typedef iec61850_tagtype_t data_type_t;			///< IEC61850 Tag type for all data atributes
		typedef iec61850_message_type_t message_type_t;	///< Message type indication

		typedef TMW_SAV_Context sav_context_t;		///< SAV context in stack
		typedef TMW_SAV_APDU sav_apdu_t;			///< SAV APDU in stack
		typedef TMW_SAV_ASDU sav_asdu_t;			///< SAV APDU in stack

		typedef TMW_Mac_Address mac_address_t;		///< MAC address details structure

		typedef MMSd_descriptor MMSd_descriptor_t;	///< MMSd descriptor type information
		typedef uint8_t MMSd_Quality[2];		///< MMSd Quality type information

		typedef TMW_SAV_Error sav_error_t;		///< SAV error type information


		/**
		 * @brief Container for the Sav Control Block - user report parameter config params.
		 */
		struct sav_user_report_param_t	///< Sav user data structure
		{
			uint8_t num_data_obj;		///< Number of data object details
			struct
			{
				uint8_t num_data_attr;				///< Number of data attributes of each data objects details
				struct
				{
					data_type_t data_type;			///< Number of data type details of data attribute
					void* data_value[MAX_NUM_ASDU];	///< Ponter to data attribute variable
				} data_attr[MAX_NUM_DATA_ATTR_SAV];	///< Array of all Data atribute structure
			} data_obj[MAX_NUM_DATA_OBJ_SAV];		///< Array of Data objects structure
		};

		/**
		 * @brief Container for the Sav Control Block config params.
		 */
		struct user_sav_config_param_t
		{
			uint8_t vlan_priority;						///< VLAN priority information
			uint16_t vlan_vID;							///< VLAN VID information
			uint16_t vlan_appID;						///< VLAN Applicatioon ID information
			mac_address_t multicast_addr;				///< Multi cast address information

			char_t svID[MMSD_OBJECT_NAME_LEN];			///< savID name information
			char_t svcbRef[MMSD_OBJECT_NAME_LEN];			///< control block refernce name
			char_t data_set_name[MMSD_OBJECT_NAME_LEN];	///< data set name information
			uint32_t conf_rev;							///< configuration revision information
			uint8_t samp_sync;							///< Sample sync
			uint16_t samp_rate;							///< Sample rate
			int16_t samp_mode;							///< sample mode
			int16_t num_cycles_per_sec;					///< Cycles/sec
			int16_t num_asdu;							///< number of ASDU
			uint8_t num_entries;						///< Number of Data object Details
			sav_user_report_param_t user_report_param;	///< Pointer to user_report_param_t structure
		};

		/* structure for sav control block config params */
		/**
		 * @brief Container for the Sav control block - Data attributes info.
		 */
		struct data_attr_t
		{
			data_type_t data_type;						///< Tag details of data attribute
			void* data_value[MAX_NUM_ASDU];				///< Pointer to data attribute variable
			data_attr_t* next;							///< Pointer to next data attribute structure
		};	/* data variable for each data objects */

		/**
		 * @brief Container for the Sav control block - Data object info.
		 */
		struct data_obj_t
		{
			uint8_t num_data_attr;						///< Number of Data attribute details
			data_attr_t* data_attr;						///< Pointer to Data attribute structure
			data_obj_t* next;							///< Pointer to data_obj_t structre
		};

		/**
		 * @brief Container for the Sav control block - Report params with Data object and data attributes structure.
		 */
		struct sav_report_param_t
		{
			uint8_t num_data_obj;						///< Number of Data object details
			data_obj_t* data_obj;						///< Pointer to Data data_obj_t structure
			sav_report_param_t* next;					///< Pointer to sav_report_param_t structure
		};

		/**
		 * @brief Container for the entire identity of Sav Conrol Block config params.
		 */
		struct sav_config_param_t
		{
			uint8_t vlan_priority;						///< VLAN priority information
			uint16_t vlan_vID;							///< VLAN VID information
			uint16_t vlan_appID;						///< VLAN Applicatioon ID information
			mac_address_t multicast_addr;				///< Multi cast address information

			char_t svID[MMSD_OBJECT_NAME_LEN];			///< savID name information
			char_t svcbRef[MMSD_OBJECT_NAME_LEN];			///< control block refernce name
			char_t data_set_name[MMSD_OBJECT_NAME_LEN];	///< data set name information
			uint32_t conf_rev;							///< configuration revision information
			uint8_t samp_sync;							///< Sample sync
			uint16_t samp_rate;							///< Sample rate
			int16_t samp_mode;							///< sample mode
			int16_t num_cycles_per_sec;					///< Cycles/sec
			int16_t num_asdu;							///< number of ASDU
			uint8_t num_entries;						///< Number of Data object Details

			uint16_t samp_cnt;							///< sample count infor
			uint8_t have_seen_one;						///< debug info
			uint32_t samples_received;					///< number of samples received info for debug
			uint32_t errors;							///< number of error info for debug
			uint8_t running_asdu;						///< running ASDU

			uint16_t useSecurity;						///< Security info
			uint8_t security[SAV_MAX_SECURITY];			///< Security info
			uint16_t nSecurityLen;						///< Security info
			uint16_t valid_members;						///< Valid members details for 9_2LE standard

			uint32_t usec_per_publish;					///< Calculation period for next publish
			int16_t publishes_per_interval;				///< number of publishes per interval
			uint32_t interval;							///< interval period
			uint8_t running;							///< Thread running info

			TmwTargEve_t linux_event;					///< Event info
			sav_apdu_t* ptr_apdu;						///< pointer to APDU context
			sav_context_t* ptr_sav_context;				///< Pointer to report SAV context structure
			sav_report_param_t sav_report_param;		///< Pointer to report sav report(Sample) param structure
			sav_config_param_t* next;					///< Pointer to next sav_config_param_t structure
		};

		/**
		 *  @brief Constructor
		 *  @details Creates a CIEC61850_SAV_Interface.
		 *  @return None
		 */
		CIEC61850_SAV_Interface( void );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of CIEC61850_SAV_Interface goes out of scope
		 *  @return None
		 */
		~CIEC61850_SAV_Interface( void ){}

		/**
		 *  @brief           Initialize Sav context structure.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Initialize( void );

		/**
		 *  @brief           Set user sav contol config params to create sav app config param to publish message.
		 *  @param[in]		 user_sav_config_param, usser sav config param.
		 *  @param[out]		 None.
		 *  @return          retrun error type,
		 */
		iec61850_error_t Set_Publish_Config( user_sav_config_param_t* user_sav_config_param );

		/**
		 *  @brief           Set user sav contol config params to create sav app config param to Subscribe message.
		 *  @param[in]		 user_sav_config_param, usser sav config param.
		 *  @param[out]		 None.
		 *  @return          retrun error type
		 */
		iec61850_error_t Set_Subscribe_Config( user_sav_config_param_t* user_sav_config_param );

		/**
		 *  @brief           Sav start function to allocate memory to start sav message transmit and receive.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Start( void );

		/**
		 *  @brief           Stop Sav publish/subscribe message.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Stop( void );

		/**
		 *  @brief           Publish sav message as per sav strategy and check if any message is received.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          None
		 */
		void Service( void );

		/**
		 *  @brief           This function removes the activ out pool entry for publish message.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Delete( void );

		/**
		 *  @brief           This function removes the active in pool entry for subscription message.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Unsubscribe( void );

		/**
		 *  @brief           This function removes the active in pool entry for subscription message.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Close( void );

		static const uint8_t DISABLE_VALUE = 0U;				///< Variable used to initialize the value to zero
		static const uint8_t ENABLE_VALUE = 1U;					///< Variable used to initialize the value to One
		static const uint8_t IEC_SAV_IDENT_SIZE = 64U;			///< MAX SAV Identifier size
		static const uint16_t SAV_SUB_SIZE_OF_DATALEN = 48U;	///< Size of received params length
		static const uint32_t SAV_MEG_VALUE = 1000000L;			///< Constant value
		static const uint16_t SAV_KILO_VALUE = 1000U;			///< Constant value
		static const uint16_t sample7 = 0x87U;					///< IEC61850 stack macro, tag id for SAV sample Data
		static const uint16_t IEC61850_SAV_PUB_SLEEP_TIME_MS = 1U;	///< ASV Task sleep time

		static TaskHandle_t m_sav_thread_wakeup;				///< SAV thread wakeup

		/**
		 *  @brief           Sav Thread task.
		 *  @param[in]		 pThreadArgs, sav config param structure.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static void Iec61850_Sav_CR_Tasker( CR_Tasker* cr_task, CR_TASKER_PARAM pThreadArgs );

		/**
		 *  @brief           Publish SAV messages.
		 *  @param[in]		 pThreadArgs, sav config param structure.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static void Sav_Thread_Function( void* pThreadArgs );

	private:
		/**
		 * @brief Pointer to Sav context structure.
		 */
		sav_context_t* m_sav_context;

		/**
		 * @brief Pointer to Sav Control block params for publish.
		 */
		sav_config_param_t* m_sav_config_param_pub;

		/**
		 * @brief Variable which carries maximum number of sav control params for publish.
		 */
		uint8_t m_num_sav_config_param_pub;


		/**
		 * @brief Pointer to Sav Control block params for subscribe.
		 */
		sav_config_param_t* m_sav_config_param_sub;

		/**
		 * @brief Variable which carries maximum number of sav control params for subscribe.
		 */
		uint8_t m_num_sav_config_param_sub;

		/**
		 * @brief variabe to indicate Sav operation state.
		 */
		iec61850_sav_state_t m_sav_state;

		/**
		 * @brief variabe to indicate little indian type or not to publish message.
		 */
		static bool_t m_little_endian;

		/**
		 *  @brief           Initialize Sav context structure.
		 *  @param[in]		 ptr_iec_sav_context, sav context structure.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Open_Context( sav_context_t* ptr_iec_sav_context );

		/**
		 *  @brief           set user config to sav config for publish or subscribed message.
		 *  @param[in]		 user_sav_config_param_t, user config param structure.
		 *  @param[in]		 message_type, indicate publish or Subscribe message config.
		 *  @return          None
		 */
		iec61850_error_t Set_Config( user_sav_config_param_t* user_sav_config_param_t,
									 message_type_t message_type );

		/**
		 *  @brief           Config sav control block as per user config data with data type.
		 *  @param[in]		 sav_node, point to sav config param structure
		 *  @param[in]		 data_type, indicate data type of variable to config.
		 *  @param[in]		 data_value, load user data value to config sav config structre.
		 *  @return          data_attr structure
		 */
		data_attr_t* Push_Node( sav_config_param_t** sav_node, data_type_t data_type, void* data_value );

		/**
		 *  @brief           Sav config param to create memory to start publish transmission.
		 *  @param[in]		 ptr_sav_params, sav config param structure.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Create( sav_config_param_t* ptr_sav_params );

		/**
		 *  @brief           Sav config param to create memory to start subscribe transmission.
		 *  @param[in]		 ptr_sav_params, sav config param structure.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Subscribe( sav_config_param_t* ptr_sav_params );

		/**
		 *  @brief           This call back function to Encode sav message.
		 *  @param[in]		 pApdu, Pointer to APDU structure
		 *  @param[in]		 pAsdu, Pointer to ASDU structure
		 *  @param[in]		 smpCnt, sample count info
		 *  @param[in]		 encode_callback_data, Pointer to input SAV config param structure
		 *  @param[out]		 ptr_output, Encoded data update into buffer
		 *  @return          length, length of encoded data
		 */
		static uint16_t Encode_Sav( sav_apdu_t* ptr_apdu, sav_asdu_t* pAsdu, int samp_count, void* encode_callback_data,
									MMSd_descriptor_t* ptr_output );

		/**
		 *  @brief           Encode sav contrlo block param to publish message, Data encode in to reverse order.
		 *  @param[in]		 ptr_sav_param, pointer to sav params.
		 *  @param[out]		 ptr_output, Encoded data update into buffer
		 *  @param[in]		 running_asdu, current asdu info.
		 *  @return          None
		 */

		static uint16_t Encode_Dataset( sav_report_param_t* ptr_sav_param, MMSd_descriptor_t* ptr_output,
										uint8_t running_asdu );

		/**
		 *  @brief           Encode Data attribute of data object into reverse order, .
		 *  @param[in]		 ptr_data, point to data obj to encode data.
		 *  @param[out]		 ptr_output, Encoded data update into buffer.
		 *  @param[in]		 running_asdu, current asdu info.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static uint16_t Encode_Data_Attribute( data_obj_t* ptr_data, uint8_t offset, MMSd_descriptor_t* ptr_output,
											   uint8_t running_asdu );

		/**
		 *  @brief           Initialize Sav context structure.
		 *  @param[in]		 ptr_data, point to data attr to encode data.
		 *  @param[out]		 ptr_output, Encoded data update into buffer.
		 *  @param[in]		 running_asdu, current asdu info.
		 *  @return          length, indicate encoded data length
		 */
		static uint16_t Encode_Emit_Data( data_attr_t* ptr_data, uint8_t offset, MMSd_descriptor_t* ptr_output,
										  uint8_t running_asdu );


		/**
		 *  @brief           This callback function will be called once if received subscribed message.
		 *  @param[in]		 pHandle, point to handler function.
		 *  @param[in]		 pApdu, point to APDU structure.
		 *  @return          None
		 */
		static void Sampled_Values_Indication( void* pHandle, sav_apdu_t* pApdu );

		/**
		 *  @brief           Decode dataset of subscribed message.
		 *  @param[in]		 ptr_input, input buffer to decode message
		 *  @param[out]		 ptr_sav_params, update decode message into param structure.
		 *  @param[in]		 running_asdu, current asdu info.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static bool Decode_Dataset( uint8_t** ptr_input, sav_report_param_t const* const ptr_sav_params,
									uint8_t running_asdu );

		/**
		 *  @brief           Decode data attr of data objech which received sav message.
		 *  @param[in]		 ptr_input, pointer to decode message.
		 *  @param[out]		 data_obj, indicate data object to decode message.
		 *  @param[in]		 offset, indicates nth data offset.
		 *  @param[in]		 running_asdu, current asdu info.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static bool Decode_Data_Attribute( uint8_t** ptr_input, data_obj_t const* const data_obj,
										   uint8_t offset, uint8_t running_asdu );

		/**
		 *  @brief           Fetch the data as per data type of data attr..
		 *  @param[in]		 ptr_input, pointer to received buffer.
		 *  @param[out]		 ptr_data, pointer to decode data
		 *  @param[in]		 running_asdu, current asdu info.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static bool Decode_Fetch_Data( uint8_t** ptr_input, data_attr_t* const ptr_data,
									   uint8_t running_asdu );

		/**
		 *  @brief           Calculate publish time interval based on config input.
		 *  @param[in]		 ptr_sav_params, sav config param structure.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Compute_Publish_Intervals( sav_config_param_t* ptr_sav_params );

		/**
		 *  @brief           To create smpling thread to publish SAV message.
		 *  @param[in]		 ptr_sav_params, sav config param structure.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		void Start_Sampling_Thread( sav_config_param_t* ptr_sav_params );

		/**
		 *  @brief           Handle timer elapsed callback function.
		 *  @param[in]		 Callback param.
		 *  @return          None
		 */
		static void Single_Timer_Cback( uC_Single_Timer::cback_param_t param );

		OS_Task* m_sampling_task;	///< Pointer to sampling task

};

#endif	// CIEC61850_SAV_INTERFACE_H
