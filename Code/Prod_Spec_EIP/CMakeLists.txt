#Set source files 
set( srcs "EIP_if_C_Connector.cpp"
	  "EIP_Service_C_Connector.cpp"
	  "EIP_Service.cpp"
	  "platform_app.cpp"
	  "../BFData/DynamicAssembly_DCI_Data.cpp"
	  "../BFData/EIP_DCI_Data.cpp"
	  "Prod_Spec_EIP.cpp"
	  "Shadow_LTK.cpp"
    )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "../Config"
				  "../BFData"
				  "../../Babelfish/Code/BF"
				  "../../Babelfish/Code/NV"
				  "../../Babelfish/Code/DebugManager"
				  "../../Babelfish/Code/BF/BLE"
				  "../../Babelfish/Code/Lib"
				  "../../Babelfish/Code/MiscCode"
				  "../../Babelfish/Code/uC/ESP32D0WD"
				  "../../Babelfish/Code/OS_FreeRTOS"
				  "../../Babelfish/Code/BF/DCI"
				  "../../Babelfish/Code/BF/EtherNetIP"
				  "../../Babelfish/Code/BF/IOT"
				  "../../Babelfish/Code/Ethernet"
				  "../../Babelfish/Code/ThirdParty/Pyramid_EIP/Code"
				   "../../Babelfish/Code/ThirdParty/lwIP/Code/include/lwip"
				  "../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../esp32_common/esp-idf/components/freertos/include/freertos"				  
				  "../../esp32_common/esp-idf/components/bt/include"				  
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/api/include/api"
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include/common"
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/common/include"
				  "../../esp32_common/esp-idf/components/bt/common/include"
				  "../../esp32_common/esp-idf/components/bt/host/bluedroid/stack/include"			  				  
				  "../../esp32_common/esp-idf/components/spi_flash/include"				  				  
				  "../../esp32_common/esp-idf/components/nvs_flash/include"				  				  
				  
    )
    
#Register source files and include directories
idf_component_register( SRCS         "${srcs}"
                        INCLUDE_DIRS "${include_dirs}"
                       )
					  
#Add compiler based option for disabled reorder warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder -Wno-unused-variable -Wstringop-truncation")
