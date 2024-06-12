#Set source files 
set( srcs "Code/eipasm.c"
           "Code/eipclien.c"
           "Code/eipcnct.c"
           "Code/eipcnlst.c"
           "Code/eipcnmgr.c"
           "Code/eipdatbl.c"
           "Code/eipdbg.c"
           "Code/eipetlnk.c"
           "Code/eipfile.c"
           "Code/eipid.c"
           "Code/eipio.c"
           "Code/eipnotfy.c"
           "Code/eippccc.c"
           "Code/eippdu.c"
           "Code/eipport.c"
           "Code/eipqos.c"
           "Code/eiproutr.c"
           "Code/eiprqgrp.c"
           "Code/eiprqst.c"
           "Code/eipsckt.c"
           "Code/eipsessn.c"
           "Code/eiptcpip.c"
           "Code/eiptrace.c"
           "Code/eipucmm.c"
           "Code/eiputil.c"
           		   

	)
   
#Set include directories
set (include_dirs 
				  "."
				  "../"
				  "Code"
				  "../../Lib"
				  "../../BF"
				  "../../BF/IOT"
				  "../../BF/DCI"
				  "../../BF/EtherNetIP"
				  "../../Ethernet"
				  "../../MiscCode"
				  "../../../../Code"
				  "../../../../Code/Prod_Spec_EIP"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../uC/ESP32D0WD"
				  "../../OS_FreeRTOS"
				  
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../../esp32_common/esp-idf/components/bootloader_support/include"
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                        INCLUDE_DIRS "${include_dirs}"
                      )

# Force GCC to include the file Override.h at the top of eiproutr.c 
# GCC's -include FILE is equivalent to having the line #include FILE at the top of the file
set_source_files_properties(Code/eiproutr.c PROPERTIES COMPILE_FLAGS "-include ../../../../Code/Prod_Spec_EIP/Override.h")

#Add compiler flags
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-unused-variable -Wno-unused-but-set-variable -Wno-array-bounds -Wno-empty-body")					  
					  
