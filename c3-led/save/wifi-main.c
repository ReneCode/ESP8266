// void app_main() {}



#include <stdio.h>
#include "freertos/FreeRTOS.h" // For vTaskDelay
#include "freertos/task.h"     // For tasks
#include "driver/gpio.h"       // For GPIO functions
#include "esp_log.h"           // For logging

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"


#define WIFI_SSID      "K-3000"
#define WIFI_PASS      "1235808024535755"

// #define WIFI_SSID "your_ssid"
// #define WIFI_PASS "your_password"
#define WIFI_MAX_RETRY 5

#define BLINK_GPIO GPIO_NUM_8 // Example: Use GPIO 2 for built-in LED
#define DELAY_MS 1000

static const char *TAG = "wifi_connect";
static int s_retry_num = 0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{




    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {


    wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *) event_data;
    if (event) {

        ESP_LOGI(TAG, "disconnected %d", event->reason);
    }



        if (s_retry_num < WIFI_MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            ESP_LOGI(TAG,"connect to the AP fail");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        // ESP_LOGI(TAG, "got ip: %s", ip4addr_ntoa(&event->ip_info.ip));
        ESP_LOGI(TAG, "got ip :-) ");
        s_retry_num = 0;
    }
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                       ESP_EVENT_ANY_ID,
                                                       &wifi_event_handler,
                                                       NULL,
                                                       &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                       IP_EVENT_STA_GOT_IP,
                                                       &wifi_event_handler,
                                                       NULL,
                                                       &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            // .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            // .threshold.authmode = WIFI_AUTH_WPA_PSK
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // Configure GPIO pin as output
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    ESP_LOGI("BLINK", "Starting LED Blink Task on GPIO %d", BLINK_GPIO);

    while (1) {
        // ESP_LOGI("BLINK", "LED ON");
        gpio_set_level(BLINK_GPIO, 1); // Turn LED ON
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Wait for 1 second
        // ESP_LOGI("BLINK", "LED OFF");
        gpio_set_level(BLINK_GPIO, 0); // Turn LED OFF
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Wait for 1 second
    }
}