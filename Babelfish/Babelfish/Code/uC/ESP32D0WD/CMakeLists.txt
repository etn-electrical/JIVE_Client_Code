#Set source files 
set( srcs "uC_IO_Config.cpp"
		  "uC_Base.cpp"
		  "uC_Base_HW_IDs_ESP32.cpp"
		  "uC_USART_DMA.cpp"
		  "uC_USART_HW.cpp"
		  "uC_UHCI.cpp"
		  "uC_Watchdog.cpp"
		  "uC_Watchdog_HW.cpp"
		  "uC_Reset.cpp"
		  "uC_Flash.cpp"
		  "uC_Rand_Num.cpp"
		  "uC_RTC.cpp"
		  #Ahmed"ESP32_Eth_Driver.cpp"
		  #Ahmed"Ethernetif_ESP32.cpp"
		  "uC_Code_Range.cpp"
		  "uC_IPCC.cpp"
		  "uC_OS_Tick.cpp"
		  "uC_Single_Timer.cpp"
		  "uC_Interrupt.cpp"
		  "uC_Base_HW_IDs_ESP32.cpp"
		  "uC_Ext_Int.cpp"
   )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "../../OS_FreeRTOS"
				  "../../../../Code"
				  "../../../../Code/Config"
				  "../../../../Code/BFData"
				  "../../MiscCode/FUS"	
				  "../../TimeConversion"
				  "../../Lib"
				  "../../BF"
				  "../../NVCtrls"	
				  "../../DebugManager"
				  "../Generic"
				  "../../BF/DCI"				
				  "../../NV"	  
				  "../../Ethernet"
				  "../../MiscCode"
				  #Ahmed
				  "../../../../esp32_common/esp-idf/components/esp_system/include/esp_private"
				  "../../../../esp32_common/esp-idf/components/soc/esp32/include"
				  "../../../../esp32_common/esp-idf/components/spi_flash/include"
				  "../../../../esp32_common/esp-idf/components/bootloader_support/include"
				  "../../../../esp32_common/esp-idf/components/app_update/include"
				  "../../../../esp32_common/esp-idf/components/esp_rom/include"

				  "../../../../esp32_common/esp-idf/components/bootloader_support/include"	  
				  "../../../../esp32_common/esp-idf/components/app_update/include"
				  "../../../../esp32_common/esp-idf/components/freertos/include"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../../esp32_common/esp-idf/components/esp_ipc/include"
				  #Ahmed
				  "../../../../esp32_common/esp-idf/components/nvs_flash/include"


    )

#ahmed
#------------------ ETHERNET---------------------------#
if(ETHERNET_SETUP_ENABLE)
list(APPEND srcs
		"ESP32_Eth_Driver.cpp"
		"Ethernetif_ESP32.cpp")
endif(ETHERNET_SETUP_ENABLE)


#Conditional compilation based on target selected
if (CONFIG_IDF_TARGET STREQUAL "esp32")
list(APPEND include_dirs
		"../../../../esp32_common/esp-idf/components/soc/esp32/include")
endif(CONFIG_IDF_TARGET STREQUAL "esp32")

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )