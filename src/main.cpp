#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_UNIT        ADC_UNIT_1
#define ADC_CHANNEL     ADC_CHANNEL_2   // GPIO 3
#define ADC_ATTEN_CFG   ADC_ATTEN_DB_12
#define SAMPLE_FREQ_HZ  83333           // max for ESP32-S3

// 256 samples per DMA frame (each result is 4 bytes on ESP32-S3)
#define SAMPLES_PER_FRAME   256
#define FRAME_SIZE          (SAMPLES_PER_FRAME * SOC_ADC_DIGI_RESULT_BYTES)

static adc_continuous_handle_t adc_handle;
static adc_cali_handle_t       cali_handle;

static void adc_init(void) {
    adc_continuous_handle_cfg_t handle_cfg = {
        .max_store_buf_size = FRAME_SIZE * 4,
        .conv_frame_size    = FRAME_SIZE,
        .flags              = {},
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&handle_cfg, &adc_handle));

    adc_digi_pattern_config_t pattern = {
        .atten     = ADC_ATTEN_DB_12,
        .channel   = ADC_CHANNEL_2,
        .unit      = ADC_UNIT_1,
        .bit_width = ADC_BITWIDTH_12,
    };

    adc_continuous_config_t cfg = {
        .pattern_num    = 1,
        .adc_pattern    = &pattern,
        .sample_freq_hz = SAMPLE_FREQ_HZ,
        .conv_mode      = ADC_CONV_SINGLE_UNIT_1,
        .format         = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
    };
    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &cfg));

    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id  = ADC_UNIT,
        .chan     = ADC_CHANNEL,
        .atten   = ADC_ATTEN_CFG,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_cfg, &cali_handle));

    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
}

extern "C" void app_main(void) {
    uart_set_baudrate(UART_NUM_0, 1000000);
    adc_init();

    printf("ADC (mV)\n");

    static uint8_t  buf[FRAME_SIZE];
    static uint32_t acc   = 0;
    static uint32_t count = 0;

    while (1) {
        uint32_t out_len = 0;
        esp_err_t ret = adc_continuous_read(adc_handle, buf, FRAME_SIZE, &out_len, portMAX_DELAY);
        if (ret != ESP_OK) continue;

        for (uint32_t i = 0; i < out_len; i += SOC_ADC_DIGI_RESULT_BYTES) {
            adc_digi_output_data_t *d = (adc_digi_output_data_t *)&buf[i];
            int mv = 0;
            adc_cali_raw_to_voltage(cali_handle, d->type2.data, &mv);
            acc += (uint32_t)mv;
            if (++count == 5) {
                printf("%lu\n", acc / 5);
                acc   = 0;
                count = 0;
            }
        }
    }
}
