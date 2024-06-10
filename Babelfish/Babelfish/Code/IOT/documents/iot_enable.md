# How to enable IoT and incorporate into product code

## Initial steps

>This link guides you through the initial steps to set up IoT:
<https://confluence-prod.tcc.etn.com/display/LTK/PX+Green+IOT+Setup+Guide>
>The file iot_define_config.xcl is located at <https://bitbucket-prod.tcc.etn.com/projects/LTK/repos/ltk_sample_app/browse/RTK_Example/Code/Config/iot_define_config.xcl>

## How to add a new IoT channel

>In order to add a new IoT channel, you need to do modifications in RTK DCI DB Creator workbook. Suppose we want to add a new IoT channel named "Power converter input voltage". Below are the steps that you need to follow:

1. Navigate to DCI descriptors sheet in RTK DCI DB Creator workbook. Insert a row just below the last IoT parameter(Above the DCI "Group 0 Cadence - Normal SeeMe"). Add a parameter "Power converter input voltage" and fill all the details like code define, data types, RO value, etc. Save and click on "Create DB" button. Make sure there are no errors. If you get any errors then navigate to the rightmost column which will guide you on errors.
2. Navigate to NVM address sheet. Insert a parameter name and its 2 byte checksum just above "NV_DATA_END_NVCTRL0" located at the end of the sheet. As you inserted 3 rows, adjust the values in Dec Add and Hex Add columns. Save and click on "Generate NVM" button. Make sure that you don't get any errors in the Missing definitions column. This allows you to save this new parameter in non-volatile memory.
[Note: You can skip this step if you do not want to retain the value of the new parameter on power loss]
3. Navigate to REST sheet. Insert a row just above "BEGIN_IGNORED_DATA" and add a parameter "Power converter input voltage" and fill its details like description, code define, param ID, r as 7 and w as 99. Save and click on "Create REST DB" button. This allows you to access this new parameter over REST interface
4. Navigate to IOT sheet in RTK DCI DB Creator workbook. Insert a row just above "BEGIN_IGNORED_DATA" and add a parameter "Power converter input voltage" and fill its details like description, code define, tag, group membership, short description override and long description override. Save and click on "Create IOT DB" button. Make sure that you don't get any errors. This allows you to access this new parameter over swagger UI(Azure IOT hub)

>>[Note: Never insert any data below "BEGIN_IGNORED_DATA" text as it will be ignored. The parameter which you add in IOT sheet should always be present in DCI descriptors sheet]

## How to delete an existing IoT channel

1. Navigate to DCI descriptors sheet in RTK DCI DB Creator workbook and delete the entire row containing parameter you want to delete. Save and click on "Create DB" button.
2. Navigate to NVM address sheet and delete 3 rows containing parameter and its checksum. Adjust the values in Dec Add and Hex Add columns. Save and click on "Generate NVM" button.
3. Navigate to REST sheet and delete the entire row containing parameter you want to delete. Save and click on "Create REST DB" button.
4. Navigate to IOT sheet and delete the entire row containing parameter you want to delete. Save and click on "Create IOT DB" button.

## How to modify cadence rate for an existing group

### Configuration using DCI sheet

1. Navigate to DCI descriptors sheet in RTK DCI DB Creator workbook and search for a cadence group DCID whose cadence rate you want to change.
2. Go to "Default(RO value)" column and enter the cadence rate of your choice in milliseconds as int32 value. For example, if you want to set the cadence rate as 100 seconds then you need to enter 100000 in "Default(RO value)" column for that group
3. Additionally, the set of enum values representing multiple cadence rates supported by that group can also be configured
4. Save and click on "Create DB" button.

### Configuration at run time

As these DCIs are configured with write access, any cadence rate defined in DCI enum can be configured dynamically using DCI patron interface. In the sample application these Cadence groups DCIs are available over REST interface. The cadence rate is applied immediately without requiring any reboot or IOT reconnection.

### Additional notes on cadence rate

1. Setting the cadence rate to -1 seconds stops the device from publishing the trends ( store me ) and realtimes data ( see me )
2. Setting the cadence rate to 0 seconds makes the device publish immediately within 100ms(configured by IOT_MESSAGE_SLEEP_MS) once the channel value is changed. This is applicable only to those groups that publish their data on change i.e group 0, 2 and 3
3. Group 1 i.e StoreMe on interval has minimum cadence rate of 10 seconds. Any value from 0 seconds to 9 seconds is not allowed. If Group 1 cadence rate is set to any value from 0 seconds to 9 seconds then an assert will be triggered
4. Although it is possible to publish at 100ms resolution(by the setting cadence rate as 0 seconds), the time stamp that we send is in seconds. Using time stamp in milliseconds creates additional dependencies. Currently, PX White implementation to support timestamp in milliseconds is not in place. Another dependency is hardware RTC with milliseconds resolution. For example, compact MCUs like ESP32 don't have hardware RTC or any other time source that provides timestamp in milliseconds

### Known issue

Suppose that the channel that belongs to group 3(StoreMe on change) is configured for immediate publishing. If the channel's value changes to value 4 and then changes to 5 after 100ms then both the messages with be published with same timestamp(as the timestamp is in seconds). Cloud will store the latest value 5. In this case, the value 4 will be lost

## Debug manager

>Debug manager is a mechanism that allows you to view debug messages on Terminal I/O in IAR. In order to enable debug manager for IoT, make sure that you have the following macros defined as DBG_ON. By default, they are defined as DBG_OFF

`Debug_Manager_Config.h`

``` h
#define BF_DBG_MSG_CRITICAL     DBG_ON
#define BF_DBG_MSG_INFO         DBG_ON
#define BF_IOT_DBG              DBG_ON
```
