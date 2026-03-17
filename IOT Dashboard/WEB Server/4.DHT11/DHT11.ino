#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// -------- WiFi Credentials --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase REST --------
#define FIREBASE_HOST "https://esp-projects-92383-default-rtdb.firebaseio.com"
#define FIREBASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- DHT11 Configuration --------
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  // Start DHT sensor
  dht.begin();

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
  Serial.println("OPEN DHT11 SENSOR DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/dht11");
  Serial.println("================================");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT11 read error");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Send temperature
    String urlTemp = String(FIREBASE_HOST) +
      "/sensors/dht11/temperature.json?auth=" +
      FIREBASE_SECRET;

    http.begin(urlTemp);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(temperature));
    http.end();

    // Send humidity
    String urlHum = String(FIREBASE_HOST) +
      "/sensors/dht11/humidity.json?auth=" +
      FIREBASE_SECRET;

    http.begin(urlHum);
    http.addHeader("Content-Type", "application/json");
    http.PUT(String(humidity));
    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }

  delay(2000); // DHT11 minimum delay
}
