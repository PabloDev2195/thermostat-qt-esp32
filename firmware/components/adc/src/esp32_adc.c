#include "esp32_adc.h"

#include "esp_adc/adc_oneshot.h"

#define ADC_CHANNEL    ADC_CHANNEL_6

static adc_oneshot_unit_handle_t adc_handle;

adc_status_t esp32_adc_init(void)
{
    adc_oneshot_unit_init_cfg_t config = {
        .unit_id = ADC_UNIT_1
    };

    esp_err_t err = adc_oneshot_new_unit(
        &config,
        &adc_handle
    );

    if (err != ESP_OK)
    {
        return ADC_STATUS_ERROR;
    }

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12
    };

    err = adc_oneshot_config_channel(
        adc_handle,
        ADC_CHANNEL,
        &channel_config
    );

    if (err != ESP_OK)
    {
        return ADC_STATUS_ERROR;
    }

    return ADC_STATUS_OK;
}

adc_status_t esp32_adc_read(
    uint16_t *value
)
{
    if (value == NULL)
    {
        return ADC_STATUS_INVALID_PARAM;
    }

    int raw_value;

    esp_err_t err = adc_oneshot_read(
        adc_handle,
        ADC_CHANNEL,
        &raw_value
    );

    if (err != ESP_OK)
    {
        return ADC_STATUS_ERROR;
    }

    *value = (uint16_t)raw_value;

    return ADC_STATUS_OK;
}