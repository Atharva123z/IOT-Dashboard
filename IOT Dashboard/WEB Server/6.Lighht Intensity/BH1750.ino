#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <BH1750.h>

// -------- WiFi Credentials --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase REST Config --------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- BH1750 --------
BH1750 lightMeter(0x23);

void setup() {
  Serial.begin(115200);

  // Start I2C
  Wire.begin(21, 22);

  // Init BH1750
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("❌ BH1750 not found!");
    while (1);
  }
  Serial.println("✅ BH1750 initialized");

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

  // 🔗 CLICKABLE WEBSITE LINK
  Serial.println("================================");
  Serial.println("OPEN LIGHT SENSOR DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/light");
  Serial.println("================================");
}

void loop() {
  float lux = lightMeter.readLightLevel();

  Serial.print("Light Intensity: ");
  Serial.print(lux);
  Serial.println(" lx");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Firebase REST endpoint
    String url = String(FIREBASE_HOST) +
      "/sensors/light/lux.json?auth=" + FIREBASE_SECRET;

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(String(lux));

    Serial.print("Firebase HTTP code: ");
    Serial.println(httpCode);

    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(2000);
}
