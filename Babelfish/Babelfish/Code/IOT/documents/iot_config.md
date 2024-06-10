# How-to and explanation of IoT configuration

## How to configure IoT

* Follow these [Initial steps](iot_enable.md) to configure IoT.
* In order to create and register your own device to IoT hub, follow the steps mentioned in [How to configure IoT from swagger](https://confluence-prod.tcc.etn.com/display/LTK/How+to+configure+IoT+from+Swagger)

## Explanation of each IoT configuration parameter

### 1. Key configuration parameters

> These parameters are not enabled by default in DCI Descriptors sheet in RTK DCI DB Creator workbook. You need to explicitly enable them to get IoT working.

1. **Network settings**  
IOT device will need an internet connectivity to get connected to IoT hub which is in public network. Easiest method could be to keep device on DHCP mode and get the internet connectivity. The DCID DCI_IP_ADDRESS_ALLOCATION_METHOD should be set to 1 in order to enable DHCP. Alternatively, this can also be achieved using static IP provided by your IT team. Please make sure that the network/port provided can access public internet. Refer [IOT set up guide](https://confluence-prod.tcc.etn.com/display/LTK/PX+Green+IOT+Setup+Guide) for more details.

2. **IoT enable**  
In order to enable IoT functionality in code, you need to set the DCID DCI_IOT_CONNECT to 1.

3. **Device GUID**  
This is the unique ID for each device which is used while connecting to IoT hub. IoT hub allows only registered devices to get connected. You need to set DCID DCI_IOT_DEVICE_GUID to your registered device's GUID

4. **Device profile ID**  
This is a unique ID assigned to device's profile just like Innova profile. Device's profile contains information of device and tags used by different parameters of the device. You need to set DCID DCI_IOT_DEVICE_PROFILE to your device's profile ID.

5. **IoT hub server connection string**  
Each registered device has its own connection string. Connection string contains IoT hub host name, registered device's GUID and shared access key. It is mainly used by devices to send device to cloud messages and receive cloud to device messages. You need to set DCID DCI_IOT_CONN_STRING to your registered device's connection string.

### 2. Default configuration parameters

> These parameters are enabled by default in DCI Descriptors sheet in RTK DCI DB Creator workbook. They can be used as is.

1. **SNTP**  
While initiating a connection to IoT hub, your device's clock should always be in synchronization with the current time. Device uses SNTP server to fetch current time. If the device is not in sync with the current time then IoT will not get connected. The DCID DCI_SNTP_SERVICE_ENABLE will always be 1 indicating that SNTP is enabled

2. **IoT connection status**  
This is Read-only parameter which shows status as "true" when IoT is connected and "false" when IoT is not connected. The DCID DCI_IOT_STATUS is monitored for checking IoT connection status.

3. **Group 0 Cadence - Normal SeeMe**  
The DCID DCI_GROUP0_CADENCE set to 10000(by default) indicates that Group 0 real time data will be published after 10 seconds on changing the parameter value. Different cadence rate values supported by group 0 are configured using DCI enum

4. **Group 1 Cadence - 1 minute StoreMe**  
The DCID DCI_GROUP1_CADENCE set to 60000(by default) indicates that Group 1 trend data will be published after every 60 seconds, regardless of changes. Different cadence rate values supported by group 1 are configured using DCI enum

5. **Group 2 Cadence - Fast SeeMe**  
The DCID DCI_GROUP2_CADENCE set to 1000(by default) indicates that Group 2 real time data will be published after 1 second on changing the parameter value. Different cadence rate values supported by group 2 are configured using DCI enum

6. **Group 3 Cadence - StoreMe on change**  
The DCID DCI_GROUP3_CADENCE set to 1000(by default) indicates that Group 3 trend data will be published after 1 second on changing the parameter value. Different cadence rate values supported by group 3 are configured using DCI enum

7. **IOT Connection Status reason**  
When IoT fails to get connected then the failure reason is provided by DCID DCI_IOT_CONN_STAT_REASON. It can have one of the following reasons.

* IoT initial state (default)
* Cloud connected
* IoT disabled
* IoT clock not correctly set
* Reconnecting
* Failed to open connection
* Invalid connection string
* Invalid configuration
* SAS token expired
* Device disabled by user on IoT hub
* Retry expired
* No network
* Communication error
* Unknown error
* TLS initialization failure
* Ethernet/network interface link down
* Daily cloud message limit reached
* Too many lost messages
