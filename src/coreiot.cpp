#include "coreiot.h"
#include "global.h"

// ===================== CoreIoT =====================
constexpr char TOKEN[] = "gya9l6p6azscc0zjn9cd";
constexpr char THINGSBOARD_SERVER[] = "app.coreiot.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t TELEMETRY_INTERVAL = 10000U;  // 10 giây

WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

// =======================================================
// Kết nối CoreIoT / ThingsBoard
// =======================================================
bool connectThingsBoard() {
  if (tb.connected()) {
    return true;
  }

  if (WiFi.status() != WL_CONNECTED) {
    coreiot_status = 1;
    return false;
  }

  coreiot_status = 1;
  Serial.println("Connecting to CoreIoT...");

  if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
    Serial.println("Connect CoreIoT failed");
    coreiot_status = 1;
    return false;
  }

  Serial.println("CoreIoT connected");
  return true;
}

// =======================================================
// Gửi dữ liệu
// =======================================================
bool sendTelemetry() {
  float temperature = glob_temperature;
  float humidity = glob_humidity;

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Read sensor failed");
    coreiot_status = 1;
    return false;
  }

  bool sentTemperature = tb.sendTelemetryData("temperature", temperature);
  bool sentHumidity = tb.sendTelemetryData("humidity", humidity);

  if (sentTemperature && sentHumidity) {
    Serial.println("Telemetry sent");
    coreiot_status = 2;
    return true;
  } else {
    Serial.println("Telemetry send failed");
    coreiot_status = 1;
    return false;
  }
}

// =======================================================
// Task chính
// =======================================================
void coreiot_task(void *pvParameters) {
  uint32_t lastSendTime = 0;

  while (true) {
    // Chưa có WiFi thì chờ task khác xử lý WiFi
    if (WiFi.status() != WL_CONNECTED) {
      coreiot_status = 1;

      if (tb.connected()) {
        tb.disconnect();
      }

      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    // Có WiFi rồi thì kết nối CoreIoT
    if (!connectThingsBoard()) {
      coreiot_status = 1;
      vTaskDelay(pdMS_TO_TICKS(2000));
      continue;
    }

    // Gửi dữ liệu định kỳ
    if (millis() - lastSendTime >= TELEMETRY_INTERVAL) {
      lastSendTime = millis();

      if (!sendTelemetry()) {
        coreiot_status = 1;
      }
    }

    tb.loop();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}