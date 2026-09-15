idf_component_register(
    SRCS
        "src/adc.c"
		"src/esp32_adc.c"

    INCLUDE_DIRS
        "include"
		
	REQUIRES
        esp_adc
)