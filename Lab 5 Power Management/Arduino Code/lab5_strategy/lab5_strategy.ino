

#include <WiFi.h>
// #include <FirebaseESP32.h>
#include <Firebase_ESP_Client.h>

#define TRIG_PIN 2                                                          // 触发信号引脚
#define ECHO_PIN 21                                                         // 回声信号引脚
#define WIFI_SSID "UW MPSK"                                                 // 你的 WiFi SSID
#define WIFI_PASSWORD "p{/qVnxJ6E"                                          // 你的 WiFi 密码
#define FIREBASE_HOST "https://techin514-lab-default-rtdb.firebaseio.com/"  // Firebase 地址
#define FIREBASE_AUTH "AIzaSyBSJ6jBTIXPZtg3FMmAWm6PNccxalMQBTs"             // Firebase 认证密钥

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;
FirebaseJson json;

float distance;
int retries;
float measuredDistance;

// NTP Server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

float interval = 8000;        // read every 5 seconds
float deepSleepDuration = 65;  // sleep duration in sec

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;  // convert to cm
}

static unsigned long lastMovementMillis = millis();  // Tracks last motion detection time
unsigned long previousMillis = millis();

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("in setup");
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Serial initialized");
  esp_sleep_enable_timer_wakeup(deepSleepDuration * 1000 * 1000);

  
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


  // Initialize Firebase
  Firebase.begin(&config, &auth);
}

void loop() {

  unsigned long currentMillis = millis();
  // Check if it's time to take a measurement
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    measuredDistance = measureDistance();  // Measure distance from the ultrasonic sensor

    if (measuredDistance <= 50.0) {
      // Motion detected: reset the idle timer
      lastMovementMillis = currentMillis;
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("Waiting.");
      }
      Serial.println("Connected to WiFi!");
      // Prepare JSON data for Firebase
      json.clear();
      json.set("/Timestamp", currentMillis);  // Unix timestamp in milliseconds
      json.set("/Distance", measuredDistance);
      // json.set("/Transmission Rate", interval);

      String path = "/ESP32/Data/" + String(currentMillis);

      // Upload the data to Firebase
      if (Firebase.RTDB.setJSON(&firebaseData, path.c_str(), &json)) {
        Serial.println("Distance uploaded to Firebase");
      } else {
        Serial.println("Firebase upload failed");
      }
      WiFi.disconnect();
    }
  }

  // Check if no motion has been detected for 20 seconds
  if (currentMillis - lastMovementMillis >= 16000) {
    Serial.println("No movement detected, entering deep sleep...");
    esp_deep_sleep_start();
  }
}
