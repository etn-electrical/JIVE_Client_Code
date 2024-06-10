set(srcs "eeram_comp.c"
  "i2c_comp.c"
    )

set (include_dirs "."
                  "../../esp32_common/esp-idf/components/nvs_flash/include"
    
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "${include_dirs}"
                    )

                    #Set include directories

                   