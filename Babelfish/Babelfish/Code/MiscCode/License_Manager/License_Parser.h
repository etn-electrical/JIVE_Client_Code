/**
 *****************************************************************************************
 *	@file License_Parser.h
 *
 *	@brief Abstract class for license token parser.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LICENSE_PARSER_H
	#define LICENSE_PARSER_H

#include "Includes.h"

namespace BF_Misc
{

/**
 **************************************************************************************************
 *  @brief Abstract class for license parser
 **************************************************************************************************
 */
class License_Parser
{
	public:
		typedef void* license_t;
		typedef uint16_t license_length_t;

		/**
		 * @brief Validation status of received token
		 */
		enum flag_t
		{
			NOT_AVAILABLE,
			AVAILABLE
		};

		/**
		 * @brief Validation status of received token
		 */
		enum token_status_t
		{
			INVALID,
			VALID
		};

		/**
		 * @brief License command
		 */
		enum command_t
		{
			DISALLOW,
			ALLOW
		};

		/**
		 * @brief License model type
		 */
		enum model_type_t
		{
			NONE,
			FREE,
			EXPIRY,
			METERED
		};

		/**
		 * @brief License models
		 */
		union model_t
		{
			struct expiry_based
			{
				// TODO
				// Unix EPOC time
			} expiry;

			struct usage_based
			{
				// TODO
				// Counter
			} usage;
		} model;

		/**
		 * @brief Token will get transformed into license
		 */
		struct license_data_t
		{
			flag_t flag;
			command_t command;
			model_type_t model_type;
			model_t model;
		};

		/**
		 *  @brief Destructor
		 *  @details It will get called when object NV_Ctrl goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~License_Parser( void )
		{}

		/**
		 *  @brief Verify interface will validate received token
		 *  @param[in] token: token received from external entity
		 *  @param[in] token_length: length of token
		 *  @return validation status
		 */
		virtual token_status_t Verify( license_t token, license_length_t token_length ) = 0;

		/**
		 *  @brief Responsible for parsing received tokens
		 *  @param[in] token: token received from external entity
		 *  @param[in] token_length: length of token
		 *  @param[out] license: parsed information will get updated here
		 *  @return Parsing status
		 */
		virtual bool Parse( license_t token, license_length_t token_length, license_data_t* license ) = 0;

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via child class
		 */
		License_Parser()
		{}

	private:
};



}	// end of namespace::BF_Misc
#endif	// End of LICENSE_PARSER_H
