#include "temp_humi_monitor.h"
DHT20 dht20;

void temp_humi_monitor(void *pvParameters){

    Wire.begin(11,12);
    Serial.begin(115200);
    dht20.begin();

    while (1){
     
        dht20.read();
        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();

        glob_temperature = temperature;
        glob_humidity = humidity;

        Serial.printf("Temp: %.2f °C  Humidity: %.2f %%\r\n", temperature, humidity);
        
        vTaskDelay(5000);
    }
    
}