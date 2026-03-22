#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>

extern int glob_var;
extern float glob_temperature;
extern float glob_humidity;
extern float glob_result;

extern String wifi_ssid;
extern String wifi_password;

extern String ssid;
extern String password;

extern bool isWifiConnected;

extern volatile int system_status;
extern volatile int coreiot_status ;

extern SemaphoreHandle_t sensorMutex;

#endif