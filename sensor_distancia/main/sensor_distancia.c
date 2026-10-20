#include <stdio.h>
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main(void){

    gpio_config_t config_trig = {
        .pin_bit_mask = (1ULL << 25),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&config_trig);

    gpio_config_t config_echo = {
        .pin_bit_mask = (1ULL << 26),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&config_echo);



    while(1){

        int falha_sensor = 0;
        float duracao, distancia_cm, distancia_m, distancia_final, leituras[5];

        for (int i = 0; i < 5; i++) {

            gpio_set_level(25, 1);
            esp_rom_delay_us(10);
            gpio_set_level(25, 0);

            int64_t inicio_subida = esp_timer_get_time();
            while (gpio_get_level(26) == 0) {
                if (esp_timer_get_time() - inicio_subida > 30000) {// sensor não respondeu a tempo
                    falha_sensor = 1;
                    break;
                }
            }
            int64_t tempo_subida = esp_timer_get_time();

            int64_t inicio_descida = esp_timer_get_time();
            while (gpio_get_level(26) == 1) {
                if (esp_timer_get_time() - inicio_descida > 30000) {// sensor não respondeu a tempo
                    falha_sensor = 1;
                    break;
                }
            }
            int64_t tempo_descida = esp_timer_get_time();

            duracao = tempo_descida - tempo_subida;
            distancia_cm = duracao / 57.3;
            distancia_m = distancia_cm / 100.0;

            leituras[i] = distancia_m;

            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        if (falha_sensor == 0){
                for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    if(leituras[j] > leituras[j + 1]){
                        float temp = leituras[j];
                        leituras[j] = leituras[j + 1];
                        leituras[j + 1] = temp;
                    }
                }
            }
            distancia_final = leituras[2];

            printf("A distancia e %.3f metros\n", distancia_final);

            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        else{
            printf("A distancia e null\n");

            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}
