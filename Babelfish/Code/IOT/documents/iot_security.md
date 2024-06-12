# IoT Security Considerations

## Keys and Certificates
For the TLS client session used in the connection to the Azure IoT Hub (server), the PXGreen devices
use the "Baltimore" public certificate that Azure provides to authenticate that your
device has in fact reached a valid Azure site. 
This certificate is located in the azure_certs.c file in the Eaton IoT SDK.

After the handshake, the Azure server and the device TLS client code negotiate a private session key
to use for the TLS encryption of all subsequent messages.


## Configuration Security

The main IoT configuration security concern is for the content of the Connection String used to authenticate this device's connection to the Azure IoT Hub. 
If a malicious party were to copy this string, they could spoof the genuine device
by connecting to Azure and publishing their own data and intercepting any C2D commands
intended for the genuine device.

There are 3 parts to the connection string: the URL of the IoT Hub that this device should connect to, the UUID of this device, and the Shared Access Token (SAS Token) that authenticates this device to Azure. 
The first two parts are not secrets, but the last part is.

Consequently, the contents of the Connection String in the DCI should either be readable only by a highly privileged user, or not readable at all (ie, write-only contents).

The other part of the IoT configuration is the optional proxy username and password, if they are needed. 
(They are not needed in the Eaton proxy case, which may be the common case.)
Access to the proxy password, in particular, must be protected like the Connection String.


## Connection Security

### Connection Authentication
PXGreen devices use the Shared Access Token (SAS Token) of the Connection String to authenticate themselves to the Azure IoT Hub when connecting.
Each device has a unique SAS token assigned to it when you register it with PXWhite;
you provide each device's UUID as input to register the device, and this UUID is also
contained in the Connection String that PXWhite gives you.

The SAS Token is used to authenticate your device to Azure when you first connect; 
it has an expiration time that is set by the IOT_SAS_TOKEN_REFRESH_SECS
definition in the iot_define_config.xcl file.
When the SAS Token expires, the IoT connection will be closed and then immediately re-opened.

### Network-level Security
The PXGreen devices all use the MQTT WebSocket transport to connect to Azure.
The WebSocket connection, which is outgoing, connects to port 443 at the Azure IoT Hub end,
and uses TLS encryption.

This is implemented in the PXGreen devices using Matrix SSL for its TLS services, including the initial TLS handshake and ECC-based encryption of all messages once the connection to Azure has been established. 
The PXGreen devices use LWIP services for the socket-level connection.

## Cloud Security

### Azure sites
The Connection String used to connect to Azure has been registered in a particular "site"
in the Azure IoT Hub framework. For customers, this is normally a "production" site that is owned and maintained
by the product group that provides this device (the "publisher" device).

For development or test, a different site is likely to be used, one for which 
engineers have been granted the access to the data and granted the privileges to 
send C2D commands to the device.

### Data access
There is not much to say here from the device-side considerations; the access that operators
have to data in Azure is managed by PX White in combination with Azure user management. 
See the [PXWhite documentation](http://wordpress-prod.tcc.etn.com/wordpress/pxwhite/) for details.
