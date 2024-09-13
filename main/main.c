#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_cali.h"

void app_main(void)
{
    // 1. config adc_oneshot variables and create handle
    int adc_read0;
    int mv_output;

    adc_oneshot_unit_handle_t handle = NULL;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));

    // 2. config adc channel
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_0, &config));
    
    // 3. calibrate adc
    adc_cali_handle_t cali_handle = NULL;
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,   // ใช้ . ข้างหน้าเพื่อระบุฟิลด์ในโครงสร้าง
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle));  // ใช้ฟังก์ชันนี้แทน

    // 4. main loop 
    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_CHANNEL_0, &adc_read0));  // แก้ไขการส่งพารามิเตอร์ให้ถูกต้อง
        printf("Adc channel-0 raw read result: %d \n", adc_read0);

        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_read0, &mv_output));
        printf("ADC milivolt output: %d mV\n", mv_output);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    } 
}
