/**
 *****************************************************************************************
 *  @file
 *	@brief It provides the interface between Ble protocol and DCI database
 *
 *	@details Ble protocol access the Ble unique characterisitic ID's & the Ble unique char id
 *  @ are linked to DCI database (DCI - Data store)
 *
 *	@copyright 2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef BLE_DCI_H
#define BLE_DCI_H

#include "includes.h"
#include "DCI_Patron.h"
#include "Ble_Defines.h"

/*
 **************************************************************************************************
 *  Class Declaration
 **************************************************************************************************
 */
class Ble_DCI
{
	public:
		typedef uint16_t CHAR_ID_TD;
		DCI_SOURCE_ID_TD m_source_id;
		DCI_Patron* m_dci_access;
		uint8_t* m_individual_readvalue;

		// This structure defines the Ble UUID to DCID lookup.
		struct dci_ble_to_dcid_lkup_st_td_t
		{
			const CHAR_ID_TD ble_char_id;
			const DCI_ID_TD dcid;
			const uint8_t read;
			const uint8_t write;
#ifdef STM32WB55_WPAN
			const uint8_t ble_attrib;
			const uint8_t service_uuid[16U];
			const uint8_t char_uuid[16U];
#endif
		};

		// Defines the ble target. This is a container for the entire identity of a Ble Interface.
		struct ble_target_info_st_td_t
		{
			CHAR_ID_TD total_char_ids;
			const dci_ble_to_dcid_lkup_st_td_t* ble_to_dcid_struct;
		};

		// This structure defines the ble Assembly Groups.
		struct ble_group_assembly_t
		{
			CHAR_ID_TD total_parameters;
			CHAR_ID_TD group_dcid;
			CHAR_ID_TD ble_dcid[32];
		};

		// Defines the ble group. This is a container for the entire identity of a Ble group.
		struct ble_group_info_st_td_t
		{
			CHAR_ID_TD total_group_id;
			const ble_group_assembly_t* ble_dci_group_struct;
		};

		/**
		 *  @brief Constructor
		 *  @details Creates a Ble DCI
		 *  @param[in] ble_dev_profile , pointer to access data in the target structure.
		 *  @param[in] unique_id , A source ID for the patron access.
		 */
		Ble_DCI( const ble_target_info_st_td_t* ble_dev_profile, const ble_group_info_st_td_t* ble_group_profile,
				 DCI_SOURCE_ID_TD unique_id );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Ble_DCI goes out of scope
		 *  @return None
		 */
		~Ble_DCI( void );

		/**
		 *  @brief            Provides "GET" data access to single BLE_DCI parameter.
		 *  @details          This Function is used to retrieve the ram data from the DCI
		 *  @param[in]        dcid, dcid for which data will be retrieved
		 *  @param[out]		  length_ptr,Pointer to location where the data length
		 *  will be written. Used outside function to index data block for multiple gets
		 *  @param[out]       dest_data_ptr,  Pointer to starting location where data will be
		 *  placed
		 *  @param[in]        offset, offset of data array
		 *  @return          DCI_ERROR_TD , Error code return while retriving the data from the
		 *  memory
		 */
		DCI_ERROR_TD Get_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD* length_ptr,
									uint8_t* dest_data_ptr, DCI_LENGTH_TD offset = 0U );

		/**
		 *  @brief            Provides "GET" data access to single dcid.
		 *  @details          This Function is used to retrieve the ram data from the DCI
		 *  @n                for a single BLE unique characteristic id.
		 *  @param[in]        ble_unique_id, Ble id for which data will be retrieved from the DCI
		 *  @param[out]		  length_ptr,Pointer to location where the data length
		 *  will be written. Used outside function to index data block for multiple gets
		 *  @param[out]       dest_data_ptr, Pointer to starting location where data will be
		 *  placed
		 *  @return          DCI_ERROR_TD , Error code return while retriving the data from the
		 *  memory
		 */
		DCI_ERROR_TD Get_Char_Id_Data( DCI_ID_TD dcid, DCI_LENGTH_TD* length_ptr,
									   uint8_t* dest_data_ptr );

		/**
		 *  @brief            Provides "SET" data access to single BLE unique characteristic id.
		 *	@n                Sets only RAM value
		 *  @details          This Function is used to  write ram data to DCID
		 *  @n                for a single BLE unique characteristic id.
		 *  @param[in]		  dcid for which data will be written
		 *	from the DCI
		 *  @param[in]        length, Data length of data (0) = Write all data
		 *  @param[in]		  src_data_ptr, Pointer to starting location where data to be
		 *  written is located
		 *  @return           Dci error, Error code return while writing the data to the
		 *  memory
		 */
		DCI_ERROR_TD Set_Dcid_Data( DCI_ID_TD dcid, DCI_LENGTH_TD length,
									uint8_t* src_data_ptr );

		/**
		 *  @brief          Interprets the DCID errors to the BGLIB errors.
		 *  @param[in]		dci_error, The error code returning from the DCID get and set fucntion
		 *  @return         BLE error code as per the BGLIB SDK, Error code return while
		 *  reading/writing the data to the memory
		 */
		uint8_t Interpret_DCI_Error( DCI_ERROR_TD dci_error ) const;

		/**
		 *  @brief           Provides access to the Access Flags associated with the passed ble
		 *					 unique id.
		 *  @param[in]		 ble_unique_id, Parameter whose length should be returned
		 *  @param[out]      read_access - Read Access flag for the supplied unique Ble char id#
		 *  @param[out]      write_access - Write Access flag for the supplied unique Ble char id#
		 */
		DCI_ID_TD Find_Ble_Mod_DCID( CHAR_ID_TD ble_unique_id, uint8_t* read_access,
									 uint8_t* write_access );

		/**
		 *  @brief           Provides access to the Parameters with the passed ble unique id.
		 *  @param[in]		 ble_unique_id, Parameter whose length should be returned
		 *  @param[out]		 length_ptr,Pointer to location where the data length
		 *  will be written. Used outside function to index data block for multiple gets
		 *  @param[out]      dest_data_ptr, Pointer to starting location where data will be
		 *  placed
		 *  @param[out]		 active_access_level, current access level of the user
		 */
		DCI_ERROR_TD Group_Parameters_Assembly( CHAR_ID_TD ble_unique_id, DCI_LENGTH_TD* length_ptr,
												uint8_t* dest_data_ptr, uint8_t active_access_level );

		/**
		 *  @brief           Get the length of DCID.
		 *  @param[in]		 dcid, Parameter whose length should be returned
		 *  @param[out]		 data length
		 */
		DCI_LENGTH_TD Get_Length( DCI_ID_TD dcid ) const;

		const ble_target_info_st_td_t* m_dev_profile;
		const ble_group_info_st_td_t* m_group_profile;

	private:
		/**
		 * @brief                   Copy Constructor. Definitions made private to disallow usage
		 * @param[in] object        Ble_DCI class object
		 * @return
		 */
		Ble_DCI( const Ble_DCI& object );

		/**
		 * @brief                   Copy Assignment Operator.
		 *							definitions made private to disallow usage
		 * @param[in] object        Ble_DCI class object
		 * @return
		 */
		Ble_DCI & operator =( const Ble_DCI& object );

		/**
		 * @brief           Provides access to the DCID associated with the passed ble unique id.
		 * 0xFFFF means it was not found.
		 * @param[in]		ble_unique_id, Parameter whose length should be returned
		 * @return          dcid - DCI id# for the supplied unique Ble char id#
		 */
		DCI_ID_TD Find_Ble_Mod_DCID( CHAR_ID_TD ble_unique_id );


		const uint16_t BLE_PACKET_PAYLOAD = 256U;

};

#endif
