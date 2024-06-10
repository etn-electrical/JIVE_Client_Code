# IoT in PXGreen  - Documentation {#mainpage}
<!-- This markdown file is the "main page" of the Doxygen output -->

# 1. Table Of Contents
[TOC]



# 2. Overview

## Tasks to manage Internet-of-Things activity (IoT)

When IoT is enabled in the PXGreen code, if it is configured with a valid Azure IoT Hub Connection String, will initiate the
connection to that IoT Hub via an MQTT WebSocket connection.
Once the connection is made, the Agent will publish messages to the Azure cloud,
where they will be received by PX White services; these are Device-to-Cloud (D2C) messages.
PX White can also send messages from the cloud to the device (C2D), and these will be received by the IoT code and responded to.

The IoT code will also try to determine the cause of any
failure to connect initially or of any disconnect experienced while connected,
and set the reason in a DCI variable for feedback to the user.

## Context views for the IoT code

### Overall context view of IoT Code in PXGreen

This shows IoT code in the context of the overall PXGreen code system.

@startuml "Overall Context View of IoT Code in PXGreen"

package "Product Code" as Prod {
    node "Babelfish" {
        [IoT Code]
    }
}
Prod - [DHCP Server]
Prod - [SNTP Servers]
Prod - [Firewall]
Prod ... [Proxy Server]
[Proxy Server] ... [Firewall]
cloud Internet
[Firewall] - Internet
cloud "Azure IoT Hub" as Azure {
    [PXWhite services]
}
Internet - Azure
@enduml

### Internal Context Diagram for the IoT code

 This shows the relative relationships of the components in Babelfish for the 
 IoT code.

@startuml "Internal Context View of IoT Code in PXGreen"

package "Product Code" {
  node "Babelfish" {
    [IoT Classes] as IOT
    IOT - [Eaton IoT SDK]
    [Eaton IoT SDK] - [Azure Device SDK]
    node "LTK Adapters" as Adapt
    IOT - Adapt
    Adapt - [LWIP Socket]
    Adapt - [FreeRTOS Tasks]
    Adapt - [FreeRTOS Timers]
    [Azure Device SDK] ... Adapt
    IOT - [FreeRTOS Tasks]
    IOT - [FreeRTOS Timers]
    IOT - [Matrix SSL Client]
    IOT - [Change Tracker+]
    IOT - [Debug Manager]
    [LWIP Socket] - () "MQTT Websocket"
  }

  [DCI_Data.cpp] - [Prod_Spec_IOT.cpp]
  [Prod_Spec_IOT.cpp] - IOT
}

@enduml

### Data Flows for the IoT code

 This illustrates the important data interfaces and flows for the IoT code.
 
@startuml "Data Flows for the IoT Code in PXGreen"
node "IoT Code" as IOT
cloud "Azure IOT" as Azure
folder "IoT DCI Configuration" as DCI_CFG
folder "DCI Parameters for IoT" as DCI_PARAM
[UI] <--> DCI_CFG
[REST] <--> DCI_CFG
DCI_CFG --> IOT : Read
DCI_CFG <-- IOT : Subscribe
DCI_CFG --> IOT : Publish Changes
[Device Data] <--> DCI_PARAM
DCI_PARAM --> IOT : Read
DCI_PARAM <-- IOT : Subscribe
DCI_PARAM --> IOT : Publish Changes
IOT <--> Azure : TLS\n Handshake
IOT --> Azure : Connect
IOT --> Azure : Publish Device\n Tree message
IOT --> Azure : Publish\n Realtimes\n message
IOT --> Azure : Publish Trends\n message
IOT <-- Azure : Message\n confirmations
IOT <-- Azure : C2D Device\n Commands
IOT --> Azure : Device\n Command\n Responses
IOT <-> Azure : Firmware Update\n Messages

@enduml

[to top](#)
***

# 3. Features {#mainpage_features}

* [Description of Publishing, Cadence, and Message Types](iot_publishing_cadence.md)
* [Firmware Update Service (FUS) via IoT](iot_firmware_update.md)
* [Security Considerations](iot_security.md)


[to top](#)
***

# 4. Code Modules {#mainpage_modules}

 The modules of the IoT code are listed here; each module reference may be found in headers for the corresponding code.
<!-- The lines below are for markdown files that describe their code. The markdown groups begin with "md_" -->

 ## Configuration

* [How-to and explanation of IoT configuration](iot_config.md)
* [IoT Device Profiles](iot_device_profiles.md)


## Startup

* [IoT Code startup details](iot_startup.md)

## Operation

* [Tasks and Timers design](iot_tasks_timers.md)
<!-- * [Device and Channel Data](iot_devices_channels) -->
* [Data Publishing and Group Cadence](iot_publishing_cadence.md)
* [Cloud-to-Device (C2D) Commands](iot_commands.md)


## The Eaton IoT Device SDK

* [Eaton IoT Device SDK](https://bitbucket-prod.tcc.etn.com/projects/IOT/repos/platform_device_sdk_v2/browse/docs/iot_device_sdk.md)


## Library interfaces

* [Matrix SSL Client for IoT](https://bitbucket-prod.tcc.etn.com/projects/LTK/repos/babelfish/browse/Code/ThirdParty/Azure_IOT/Azure_ltk_adapters/explainer_matrixssl_integration.md)
<!-- * [Handlers for DCI data and callbacks](dci_handler) -->

## Building and Debug Logging

* [How to enable IoT and incorporate into product code](iot_enable.md)
<!-- * [Debug Logging](iot_logging) -->


[to top](#)
***

# 5. How to's {#mainpage_howtos}
  * [How to - Debugging](https://confluence-prod.tcc.etn.com/display/LTK/PX+Green+Diagnostics+and+Debugging+Features)
  * [How to Send C2D Commands](how_to_send_c2d_commands.md)
  * [How to - Firmware Update](how_to_do_firmware_update.md)
  * [How to push Blobs to Azure](how_to_push_blobs_to_azure.md)


[to top](#)
***

# 6. Tutorials 

<!-- * [Tutorial to develop ...] (@ref tutorial_TBD) -->


[to top](#)
***

# 7. More Information 

* [Babelfish Source code in Bitbucket](https://bitbucket-prod.tcc.etn.com/projects/LTK/repos/babelfish/browse)
* [LTK_Sample_App (example product) Source code in Bitbucket](https://bitbucket-prod.tcc.etn.com/projects/LTK/repos/ltk_sample_app/browse)
* [PXGreen development home](https://confluence-prod.tcc.etn.com/display/LTK/PX+Green+Development+Home)
* [The Green Pages](https://confluence-prod.tcc.etn.com/display/PXG/Power+Xpert+Green)
* [PX Embedded](https://confluence-prod.tcc.etn.com/display/PXEMBED/Embedded+Software+Center+of+Excellence+Home)
* [PX White Pages](https://confluence-prod.tcc.etn.com/display/UDIP)


[to top](#)
***

# 8. Version
Revision A
***
&copy; Eaton Corporation 2020+

# 9. About
<!-- text here is added by Bamboo script during generation -->
