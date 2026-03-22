#include "global.h"
int global_var = 0;
float glob_temperature;
float glob_humidity;
float glob_result;

String ssid = "YOUR ESP32";
String password = "12345678";
String wifi_ssid = "OPPO A95";
String wifi_password = "12345678";
boolean isWifiConnected = false;

volatile int system_status = 0;
volatile int coreiot_status = 0;

SemaphoreHandle_t sensorMutex = NULL;