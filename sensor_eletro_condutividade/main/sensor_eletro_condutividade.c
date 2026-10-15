#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


void app_main(void){

    adc_oneshot_unit_handle_t adc1_handle;

    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config);

    while(1){

        int media_leitura = 0;

        for (int i = 0; i < 10; i++) {

            int leitura;
            adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &leitura);

            media_leitura = media_leitura + leitura;
        }

        printf("A leitura e de %d \n", media_leitura / 10);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
