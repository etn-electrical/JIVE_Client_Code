# Eaton IoT Device SDK

## Overview

The Eaton IoT SDK is an API-based layer between your product code and the Azure "C" Device SDK. The Eaton IoT SDK has the following features:

### Manages the Connection to Azure {#md_connection_status_apis}

* Your product code provides a configuration for the connection, and then uses the SDK to initiate the connection to an Azure IoT Hub. 
* When Azure signals a change in the connection status (either good or bad), the IoT SDK will invoke a callback that your code provided so that your code can take the appropriate next action.
* The IoT SDK maintains an internal connection structure, and provides your code with a "handle" into the connection so your code can send messages.

### Provides the data structures compatible with PXWhite

* The PXWhite services are a layer on top of the Azure cloud services that provide Eaton-specific data handling; think of Azure as the network provider and PXWhite as the database manager for the data sent.
* Azure is relatively agnostic about the content of the messages, but the IoT SDK structures the message content in the forms agreed to with PXWhite.
* The IoT SDK defines the structure for these Device-to-Cloud (D2C) messages:
  * DeviceTree -  message consisting of static information about each configured device, starting with the main or "Publisher" device.
  * Realtimes - message containing the latest value and timestamp for one or more channels. PXWhite only holds the latest Realtimes values (ie, no history).
  * Trends - message containing any combination of Last, Minimum, Average, or Maximum values for each channel in the message, with timestamp. This data is archived by PXWhite. Normally Trend messages are sent on a regular trend interval.
  * D2C Commands - play a supporting role in firmware update from the cloud
* The IoT SDK defines the structure for Cloud-to-Device (C2D) messages:
  * The Device Command - provides a command with key-value parameters
    * Some PXWhite-defined commands exist, such as Get and SetChannelValues
    * Same message structure can be used for product-defined messages
    * The product code can build a response to return to PXWhite
  * The Device Lifecyle Messages (DLM), used to do firmware update from the cloud

### Provides the services for message sending {#md_message_handling_apis}

* Provides the function to send either
  * A data message (DeviceTree, Realtimes, Trends)
  * A D2C Command
* Provides internal serializing of the data for the message
  * Serializes the data to a JSON string
  * Any binary arrays are encoded as Base64 ASCII strings
* Provides a callback to signal when Azure confirms the message
* Starts a "messageThread" that runs periodically to dispatch any queued messages and lets Azure code "DoWork"

### Provides callbacks to the product code for cloud-driven events {#md_iot_sdk_callbacks}

* The IoT SDK provides a method to register and unregister each supported callback
* The callbacks are:
  * C2DDeviceCommandCallback for processing Cloud-To-Device device commands
  * TimerCallback invoked periodically by the messageThread, if your product code needs to be woken up regularly
  * ConnectionStatusCallback which is invoked when the status of the connection to IoT Hub changes.
  * MessageDoneCallback which is invoked when a published Message (D2C) is confirmed by Azure (Success), or if it Fails.

### Provides Debug logging {#md_iot_sdk_logging}

* Provides a means for the product to substitute its own debug logger for
the default logger in the IoT SDK.
* The default logging macros are defined in the product interface header  (xxx_device_platform.h):
  * Log_Error()
  * Log_Info()
  * Log_Debug()
* Lets the product set the maximum verbosity level of debug output.


## API Modules

* [Connection and Status APIs](@ref connection_status_apis)
* [Message handling APIs](@ref message_handling_apis)
* [Callbacks from the Eaton IoT SDK](@ref iot_sdk_callbacks)


## Structure of the SDK

The following directory tree shows the structure and purposes for the IoT Device SDK:

```
   ├───docs    - Documentation about the SDK
   ├───include - Headers for the SDK source code
   ├───make    - Build instructions for the SDK as a library under linux
   └───source  - The "C" source code files for the SDK
```

### Source code files

The source folder contains the following source files:

	1. iot_device.c
		- iot_open function –  provided with ConnectionOptions, creates a messageThread to operate IOT things.
		- iot_close function – closing down the existing connection and delete the Thread.
		- iot_send function – Create message for the data to be pushed to the cloud.
		- Callbacks registration for timer, c2dmessage, commands.
		- List functions to create, delete lists, add and remove items from the list.

	2. iot_device_common.c
		- Connection related functions – create connection, get connecttion details, delete connection, validate the connection options, initialize the connection.

	3. iot_serialization.c
		- Serializing the Json data to be sent to the cloud from the device.
		- Deserializing the Json format data coming from the cloud.

	4. iot_json_parser.c (JSMN - https://github.com/zserge/jsmn)
		- Contains functions for parsing operations on json string.

	5. iot_encoding.c
		- Contains functions to encode/decode Base64
		- For safely communicating binary data
	
	6. azure_certs.c (file by Microsoft)
		- Contains the "Baltimore" Certificate Authority file
	
### Header files

The include folder contains the following header files:

	1. iot_device.h
		- Contains constants, enums and structures for IT commands and requests.
		- Contains callback declarations and function declarations.
		
	2. iot_json_parser.h (JSMN - https://github.com/zserge/jsmn)
		- json parser header file
		
	3. pxred_device_platform.h
		- The #defines to enable logging, enable protocols, for platform target PXRed.
		
	4. sample_iot_device_platform.h
		- Example #defines to enable logging, enable protocols, and platform target (Px-Red or Px-Green).
		
		

