#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "onewire_bus.h"
#include "ds18b20.h"

#define GPIO_SONDA_DS18B20 27

static const char *TAG = "sensor_temperatura";

void app_main(void)
{
    onewire_bus_handle_t bus = NULL;
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = GPIO_SONDA_DS18B20,
        .flags = {
            .en_pull_up = false, // ja existe o resistor externo de 4,7 kOhm
        },
    };
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10, // 1 byte comando ROM + 8 bytes endereco ROM + 1 byte comando do dispositivo
    };
    ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &bus));
    ESP_LOGI(TAG, "Barramento 1-Wire instalado no GPIO%d", GPIO_SONDA_DS18B20);

    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t dispositivo_encontrado;
    ds18b20_device_handle_t sonda = NULL;
    bool sonda_encontrada = false;

    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
    ESP_LOGI(TAG, "Procurando dispositivos no barramento...");

    esp_err_t resultado_busca;
    do {
        resultado_busca = onewire_device_iter_get_next(iter, &dispositivo_encontrado);
        if (resultado_busca == ESP_OK) {
            ds18b20_config_t config_sonda = {};
            if (ds18b20_new_device_from_enumeration(&dispositivo_encontrado, &config_sonda, &sonda) == ESP_OK) {
                ESP_LOGI(TAG, "Sonda DS18B20 encontrada, endereco: %016llX", dispositivo_encontrado.address);
                sonda_encontrada = true;
                break;
            } else {
                ESP_LOGI(TAG, "Encontrado um dispositivo que nao e DS18B20, endereco: %016llX", dispositivo_encontrado.address);
            }
        }
    } while (resultado_busca != ESP_ERR_NOT_FOUND);

    ESP_ERROR_CHECK(onewire_del_device_iter(iter));

    if (!sonda_encontrada) {
        ESP_LOGE(TAG, "Nenhuma sonda DS18B20 encontrada no barramento!");
    }

    float temperatura;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (sonda_encontrada) {
            esp_err_t resultado_trigger = ds18b20_trigger_temperature_conversion_for_all(bus);
            if (resultado_trigger == ESP_OK) {
                esp_err_t resultado_leitura = ds18b20_get_temperature(sonda, &temperatura);
                if (resultado_leitura == ESP_OK) {
                    ESP_LOGI(TAG, "Temperatura lida: %.2f C", temperatura);
                } else {
                    ESP_LOGE(TAG, "Falha ao ler a temperatura");
                }
            } else {
                ESP_LOGE(TAG, "Falha ao disparar a conversao de temperatura");
            }
        }
    }
}
