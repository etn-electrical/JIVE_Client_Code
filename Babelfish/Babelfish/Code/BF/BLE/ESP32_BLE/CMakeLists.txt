#Set source files 
set( srcs "Esp32_Ble_Api.cpp")



#Set include directories
set (include_dirs "."
				  "../"
				  "../../"				  
				  "../../DCI"
				  "../../../DebugManager"
				  "../../../MiscCode"
				  "../../../Lib"
				  "../../../uC/ESP32D0WD"
				  "../../../../../Code"
				  "../../../../../Code/BFData"
				  "../../../../../Code/Config"
				  "../../../OS_FreeRTOS"
				  "../../../../../esp32_common/esp-idf/components/freertos/include/freertos"				  
				  "../../../../../esp32_common/esp-idf/components/bt/host/bluedroid/api/include/api"
				  "../../../../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include/common"
				  "../../../../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include"
				  "../../../../../esp32_common/esp-idf/components/bt/common/include"
				  "../../../../../esp32_common/esp-idf/components/bt/common/osi/include"
				  "../../../../../esp32_common/esp-idf/components/bt/host/bluedroid/stack/include"
				  "../../../../../esp32_common/esp-idf/components/spi_flash/include"				  				  
				  "../../../../../esp32_common/esp-idf/components/nvs_flash/include"
    )

#Conditional compilation based on target selected
if (CONFIG_IDF_TARGET STREQUAL "esp32")
list(APPEND include_dirs
		"../../../../../esp32_common/esp-idf/components/bt/include/esp32/include")
endif(CONFIG_IDF_TARGET STREQUAL "esp32")

#Register source files and include directories
#ahmed
if(BLE_GATTS_ENABLE)
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

					  
#Adding Compiler flags
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-unused-but-set-variable -Wno-cast-function-type")
endif(BLE_GATTS_ENABLE)