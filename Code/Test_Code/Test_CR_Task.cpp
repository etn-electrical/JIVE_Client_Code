/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "CR_Tasker.h"
#include "DCI_Owner.h"
#include "Toolkit_Revision.h"
#include "Test_CR_Task.h"

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_Task( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
{
	CR_Tasker_Begin( cr_task );

	printf( "Inside Test_Task \n" );

	while ( true )
	{
		printf( "Inside CR Tasker \n" );
		printf( "esp-idf version: %s\n", esp_get_idf_version() );
		printf( "Babelfish version: Major: %u, Minor: %u, Build: %u \n", CONST_LTK_MAJOR, CONST_LTK_MINOR,
				CONST_LTK_BUILD );


		// Sleep task
		CR_Tasker_Delay( cr_task, 100U );
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Test_CR_Task( void )
{
	// This is test code
	// coverity[leaked_storage]
	new CR_Tasker( Test_Task, NULL, 3, "Test Task" );
}
