#Set source files 
set( srcs "Ssl_Context.cpp"			
          "ssl_connection.cpp"
          "Tls_Server.cpp"		
  
   )
   
#Set include directories
set (include_dirs 
				  "."
				  "../"
				  "../Web"
				  "../../BF"
				  "../../BF/DCI"
				  "../../Lib"
				  "../../uC/ESP32D0WD"
				  "../../OS_FreeRTOS"
				  "../../DebugManager"
				  "../../NV"
				  "../../NVCtrls"
				  "../../MiscCode"
				  "../../MiscCode/FUS"			  
				  "../../Crypt_API"
				  "../../Crypt_API/API_Matrixssl"
				  "../../ThirdParty/crypto_matixssl"
				  "../../ThirdParty/crypto_matixssl/matrixssl"
				  "../../ThirdParty/Azure_IOT/Azure_ltk_adapters"
				  "../../ThirdParty/crypto_matixssl/matrixssl_ltk"
				  "../../ThirdParty/crypto_matixssl/apps/ssl"
				  "../../ThirdParty/crypto_matixssl/core"
				  "../../ThirdParty/crypto_matixssl/core/include"
				  "../../ThirdParty/crypto_matixssl/core/osdep/include"
				  "../../ThirdParty/crypto_matixssl/crypto"
				  "../../../../Code"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../../../esp32_common/esp-idf/components/lwip/lwip/src/include"
				  "../../../../esp32_common/esp-idf/components/lwip/lwip/src/include/lwip"
				  "../../../../esp32_common/esp-idf/components/lwip/lwip/src/include/compat/posix"
				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )
#Adding Compiler option for this component to disable warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder")
