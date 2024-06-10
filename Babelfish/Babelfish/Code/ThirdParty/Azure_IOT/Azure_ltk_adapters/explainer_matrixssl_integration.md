# Integration of Matrix SSL into the Azure SDK stack

The Matrix SSL interface is used to provide the encrypting and decrypting
functions for the connection to Azure. It was selected for its minimal footprint 
and simple API interface.

Importantly, the Matrix SSL code does not provide the socket-level interface or the 
blocking code that waits for and receives input; all of its calls are
synchronous and non-blocking.

## Positioning

The Azure stack uses the Matrix SSL layer (found here in tlsio_matrixssl.cpp)
as one layer in the series of layers for its IO.
This call stack for socketio_open() illustrates the layers in action;
this is shown for the case which uses an HTTP Proxy layer:

```
    socketio_open
	xio_open
	http_proxy_io_open
    xio_open
	tlsio_matrixssl_open()
	xio_open
	uws_client_open_async
	wsio_open
	xio_open
	mqtt_client_connect
	SendMqttConnectMsg
	InitializeConnection
	IoTHubTransport_MQTT_Common_DoWork
	IoTHubClientCore_LL_DoWork
	initializeConnection
	iot_open()
	IOT_Net::IOT_Net
	PROD_SPEC_IOT_Init()
	Eth_if::Ethernet_Start_Thread()
	prvTaskExitError()
```

Notice that there is an xio layer between each of the working layers;
the Azure documents describe xio as "a generic C-language bits-in-bits-out interface".

As seen here, the websocket layers are the "upper layer" that calls into
the tlsio layer (which implements the interface to Matrix SSL). 
In turn, the next lower layer here is the HTTP Proxy layer, which has a final lower layer
which is the Socket IO layer (socketio). The Azure code provides the information
about the HTTP Proxy layer for us when calling tlsio_create().

In the case where the Azure connection does not use a proxy, the tlsio layer uses the socketio
layer as its next lower (and final) layer.
Oddly, and in contrast to the proxy case, the Azure code does not provide
the socketio information to us as the alternative to the proxy layer,
so our code in tlsio_create() must interject the interface down to the socketio layer.

In both cases, this tlsio layer handles outgoing data on the send() function, encrypting it first via the MatrixSSL API interface before
sending it down to the next lower layer.
This tlsio layer handles incoming data received by the lower layers,
decrypting it before sending it up to the next upper layer.


## Relationship to LTK MatrixSSL code

We are specifically trying not to use the matrixssl_ltk functions, such as SSL_connect().
While these functions are normally convenient, they assume that the SSL/TLS connection owns the socket 
(ie, they use functions that begin with "mtxssl_if_", such as mtxssl_if_SSL_CTX_new).
This is not the case for the Azure stack, particularly for the proxied connection, where the Azure code 
wants to call up and down through its
layers of XIO, and the socket is owned by the final socketio layer.

We do depend on the SSL_Context functions to initialize the library (Ssl_Context::Library_Init()) and provide the keys and Azure root certificate to MatrixSSL. 
Then we can use Ssl_Context::Get_Context() to get
the opaque Matrix SSL context pointer for our use.
We also assume that we can access and use its ->keys structure (so it is not totally opaque to us).


## For more information

The most helpful resources for explaining the tlsio layer are found in 
the respective repositories for Azure and Matrix SSL:

* The Azure porting guide (at devdoc/porting_guide.md)
* The Matrix SSL API guide (at doc/MatrixSSL_API.pdf)

