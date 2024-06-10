# PX Green IoT Startup and Connection to Azure

## IoT startup details

The PX Green IOT device goes through a number of steps to determine its configuration and arrange itself before it enters Cadence_Check_Timer_Task().These steps are:

* Initialization of IOT on product side, creation of various tasks and timers
* Register various callback functions with Azure
* TLS initialization

All the above steps are described in detail in [IOT initialization of tasks](iot_tasks_timers.md), [Securing the connection](iot_security.md).

Once IOT is initialized and connected to Azure IOT hub, the device keeps running IOT_Task() and its worker function, messageThread() to do any sending receiving or housekeeping

[Module for Startup Code](@ref iot_init_tasks)

## Process for Connection to the Azure IoT Hub cloud {md_azure_connect}

The process in the PX Green IOT device for connecting to Azure is as follows:

* Fill out the iot_connection_options structure:
  * Choice of protocol (we currently only use MQTT Websockets for PX Green)
  * Copy of the Publisher Device's UUID, the Connection String, and the Proxy Settings, if enabled.

* Call the IoT SDK's iot_open()
  * This calls the low-level Azure connection functions and tries to establish the connection.
  * If this attempt fails immediately (which is not common), it returns a failure code.
  * But if the attempt appears to succeed, that is not proof that the connection has really been established; that won't be known until the first message is acknowledged by Azure.
* Assuming success,
  * Register the callbacks for Connection Status and Cloud-to-Device (C2D) Device Commands
* Send the first messages:
  * DeviceTree
  * All Realtimes and Trends messages if configured to do so on product side
