// void app_main() {}



#include <stdio.h>
#include "freertos/FreeRTOS.h" // For vTaskDelay
#include "freertos/task.h"     // For tasks
#include "driver/gpio.h"       // For GPIO functions
#include "esp_log.h"           // For logging

#define BLINK_GPIO GPIO_NUM_8 // Example: Use GPIO 2 for built-in LED
#define DELAY_MS 500

void app_main(void) {
    // Configure GPIO pin as output
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    ESP_LOGI("BLINK", "Starting LED Blink Task on GPIO %d", BLINK_GPIO);

    while (1) {

        ESP_LOGI("BLINK", "LED ON");

        gpio_set_level(BLINK_GPIO, 1); // Turn LED ON
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Wait for 1 second

        ESP_LOGI("BLINK", "LED OFF");

        gpio_set_level(BLINK_GPIO, 0); // Turn LED OFF
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Wait for 1 second
    }
}