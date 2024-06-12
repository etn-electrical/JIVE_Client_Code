#Set source files 
set( srcs "Code_Sign_ECC_Matrix.cpp"
		  "Sha256_Matrix.cpp"	
		  "Sig_Verify_ECC_Matrix.cpp"
		  "Cert_Matrix.cpp"
		  "Pub_Key_ECC_Matrix.cpp"
		  "Csr_Matrix.cpp"
		  "Key_ECC_Matrix.cpp"
		  "Cert_Conf_ECC_Matrix.cpp"
		  "Cert_Signed_CA_Matrix.cpp"
		  "Cert_Signed_SS_Matrix.cpp"

   )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "../../../../Code"
				  "../../../../Code/BFData"
				  "../../../../Code/Config"
				  "../../uC/ESP32D0WD"
				  "../../uC/Generic"
				  "../../BF"
				  "../../Ethernet/Web"
				  "../../MiscCode"
				  "../../MiscCode/FUS"
				  "../../BF/DCI"
				  "../../NV"
				  "../../NVCtrls"
				  "../../OS_FreeRTOS"	
				  "../../DebugManager"
				  "../../Lib"
				  "../../ThirdParty/crypto_matixssl"
				  "../../ThirdParty/crypto_matixssl/crypto"
				  "../../ThirdParty/crypto_matixssl/crypto/common"
				  "../../ThirdParty/crypto_matixssl/core"
				  "../../ThirdParty/crypto_matixssl/core/src"
				  "../../ThirdParty/crypto_matixssl/core/osdep/include"
				  "../../ThirdParty/crypto_matixssl/core/include"
				  "../../ThirdParty/crypto_matixssl/crypto/math"
				  "../../ThirdParty/crypto_matixssl/crypto/digest"
				  "../../ThirdParty/crypto_matixssl/crypto/pubkey"
				  "../../ThirdParty/crypto_matixssl/crypto/keyformat"
				  "../../ThirdParty/crypto_matixssl/matrixssl"
				  "../../ThirdParty/crypto_matixssl/matrixssl_ltk"

				  "../../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../../esp32_common/esp-idf/components/freertos/port/xtensa/include/freertos"
		  
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )	
					  
#Adding Compiler option for this component to disable re-order warnings
set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-reorder")  