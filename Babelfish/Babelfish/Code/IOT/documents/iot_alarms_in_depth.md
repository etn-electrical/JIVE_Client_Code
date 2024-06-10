# Alarms over IOT in-depth explanation

## Startup and initialization

1. The Alarms over IOT code begins with PROD_SPEC_ALARM_Init(), which is called as part of the PROD_SPEC_Target_Main_Init() function.

2. The function PROD_SPEC_ALARM_Init() instantiates IOT_Alarms::IOT_Alarms()

3. IOT_Alarms::IOT_Alarms() initializes alarm publishing head index, cadence owners and alarm interface handle for IOT

4. The function PROD_SPEC_IOT_Init() instantiates IOT_Net::IOT_Net() where it calls functions to initialize last published head index and initialize alarm publishing cadence as per configured severity level

## Tasks and timers design

The function IOT_Alarms::Init_Cadence() instantiates IOT_Alarms_Cadence::IOT_Alarms_Cadence() class where BF_Lib::Timers are created for each severity level. The time base for these timers are set by cadence rate corresponding to each severity level. It also registers callback function Reinitialize_Cadence_Rate_Static() that provides a functionality to set alarm publishing cadence rate dynamically(over REST interface)

## Modules or interfaces

Alarm generation and management is handled by Alarm_Manager class located at "\ltk_sample_app\RTK_Example\Babelfish\Code\MiscCode\Alarms\Alarm_Manager.cpp"

Alarm manager provides all the necessary functions that are required to handle alarms while progressing through its life cycle. IOT modules access these functions through Alarm interface

## Alarm operations and control

### Swagger APIs

[Mapping of swagger APIs with PXGreen alarm APIs](https://confluence-prod.tcc.etn.com/display/LTK/Mapping+of+swagger+APIs+with+PXGreen+alarm+APIs)

[Swagger APIs for alarms](https://confluence-prod.tcc.etn.com/display/LTK/Alarms+over+IOT#AlarmsoverIOT-SwaggerAPIsforalarms)

### Alarm control functions in code

#### Device-to-cloud alarm messages publishing

IOT_Net::Timer_Task() handles the publishing of generated alarms when their cadence interval is reached. The flow of alarm generation to alarm publishing is as follows:

1. As soon as alarms are generated, Alarm manager invokes New_Alarm_Params_Cb() callback function that logs the generated alarm in the memory(Flash memory by default) using logging module. At the same, it also saves the severity level of that alarm using function IOT_Alarms_Cadence::Mark_Severity_Level() which will be helpful later to identify the cadence interval during publishing

2. We have Cadence_Timer_Task() per severity level or per alarm publishing cadence rate. These timers execute periodically as per their configured cadence rate. Once the cadence rate of the generated alarm is reached, it will set m_alarms_ready flag which indicates that alarm update is ready to be published

3. As IOT_Net::Timer_Task() is invoked periodically every 100ms (as defined by IOT_MESSAGE_SLEEP_MS in IoT_Config.h ), it checks m_alarms_ready flag and published the alarm to IOT hub by calling function Publish_Alarms_Message()

Note that each alarm message has size approximately 400 bytes. For STM32F427, only one alarm update can be published at a time as the default message buffer size is 512 bytes(IOT_MAX_OUTBOUND_BUFFER_SIZE is defined as 512). For STM32F767, 10 alarm updates can be published at a time as the default message buffer size is 4096 bytes

#### Cloud-to-device alarm control commands

All C2D alarm commands are processed by IOT_Device_Command::Execute_C2D_Command() callback function. These commands are as follows:

* **Alarm acknowledgement**  
The function IOT_Alarms::Ack_Alarms() parses alarm acknowledge request received from cloud and fills in the appropriate response to be sent to cloud. It calls alarm acknowledgement function provided by alarm interface Alarm_Interface::Ack() which will then return success or error. The list of error codes returned on failure are as follows:

    1. ALARM_ALREADY_ACK - "Alarm already acknowledged". This error shows up when you try to acknowledge an already acknowledged alarm

    2. ALARM_ACK_LEVEL_MISMATCH - "Alarm ack level mismatched". This error shows up when the ack level provided in C2D command payload is different from the one configured

    3. ALARM_RULE_OUT_OF_RANGE - "Alarm rule out of range". This error shows up when the rule index(after extracting from alarm id) goes out of bounds

    4. ALARM_ID_NOT_FOUND - "Alarm rule id not found". This error shows up when you try to acknowledge an alarm which is not generated

* **Close alarms**  
The function IOT_Alarms::Close_Alarms() parses alarm close request received from cloud and fills in the appropriate response to be sent to cloud. It calls alarm close function provided by alarm interface Alarm_Interface::Close() which will then return success or error. The flag close_all which is sent as an argument to this function is set to true when all alarms are to be closed at once.
The list of error codes returned on failure are as follows:

    1. NO_ALARMS_TO_CLOSE - "No alarms to close". This error shows up when you try to close when there are no alarms generated. For e.g, when you try to close all alarms after clearing alarm history, the device will respond with this error.

    2. ALARM_RULE_DISABLE - "Alarm rule disabled". This error shows up when you try to close an alarm when alarms rules are configured as disabled. For e.g, currently the DCID DCI_ALARM_ENABLE is set to 0x000F which indicates that 4 alarm rules are enabled. If you set this DCI_ALARM_ENABLE to 0x0000 and then try to close all alarms then the device will respond with this error.

    3. ALARM_NOT_CLEARED- "Alarm latched and need to clear first". This is specific to latching type of alarms. This error shows up when you try to close an alarm(latching type) when its alarm condition is not yet cleared.

    4. ALARM_RULE_OUT_OF_RANGE - "Alarm rule out of range". This error shows up when the rule index(after extracting from alarm id) goes out of bounds

    5. ALARM_ID_NOT_FOUND - "Alarm rule id not found". This error shows up when you try to acknowledge an alarm which is not generated

* **Clear alarm history**  
The function IOT_Alarms::Clear_Alarm_History() parses clear alarm history request received from cloud and fills in the appropriate response to be sent to cloud. It calls a function provided by alarm interface Alarm_Interface::Log_Clear_All_Events() which will then return success or error.
The list of error codes returned on failure are as follows:

    1. CLEAR_INPROGRESS - "Clearing alarm history in progress". This error shows up logging module is busy

    2. CLEAR_ERROR - "Failed to clear alarm history". This error shows up when logging module fails to clear memory due to some unknown error

## Alarm handling during network disconnect and device reset scenarios

A temporary disconnect (eg, SAS Token refresh or longer network interruption) does not result in the loss of any published updates in PX White. Suppose that device has temporarily disconnected and 2 alarms are generated at the same time. Once the device is back online, it will immediately(assuming that their cadence rate had been reached already) publish those 2 alarm updates to cloud.

IOT module maintains last published index in order to handle this scenario. The DCID DCI_IOT_ALARMS_LAST_PUB_INDEX which handles last published index is also non-volatile(present in NVM sheet) so that device reboots do not cause loss of any published updates.
