#Set source files 
set( srcs "AIObject.cpp"
		  "AVObject.cpp"
		  "BACnet_DCI.cpp"
		  "BACnetDCIInterface.cpp"
		  "BACnetStackInterface.cpp"
		  "BACnetUARTInterface.cpp"
		  "Binary_Search.cpp"
		  "BVObject.cpp"
		  "Cov.cpp"
		  "CovChangeTracker.cpp"
		  "DevObject.cpp"
		  "MSVObject.cpp"
		  "../DCI/Range_Check.cpp"
		  "../DCI/DCI_Patron.cpp"
		  "../../NV/NV_Mem.cpp"
		  "../../Ethernet/Trusted_IP_Filter.cpp"
		  "../../ThirdParty/BACnetStack/bip.cpp"
		  "../../ThirdParty/BACnetStack/bvlc.cpp"
		  "../../ThirdParty/BACnetStack/endian.cpp"
		  "../../ThirdParty/BACnetStack/mac.cpp"
		  "../../ThirdParty/BACnetStack/mstp.cpp"
		  "../../ThirdParty/BACnetStack/npool.cpp"
		  "../../ThirdParty/BACnetStack/semaphore.cpp"
		  "../../ThirdParty/BACnetStack/Util.cpp"
		  "../../ThirdParty/BACnetStack/vsbhp.cpp"
		  "../../MiscCode/Format_Handler.cpp" 
		  #Ahmed "../../../../Code/BFData/Modbus_DCI_Data.cpp"   
		  "../../../../Code/Prod_Spec_BACnet/Prod_Specific_BACnet.cpp"
		  "../../../../Code/Prod_Spec_BACnet/Shadow_BACnet.cpp"
   )

#Set include directories
set (include_dirs "."
                  "../"
				  "../DCI"
				  "../Modbus"
				  "../../Lib"
				  "../../NV"
				  "../../MiscCode"           
				  "../../Ethernet"
				  "../../DebugManager"
				  "../../uC/ESP32D0WD"
				  "../../OS_FreeRTOS"
				  "../../ThirdParty/BACnetStack"
				  "../../../../Code"
				  "../../../../Code/BFData"
				  "../../../../Code/Prod_Spec_BACnet"
				  "../../../../Code/Config"				  				  
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  
			
    )

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )

#Adding Compiler option for this component to disable re-order warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wunused-value -Wno-maybe-uninitialized")