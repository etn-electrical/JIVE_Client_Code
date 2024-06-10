#Set source files 
set( srcs "IOT_DCI_Config.cpp"
		  "IoT_DCI.cpp"
		  "IOT_Net.cpp"
		  "IOT_Device_Command.cpp"
		  "IOT_Group_Handler.cpp"
		  "IOT_Pub_Sub.cpp"
		  "IOT_Fus.cpp"
		 
    )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "../../BF"
				  "../../BF/DCI"
				  "../../Ethernet/Web"
				  "../../Ethernet/TLS"
				  "../../MiscCode"
				  "../../MiscCode/FUS"
				  "../../DebugManager"
				  "../../../../Code"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../Lib"
				  "../../uC/ESP32D0WD"
				  "../../OS_FreeRTOS"
				  "../../NV"
				  "../../NVCtrls"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"
				  "../../../Code/ThirdParty/Azure_IOT/Azure_ltk_adapters"
				  "../../IOT/platform_device_sdk_v2/include"
				  "../../../Code/ThirdParty/Azure_IOT/Azure_repo/iothub_client/inc"
				  "../../../Code/ThirdParty/Azure_IOT/Azure_repo/umock_c/inc"
				  "../../../Code/ThirdParty/Azure_IOT/Azure_repo/c_utility/inc"
				  "../../../Code/ThirdParty/crypto_matixssl"
				  "../../../Code/ThirdParty/crypto_matixssl/matrixssl"
				  "../../../Code/ThirdParty/Azure_IOT/Azure_ltk_adapters"
				  "../../../Code/ThirdParty/crypto_matixssl/matrixssl_ltk"
				  "../../../Code/ThirdParty/crypto_matixssl/apps/ssl"
				  "../../../Code/ThirdParty/crypto_matixssl/core"
				  "../../../Code/ThirdParty/crypto_matixssl/core/include"
				  "../../../Code/ThirdParty/crypto_matixssl/core/osdep/include"
				  "../../../Code/ThirdParty/crypto_matixssl/crypto"
				  "../../../../esp32_common/esp-idf/components/esp_system/include"
				  "../../../../esp32_common/esp-idf/components/esp_wifi/include"
				  "../../../../esp32_common/esp-idf/components/esp_event/include"
				  "../../../../esp32_common/esp-idf/components/nvs_flash/include"
				  "../../../../esp32_common/esp-idf/components/spi_flash/include"
				  "../../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"	
				  "../../../../esp32_common/esp-idf/components/esp_common/include"
    )

#Conditional compilation based on target selected
if (CONFIG_IDF_TARGET STREQUAL "esp32")
list(APPEND include_dirs
		"../../../../esp32_common/esp-idf/components/lwip/port/esp32/include")
endif(CONFIG_IDF_TARGET STREQUAL "esp32")

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
#Adding Compiler option for this component to disable re-order warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder -Wno-format-truncation -Wno-stringop-truncation")
