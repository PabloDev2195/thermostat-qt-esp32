idf_component_register(
    SRCS
	"src/esp32_adc.c"
    "src/esp32_gatt_svr.c"
    "src/esp32_pwm.c"
    "src/esp32_gpio.c"

    INCLUDE_DIRS
        "include"
		
	REQUIRES
        esp_adc
        nimble_peripheral_utils
        nvs_flash
        bt
        esp_driver_ledc
	    esp_driver_gpio
)