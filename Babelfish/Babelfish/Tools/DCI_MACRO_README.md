# DCI DB Creator Macros
The DCI DB Creator Macros links all the macro files to the RTK DCI DB Creator file.
This file contains the "Compatibility Number" and "Change Log" information.

compatibility_number: Compatibility number links the compatibility of RTK DCI DB Creator(product spec xlsm) with the DCI DB Creator Macros(macros xlsm) file.
revision_number: Revision number depicts the features/improvements/bug fix provided by the PX-Green team.

** Thou shalt never change the format of the "Change Log Information" section.
** The change log format should be as follows.
$[Space]COMPATIBILITY_NUMBER[Space]=[Space][Decimal value]
[Tab]$[Space]REVISION_NUMBER[Space]=[Space][Float Value]
[Tab]-[Space][Comments]
[Tab]-[Space][Comments]

** Thou shalt only and only change the compatibility number when the changes in the Macro codes require changes in the RTK DCI DB Creator workbook or vice-a-versa.
** The Compatibility number and version number will always be in the decreasing order i.e. latest changes will come first.
** Thou shalt prefer notepad for modifying this file.


## Change Log Information
$ COMPATIBILITY_NUMBER = 16
  $ REVISION_NUMBER = 16.7
  - LTK-11052: Handle all REST DCIDs format handlers from UI side
	$ REVISION_NUMBER = 16.6
	- LTK-11002: Datatype in VBA and parsing of Default Col Cell Value changed for DCID with Float datatype and added 1-6 digit precision for the same.
	$ REVISION_NUMBER = 16.5
	- LTK-6945: The DCI workbook has a mix of fonts . Make consistent with Font Name - 'Arial' and Font size -10.
	$ REVISION_NUMBER = 16.4
	- LTK-10625: DCI Import Export feature merged with dev branch.
	$ REVISION_NUMBER = 16.3
	- LTK-8333: Created DCID for DNS server address. Adopter configuring their own DNS server address in DCI sheet instead of hardcoding.
    $ REVISION_NUMBER = 16.2
	- LTK-10237: Supported conversion of 32bit hexadecimal value to decimal in VBA code 
    $ REVISION_NUMBER = 16.1
	- LTK-3087: During DCI DB Creator Macros.xlsm close, disabled the save pop up message   
	$ REVISION_NUMBER = 16.0
	- LTK-9165: Popup error message added,if Bitfield DCI is not of BYTE, WORD or DWORD datatype		
$ COMPATIBILITY_NUMBER = 15
	$ REVISION_NUMBER = 15.1
	- LTK-9888: IE not supported text message bug fixed
	$ REVISION_NUMBER = 15.0
	- LTK-8210: Added Alarms sheet and VBA code to support the same
$ COMPATIBILITY_NUMBER = 14
	$ REVISION_NUMBER = 14.3
	- LTK-9661: Generate JSON and JSON tree files for all BLE modules
	$ REVISION_NUMBER = 14.2
	- LTK-9678: Generate UI configuration files correctly when "Param Hidden" is not used
	$ REVISION_NUMBER = 14.1
	- LTK-9644: Banner implementation for showing no support for IE
    $ REVISION_NUMBER = 14.0
	- LTK-9162: DCI_Sruct_Builder updated to handle VBA code updating the datatype for Bitfeild_t typedef to maintain the highest dci datatype used for bitfeilds
$ COMPATIBILITY_NUMBER = 13
	$ REVISION_NUMBER = 13.1
	- LTK-9089: Added monetization and Boolean Entitlement realated dci id's 
	$ REVISION_NUMBER = 13.0
	- LTK-9107: BLE Sheet macro updated to handle all three different BLE Modules (Silabs,STM32WB55,ESP32) by using drop down button.
$ COMPATIBILITY_NUMBER = 12
	$ REVISION_NUMBER = 12.1
	- LTK-8184: Macro changes for removal of format handling from firmware side.
	$ REVISION_NUMBER = 12.0
	- LTK-8353: Implementation of Custom Table component to use DCI info
$ COMPATIBILITY_NUMBER = 11
	$ REVISION_NUMBER = 11.2
	- LTK-8765: Added support for 10 digit enum number in EnumForm.bas file, earlier 5 digit number only supported.
	$ REVISION_NUMBER = 11.1
	- LTK-4689: Support DCI parameters with specific Index value(s)
	$ REVISION_NUMBER = 11.0
	- LTK-8063: Added NV Memory Address Validation in NV Tab
$ COMPATIBILITY_NUMBER = 10
	$ REVISION_NUMBER = 10.6
	- LTK-8427: Added DCI enum support for IOT cadence rate DCIDs 
	$ REVISION_NUMBER = 10.5
	- LTK-8492: Added float support to DCI Enum VBA macro code.
	$ REVISION_NUMBER = 10.4
	- LTK-8115: Fix issue with enumeration display in the DCI workbook.
	$ REVISION_NUMBER = 10.3
	- LTK-7330: Implement the code optimization of REST_Builder VBA Macro for shorter execution time
	$ REVISION_NUMBER = 10.2
	- LTK-8076: Updated IOT Groups sheet and VBA code to support "publish-all-on-connect" for each cadence group
	$ REVISION_NUMBER = 10.1
	- LTK-8089: PTP timestamp of system available on other interface for automation
    $ REVISION_NUMBER = 10.0
	- LTK-8118: Updated DCI workbook and VBA code for "StoreMe on change" IOT feature    
$ COMPATIBILITY_NUMBER = 9
	$ REVISION_NUMBER = 9.0
	- LTK-7990: DCI workbook improvements related to NVM address(auto generation of address ) and modbus sheet
$ COMPATIBILITY_NUMBER = 8
	$ REVISION_NUMBER = 8.4
	- LTK-7979: Updated macro code to support both dynamic and meta param data by re-constructing the dciMeta.js file
	$ REVISION_NUMBER = 8.3
	- LTK-8060: Updated the macro to include sub tab strings in the language json files.
	$ REVISION_NUMBER = 8.2
	- LTK-7838: Optimize meta data generated by UI macro
	$ REVISION_NUMBER = 8.1
	- LTK-7915: Updated Update_DCI_List macro code to capture Bitfield descriptions correctly, updated DCI list in Lang Translation sheet.
	$ REVISION_NUMBER = 8.0
	- LTK-7823: DCI Workbook and Macro updates for supporting Horizontal Tabs
$ COMPATIBILITY_NUMBER = 7
	$ REVISION_NUMBER = 7.0
	- LTK-7635: Update Seed UI 2.0 macro code to fetch meta data and generate JavaScript file
$ COMPATIBILITY_NUMBER = 6
	$ REVISION_NUMBER = 6.1
	- LTK-7567: Fixed the bug and revised json file generation logic to handle space in a folder path
	$ REVISION_NUMBER = 6.0
	- LTK-6671: Added DCI sheet for IEC61850 GOOSE configuration parameters.
$ COMPATIBILITY_NUMBER = 5
    $ REVISION_NUMBER = 5.14
	- LTK-6854: Added "Verify DCI list" button in Lang Translation sheet. Updated the macro code to verify the short description, long description and units present in the sheet.
	$ REVISION_NUMBER = 5.13
	- LTK-7273: Renamed "DCI Lang Translation" sheet to "Lang Translation" and added Non-DCI strings section in it. Updated macro code to handle non-DCI strings, non-DCI and DCI strings rows are movable now.
	$ REVISION_NUMBER = 5.12
	- LTK-7339: Modified IOT_builder.bas to accept file name from IOT sheet rather than using hardcoded "IOT_DCI_Data.h"
	$ REVISION_NUMBER = 5.11
	- LTK-7175: Added validation to check if total length defined in DCI sheet is sufficient to hold default value.
	$ REVISION_NUMBER = 5.10
	- LTK-7004: Removal of CCM usage DCID. Added a dcid which keeps the counter of the device being alive after the last reboot.  
	$ REVISION_NUMBER = 5.9
	- LTK-6814: Added a path in the Seed UI 2.0 sheet and generated the dateTimePicker translation file via excel macro. 
	$ REVISION_NUMBER = 5.8
	- LTK-6781: Added new properties in the BLE sheet and the generated files for BLE. 
	$ REVISION_NUMBER = 5.7
	- LTK-6520: Added support for compression of language files in gzip format.
	$ REVISION_NUMBER = 5.6
	- LTK-6582: Added warning messages to handle run-time exceptions generated while creating language files, if language data is missing in "Seed UI 2.0" sheet or "WebUI_2.0\locales" folder
	$ REVISION_NUMBER = 5.5
	- LTK-6676: DCID which does not have default value has its static memory allocation corrected by modifying VBA code
	$ REVISION_NUMBER = 5.4
    - LTK-6470: Used uint32 type DCIDs to store version information, also removed all version number ASCII DCIDs to optimize DCI Ram usage. it will be converted from int to ASCII at runtime wherever needed
	$ REVISION_NUMBER = 5.3
	- LTK-6455: Seperate out common language header fields, add 10 blank columns before language columns in SEED UI 2.0 sheet and modify the VBA code accordingly
	- LTK-6468: Revised VBA code to create json file with UTF-8 encoding format
	- LTK-6332: Revised VBA code to enable/disable inclusion of languages, display warning if any of the header field missing for enabled language
	- LTK-6293: Revised VBA code to handle UTF-8 encoded characters in common.json file, Enabled Microsoft ActiveX Data Objects 6.1 library (ADO) from Tools->references
	$ REVISION_NUMBER = 5.2
	- LTK-6109: Update the DCI lang Translation sheet to add language header information
	- LTK-6116: Modify SEED UI 2.0 sheet to have a button to create language INI and JSON file
	- LTK-5709: To update SEED UI 2.0 sheet to add translation language column for French,German and Chinese
	$ REVISION_NUMBER = 5.1
	- LTK-5228: Removed ENABLE_SNTP_SERVICE_AUTOMATIC Enum for DCID "DCI_SNTP_SERVICE_ENABLE"
	$ REVISION_NUMBER = 5.0
	- LTK-5707: Add a "Language Translation" Worksheet in DCI workbook (  Multi Language Support )
$ COMPATIBILITY_NUMBER = 4
	$ REVISION_NUMBER = 4.2
	- LTK-5702: Add Language preference setting DCI
	$ REVISION_NUMBER = 4.1
	- LTK-5392: Optimizing the DCI DATA Block by removing the double ram pointer
	- LTK-5441: Modifying the DCI Data block structure to make use of array index for range and callback block
	- LTK-5393: Introducing a default pointer which will point to default value in memory, a new member introduced in DCI_DATA_BLOCK_TD structure
	- LTK-5490: Creating the Default, Range and Enum blocks with descending order of datatypes
	- LTK-5556: Creating RAM based values on the descending order of datatypes and re arranging DCI_DATA_BLOCK structure
	- LTK-5679: Create enum list with descending datatype value
	$ REVISION_NUMBER = 4.0
	- LTK-5332: Revised VBA code to handle errors in IOT groups sheet.
	- LTK-5253: Revised VBA code to export IOT grouping with cadence. Revised DCI workbook to add IOT Groups sheet and updated IOT sheet.
$ COMPATIBILITY_NUMBER = 3
	$ REVISION_NUMBER = 3.1
	- LTK-4871: Write VBA code for generating DCI_data.cpp and .h files for J1939
$ COMPATIBILITY_NUMBER = 2
	$ REVISION_NUMBER = 2.2
	- LTK-4453: Convert ANSI to UTF-8 encoding for dci.js
	$ REVISION_NUMBER = 2.1
	- LTK_4417: Optimize memory usage by handling REST parameters (names and descriptions) and making it optional for adopters to use
	$ REVISION_NUMBER = 2.0
	- LTK_3561: Optimize user management design to combine last login date date and last login time and password set date to single 32 bit epoch time.
	- LTK_3562: Optimize UI FW upgrade design to combine FW upgrade date and time DCI to single 32 bit epoch time.
	- LTK_4243: Declare statement added with PtrSafe keyword for VBA7 in 64bit version office 
$ COMPATIBILITY_NUMBER = 1
	$ REVISION_NUMBER = 1.4
	- LTK-3610: Added range checking for bitfields
	$ REVISION_NUMBER = 1.3
	- LTK-3535: Add Bitfield details to dci.js using macro.
	$ REVISION_NUMBER = 1.2
	- LTK 3537: Seed UI DCI generation is linked to "DCI Descriptors" sheet FORMAT column instead of "Rest sheet" FORMAT column.
	$ REVISION_NUMBER = 1.1
	- LTK_2213: Lower case vowels a,e,i are removed from the ENCODE_STRING to avoid formation of Java-Script keywords.
	- LTK_3315: REQUEST_LEVEL and WAIVER formats were handled separately while creating format data structures but now we made generic algorithm for all type of formats for creating format data structures and removed REQUEST_LEVEL.
	$ REVISION_NUMBER = 1.0
	- LTK_1302: Added feature to check for compatibility between RTK DCI DB Creator workbook and DCI DB Creator Macros workbook.
	- Created the ReadME file for RTK DCI DB Creator workbook and DCI DB Creator Macros workbook.