#ifndef COMMON_H
#define COMMON_H

#include <SmartLeds.h>
#include "led_controller.h"

// LED Config
const int LED_COUNT = 64;
const int DATA_PIN = 2;
const int CHANNEL = 0;

// SmartLed -> RMT driver (WS2812/WS2812B/SK6812/WS2813)
extern SmartLed leds;


#endif // COMMON_H
