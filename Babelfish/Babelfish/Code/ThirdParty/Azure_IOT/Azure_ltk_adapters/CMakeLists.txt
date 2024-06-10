#Set source files 
set( srcs "agenttime_ltk.cpp"
		  "lock_ltk.cpp"
		  "platform_ltk.cpp"
		  "socketio_ltk.cpp"
		  "threadapi_ltk_pthreads.cpp"
		  "tickcounter_ltk.cpp"
		  "tlsio_matrixssl.cpp"
		  "../../../OS_FreeRTOS/OS_Mutex.cpp"
		  "../../../OS_FreeRTOS/OS_Semaphore.cpp"

	)
   
#Set include directories
set (include_dirs 
				  "."
				  "../../../../../Code"
				  "../../../../../Code/BFData"
				  "../../../../../Code/Config"
				  "../../../BF/DCI"
				  "../../../Ethernet/"
				  "../../../OS_FreeRTOS"
				  "../../../Ethernet/TLS"
				  "../../../Ethernet/Web"
				  "../../../Lib"
				  "../../../BF"
				  "../../../BF/IOT"
				  "../../../DebugManager"
				  "../../../uC/ESP32D0WD"
				  "../../../IOT/platform_device_sdk_v2/include"
				  "../Azure_repo/c_utility/inc"
				  "../Azure_repo/umock_c/inc"
				  "../Azure_repo/iothub_client/inc"
				  "../../crypto_matixssl/matrixssl"
				  "../../crypto_matixssl/matrixssl_ltk"
				  "../../crypto_matixssl"
				  "../../crypto_matixssl/core"
				  "../../crypto_matixssl/core/osdep/include"
				  "../../crypto_matixssl/core/include"
				  "../../../../../esp32_common/esp-idf/components/lwip/lwip/src/include"
				  "../../../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"
				  "../../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../../../esp32_common/esp-idf/components/tcpip_adapter/include"
				  "../../../../../esp32_common/esp-idf/components/esp_wifi/include"
    )
    
#Register source files and include directories
idf_component_register( SRCS 		 "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

#Add compiler flags
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-unused-variable -Wno-unused-but-set-variable")					  
					  