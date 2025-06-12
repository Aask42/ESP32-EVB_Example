// esp_now_broadcast.h
#pragma once

#include <stdint.h>
#include <string.h>
#include "esp_now.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Message types
typedef enum {
    MSG_TYPE_BUTTON_PRESSED = 1,
    // Add more message types as needed
} message_type_t;

// Message structure
typedef struct {
    message_type_t type;
    uint8_t device_id;
    uint8_t data[16]; // Payload data
} broadcast_message_t;

// Message handler callback type
typedef void (*message_handler_t)(const broadcast_message_t *msg, const uint8_t *mac_addr);

/**
 * Initialize ESP-NOW for broadcast communication
 * @param id Device ID to use in messages
 * @param handler Callback function for received messages
 * @return ESP_OK on success
 */
esp_err_t esp_now_broadcast_init(uint8_t id, message_handler_t handler);

/**
 * Send a broadcast message
 * @param type Message type
 * @param data Optional payload data
 * @param data_len Length of payload data
 * @return ESP_OK on success
 */
esp_err_t esp_now_broadcast_message(message_type_t type, const uint8_t *data, size_t data_len);

/**
 * Get the broadcast MAC address
 * @return Pointer to the broadcast MAC address
 */
const uint8_t* get_broadcast_address();

#ifdef __cplusplus
}
#endif