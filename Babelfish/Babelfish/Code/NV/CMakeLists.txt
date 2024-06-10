#Set source files 
set( srcs "NV_Ctrl.cpp"
	      "NV_Mem.cpp"
	      "NV_Refresh.cpp"
    )
   
#Set include directories
set (include_dirs "."
				  "../../../Code"
				  "../../../Code/BFData"
				  "../../../Code/Config"
				  "../Lib"
				  "../uC/ESP32D0WD"
				  "../OS_FreeRTOS"
				  "../NV"
				  "../NVCtrls"
				  "../BF"
				  "../BF/DCI"
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

  
