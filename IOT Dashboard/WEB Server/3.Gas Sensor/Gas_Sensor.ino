#include <WiFi.h>
#include <HTTPClient.h>

// -------- WiFi Credentials --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase REST --------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- MQ-3 Gas Sensor --------
#define MQ3_PIN 34   // Analog pin (ADC)

// Read MQ-3 value
int getGasValue() {
  return analogRead(MQ3_PIN); // 0–4095
}

void setup() {
  Serial.begin(115200);

  // MQ-3 warm-up (important)
  Serial.println("Warming up MQ-3 sensor...");
  delay(10000);
  Serial.println("MQ-3 Sensor Ready");

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
  Serial.println("OPEN GAS SENSOR DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/gas");
  Serial.println("================================");
}

void loop() {
  int gasValue = getGasValue();

  Serial.print("Gas Sensor Value: ");
  Serial.println(gasValue);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Standardized database path
    String url = String(FIREBASE_HOST) +
      "/sensors/gas/value.json?auth=" +
      FIREBASE_SECRET;

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(String(gasValue));

    Serial.print("Firebase HTTP code: ");
    Serial.println(httpCode);

    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(2000); // update every 2 seconds
}
