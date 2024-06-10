#Set source files 
set( srcs "INT_ASCII_Conversion.cpp"
		  "BIN_ASCII_Conversion.cpp"
		  "ASCII_Conversion.cpp"
		  "Format_Handler.cpp"

		  "Esp32_Partition.cpp"
		  "User_Account.cpp"	
		  "Shadow.cpp"
		  "Mem_Check.cpp"
		  "LED.cpp"
   )
   
#Set include directories
set (include_dirs "."
				  "../../../Code"
				  "../../../Code/BFData"
				  "../../../Code/Config"
				  "../uC/ESP32D0WD"
				  "../BF"
#				  "../BF/BLE"
				  "../Lib"
				  "../BF/DCI"
				  "../NV"
				  "../NVCtrls"
				  "../OS_FreeRTOS"	
				  "./FUS"
				  "../DebugManager"
				  "../Lib"
				  "../../../esp32_common/esp-idf/components/bootloader_support/include"
				  "../../../esp32_common/esp-idf/components/spi_flash/include"
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../esp32_common/esp-idf/components/freertos/port/xtensa/include/freertos"
				  "../../../esp32_common/esp-idf/components/app_update/include"				  
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )		  
					  
#Adding Compiler option for this component to disable warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder")
					  
