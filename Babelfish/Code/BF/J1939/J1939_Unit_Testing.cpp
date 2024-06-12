/**
 **********************************************************************************************
 *	@file            J1939_Unit_Testing.cpp
 *
 *	@brief			 Unit testing of J1939 DCI integration functionality
 *
 *
 *	@details
 *	@copyright 		 2019 Eaton Corporation. All Rights Reserved.
 *  @remark          Eaton Corporation claims proprietary rights to the material disclosed
 *                   hereon.  This technical information may not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#include "Includes.h"
#include "J1939_DCI.h"
#include "J1939_Msg.h"		// #include "j1939.h"
#include "Ram.h"
#include "stdio.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Unit_Test_J1939_Msg()
{
	j1939_t* j1939_msg = nullptr;

	j1939_msg = reinterpret_cast<j1939_t*>( Ram::Allocate( sizeof( j1939_t ) ) );
	j1939_msg->buf = reinterpret_cast<uint8_t*>( Ram::Allocate( 8U ) );

	J1939_DCI::j1939_error_codes_t return_value_test = J1939_DCI::NO_ERROR;
	static J1939_DCI* j1939_interface = new J1939_DCI();


	/*  ---------  test case 1:  ---------  */

	j1939_msg->pgn = 1001;
	/* Prepare J1939 message */
	j1939_msg->buf[0] = 0xA5;
	j1939_msg->buf[1] = 0xA5;
	j1939_msg->buf[2] = 0xA5;
	j1939_msg->buf[3] = 0xA5;
	j1939_msg->buf[4] = 0xA5;
	j1939_msg->buf[5] = 0xA5;
	j1939_msg->buf[6] = 0xA5;
	j1939_msg->buf[7] = 0xA5;

	return_value_test = j1939_interface->Set_J1939_Message( j1939_msg );
	if ( return_value_test != J1939_DCI::NO_ERROR )
	{
		printf( "Error in updating DCIDs for PGN : 1001 \n" );
	}

	/* Clear J1939 message */
	j1939_msg->buf[0] = 0x00;
	j1939_msg->buf[1] = 0x00;
	j1939_msg->buf[2] = 0x00;
	j1939_msg->buf[3] = 0x00;
	j1939_msg->buf[4] = 0x00;
	j1939_msg->buf[5] = 0x00;
	j1939_msg->buf[6] = 0x00;
	j1939_msg->buf[7] = 0x00;

	return_value_test = j1939_interface->Get_J1939_Message( j1939_msg );

	if ( return_value_test != J1939_DCI::NO_ERROR )
	{
		printf( "Error in creating message with PGN : 1001 \n" );
	}

	if ( ( j1939_msg->buf[0] == 0xA5 ) && ( j1939_msg->buf[1] == 0xA5 ) &&
		 ( j1939_msg->buf[2] == 0xA5 ) && ( j1939_msg->buf[3] == 0xA5 ) &&
		 ( j1939_msg->buf[4] == 0xA5 ) && ( j1939_msg->buf[5] == 0xA5 ) &&
		 ( j1939_msg->buf[6] == 0xA5 ) && ( j1939_msg->buf[7] == 0xA5 ) )
	{
		printf( "\n Test case 1 Pass \n" );
	}
	else
	{
		printf( "\n Test case 1 Fail \n " );
	}

	/*  ---------  test case 2:  ---------  */

	j1939_msg->pgn = 1002;
	/* Prepare J1939 message */
	j1939_msg->buf[0] = 0xA5;
	j1939_msg->buf[1] = 0xF5;
	j1939_msg->buf[2] = 0xAF;
	j1939_msg->buf[3] = 0xA5;
	j1939_msg->buf[4] = 0xA5;
	j1939_msg->buf[5] = 0xF5;
	j1939_msg->buf[6] = 0xAF;
	j1939_msg->buf[7] = 0xA5;

	return_value_test = j1939_interface->Set_J1939_Message( j1939_msg );
	if ( return_value_test != J1939_DCI::NO_ERROR )
	{
		printf( "Error in updating DCIDs for PGN : 1002 \n" );
	}

	/* Clear J1939 message */
	j1939_msg->buf[0] = 0x00;
	j1939_msg->buf[1] = 0x00;
	j1939_msg->buf[2] = 0x00;
	j1939_msg->buf[3] = 0x00;
	j1939_msg->buf[4] = 0x00;
	j1939_msg->buf[5] = 0x00;
	j1939_msg->buf[6] = 0x00;
	j1939_msg->buf[7] = 0x00;

	return_value_test = j1939_interface->Get_J1939_Message( j1939_msg );

	if ( return_value_test != J1939_DCI::NO_ERROR )
	{
		printf( "Error in creating message with PGN : 1002 \n" );
	}

	if ( ( j1939_msg->buf[0] == 0xA5 ) && ( j1939_msg->buf[1] == 0xF5 ) &&
		 ( j1939_msg->buf[2] == 0xAF ) && ( j1939_msg->buf[3] == 0xA5 ) &&
		 ( j1939_msg->buf[4] == 0xA5 ) && ( j1939_msg->buf[5] == 0xF5 ) &&
		 ( j1939_msg->buf[6] == 0xAF ) && ( j1939_msg->buf[7] == 0xA5 ) )
	{
		printf( " Test case 2 Pass \n " );
	}
	else
	{
		printf( " Test case 2 Fail \n " );
	}

	/*  ---------  test case 3:  ---------  */

	j1939_msg->pgn = 1003;
	/* Prepare J1939 message */
	j1939_msg->buf[0] = 0xFF;
	j1939_msg->buf[1] = 0xA5;
	j1939_msg->buf[2] = 0xA5;
	j1939_msg->buf[3] = 0x0F;
	j1939_msg->buf[4] = 0xF0;
	j1939_msg->buf[5] = 0xA5;
	j1939_msg->buf[6] = 0xA5;
	j1939_msg->buf[7] = 0x00;

	return_value_test = j1939_interface->Set_J1939_Message( j1939_msg );
	if ( return_value_test != J1939_DCI::NO_ERROR )
	{
		printf( "Error in updating DCIDs for PGN : 1003 \n" );
	}

	/* Clear J1939 message */
	j1939_msg->buf[0] = 0x00;
	j1939_msg->buf[1] = 0x00;
	j1939_msg->buf[2] = 0x00;
	j1939_msg->buf[3] = 0x00;
	j1939_msg->buf[4] = 0x00;
	j1939_msg->buf[5] = 0x00;
	j1939_msg->buf[6] = 0x00;
	j1939_msg->buf[7] = 0x00;

	return_value_test = j1939_interface->Get_J1939_Message( j1939_msg );

	if ( return_value_test != J1939_DCI::NO_ERROR )
	{
		printf( "Error in creating message with PGN : 1003 \n" );
	}

	if ( ( j1939_msg->buf[0] == 0xFF ) && ( j1939_msg->buf[1] == 0xA5 ) &&
		 ( j1939_msg->buf[2] == 0xA5 ) && ( j1939_msg->buf[3] == 0x0F ) &&
		 ( j1939_msg->buf[4] == 0xF0 ) && ( j1939_msg->buf[5] == 0xA5 ) &&
		 ( j1939_msg->buf[6] == 0xA5 ) && ( j1939_msg->buf[7] == 0x00 ) )
	{
		printf( " Test case 3 Pass \n " );
	}
	else
	{
		printf( " Test case 3 Fail \n " );
	}

	return ( 1 );
}

/*
   test case 1: random length of dcids(created the pgn 1001 for this case)
   test case 2: when bit length is more then 8bits.(pgn 1002 created for this case)
   test case 3: all 32 bit length dcids

 */
