#include <WiFi.h>
#include <HTTPClient.h>

// -------- WiFi Credentials --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase REST --------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- Soil Sensor --------
#define SOIL_PIN 34   // ADC pin

// Read soil moisture
int getSoilValue() {
  return analogRead(SOIL_PIN); // 0–4095
}

void setup() {
  Serial.begin(115200);

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
  Serial.println("OPEN SOIL SENSOR DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/soil");
  Serial.println("================================");
}

void loop() {
  int soilValue = getSoilValue();

  Serial.print("Soil Moisture Value: ");
  Serial.println(soilValue);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Correct standardized path
    String url = String(FIREBASE_HOST) +
      "/sensors/soil/moisture.json?auth=" +
      FIREBASE_SECRET;

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(String(soilValue));

    Serial.print("Firebase HTTP code: ");
    Serial.println(httpCode);

    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(2000); // update every 2 seconds
}
