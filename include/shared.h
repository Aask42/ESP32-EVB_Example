// shared.h
#ifndef SHARED_H
#define SHARED_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern volatile int new_message_received;
extern char received_data[256];
extern char received_topic[256];

void trigger_effect();

#endif // SHARED_H
