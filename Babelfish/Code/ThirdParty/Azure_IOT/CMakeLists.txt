#Set source files 
set( srcs 	"Azure_repo/c_utility/src/azure_base64.c"
		    "Azure_repo/c_utility/src/azure_buffer.c"
			"Azure_repo/c_utility/src/azure_hmac.c"
			"Azure_repo/c_utility/src/azure_hmacsha256.c"
			"Azure_repo/c_utility/src/azure_sha1.c"
		    "Azure_repo/c_utility/src/azure_sha224.c"
		    "Azure_repo/c_utility/src/azure_sha384-512.c"
		    "Azure_repo/c_utility/src/connection_string_parser.c"
		    "Azure_repo/c_utility/src/constbuffer.c"
			"Azure_repo/c_utility/src/constmap.c"
		    "Azure_repo/c_utility/src/crt_abstractions.c"
		    "Azure_repo/c_utility/src/doublylinkedlist.c"
			"Azure_repo/c_utility/src/gb_rand.c"
		    "Azure_repo/c_utility/src/gb_stdio.c"
		    "Azure_repo/c_utility/src/gb_time.c"
			"Azure_repo/c_utility/src/http_proxy_io.c"
			"Azure_repo/c_utility/src/map.c"
			"Azure_repo/c_utility/src/optionhandler.c"
			"Azure_repo/c_utility/src/sastoken.c"
		    "Azure_repo/c_utility/src/singlylinkedlist.c"
		    "Azure_repo/c_utility/src/string_tokenizer.c"
		    "Azure_repo/c_utility/src/strings.c"
		    "Azure_repo/c_utility/src/urlencode.c"
		    "Azure_repo/c_utility/src/usha.c"
			"Azure_repo/c_utility/src/utf8_checker.c"
			"Azure_repo/c_utility/src/uws_client.c"
			"Azure_repo/c_utility/src/uws_frame_encoder.c"
		    "Azure_repo/c_utility/src/vector.c"
			"Azure_repo/c_utility/src/wsio.c"
			"Azure_repo/c_utility/src/xio.c"
			"Azure_repo/c_utility/src/consolelogger.c"
			"Azure_repo/c_utility/src/xlogging.c"
			
			"Azure_repo/iothub_client/src/iothub_client.c"
			"Azure_repo/iothub_client/src/iothub_client_authorization.c"
		    "Azure_repo/iothub_client/src/iothub_client_core_ll.c"
		    "Azure_repo/iothub_client/src/iothub_client_diagnostic.c"
		    "Azure_repo/iothub_client/src/iothub_client_ll.c"
			"Azure_repo/iothub_client/src/iothub_client_retry_control.c"
		    "Azure_repo/iothub_client/src/iothub_message.c"
			"Azure_repo/iothub_client/src/iothub_transport_ll_private.c"
		    "Azure_repo/iothub_client/src/iothubtransport.c"
			"Azure_repo/iothub_client/src/iothubtransport_mqtt_common.c"
			"Azure_repo/iothub_client/src/iothubtransportmqtt.c"
			"Azure_repo/iothub_client/src/iothubtransportmqtt_websockets.c"
		    "Azure_repo/iothub_client/src/version.c"
		    
		    "Azure_repo/umqtt/src/mqtt_client.c"
			"Azure_repo/umqtt/src/mqtt_codec.c"
			"Azure_repo/umqtt/src/mqtt_message.c"
			
			"../../IOT/Ltk_ConsoleLogger.cpp"

	)
   
#Set include directories
set (include_dirs 
				  "."
				  "../"
				  "../../Lib"
				  "../../BF"
				  "../../BF/IOT"
				  "../../BF/DCI"
				  "../../../../Code"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../IOT"
				  "../../IOT/platform_device_sdk_v2/include"
				  "../../DebugManager"
				  "../../uC/ESP32D0WD"
				  "../../OS_FreeRTOS"
				  "Azure_repo/c_utility/inc"
				  "Azure_repo/c_utility/pal/generic"
				  "Azure_repo/umock_c/inc"
				  "Azure_repo/iothub_client/inc"
				  "Azure_repo/umqtt/inc"
				  "Azure_ltk_adapters"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
    )
    
#Register source files and include directories
idf_component_register( SRCS 		 "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
					  
#Add compiler flags
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-maybe-uninitialized -Wno-return-type -Wno-unused-variable -Wno-unused-but-set-variable -Wno-implicit-function-declaration")					  

