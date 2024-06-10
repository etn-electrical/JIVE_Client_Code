/**
 *****************************************************************************************
 *	@file DevObject.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "DevObject.h"
#include "vsbhp.h"
#include "string.h"
#include "stdio.h"
#include "Util.h"
#include "BACnetDCIInterface.h"
#include "BACnet_DCI.h"

static deviceInfo deviceObjectInfo;
extern BACnet_DCI* BACnetDCIPtr;

uint32_t ourinstance = 108U;
static dword ourdatabaserev;
static byte ourapduretries = 0U;
static byte ourapdutimeout = 6U;
static byte ourstation = 0U;
static byte ourmaxmaster;	// =127;
static byte ourmaxinfoframes;	// =1;
static byte ourvendorid;// = 191

static char_t ourDeviceNameStr[DEVICE_NAME_MAX_LEN];// Fixed DH1-217
static frString ourdevicename;
static char_t ourvendornameStr[32U];
static frString ourvendorname;
static char_t ourmodelnameStr[32U];
static frString ourmodelname;	// ={7,64,"Frame 1"};
static char_t ourfirmwarerevStr[32U];
static frString SerialNumber;
static frString ourfirmwarerev;	// ={3,64,"1.0"};
static frString ourappver;	// ={3,64,"0.1"};
static char_t ourdevicedescriptionStr[32U];
static frString ourdevicedescription;	// ={24,64,"Variable Frequency Drive"};
static char_t ourProfileNameStr[32U];
static frString ourProfileName;	// = {5,64, "B-ASC"};
static char_t ourPasswordStr[20U];
static frString ourPassword =
{ 0U, 20U, "" };

static word ourbipudp;
static dword ourbipipaddr;
static dword ourbipipsubnet;

// New BBMD specific parameters added //Prateek
static dword ourbbmdipaddr;
static word ourbbmdudp;
static word ourbbmdttl;
static uint32_t udwSerialNumber;
extern bool_t Is_Bacnet_Ip_Enabled();

void Init_Device_Info( void );

extern uint8_t Update_Our_Station( void );

// Nishchal: Added this function to get the string serial number. DH1-45
void toString( char_t str[], uint32_t srno );

/** @fn     void  fraGetDeviceInfo(frDevInfo *pdev)
 *   @brief : get the Device object
 *   @details : get the Device Object properties
 *   @param[]  pdev	points to structure to be filled-in
 *   @return:None
 */
void fraGetDeviceInfo( frDevInfo* pdev )
{
	Init_Device_Info();
	memcpy( pdev, &deviceObjectInfo, sizeof( frDevInfo ) );
	pdev->object_types_supported[0U] = ( vsbots_AV ) | ( vsbots_BV ) | ( vsbots_AI );	// AV object, BV object, AI
																						// object
	pdev->object_types_supported[1U] = ( vsbots_DEV );					// DEV object (always)
	pdev->object_types_supported[2U] = ( vsbots_MSV );				// Nishchal for DH1-49
	pdev->object_types_supported[3U] = 0U;
}

// Nishchal: Added this function to get the serial number in ASCII (linked with DH1-45)
/** @fn     void toString(char_t str[], uint32_t srno)
 *   @brief : takes in the serial number uint32_t and provides result in String in str[] i.e. ASCII serial number
 *   @details : required as we need Serial Number in char_t string.
 *   @param[]  str[] write in the converted serial number in string format.
 *   @param[]  srno serial number read from parameter in uint32_t
 *   @return: None
 */
void toString( char_t str[], uint32_t srno )
{
	uint8_t i;
	uint8_t rem;
	uint8_t len = 0U;
	uint32_t u32srno = srno;

	if ( srno == 0 )
	{
		len = 1;
	}
	else
	{
		while ( u32srno != 0U )
		{
			len++;
			u32srno /= 10U;
		}
	}

	for ( i = 0U; i < len; i++ )
	{
		rem = srno % 10U;
		srno = srno / 10U;
		str[len - ( i + 1U )] = rem + '0';
	}
	str[len] = '\0';
}

/** @fn     int32_t bpublic fraReinitDevice(int32_t state, frString *pass)
 *   @brief : Reinitialise device from BACnet network
 *   @details :
 *   @param[in] state  0=cold, 1=warm
 *   @param[in] pass  NULL or string password
 *   @return: 0 success, -1 bad password, -2 can't find it
 */
int32_t bpublic fraReinitDevice( int32_t state, frString* pass )
{
	int32_t retVal = 0;

	if ( pass == NULL )	// if user has not given password
	{
		if ( deviceObjectInfo.app_password->len == 0 )	// check if device password is also empty
		{
			// User has not given password and Device password is also empty hence reboot
			BF::System_Reset::Reset_Device( BF::System_Reset::SOFT_RESET );	// Check if Write Ack has send before reboot
		}
		else
		{
			retVal = -1;// bad password
		}
	}
	else
	{
		if ( ( ( ( pass->len != 0U ) && ( deviceObjectInfo.app_password->len != 0U ) ) &&	// Neither password is empty
																							// -AND-
			   ( pass->len == ( dword )deviceObjectInfo.app_password->len ) &&	// the password lengths are equal -AND-
			   ( _strnicmp( pass->text, deviceObjectInfo.app_password->text, ( size_t )pass->len ) == 0 ) )	// the
																											// password
																											// strings
																											// are
																											// equivalent	***EATON
			  )
		{
			// reboot
			BF::System_Reset::Reset_Device( BF::System_Reset::SOFT_RESET );	// Check if Write Ack has send before reboo
		}
		else
		{
			retVal = -1;// bad password
		}
	}

	return ( retVal );
}

frTimeDate ts_g;// For testing later remove it

/** @fn bool_t  bpublic fraSetTimeDate(frTimeDate *ts)
 *   @brief :Save the corrected time
 *   @details :
 *   @param[in]  ts		points to structure which carries date and time
 *   @return: TRUE	if time/date are supported, else FALSE
 */
bool_t bpublic fraSetTimeDate( frTimeDate* ts )
{
	// save it in local Clock
	ts_g.hour = ts->hour;								// 0..23
	ts_g.minute = ts->minute;								// 0..59
	ts_g.second = ts->second;								// 0..59
	ts_g.hundredths = ts->hundredths;							// 0..99

	ts_g.year = ts->year;
	ts_g.month = ts->month;								// 1..12
	ts_g.day = ts->day;									// 1..31
	ts_g.weekday = ts->weekday;								// 1..7 (1=Monday)
	ts_g.dst = ts->dst;									// 0=no, 1=in effect
	ts_g.utcoffset = ts->utcoffset;	// -720..+720 time is offset from UTC by this amount of minutes
	return ( TRUE );
}

/** @fn bool_t  bpublic fraGetTimeDate(frTimeDate *ts)
 *   @brief :Find out the time
 *   @details :
 *   @param[out]  ts		points to structure to be filled-in
 *   @return: TRUE	if time/date are supported, else FALSE
 */
bool_t bpublic fraGetTimeDate( frTimeDate* ts )
{
	// TODO: SET TIME AND DATE from your local clock/calendar

	ts->hour = ts_g.hour;								// 0..23
	ts->minute = ts_g.minute;								// 0..59
	ts->second = ts_g.second;								// 0..59
	ts->hundredths = ts_g.hundredths;							// 0..99

	ts->year = ts_g.year;
	ts->month = ts_g.month;								// 1..12
	ts->day = ts_g.day;									// 1..31
	ts->weekday = ts_g.weekday;								// 1..7 (1=Monday)
	ts->dst = ts_g.dst;									// 0=no, 1=in effect
	ts->utcoffset = ts_g.utcoffset;	// -720..+720 time is offset from UTC by this amount of minutes
	return ( TRUE );								// we do support time/date
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
dword bpublic fraGetDatabaseRevision( void )
{
	return ( ourdatabaserev );
}

/** @fn int32_t bpublic fraSaveDeviceVars(frDevInfo *di)
 *   @brief : Save the values on non volatile memory
 *   @param[in]   di Pointer to Device Object whose values needs to be saved
 *   @return: vsbe_noerror there is no error in Saving the parameters
 */
int32_t bpublic fraSaveDeviceVars( frDevInfo* di )
{
	ourinstance = di->deviceinstance;
	// utcoffset=di->utcoffset;
	ourstation = di->mstpstation;
	ourmaxmaster = di->maxmaster;
	// To update changed value of max master // DH1-131
	BACnetDCIPtr->Write_Param( DCI_MAX_MASTER_DCID, ( UINT8* )&ourmaxmaster, DCI_ACCESS_SET_RAM_CMD );
	ourmaxinfoframes = di->maxinfoframes;
	ourapduretries = di->apduretries;
	ourapdutimeout = di->apdu_timeout;
	// Fixed DH1-217 to make Object_name writable
	BACnetDCIPtr->Write_Param( DCI_PRODUCT_NAME_DCID, ( UINT8* )ourdevicename.text, DCI_ACCESS_SET_RAM_CMD );
	return ( vsbe_noerror );
}

/** @fn void bpublic fraIncrementDatabaseRevision(void)
 *   @brief :increment the BACnet database revision
 *   @details : This function should get called whenever there is a change in Object Nane
 *   @param[]  None
 *   @return: None
 */
void bpublic fraIncrementDatabaseRevision( void )
{
	ourdatabaserev++;
	// Fixed DH1-217 to make Database Revision writable
	BACnetDCIPtr->Write_Param( DCI_BACNET_DATABASE_REVISION_DCID, ( UINT8* )&ourdatabaserev, DCI_ACCESS_SET_RAM_CMD );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Init_Device_Info()
{
	static DCI_Owner* ub_BACnet_Vendor_Identifier = new DCI_Owner( getDCIDForDevObject( 1 ) );
	static DCI_Owner* ub_Max_Info_Frames = new DCI_Owner( getDCIDForDevObject( 4 ) );
	static DCI_Owner* ub_Profile_Name = new DCI_Owner( getDCIDForDevObject( 5 ) );

	// our MS/TP Device address
	ourstation = Update_Our_Station();
#ifndef ESP32_SETUP
	// Reading the value of MAX MASTER at power up // DH1-131
	BACnetDCIPtr->Read_Param( DCI_MAX_MASTER_DCID, ( UINT8* )&ourmaxmaster,
							  DCI_ACCESS_GET_RAM_CMD );
	BACnetDCIPtr->Read_Param( DCI_BACNET_DATABASE_REVISION_DCID, ( UINT8* )&ourdatabaserev,
							  DCI_ACCESS_GET_RAM_CMD );		// This code is for STM so exclude from This
#endif
	// Device Instance
	if ( Is_Bacnet_Ip_Enabled() == TRUE )
	{
		BACnetDCIPtr->Read_Param( DCI_BACNET_IP_INSTANCE_NUM_DCID, ( uint8_t* )&ourinstance,
								  DCI_ACCESS_GET_RAM_CMD );
	}
	else
	{
		BACnetDCIPtr->Read_Param( DCI_BACNET_MSTP_INSTANCE_NUM_DCID, ( uint8_t* )&ourinstance,
								  DCI_ACCESS_GET_RAM_CMD );
	}
#ifndef ESP32_SETUP
	// Vendor identifier
	ub_BACnet_Vendor_Identifier->Check_Out( ourvendorid );	// This code is for STM so exclude from This
#endif
	// Max master
	// Max info frames
	ub_Max_Info_Frames->Check_Out( ourmaxinfoframes );

	/****************handle strings******************/
	// Device Name
	BACnetDCIPtr->Read_Param( DCI_PRODUCT_NAME_DCID, ( uint8_t* )&ourDeviceNameStr,
							  DCI_ACCESS_GET_RAM_CMD );
	ourdevicename.text = ourDeviceNameStr;
	// Vendor name
	BACnetDCIPtr->Read_Param( DCI_VENDOR_NAME_DCID, ( uint8_t* )&ourvendornameStr,
							  DCI_ACCESS_GET_RAM_CMD );
	ourvendorname.text = ourvendornameStr;
	// Model name
	BACnetDCIPtr->Read_Param( DCI_MODEL_NAME_DCID, ( uint8_t* )&ourmodelnameStr,
							  DCI_ACCESS_GET_RAM_CMD );
	ourmodelname.text = ourmodelnameStr;
	// profile name
	ub_Profile_Name->Check_Out( ourProfileNameStr );
	ourProfileName.text = ourProfileNameStr;
	// device description
	BACnetDCIPtr->Read_Param( DCI_USER_APP_NAME_DCID, ( uint8_t* )&ourdevicedescriptionStr,
							  DCI_ACCESS_GET_RAM_CMD );
	ourdevicedescription.text = ourdevicedescriptionStr;
	// firmware revision
	uint32_t app_version = 0U;
	uint8_t temp_version[4] = {0};

	BACnetDCIPtr->Read_Param( DCI_APP_FIRM_VER_NUM_DCID, ( uint8_t* )( &temp_version ),
							  DCI_ACCESS_GET_RAM_CMD );
	app_version = temp_version[0] << 24 | temp_version[1] << 16 | temp_version[3] << 8 | temp_version[2];
	Convert_Rev_Num_To_Ascii( app_version, reinterpret_cast<uint8_t*>( ourfirmwarerevStr ) );
	BACnetDCIPtr->Read_Param( DCI_PRODUCT_SERIAL_NUM_DCID, ( uint8_t* )&udwSerialNumber,
							  DCI_ACCESS_GET_RAM_CMD );
	static char_t str_srno[11U];// Nishchal: max value of uint32_t is 4294967295; 10 char_t bytes + 1 string termination

	// character.

	toString( str_srno, udwSerialNumber );
	SerialNumber.text = str_srno;
	SerialNumber.len = strlen( SerialNumber.text );
	SerialNumber.maxlen = 11U;
	SerialNumber.readonly = 0U;
#ifdef ESP32_SETUP
	// Reading the value of MAX MASTER at power up // DH1-131
	BACnetDCIPtr->Read_Param( DCI_MAX_MASTER_DCID, ( UINT8* )&ourmaxmaster,
							  DCI_ACCESS_GET_RAM_CMD );
	BACnetDCIPtr->Read_Param( DCI_BACNET_DATABASE_REVISION_DCID, ( UINT8* )&ourdatabaserev,
							  DCI_ACCESS_GET_RAM_CMD );

	// Vendor identifier
	ub_BACnet_Vendor_Identifier->Check_Out( ourvendorid );
	/*we change the position ,because this database revision data begin is overwritten by Device Name data.*/
#endif

	// strcpy(ourfirmwarerev.text, ourfirmwarerevStr);
	ourfirmwarerev.text = ourfirmwarerevStr;
	// Application Software version
	// ubStandardAppVersion_g->Check_Out(ourApprevStr8);
	// Static_Patron::Data_Access(DCI_ubStandardAppVersion_DCID,(uint8_t*)ourApprevStr8, DCI_ACCESS_GET_RAM_CMD );
	// sprintf(ourappverStr,"%d", ourApprevStr8[0] );
	// strcpy(ourappver.text, ourappverStr);
	// strcpy(ourappver.text, ourfirmwarerevStr); //App version is same as Firmware Rev
	ourappver.text = ourfirmwarerevStr;	// App version is same as Firmware Rev
	// strcpy(ourPassword.text, ourPasswordStr);
	ourPassword.text = ourPasswordStr;

	ourdevicename.len = strlen( ourdevicename.text );
	ourdevicename.maxlen = DEVICE_NAME_MAX_LEN;
	ourvendorname.len = strlen( ourvendorname.text );
	ourmodelname.len = strlen( ourmodelname.text );
	ourfirmwarerev.len = strlen( ourfirmwarerev.text );
	ourappver.len = strlen( ourappver.text );
	ourdevicedescription.len = strlen( ourdevicedescription.text );
	ourProfileName.len = strlen( ourProfileName.text );
	ourPassword.len = strlen( ourPassword.text );
	ourPassword.readonly = 0;

	BACnetDCIPtr->Read_Param( DCI_ACTIVE_IP_ADDRESS_DCID, ( uint8_t* )&ourbipipaddr,
							  DCI_ACCESS_GET_RAM_CMD );
	BACnetDCIPtr->Read_Param( DCI_ACTIVE_SUBNET_MASK_DCID, ( uint8_t* )&ourbipipsubnet,
							  DCI_ACCESS_GET_RAM_CMD );
	BACnetDCIPtr->Read_Param( DCI_BACNET_IP_UPD_PORT_NUM_DCID, ( uint8_t* )&ourbipudp,
							  DCI_ACCESS_GET_RAM_CMD );
	BACnetDCIPtr->Read_Param( DCI_BACNET_IP_BBMD_IP_DCID, ( uint8_t* )&ourbbmdipaddr,
							  DCI_ACCESS_GET_RAM_CMD );
	BACnetDCIPtr->Read_Param( DCI_BACNET_IP_BBMD_PORT_DCID, ( uint8_t* )&ourbbmdudp,
							  DCI_ACCESS_GET_RAM_CMD );
	BACnetDCIPtr->Read_Param( DCI_BACNET_IP_REGISTRATION_INTERVAL_DCID, ( uint8_t* )&ourbbmdttl,
							  DCI_ACCESS_GET_RAM_CMD );

	deviceObjectInfo.app_device_name = &ourdevicename;
	deviceObjectInfo.app_vendor_name = &ourvendorname;
	deviceObjectInfo.app_model_name = &ourmodelname;
	deviceObjectInfo.app_firmware_rev = &ourfirmwarerev;
	deviceObjectInfo.app_application_software_ver = &ourappver;
	deviceObjectInfo.app_device_description = &ourdevicedescription;
	deviceObjectInfo.app_device_profile = &ourProfileName;
	deviceObjectInfo.app_password = &ourPassword;
	deviceObjectInfo.app_deviceinstance = ourinstance;			// our BACnet device instance
	deviceObjectInfo.app_vendor_id = ourvendorid;				// Eaton vendorID
	deviceObjectInfo.app_maxmaster = ourmaxmaster;				// maxmaster
	deviceObjectInfo.app_mstpstation = ourstation;
	deviceObjectInfo.app_maxinfoframes = ourmaxinfoframes;
	deviceObjectInfo.app_system_status = OPERATIONAL;			// see BACnetDeviceStatus
	deviceObjectInfo.app_apdu_timeout = ourapdutimeout;			// in whole seconds
	deviceObjectInfo.app_apduretries = ourapduretries;

	deviceObjectInfo.app_bipudp = ourbipudp;
	deviceObjectInfo.app_bipipaddr = ourbipipaddr;
	deviceObjectInfo.app_bipipsubnet = ourbipipsubnet;

	deviceObjectInfo.app_bbmdipaddr = ourbbmdipaddr;
	deviceObjectInfo.app_bbmdudp = ourbbmdudp;
	deviceObjectInfo.app_bbmdttl = ourbbmdttl;

	deviceObjectInfo.serial_number = &SerialNumber;
}
