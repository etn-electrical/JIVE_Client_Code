/*
	/example DCI Access Patron Access Examples
	/brief  This file is used to demonstrate the patron access of the DCI Access command
	/       which is implemented in DCI_Patron.cpp
	\par
	\details Following DCID needs to be created in Tools\RTK DCI DB Creator.xlsm
	\n 1. DCI_TEST_2_DCID enable Patron characterictics
	\n    enable Ram Val Write Access,Default Read Access,Range Access in DCI sheet
	\n 2. DCI_TEST_1_DCID enable Patron characterictics
	\n    enable Ram Val Write Access,Init Val Read Access,Init Val Write Access,Default Read Access in DCI Sheet
	\n create a patron
	\note Function should be called only after the owner is created,
	\n    patron and owner should not be created in same file.
	\n    If need to test the DCI access example insert this file in project settings
	\n
	~~~~{.cpp}*/

#include "Includes.h"
#include "DCI_Defines.h"
#include "DCI_Patron.h"
#include "Prod_Spec_Debug.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
DCI_Patron* m_patron_access;

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

// DCI Testing functions

void Test_DCI_Access_example( void );

/**
 **********************************************************************************************
 * @brief 		Below funtions created to demonstrate the Patron access
 * @details 	Function provides an example how to create a patron access and access the DCI
 * 				specific DCI Access commands
 **********************************************************************************************
 */
void Test_DCI_Access( void );

void Test_Set_DCI_Access( uint8_t* temp_buffer );

void Test_Read_DCI_Access( uint8_t* temp_buffer );

void Test_INIT_DCI_Access( void );

void Test_Set_INIT_DCI_Access( uint8_t* temp_buffer );

void Test_Read_INIT_DCI_Access( uint8_t* temp_buffer );

void Test_DCI_Access_Length( void );

void Test_Read_Default_DCI_Access( uint8_t* temp_buffer );

void Test_Read_length_DCI_Access( uint8_t* temp_buffer );

void Test_DCI_Access_Range( void );

void Test_Read_Min_DCI_Access( uint8_t* temp_buffer );

void Test_Read_Max_DCI_Access( uint8_t* temp_buffer );

void Test_DCI_Access_Enum( void );

void Test_Enum_List_Len_DCI_Access( uint8_t* temp_buffer );

void Test_Enum_DCI_Access( uint8_t* temp_buffer );

void Test_DCI_Ram_To_Init( void );

void Test_Ram_To_Init_DCI_Access( uint8_t* temp_buffer );

void Test_Init_To_Ram_DCI_Access( uint8_t* temp_buffer );

void Test_DCI_Default_Ram_To_Init( void );

void Test_Default_To_Init_DCI_Access( uint8_t* temp_buffer );

void Test_Default_To_Ram_DCI_Access( uint8_t* temp_buffer );

void Test_Get_Attrib_Support_DCI_Access( uint8_t* temp_buffer );

/**
 **********************************************************************************************
 * @brief							This function is to initialize and demonstrate
 *									Patron access and callback support of the each DCI Access
 *									commands
 * @param[in] void					none
 * @return[out] void				none
 * @note 							This funtion can be called from Prod_Spec_APP.CPP to initilaize
 *									and run the DCI Access command examples
									one example can be enabled at a time.
 **********************************************************************************************
 */
void PROD_SPEC_Test_DCI_Access( void )
{
	/**
	 * This function is mandatory to create the DCI owners and attach callback to demonstate each
	 * DCI Access commands, this function should be called before triggering the patron access
	 */
	Test_DCI_Access_example();

	/**
	 * Below functions used to demonstrate the patron access for the specific DCI Commands
	 * Only one funtion should be enabled at a time
	 */

	/// This function shall be called to demonstrate the DCI_ACCESS_SET_RAM_CMD/DCI_ACCESS_POST_SET_RAM_CMD
	 //Test_DCI_Access();

	/// This function shall be called to demonstrate the DCI_ACCESS_SET_INIT_CMD/ DCI_ACCESS_POST_SET_INIT_CMD
	// Test_INIT_DCI_Access();

	/**
	 * This function shall be called to demonstrate the DCI_ACCESS_GET_DEFAULT_CMD/DCI_ACCESS_SET_DEFAULT_CMD
	 * DCI_ACCESS_GET_LENGTH_CMD/DCI_ACCESS_SET_LENGTH_CMD
	 */
	// Test_DCI_Access_Length();

	/**
	 * This function shall be called to demonstrate the below set of commands
	 * DCI_ACCESS_GET_MIN_CMD/DCI_ACCESS_SET_MIN_CMD
	 * DCI_ACCESS_GET_MAX_CMD/DCI_ACCESS_SET_MAX_CMD
	 */
	// Test_DCI_Access_Range();

	/**
	 * This function shall be called to demonstrate the DCI_ACCESS_GET_ENUM_LIST_LEN_CMD/DCI_ACCESS_GET_ENUM_CMD
	 * DCI_ACCESS_GET_ENUM_CMD/DCI_ACCESS_SET_ENUM_CMD
	 */
	/// Test_DCI_Access_Enum();

	/// This function shall be called to demonstrate the DCI_ACCESS_SET_ENUM_CMD
	// Test_DCI_Ram_To_Init();

	/// This function shall be called to demonstrate the DCI_ACCESS_SET_ENUM_CMD
	// Test_DCI_Default_Ram_To_Init();

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD
 * @details							This function will get the attrib support information of the
 *									specific DCID's through the patron access
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Get_Attrib_Support_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " Attrib_Support selected for the DCID is  %d", temp_buffer );

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_RAM_CMD
 * @details							This function to get the Ram value of the DCID	through
 *									patron access
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Read_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_SET_RAM_CMD
 * @details							This function to set the Ram value of the DCID	through
 *									patron access
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Set_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	// uint16_t string_length;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr )
	access_struct.command = DCI_ACCESS_SET_RAM_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*Please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_INIT_CMD
 * @details							This function to get the NV value of the DCID through
 *									patron access
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Read_INIT_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );
	access_struct.command = DCI_ACCESS_GET_INIT_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_1_DCID;
	access_struct.data_access.data = temp_buffer;

	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_SET_INIT_CMD
 * @details							This function to Set the NV value of the DCID through
 *									patron access
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Set_INIT_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;

	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_SET_INIT_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	access_struct.data_id = DCI_TEST_1_DCID;
	access_struct.data_access.data = temp_buffer;

	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );
}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_SET_RAM_CMD/DCI_ACCESS_GET_RAM_CMD
 * @details							This function shall be called to set the Ram value and also
 *									it helps us to demonstrate the callback trigggers for the
 *									the DCI_ACCESS_SET_RAM_CMD/DCI_ACCESS_POST_SET_RAM_CMD
 *									patron access
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_DCI_Access( void )
{

	uint8_t temp_var = 40;

	BF_ASSERT( m_patron_access == static_cast<DCI_Patron*>( nullptr ) );
	m_patron_access = new DCI_Patron();

	/*Function to set the RAM value using patron*/
	// Test_Set_DCI_Access( &temp_var );

	temp_var = 0;

	/*Function  to Get the RAM value using patron*/
	Test_Read_DCI_Access( &temp_var );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " RAM value of DCID is  %d", temp_var );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_INIT_CMD and DCI_ACCESS_SET_INIT_CMD
 * @details							This function to Set and Get the NV value of the DCID through
 *									patron access and also it helps us to demonstrate the callback
 *									trigggers for the the DCI_ACCESS_SET_INIT_CMD/DCI_ACCESS_POST_SET_INIT_CMD
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_INIT_DCI_Access( void )
{

	uint8_t temp1 = 10;

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
						   "To test DCI_ACCESS_SET_INIT_CMD or DCI_ACCESS_POST_SET_INIT_CMD update the value( temp1) Manually" );


	BF_ASSERT( m_patron_access == static_cast<DCI_Patron*>( nullptr ) );
	m_patron_access = new DCI_Patron();

	/*Function set the value in NV using patron*/
	Test_Set_INIT_DCI_Access( &temp1 );

	temp1 = 0;

	/*Function get  the value in NV using patron*/
	Test_Read_INIT_DCI_Access( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " NV value of DCID is  %d", temp1 );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_DEFAULT_CMD
 * @details							This function to get the default value of the DCID through
 *									patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Read_Default_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_DEFAULT_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );
}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_LENGTH_CMD
 * @details							This function to get the length of the DCID through
 *									patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Read_length_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_LENGTH_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_MIN_CMD
 * @details							This function to get the minimum value of the DCID through
 *									patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Read_Min_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_MIN_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_MAX_CMD
 * @details							This function to get the maximum value of the DCID through
 *									patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Read_Max_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_MAX_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_ENUM_LIST_LEN_CMD
 * @details							This function to get the Enum list length of the DCID through
 *									patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Enum_List_Len_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_ENUM_LIST_LEN_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_ENUM_CMD
 * @details							This function to get the Enum value of the DCID through
 *									patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Enum_DCI_Access( uint8_t* temp_buffer )
{
	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_GET_ENUM_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_RAM_TO_INIT_CMD
 * @details							This function to Set the Ram value of the DCID to NV value
 *									through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Ram_To_Init_DCI_Access( uint8_t* temp_buffer )
{

	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_RAM_TO_INIT_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_1_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_INIT_TO_RAM_CMD
 * @details							This function to load the NV Value to Ram value
 *									of the DCID through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Init_To_Ram_DCI_Access( uint8_t* temp_buffer )
{

	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_INIT_TO_RAM_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_1_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_DEFAULT_TO_INIT_CMD
 * @details							This function to load the Default value to NV Value
 *									of the DCID through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Default_To_Init_DCI_Access( uint8_t* temp_buffer )
{

	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_DEFAULT_TO_INIT_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_1_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_DEFAULT_TO_RAM_CMD
 * @details							This function to load the Default value to Ram Value
 *									of the DCID through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @n
 **********************************************************************************************
 */
void Test_Default_To_Ram_DCI_Access( uint8_t* temp_buffer )
{

	DCI_ACCESS_ST_TD access_struct;
	DCI_ERROR_TD dci_error;

	BF_ASSERT( temp_buffer != nullptr );

	access_struct.command = DCI_ACCESS_DEFAULT_TO_RAM_CMD;
	access_struct.data_access.offset = ACCESS_OFFSET_NONE;
	access_struct.data_access.length = ACCESS_LENGTH_GET_ALL;
	/*please use the DCID  that needs to be tested for DCI Access*/
	access_struct.data_id = DCI_TEST_2_DCID;
	access_struct.data_access.data = temp_buffer;

	/*checks if any error occurs*/
	dci_error = m_patron_access->Data_Access( &access_struct );
	BF_ASSERT( dci_error == DCI_ERR_NO_ERROR );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_DEFAULT_CMD
 *									and DCI_ACCESS_GET_LENGTH_CMD
 * @details							This function to get the length/Default value
 *									of the DCID through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @note							Enable one example at one time
 **********************************************************************************************
 */
void Test_DCI_Access_Length( void )
{
	uint8_t temp1 = 0U;

	BF_ASSERT( m_patron_access == static_cast<DCI_Patron*>( nullptr ) );
	m_patron_access = new DCI_Patron();

	// Function to Get Defualt RAM value using patron
	Test_Read_Default_DCI_Access( &temp1 );

	// Function to Get length of RAM value using patron
	// Test_Read_length_DCI_Access(  &temp1 );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_MIN_CMD
 *									and DCI_ACCESS_GET_MAX_CMD
 * @details							This function to get the Max/Min value
 *									of the DCID through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @note							Enable one example at one time
 **********************************************************************************************
 */
void Test_DCI_Access_Range( void )
{
	uint8_t temp1 = 0U;

	BF_ASSERT( m_patron_access == static_cast<DCI_Patron*>( nullptr ) );
	m_patron_access = new DCI_Patron();

	// Function to Get Minimum RAM value using patron
	// Test_Read_Min_DCI_Access( &temp1 );

	// Function to Get Maximum RAM value using patron
	Test_Read_Max_DCI_Access( &temp1 );

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_GET_ENUM_LIST_LEN_CMD,
 *									DCI_ACCESS_GET_ENUM_CMD and DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD
 * @details							This function to get the Enum list length (or) Enum value
 *									(or) get the attribute information of the DCID through 
 *									patron access
 *									
 * @param[in] void					none
 * @return[out] void				none
 * @note							Enable one example at one time
 **********************************************************************************************
 */
void Test_DCI_Access_Enum( void )
{
	uint8_t temp1 = 0U;

	BF_ASSERT( m_patron_access == static_cast<DCI_Patron*>( nullptr ) );
	m_patron_access = new DCI_Patron();

	// Function to Get Enum count RAM value using patron
	// Test_Enum_List_Len_DCI_Access( &temp1 );

	// Function to Get Enum RAM value using patron
	// Test_Enum_DCI_Access( &temp1);

	// Function to Get the attribute settings of the DCID
	// Test_Get_Attrib_Support_DCI_Access(&temp1);

}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_RAM_TO_INIT_CMD,
 *								    DCI_ACCESS_INIT_TO_RAM_CMD
 * @details							This function to load Ram value to NV value
 *									(or) load the NV value to the Ram value
 *									of the DCID through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @note							Enable one example at one time
 **********************************************************************************************
 */
void Test_DCI_Ram_To_Init( void )
{
	uint8_t temp1 = 0U;

	BF_ASSERT( m_patron_access == static_cast<DCI_Patron*>( nullptr ) );
	m_patron_access = new DCI_Patron();

	// Function to load the Ram value to NV value using patron
	// Test_Ram_To_Init_DCI_Access( &temp1 );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " RAM value of DCID is  %d", temp1 );

	// Function to load the NV value to RAM value using patron
	Test_Init_To_Ram_DCI_Access( &temp1 );
}

/**
 **********************************************************************************************
 * @brief							Provides an example for DCI_ACCESS_DEFAULT_TO_INIT_CMD,
 *								    DCI_ACCESS_DEFAULT_TO_RAM_CMD
 * @details							This function to load default value to NV value
 *									(or) load the default value to the Ram value
 *									of the DCID through patron access
 *
 * @param[in] void					none
 * @return[out] void				none
 * @note							Enable one example at one time
 **********************************************************************************************
 */
void Test_DCI_Default_Ram_To_Init( void )
{

	uint8_t temp1 = 0U;
	uint8_t test_var_default = 5u;

	BF_ASSERT( m_patron_access == static_cast<DCI_Patron*>( nullptr ) );
	m_patron_access = new DCI_Patron();

	// Function to load Default value to NV value using patron
	// Test_Default_To_Init_DCI_Access( &temp1 );

	// Function to load Default value to value using patron
	Test_Default_To_Ram_DCI_Access( &test_var_default );

	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, " after pattern access RAM value of DCID is  %d", test_var_default );

}

// ~~~~