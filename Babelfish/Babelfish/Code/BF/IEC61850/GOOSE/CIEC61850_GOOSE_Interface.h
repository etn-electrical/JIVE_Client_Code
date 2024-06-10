/**
 *****************************************************************************************
 *  @file
 *	@brief It provides the interface between user application and the IEC61850 protocol stack
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef CIEC61850_GOOSE_INTERFACE_H
   #define CIEC61850_GOOSE_INTERFACE_H

#include "includes.h"
#include "IEC61850_Config.h"
#include "IEC61850_Defines.h"
#include "Iec61850_Debug.h"
#include "OS_Tasker.h"
#include "OS_Task_Config.h"

#include <stdio.h>
#include "sys.h"
#include "tmwtarg.h"
#include "vendor.h"
#include "ber.h"
#include "mms_erro.h"
#include "eth1.h"
#include "iecgoose.h"
#include "igotags.h"

class CIEC61850_GOOSE_Interface
{
	public:
		static const uint8_t QUALITY_TYPE_SIZE = 2U;			///< Quality type variable size

		typedef iec61850_tagtype_t data_type_t;	///< IEC61850 Tag type for all data atributes
		typedef iec61850_message_type_t message_type_t;	///< Message type indication

		typedef IEC_Goose_Context goose_context_t;	///< GOOSE context in stack
		typedef TMW_Mac_Address mac_address_t;	///< MAC address details structure
		typedef IEC_Goose_Strategy goose_strategy_t;///< Strategy information

		typedef MMSd_descriptor MMSd_descriptor_t;	///< MMSd descriptor type information
		typedef uint8_t quality_t[QUALITY_TYPE_SIZE];	///< MMSd Quality type information

		typedef void (* IEC_Goose_Indication)( void* handle, void* param, IEC_GooseHeader* header,
											   unsigned char* dataPtr, MMSd_length dataLength, int timed_out );	///< Stack's
																												///< GOOSE
																												///< indication
																												///< function

		typedef void (* notify_sub_indication_t) ( char_t* cbref );		///< function pointer for subscribtion callback

		/**
		 * @brief Container for the Goose Control Block - user report parameter config params.
		 */
		struct user_report_param_t
		{
			uint8_t num_data_obj;							///< Number of data object details
			struct
			{
				uint8_t num_data_attr;						///< Number of data attributes details
				struct
				{
					data_type_t data_type;					///< Number of data type details of data attribute
					void* data_value;						///< Ponter to data attribute variable
				} data_attr[MAX_NUM_DATA_ATTR_GOOSE];		///< Array of all Data atribute structure
			} data_obj[MAX_NUM_DATA_OBJ_GOOSE];				///< Array of Data objects structure
		};

		/**
		 * @brief Container for the Goose Control Block config params.
		 */
		struct user_gcb_config_param_t
		{
			uint8_t vlan_priority;							///< VLAN priority information
			uint16_t vlan_vID;								///< VLAN VID information
			int16_t vlan_appID;								///< VLAN Applicatioon ID information
			mac_address_t multicast_addr;					///< Multi cast address information

			// Dataset info
			char_t goID[MMSD_OBJECT_NAME_LEN];				///< goID name information
			char_t gocbRef[MMSD_OBJECT_NAME_LEN];			///< control block refernce name
			char_t datSetName[MMSD_OBJECT_NAME_LEN];		///< data set name information
			uint32_t confRev;								///< configuration revision information
			uint8_t ndsComm;								///< ndsComm info
			uint8_t test;									///< test detaiils
			uint8_t num_entries;							///< Number of data objects details
			goose_strategy_t* strategy;						///< Pointer to Strategy structure
			goose_context_t* ptr_goose_context;				///< Pointer to GOOSE context structure
			user_report_param_t user_report_param;			///< Pointer to user_report_param_t structure
		};

		/**
		 * @brief Container for the ethernet config params.
		 */
		struct eth_config_t
		{
			uint8_t port_number;
		};

		/**
		 * @brief Container for the vlan config params.
		 */
		struct vlan_config_t
		{
			uint8_t vlan_priority;							///< VLAN priority information
			uint16_t vlan_vID;								///< VLAN VID information
			int16_t vlan_appID;								///< VLAN Applicatioon ID information
		};

		/**
		 * @brief Container for the Goose Control Block - pdu config params.
		 */
		struct pdu_config_t
		{
			uint8_t goID[MMSD_OBJECT_NAME_LEN];				///< goID name information
			uint8_t gocbRef[MMSD_OBJECT_NAME_LEN];			///< control block refernce name
			uint8_t datSetName[MMSD_OBJECT_NAME_LEN];		///< data set name information
			uint32_t confRev;								///< configuration revision information
			uint8_t ndsComm;								///< ndsComm info
			uint8_t test;									///< test detaiils
			uint8_t num_entries;							///< Number of data objects details
		};

		/**
		 * @brief Container for the Goose control block - Data attributes info.
		 */
		struct data_attr_t
		{
			data_type_t data_type;							///< Tag details of data attribute
			void* data_value;								///< Pointer to data attribute variable
			data_attr_t* next;								///< Pointer to next data attribute structure
		};

		/**
		 * @brief Container for the Goose control block - Data object info.
		 */
		struct data_obj_t
		{
			uint8_t num_data_attr;							///< Number of Data attribute details
			data_attr_t* data_attr;							///< Pointer to Data attribute structure
			data_obj_t* next;								///< Pointer to data_obj_t structre
		};

		/**
		 * @brief Container for the Goose control block - Report params with Data object and data attributes structure.
		 */
		struct gcb_report_param_t
		{
			uint8_t num_data_obj;							///< Number of Data object details
			data_obj_t* data_obj;							///< Pointer to Data data_obj_t structure
			gcb_report_param_t* next;						///< Pointer to gcb_report_param_t structure
		};

		/**
		 * @brief Container for the entire identity of Goose Conrol Block config params.
		 */
		struct gcb_config_param_t
		{
			uint8_t vlan_priority;							///< VLAN priority information
			uint16_t vlan_vID;								///< VLAN VID information
			int16_t vlan_appID;								///< VLAN Applicatioon ID information
			mac_address_t multicast_addr;					///< Multi cast address information
			char_t goID[MMSD_OBJECT_NAME_LEN];				///< goID name information
			char_t gocbRef[MMSD_OBJECT_NAME_LEN];			///< control block refernce name
			char_t datSetName[MMSD_OBJECT_NAME_LEN];		///< data set name information
			uint32_t confRev;								///< configuration revision information
			uint8_t ndsComm;								///< ndsComm info
			uint8_t test;									///< test detaiils
			uint8_t num_entries;							///< Number of data objects details
			goose_strategy_t* strategy;						///< Pointer to Strategy structure
			goose_context_t* ptr_goose_context;				///< Pointer to GOOSE context structure
			gcb_report_param_t gcb_report_param;			///< Pointer to user_report_param_t structure
			gcb_config_param_t* next;						///< Pointer to next gcb_config_param_t structure
		};

		/**
		 *  @brief Constructor
		 *  @details Creates a CIEC61850_GOOSE_Interface.
		 *  @return None
		 */
		CIEC61850_GOOSE_Interface( void );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of CIEC61850_GOOSE_Interface goes out of scope
		 *  @return None
		 */
		~CIEC61850_GOOSE_Interface( void );

		/**
		 *  @brief           Initialize Goose context structure.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Initialize( void );

		/**
		 *  @brief           Set vlan config structure.
		 *  @param[in]		 vlan_config, structrue for vlan config params.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Set_Vlan_Info( vlan_config_t vlan_config );

		/**
		 *  @brief           Set user goose contol config params to create goose app config param to publish message.
		 *  @param[in]		 user_gcb_config_param, usser goose config param.
		 *  @param[out]		 None.
		 *  @return          retrun error type,
		 */
		iec61850_error_t Set_Publish_Config( user_gcb_config_param_t* user_gcb_config_param );

		/**
		 *  @brief           Set user goose contol config params to create goose app config param to Subscribe message.
		 *  @param[in]		 user_gcb_config_param, usser goose config param.
		 *  @param[out]		 None.
		 *  @return          retrun error type
		 */
		iec61850_error_t Set_Subscribe_Config( user_gcb_config_param_t* user_gcb_config_param );

		/**
		 *  @brief           Goose start function to allocate memory to start goose message transmit and receive.
		 *  @param[in]		rx_task_init- this flag control the creation of Receive and Process OS thread. should be set
		 * to false if goose subscription is not required.
		 *  @param[in]		callback- subscribe callback for user notification. This callback function gets called
		 * whenever there is a subscribed message received by goose stack. this is an optional argument, if subscription
		 * is not required, nullptr can be passed
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Start( bool rx_task_init, notify_sub_indication_t callback );

		/**
		 *  @brief           new message to publish with updated data.
		 *  @param[in]		 gcb_num, indicates which goose control block to update(like 1, 2, ...).
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Send_Publish_Update_Data( uint8_t gcb_num );

		/**
		 *  @brief           Stop Goose publish/subscribe message.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Stop( void );

		/**
		 *  @brief           Publish goose message as per goose strategy and check if any message is received.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          None
		 */
		void Service( void );

		/**
		 *  @brief           Publish goose message as per goose strategy for retransmission control blocks
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          None
		 */
		void Transmit( void );

		/**
		 *  @brief           It checks to receive goose message.it wakes up to process Goose message if message is
		 * received.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Receive( void );

		/**
		 *  @brief           It process the received Goose message. if message is valid, it decode Goose message and
		 * call user callback function.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          None
		 */
		void Process( void );

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
		 *  @brief           This function shuts down all goose activity.
		 *  @param[in]		 cntxt, indicates goose contex structure.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Close( void );

		/**
		 *  @brief           OS task static
		 *  @param[in]	     param.
		 *  @return          None
		 */
		static void Receive_OS_Task_Static( void* param )
		{
			( ( CIEC61850_GOOSE_Interface* )param )->Receive_OS_Task();
		}

		/**
		 *  @brief           OS task static
		 *  @param[in]	     param.
		 *  @return          None
		 */
		static void Process_OS_Task_Static( void* param )
		{
			( ( CIEC61850_GOOSE_Interface* )param )->Process_OS_Task();
		}

		static const uint8_t DISABLE_VALUE = 0U;					///< Variable used to initialize the value to zero
		static const uint8_t ENABLE_VALUE = 1U;	///< Variable used to initialize the value to One

		static bool m_subscribe_notify;
		static bool m_sub_timeout;
		static char_t m_sub_goose_cb_ref[CB_IDENT_SIZE_GOOSE];

	private:

		/**
		 *  @brief           Initialize Goose context structure.
		 *  @param[in]		 ptr_iec_goose_context, goose context structure.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Open_Context( IEC_Goose_Context* ptr_iec_goose_context );

		/**
		 *  @brief           Goose config param to create memory to start publish transmission.
		 *  @param[in]		 ptr_goose_params, goose config param structure.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Create( gcb_config_param_t* ptr_goose_params );

		/**
		 *  @brief           Goose config param to create memory to start subscribe transmission.
		 *  @param[in]		 None.
		 *  @param[out]		 None.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Subscribe( gcb_config_param_t* ptr_goose_params );

		/**
		 *  @brief           Encode goose contrlo block param to publish message, Data encode in to reverse order.
		 *  @param[in]		 ptr_goose_param, pointer to goose params.
		 *  @param[in]		 ptr_buffer, pointer to buffer with encoded data.
		 *  @param[in]		 buffer_Size, indicates the maximum data length of goose message.
		 *  @param[out]		 ptr_count, indicate num of goose entries
		 *  @param[out]		 ptr_offset, point to bufffer data.
		 *  @param[out]		 ptr_length, indicate length of encoded data.
		 *  @return          None
		 */
		bool Encode_Dataset( gcb_report_param_t* ptr_goose_param, uint8_t* ptr_buffer, uint16_t buffer_Size,
							 uint16_t* ptr_count, uint16_t* ptr_offset, uint16_t* ptr_length );

		/**
		 *  @brief           Encode Data attribute of data object into reverse order, .
		 *  @param[in]		 ptr_data, point to data obj to encode data.
		 *  @param[in]		 offset, indicate nth pointer of data object.
		 *  @param[out]		 ptr_output, Encoded data update into buffer.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		bool Encode_Data_Attribute( data_obj_t* ptr_data, uint8_t offset, MMSd_descriptor_t* ptr_output );

		/**
		 *  @brief           Initialize Goose context structure.
		 *  @param[in]		 ptr_data, point to data attr to encode data.
		 *  @param[in]		 offset, indicate nth pointer of data attr.
		 *  @param[in]		 ptr_output, Encoded data update into buffer.
		 *  @return          length, indicate encoded data length
		 */
		int16_t Encode_Emit_Data( data_attr_t* ptr_data, uint8_t offset, MMSd_descriptor_t* ptr_output );

		/**
		 *  @brief           Config goose control block as per user config data with data type.
		 *  @param[in]		 gcb_node, point to goose config param structure
		 *  @param[in]		 data_type, indicate data type of variable to config.
		 *  @param[in]		 data_value, load user data value to config goose config structre.
		 *  @return          data_attr structure
		 */
		data_attr_t* Push_Node( gcb_config_param_t** gcb_node, data_type_t data_type, void* data_value );

		/**
		 *  @brief           This callback function will be called once if received subscribed message.
		 *  @param[in]		 ptr_handle, handle subscribed message structure.
		 *  @param[in]		 ptr_param, pointer to param structure.
		 *  @param[in]		 ptr_header, pointer goose header structure params.
		 *  @param[in]		 ptr_data, point to received buffer data.
		 *  @param[in]		 data_length, indicates received length of received goose message.
		 *  @param[in]		 timed_out, indicate timeout occur or not.
		 *  @return          None
		 */
		static void Iec_Goose_Indication( void* ptr_handle, void* ptr_param, IEC_GooseHeader* ptr_header,
										  unsigned char* ptr_data, MMSd_length data_length, int timed_out );

		/**
		 *  @brief           Decode dataset of subscribed message.
		 *  @param[in]		 ptr_input,
		 *  @param[out]		 ptr_goose_params,
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static bool Decode_Dataset( MMSd_descriptor_t* const ptr_input,
									gcb_report_param_t const* const ptr_goose_params );

		/**
		 *  @brief           Decode data attr of data objech which received goose message.
		 *  @param[in]		 ptr_input, pointer to decode message.
		 *  @param[in]		 data_obj, indicate data object to decode.
		 *  @param[in]		 offset, indicates nth data offset.
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static bool Decode_Data_Attribute( MMSd_descriptor_t* const ptr_input,
										   data_obj_t const* const data_obj, uint8_t offset );

		/**
		 *  @brief           Fetch the data as per data type of data attr..
		 *  @param[in]		 ptr_input, pointer to received buffer.
		 *  @param[out]		 ptr_data, pointer to decode data
		 *  @return          TRUE if succeeded, else FALSE
		 */
		static bool Decode_Fetch_Data( MMSd_descriptor_t* const ptr_input, data_attr_t* const ptr_data );

		/**
		 *  @brief           set user config to goose config for publish or subscribed message.
		 *  @param[in]		 user_gcb_config_param, user config param structure.
		 *  @param[in]		 message_type, indicate publish or Subscribe message config.
		 *  @return          None
		 */
		iec61850_error_t Set_Config( user_gcb_config_param_t* user_gcb_config_param,
									 message_type_t message_type );

		/**
		 * @brief variabe to indicate Goose operation state.
		 */
		iec61850_goose_state_t m_goose_state;

		/**
		 * @brief Pointer to Goose context structure.
		 */
		goose_context_t* m_goose_context;

		/**
		 * @brief Pointer to Goose Control block params for publish.
		 */
		gcb_config_param_t* m_gcb_config_param_pub;

		/**
		 * @brief Variable which carries maximum number of goose control params for publish.
		 */
		uint8_t m_num_gcb_config_param_pub;

		/**
		 * @brief Variable which carries wait time to receive first subscribed message.
		 */
		uint16_t m_initial_goose_wait_ms;

		/**
		 * @brief Pointer to Goose Control block params for subscribe.
		 */
		gcb_config_param_t* m_gcb_config_param_sub;

		/**
		 * @brief Variable which carries maximum number of goose control params for subscribe.
		 */
		uint8_t m_num_gcb_config_param_sub;

		static uint16_t m_state_num;	///< Indicates the number of new subscribe messages

		/**
		 *  @brief      callback function is used for indication to user to update param.
		 *  @param[in]	none
		 *  @param[out]	char_t* cbref
		 *  @return     TRUE if succeeded, else FALSE
		 */
		static notify_sub_indication_t Subscribtion_Callback;

		/**
		 *  @brief Create Receive tasks for receiving and processing the goose message
		 *  @param[in] void : None
		 *  @param[out] None
		 *  @return void: None
		 */
		void Init_Receive_Tasks( void );

		/**
		 *  @brief           OS task to receive Goose Message handling, call Receive().
		 *  @param[in]		None
		 *  @return          None
		 */
		void Receive_OS_Task( void );

		/**
		 *  @brief           OS task to process the Goose message. It wakes up from Receive_OS_Task if message is
		 * received, call process()
		 *  @param[in]		None
		 *  @return          None
		 */
		void Process_OS_Task( void );

};

#endif	// CIEC61850_GOOSE_INTERFACE_H