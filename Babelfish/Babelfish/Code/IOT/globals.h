/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/*
 * File:        globals.h
 * Author:      Eaton
 * Created On:  May 23, 2016, 1:32 PM
 */

#ifndef GLOBALS_H
#define GLOBALS_H


/* *********************************************************************************************
     SECTION  START  |             USER-MODIFIABLE VARIABLES                 | SECTION  START
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*
 * Do you want this application to run with the predefined, sample data? If yes (default),
 * then proceed to execute the application. If not, change useSampleData to 'false' from 'true'
 * and modify the values associated with the user-defined variables, below,
 * prefixed with "custom*".
 *
 * DEFAULT: true
 */
bool useSampleData = true;

/*
 * If useSampleData is set to false, review and\or update custom (e.g., custom*)
 * user-defined variable values
 *
 * (The channel tags\identifiers should reference a standard, active MCL MasterName, such as: mACIavg, mWtotal,
 * mTotalEnergy, etc.
 *
 */
char* customConnectionString =
	"HostName=[Enter Your IoT Hub Prefix Name Here].azure-devices.net;DeviceId=[Enter Your Device Name Here];SharedAccessKey=[Enter the DeviceId's SharedAccessKey Here]";
char* customDeviceUUID = "00000000-0000-0000-00000000000000000";
char* customDeviceProfileUUID = "00000000-0000-0000-00000000000000000";
char* customDeviceName = "Device Model Name";
char* customDeviceSerialNumber = "Device Serial Number";
char* customDeviceAssetTag = "Device Asset Tag";
char* customDeviceMAC = "Device MAC";
char* customChannelTag = "MCL_MasterName";


/*
 * The number of minutes between requests for data, respectively, from
 * any given, connected device.
 *
 * If 0, the default interval (i.e., every 10 seconds for trend, 5 seconds for real-time, etc...)
 * will be used as the respective callback interval, where applicable
 *
 * DEFAULT: 0
 */
int customTrendInterval = 0;
int customRealtimeInterval = 0;

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    END SECTION |                      USER-MODIFIABLE VARIABLES                | END SECTION
 **********************************************************************************************/


/* ###########################################################################################

             >>>>>      WARNING - DO NOT EDIT BELOW THIS LINE - WARNING      <<<<<

 ###########################################################################################*/



/* *********************************************************************************************
                       (DO NOT MODIFY) DEFAULT VARIABLES (DO NOT MODIFY)
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/**
 *  Interval-related variables. values entered will represent the number of seconds
 *  between sending and/or requests to send data
 *
 * !DO NOT MODIFY!
 */
int defaultTrendInterval = 10;
int defaultRealtimeInterval = 5;

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                       (DO NOT EDIT) SAMPLE DATA (DO NOT EDIT)
 **********************************************************************************************/

/* *********************************************************************************************
                       (DO NOT MODIFY) SAMPLE DATA (DO NOT MODIFY)
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*
 * Predefined, sample data to be used if useSampleData is 'true' (default).
 *
 * !DO NOT MODIFY!
 *
 */
char* sampleConnectionString =
	"HostName=EatonTestIothub2.azure-devices.net;DeviceId=EatonIoTDeviceSDKClient-DummyDevice1;SharedAccessKey=1OH25k28sx5fVGXfcuOfbGL854ftB57k7bOA9KhnX5A=";
char* sampleDeviceUUID = "EatonIoTDeviceSDKClient-DummyDevice1";
char* sampleDeviceProfileUUID = "d4967aa8-28d9-11e6-b67b-9e71128cae77";	/* This Device Profile UUID is tied to the
																		   Beagle Bone BBB-IoT-Demo Profile --
																		   http://esgit.tcc.etn.com:7990/projects/IOT/repos/platform_device_profile/browse/LinuxToolkit/BeagleBoneBlack_3.1.json
																		 */
char* sampleDeviceName = "SDKClientDevice1";
char* sampleDeviceSerialNumber = "SerialNumber";
char* sampleDeviceAssetTag = "AssetTag";
char* sampleDeviceMAC = "MAC";
char* sampleChannelTag = "mACIA";	/* Tag ID = 41 */

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                       (DO NOT EDIT) SAMPLE DATA (DO NOT EDIT)
 **********************************************************************************************/

/* *********************************************************************************************
                       (DO NOT MODIFY) GLOBAL APPLICATION VARIABLES (DO NOT MODIFY)
   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*
 *  The variables in this section are the 'global' application variables, which are
 *  derived from the predefined, sample or user-defined data members. The client code
 *  will reference the values of these variables, throughout.
 *
 * !DO NOT MODIFY!
 */
bool globalsInitialized = false;
int maxDevicesSupported = 1;/* This is a single device (low-cost), stand-alone sample app for now */


int trendInterval = 0;
int realtimeInterval = 0;

int numConnStrings = 0;
int numDeviceUUIDs = 0;
int numDeviceProfileUUIDs = 0;
int numDeviceNames = 0;
int numDeviceSerialNumbers = 0;
int numDeviceAssetTags = 0;
int numDeviceMACs = 0;
int numChannelIds = 0;

const char* connectionString;
const char* deviceUUID;
const char* deviceProfileUUID;
const char* deviceName;
const char* deviceSerialNumber;
const char* deviceAssetTag;
const char* deviceMAC;
const char* channelTag;

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                       (DO NOT MODIFY) GLOBAL APPLICATION VARIABLES (DO NOT MODIFY)
 **********************************************************************************************/


void setGlobals();

/* ###########################################################################################

             >>>>>      WARNING - DO NOT EDIT ABOVE THIS LINE - WARNING      <<<<<

 ###########################################################################################*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif	/* GLOBALS_H */