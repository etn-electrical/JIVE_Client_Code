/**
 **********************************************************************************************
 *	@file            Boolean License_Parser.cpp
 *
 *	@brief           This object will parse boolean license tokens
 *	@copyright       2021 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Babelfish_Assert.h"
#include "Bool_License_Parser.h"
#include "StdLib_MV.h"

namespace BF_Misc
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Bool_License_Parser::Bool_License_Parser( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
License_Parser::token_status_t Bool_License_Parser::Verify( license_t token, license_length_t token_length )
{
	token_status_t status = License_Parser::VALID;

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Bool_License_Parser::Parse( license_t token, license_length_t token_length, license_data_t* license )
{
	bool status = true;

	bool* boolean_token = reinterpret_cast<bool*>( token );

	if ( *boolean_token > 0 )
	{
		license->command = License_Parser::ALLOW;
	}
	else
	{
		license->command = License_Parser::DISALLOW;
	}

	license->flag = License_Parser::AVAILABLE;
	license->model_type = License_Parser::NONE;

	// Clear model field as feature do not support any model type
	BF_Lib::Copy_Val_To_String( reinterpret_cast<uint8_t*>( &license->model ), 0, sizeof( license->model_type ) );

	return ( status );
}

}	// End of namespace BF_Misc
