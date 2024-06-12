#Set source files 
set( srcs 	"OS_Tasker.cpp"
			"CR_Tasker.cpp"
			"CR_Queue.cpp"
			"OS_Task.cpp"
			"OS_Task_Mon.cpp"
			"OS_Mutex.cpp"
			"OS_Semaphore.cpp"
			"OS_Binary_Semaphore.cpp"		
			"OSCR_Dog.cpp"		
   )
   
#Set include directories
set (include_dirs   "."
					"../OS_FreeRTOS"
				    "../Lib"
				    "../uC/ESP32D0WD"
				    "../DebugManager"
				    "../MiscCode/Shell"
				    "../BF"
				    "../BF/DCI"
					"../../../Code"
					"../../../Code/Config"
					"../../../Code/BFData"
					"../../../esp32_common/esp-idf/components/freertos/include/freertos"
					
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
					  
#Add compiler flags
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-cast-function-type" ) 