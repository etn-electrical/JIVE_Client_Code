#Set source files
set( srcs "Test_Example.cpp"
		  "gpio_example.cpp"
	      	  "UDMA_example.cpp"
		  "Test_OS_Example.cpp"
		  "Blink_Example.cpp"
		  "Test_Soft_Timers.cpp"
		  "Test_Fault_Catch.cpp"
		  "Test_Debug_manager.cpp"
		  "IOT_Test_Task.cpp"
		  "Test_CR_Task.cpp"
		  "Test_SNTP.cpp"
		  
		  #Aarti "Spi_flash_test_example.cpp"
		  "Test_IPCC.cpp"
		  "Test_Esp32_Partition.cpp"
		  "../../Babelfish/Code/MiscCode/Esp32_Partition.cpp"
		  "../../Babelfish/Code/uC/ESP32D0WD/uC_Code_Range.cpp"
		  "../../Babelfish/Code/uC/Generic/uC_Fw_Code_Range.cpp"
		  "Test_nSec_Timer.cpp"
		  "Test_Ext_Int.cpp"
		
	 )

#Set include directories
set( include_dirs   "../"
					 "../Config"
					 "../../Code"
					 "../../Code/Shell"
					 "../../Code/BFData"
					 "../../Babelfish/Code/uC"
					 "../../Babelfish/Code/uC/ESP32D0WD"
					 "../../Babelfish/Code/uC/Generic"
					 "../../Babelfish/Code/Lib"
					 "../../Babelfish/Code/OS_FreeRTOS"
					 "../../Babelfish/Code/BF"
					 "../../Babelfish/Code/NV"
					 "../../Babelfish/Code/BF/Wifi"
					 "../../Babelfish/Code/BF/DCI"
					 "../../Babelfish/Code/DebugManager"
					 "../../Babelfish/Code/Ethernet"
					 "../../Babelfish/Code/MiscCode"
					 "../../Babelfish/Code/MiscCode/FUS"
					 "../../esp32_common/esp-idf/components/freertos/include/freertos"
					 
					 #Ahmed
				     "../../esp32_common/esp-idf/components/esp_system/include/esp_private"
				  
					 "../../esp32_common/esp-idf/components/esp_common/include"
					 "../../esp32_common/esp-idf/components/spi_flash/include"
					 "../../esp32_common/esp-idf/components/xtensa/include"
					 "../../esp32_common/esp-idf/components/driver/include/driver"
					 "../../esp32_common/esp-idf/components/bootloader_support/include"
					 "../../esp32_common/esp-idf/components/app_update/include"
					 "../../esp32_common/esp-idf/tools/unit-test-app/components/test_utils/include"
					 "../../esp32_common/esp-idf/components/idf_test/include"

	 )

#Conditional compilation based on target selected
if (CONFIG_IDF_TARGET STREQUAL "esp32")
list(APPEND include_dirs
		"../../esp32_common/esp-idf/components/esp32/include"
		"../../esp32_common/esp-idf/components/idf_test/include/esp32")
endif(CONFIG_IDF_TARGET STREQUAL "esp32")

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
						INCLUDE_DIRS "${include_dirs}"
						)