


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

// 设备状态
enum State {
  IDLE,
  ULTRASONIC_ONLY,
  ULTRASONIC_WIFI,
  ULTRASONIC_WIFI_FIREBASE,
  DEEP_SLEEP
};

State currentState = IDLE;

// 预先声明变量，避免 case 跨作用域
float distance;
int retries;
float measuredDistance;

// 测量超声波距离
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;  // 转换成 cm
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("Serial initialized");
  esp_sleep_enable_timer_wakeup(5000000);


  // while (WiFi.status() != WL_CONNECTED) {
  //     delay(500);
  //     Serial.print("Waiting.");
  // }
  // Serial.println("Connected to WiFi!");

  // Firebase 配置
}

unsigned long previousMillis = 0;  // 用于记录上次时间
const long interval = 5000;        // 每个阶段的持续时间（5秒）
float d = 0;
void loop() {
  unsigned long currentMillis = millis();  // 获取当前时间
  while (currentMillis - previousMillis < interval) {
    // 更新上次执行时间
    currentMillis = millis();
    Serial.println("idle");  // 执行 Ultrasonic 阶段
  }
  previousMillis = currentMillis;
  // Ultrasonic阶段
  while (currentMillis - previousMillis < interval) {
    // 更新上次执行时间
    currentMillis = millis();
    ultrasonic();  // 执行 Ultrasonic 阶段
  }
  previousMillis = currentMillis;

  delay(100);

  // Wifi阶段
  while (currentMillis - previousMillis < interval) {
    // previousMillis = currentMillis;  // 更新上次执行时间
    currentMillis = millis();
    ultrasonic();
    wifi();  // 执行 Wifi 阶段
  }
  previousMillis = currentMillis;

  delay(100);
  // 发送数据到 Firebase
  while (currentMillis - previousMillis < interval) {
    // previousMillis = currentMillis;  // 更新上次执行时间
    currentMillis = millis();
    d = ultrasonic();  // 获取测量的距离
    // wifi();
    sendDataToFirebase(d);  // 发送数据到 Firebase
    delay(500);
  }

  previousMillis = currentMillis;
  delay(100);
  // deepSleep();
  deepsleep();
}

float ultrasonic() {
  // 进入 Ultrasonic 阶段
  Serial.println("Entering Ultrasonic stage");
  distance = measureDistance();
  Serial.print("Measured Distance: ");
  Serial.println(distance);
  return distance;
}

void wifi() {
  // 进入 Wifi 阶段
  Serial.println("Entering Wifi stage");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // 连接WiFi
  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.println("WiFi Connected");
  // } else {
  //   Serial.println("WiFi Connection Failed");
  // }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Waiting.");
  }
  Serial.println("Connected to WiFi!");
  WiFi.disconnect();
}
void sendDataToFirebase(float distance) {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Waiting.");
  }
  Serial.println("Connected to WiFi!");
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  measuredDistance = ultrasonic();
  // 检查数据是否发送成功'
   delay(500);
  if (Firebase.RTDB.setFloat(&firebaseData, "/ESP32/Distance", measuredDistance)) {
    Serial.println("Distance uploaded to Firebase");
  } else {
    Serial.println("Firebase upload failed");
    Serial.println(firebaseData.errorReason());
  }
  // Firebase.end(&config, &auth);
  WiFi.disconnect();
}
void deepsleep() {
  // 进入 Deepsleep 阶段
  Serial.println("Entering Deepsleep stage");

  // 执行DeepSleep
  Serial.println("Going to Deep Sleep");
  // LowPower.sleep();  // 调用库将设备进入深度睡眠
  esp_deep_sleep_start();
}



