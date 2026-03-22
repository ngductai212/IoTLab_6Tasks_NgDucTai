#include "led_blinky.h"

void led_blinky(void *pvParameters){
  pinMode(LED_GPIO, OUTPUT);
  
  while(1) {                        

      switch (coreiot_status)
        {
            case 1:
                // Chuẩn bị Setup: Tắt hẳn
                digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
                vTaskDelay(500);
                digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
                vTaskDelay(500);
                break;

            case 2:
                // Đã vào webserver ở AP: LED sáng hẳn
                digitalWrite(LED_GPIO, HIGH);
                vTaskDelay(pdMS_TO_TICKS(200));
                break;

            default:
                // Trạng thái lạ thì tắt
                digitalWrite(LED_GPIO, LOW);
                vTaskDelay(pdMS_TO_TICKS(200));
                break;
        }

  }
}


