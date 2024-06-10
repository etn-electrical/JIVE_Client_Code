#Set source files 
set( srcs "Shell_Base.cpp"
		  "Command_Base.cpp"
		  "Command_Line_Editor.cpp"
		  "Command_Line_Interpreter.cpp"
   )
   
#Set include directories
set (include_dirs "."
				  "../../BF/DCI"
				  "../../Lib"
				  "../../BF"
				  "../../uC/ESP32D0WD"
				  "../../../Code"
				  "../../../../Code/Shell"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../OS_FreeRTOS"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"	 
				  "../../DebugManager"
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

  
