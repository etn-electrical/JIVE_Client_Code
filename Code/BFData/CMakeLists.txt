#Set source files 
set( srcs
	 
	  "IOT_DCI_Data.cpp"
	  "REST_DCI_Data.cpp"
	  "Base_DCI_BACnet_Data.cpp"
	  #Ahmed "Modbus_DCI_Data.cpp"
          "Fault_Data.cpp"
	  "DynamicAssembly_DCI_Data.cpp"
	  "EIP_DCI_Data.cpp"

    )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "../Config"
				  "../../Babelfish/Code/BF"
				  "../../Babelfish/Code/BF/BLE"
				  "../../Babelfish/Code/BF/Modbus"
				  "../../Babelfish/Code/BF/EtherNetIP"
				  "../../Babelfish/Code/Lib"
				  "../../Babelfish/Code/MiscCode"
				  "../../Babelfish/Code/Ethernet/Web"
				  "../../Babelfish/Code/uC/ESP32D0WD"
				  "../../Babelfish/Code/OS_FreeRTOS"
				  "../../Babelfish/Code/BF/DCI"
				  "../../Babelfish/Code/BF/IOT"
				  "../../Babelfish/Code/ThirdParty/BACnetStack"
				  "../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../esp32_common/esp-idf/components/freertos/include/freertos"				  
				  "../../esp32_common/esp-idf/components/bt/include"				  
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/api/include/api"
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include/common"
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include"
				  "../../esp32_common/esp-idf/components/bt/common/include"
				  "../../esp32_common/esp-idf/components/bt/common/osi/include"
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/stack/include"
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/stack/include/stack"				  				  
				  "../../esp32_common/esp-idf/components/spi_flash/include"				  				  
				  "../../esp32_common/esp-idf/components/nvs_flash/include"				  				  
				  
    )
    
#Register source files and include directories
idf_component_register( SRCS         "${srcs}"
                        INCLUDE_DIRS "${include_dirs}"
                       )
					  
#Add compiler based option for disabled reorder warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder -Wno-unused-variable")
