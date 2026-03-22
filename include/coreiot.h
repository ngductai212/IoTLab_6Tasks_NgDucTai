#ifndef __COREIOT_H__
#define __COREIOT_H__

#include <Arduino.h>
#include <WiFi.h>
#include "global.h"

#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>


void coreiot_task(void *pvParameters);

#endif