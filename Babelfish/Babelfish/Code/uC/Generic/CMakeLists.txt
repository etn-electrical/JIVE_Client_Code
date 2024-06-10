#Set source files 
set( srcs "Rtc_If.cpp"
		  "../ESP32D0WD/uC_RTC.cpp"
   )
   
#Set include directories
set (include_dirs "."
				  "../ESP32D0WD"
				  "../../Lib"
				  "../../TimeConversion"
				  "../../OS_FreeRTOS"				  
				  "../../../../Code"
				  "../../../../Code/Config"
)			

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
