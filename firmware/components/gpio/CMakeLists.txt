idf_component_register(
    SRCS
        "src/gpio.c"

    INCLUDE_DIRS
        "include"
		
	REQUIRES
        esp32_hal
)