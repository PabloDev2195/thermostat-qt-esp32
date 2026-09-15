idf_component_register(
    SRCS
        "src/adc.c"

    INCLUDE_DIRS
        "include"
		
	REQUIRES
        esp32_hal
)