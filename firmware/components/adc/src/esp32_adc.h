#ifndef ESP32_ADC_H
#define ESP32_ADC_H

#include <stdint.h>

#include "adc.h"

adc_status_t esp32_adc_init(void);

adc_status_t esp32_adc_read(
    uint16_t *value
);

#endif /* ESP32_ADC_H */