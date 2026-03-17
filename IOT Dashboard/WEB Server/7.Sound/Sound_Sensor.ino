#include <WiFi.h>
#include <HTTPClient.h>

// -------- WiFi Credentials --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase REST --------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- Sound Sensor Pins --------
#define SOUND_DIGITAL_PIN 15
#define SOUND_ANALOG_PIN 34

void setup() {
  Serial.begin(115200);

  pinMode(SOUND_DIGITAL_PIN, INPUT);

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
  Serial.println("OPEN SOUND SENSOR DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/sound");
  Serial.println("================================");
}

void loop() {
  int digitalValue = digitalRead(SOUND_DIGITAL_PIN); // LOW = sound detected
  int analogValue  = analogRead(SOUND_ANALOG_PIN);  // 0–4095

  int soundLevel = map(analogValue, 100, 3000, 0, 100);
  soundLevel = constrain(soundLevel, 0, 100);

  Serial.print("Digital: ");
  Serial.print(digitalValue);
  Serial.print(" | Analog: ");
  Serial.print(analogValue);
  Serial.print(" | Level: ");
  Serial.println(soundLevel);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Digital
    String urlDigital = String(FIREBASE_HOST) +
      "/sensors/sound/digital.json?auth=" +
      FIREBASE_SECRET;

    http.begin(urlDigital);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(digitalValue));
    http.end();

    // Analog
    String urlAnalog = String(FIREBASE_HOST) +
      "/sensors/sound/analog.json?auth=" +
      FIREBASE_SECRET;

    http.begin(urlAnalog);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(analogValue));
    http.end();

    // Level
    String urlLevel = String(FIREBASE_HOST) +
      "/sensors/sound/level.json?auth=" +
      FIREBASE_SECRET;

    http.begin(urlLevel);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(soundLevel));
    http.end();
  }

  delay(2000);
}
