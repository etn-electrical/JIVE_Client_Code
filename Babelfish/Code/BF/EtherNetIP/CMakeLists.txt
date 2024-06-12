#Set source files 
set( srcs "Dynamic_In_Asm.cpp"
          "CIP_Asm.cpp"
          "CIP_Ctrl_Spvsr.cpp"
          "CIP_Ident_Svc.cpp"
          "Dynamic_Out_Asm.cpp"
          "EIP_Device_Identity.cpp"
          "EIP_Display.cpp"
          "EIP_Obj_Svc.cpp"
          "EtherNetIP_DCI.cpp"

   )

#Set include directories
set (include_dirs "."
                  "../"
                  "../DCI"
                  "../Delta_Com"
                  "../../ThirdParty/Pyramid_EIP/Code"
                  "../../MiscCode"
                  "../../NV"
                  "../../DebugManager"
                  "../../Ethernet"
                  "../../Lib"
                  "../../../../Code"
                  "../../../../Code/BFData"
                  "../../../../Code/Config"
                  "../../../../Code/Prod_Spec_EIP"
                  "../../uC/ESP32D0WD"
                  "../../OS_FreeRTOS"
                  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"	
				  			  
			  				  
    )

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                        INCLUDE_DIRS "${include_dirs}"
                      )

#Adding Compiler option for this component to disable re-order warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder -Wno-array-bounds")
					  
