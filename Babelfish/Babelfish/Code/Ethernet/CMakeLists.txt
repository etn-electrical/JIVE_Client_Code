# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
#cmake_minimum_required(VERSION 3.5)

#set(COMPONENT_ADD_INCLUDEDIRS "")

#Set source files 
set( srcs "sntp_if.cpp"
		  "LWIPService.cpp"
		  #Ahmed "Driver/IP101GRI.cpp"
		  "Trusted_IP_Filter.cpp"
          "Eth_if.cpp"
          "Eth_PHY_if.cpp"
          "Eth_Port.cpp"
          "Sntp_If_C_Connector.cpp"
          "Trusted_IP_Filter_C_Connector.cpp"

   )
   
#Set include directories
set (include_dirs "."
                  "../"
                  "../uC/ESP32D0WD"
				  "../Lib"
				  "../OS_FreeRTOS"
				  "../uC/Generic"
				  "../TimeConversion"
				  "../ThirdParty/BACnetStack"
				  "../BF"
  				  "../BF/DCI"
				  "../DebugManager"
				  "../Ethernet"
				  "../MiscCode"
				  "../../../Code"
				  "../../../Code/BFData"
				  "../../../Code/Config"				  
				  "../../../esp32_common/esp-idf/components/lwip/include/apps/sntp"
				  "../../../esp32_common/esp-idf/components/lwip/include/apps"
				  "../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip/apps"
				  "../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  #Ahmed
				  "../../../esp32_common/esp-idf/components/esp_system/include/esp_private"
)			


#ahmed
#------------------ ETHERNET---------------------------#
if(ETHERNET_SETUP_ENABLE)
list(APPEND srcs
		"Driver/IP101GRI.cpp"
		)
endif(ETHERNET_SETUP_ENABLE)


#Conditional compilation based on target selected
if (CONFIG_IDF_TARGET STREQUAL "esp32")
list(APPEND include_dirs
		"../../../esp32_common/esp-idf/components/lwip/port/esp32/include/netif")
endif(CONFIG_IDF_TARGET STREQUAL "esp32")

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
