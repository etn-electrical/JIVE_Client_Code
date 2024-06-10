#Set source files 
set( srcs "Flash_NV.cpp"
		  "NV_Crypto.cpp" 	
   )
   
#Set include directories
set (include_dirs "."
				  "../../../Code"
				  "../../../Code/BFData"
				  "../../../Code/Config"
				  "../uC/ESP32D0WD"
				  "../BF"
				  "../BF/DCI"
				  "../NV"
				  "../NVCtrls"
				  "../OS_FreeRTOS"				  
				  "../DebugManager"
				  "../Crypt_API"
				  "../Lib"
				  "../MiscCode"
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  #Ahmed
				  "../../../esp32_common/esp-idf/components/esp_system/include/esp_private"

				  "../../../esp32_common/esp-idf/components/spi_flash/include"
				  "../../../esp32_common/esp-idf/components/bootloader_support/include"
				  "../../../esp32_common/esp-idf/components/app_update/include"
				  "../../../esp32_common/esp-idf/components/esp_rom/include"
		
				  "../../../esp32_common/esp-idf/components/bootloader_support/include"	  
				  "../../../esp32_common/esp-idf/components/app_update/include"
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

#CMake Compiler Options 
#idf_build_set_property(COMPILE_OPTIONS "-Wno-reorder" APPEND)  
