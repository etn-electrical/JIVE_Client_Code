set(srcs "spi_comp.c"
    "metro.c"
    "metrology_hal.c"
    "interrupt_comp.c"
    "metrology.c"
    "metro_utilities.c"
    )

set (include_dirs "."
                  "../../esp32_common/esp-idf/components/nvs_flash/include"
    
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "${include_dirs}"
                    )

                    #Set include directories

                   