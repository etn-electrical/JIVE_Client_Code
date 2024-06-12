/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Delta_Com.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com::Delta_Com()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com::~Delta_Com()
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Delta_Com::Clear_Command_Byte( uint8_t command_byte )
{
	if ( command_byte > 0U )
	{
		command_byte = 0U;
	}
	return ( 0U );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Delta_Com::Is_Fragmented( uint8_t command_byte )
{
	bool result;

	if ( ( command_byte & FRAGMENTATION_MASK ) != 0x00U )
	{
		result = true;
	}
	else
	{
		result = false;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Delta_Com::Set_Fragmented( uint8_t command_byte )
{
	return ( command_byte | FRAGMENTATION_MASK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com::cmd_type_t Delta_Com::Get_Command_Type( uint8_t command_byte )
{
	uint8_t command_process;

	command_process = ( command_byte & COMMAND_TYPE_ENUMERATION_MASK ) >> COMMAND_TYPE_OFFSET;

	return ( static_cast<cmd_type_t>( command_process ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Delta_Com::cmd_t Delta_Com::Get_Command( uint8_t command_byte )
{
	uint8_t command_process;

	command_process = command_byte & COMMAND_ENUMERATION_MASK;

	return ( static_cast<cmd_t>( command_process ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Delta_Com::Set_Command_Type( uint8_t command_byte, cmd_type_t command_type )
{
	uint8_t command_type_mask;
	uint8_t return_cmd_byte;

	command_type_mask = static_cast<uint8_t>( command_type );

	return_cmd_byte = command_byte & COMMAND_TYPE_UMASK;
	return_cmd_byte |= static_cast<uint8_t>( command_type_mask << COMMAND_TYPE_OFFSET );

	return ( return_cmd_byte );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t Delta_Com::Set_Command( uint8_t command_byte, cmd_t command )
{
	uint8_t cmd_type_mask;
	uint8_t return_cmd_byte;

	cmd_type_mask = static_cast<uint8_t>( command );

	return_cmd_byte = command_byte & COMMAND_ENUMERATION_UMASK;
	return_cmd_byte |= cmd_type_mask;

	return ( return_cmd_byte );
}
