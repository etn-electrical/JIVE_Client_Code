#Set source files 
set( srcs "Esp32_Sta_Config.cpp"
		  "Esp32_Station.cpp"	
		  "Esp32_AP_Config.cpp"
		  "Esp32_Access_Point.cpp"
		  	  
   )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "../Wifi"
				  "../DCI"
				  "../../OS_FreeRTOS"
				  "../../uC/ESP32D0WD"
				  "../../Lib"
				  "../../../../Code"
				  "../../../../Code/Config"
				  "../../../../Code/BFData"
				  "../../../../esp32_common/esp-idf/components/log/include/"
				  "../../../../esp32_common/esp-idf/components/freertos"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../../esp32_common/esp-idf/components/esp_system/include"
				  "../../../../esp32_common/esp-idf/components/esp_wifi/include"
				  "../../../../esp32_common/esp-idf/components/esp_event/include"
				  "../../../../esp32_common/esp-idf/components/nvs_flash/include"
				  "../../../../esp32_common/esp-idf/components/spi_flash/include"
				  "../../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"	
				  "../../../../esp32_common/esp-idf/components/esp_common/include"
				  "../../../../esp32_common/esp-idf/components/lwip/include/apps/sntp"
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

#Supress certain compiler warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations")	
