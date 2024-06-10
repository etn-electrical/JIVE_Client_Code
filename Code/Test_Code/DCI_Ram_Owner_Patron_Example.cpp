/**
 *****************************************************************************************
 *	@file		DCI_Ram_Owner_Example.cpp
 *
 *	@brief		The file contains the DCI Owner,Patron basic testing for RAM functionality
 *
 *****************************************************************************************
 */

#include <stdio.h>
#include "Includes.h"
#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Etn_Types.h"
#include "DCI_Ram_Owner_Patron_Example.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*
 *****************************************************************************************
 *		DCI owners for testing check_in and check_out
 *****************************************************************************************
 */
DCI_Owner* bool_data_test_owner;
DCI_Owner* uint8_data_test_owner;
DCI_Owner* uint16_data_test_owner;
DCI_Owner* uint64_data_test_owner;
DCI_Owner* float_data_test_owner;
DCI_Owner* dfloat_data_test_owner;
DCI_Owner* sint8_data_test_owner;
DCI_Owner* sint16_data_test_owner;
DCI_Owner* sint32_data_test_owner;
DCI_Owner* sint64_data_test_owner;
DCI_Owner* string8_data_test_owner;

/// DCI owners for testing Attributes
DCI_Owner* attributes_test1_owner;
DCI_Owner* attributes_test2_owner;

/// DCI owners for testing Lock & Unlock
DCI_Owner* test_lock_unlock;

/// DCI owners for testing Default check In & Out
DCI_Owner* test_default_check_in_out;

// DCI Patron access tests
DCI_Patron* m_dci_access;
DCI_SOURCE_ID_TD m_source_id;

/*
 *****************************************************************************************
 *      Variables
 *****************************************************************************************
 */
static char var_temp = 0U;
bool test_value = false;
uint8_t test_value1 = 28U;
uint16_t test_value2 = 500U;
uint32_t test_value3 = 123456789U;
uint64_t test_value4 = 87654321U;
float32_t test_value5 = 500.0;
float64_t test_value6 = 558.0;
int8_t test_value7 = -15;
int16_t test_value8 = -500;
int32_t test_value9 = -600;
int64_t test_value10 = -123;
uint8_t test_value14[33] = { "Administrator"
};
uint8_t test_value15[33] = { "Default Value Test" };
bool_t retVal = false;
/*
 *****************************************************************************************
 *      Constants
 *****************************************************************************************
 */
static const uint8_t TEST_VALUE20 = 52U;
static const uint16_t TEST_VALUE21 = 513U;
static const uint32_t TEST_VALUE31 = 1514955660U;
static const uint64_t TEST_VALUE41 = 54321456U;
static const float32_t TEST_VALUE51 = 528.9;
static const float64_t TEST_VALUE61 = 543.4;
static const int8_t TEST_VALUE71 = -28;
static const int16_t TEST_VALUE81 = -550;
static const int32_t TEST_VALUE91 = -650;
static const int64_t TEST_VALUE101 = -200;
static const uint8_t TEST_VALUE141[33] = { "ESP32_Evaluation" };

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void DCI_CheckIn_CheckOut_Data_Variables_Test( void )
{
	if ( var_temp == 0U )
	{
		/// DCI_Owner for bool datatype
		bool_data_test_owner = new DCI_Owner( DCI_HTTP_ENABLE_DCID );

		/// DCI_Owner for uint_8 datatype
		uint8_data_test_owner = new DCI_Owner( DCI_ETH_ACD_CONFLICTED_STATE_DCID );

		/// DCI_Owner for uint_16 datatype
		uint16_data_test_owner = new DCI_Owner( DCI_MODBUS_SERIAL_COMM_TIMEOUT_DCID );

		/// DCI_Owner for string  datatype
		string8_data_test_owner = new DCI_Owner( DCI_USER_APP_NAME_DCID );


		/// DCI_Owner for float_32 datatype
		float_data_test_owner = new DCI_Owner( DCI_LOG_TEST_1_DCID );

		/// DCI_Owner for int_8 datatype
		sint8_data_test_owner = new DCI_Owner( DCI_BLE_TEST_SETTING_2_DCID );

		/// DCI_Owner for int_16 datatype
		sint16_data_test_owner = new DCI_Owner( DCI_BLE_TEST_SETTING_5_DCID );

		/// DCI_Owner for int_32 datatype
		sint32_data_test_owner = new DCI_Owner( DCI_BLE_TEST_SETTING_6_DCID );

		/// DCI_Owner for int_64 datatype
		sint64_data_test_owner = new DCI_Owner( DCI_BLE_TEST_EVENT_LIST_DCID );


		var_temp = 1U;
	}


	printf( "\n----Check in & Check Out different types of Data Types test (using DCID's) ------\n" );

	bool_data_test_owner->Check_In( false );
	bool_data_test_owner->Check_Out( test_value );

	if ( test_value == true )
	{
		printf( ( "Check_out  value of bool_data_test_owner After Check_In( false ) : TRUE\n" ) );
	}
	else
	{
		printf( ( "Check_out value of bool_data_test_owner after Check_In( false ) : FALSE\n" ) );
	}

	bool_data_test_owner->Check_In( true );
	bool_data_test_owner->Check_Out( test_value );

	if ( test_value == true )
	{
		printf( ( " value of bool_data_test_owner after Check_In( true )  : TRUE\n" ) );
	}
	else
	{
		printf( ( " value of bool_data_test_owner Check_In( true ) : FALSE\n" ) );
	}


	uint8_data_test_owner->Check_In( static_cast<uint8_t>( TEST_VALUE20 ) );
	uint8_data_test_owner->Check_Out( test_value1 );
	printf( "Value of uint8_data_test_owner : %d\n", test_value1 );


	uint16_data_test_owner->Check_In( static_cast<uint16_t>( TEST_VALUE21 ) );
	uint16_data_test_owner->Check_Out( test_value2 );
	printf( "Value of uint16_data_test_owner : %d\n", test_value2 );

	string8_data_test_owner->Check_In( TEST_VALUE141 );
	string8_data_test_owner->Check_Out( test_value14 );
	printf( "Value of string8_data_test_owner : %s\n", test_value14 );

	float_data_test_owner->Check_In( static_cast<float32_t>( TEST_VALUE51 ) );
	float_data_test_owner->Check_Out( test_value5 );
	printf( "Value of float_data_test_owner : %f\n", test_value5 );

	sint8_data_test_owner->Check_In( static_cast<int8_t>( TEST_VALUE71 ) );
	sint8_data_test_owner->Check_Out( test_value7 );
	printf( "Value of sint8_data_test_owner : %d\n", test_value7 );

	sint16_data_test_owner->Check_In( static_cast<int16_t>( TEST_VALUE81 ) );
	sint16_data_test_owner->Check_Out( test_value8 );
	printf( "Value of sint16_data_test_owner : %d\n", test_value8 );


	sint32_data_test_owner->Check_In( static_cast<int32_t>( TEST_VALUE91 ) );
	sint32_data_test_owner->Check_Out( test_value9 );
	printf( "Value of sint32_data_test_owner : %d\n", test_value9 );

	sint64_data_test_owner->Check_In( static_cast<int64_t>( TEST_VALUE101 ) );
	sint64_data_test_owner->Check_Out( test_value10 );
	printf( "Value of sint64_data_test_owner : %f\n", ( float64_t ) test_value10 );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void DCI_Test_Owner_Attributes( void )
{

	/// DCI owner test attributes

	printf( "\n-------------DCI Get Owner Attributes test --------\n" );

	/// DCI_Owner for attributes_test1
	attributes_test1_owner = new DCI_Owner( DCI_DEVICE_STATE_DCID );

	DCI_OWNER_ATTRIB_TD retrivedAttributes = 0x0;

	retrivedAttributes = attributes_test1_owner->Get_Owner_Attrib();
	printf( "retrieved owner Attributes of DCI_DEVICE_STATE_DCID in Hex = %x \n", retrivedAttributes );

	retrivedAttributes = attributes_test1_owner->Get_Patron_Attrib();
	printf( "retrieved Patron Attributes of DCI_DEVICE_STATE_DCID in Hex = %x \n", retrivedAttributes );

	/// DCI_Owner for attributes_test2
	attributes_test2_owner = new DCI_Owner( DCI_FW_UPGRADE_MODE_DCID );

	retrivedAttributes = 0x0;
	retrivedAttributes = attributes_test2_owner->Get_Owner_Attrib();
	printf( "retrieved Owner Attributes of DCI_FW_UPGRADE_MODE_DCID in Hex = %x \n", retrivedAttributes );

	retrivedAttributes = 0x0;
	retrivedAttributes = attributes_test2_owner->Get_Patron_Attrib();
	printf( "retrived Patron Attributes of DCI_FW_UPGRADE_MODE_DCID in Hex = %x \n", retrivedAttributes );

}

void DCI_CheckIn_Default_CheckOut_Default_Test( void )
{

	printf( "\n-------------DCI Owner Default check In & Check Out --------\n" );
	test_default_check_in_out = new DCI_Owner( DCI_BLE_USER3_NAME_DCID );

	test_default_check_in_out->Check_Out( test_value14 );
	printf( "Value of DCI_BLE_USER3_NAME_DCID (Already available ) : %s\n", test_value14 );

	test_default_check_in_out->Check_In( test_value15 );
	test_default_check_in_out->Check_Out( test_value14 );
	printf( "Value of DCI_BLE_USER3_NAME_DCID(Changed value with Default check In) : %s\n", test_value14 );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void DCI_owner_Test_Lock_Unlock( void )
{

	printf( "\n-------------DCI Owner Test Lock -------\n" );

	/// DCI owner test lock
	test_lock_unlock = new DCI_Owner( DCI_RESET_COUNTER_DCID );

	/// DCI owner test lock
	test_lock_unlock->Lock();
	if ( TRUE == test_lock_unlock->Is_Locked() )
	{
		printf( "Lock status after calling test_lock_unlock->Lock() : TRUE\n" );
	}
	else
	{
		printf( "Lock status after calling test_lock_unlock->Lock() : FALSE\n" );
	}

	/// DCI owner test unlock

	test_lock_unlock->Unlock();
	if ( TRUE == test_lock_unlock->Is_Locked() )
	{
		printf( "Lock status after calling test_lock_unlock->UnLock() : TRUE\n" );
	}
	else
	{
		printf( "Lock status after calling test_lock_unlock->UnLock() : FALSE\n" );
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Patron_access_test( void )
{
	DCI_DATATYPE_TD datatype;
	float32_t val = 0.0F;
	DCI_ERROR_TD dci_error = DCI_ERR_NO_ERROR;
	DCI_ACCESS_ST_TD access_struct;

	printf( "\n-------------DCI Patron Access Tests-------\n" );

	m_dci_access = new DCI_Patron( TRUE );

	m_source_id = DCI_SOURCE_IDS_Get();

	/// Get Data type length using Patron
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_access.data = ( uint8_t* )&val;
	access_struct.source_id = m_source_id;
	m_dci_access->Get_Length( DCI_RESET_COUNTER_DCID, &access_struct.data_access.length );
	printf( "DCI_RESET_COUNTER_DCID data_access.length Using DCI Patron  =%d \n", access_struct.data_access.length );

	/// Check data access using Patron
	access_struct.data_access.offset = 0U;
	access_struct.data_id = DCI_RESET_COUNTER_DCID;
	dci_error = m_dci_access->Data_Access( &access_struct );
	printf( "DCI Patron->Data_Access ( with DCI_ACCESS_GET_RAM_CMD)  return value =%d (DCI_ERR_NO_ERROR)\n",
			dci_error );

	/// Get Data type using Patron
	m_dci_access->Get_Datatype( DCI_RESET_COUNTER_DCID, &datatype );
	printf( "DCI_RESET_COUNTER_DCID Data type using DCI Patron access =%d\n "
			"( 0 :DCI_DTYPE_BOOL,1:	DCI_DTYPE_UINT8,2:DCI_DTYPE_SINT8,3: DCI_DTYPE_UINT16,4:DCI_DTYPE_SINT16 etc )\n",
			datatype );
}

/*
 * @brief  In this example, Basic Test cases DCI owner & Patron are executed  .
 */
void DCI_RAM_Test_App_Main()
{
	DCI_CheckIn_CheckOut_Data_Variables_Test();
	DCI_Test_Owner_Attributes();
	DCI_owner_Test_Lock_Unlock();
	DCI_Patron_access_test();
	DCI_CheckIn_Default_CheckOut_Default_Test();
}

#ifdef __cplusplus
}
#endif
