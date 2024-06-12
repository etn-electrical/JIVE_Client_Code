# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

#-------------To Enable Features, set TRUE/FALSE--------------------------#

option(ETHERNET_SETUP_ENABLE "Enable Ethernet Setup" FALSE)

option(BLE_GATTS_ENABLE "Enable BLE GATTS" FALSE)

option(WIFI_STATION_MODE_ENABLE "Enable Wifi on Station Mode" TRUE)

option(WIFI_ACCESS_POINT_MODE_ENABLE "Enable Wifi as Access Point" FALSE)

option(PX_GREEN_IOT_SUPPORT_ENABLE "Enable overall IoT building" TRUE)

# To Enable BacNet IP, Modbus TCP, EIP Setup and Webserver Support, Ethernet must be enabled
option(BACNET_IP_SETUP_ENABLE "Enable BacNet IP Setup" FALSE)

option(MODBUS_TCP_SETUP_ENABLE "Enable Modbus TCP Communication" FALSE)

option(EIP_SETUP_ENABLE "Enable EIP Setup" FALSE)

option(WEB_SERVER_SUPPORT_ENABLE "Enable Webserver Support" FALSE)

#add_definitions(-DDCI_FLASH_NV_MIRROR_SUPPORT_DISABLE) # To disable mirror support for Flash NV

add_definitions(-DPKI_SUPPORT)

##add_definitions(-DNV_CRYPTO)

if(WIFI_STATION_MODE_ENABLE OR WIFI_ACCESS_POINT_MODE_ENABLE)
option(ETHERNET_SETUP_ENABLE "Enable Ethernet Setup" FALSE)
endif(WIFI_STATION_MODE_ENABLE OR WIFI_ACCESS_POINT_MODE_ENABLE)

if(WIFI_STATION_MODE_ENABLE)
option(WIFI_ACCESS_POINT_MODE_ENABLE "Enable Wifi as Access Point" FALSE)
endif(WIFI_STATION_MODE_ENABLE)

if(WIFI_ACCESS_POINT_MODE_ENABLE)
option(WIFI_STATION_MODE_ENABLE "Enable Wifi on Station Mode" FALSE)
endif(WIFI_ACCESS_POINT_MODE_ENABLE)

if(PX_GREEN_IOT_SUPPORT_ENABLE)
add_definitions(-DRTC_INTERFACE)
add_definitions(-DSNTP_SETUP)
endif(PX_GREEN_IOT_SUPPORT_ENABLE)

if( WIFI_STATION_MODE_ENABLE OR WIFI_ACCESS_POINT_MODE_ENABLE )
add_definitions(-DNV_CRYPTO)
endif( WIFI_STATION_MODE_ENABLE OR WIFI_ACCESS_POINT_MODE_ENABLE )

#---------------------End of features selection--------------------------#

set( EXTRA_COMPONENT_DIRS 
						  "Code/Config"
						  "Code/BFData"
						  "Code/Prod_Spec_BACnet"
						  "Code"
						  "Code/Test_Code"
						  "Code/Shell"
						  "Code/Shell/Commands"
						  "Babelfish/Code/BF/DCI"
						  "Babelfish/Code/TimeConversion"
						  "Babelfish/Code/uC/Generic"
						  "Babelfish/Code/MiscCode"
						  "Babelfish/Code/MiscCode/Shell"
						  "Babelfish/Code/ThirdParty/Pyramid_EIP"
						  "Babelfish/Code/Lib"
						  "Babelfish/Code/NV"
						  "Babelfish/Code/BF"
						  "Babelfish/Code/BF/BLE"
						  "Babelfish/Code/BF/EtherNetIP"
						  "Code/BFData"      
						  "Code/Prod_Spec_EIP"
						  "Babelfish/Code/BF/BACnet"
						  "Babelfish/Code/BF/BLE/ESP32_BLE"
						  "Babelfish/Code/Crypt_API"
						  "Babelfish/Code/Crypt_API/API_Matrixssl"
						  "Babelfish/Code/NVCtrls"
						  "Babelfish/Code/DebugManager"
						  "Babelfish/Code/BF/Wifi"
						  "Babelfish/Code/Ethernet/TLS"
						  "Babelfish/Code/Ethernet/Web"
						  "Babelfish/Code/BF/Modbus"
						  "Babelfish/Code/Ethernet"
						  "Babelfish/Code/OS_FreeRTOS"
						  "Babelfish/Code/uC/ESP32D0WD"
						  "Babelfish/Code/IOT"
						  "Babelfish/Code/ThirdParty/Azure_IOT"
						  "Babelfish/Code/ThirdParty/BACnetStack"
						  "Babelfish/Code/ThirdParty/Azure_IOT/Azure_ltk_adapters"
						  "Babelfish/Code/ThirdParty/crypto_matixssl"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto/common"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/core/src"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto/math"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto/digest"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto/prng"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto/pubkey"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto/keyformat"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/crypto/symmetric"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/matrixssl"
						  "Babelfish/Code/ThirdParty/crypto_matixssl/matrixssl_ltk"
   )
   
#Project level macros

#------------------ Ethernet -----------------------------------#
if(ETHERNET_SETUP_ENABLE)
add_definitions(-DETHERNET_SETUP)
list(APPEND EXTRA_COMPONENT_DIRS
						  "Babelfish/Code/Ethernet"
						  "Babelfish/Code/Ethernet/TLS")
endif(ETHERNET_SETUP_ENABLE)

#------------------ BACnet IP ----------------------------------#
if(BACNET_IP_SETUP_ENABLE)
add_definitions(-DBACNET_IP_SETUP)
list(APPEND EXTRA_COMPONENT_DIRS
						  "Code/Prod_Spec_BACnet"
						  "Babelfish/Code/BF/BACnet"
						  "Babelfish/Code/ThirdParty/BACnatStack")
endif(BACNET_IP_SETUP_ENABLE)

#------------------ BLE GATTS-----------------------------------#
if(BLE_GATTS_ENABLE)
add_definitions(-DBLE_GATTS)
list(APPEND EXTRA_COMPONENT_DIRS
						  "Babelfish/Code/BF/BLE"
						  "Babelfish/Code/BF/BLE/ESP32_BLE")
endif(BLE_GATTS_ENABLE)

#------------------ WIFI Station Mode---------------------------#
if(WIFI_STATION_MODE_ENABLE)
add_definitions(-DWIFI_STATION_MODE)
list(APPEND EXTRA_COMPONENT_DIRS
						  "Babelfish/Code/BF/Wifi")
endif(WIFI_STATION_MODE_ENABLE)

#------------------ WIFI Access Point Mode---------------------#
if(WIFI_ACCESS_POINT_MODE_ENABLE)
add_definitions(-DWIFI_ACCESS_POINT_MODE)
list(APPEND EXTRA_COMPONENT_DIRS
						  "Babelfish/Code/BF/Wifi")
endif(WIFI_ACCESS_POINT_MODE_ENABLE)

#------------------ Modbus TCP---------------------------------#
if(MODBUS_TCP_SETUP_ENABLE)
add_definitions(-DMODBUS_TCP_SETUP)
list(APPEND EXTRA_COMPONENT_DIRS
						  "Babelfish/Code/BF/Modbus")
endif(MODBUS_TCP_SETUP_ENABLE)

#------------------ IOT Support---------------------------------#
if(PX_GREEN_IOT_SUPPORT_ENABLE)
add_definitions(-DPX_GREEN_IOT_SUPPORT)
list(APPEND EXTRA_COMPONENT_DIRS
						  "Babelfish/Code/BF/IOT"
						  "Babelfish/Code/IOT"
						  "Babelfish/Code/ThirdParty/Azure_IOT"
						  "Babelfish/Code/ThirdParty/Azure_IOT/Azure_ltk_adapters")
endif(PX_GREEN_IOT_SUPPORT_ENABLE)

#------------------ EIP Setup---------------------#
if(EIP_SETUP_ENABLE)
add_definitions(-DEIP_SETUP)
endif(EIP_SETUP_ENABLE)

#------------------ WebServer Support---------------------#
if(WEB_SERVER_SUPPORT_ENABLE)
add_definitions(-DRTC_INTERFACE)
add_definitions(-DWEB_SERVER_SUPPORT)
add_definitions(-DHTTP_SUPPORT)
add_definitions(-DHTTPS_SUPPORT)
add_definitions(-DWEBUI_ON_EXT_FLASH)		#Enable this to set Web Image download from external flash
endif(WEB_SERVER_SUPPORT_ENABLE)
   
add_definitions(-DFREERTOS_V10)
add_definitions(-DESP32_SETUP)

#This is the overall enabler for IoT building:
#Ahmed
add_definitions(-DPX_GREEN_IOT_SUPPORT)
add_definitions(-DPX_GREEN_IOT_DEVICE)
#We are a client for TLS, to connect to Azure Server
add_definitions(-DUSE_CLIENT_SIDE_SSL)

#Enable this to set the OPTION_SAS_TOKEN_LIFETIME to the given value.
#The Azure default is 60min => 3600secs, if not set here. 
#For MQTT, it uses 80% of the value we set as the interval at which it reconnects.
add_definitions(-DIOT_SAS_TOKEN_REFRESH_SECS=86400)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#Ahmed
project(Smart_Breaker_2_0)

#Python command to fill unused memory with 0xFF till starting of code signing information address
#Maximum binary image size defined for ESP32 is 1536 KB. 
#MAX_IMAGE_SIZE size is in Bytes. Calculation is like (1536 * 1024) Bytes in decimal
set( MAX_IMAGE_SIZE 0x180000 )
#CODE_SIGN_SIZE is the code signing information i.e 4 KB.(CODE_SIGN_SIZE in bytes)
set(CODE_SIGN_SIZE 0x1000)
#Calculation for starting address of code signing information.(CODE_SIGN_LOC in hexadecimal)
math( EXPR CODE_SIGN_LOC "${MAX_IMAGE_SIZE} - ${CODE_SIGN_SIZE}" OUTPUT_FORMAT HEXADECIMAL )

#Command should be in below given format
#"Fill_Unused_Mem.py" "Path of Project Binary file" "Code Signing Offset in App Image" 
#Note : Code signing offset should be in hex only.
set (py_cmd "Fill_Unused_Mem.py" "${CMAKE_BINARY_DIR}/${PROJECT_BIN}" "${CODE_SIGN_LOC}")

#Command details to code sign the image

#Command should be run with following given options 
#Command syntax: "Package_Builder.bat" "User Option I" "User Option II" "User Option III" 
#User Option I->Create Code Pack for ESP32 PROCESSOR 
#User Option II->Enter the number for your choice of signing method:
#            1  ->  Local Sign 
#            2  ->  Server Sign
#User Option III->Enter the number for your choice of Code pack generation:
#            1  ->  Create Code Pack for Web UI
#            2  ->  Create Code Pack for Application
#            3  ->  Create Code Pack for both

#The default configuration is ESP32 processor with Local sign for app image
set (codesign_cmd "Package_Builder.bat" "1" "1" "2") 

add_custom_command(
                  TARGET app
                  POST_BUILD 
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Tools/Firmware_Package_Builder/
                  COMMAND ${PYTHON_EXECUTABLE} ${py_cmd}
                  COMMAND cmd /c ${codesign_cmd}
                  )


#CMake Compiler Options 
#idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)