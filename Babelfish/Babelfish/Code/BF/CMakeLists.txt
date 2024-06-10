#Set source files 
set( srcs "System_Reset.cpp"
		  "../OS_FreeRTOS/CR_Tasker.cpp"
		  "../uC/ESP32D0WD/uC_Reset.cpp"
    )
   
#Set include directories
set (include_dirs "."
				  "Modbus"
				  "../Lib"
				  "../OS_FreeRTOS"
				  "../uC/ESP32D0WD"
				  "../DebugManager"
				  "../BF/DCI"
				  "../../../Code"
				  "../../../Code/BFData"
				  "../../../Code/Config"				  
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  #Ahmed
				  "../../../esp32_common/esp-idf/components/esp_system/include/esp_private"
			
				  
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

  
