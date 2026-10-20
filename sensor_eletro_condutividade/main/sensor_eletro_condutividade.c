#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_adc/adc_cali.h"


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

    adc_cali_handle_t cali_handle;

    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .default_vref = 1100,
    };

    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle));


    while(1){

        int mediana_bruta, leitura_bruta, tensao_mv, leituras[10];

        for (int i = 0; i < 10; i++) {

            adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &leitura_bruta);

            leituras[i] = leitura_bruta;
        }

        for(int i = 0; i < 9; i++){
            for(int j = 0; j < 9; j++){
                if(leituras[j] > leituras[j + 1]){
                    int temp = leituras[j];
                    leituras[j] = leituras[j + 1];
                    leituras[j + 1] = temp;
                }
            }
        }

        mediana_bruta = (leituras[4] + leituras[5]) / 2;

    

        adc_cali_raw_to_voltage(cali_handle, mediana_bruta, &tensao_mv);

        printf("Bruto mediana: %d | Tensao calibrada: %d mV\n", mediana_bruta, tensao_mv);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
