/**
 *****************************************************************************************
 *  @file
 *	@brief It provides the interface between user application and the IEC61850 protocol stack with DCI database
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef CIEC61850_GOOSE_DCI_MANAGER_H
   #define CIEC61850_GOOSE_DCI_MANAGER_H

#include "includes.h"
#include "DCI_Patron.h"
#include "IEC61850_GOOSE_DCI.h"
#include "DCI_Owner.h"
#include "CIEC61850_GOOSE_Interface.h"

/*
 **************************************************************************************************
 *  Class Declaration
 **************************************************************************************************
 */
class CIEC61850_GOOSE_DCI_Manager
{
	public:

		typedef IEC61850_GOOSE_DCI::group_param_t group_param_t;///< IEC61850 GOOSE group param ID
		typedef IEC61850_GOOSE_DCI::sub_group_param_t sub_group_param_t;///< IEC61850 GOOSE Sub group param ID
		typedef CIEC61850_GOOSE_Interface::user_gcb_config_param_t user_config_t;///< IEC61850 user gcb configuration
																				///< structure
		typedef CIEC61850_GOOSE_Interface::mac_address_t mac_address_t;	///< MAc address structure details
		/**
		 * @brief GOOSE Message sub group parameters array start index
		 */
		struct group_len_details_t
		{
			uint16_t main_group_start;	///< Main group start index
			uint16_t sub_group_start;	///< Sub group start index
		};

		/**
		 *  @brief Provides access to create the CIEC61850_GOOSE_DCI_Manager class
		 *  @param[in] goose_interface: Pointer to CIEC61850_GOOSE_Interface class
		 *  @return CIEC61850_GOOSE_DCI_Manager : Pointer to CIEC61850_GOOSE_DCI_Manager class object
		 */
		static CIEC61850_GOOSE_DCI_Manager* Get_Instance(
			CIEC61850_GOOSE_Interface* goose_interface );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of CIEC61850_GOOSE_DCI_Manager goes out of scope
		 *  @return None
		 */
		~CIEC61850_GOOSE_DCI_Manager( void );

		/**
		 *  @brief Provides access to initialize the GOOSE parameters.
		 *  @param[in] Void: None
		 *  @return iec61850_error_t : Error code while initializing the goose configurations
		 */
		iec61850_error_t DCI_Manager_Init( void );

		/**
		 *  @brief Call back function for publish message
		 *  @param[in] handle: Callback handler
		 *  @param[in] access_struct: Access structure for DCI patron access
		 *  @return DCI_CB_RET_TD : Error code return while retriving the data from the memory
		 */
		static DCI_CB_RET_TD Publish_Change_Tracker_Static( DCI_CBACK_PARAM_TD handle,
															DCI_ACCESS_ST_TD* access_struct );

		/**
		 *  @brief Function to set the publish indication flag and initializes the DCID
		 *  @param[in] access_struct: Access structure for DCI patron access
		 *  @return DCI_CB_RET_TD : Error code return while retriving the data from the memory
		 */
		DCI_CB_RET_TD Publish_Change_Tracker( DCI_ACCESS_ST_TD* access_struct );

		/**
		 *  @brief Updates the Goose interface parameters to publish the GOOSE message
		 *  @return iec61850_error_t : Error code return while retriving the data from the memory
		 */
		iec61850_error_t Send_Publish_Update_Data( void );

		/**
		 *  @brief Function to set the subscriber GOOSE parameters
		 *  @return Void: None
		 */
		void Subscribe_Change_Tracker( void );

		/**
		 *  @brief Provides the write access to publish attributes
		 *  @param[in] state_change: pointer to the variable which caries the new value to be set
		 *  @param[in] group_id: Publish group ID inofrmation
		 *  @param[in] sub_group_id: Publish Sub group ID inofrmation
		 *  @param[in] data_atribute_num: Data attribute number whose value needs to be set
		 *  @return Void: None
		 */
		void Write_Pub_Attr( void* state_change, group_param_t group_id,
							 sub_group_param_t sub_group_id, uint8_t data_atribute_num );

		static bool m_sub_ind_flag;	///< Subscribe indication flag
		static char m_cb_ref[CB_IDENT_SIZE_GOOSE];	///< CB refernce name

	private:

		/**
		 * @brief Copy Constructor. Definitions made private to disallow usage
		 * @param[in] object CIEC61850_GOOSE_DCI_Manager class object
		 * @return Void: None
		 */
		CIEC61850_GOOSE_DCI_Manager( const CIEC61850_GOOSE_DCI_Manager& object );

		/**
		 * @brief Copy Assignment Operator definitions made private to disallow usage
		 * @param[in] object CIEC61850_GOOSE_DCI_Manager class object
		 * @return None
		 */
		CIEC61850_GOOSE_DCI_Manager & operator =( const CIEC61850_GOOSE_DCI_Manager& object );

		/**
		 *  @brief Constructor
		 *  @details Creates a CIEC61850_GOOSE_DCI_Manager
		 *  @param[in] goose_interface: pointer to CIEC61850_GOOSE_Interface class's object.
		 *  @param[in] unique_id: A source ID for the patron access.
		 */
		CIEC61850_GOOSE_DCI_Manager( CIEC61850_GOOSE_Interface* goose_interface );

		/**
		 * @brief   Instance of the singleton class.
		 */
		static CIEC61850_GOOSE_DCI_Manager* m_dci_manager;

		/**
		 * @brief Provides the access to stop the GOOSE communication
		 * @return None
		 */
		void Stop( void );

		/**
		 * @brief Provides the access to Delete the GOOSE communication and
		 * @n frees the memory allocated for the GOOSE configuration paraneters
		 * @return None
		 */
		void Delete( void );

		/**
		 *  @brief Updates the GOOSE publish user configuration
		 *  @param[in] user_config: structure pointer to the which caries the user configuration info
		 *  @param[in] group_index: Publish group ID inofrmation
		 *  @return Void: None
		 */
		void Get_User_Config_Pub( user_config_t* user_config, group_param_t group_index );

		/**
		 *  @brief Creates the DCI_Owner for GOOSE publish configuration parameters
		 *  @return DCI_ERROR_TD : Error code return while retriving the data from the memory
		 */
		DCI_ERROR_TD Create_Pub_Config_Dci_Owners( void );

		/**
		 *  @brief Creates the DCI_Owner for GOOSE subscribe configuration parameters
		 *  @return DCI_ERROR_TD : Error code return while retriving the data from the memory
		 */
		DCI_ERROR_TD Create_Sub_Config_Dci_Owners( void );

		/**
		 *  @brief Updates the GOOSE subscrber user configuration
		 *  @param[in] user_config: structure pointer to the which caries the user configuration info
		 *  @param[in] group_index: Publish group ID inofrmation
		 *  @return DCI_ERROR_TD : Error code return while retriving the data from the memory
		 */
		void Get_User_Config_Sub( user_config_t* user_config, group_param_t group_index );

		/**
		 *  @brief Gives the array index of DCI owner
		 *  @param[in] message_type: Message type information
		 *  @param[in] group_id: GOOSE message group ID inofrmation
		 *  @param[in] sub_group_id: GOOSE message Sub group ID inofrmation
		 *  @param[in] data_atribute_num: GOOSE message data atribute number inofrmation
		 *  @return Void : None
		 */
		void Get_Owner_Array_Index( uint8_t message_type, group_param_t group_id,
									sub_group_param_t sub_group_id, uint16_t* index,  uint8_t data_atribute_num );

		/**
		 *  @brief Prints the DCI checked in Dataused for demo purpose
		 *  @param[in] tag_type: GOOSE message tag type inofrmation
		 *  @param[in] dcid: DCI ID whose value to be printed on consol
		 *  @return Void : None
		 */
		void Print_DCI_Checked_Data( iec61850_tagtype_t tag_type, DCI_ID_TD dcid );

		/**
		 * @brief pointer to DCI_Patron class
		 */
		DCI_Patron* m_dci_access;

		/**
		 * @brief DCI Patron Source ID
		 */
		DCI_SOURCE_ID_TD m_source_id;

		/**
		 * @brief pointer to IEC61850_GOOSE_DCI class to get the set and get access of GOOSE configuration parameters
		 */
		IEC61850_GOOSE_DCI* m_goose_dci;

		/**
		 * @brief GOOSE Publish message indication flag
		 */
		bool m_pub_ind_flag;

		/**
		 * @brief Carries the DCI ID which got updated internally which initiaites the publish callback
		 */
		DCI_ID_TD m_data_id;

		static const uint8_t DISABLE_VALUE = 0U;	///< Variable used to initialize the value to zero
		static const uint8_t MAX_MAC_ADDRESS_SIZE = 6U;	///< Carries the maximum mac address array size

		/**
		 * @brief Array for a GOOSE publish configuration parameter
		 */
		uint8_t m_pub_config_array[MAX_CONFIG_ARRAY_SIZE];

		/**
		 * @brief Array for a GOOSE Subscribe configuration parameter
		 */
		uint8_t m_sub_config_array[MAX_CONFIG_ARRAY_SIZE];

		/**
		 * @brief Array index for the GOOSE Publish configuration parameter array
		 */
		uint16_t m_pub_array_index;

		/**
		 * @brief Array index for the GOOSE subscribe configuration parameter array
		 */
		uint16_t m_sub_array_index;

		/**
		 * @brief a pointer to CIEC61850_GOOSE_Interface class
		 */
		CIEC61850_GOOSE_Interface* m_goose_interface;

		/**
		 * @brief array which carries publish goose message group length details
		 */
		group_len_details_t m_pub_group_index[MAX_GOOSE_PUB_GROUP];

		/**
		 * @brief array which carries Subscribe goose message group length details
		 */
		group_len_details_t m_sub_group_index[MAX_GOOSE_SUB_GROUP];

		/**
		 * @brief Array of DCI_Owner for all GOOSE publish groups parameters
		 */
		DCI_Owner* m_pub_group_owner[MAX_GOOSE_PUB_GROUP][MAX_NUM_GOOSE_PUB_MSG];

		/**
		 * @brief Array of DCI_Owner for all GOOSE publish groups parameters
		 */
		DCI_Owner* m_sub_group_owner[MAX_GOOSE_SUB_GROUP][MAX_NUM_GOOSE_SUB_MSG];

		/**
		 * @brief Pointer to get individual RAM access
		 */
		uint8_t* m_individual_readvalue;
};

#endif
