#include "shared.h"
#include "esp_log.h"

volatile int new_message_received = 0;
char received_data[256];
char received_topic[256];

static const char *TAG = "SHARED";

void trigger_effect() {
    // Implement your effect here
    ESP_LOGI(TAG, "Triggering effect based on received MQTT message:");
    ESP_LOGI(TAG, "Topic: %s", received_topic);
    ESP_LOGI(TAG, "Data: %s", received_data);
}
