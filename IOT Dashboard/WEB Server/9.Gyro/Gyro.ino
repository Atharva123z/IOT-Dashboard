#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <MPU6050.h>

// -------- WiFi Credentials --------
#define WIFI_SSID "Vivo Y21e"
#define WIFI_PASSWORD "123456789"

// -------- Firebase Credentials --------
#define DATABASE_URL "https://esp-projects-92383-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "npuRerfKNuU8nUmAM2XZVsgVFsfJQXNSXDdIerLH"

// -------- MPU6050 --------
MPU6050 mpu;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Start I2C
  Wire.begin(21, 22);

  // Init MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("❌ MPU6050 connection failed");
    while (1);
  }
  Serial.println("✅ MPU6050 connected");

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

  // 🔗 CLICKABLE WEBSITE LINK (OPTION 2)
  Serial.println("================================");
  Serial.println("OPEN GYRO SENSOR DASHBOARD:");
  Serial.println("https://esp-projects-92383.web.app/gyro");
  Serial.println("================================");

  // Firebase setup
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase connected");
}

void loop() {
  int16_t gx, gy, gz;

  mpu.getRotation(&gx, &gy, &gz);

  Serial.print("GX: ");
  Serial.print(gx);
  Serial.print(" | GY: ");
  Serial.print(gy);
  Serial.print(" | GZ: ");
  Serial.println(gz);

  // Upload to Firebase
  Firebase.RTDB.setInt(
    &fbdo,
    "/gyro/gx",
    gx
  );

  Firebase.RTDB.setInt(
    &fbdo,
    "/gyro/gy",
    gy
  );

  Firebase.RTDB.setInt(
    &fbdo,
    "/gyro/gz",
    gz
  );

  delay(200); // gyro updates faster than others
}
