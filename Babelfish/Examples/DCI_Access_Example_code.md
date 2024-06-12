/*
/example DCI Access Command Examples
/brief  This file is to demonstrate the DCI Access commands which is implemented in DCI_Patron.cpp and DCI_Owner.cpp
/   List of commands tested :
/	1.DCI_ACCESS_GET_RAM_CMD
/	2.DCI_ACCESS_SET_RAM_CMD
/	3.DCI_ACCESS_GET_INIT_CMD
/	4.DCI_ACCESS_SET_INIT_CMD
/	5.DCI_ACCESS_GET_DEFAULT_CMD
/	6.DCI_ACCESS_SET_DEFAULT_CMD
/	7.DCI_ACCESS_GET_LENGTH_CMD
/	8.DCI_ACCESS_SET_LENGTH_CMD
/	9.DCI_ACCESS_GET_MIN_CMD
/	10.DCI_ACCESS_SET_MIN_CMD - Not implemented in DCI_Patron.CPP
/	11.DCI_ACCESS_GET_MAX_CMD
/	12.DCI_ACCESS_SET_MAX_CMD- Not implemented in DCI_Patron.CPP
/	13.DCI_ACCESS_GET_ENUM_LIST_LEN_CMD
/	14.DCI_ACCESS_SET_ENUM_LIST_LEN_CMD - Not implemented in DCI_Patron.CPP
/	15.DCI_ACCESS_GET_ENUM_CMD - Not working
/	16.DCI_ACCESS_SET_ENUM_CMD - Not implemented in DCI_Patron.CPP
/	17.DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD
/	18.DCI_ACCESS_RAM_TO_INIT_CMD
/	19.DCI_ACCESS_INIT_TO_RAM_CMD
/	20.DCI_ACCESS_DEFAULT_TO_INIT_CMD
/	21.DCI_ACCESS_DEFAULT_TO_RAM_CMD
/	22.DCI_ACCESS_POST_SET_RAM_CMD
/	23.DCI_ACCESS_POST_SET_INIT_CMD
/	24.DCI_ACCESS_CLEAR_RAM_CMD - Not implemented in DCI_Patron.CPP
/
\par
\details Following DCID needs to be created in Tools\RTK DCI DB Creator.xlsm
\n 1. DCI_TEST_2_DCID with Default Support,RW Default,Range Support,RW Range,Callback Support,RW Length
\n    for Testing commands 1,2,5,6,7,8,9,10,11,12,13,17,21,22 please refer the below table 
\n    for reference
\n 2. DCI_TEST_1_DCID with Init Val Support,Default Support,RW Default,Callback Support,
\n    for Testing the commands 3,4,18,19,20,23 please refer below table for reference
 ----------------------------------------------------------------------------------
\n					|			DCI_TEST_2			|			DCI_TEST_1			|
\n ----------------------------------------------------------------------------------
\n Description		|	Test DCI access comand		|	Test DCI access comand NV	|
\n Code define		|			DCI_TEST_2			|			DCI_TEST_1			|
\n Data type		|		DCI_DTYPE_UINT8			|		DCI_DTYPE_UINT8			|
\n Data type length	|				1				|				1				|
\n Total Length		|				1				|				1				|
\n Default			|				1				|				2				|
\n Min              |               1               |             100               |
\n Max              |                               |                               |
\nBitfield Details  | can add your own bitfeild of  |                               |
\n                  |  size 2                       |                               |
\n					|								|								|
\n Owner			| 								|								|
\n Characteristics	|								|								|
\n Enum Details     |               2;              |                               |
\n (RO Val)			|	            X				|								|
\n Init Val Support	|								|				X				|
\n Default support	|				X				|				X				|
\n RW Default		|				X				|				X				|
\n Range Support    |               X               |                               |
\n RW Range         |               X               |                               |
\n Callback support	|				X				|				X				|
\n RW Length        |               X               |                               |
\n Enum Support     |               X               |                               |
\n Application Data |                               |                               |
\n					|								|								|
\n Patron			|								|								|
\n Characteristics	|								|								|
\n					|								|								|
\n Ram Val Write	|								|								|
\n access			|				X				|				X				|
\n Init Val Read 	|								|								|
\n Access			|								|				X				|
\n Init Val write	|								|								|
\n Access			|								|				X				|
\n Default Read		|								|								|
\n Access			|				X				|				X				|
\n Range Access     |               X               |                               |
\n Enum Access      |               X               |                               |
\n ----------------------------------------------------------------------------------

\n How to use this example code
\n Copy the files, DCI_Access_Example_code.md and DCI_Patron_Access_Example_code.md to
\n RTK_Example\Code\ directory. And the rename them as .cpp files.

\n Add these files into the IAR Project.

\n Uncomment the corresponding function for the access command at Test_DCI_Access_example()
\n at DCI_Access_Example_code.cpp, which needs to be tested. For example, for testing
\n DCI_ACCESS_GET_RAM_CMD, uncomment the function, Test_Get_Cmd().

\n Uncomment the corresponding function for the the access command in PROD_SPEC_Test_DCI_Access()
\n at DCI_Patron_Access_Example_code.cpp, which needs to be tested. For example, for testing
\n DCI_ACCESS_GET_RAM_CMD, uncomment the function, Test_DCI_Access().

\n Call the PROD_SPEC_Test_DCI_Access() function in PROD_SPEC_APP_Init() function at 
\n Product_Spec_APP.cpp

*/

#include "Includes.h"
#include "Prod_Spec_LTK_STM32F.h"
#include "Prod_Spec_APP.h"
#include "DCI_Owner.h"
#include "DCI.h"
#include "DCI_Data.h"
#include "DCI_NV_Mem.h"
#include "Prod_Spec_Debug.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
DCI_Owner* m_test_RAM;

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

void Test_Get_Cmd( void );

void Test_Set_Cmd( void );

void Test_Get_Init_cmd( void );

void Test_Set_Init_cmd( void );

void Test_Post_Set_Cmd( void );

void Test_Post_Set_Init_Cmd( void );

void Test_Get_Default_Cmd( void );

void Test_Get_Length_Cmd( void );

void Test_Min_Cmd( void );

void Test_Max_Cmd( void );

void Test_Enum_List_Len_Cmd( void );

void Test_Enum_Cmd( void );

void Test_Ram_To_Init_Cmd( void );

void Test_Init_To_Ram_Cmd( void );

void Test_Default_To_Init_Cmd( void );

void Test_Default_To_Ram_Cmd( void );

void Test_Get_Attrib_Support( void );

// Callback functions prototypes

/**
 **********************************************************************************************
 * @brief Below callback funtions for DCI Access Commands
 * @details These functions shall be called inside the actual callback "Test_DCI_Access_CB" funtion
 * based on each DCI Access commands from the patron
 **********************************************************************************************
 */
void Test_Get_RAM_CB( void );

void Test_Set_RAM_CB( void );

void Test_Get_Init_CB( void );

void Test_Set_Init_CB( void );

void Test_Post_Set_RAM_CB( void );

void Test_Post_Set_Init_CB( void );

void Test_Get_Default_CB( void );

void Test_Get_Length_CB( void );

void Test_Min_Cmd_CB( void );

void Test_Max_Cmd_CB( void );

void Test_Enum_List_Len_Cmd_CB( void );

void Test_Enum_Cmd_CB( void );

void Test_Ram_To_Init_Cmd_CB( void );

void Test_Init_To_Ram_Cmd_CB( void );

void Test_Default_To_Init_Cmd_CB( void );

void Test_Default_To_Ram_Cmd_CB( void );

void Test_Get_Attrib_Support_CB( void );

// Actual callback prototype
DCI_CB_RET_TD Test_DCI_Access_CB( DCI_CBACK_PARAM_TD cback_param, DCI_ACCESS_ST_TD* access_struct );

/**
 **********************************************************************************************
 * @brief						DCI Access example function
 * @details						This functions shall be used for understanding each DCI Access
								commands by running each example code
 * @param[in] void				none

 * @return[out] none			none
 * @n
 **********************************************************************************************
 */
void Test_DCI_Access_example( void )
{
	/*@ Note:Enable the respective patron access in PROD_SPEC_Test_DCI_Access( ) function
	 */

	// Note : Enable one example at a time

	/*@ Example 1 :DCI_ACCESS_GET_RAM_CMD triggers the
	 *				call back when patron read the
	 *				DCID value.In  GET_RAM_CMD
	 *				first it servers the callback then
	 *				so prints the default value.
	 */
	 //Test_Get_Cmd();

	/*@ Example 2:	DCI_ACCESS_SET_RAM_CMD triggers the
	 *				call back whenever their is a change
	 *				in the Ram value of the DCI.In  SET_RAM_CMD
	 *				first it servers the callback then sets the
	 *				value so prints the value before setting new value
	 */
	// Test_Set_Cmd();

	/*@ Example 3:	DCI_ACCESS_GET_INIT_CMD triggers the
	 *				callback when patron read the DCID value
	 *				from NV.In GET_INIT_CMD  first serves the
	 *				callback so prints the default NV value
	 */
	// Test_Get_Init_cmd();

	/*@ Example 4:	DCI_ACCESS_SET_INIT_CMD triggers the
	 *				call back whenever there is a change
	 *				in the NV value of the DCID.In SET_INIT_CMD
	 *				first it servers the callback then sets the NV
	 *				value so prints the value before setting
	 *				new to the NV.
	 */
	// Test_Set_Init_cmd();

	/*@ Example 5:	DCI_ACCESS_POST_SET_RAM_CMD triggers a
	 *				callback  when there is a change in the RAM
	 *				value of the DCID.In the POST_SET_RAM first it
	 *				sets the RAM value and comes to the call Back function
	 *				and prints the updated value of DCI
	 */
	// Test_Post_Set_Cmd();

	/*@ Example 6:	DCI_ACCESS_POST_SET_INIT_CMD triggers a
	 *				callback  when there is a change in the NV value
	 *				value.In the POST_SET_INIT_CMD first it
	 *				sets the NV value and comes to the call Back function
	 *				and prints the updated value of DCI by reading from NV
	 */
	// Test_Post_Set_Init_Cmd();


	/*@ Example 7:	DCI_ACCESS_GET_DEFAULT_CMD triggers a
	 *				callback  when patron read the default
	 *				value.In the GET_DEFAULT_CMD first it
	 *				serves call Back function and prints
	 *				default DCID value
	 */
	// Test_Get_Default_Cmd();


	/*@ Example 8:	DCI_ACCESS_GET_LENGTH_CMD triggers a
	 *				callback  when patron read DCI length
	 *				value.In the GET_LENGTH_CMD first serves the
	 *				callback function and prints the length of
	 *				DCID value
	 */
	// Test_Get_Length_Cmd();


	/*@ Example 9:	DCI_ACCESS_GET_MIN_CMD triggers a
	 *				callback  when patron read the DCI Minimum
	 *				value of DCID.In the GET_MIN_CMD first it serves
	 *				callback function and prints the Minimum
	 *				value of DCID
	 */
	// Test_Min_Cmd();


	/*@ Example 10:	DCI_ACCESS_GET_MAX_CMD triggers the
	 *				callback  when patron reads the Maximim
	 *				value of DCID.In the GET_MAX_CMD first it serves
	 *				callback function and prints the Maximum value
	 *				of DCID
	 */
	// Test_Max_Cmd();


	/*@ Example 11:	DCI_ACCESS_GET_ENUM_LIST_LEN_CMD triggers a
	 *				callback  when patron read the length of enum
	 *				list value  of DCID.In the GET_ENUM_LIST_LEN first serves
	 *				callback function and prints the Maximum value
	 *				of DCID
	 */
	// Test_Enum_List_Len_Cmd();


	/*@ Example 12:	DCI_ACCESS_GET_ENUM_CMD triggers a
	 *				callback  when patron read the Maximim
	 *				value of DCID.In the GET_ENUM_CMD first serves
	 *				callback function and prints the Maximum value
	 *				of DCID
	 */
	// Test_Enum_Cmd();


	/*@ Example 13:	DCI_ACCESS_RAM_TO_INIT_CMD triggers a
	 *				callback  when patron sets the current RAM
	 *				value of DCID to NV value.
	 */
	// Test_Ram_To_Init_Cmd();


	/*@ Example 14:	DCI_ACCESS_INIT_TO_RAM_CMD triggers a
	 *				callback  when patron sets the current RAM
	 *				value of DCID to NV value.
	 */
	// Test_Init_To_Ram_Cmd();

	/*@ Example 15:	DCI_ACCESS_DEFAULT_TO_INIT_CMD triggers a
	 *				callback   when patron sets the default
	 *				value of DCID to NV value.
	 */
	// Test_Default_To_Init_Cmd();


	/*@ Example 16:	DCI_ACCESS_DEFAULT_TO_RAM_CMD triggers a
	 *				callback  when patron sets the default NV to
	 *				cureent value of DCID to NV value.
	 */
	// Test_Default_To_Ram_Cmd();


}

/**
 **********************************************************************************************
 * @brief								This is DCI_Access call back function
 * @details								This functions shall be attached while creating the DCI owner
 *										and mask for the specific DCI Access commands to understand the
 *										functionality of each DCI Access commands
 * @param[in] access_struct				DCI_CBACK_PARAM_TD information
 * @param[in] access_struct				DCI_ACCESS_ST_TD information
 * @return[out] DCI_CB_RET_TD			None
 **********************************************************************************************
 */
DCI_CB_RET_TD Test_DCI_Access_CB( DCI_CBACK_PARAM_TD cback_param, DCI_ACCESS_ST_TD* access_struct )
{

	if ( access_struct->command == DCI_ACCESS_GET_RAM_CMD )
	{

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 1 :DCI_ACCESS_GET_RAM_CMD " );

		// To Print GET RAM value
		Test_Get_RAM_CB();

	}

	if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
	{

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 2 :DCI_ACCESS_SET_RAM_CMD " );

		// To Print SET RAM value
		Test_Set_RAM_CB();

	}

	if ( access_struct->command == DCI_ACCESS_GET_INIT_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 3 :DCI_ACCESS_GET_INIT_CMD " );

		// To print GET INIT value from NV
		Test_Get_Init_CB();
	}

	if ( access_struct->command == DCI_ACCESS_SET_INIT_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 4 :DCI_ACCESS_SET_RAM_CMD " );

		// To Print SET INIT value in NV
		Test_Set_Init_CB();
	}

	if ( access_struct->command == DCI_ACCESS_POST_SET_RAM_CMD )
	{

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 5 :DCI_ACCESS_POST_SET_RAM_CMD " );

		// To Print POST SET value in RAM
		Test_Post_Set_RAM_CB();
	}

	if ( access_struct->command == DCI_ACCESS_POST_SET_INIT_CMD )
	{

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 6 :DCI_ACCESS_POST_SET_INIT_CMD " );

		// To Print POST SET INIT value in NV
		Test_Post_Set_Init_CB();
	}

	if ( access_struct->command == DCI_ACCESS_GET_DEFAULT_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 7 :DCI_ACCESS_GET_DEFAULT_CMD " );

		// To Print Default value of DCID
		Test_Get_Default_CB();
	}

	if ( access_struct->command == DCI_ACCESS_GET_LENGTH_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 8 :DCI_ACCESS_GET_LENGTH_CMD " );

		// To Print length of DCID
		Test_Get_Length_CB();
	}

	if ( access_struct->command == DCI_ACCESS_GET_MIN_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 9 :DCI_ACCESS_GET_MIN_CMD " );

		// To Print Minimum value of DCID
		Test_Min_Cmd_CB();
	}

	if ( access_struct->command == DCI_ACCESS_GET_MAX_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 10 :DCI_ACCESS_GET_MAX_CMD " );

		// To Print Maximun value in DCID
		Test_Max_Cmd_CB();
	}



	if ( access_struct->command == DCI_ACCESS_GET_ENUM_LIST_LEN_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 11 :DCI_ACCESS_GET_ENUM_LIST_LEN_CMD " );

		// To Print Enum list length value in DCID
		Test_Enum_List_Len_Cmd_CB();
	}


	if ( access_struct->command == DCI_ACCESS_GET_ENUM_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 12 :DCI_ACCESS_GET_ENUM_CMD " );

		// To Print Enum value in DCID
		Test_Enum_Cmd_CB();
	}

	if ( access_struct->command == DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 13 :DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD " );

		// To Print Enum value in DCID
		// Test_Enum_Cmd_CB();
	}

	if ( access_struct->command == DCI_ACCESS_RAM_TO_INIT_CMD )
	{
		uint8_t temp;

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 14 :DCI_ACCESS_RAM_TO_INIT_CMD " );

		m_test_RAM->Check_Out( &temp );

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Before callback %d ", temp );
		// To Print NV Value in DCID (after copying the Ram value to NV )
		Test_Ram_To_Init_Cmd_CB();
	}

	if ( access_struct->command == DCI_ACCESS_INIT_TO_RAM_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 15 :DCI_ACCESS_INIT_TO_RAM_CMD " );

		// To Print Ram Value in DCID (after copying the NV value to Ram )
		Test_Init_To_Ram_Cmd_CB();
	}

	if ( access_struct->command == DCI_ACCESS_DEFAULT_TO_INIT_CMD )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 16 :DCI_ACCESS_DEFAULT_TO_INIT_CMD " );

		// To Print NV Value in DCID (after copying the Default value to NV )
		Test_Default_To_Init_Cmd_CB();
	}

	if ( access_struct->command == DCI_ACCESS_DEFAULT_TO_RAM_CMD )
	{

		PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Testing  Example 17 :DCI_ACCESS_DEFAULT_TO_RAM_CMD " );

		// To Print Ram Value in DCID (after copying the Default value to Ram )
		Test_Default_To_Ram_Cmd_CB();
	}


	return ( 0 );
}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any get method
 *									 called to access the Ram value of DCI, First it reaches this callback
 *									 (pre-get)if any action to be taken before someone using the present
 *									 Ram value can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Get_RAM_CB( void )
{
	uint8_t temp1 = 0;

	// Reading the Value of DCID
	m_test_RAM->Check_Out( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "DCI_ACCESS_GET_RAM_CMD callback is triggered before setting RAM value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_GET_RAM_CMD callback is  %d", temp1 );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when Set method
 *									called to access and update Ram value of DCI, First it reaches this callback
 *									(pre-set)if any action to be taken before someone updating the present
 *									Ram value can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Set_RAM_CB( void )
{
	uint8_t temp1 = 0;

	/** Important Note : If you do a checkout here in the pre-set callback
	 * for set ram, you will be fetching the value which is currently in the DCI and NOT the
	 * value which will be actually put into the DCI.
	 * In other words - if you want to see the value that is going to get written into the DCI
	 * then you need to look at the DCI access struct which is passed in.
	 */
	// Reading the value of DCID
	m_test_RAM->Check_Out( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "DCI_ACCESS_SET_RAM_CMD callback is triggered before setting RAM value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_SET_RAM_CMD callback is  %d", temp1 );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when Get Init method
 *									called to access the NV value of DCI, First it reaches this callback
 *									(pre-get)if any action to be taken before someone using the present
 *									NV value can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Get_Init_CB( void )
{
	uint8_t temp1 = 0;

	// Reading the NV value of DCID
	m_test_RAM->Check_Out_Init( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "DCI_ACCESS_GET_INIT_CMD callback is triggered before setting NV value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_GET_INIT_CMD callback is  %d", temp1 );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any set method
 *									called to access the NV value of DCI, First it reaches this callback
 *									(pre-set)if any action to be taken before someone updating the present
 *									NV value can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */

void Test_Set_Init_CB( void )
{
	uint8_t temp_var = 0;

	/** Important Note : If you do a checkout here in the pre-set callback
	 * for set init, you will be fetching the value which is currently in the NV of the DCID
	 * and NOT the value which will be actually put into the NV of the DCID.
	 * In other words - if you want to see the value that is going to get written into the DCI
	 * then you need to look at the DCI access struct which is passed in.
	 */
	// Reading the NV value of DCID
	m_test_RAM->Check_Out_Init( &temp_var );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "DCI_ACCESS_SET_INIT_CMD callback is triggered before setting NV value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_SET_INIT_CMD callback is  %d", temp_var );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any post set method
 *									called to access the Ram value of DCI, It updates the Ram value and
 *									then it reaches this callback (post-set)if any action to
 *									be taken on the updated Ram value can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Post_Set_RAM_CB( void )
{
	uint8_t temp1 = 0;

	/** Important Note :
	 * The Check out in the Post Set command will actually get the latest value which was written.
	 * This is very important to describe here.
	 */
	// Reading the value of DCID
	m_test_RAM->Check_Out( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   "DCI_ACCESS_POST_SET_RAM_CMD callback is triggered after setting RAM value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_POST_SET_RAM_CMD callback is  %d", temp1 );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any post set method
 *									called to access the NV value of DCI, It updates the NV value and
 *									then it reaches this callback (post-set)if any action to
 *									be taken on the updated NV value can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Post_Set_Init_CB( void )
{
	uint8_t temp1 = 0;

	/** Important Note :
	 * The Check out in the Post Set Init command will actually get the latest value which was written
	 * into the NV value of the DCI.
	 */

	// Reading the NV value of DCID
	m_test_RAM->Check_Out_Init( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   "DCI_ACCESS_POST_SET_INIT_CMD callback is triggered after setting NV value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_POST_SET_INIT_CMD callback is  %d", temp1 );
}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any get method called to access
 *									the default value of DCI, First it reaches this callback
 *									(pre-get)if any action to be taken before someone using the present
 *									default value can be done here.
 * @param[in]						None
 * @return							None
 * @note							As SET_DEFAULT_CMD is not having a
 *									callback in DCI patron using the same callback to test the
 *									SET_DEFAULT_CMD
 **********************************************************************************************
 */
void Test_Get_Default_CB( void )
{
	uint8_t default_temp = 0;

	uint8_t default_new = 5;

	// Reading the default value of DCID
	m_test_RAM->Check_Out_Default( &default_temp );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_GET_DEFAULT_CMD value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   " value of DCI_ACCESS_GET_DEFAULT_CMD before changing the default value is %d",
						   default_temp );

	// changing the default value of DCID
	m_test_RAM->Check_In_Default( &default_new );

	// Reading the default value of DCID after changing
	m_test_RAM->Check_Out_Default( &default_new );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To set the DCI_ACCESS_SET_DEFAULT_CMD value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   "value of DCID in  DCI_ACCESS_GET_DEFAULT_CMD  after changing the default value is %d",
						   default_new );
}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any get method
 *									called to access the length value of DCI, First it reaches this callback
 *									(pre-get)if any action to be taken before someone using the present
 *									length value can be done here.
 * @param[in]						None
 * @return							None
 * @note							As SET_LENGTH_CMD  is not having a
 *									callback in DCI patron using the same callback to test the
 *									SET_LENGTH_CMD
 **********************************************************************************************
 */
void Test_Get_Length_CB( void )
{
	uint16_t temp_len = 0;

	uint16_t new_len = 2;

	// Reading the length of DCID
	m_test_RAM->Check_Out_Length( &temp_len );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_GET_LENGTH_CMD value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   "length of DCID in  DCI_ACCESS_GET_LENGTH_CMD  before changing the length is  %d",
						   temp_len );

	// Setting the length of DCID
	m_test_RAM->Check_In_Length( &new_len );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_SET_LENGTH_CMD value " );

	m_test_RAM->Check_Out_Length( &new_len );
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   "value of DCID in  DCI_ACCESS_SET_DEFAULT_CMD  after changing the length is  %d", new_len );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any get method
 *									called to access the Minimum value of DCI, First it reaches this callback
 *									(pre-get)if any action to be taken before someone using the present
 *									Minimum value can be done here.
 * @param[in]						None
 * @return							None
 * @note							As SET_MIN_CMD is not having a
 *									callback in DCI patron using the same callback to test the
 *									SET_MIN_CMD
 **********************************************************************************************
 */
void Test_Min_Cmd_CB( void )
{
	uint8_t temp_min = 0;

	uint8_t new_min_val = 2;

	// Reading the Minimum value of DCID
	m_test_RAM->Check_Out_Min( &temp_min );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_GET_MIN_CMD value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Minimum value of DCID in  DCI_ACCESS_GET_MIN_CMD  before changing is %d",
						   temp_min );

	// Setting the Minimum value of DCID
	m_test_RAM->Check_In_Min( &new_min_val );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_SET_MIN_CMD value " );

	m_test_RAM->Check_Out_Min( &temp_min );
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Minimum value of DCID in  DCI_ACCESS_SET_MIN_CMD  after changing is  %d",
						   temp_min );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any get method
 *									called to access the (Range) Maximum value of DCI, Before getting the maximum value
									First it reaches this callback (pre-get)if any action to be taken
 *									before someone using the present Maximum value can be done here.
 *
 * @param[in]						None
 * @return							None
 * @note							As DCI_ACCESS_SET_MAX_CMD is not having a
 *									callback in DCI patron using the same callback to demonstrate the
 *									the SET_MAX_CMD
 **********************************************************************************************
 */
void Test_Max_Cmd_CB( void )
{

	uint8_t temp_max = 0;

	uint8_t new_max1 = 200;

	// Reading the Maximum value of DCID
	m_test_RAM->Check_Out_Max( &temp_max );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_GET_MAX_CMD value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Maximum value of DCID in  DCI_ACCESS_GET_MAX_CMD  before changing  is  %d",
						   temp_max );

	// Seting a new Maximum value of DCID
	m_test_RAM->Check_In_Max( &new_max1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_SET_MAX_CMD value " );

	// Reading the new Maximum value of DCID
	m_test_RAM->Check_Out_Max( &new_max1 );
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Maximum value of DCID in  DCI_ACCESS_SET_MAX_CMD  after changing is  %d",
						   new_max1 );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any get method
 *									called to access the Enum list length value of DCI, Before getting
 *									the maximum value First it reaches this callback (pre-get)if any
 *									action to be taken before someone using the present Enum list length
 *									value can be done here.
 *
 * @param[in]						None
 * @return							None
 * @note							As DCI_ACCESS_SET_ENUM_LIST_LEN_CMD is not having a
 *									callback in DCI patron So it is not tested.Only testing
 *									DCI_ACCESS_GET_ENUM_LIST_LEN_CMD
 **********************************************************************************************
 */
void Test_Enum_List_Len_Cmd_CB( void )
{
	uint16_t enum_len = 0;

	// Reading the Maximum value of DCID
	m_test_RAM->Check_Out_Enum_Element_Count( &enum_len );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_GET_ENUM_LIST_LEN_CMD value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Enum count  value of DCID in  DCI_ACCESS_GET_ENUM_LIST_LEN_CMD is  %d",
						   enum_len );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when any get method
 *									called to Enum value of DCI, Before getting
 *									the Enum value First it reaches this callback (pre-get)if any
 *									action to be taken before someone using the present Enum
 *									value can be done here.
 *
 * @param[in]						None
 * @return							None
 * @note							As DCI_ACCESS_SET_ENUM_CMD is not having a
 *									callback in DCI patron it is not tested.Only testing
 *									DCI_ACCESS_GET_ENUM_CMD
 **********************************************************************************************
 */
void Test_Enum_Cmd_CB( void )
{

	uint8_t temp_enum = 0U;

	uint16_t enum_offset = 0U;

	uint16_t enum_count = 0U;


	// Reading the Enum value of DCID
	m_test_RAM->Check_Out_Enum( &temp_enum, enum_offset, enum_count );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " To get the DCI_ACCESS_GET_ENUM_CMD value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Enum  value of DCID in  DCI_ACCESS_GET_ENUM_CMD   is  %d ", temp_enum );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Enum  offset value of DCID in  DCI_ACCESS_GET_ENUM_CMD   is  %d ",
							enum_offset );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "Enum count value of DCID in  DCI_ACCESS_GET_ENUM_CMD   is  %d ",
							enum_count );


}

/**
 **********************************************************************************************
 * @brief							This callback function is called when patron access using
 *									DCI_ACCESS_RAM_TO_INIT_CMD method to set Ram value of the
 *									DCI to NV value of the DCI , Before setting the Ram value
 *									to NV of the DCI . First it reaches this callback (pre-load)if any
 *									action to be taken before someone using the updated
 *									NV value of the DCI can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Ram_To_Init_Cmd_CB( void )
{
	uint8_t temp_var = 10;

	m_test_RAM->Check_Out_Init( &temp_var );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   "DCI_ACCESS_RAM_TO_INIT_CMD callback is triggered after setting RAM value to NV value" );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_RAM_TO_INIT_CMD callback is  %d", temp_var );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when patron access using
 *									DCI_ACCESS_INIT_TO_RAM_CMD to set NV value of the
 *									DCI to Ram value of the DCI , Before setting the NV value
 *									to Ram value of the DCI . First it reaches this callback (pre-load)if any
 *									action to be taken before someone using the updated
 *									Ram value of the DCI can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Init_To_Ram_Cmd_CB( void )
{
	uint8_t temp1 = 25;

	m_test_RAM->Check_Out( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
							"DCI_ACCESS_INIT_TO_RAM_CMD callback is triggered after setting the NV value to Ram value" );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_INIT_TO_RAM_CMD callback is  %d", temp1 );

}

/**
 **********************************************************************************************
 * @brief							This callback function is called when patron access using
 *									DCI_ACCESS_DEFAULT_TO_INIT_CMD to set NV value of the
 *									DCI to Default value of the DCI , Before setting the NV value
 *									to Default value of the DCI . First it reaches this callback
 *									(pre-load)if any action to be taken before someone using the
 *									updated default value of the DCI can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Default_To_Init_Cmd_CB( void )
{
	uint8_t temp1 = 0;

	// uint8_t new_temp = 24;

	// Reading the Value of DCID
	// m_test_RAM->Check_In_Init( &temp1 );

	// PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "DCI_ACCESS_DEFAULT_TO_INIT_CMD callback is triggered before setting RAM
	// value to NV value" );

	// PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_DEFAULT_TO_INIT_CMD callback is  %d", temp1
	// );

	m_test_RAM->Check_Out_Init( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
							"DCI_ACCESS_DEFAULT_TO_INIT_CMD callback is triggered after setting Default value to NV" );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_DEFAULT_TO_INIT_CMD callback is  %d", temp1 );



}

/**
 **********************************************************************************************
 * @brief							This callback function is called when patron access using
 *									DCI_ACCESS_DEFAULT_TO_RAM_CMD to set Default value of the
 *									DCI to Ram value of the DCI, Before setting the Default value
 *									to Ram value of the DCI . First it reaches this callback
 *									(pre-load)if any action to be taken before someone using the
 *									updated Ram value of the DCI can be done here.
 * @param[in]						None
 * @return							None
 * @n
 **********************************************************************************************
 */
void Test_Default_To_Ram_Cmd_CB( void )
{
	uint8_t temp1 = 0;

	uint8_t new_temp = 0;

	// Reading the Value of DCID
	// m_test_RAM->Check_In( &temp1 );

	// PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "DCI_ACCESS_DEFAULT_TO_RAM_CMD callback is triggered before setting Default
	// value " );

	// PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_DEFAULT_TO_RAM_CMD callback is  %d", temp1 );

	m_test_RAM->Check_Out_Default( &temp1 );

	// PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "DCI_ACCESS_DEFAULT_TO_RAM_CMD callback is triggered after setting the
	// Default value to Ram value " );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of DCID in  DCI_ACCESS_DEFAULT_TO_RAM_CMD callback is  %d", temp1 );

	m_test_RAM->Check_Out( &new_temp );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "value of RAM  DCID in  DCI_ACCESS_DEFAULT_TO_RAM_CMD callback is  %d",
						   new_temp );



}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_RAM_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Get_Cmd( void )
{
	// Creating Owner for DCI
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback*
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_RAM_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_SET_RAM_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Set_Cmd( void )
{
	// Creating Owner for DCI
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_SET_RAM_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_INIT_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Get_Init_cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_1_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_INIT_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_SET_INIT_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Set_Init_cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_1_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_SET_INIT_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_POST_SET_RAM_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Post_Set_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_POST_SET_RAM_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_POST_SET_INIT_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Post_Set_Init_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_1_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_POST_SET_INIT_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_DEFAULT_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Get_Default_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_DEFAULT_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_LENGTH_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Get_Length_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_LENGTH_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_MIN_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Min_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_MIN_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_MAX_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Max_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_MAX_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_ENUM_LIST_LEN_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Enum_List_Len_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_ENUM_LIST_LEN_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_ENUM_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Enum_Cmd( void )
{
	// Creating Owner for DCID
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_ENUM_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Get_Attrib_Support( void )
{
	// Creating Owner for DCI
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	// Attaching Callback
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_RAM_TO_INIT_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Ram_To_Init_Cmd( void )
{
	uint8_t temp = 22U;

	// Creating Owner for DCI
	m_test_RAM = new DCI_Owner( DCI_TEST_1_DCID );

	m_test_RAM->Check_In( &temp );
	// Attaching Callback*
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_RAM_TO_INIT_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_INIT_TO_RAM_CMD_MSK
 *
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Init_To_Ram_Cmd( void )
{
	uint8_t temp2 = 98;

	// Creating Owner for DCI
	m_test_RAM = new DCI_Owner( DCI_TEST_1_DCID );

	m_test_RAM->Check_In_Init( &temp2 );
	// Attaching Callback*
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_INIT_TO_RAM_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_DEFAULT_TO_INIT_CMD_MSK
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Default_To_Init_Cmd( void )
{
	uint8_t temp2 = 55;

	// Creating Owner for DCI
	m_test_RAM = new DCI_Owner( DCI_TEST_1_DCID );

	m_test_RAM->Check_In_Init( &temp2 );
	// Attaching Callback*
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_DEFAULT_TO_INIT_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function provides an example of creating DCI owner
 *							and attaching the callback with DCI_ACCESS_DEFAULT_TO_RAM_CMD_MSK
 *
 * @param[in] void			none
 * @return[out] void			none
 * @n
 **********************************************************************************************
 */
void Test_Default_To_Ram_Cmd( void )
{
	uint8_t temp1 = 90;

	// Creating Owner for DCI
	m_test_RAM = new DCI_Owner( DCI_TEST_2_DCID );

	m_test_RAM->Check_In( &temp1 );


	// Attaching Callback*
	m_test_RAM->Attach_Callback( &Test_DCI_Access_CB, NULL, ( DCI_ACCESS_DEFAULT_TO_RAM_CMD_MSK ) );

}

/**
 **********************************************************************************************
 * @brief				 	This function just added to read the ram value repeatedly to see the
 *							changes before and after the patron access.
 *							Since the callback is triggered for every patron access for demonstration
 *							purpose added this funtion to read the Ram value after the callback operations.
 * @param[in] void			none
 * @return[out] void			none
 * @note						This function can be called in the single timer for every 1 to 5 seconds
 *							For testing purpose - added inside the callback_function of the LED_ON_OFF_Example( )
 *							in Prod_Spec_APP.cpp
 **********************************************************************************************
 */
void Sample_Get_Ram_Value_Repeatedly( void )
{

	uint8_t test_val = 0u;

	m_test_RAM->Check_Out( &test_val );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Printing RAM value everyone sec  %d ", test_val );

}

// ~~~~