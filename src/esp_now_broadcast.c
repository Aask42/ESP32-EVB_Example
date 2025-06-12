// esp_now_broadcast.c
#include "esp_now_broadcast.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_netif.h"

static const char* TAG = "ESP_NOW_BROADCAST";
static uint8_t BROADCAST_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static uint8_t device_id = 0;
static message_handler_t message_callback = NULL;

// Default send callback
static void send_callback(const uint8_t *mac_addr, esp_now_send_status_t status) {
    char mac_str[18];
    sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", 
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    
    printf("Broadcast to %s status: %s\n", mac_str, 
           status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Receive callback - processes incoming messages
static void recv_callback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (len < sizeof(broadcast_message_t)) {
        printf("Received message too short (%d bytes), expected at least %d bytes\n", 
               len, (int)sizeof(broadcast_message_t));
        return;
    }
    
    broadcast_message_t *msg = (broadcast_message_t *)data;
    
    // Skip messages from self
    if (msg->device_id == device_id) {
        return;
    }
    
    const uint8_t *mac_addr = recv_info->src_addr;
    char mac_str[18];
    sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", 
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    
    printf("Received message from %s, type: %d, device_id: %d\n", 
           mac_str, msg->type, msg->device_id);
    
    // Call user-provided handler
    if (message_callback) {
        message_callback(msg, mac_addr);
    }
}

esp_err_t esp_now_broadcast_init(uint8_t id, message_handler_t handler) {
    device_id = id;
    message_callback = handler;
    
    // Initialize WiFi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    
    // Set WiFi protocol for ESP-NOW
    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, 
               WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_LR));
    
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_disconnect());  // Ensure not connected to any AP
    
    // Initialize ESP-NOW
    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK) {
        printf("ESP-NOW init failed: %s\n", esp_err_to_name(ret));
        return ret;
    }
    
    // Register callbacks
    ESP_ERROR_CHECK(esp_now_register_send_cb(send_callback));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(recv_callback));
    
    // Add broadcast peer
    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, BROADCAST_MAC, ESP_NOW_ETH_ALEN);
    peer.channel = 1;  // Use a specific channel
    peer.ifidx = ESP_IF_WIFI_STA;
    peer.encrypt = false;
    
    // Add broadcast peer (ignore if already exists)
    ret = esp_now_add_peer(&peer);
    if (ret != ESP_OK && ret != ESP_ERR_ESPNOW_EXIST) {
        printf("Failed to add broadcast peer: %s\n", esp_err_to_name(ret));
        return ret;
    }
    
    // Print MAC address for debugging
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    printf("ESP-NOW initialized with MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", 
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
           
    printf("ESP-NOW broadcast initialized successfully, device ID: %d\n", device_id);
    return ESP_OK;
}

esp_err_t esp_now_broadcast_message(message_type_t type, const uint8_t *data, size_t data_len) {
    if (data_len > sizeof(((broadcast_message_t*)0)->data)) {
        printf("Data too large for broadcast message\n");
        return ESP_ERR_INVALID_ARG;
    }
    
    broadcast_message_t msg;
    msg.type = type;
    msg.device_id = device_id;
    memset(msg.data, 0, sizeof(msg.data));
    
    if (data && data_len > 0) {
        memcpy(msg.data, data, data_len);
    }
    
    printf("Sending broadcast message, type: %d, device_id: %d\n", msg.type, msg.device_id);
    
    esp_err_t ret = esp_now_send(BROADCAST_MAC, (const uint8_t*)&msg, sizeof(msg));
    if (ret != ESP_OK) {
        printf("Failed to send broadcast: %s\n", esp_err_to_name(ret));
    }
    
    return ret;
}

const uint8_t* get_broadcast_address() {
    return BROADCAST_MAC;
}