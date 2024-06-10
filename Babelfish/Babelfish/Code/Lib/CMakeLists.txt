#Set source files 
set( srcs "Toolkit_Revision.cpp"
		  "Ram_Hybrid_ESP32.cpp"
		  "Ram.cpp"
		  "Bit_List.cpp"
		  "StdLib_MV.cpp"
		  "Ram_Static.cpp"
		  "../../../Code/BFData/DCI_Data.cpp"
		  "../BF/DCI/DCI_Owner.cpp"
		  "Timers_Lib.cpp"
		  "Ram_Handler.cpp"
		  "CRC16.cpp"
		  "Ram_C_Connector.cpp"
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
				  "../MiscCode"
				  "../NVCtrls"
				  "../OS_FreeRTOS"				  
				  "../DebugManager"
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

  
