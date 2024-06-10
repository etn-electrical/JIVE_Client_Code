#Set source files 
set( srcs "BF_Shell_Main.cpp"
		  "BF_Shell.cpp"	
		  "../../Babelfish/Code/MiscCode/Shell/Shell_Base.cpp" 
   )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "../../Babelfish/Code/Lib"
				  "../../Babelfish/Code/MiscCode/Shell"
				  "../../Babelfish/Code/BF/DCI"
				  "../../Babelfish/Code/BF"
				  "../../Babelfish/Code/uC/ESP32D0WD"
				  "../BFData"
				  "../Config"
				  "../../Babelfish/Code/OS_FreeRTOS"
				  "../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../Babelfish/Code/DebugManager"
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

  
