#include "neo_blinky.h"


void neo_blinky(void *pvParameters){

    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();

    while(1) {                          
        switch (system_status)
        {
            case 0:
                // Chuẩn bị Setup: Tắt hẳn
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(200));
                break;

            case 1:
                // AP Mode: Đỏ chớp tắt chu kỳ 1Hz
                strip.setPixelColor(0, strip.Color(255, 0, 0));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(500));

                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(500));
                break;

            case 2:
                // Đã vào webserver ở AP: Đỏ sáng hẳn
                strip.setPixelColor(0, strip.Color(255, 0, 0));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(200));
                break;

            case 3:
                // STA Mode: Xanh dương chớp chậm
                strip.setPixelColor(0, strip.Color(0, 0, 255));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(500));
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(500));
                break;

            case 4:
                // Đã vào webserver ở STA: Xanh lá sáng hẳn
                strip.setPixelColor(0, strip.Color(0, 255, 0));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(200));
                break;

            case 5:
                // Đã có IP ở STA Mode, đợi người dùng truy cập Webserver: xanh dương sáng hẳn
                strip.setPixelColor(0, strip.Color(0, 0, 255));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(200));
                break;



            default:
                // Trạng thái lạ thì tắt
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
                vTaskDelay(pdMS_TO_TICKS(200));
                break;
        }
    }
}