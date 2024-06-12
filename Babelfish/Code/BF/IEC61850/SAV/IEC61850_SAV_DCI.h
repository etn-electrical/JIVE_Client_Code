/**
 *****************************************************************************************
 *  @file
 *	@brief It provides the interface between IEC61850 protocol and DCI database
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IEC61850_SAV_DCI_H
#define IEC61850_SAV_DCI_H

#include "Includes.h"
#include "DCI_Patron.h"
#include "IEC61850_Defines.h"

/*
 **************************************************************************************************
 *  Class Declaration
 **************************************************************************************************
 */
class IEC61850_SAV_DCI
{
	public:
		typedef uint16_t group_param_t;	///< IEC61850 SAV group param ID

		typedef uint16_t sub_group_param_t;	///< IEC61850 SAV Sub group param ID

		static const uint8_t MAX_GROUP_PARAMETERS = 30U;	///< Maximum group and subgroup array size
		/**
		 * @brief SAV Message types details
		 */
		enum message_type_t
		{
			PUBLISH,
			SUBSCRIBE
		};

		/**
		 * @brief SAV Message sub group assembly details
		 */
		struct subgroup_assembly_t
		{
			const sub_group_param_t* subgroup_param_id;	///< Sub group Parameters
			const iec61850_tagtype_t* subgroup_param_tag_id;	///< Sub group parameters tag ID
			sub_group_param_t subgroup_total_params;	///< Total sub group parameters
		};

		/**
		 * @brief SAV Message main group assembly details
		 */
		struct group_assembly_t
		{
			group_param_t group_dcid;								///< Main Group ID deatils
			const char_t* sav_id;									///< SAV ID details
			const group_param_t* maingroup_param_id;				///< Main group parameters
			const subgroup_assembly_t* subgroup_array;				///< Sub group parameters
			sub_group_param_t total_subgroups;						///< Total sub group count
			group_param_t maingroup_total_params;					///< Total main group parameters count
		};

		/**
		 * @brief Container for the entire identity of a IEC61850 SAV Publish or subscribe messages
		 */
		struct sav_group_info_t
		{
			group_param_t total_group;	///< Total group count deatils
			const group_assembly_t* dci_group_struct;	///< Pointer to IEC61850 publish or subscribe message groups
		};

		/**
		 * @brief Container for the entire identity of a IEC61850 SAV Message
		 */
		struct sav_message_info_t
		{
			const sav_group_info_t* pub_group_profile;	///< Pointer to publish group
			const sav_group_info_t* sub_group_profile;	///< Pointer to Subscribe group
		};

		/**
		 *  @brief Constructor
		 *  @details Creates a IEC61850_SAV_DCI
		 *  @param[in] sav_message_info , pointer to access IEC61850 SAV message continer
		 *  @param[in] unique_id , A source ID for the patron access.
		 */
		IEC61850_SAV_DCI( const sav_message_info_t* sav_message_info, DCI_SOURCE_ID_TD unique_id );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of IEC61850_SAV_DCI goes out of scope
		 *  @return None
		 */
		~IEC61850_SAV_DCI( void );

		/**
		 *  @brief Provides access to the Parameters with the passed IEC61850 unique group ID.
		 *  @param[in] group_id, Parameter whose length should be returned
		 *  @param[in] message_type, message type details
		 *  @param[out] length_ptr,Pointer to location where the data length
		 *  @n will be written. Used outside function to index data block for multiple gets
		 *  @param[out] dest_data_ptr, Pointer to starting location where data will be placed
		 *  @return iec61850_error_t , Error code return while retriving the data from the memory
		 */
		iec61850_error_t Get_Group_Assembly_Parameters( group_param_t group_id, DCI_LENGTH_TD* length_ptr,
														uint8_t* dest_data_ptr, uint8_t message_type );

		/**
		 *  @brief Provides access to find the IEC61850 parameters group ID
		 *  @param[in] dcid, Parameter whose group ID should be returned
		 *  @param[out] group_id group ID corresponds to IEC61850 parameter
		 *  @param[out] group_id group ID corresponds to IEC61850 parameter
		 *  @return iec61850_error_t , Error code return while retriving the data from the memory
		 */
		iec61850_error_t Get_Group_Id_Details( DCI_ID_TD dcid, group_param_t* group_id, sub_group_param_t* sub_group_id,
											   uint8_t message_type );

		/**
		 *  @brief Provides access to the Parameters with the passed IEC61850 unique group ID and Sub group ID.
		 *  @param[in] group_id, Parameter whose length should be returned
		 *  @param[in] sub_group_id, Parameter whose length should be returned
		 *  @param[in] message_type, message type details
		 *  @param[out] length_ptr,Pointer to location where the data length
		 *  @n will be written. Used outside function to index data block for multiple gets
		 *  @param[out] dest_data_ptr, Pointer to starting location where data will be placed
		 *  @return iec61850_error_t , Error code return while retriving the data from the memory
		 */
		iec61850_error_t Get_Subgroup_Assembly_Parameters( group_param_t group_id, sub_group_param_t sub_group_id,
														   DCI_LENGTH_TD* length_ptr, uint8_t* dest_data_ptr,
														   uint8_t message_type );

		/**
		 *  @brief Provides the dataype of all sub group parameters
		 *  @param[in] group_id, Parameter whose length should be returned
		 *  @param[in] sub_group_id, Parameter whose length should be returned
		 *  @param[in] message_type, message type details
		 *  @param[out] dest_data_ptr, Pointer to starting location where data will be placed
		 *  @return iec61850_error_t , Error code return while retriving the data from the memory
		 */
		iec61850_error_t Get_Subgroup_Param_Length( group_param_t group_id, sub_group_param_t sub_group_id,
													DCI_LENGTH_TD* length_ptr, uint8_t message_type );

		/**
		 *  @brief Provides the length of all main group parameters
		 *  @param[in] group_id, Parameter whose length should be returned
		 *  @param[in] message_type, message type details
		 *  @param[out] length_ptr, Pointer to starting location where data will be placed
		 *  @return iec61850_error_t , Error code return while retriving the data from the memory
		 */
		iec61850_error_t Get_Maingroup_Params_Length( group_param_t group_id, DCI_LENGTH_TD* length_ptr,
													  uint8_t message_type );

		/**
		 *  @brief Provides access to the set the sub group Parameters with the passed IEC61850 unique group
		 * 	@n ID and Sub group ID.
		 *  @param[in] group_id, Parameter whose length should be returned
		 *  @param[in] sub_group_id, Parameter whose length should be returned
		 *  @param[in] message_type, message type details
		 *  @param[out] length_ptr,Pointer to location where the data length
		 *  @n will be written. Used outside function to index data block for multiple gets
		 *  @param[out] src_data_ptr, Pointer to starting location where data will be
		 *  @n read to set the suub group DCI IDs
		 *  @return iec61850_error_t , Error code return while retriving the data from the memory
		 */
		iec61850_error_t Set_SubGroup_Assembly_Parameters( group_param_t group_id, sub_group_param_t sub_group_id,
														   uint8_t* src_data_ptr, uint8_t message_type,
														   DCI_LENGTH_TD* length_ptr );

	private:

		/**
		 * @brief Copy Constructor. Definitions made private to disallow usage
		 * @param[in] object IEC61850_SAV_DCI class object
		 * @return None
		 */
		IEC61850_SAV_DCI( const IEC61850_SAV_DCI& object );

		/**
		 * @brief Copy Assignment Operator definitions made private to disallow usage
		 * @param[in] object IEC61850_SAV_DCI class object
		 * @return None
		 */
		IEC61850_SAV_DCI & operator =( const IEC61850_SAV_DCI& object );

		/**
		 *  @brief Provides "GET" data access to single IEC61850_SAV_DCI parameter.
		 *  @details This Function is used to retrieve the ram data from the DCI
		 *  @param[in] dcid, dcid for which data will be retrieved
		 *  @param[out] length_ptr,Pointer to location where the data length
		 *  @n will be written. Used outside function to index data block for multiple gets
		 *  @param[out] dest_data_ptr,  Pointer to starting location where data will placed
		 *  @return DCI_ERROR_TD , Error code return while retriving the data from the memory
		 */
		DCI_ERROR_TD Get_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD* length_ptr,
									uint8_t* dest_data_ptr );

		/**
		 *  @brief Provides "SET" data access to single IEC61850 unique configuration id. Sets only RAM value
		 *  @details This Function is used to  write ram data to DCID for a single IEC61850 unique configuration id.
		 *  @param[in] dcid for which data will be written from the DCI
		 *  @param[in] length, Data length of data (0) = Write all data
		 *  @param[in] src_data_ptr, Pointer to starting location where data to be written is located
		 *  @return Dci error, Error code return while writing the data to the memory
		 */
		DCI_ERROR_TD Set_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD length, uint8_t* src_data_ptr );

		/**
		 * @brief Variable which carries DCI source ID information
		 */
		DCI_SOURCE_ID_TD m_source_id;

		/**
		 * @brief Pointer to DCI patron to get patron access functionalities.
		 */
		DCI_Patron* m_dci_access;

		/**
		 * @brief Pointer to SAV message continer
		 */
		const sav_message_info_t* m_sav_message_info;

		/**
		 * @brief Pointer to get individual RAM access
		 */
		uint8_t* m_individual_readvalue;

		const uint16_t IEC61850_PACKET_PAYLOAD = 256U;					///< IEC61850 packet payload value
		const sub_group_param_t IEC61850_SAV_INVALID_SUBGROUP_ID = 256U;	///< IEC61850 SAV invalid subgroup ID
		const uint8_t DISABLE_VALUE = 0U;								///< Used to intialize variables with 0U
};

#endif
