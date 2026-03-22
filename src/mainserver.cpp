#include "mainserver.h"
#include <WiFi.h>
#include <WebServer.h>


#define STATUS_IDLE_STARTUP   0
#define STATUS_AP_WAITING     1
#define STATUS_WEB_ACCESSED   2


bool isAPMode = true;

WebServer server(80);

unsigned long connect_start_ms = 0;
bool connecting = false;

String mainPage()
{
  float temperature = glob_temperature;
  float humidity = glob_humidity;
  float inference_result = glob_result;

  return R"rawliteral(
  <!DOCTYPE html>
  <html lang="vi">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Dashboard</title>
    <style>
      * {
        box-sizing: border-box;
      }

      body {
        font-family: "CMU Serif", Georgia, "Times New Roman", serif;
        background: #f8eeee;
        color: #333;
        margin: 0;
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        padding: 20px;
      }

      .container {
        background: linear-gradient(135deg, #7b1e2b, #a52a3a);
        padding: 30px 25px;
        border-radius: 20px;
        box-shadow: 0 4px 25px rgba(0,0,0,0.15);
        width: 100%;
        max-width: 420px;
        color: #fff;
        text-align: center;
      }

      h1 {
        font-size: 1.8em;
        margin-top: 0;
        margin-bottom: 10px;
      }

      .subtitle {
        font-size: 0.95em;
        opacity: 0.95;
        margin-bottom: 25px;
      }

      .sensor-card {
        background: rgba(255,255,255,0.15);
        border-radius: 16px;
        padding: 16px;
        margin-bottom: 15px;
        box-shadow: inset 0 1px 3px rgba(255,255,255,0.15);
      }

      .sensor-title {
        font-size: 1.5em;
        margin-bottom: 8px;
      }

      .sensor-value {
        font-size: 1.8em;
        font-weight: bold;
        color: #fff8e7
      }

      .sensor-unit {
        font-size: 0.9em;
        color: #eafcff;
      }

      .footer-note {
        margin-top: 18px;
        font-size: 0.9em;
        opacity: 0.9;
        font-style: italic;
        text-align: left;
        padding-left: 4px;
      }

      button {
        margin-top: 18px;
        background: #f2f3f5;
        color: #007bff;
        font-weight: bold;
        border: none;
        border-radius: 20px;
        padding: 11px 22px;
        cursor: pointer;
        transition: all 0.3s;
        font-size: 1em;
        font-family: "CMU Serif", Georgia, "Times New Roman", serif;
      }

      button:hover {
        background: #e6f3ff;
        transform: scale(1.05);
      }


    </style>
  </head>

  <body>
    <div class="container">
      <h1>📊 ESP32 Dashboard</h1>
      <div class="subtitle">Real-time Sensor Monitoring (DHT20)</div>

      <div class="sensor-card">
        <div class="sensor-title">🌡️ Temperature</div>
        <div class="sensor-value">
          <span id="temp">)rawliteral" + String(temperature, 2) + R"rawliteral(</span>
          <span class="sensor-unit">&deg;C</span>
        </div>
      </div>

      <div class="sensor-card">
        <div class="sensor-title">💧 Humidity</div>
        <div class="sensor-value">
          <span id="hum">)rawliteral" + String(humidity, 2) + R"rawliteral(</span>
          <span class="sensor-unit">%</span>
        </div>
      </div>


      <div class="sensor-card">
        <div class="sensor-title">🧠 Inference Result</div>
        <div class="sensor-value">
          <span id="hum">)rawliteral" + String(inference_result, 2) + R"rawliteral(</span>
        </div>
      </div>

      <div class="footer-note">🔄 Data updates automatically every 5 seconds</div>

      <button onclick="window.location='/settings'">⚙️ Settings</button>
    </div>

    <script>
      setInterval(() => {
        fetch('/sensors')
          .then(res => res.json())
          .then(d => {
            document.getElementById('temp').innerText = d.temp;
            document.getElementById('hum').innerText = d.hum;
          });
      }, 3000);
    </script>
  </body>
  </html>
  )rawliteral";
}

String settingsPage()
{
  return R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wi-Fi Settings</title>
    <style>
      * {
        box-sizing: border-box;
      }

      body {
        font-family: "CMU Serif", Georgia, "Times New Roman", serif;
        background: #fcf4f4;
        color: #333;
        margin: 0;
        min-height: 100vh;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        padding: 20px;
      }

      .container {
        background: linear-gradient(135deg, #7b1e2b, #a52a3a);
        padding: 30px 24px;
        border-radius: 20px;
        box-shadow: 0 4px 25px rgba(0,0,0,0.15);
        width: 100%;
        max-width: 420px;
        color: #fffaf0;
        text-align: left;
      }

      h1 {
        font-size: 1.8em;
        margin-top: 0;
        margin-bottom: 10px;
        text-align: center;
      }

      .subtitle {
        font-size: 0.95em;
        opacity: 0.95;
        margin-bottom: 22px;
        text-align: center;
        font-style: italic;
      }

      label {
        display: block;
        font-weight: 600;
        margin-bottom: 6px;
        margin-top: 12px;
      }

      input[type=text],
      input[type=password] {
        width: 100%;
        padding: 12px 14px;
        border: none;
        border-radius: 20px;
        font-size: 1em;
        margin-bottom: 6px;
        outline: none;
        font-family: "CMU Serif", Georgia, "Times New Roman", serif;
      }

      input::placeholder {
        color: #888;
      }

      .button-group {
        display: flex;
        gap: 12px;
        margin-top: 18px;
        justify-content: center;
      }

      button {
        border: none;
        border-radius: 24px;
        padding: 12px 20px;
        cursor: pointer;
        font-size: 1em;
        font-weight: bold;
        transition: all 0.3s;
        min-width: 120px;
        font-family: "CMU Serif", Georgia, "Times New Roman", serif;
      }

      .connect-btn {
        background: #fffaf0;
        color: #8b1e2d;
      }

      .connect-btn:hover {
        background: #f8ede0;
        transform: scale(1.04);
      }

      .back-btn {
        background: rgba(255,255,255,0.18);
        color: #fffaf0;
        border: 1px solid rgba(255,255,255,0.25);
      }

      .back-btn:hover {
        background: rgba(255,255,255,0.28);
        transform: scale(1.04);
      }

      #msg {
        margin-top: 18px;
        text-align: center;
        font-weight: 600;
        color: #fff8e7;
        min-height: 22px;
      }

    </style>
  </head>

  <body>
    <div class="container">
      <h1>⚙️ Wi-Fi Settings</h1>
      <div class="subtitle">Enter Wi-Fi credentials for ESP32</div>

      <form id="wifiForm">
        <label for="ssid">Wi-Fi SSID</label>
        <input name="ssid" id="ssid" type="text" placeholder="Enter Wi-Fi name" required>

        <label for="pass">Password</label>
        <input name="password" id="pass" type="password" placeholder="Leave blank if not required">

        <div class="button-group">
          <button type="submit" class="connect-btn">🔗 Connect</button>
          <button type="button" class="back-btn" onclick="window.location='/'">↩️ Back</button>
        </div>
      </form>

      <div id="msg"></div>
    </div>

    <script>
      document.getElementById('wifiForm').onsubmit = function(e) {
        e.preventDefault();

        let ssid = document.getElementById('ssid').value;
        let pass = document.getElementById('pass').value;

        fetch('/connect?ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass))
          .then(r => r.text())
          .then(msg => {
            document.getElementById('msg').innerText = msg;
          })
          .catch(() => {
            document.getElementById('msg').innerText = 'Connection request failed.';
          });
      };
    </script>
  </body>
  </html>
  )rawliteral";
}

// ========== Handlers ==========
void handleRoot() { 
  if (isAPMode)
    system_status = 2;   // Truy cập webserver ở AP
  else
    system_status = 4;   // Truy cập webserver ở STA 
  server.send(200, "text/html", mainPage()); }

void handleSensors()
{
  float t = glob_temperature;
  float h = glob_humidity;
  String json = "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
  server.send(200, "application/json", json);
}

void handleSettings() { 
  if (isAPMode)
    system_status = 2;   
  else
    system_status = 4;   
  server.send(200, "text/html", settingsPage()); }

void handleConnect()
{
  system_status = 3;
  wifi_ssid = server.arg("ssid");
  wifi_password = server.arg("pass");
  server.send(200, "text/plain", "Connecting....");
  isAPMode = false;
  connecting = true;
  connect_start_ms = millis();
  connectToWiFi();
}

// ========== WiFi ==========
void setupServer()
{
  server.on("/", HTTP_GET, handleRoot);
  server.on("/sensors", HTTP_GET, handleSensors);
  server.on("/settings", HTTP_GET, handleSettings);
  server.on("/connect", HTTP_GET, handleConnect);
  server.begin();
}

void startAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  isAPMode = true;
  connecting = false;

  system_status = 1; 
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  if (wifi_password.isEmpty())
  {
    WiFi.begin(wifi_ssid.c_str());
  }
  else
  {
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  }
  Serial.print("Connecting to: ");
  Serial.print(wifi_ssid.c_str());

  Serial.print(" __ Password: ");
  Serial.print(wifi_password.c_str());
}

// ========== Main task ==========
void main_server_task(void *pvParameters)
{
  pinMode(BOOT_PIN, INPUT_PULLUP);

  startAP();
  setupServer();

  while (1)
  {
    server.handleClient();

    // BOOT Button to switch to AP Mode
    if (digitalRead(BOOT_PIN) == LOW)
    {
      vTaskDelay(100);
      if (digitalRead(BOOT_PIN) == LOW)
      {
        if (!isAPMode)
        {
          startAP();
          setupServer();
          system_status = 1;
        }
      }
    }

    // STA Mode
    if (connecting)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print(" __ STA IP address: ");
        Serial.println(WiFi.localIP());
        isWifiConnected = true; // Internet access


        isAPMode = false;
        connecting = false;
        system_status = 5;
      }
      else if (millis() - connect_start_ms > 10000)
      { // timeout 10s
        Serial.println("WiFi connect failed! Back to AP.");
        startAP();
        setupServer();
        connecting = false;
        isWifiConnected = false;
      }
    }

    vTaskDelay(20); // avoid watchdog reset
  }
}