idf_component_register(
    SRCS
        "src/temperature.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        adc
        bt_mgr
)