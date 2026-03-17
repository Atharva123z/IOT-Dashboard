#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// -------- WiFi Credentials --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase REST --------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- BMP180 --------
Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(115200);

  // Start I2C (ESP32 default I2C pins)
  Wire.begin(21, 22);

  // Init BMP180
  if (!bmp.begin()) {
    Serial.println("❌ BMP180 not found!");
    while (1);
  }
  Serial.println("✅ BMP180 initialized");

  // WiFi connect
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // 🔗 Clickable dashboard link
  Serial.println("================================");
  Serial.println("OPEN PRESSURE SENSOR DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/pressure");
  Serial.println("================================");
}

void loop() {
  float pressure = bmp.readPressure() / 100.0; // hPa
  float temperature = bmp.readTemperature();   // °C

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.print(" hPa | Temp: ");
  Serial.print(temperature);
  Serial.println(" °C");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Send pressure
    String urlPressure = String(FIREBASE_HOST) +
      "/sensors/pressure/pressure.json?auth=" +
      FIREBASE_SECRET;

    http.begin(urlPressure);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(pressure));
    http.end();

    // Send temperature
    String urlTemp = String(FIREBASE_HOST) +
      "/sensors/pressure/temperature.json?auth=" +
      FIREBASE_SECRET;

    http.begin(urlTemp);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(temperature));
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(2000); // update every 2 seconds
}
