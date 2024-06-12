#Set source files 
set( srcs "DateUtil.cpp"
		  "POSIXTimeUtil.cpp"
		  "test_time.cpp"
		  "TimeUtil.cpp"
		  "TimeZoneDST.cpp"
		  "TimeZoneDSTAlt.cpp"

   )
   
#Set include directories
set (include_dirs  "."
				   "../../../Code"
				   "../Lib"
				   "../OS_FreeRTOS"
				   "../uC/ESP32D0WD"
				   "../uC/Generic"
				   "../../../Code/Config"
)
 
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

  
