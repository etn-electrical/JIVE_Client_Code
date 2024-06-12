/**
 **************************************************************************************************
 *  @file
 *	@brief This file provides APIs to get language information.
 *
 *	@details
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 **************************************************************************************************
 */
#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "Language_Config.h"
#include "NV_Ctrl.h"

class Language
{
	public:

		/* Structure to hold specifications of language (Language header) */
		struct lg_spec_struct_t
		{
			uint8_t spec_version;	///< Version of language specification
			uint8_t id;				///< Language ID
			uint8_t format;			///< Language format e.g Jason, XML
			uint8_t compression;	///< 0:no compression, 1:method 1, 2:method 2
			uint8_t code[LG_CODE_MAX_BYTES];///< Language code e.g. "en" for English, "de" for
			// German
			uint8_t name[LG_NAME_MAX_BYTES];///< Language name e.g  English, German
			uint32_t length;		///< Language block length excluding language header
			uint32_t next_lg_offset;		///< Next language address offset, '0' if not available
		};

		/* Structure to hold language specific metadata */
		struct lg_metadata
		{
			uint32_t lg_addr;	///< Language address
			uint8_t lg_id;	///< Language ID
		};

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members for Language class
		 * @param[in] nv_ctrl : NV control to access language pack storage memory
		 * @return None
		 */
		Language( NV_Ctrl* lg_nv_ctrl );

		/**
		 * @brief Get the total count of available languages
		 * @param[out] lg_cnt : Count of total languages
		 * @return True if language count is non-zero
		 */
		bool Get_Total_Lang_Count( uint8_t* lg_cnt );

		/**
		 * @brief Get the language ID associated with given language index
		 * @param[in] index : index should be less than total language count
		 * @param[out] id : Language ID
		 * @return True if language id is available at given index
		 */
		bool Get_Lang_Id( uint8_t index, uint8_t* id );

		/**
		 * @brief Check whether given language ID is valid or not
		 * @param[out] id : Language ID
		 * @return True if language ID is valid
		 */
		bool Is_Lang_Id_Valid( uint8_t id );

		/**
		 * @brief Get language header based on provided language ID
		 * @param[in] lg_id : Language ID
		 * @param[out] lg_header : Language header
		 * @return True if language header is read successfully else false
		 */
		bool Get_Lang_Header( uint8_t lg_id, lg_spec_struct_t* lg_header );

		/**
		 * @brief Read language block data from language pack storage space
		 * @param[out] data : A pointer to the buffer used to receive the data
		 * @param[in] addr : Start address of data to be read
		 * @param[in] offset : Language block address offset.
		 * @param[in] read_len: The number of bytes to be read.
		 * @return True if language ID is valid
		 */
		bool Lang_Read( uint8_t* data, uint32_t addr, uint32_t offset, uint32_t read_len );

		/**
		 * @brief Initialise the list of Language IDs and addresses i.e m_lg_list[]
		 * @return None
		 */
		void Language_Info_Init( void );

		/**
		 * @brief Validate specifications of language and get the next language address offset
		 * @param[in] addr : Start address of language header
		 * @param[in] total_mem_range : Total range of memory space available for storing language pack
		 * @param[out] offset : Next language address offset
		 * @return True if language spec are valid
		 */
		bool Validate_Lg_Spec( uint32_t addr, uint32_t* offset, uint32_t total_mem_range );

		/**
		 * @brief Get start address of language block (excluding language header)
		 * @param[in] id : Language ID
		 * @param[out] addr : Start address of language block
		 * @return True if id found in m_lg_list[]
		 */
		bool Get_Lang_Addr( uint32_t* addr, uint8_t id );

#if 0
		/**
		 * @brief Function to validate the language rest path based on language code
		 * @param[out] id : Language ID
		 * @param[in] data : Pointer to language rest path
		 * @param[in] length : Length language rest path
		 * @return True if id found in m_lg_list[]
		 */
		bool Match_Lg_Rest_Path( uint8_t* lg_id, const uint8_t* data, uint32_t length );

#endif
		/**
		 *  @brief Destructor
		 *  @details It will get called when object of Language goes out of scope or deleted.
		 *  @return None
		 */
		~Language( void );

	private:

		/**
		 * @brief Pointer to language pack
		 */
		lg_spec_struct_t* m_lg_ptr;

		/**
		 * @brief Total number of languages available in a language pack storage memory
		 */
		uint8_t m_lg_total;

		/**
		 * @brief Pointer to language metadata
		 */
		lg_metadata* m_lg_list;

		/**
		 * @brief NV control to access language pack storage memory
		 */
		NV_Ctrl* m_lg_nv_ctrl;
};

#endif	// LANGUAGE_H
