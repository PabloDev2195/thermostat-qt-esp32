#include "esp32_adc.h"

#include "esp_adc/adc_oneshot.h"

/**
 * @brief ADC channel used to read the analog input.
 *
 * ADC1 Channel 6 corresponds to GPIO34 on the ESP32.
 */
#define ADC_CHANNEL    ADC_CHANNEL_6

/**
 * @brief Handle for the ADC oneshot unit.
 *
 * This handle is created during ADC initialization and is used
 * for subsequent ADC read operations.
 */
static adc_oneshot_unit_handle_t adc_handle;

/**
 * @brief Initialize the ESP32 ADC peripheral.
 *
 * Creates an ADC oneshot unit using ADC Unit 1 and configures
 * the selected ADC channel with the default ADC bit width and
 * 12 dB attenuation.
 *
 * @return
 * - ADC_STATUS_OK if the ADC was initialized successfully.
 * - ADC_STATUS_ERROR if the ADC unit or channel configuration failed.
 */
adc_status_t esp32_adc_init(void)
{
    adc_status_t status = ADC_STATUS_OK;

    adc_oneshot_unit_init_cfg_t config = 
    {
        .unit_id = ADC_UNIT_1
    };

    esp_err_t err = adc_oneshot_new_unit(&config, &adc_handle);

    if (err != ESP_OK)
    {
        status = ADC_STATUS_ERROR;
    }
    else
    {
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
            status = ADC_STATUS_ERROR;
        }
    }

    return status;
}

/**
 * @brief Read the raw ADC value from the configured channel.
 *
 * Performs a single ADC conversion using the ESP32 ADC oneshot
 * driver and stores the resulting raw value in the provided
 * output variable.
 *
 * @param[out] value Pointer where the raw ADC value will be stored.
 *
 * @return
 * - ADC_STATUS_OK if the conversion was successful.
 * - ADC_STATUS_INVALID_PARAM if @p value is NULL.
 * - ADC_STATUS_ERROR if the ADC conversion failed.
 */
adc_status_t esp32_adc_read(uint16_t *value)
{
    adc_status_t status = ADC_STATUS_OK;

    if (value == NULL)
    {
        status = ADC_STATUS_INVALID_PARAM;
    }
    else
    {
        int raw_value;
        esp_err_t err = adc_oneshot_read(adc_handle, ADC_CHANNEL, &raw_value);
        if (err != ESP_OK)
        {
            status = ADC_STATUS_ERROR;
        }
        else
        {
            *value = (uint16_t)raw_value;
        }
    }

    return status;
}