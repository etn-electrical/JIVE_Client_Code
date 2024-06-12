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
#ifndef BOOLEAN_LICENSE_PARSER_H
	#define BOOLEAN_LICENSE_PARSER_H

#include "Includes.h"
#include "License_Parser.h"

namespace BF_Misc
{

/**
 **************************************************************************************************
 *  @brief Abstract class for license parser
 **************************************************************************************************
 */
class Bool_License_Parser : public License_Parser
{
	public:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via child class
		 */
		Bool_License_Parser( void );
		/**
		 *  @brief Destructor
		 *  @details It will get called when object NV_Ctrl goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Bool_License_Parser( void )
		{}

		/**
		 *  @brief Verify interface will validate received token
		 *  @param[in] token: token received from external entity
		 *  @param[in] token_length: length of token
		 *  @return validation status
		 */
		virtual token_status_t Verify( license_t token, license_length_t token_length );

		/**
		 *  @brief Responsible for parsing received tokens
		 *  @param[in] token: token received from external entity
		 *  @param[in] token_length: length of token
		 *  @param[out] license: parsed information will get updated here
		 *  @return Parsing status
		 */
		virtual bool Parse( license_t token, license_length_t token_length, license_data_t* license );

	private:
};



}	// end of namespace::BF_Misc
#endif	// End of BOOLEAN_LICENSE_PARSER_H
