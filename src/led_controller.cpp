#include "led_controller.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

LedController::LedController(int ledCount, int dataPin, int channel)
    : leds(LED_WS2812B, ledCount, dataPin, channel, DoubleBuffer), ledCount(ledCount), hue(0) {}

void LedController::delay(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void LedController::showColor(uint8_t hue) {
    for (int i = 0; i != ledCount; i++)
        leds[i] = Hsv { hue, 255, 10 };
    leds.show();
}

void LedController::showRgb() {
    leds[0] = Rgb { 255, 0, 0 };
    leds[1] = Rgb { 0, 255, 0 };
    leds[2] = Rgb { 0, 0, 255 };
    leds[3] = Rgb { 0, 0, 0 };
    leds[4] = Rgb { 255, 255, 255 };
    leds.show();
}

void LedController::colorTask() {
    while (true) {
        showColor(hue);
        delay(50);
    }
}

void LedController::hueControlTask() {
    while (true) {
        hue = (hue + 1) % 256;
        delay(50);
    }
}