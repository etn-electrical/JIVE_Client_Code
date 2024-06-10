#Set source files 
set( srcs "app_main.cpp"
		  "Main.cpp"
#Ahmed
                    "Breaker.c" 
                    "EventManager.c"
                    "KeyManager.c"
                    "Led.c"
                    "DeviceGPIO.c"
                    "Flash.c"
                    "Device_IOT_DCI_Interface.cpp"
  		     "BLE_Provisioning.c"
			"uart_events.c"
			"SBLCP/sblcp.c"
			"../Babelfish/Code/ThirdParty/crypto_matixssl/crypto/digest/hmac.c"
     	   "Log.c"
           "ErrorLog.c"
		   "Metrology/metro.c"
		   "Metrology/metrology_hal.c"
		   "Metrology/metrology.c"
		   "Metrology/spi_comp.c"
		   "Metrology/interrupt_comp.c"
		   "Metrology/metro_utilities.c"
		   "EERAM/i2c_comp.c"
		   "EERAM/eeram_comp.c"
		  "Prod_Spec_LTK_ESP32.cpp"
		  "Prod_Spec.cpp"
		  "Prod_Spec_Mem.cpp"
		  "Prod_Spec_OS.cpp"
		  "Prod_Spec_Debug_Manager.cpp"
		  "Prod_Spec_IOT.cpp"
		  "Prod_Spec_RTC.cpp"
  		  "Prod_Spec_Services.cpp"
		  "../Babelfish/Code/Ethernet/Web/diag.cpp"	
		  "Prod_Spec_Ethernet.cpp"
  		  "Prod_Spec_Services.cpp"
		  "Prod_Spec_BACnet/Prod_Spec_BACNET.cpp"
		  "Prod_Spec_BACnet/BACnetServer.cpp"  
		  "Prod_Spec_BACnet/Shadow_BACnet.cpp"
		  "../Code/BFData/REST_DCI_Data.cpp"
		  "Prod_Spec_FUS.cpp"
  		  "Prod_Spec_REST/Prod_Spec_REST.cpp"
		  "Prod_Spec_EIP/Prod_Spec_EIP.cpp"
		  "Prod_Spec_EIP/Shadow_LTK.cpp"
		  "Prod_Spec_EIP/EIP_if.cpp"
		  "Prod_Spec_EIP/EIP_Service_C_Connector.cpp"
		  "Prod_Spec_EIP/EIP_Service.cpp"
		  "Prod_Spec_EIP/platform_app.cpp"
		  "Prod_Spec_EIP/EIP_if_C_Connector.cpp"
		  "Prod_Spec_PKI.cpp"
		  "Prod_Spec_NV_Refresh.cpp"
  		  "../Babelfish/Code/Crypt_API/API_Matrixssl/Crypt_Aes_CBC_Matrix.cpp"
  		  "Prod_Spec_Webserver.cpp"
  		  "Prod_Spec_Cert_ECC_CA.cpp"
  		  "NV_Refresh_List.cpp"
		  "../Babelfish/Code/IOT/Ltk_ConsoleLogger.cpp"
		  "../Babelfish/Code/BF/Wifi/Esp32_Sta_Config.cpp"
		  "../Babelfish/Code/BF/IOT/IOT_DCI_Config.cpp"
		  "../Babelfish/Code/BF/IOT/IoT_DCI.cpp"
		  "../Babelfish/Code/BF/IOT/IOT_Net.cpp"
		  "../Babelfish/Code/BF/IOT/IOT_Device_Command.cpp"
		  "../Babelfish/Code/BF/IOT/IOT_Group_Handler.cpp"
		  "../Babelfish/Code/BF/IOT/IOT_Pub_Sub.cpp"
		  "../Babelfish/Code/Lib/StdLib_MV.cpp"
		  "../Babelfish/Code/Lib/Services.cpp"
		  "../Babelfish/Code/MiscCode/FUS/FUS.cpp"
		  "../Babelfish/Code/BF/IOT/IOT_Fus.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Component_Ctrls/Local_Component.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Images_Ctrls/Esp32_Image_Ctrl.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Images_Ctrls/Image_Header.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Cert_Store_NV.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Images_Ctrls/Commit_Copy.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Images_Ctrls/Commit_None.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Fw_Store_NV.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Component_Ctrl.cpp"
		  "../Babelfish/Code/MiscCode/FUS/Image.cpp"
		  "../Code/BFData/DCI_Data.cpp"
		  "../Code/BFData/IOT_DCI_Data.cpp"
		  "../Code/BFData/BLE_DCI_Data.cpp"
		  "../Code/BFData/Base_DCI_BACnet_Data.cpp"
		  "../Code/Shell/BF_Shell_Main.cpp"
		  "../Code/Shell/BF_Shell.cpp"
		  "../Code/Shell/Test_Base.cpp"
		  "../Babelfish/Code/DebugManager/Debug_Interface_UART.cpp"
		  "../Babelfish/Code/MiscCode/Shell/Shell_Base.cpp"
		  "../Babelfish/Code/Ethernet/sntp_if.cpp"
		  "../Babelfish/Code/BF/BLE/Ble_Defines.cpp"
		  #Ahmed "../Babelfish/Code/BF/BLE/ESP32_BLE/Esp32_Ble_Api.cpp"
		  "../Babelfish/Code/BF/BACnet"
		  "../Babelfish/Code/MiscCode/BIN_ASCII_Conversion.cpp"
		  "../Babelfish/Code/MiscCode/INT_ASCII_Conversion.cpp"
		  "../Babelfish/Code/MiscCode/ASCII_Conversion.cpp"
		  "../Babelfish/Code/MiscCode/Faults.cpp"
		  "../Babelfish/Code/OS_FreeRTOS/OS_Mutex.cpp"
		  "../Babelfish/Code/OS_FreeRTOS/OS_Semaphore.cpp"
		  "../Babelfish/Code/OS_FreeRTOS/OS_Binary_Semaphore.cpp"
		  "../Babelfish/Code/TimeConversion/POSIXTimeUtil.cpp"
		  "../Babelfish/Code/Ethernet/TLS/Ssl_Context.cpp"
		  "../Babelfish/Code/uC/ESP32D0WD/uC_Rand_Num.cpp"
		  "BFData/Fault_Data.cpp"
		 "../Babelfish/Code/Lib/Scratch_Ram.cpp"
		  "../Babelfish/Code/uC/ESP32D0WD/uC_Code_Range.cpp"
		  "../Babelfish/Code/uC/Generic/uC_Fw_Code_Range.cpp"
		  "../Babelfish/Code/MiscCode/Esp32_Partition.cpp"
   )
   
#Set include directories
set (include_dirs "Test_Code"
				  "Config"
				  "Prod_Spec_Logging"
				  "Prod_Spec_REST"
				  "Prod_Spec_BACnet"
				  "Prod_Spec_EIP"
				  "EERAM"
				  "Metrology"
				  "SBLCP"
				  "../Code"
				  "../Code/BFData"
				  "../Code/Shell"
				  "../Code/Config"
				  "../Babelfish/Code/BF"
				  "../Babelfish/Code/BF/EtherNetIP"
				  "../Babelfish/Code/BF/Wifi"
				  "../Babelfish/Code/BF/IOT"
				  "../Babelfish/Code/BF/BLE"
				  "../Babelfish/Code/BF/BACnet"
				  "../Babelfish/Code/BF/BLE/ESP32_BLE"
				  "../Babelfish/Code/MiscCode/Shell"
				  "../Babelfish/Code/Lib"
				  "../Babelfish/Code/MiscCode"
				  "../Babelfish/Code/uC"
				  "../Babelfish/Code/uC/ESP32D0WD"
				  "../Babelfish/Code/uC/Generic"
				  "../Babelfish/Code/TimeConversion"
				  "../Babelfish/Code/OS_FreeRTOS"
				  "../Babelfish/Code/BF/DCI"
				  "../Babelfish/Code/BF/Modbus"
				  "../Babelfish/Code/NV"
				  "../Babelfish/Code/NVCtrls"
				  "../Babelfish/Code/DebugManager"
				  "../Babelfish/Code/Ethernet"
				  "../Babelfish/Code/Ethernet/Driver"
				  "../Babelfish/Code/Ethernet/TLS"
				  "../Babelfish/Code/Ethernet/Web"
				  "../Babelfish/Code/IOT/platform_device_sdk_v2/include"
				  "../Babelfish/Code/ThirdParty/BACnetStack"
				  #"../Babelfish/Code/ThirdParty/Azure_IOT"
				  "../Babelfish/Code/ThirdParty/AzIoT_MW_Integration/Azure_ltk_adapters"
				  #"../Babelfish/Code/ThirdParty/Azure_IOT/Azure_repo/iothub_client/inc"
				  "../Babelfish/Code/ThirdParty/AzIoT_MW_Integration/umock_c/inc"
				  "../Babelfish/Code/ThirdParty/AzIoT_MW_Integration/c_utility/inc"
				  "../Babelfish/Code/ThirdParty/AzIoT_MW_Integration/c_utility/src"
				  "../Babelfish/Code/BF/Modbus"
				  "../Babelfish/Code/ThirdParty/Pyramid_EIP/Code"
				  "../Babelfish/Code/Crypt_API"
				  "../Babelfish/Code/Crypt_API/API_Matrixssl"
				  "../Babelfish/Code/MiscCode/FUS"
				  "../Babelfish/Code/MiscCode/FUS/Component_Ctrls"
				  "../Babelfish/Code/MiscCode/FUS/Images_Ctrls"
				  #"../Babelfish/Code/ThirdParty/Azure_IOT/Azure_ltk_adapters"
				  #"../Babelfish/Code/ThirdParty/Azure_IOT/Azure_repo/iothub_client/inc"
				  #"../Babelfish/Code/ThirdParty/Azure_IOT/Azure_repo/umock_c/inc"
				  #"../Babelfish/Code/ThirdParty/Azure_IOT/Azure_repo/c_utility/inc"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/matrixssl"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/matrixssl_ltk"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/apps/ssl"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/core"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/core/include"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/core/osdep/include"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/crypto"
				  "../Babelfish/Code/ThirdParty/crypto_matixssl/crypto/digest"
				  "../Babelfish/Code/Crypt_API/API_Matrixssl"
				  "../esp32_common/esp-idf/components/freertos/port/xtensa/include/freertos"
				  "../esp32_common/esp-idf/components/freertos/include/freertos"
				  #Ahmed
				  "../esp32_common/esp-idf/components/esp_system/include/esp_private"
				  
				  "../esp32_common/esp-idf/components/spi_flash/include"
				  "../esp32_common/esp-idf/components/bootloader_support/include"
				  "../esp32_common/esp-idf/components/app_update/include"
				  "../esp32_common/esp-idf/components/nvs_flash/include"
				  "../esp32_common/esp-idf/components/driver/include/driver"
				  "../esp32_common/esp-idf/components/driver"
				  "../esp32_common/esp-idf/components/bt/include"
				  "../esp32_common/esp-idf/components/bt/host/bluedroid/api/include/api"
				  "../esp32_common/esp-idf/components/bt/host/bluedroid/common/include/common"
				  "../esp32_common/esp-idf/components/bt/host/bluedroid/common/include"
				  "../esp32_common/esp-idf/components/bt/common/include"
				  "../esp32_common/esp-idf/components/bt/common/osi/include"
				  "../esp32_common/esp-idf/components/bt/host/bluedroid/stack/include"
				  "../esp32_common/esp-idf/components/bt/host/bluedroid/stack/include/stack"
				  "../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"		
				  
    )
#------------------ Ethernet -----------------------------------#
if(ETHERNET_SETUP_ENABLE)
list(APPEND srcs
		"Prod_Spec_Ethernet.cpp")
list(APPEND include_dirs
		"../Babelfish/Code/Ethernet")
endif(ETHERNET_SETUP_ENABLE)

#------------------ BACnet IP ----------------------------------#
if(BACNET_IP_SETUP_ENABLE)
list(APPEND srcs
		"Prod_Spec_BACnet/Prod_Spec_BACNET.cpp"
		"Prod_Spec_BACnet/BACnetServer.cpp"  
		"Prod_Spec_BACnet/Shadow_BACnet.cpp"
		"../Code/BFData/Base_DCI_BACnet_Data.cpp")
list(APPEND include_dirs
		"Prod_Spec_BACnet"
		"../Babelfish/Code/BF/BACnet"
		"../Babelfish/Code/ThirdParty/BACnetStack")
endif(BACNET_IP_SETUP_ENABLE)

#------------------ BLE GATTS-----------------------------------#
if(BLE_GATTS_ENABLE)
list(APPEND srcs
		"Prod_Spec_Ble.cpp"
		"../Code/BFData/BLE_DCI_Data.cpp"
		"../Babelfish/Code/BF/BLE/Ble_Defines.cpp"
				  #Ahmed 
			"../Babelfish/Code/BF/BLE/ESP32_BLE/Esp32_Ble_Api.cpp"
		"BFData/BLE_DCI_Data.cpp")
list(APPEND include_dirs
		"../Babelfish/Code/BF/BLE"
		"../Babelfish/Code/BF/BLE/ESP32_BLE")
endif(BLE_GATTS_ENABLE)

#------------------ WIFI Station Mode---------------------------#
if(WIFI_STATION_MODE_ENABLE)
list(APPEND srcs
		"Prod_Spec_Wifi.cpp"
		"../Babelfish/Code/BF/Wifi/Esp32_Sta_Config.cpp")
list(APPEND include_dirs
		"../Babelfish/Code/BF/Wifi")
endif(WIFI_STATION_MODE_ENABLE)

#------------------ WIFI Access Point Mode---------------------#
if(WIFI_ACCESS_POINT_MODE_ENABLE)
list(APPEND srcs
		"Prod_Spec_Wifi.cpp")
list(APPEND include_dirs
		"../Babelfish/Code/BF/Wifi")
endif(WIFI_ACCESS_POINT_MODE_ENABLE)

#------------------ Modbus TCP---------------------------------#
if(MODBUS_TCP_SETUP_ENABLE)
list(APPEND include_dirs
		"../Babelfish/Code/BF/Modbus")
endif(MODBUS_TCP_SETUP_ENABLE)

#------------------ IOT Support---------------------------------#
if(PX_GREEN_IOT_SUPPORT_ENABLE)
list(APPEND srcs
		"../Babelfish/Code/IOT/Ltk_ConsoleLogger.cpp"
		"../Babelfish/Code/BF/IOT/IOT_DCI_Config.cpp"
		"../Babelfish/Code/BF/IOT/IoT_DCI.cpp"
		"../Babelfish/Code/BF/IOT/IOT_Net.cpp"
		"../Babelfish/Code/BF/IOT/IOT_Device_Command.cpp"
		"../Babelfish/Code/BF/IOT/IOT_Group_Handler.cpp"
		"../Babelfish/Code/BF/IOT/IOT_Pub_Sub.cpp"
		"../Babelfish/Code/BF/IOT/IOT_Fus.cpp"
		"../Code/BFData/IOT_DCI_Data.cpp"
		"../Babelfish/Code/IOT/platform_device_sdk_v2/source/iot_device_common.c")
list(APPEND include_dirs
		"../Babelfish/Code/IOT"
		#"../Babelfish/Code/ThirdParty/Azure_IOT"
		#"../Babelfish/Code/ThirdParty/Azure_IOT/Azure_repo/c_utility/src"
		)
endif(PX_GREEN_IOT_SUPPORT_ENABLE)


#Conditional compilation based on target selected
if (CONFIG_IDF_TARGET STREQUAL "esp32")
list(APPEND include_dirs
		"../esp32_common/esp-idf/components/bt/include/esp32/include")
endif(CONFIG_IDF_TARGET STREQUAL "esp32")

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
						REQUIRES "wifi_provisioning" "nvs_flash" "esp_common" "esp_wifi" "efuse" "cmpAzIoT_MW" "cmpAzIoT_Utils" "cmpAzureSdkForC" "cmpUmock" "cmp_c_utility"
					  )
					  
#Adding OTA partition size in FUS, It will get used to defined data_length in PXGreen Header
#It will get use at bootloader time, Due to this not fetching from partition table.
#APP_IMAGE_SIZE size is in Kilo Bytes. calcualtion is like (APP_IMAGE_SIZE * 1024) Bytes
add_definitions(-DAPP_IMAGE_SIZE=1536)

#Adding prodinfo_for_boot_s PROD_INFO_HEADER part of PXGreen header in ESP32. This include prod info.
target_link_libraries(${COMPONENT_TARGET} "-u PROD_INFO_HEADER")
#Adding fw_header APP_HEADER part of PXGreen header in ESP32. This include basic Image info.
target_link_libraries(${COMPONENT_TARGET} "-u APP_HEADER")
#Adding crc_info crc_header part of PXGreen header in ESP32. This include Image CRC16 value.
target_link_libraries(${COMPONENT_TARGET} "-u crc_header")

#Adding Compiler option for this component to disable re-order warnings

set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder -Wno-stringop-truncation" ) 

