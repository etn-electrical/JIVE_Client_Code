# Azure IoT hub commands

IoT Hub allows the bi-directional communication with the devices. Core properties of IoT Hub messaging functionality are the reliability and durability of messages. IoT Hub implements at least once delivery guarantees for both device-to-cloud and cloud-to-device messaging.

## Device-to-cloud (D2C) messages

These are the messages that are initiated by the device. There are two types:

* Published data messages. 
  * For example, device tree, realtime and trends data
* D2C Commands. These are commands that are initiated by the Device side. 
  * There are not many of these, but examples of standard D2C commands, for FUS, are TransferImages and DeviceCommandStatus updates.

## Cloud-to-device (C2D) commands

These are the messages that are initiated by PX White from the Azure IoT hub cloud. Currently, these are all Azure Direct Method, PX White-based Device Commands. 

On receiving a C2D command, the Eaton IoT SDK calls C2D_Device_Command_Callback(), which is registered with the SDK
to receive all of these types of commands. This callback calls IOT_Device_Command::Execute_C2D_Command(), which uses the
method name of the command to dispatch it to a handler. If it is not a standard C2D command, the IOT_Fus::Command_Handler() is
called to see if it wants to handle the command; if it does not handle it, and if the product has registered a callback as
m_c2d_product_command_callback, then the product is given its chance to handle the command.

For all C2D command instances, a response is returned to PX White, with an HTTP status and message indicating success or failure,
and optionally some parameters may be part of the response.

The list of standard Cloud-to-device(C2D) commands is as follows:

* GetChannelValues  
This command is used to read values of single/multiple channels; it is handled by IOT_Device_Command::Get_Params().

* SetChannelValues  
This command is used to write values to single or multiple channels; it is handled by IOT_Device_Command::Set_Params().

Following are Cloud-to-device(C2D) commands that might be implemented in the future:

* Reboot
* Update configuration
* Upload configuration

See [Firmware Update Service (FUS) via IoT](iot_firmware_update.md) for descriptions of the D2C Device Commands for FUS.


## Memory management for C2D Device Commands

It is worth noting that storage for the copy of the incoming command is malloc'd rather than using the fixed IOT_MESSAGE iotMessage[] array.
One reason for this is to not limit the size of incoming commands, or to overburden the iotMessage[] array with maximal allocation based
on the occasional large C2D commands. The FUS commands are currently the largest commands normally received, when transferring image data.


## Babelfish Modules

[IoT Cloud-to-Device Commands](@ref iot_c2d_commands)
[Firmware Update over IoT](@ref fus_over_iot)

## How to use swagger to send Cloud-to-Device (C2D) commands

See [How to Send C2D Commands](how_to_send_c2d_commands.md)


