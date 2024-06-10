#Set source files 
set( srcs "Prod_Spec_BACNET.cpp"
		  "BACnetServer.cpp" 
		  "Prod_Specific_BACnet.cpp" 
		  "Shadow_BACnet.cpp"	   	  
   )

#Set include directories
set (include_dirs "."
                  "../"
                  "../BFData"
                  "../Config"
                  "../../Code"
                  "../../Code/Test_Code"
                  "../../Code/Shell"
				  "../../Babelfish/Code/BF"
                  "../../Babelfish/Code/Lib"
                  "../../Babelfish/Code/DebugManager"
                  "../../Babelfish/Code/MiscCode"
                  "../../Babelfish/Code/MiscCode/shell"
                  "../../Babelfish/Code/NV"
                  "../../Babelfish/Code/uC/ESP32D0WD"
                  "../../Babelfish/Code/OS_FreeRTOS"
                  "../../Babelfish/Code/BF/DCI"
                  "../../Babelfish/Code/BF/BACnet"
	              "../../Babelfish/Code/ThirdParty/BACnetStack"	
	              "../../esp32_common/esp-idf/components/freertos/include/freertos"
	              
    )

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
