idf_component_register(
    SRCS
        "src/fan.c"

    INCLUDE_DIRS
        "include"
		
	REQUIRES
        pwm
)