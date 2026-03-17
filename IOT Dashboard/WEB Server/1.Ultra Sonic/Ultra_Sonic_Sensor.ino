#include <WiFi.h>
#include <HTTPClient.h>

// ---------- WiFi Credentials ----------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// ---------- Firebase REST ----------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// ---------- Ultrasonic Pins ----------
#define TRIG_PIN 14
#define ECHO_PIN 15

// ---------- Distance Calculation ----------
float getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) return -1;  // no echo

  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // ---------- WiFi ----------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Clickable dashboard link
  Serial.println("================================");
  Serial.println("OPEN ULTRASONIC DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/ultrasonic");
  Serial.println("================================");
}

void loop() {
  float distance = getDistanceCM();

  if (distance > 0 && WiFi.status() == WL_CONNECTED) {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    HTTPClient http;

    String url = String(FIREBASE_HOST) +
      "/sensors/ultrasonic/distance.json?auth=" +
      FIREBASE_SECRET;

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.PUT(String(distance));

    Serial.print("Firebase HTTP code: ");
    Serial.println(httpCode);

    http.end();
  } else {
    Serial.println("Ultrasonic: No echo or WiFi down");
  }

  delay(2000);
}
