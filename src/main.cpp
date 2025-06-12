//================================================================================================
/// @file main.cpp
/// Author: Amelia Wietting
/// Description: Example code for the ESP32-EVB by Olimex
/// to flash LEDs and board with semi-functioning ESP-NOW functionality
/// @brief Defines `main` for the Blink example with ESP-NOW communication///
 
// Various headers for common libraries
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

// These are libraries included with the esp-idf
#include "esp_system.h"
#include "esp_log.h"
#include "esp_random.h"

// NOTE: This DRIVER class of gpio.h is bringing in the OLIMEX_* named variables below
#include "driver/gpio.h"

// Only needed if using wifi functionality
#include "esp_wifi_types.h" // Include for MAC2STR and MACSTR

// Only needed if using espnow functionality
#include "esp_now_broadcast.h"

// Used for the UART menu
#include <string.h>
#include "driver/uart.h"

// needed for common variables and functions to be stored
#include "common.h"
#include "shared.h"

// Olimex perhiperal config
#define OLIMEX_REL1_PIN GPIO_NUM_32
#define OLIMEX_REL2_PIN GPIO_NUM_33
#define OLIMEX_BUT_PIN GPIO_NUM_34

// You can run menuconfig to change relay
// Or you can set build flags and change this code
// There are a LOT of ways to set these flags, coder's choice
#if defined CONFIG_RELAY_1
    #define RELAY_GPIO OLIMEX_REL1_PIN
#else
    #define RELAY_GPIO OLIMEX_REL2_PIN
#endif

#define BUF_SIZE 256

static const char *UART_LOG = "UART_MENU";

/* #### ESPNOW Configuration ####*/
/*###############################*/

// ESPNOW: Generate a random device ID at compile time, only used for espnow functionality
#define RANDOM_DEVICE_ID __COUNTER__

// ESPNOW: Configure variables for when events in the espnow timer trigger an incoming message
bool message_received = false;
uint32_t message_received_time = 0;

// ESPNOW: LED flash duration in ms when getting incoming message from espnow
#define MESSAGE_FLASH_DURATION 1000


/* #### LED Configuration ####*/
/*############################*/
// LEDs: Set up the LED controller
SmartLed leds(LED_WS2812B, LED_COUNT, DATA_PIN, CHANNEL, DoubleBuffer);

// LEDs: Set up a global to control a background process updating hue and brightness
volatile uint8_t hue = 0;
uint8_t brightness = 1;


/* #### Helper functions ####*/
/*###########################*/
// Define a delay function
void delay(int ms) { 
    vTaskDelay(pdMS_TO_TICKS(ms)); 
}

// Since we don't have an RTC we need to get the rough time based on CPU 'ticks'
uint32_t millis() { 
    return xTaskGetTickCount() * portTICK_PERIOD_MS; 
}

// ESPNOW: Set up callback for when an ESPNOW broadcast message is received
void message_handler(const broadcast_message_t *msg, const uint8_t *mac_addr) {
    char mac_str[18];
    sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", 
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    
    printf("Received message from device %d (MAC: %s)\n", msg->device_id, mac_str);
    
    switch (msg->type) {
        case MSG_TYPE_BUTTON_PRESSED:
            // Set flag to show red LED flash
            message_received = true;
            message_received_time = millis();
            
            // Toggle relay for visual/physical feedback
            static bool relay_state = false;
            relay_state = !relay_state;
            gpio_set_level(RELAY_GPIO, relay_state);
            
            printf("Button pressed on device %d\n", msg->device_id);
            break;
            
        default:
            printf("Unknown message type %d from device %d\n", msg->type, msg->device_id);
    }
}

// LEDs: Update the array of LEDs and trigger a refresh of the LED panel
static void showColor() {

    // Normal color display
    message_received = false; // Reset flag after flash duration
    for (int i = 0; i != LED_COUNT; i++) {
        leds[i] = Hsv { hue, 255, brightness };
    }
    
    leds.show();
}

// LEDs: Refresh displayed LEDs every 50ms
void colorTask(void *pvParameters) {
    while (true) {
        showColor();
        delay(50);
    }
}

// LEDs: Cycle the hue of the displayed LEDs every 50ms
void hueControlTask(void *pvParameters) {
    while (true) {
        // Increment hue and wrap around at 255
        hue = (hue + 1) % 256;
        delay(50);  // Adjust delay as needed for hue change rate
    }
}

// Olimex Perhiperal Controller to determine if a button was pressed
// and react to it
void button_task(void* arg)
{
    while (1)
    {
        if (gpio_get_level(OLIMEX_BUT_PIN) == 0)
        {
            // Debounce first
            delay(50);
            
            // Check if button is still pressed after debounce
            if (gpio_get_level(OLIMEX_BUT_PIN) == 0) {
                gpio_set_level(RELAY_GPIO, 1);
                printf("Button pressed!\n");
                
                // Set the brightness
                //brightness = 100;
                
                // Wait for button release with timeout
                int timeout = 0;
                while (gpio_get_level(OLIMEX_BUT_PIN) == 0 && timeout < 100000) {
                    delay(10);
                    timeout++;
                }
                
                // Drop the brightness
                // brightness = 1; 

                // Turn off the relay
                gpio_set_level(RELAY_GPIO, 0);
                delay(10); // Additional debounce
            }
        }
        delay(20);
    }
}

// UART: Controlling the device over UART
void process_command(const char *cmd) {

    if(strcmp(cmd, "1") == 0){
        ESP_LOGI(UART_LOG, "\nOption 1 selected");
        gpio_set_level(RELAY_GPIO, 1);
        delay(10);

        brightness = 100;
        volatile uint8_t hue2 = 100;
        for (int i = 0; i != LED_COUNT; i++) {
            leds[i] = Rgb { hue2, 255, brightness }; 
        }
        showColor();
        delay(50);
        
        brightness = 1;
       
        gpio_set_level(RELAY_GPIO, 0);
        delay(50);
        showColor();
        delay(10);
        
    }else if(strcmp(cmd, "2") == 0){
        ESP_LOGI(UART_LOG, "\nOption 2 selected");
    }else if(strcmp(cmd, "3") == 0){
        ESP_LOGI(UART_LOG, "\nOption 3 selected");
    }else if(strcmp(cmd, "h") == 0){
        printf("\nMenu:\n1 - Option 1\n2 - Option 2\n3 - Option 3\nh - Show this menu");
    }else{
        printf("\nMenu:\n1 - Option 1\n2 - Option 2\n3 - Option 3\nh - Show this menu");
        //ESP_LOGW(UART_LOG, "Unknown command: %s", cmd);
    }
}

// Set up everything that is needed before we kick-off the main loop
void setup() {
    // Reset GPIO to enable them to control the relay and listen to the button
    gpio_reset_pin(RELAY_GPIO);
    gpio_reset_pin(OLIMEX_BUT_PIN);
    
    // Set the Relay as an output
    gpio_set_direction(RELAY_GPIO, GPIO_MODE_OUTPUT);
    
    // Set the button as input
    gpio_set_direction(OLIMEX_BUT_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(OLIMEX_BUT_PIN);  // Add pull-up
    
    // Initialize NVS flash
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // ESPNOW: Get a random device ID
    uint8_t device_id = RANDOM_DEVICE_ID;
    printf("Device initialized with ID: %d\n", device_id);
    
    /* ####  BACKGROUND TASK KICKOFF #### */
    /* NOTE: EVERYTHING IS RUNNING IN BACKGROUND TASKS */
    /* To change behavior of these functions you must update the callbacks*/
    /*###############################*/

    
    // BUTTON: Create button task to monitor and respond to button presses as a background task
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    // LEDs: Create the color update task
    //xTaskCreate(colorTask, "Color Task", 2048, NULL, 11, NULL);
    
    // LEDs: Create the hue control task
    xTaskCreate(hueControlTask, "Hue Control Task", 2048, NULL, 11, NULL);
}



extern "C" void app_main() {
    setup();

    // UART: Prepare for UART reading
    char line[BUF_SIZE];

    while (true) {
        // Check for input on the command line
        if(fgets(line, sizeof(line), stdin)) {
            process_command(line);
        }
        delay(50);
    }
}