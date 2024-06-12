#Set source files 
set( srcs "Modbus_TCP.cpp"
		  "Modbus_Net.cpp"
		  "Modbus_Msg.cpp"
		  "Modbus_SysCmd.cpp"
		  "Modbus_DCI.cpp"
		  "Modbus_CRC.cpp"
		  #Ahmed"../../../../Code/BFData/Modbus_DCI_Data.cpp"
		  "../../MiscCode/Com_Dog.cpp"
		  "../../Lib/Linked_List.cpp"
		  "../../Lib/Timers_Lib.cpp"
    )
   
#Set include directories
set (include_dirs "."
		          "../../Lib"
	          "../../Ethernet"
		          "../../DebugManager"
		          "../../uC/ESP32D0WD"
		          "../../OS_FreeRTOS" 
		          "../../BF/DCI"
		          "../../BF"
		          "../../MiscCode"
		          "../../../../Code"
		          "../../../../Code/BFData"		 
		          "../../../../Code/Config"
		          "../../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"
		          "../../../../esp32_common/esp-idf/components/freertos/include/freertos"	
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

#Adding Compiler option for this component to disable re-order warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder")
					  

  
