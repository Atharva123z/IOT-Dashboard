#include <WiFi.h>
#include <HTTPClient.h>
#include <PulseSensorPlayground.h>

// -------- WiFi --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase --------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- Google Sheets --------
String scriptURL = "https://script.google.com/macros/s/AKfycbwhRQBZmy20q2Zrqh-GzjPk5-zNZ0TQm8Qjx1SjJ5gnuHDTawl-ovDJ2Ug43TDzHx_9/exec";

// -------- Pulse Sensor --------
#define PULSE_PIN 34
#define THRESHOLD 1400

PulseSensorPlayground pulseSensor;

// -------- Variables --------
unsigned long lastSend = 0;
int lastBPM = 0;

void setup() {

  Serial.begin(115200);

  // Pulse sensor setup
  pulseSensor.analogInput(PULSE_PIN);
  pulseSensor.setThreshold(THRESHOLD);

  if (!pulseSensor.begin()) {
    Serial.println("Pulse Sensor Failed");
    while (1);
  }

  Serial.println("Pulse Sensor Ready");

  // WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Dashboard:");
  Serial.println("https://esp-projects-92383.web.app/pulse");
}

void loop() {

  int signal = pulseSensor.getLatestSample();
  int bpm = pulseSensor.getBeatsPerMinute();

  // Detect heartbeat
  if (pulseSensor.sawStartOfBeat() && bpm > 0) {

    lastBPM = bpm;

    // Serial Plotter output
    Serial.print(signal);
    Serial.print(",");
    Serial.println(lastBPM);
  }

  // Send data every 5 seconds
  if (millis() - lastSend > 5000) {

    lastSend = millis();

    if (WiFi.status() == WL_CONNECTED && lastBPM > 0) {

      HTTPClient http;

      // ================= FIREBASE =================
      String firebaseURL = String(FIREBASE_HOST) +
                           "/sensors/pulse/bpm.json?auth=" +
                           FIREBASE_SECRET;

      http.begin(firebaseURL);
      http.addHeader("Content-Type", "application/json");

      int firebaseCode = http.PUT(String(lastBPM));

      Serial.print("Firebase response: ");
      Serial.println(firebaseCode);

      http.end();

      // ================= GOOGLE SHEETS =================
      String sheetURL = scriptURL +
                        "?bpm=" + String(lastBPM) +
                        "&signal=" + String(signal);

      http.begin(sheetURL);

      int sheetCode = http.GET();

      Serial.print("Sheets response: ");
      Serial.println(sheetCode);

      http.end();

      Serial.println("Data sent\n");
    }
  }
}