/**
 *****************************************************************************************
 *  @file
 *	@brief This file provides the DCI interface to IOT.
 *
 *	@details IOT_DCI contains all the necessary structures and methods to access the DCI interface.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef IOT_DCI_H
#define IOT_DCI_H
#include "DCI_Patron.h"

typedef uint32_t iot_tag_id_t;


/// This structure defines the IOT_DCI to DCID lookup for one Channel.
typedef struct dci_iot_to_dcid_lkup_st_t
{
	/** The numeric PX White "tag" (ie, ID) for this channel. */
	iot_tag_id_t channel_tag;
	/** Same "tag" expressed as a string, as used in the messages. */
	const char_t* channel_tag_str;
	/** DCI ID of this channel */
	DCI_ID_TD dcid;
	/** One or more IOT_GROUP_MARKERS that indicate the group(s) this
	 * channel belongs to. */
	uint16_t group_memberships;
	/** Optional array offset, for hypothetical array-based channels. */
	uint16_t array_offset;
} iot_channel_config_struct_t;

typedef struct iot_cadence_group_struct
{
	///< The DCI ID for the channel that gives us the cadence, in seconds.
	DCI_ID_TD cadence_dcid;
	///< One or more IOT_CH_PUB_TYPES that indicate the type of message to publish.
	uint16_t publication_types;
	///< Enum from IOT_GROUP_MARKERS that marks which group this is.
	uint16_t group_marker;
	///< Enum from IOT_PUBLISH_CONTROL that controls publishing all channels on connect.
	bool pub_all_on_connect;

} iot_cadence_group_struct_t;


/** This structure defines the IOT target in the DCI database.
 * This is the container for the list of all IOT Channels in the DCI.
 * Also contains the metadata for the Cadence Groups. */
struct iot_target_info_st_t
{
	uint16_t total_channels;
	const dci_iot_to_dcid_lkup_st_t* channel_list;
	uint16_t total_groups;
	const iot_cadence_group_struct_t* group_list;
};


/**
 ****************************************************************************************
 * @brief This is an IoT_DCI class
 * @details It provides an interface between tag_id and DCID.
 * @ingroup iot_dci_config
 *
 * @li It provides public methods to
 * @li 1. Initialize an IOT_DCI connection.
 * @li 2. Find the index of the channel.
 * @li 3. Find the total number of channels available.
 * @li 4. Find the data length associated with the DCID.
 * @li 5. Read the data value associated with the DCID.
 * @li 6. Write the data value associated with the DCID.
 ****************************************************************************************
 */
class IOT_DCI
{
	public:

		/** Define the types used for publishing channel data.
		 * SeeMe aka "realtimes" and StoreMe aka "trends". */
		enum IOT_CH_PUB_TYPES
		{
			PUB_TYPE_NONE = 0,
			PUB_TYPE_SEEME,				///< Publishes group channels with changes, using Realtimes message
			PUB_TYPE_STOREME,			///< Publishes group channels with changes, using Trends message
			PUB_TYPE_STOREMEONINTERVAL	///< Publishes all group channels, regardless of changes, using Trends message
		};

		/** Define the markers for the Cadence Group numbers.
		 * Since a channel may belong to more than one group,
		 * the values here must be by bit positions.
		 * Defining more group markers than we ever expect to see, but more can be added if needed.
		 */
		enum IOT_GROUP_MARKERS
		{
			CADENCE_GROUP_NONE = 0,		// Default, meaning none.
			CADENCE_GROUP0 = 0x01,
			CADENCE_GROUP1 = 0x02,
			CADENCE_GROUP2 = 0x04,
			CADENCE_GROUP3 = 0x08,
			CADENCE_GROUP4 = 0x10,
			CADENCE_GROUP5 = 0x20,
			CADENCE_GROUP6 = 0x40,
			CADENCE_GROUP7 = 0x80
		};

		/** Define the types to allow or restrict publishing all channels on establishing IOT connection */
		enum IOT_PUBLISH_CONTROL
		{
			PUBLISH_ALL_ON_CONNECT = 0,		///< Allow publishing all channels on establishing IOT connection
			DONT_PUBLISH_ALL_ON_CONNECT		///< Restrict publishing all channels on establishing IOT connection
		};


		/**
		 * @brief Constructor.
		 * @details Initialize an IOT_DCI connection by initializing a new Patron and source ID for
		 * that Patron.
		 * @param iot_profile: Defines the iot target, it is a container for the entire identity
		 * of the IOT_DCI interface.
		 * @param patron_ctrl: A patron interface to be used by the IoT interface.  This will
		 * allow us to provide a locking style DCI_Patron if necessary.
		 * @param unique_id:  This id will be used when the IoT DCI fills out the access struct
		 * when using the patron interface passed in.
		 * It is important that this number be unique.  Typically we fill it in with a random
		 * number.  It can be controlled by the adopter.
		 */
		IOT_DCI( const iot_target_info_st_t* iot_profile, DCI_Patron* patron_ctrl,
				 DCI_SOURCE_ID_TD unique_id );

		/** Default XTOR */
		~IOT_DCI( void );

		/**
		 * @brief Find the total number of channels available.
		 * @details This function returns the total number of channels available.
		 * @return number of channels.
		 */
		uint16_t Get_Total_Channels( void ) const;

		/**
		 * @brief Find the data length associated with the DCID.
		 * @details This function uses Get_Length method from the Patron class to get the length of
		 * the data.
		 * @param dcid: It is the id you want to retrieve.
		 * @return Length of the data.
		 */
		DCI_LENGTH_TD Get_Length( DCI_ID_TD dcid ) const;

		/**
		 * @brief Find the data length associated with the DCID.
		 * @details This function uses Get_Length method from the Patron class to get the length of
		 * the data.
		 * @param dcid: It is the id you want to retrieve.
		 * @return Length of the data.
		 */
		DCI_DATATYPE_TD Get_Datatype( DCI_ID_TD dcid ) const;

		/**
		 * @brief Read the data value.
		 * @details This function reads the data value associated with the DCID,
		 * 		extracted from the target database.
		 * @param tag_id: ID of the channel obtained form the cloud.
		 * @param[out] dest_data: The destination data pointer.
		 * @param[out] data_length: The length of the data to get.
		 * @return DCI error codes.
		 */
		DCI_ERROR_TD Get_Tag_Value( uint16_t tag_id, uint8_t* dest_data,
									uint16_t* data_length );

		/**
		 * @brief Read the data value for the given DCI ID.
		 * @details This function reads the data value associated with the DCID.
		 * @param dcid: The id of the parameter to retrieve.
		 * @param array_offset Offset into the value array, for hypothetical array-based channels.
		 * @param[out] dest_data: The destination data pointer.
		 * @param[out] data_length: The length of the data to get.
		 * @return DCI error codes.
		 */
		DCI_ERROR_TD Get_DCI_Value( DCI_ID_TD dcid, DCI_LENGTH_TD array_offset,
									uint8_t* dest_data, uint16_t* data_length );

		/**
		 * @brief Write the data value.
		 * @details This function writes the data value to the associated DCID.
		 * @param tag_id: ID of the channel obtained form the cloud.
		 * @param src_data: The source data pointer.
		 * @param data_length: The length of the data to set.
		 * @return DCI error codes.
		 */
		DCI_ERROR_TD Set_Tag_Value( uint16_t tag_id, uint8_t* src_data,
									uint16_t* data_length );

		/**
		 * @brief returns the IOT tag number in string format.
		 * @details This function returns the string format tag from IOT array index provided..
		 * @param tag_index: Tag index number.
		 * @return ID of the corresponding tag/channel number in string format.
		 */
		const char_t* Find_Tag_Str_From_Index( uint16_t tag_index );

		/**
		 * @brief returns the IOT array index number. from tag provided in string format.
		 * @details This function returns the IOT array index from tag provided in string format.
		 * @param tag_str: ID of the channel obtained from the cloud in string format.
		 * @param length The length of the string.
		 * @return Tag index number returns IOT_INPUT_ERROR if tag not found in database.
		 */
		uint16_t Find_Index_From_Tag_Str( const char_t* tag_str, uint8_t length );

		/**
		 * @brief returns the IOT array index number. from DCID provided
		 * @details This function returns the IOT array index from from DCID provided
		 * @param dcid: DCID of the channel obtained.
		 * @return Tag index number returns IOT_INPUT_ERROR if tag not found in database.
		 */
		uint16_t Find_Index_From_DCID( DCI_ID_TD dcid );

		/**
		 * @brief Get the IoT channel's configuration, given its index.
		 * @details This is a convenience function; looks up the channel structure in
		 * 			table iot_target_info_st_t::channel_list from the given index.
		 * 			Looking up the channel is a little awkward, since the iot_target_info_st_t
		 * 			doesn't tell the compiler that this is an array, hence this function.
		 * @param index The index of the desired channel in the channel_list array.
		 * @return Pointer to the structure with the channel's configuration, or nullptr if
		 * 			the index is out of range.
		 */
		const iot_channel_config_struct_t* Get_Channel_Config( uint16_t index ) const;

		/**
		 * @brief Get the IoT Cadence Group configuration, given its index.
		 * @details This is a convenience function; looks up the group structure in
		 * 			table iot_target_info_st_t::group_list from the given index.
		 * @param index The index of the desired channel in the group_list array.
		 * @return Pointer to the structure with the group's configuration, or nullptr if
		 * 			the index is out of range.
		 */
		const iot_cadence_group_struct_t* Get_Group_Config( uint16_t index ) const;

		/**
		 * @brief error code for unknown tag
		 */
		static const uint16_t IOT_INPUT_ERROR = 0xFFFFU;

	private:

		/**
		 * @brief Find the channel in the list, based on the tag index.
		 * @param tag_index: The index of the channel list for the channel of interest.
		 * @return address of the structure corresponding to the tag_index.
		 */
		const dci_iot_to_dcid_lkup_st_t* Find_Index_IOT( uint16_t tag_index );

		const iot_target_info_st_t* m_iot_struct;
		DCI_Patron* m_dci_access;
		DCI_SOURCE_ID_TD m_source_id;
};

#endif
