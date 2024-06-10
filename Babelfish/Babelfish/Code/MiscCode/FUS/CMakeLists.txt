#Set source files
set( srcs "FUS.cpp"
          "Component_Ctrls/Local_Component.cpp"
          "Images_Ctrls/Esp32_Image_Ctrl.cpp"
          "Images_Ctrls/Image_Header.cpp"
          "Images_Ctrls/Commit_Copy.cpp"
          "Images_Ctrls/Commit_None.cpp"
          "Fw_Store_NV.cpp"
          "Component_Ctrl.cpp"
          "Image.cpp"


	 )

#Set include directories
set( include_dirs "."
	 			  "../../Crypt_API/API_Matrixssl"
#Ahmed	 		  
					"../../../Code"
	 							  

	 )

#Register source files and include directories
idf_component_register( SRCS "${srcs}"
						INCLUDE_DIRS "${include_dirs}"
						)

#Adding Compiler option for this component to disable re-order warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder -Wno-stringop-truncation" )
