#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main(void)
{
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    // definindo o gpio4 como saida

    while(1){
        gpio_set_level(GPIO_NUM_4, 1);
        // level 1 (alto/ligado) manda energia para o pino do gpio4

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // coloca um delay x(1000) em milisegundos

        gpio_set_level(GPIO_NUM_4, 0);
        // level 0 (baixo/desligado) desliga o gpio4

        vTaskDelay(1000 / portTICK_PERIOD_MS);


    }

}
