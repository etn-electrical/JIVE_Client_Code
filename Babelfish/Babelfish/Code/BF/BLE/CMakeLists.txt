#Set source files 
set( srcs "Ble_Advertisement.cpp"
		  "Eddystone_URL.cpp"
		  "Ble_Api.cpp"
		  "BLE_DCI.cpp"
		  "Ble_User_Management.cpp"
		  "Ble_Defines.cpp"
		  "Single_LED.cpp"
		  
   )

#Set include directories
set (include_dirs "."
				  "../DCI"
				  "../../Lib"
				  "../../MiscCode"
				  "../"
				  "../../uC/ESP32D0WD"
				  "../../../../Code"
				  "../../DebugManager"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../OS_FreeRTOS"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"				  
				  "../../../../esp32_common/esp-idf/components/bt/include"				  				  
				  "../../../../esp32_common/esp-idf/components/bt/host/bluedroid/api/include/api"
				  "../../../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include/common"
				  "../../../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include"
				  "../../../../esp32_common/esp-idf/components/bt/common/include"
				  "../../../../esp32_common/esp-idf/components/bt/common/osi/include"
				  "../../../../esp32_common/esp-idf/components/bt/host/bluedroid/stack/include"
				  "../../../../esp32_common/esp-idf/components/spi_flash/include"				  				  
				  "../../../../esp32_common/esp-idf/components/nvs_flash/include"				  				  
    )

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

#Adding Compiler option for this component to disable re-order warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder")
					  