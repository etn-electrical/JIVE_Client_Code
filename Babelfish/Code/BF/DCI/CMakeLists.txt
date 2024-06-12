#Set source files 
set( srcs "DCI.cpp"
		  "DCI_NV_Mem.cpp"
		  "Change_Tracker.cpp"
		  "DCI_Owner.cpp"
		  "DCI_Patron.cpp"
		  "DCI_Lock.cpp"
		  "Range_Check.cpp"
		  "Change_Tracker_Plus.cpp"
		  "../System_Reset.cpp"
		  
    )
   
#Set include directories
set (include_dirs "."
				  "../../BF"
				  "../../../../Code"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../../../Code/Test_Code"
				  "../../Lib"
				  "../../uC/ESP32D0WD"
				  "../../OS_FreeRTOS"
				  "../../DebugManager"
				  "../../NV"
				  "../../MiscCode"
				  "../../NVCtrls"
				  "../"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  
				  
				  
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
					  
#Add compiler flags
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-unused-variable -Wno-implicit-fallthrough")					  
					  