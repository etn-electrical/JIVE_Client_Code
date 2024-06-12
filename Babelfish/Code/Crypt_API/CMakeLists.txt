#Set source files 
set( srcs "API_Matrixssl/Crypt_Aes_CBC_Matrix.cpp"
   )
   
#Set include directories
set (include_dirs "."
				  "../"
				  "API_Matrixssl"
				  "../../../Code"
				  "../../../Code/BFData"
				  "../../../Code/Config"
				  "../uC/ESP32D0WD"
				  "../BF"
				  "../BF/DCI"
				  "../NV"
				  "../NVCtrls"
				  "../OS_FreeRTOS"	
				  "../DebugManager"
				  "../Lib"
				  "../Ethernet/Web"
				  "../ThirdParty/crypto_matixssl"
				  "../ThirdParty/crypto_matixssl/matrixssl"
				  "../ThirdParty/Azure_IOT/Azure_ltk_adapters"
				  "../ThirdParty/crypto_matixssl/matrixssl_ltk"
				  "../ThirdParty/crypto_matixssl/apps/ssl"
				  "../ThirdParty/crypto_matixssl/core"
				  "../ThirdParty/crypto_matixssl/core/include"
				  "../ThirdParty/crypto_matixssl/core/osdep/include"
				  "../ThirdParty/crypto_matixssl/crypto"
				  "../../../esp32_common/esp-idf/components/bootloader_support/include"
				  "../../../esp32_common/esp-idf/components/spi_flash/include"
				  "../../../esp32_common/esp-idf/components/freertos/include/freertos"
				  "../../../esp32_common/esp-idf/components/freertos/port/xtensa/include/freertos"
				  "../../../esp32_common/esp-idf/components/app_update/include"				  
    )
    
#Register source files and include directories
idf_component_register( SRCS "${srcs}"
                	    INCLUDE_DIRS "${include_dirs}"
					  )		  