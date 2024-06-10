#Set source files 
set( srcs "platform_device_sdk_v2/source/azure_certs.c"
		  "platform_device_sdk_v2/source/iot_device.c"
		  "platform_device_sdk_v2/source/iot_device_common.c"
		  "platform_device_sdk_v2/source/iot_encoding.c"
		  "platform_device_sdk_v2/source/iot_json_parser.c"
		  "platform_device_sdk_v2/source/iot_serialization.c"
		  "Ltk_ConsoleLogger.cpp"
    )
   
#Set include directories
set (include_dirs "platform_device_sdk_v2/include"
				  "."
				  "../Lib"
				  "../IOT"
				  "../BF/IOT"
				  "../OS_FreeRTOS"
				  "../uC/ESP32D0WD"
				  "../../../Code"
				  "../DebugManager"
				  "../BF/DCI"
				  "../BF"	  
				  "../ThirdParty/Azure_IOT/Azure_repo/c_utility/inc"
				  "../ThirdParty/Azure_IOT/Azure_repo/c_utility/pal/generic"
				  "../ThirdParty/Azure_IOT/Azure_repo/umock_c/inc"
				  "../ThirdParty/Azure_IOT/Azure_repo/iothub_client/inc"
				  "../ThirdParty/Azure_IOT/Azure_repo/umqtt/inc"
				  "../../../Code/BFData"
				  "../../../Code/Config"				  
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  #Ahmed
				  "../../../esp32_common/esp-idf/components/esp_system/include/esp_private"
				  "../../../esp32_common/esp-idf/components/nvs_flash/include"
				  "../../../esp32_common/esp-idf/components/spi_flash/include"			
				  
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

#Supressing warnings as we are not going to fix them 
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-stringop-truncation -Wno-stringop-overflow -Wno-format-truncation -Wno-format-overflow -Wno-unused-variable -Wno-cast-function-type" ) 
