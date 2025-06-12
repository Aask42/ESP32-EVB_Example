#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <SmartLeds.h>

class LedController {
public:
    LedController(int ledCount, int dataPin, int channel);
    void showColor(uint8_t hue);
    void showRgb();
    void colorTask();
    void hueControlTask();

private:
    SmartLed leds;
    int ledCount;
    volatile uint8_t hue;
    void delay(int ms);
};

#endif // LED_CONTROLLER_H
