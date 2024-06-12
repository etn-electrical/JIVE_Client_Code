#Set source files 
set( srcs "Debug_Manager.cpp"
		  "Debug_Interface_Term_IO.cpp"
		  "../BF/DCI/DCI_Patron.cpp"
		  "../BF/DCI/Range_Check.cpp"
		  "Debug_Interface_Uart.cpp"
   )
   
#Set include directories
set (include_dirs "."
				  "../BF/DCI"
				  "../Lib"
				  "../BF"
				  "../uC/ESP32D0WD"
				  "../../../Code"
				  "../../../Code/BFData"
				  "../../../Code/Config"
				  "../OS_FreeRTOS"
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../Code/Shell"
				  "../../../Code/Shell/Commands"
				  "../MiscCode"
				  "../MiscCode/Shell"				  				  
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

  
