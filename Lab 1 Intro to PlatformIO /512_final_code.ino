#include "Adafruit_APDS9960.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// OLED显示屏参数
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
// led corresponding pin
#define RED_PIN A1    
#define GREEN_PIN A2 
#define BLUE_PIN A0  

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define NUMFLAKES 7 // Number of snowflakes in the animation example

// 传感器实例
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_APDS9960 apds;
Adafruit_BME280 bme;





#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16


static const unsigned char PROGMEM flame[] = {
0b00000000, 0b00000000, 0b00000001, 0b00000000,
0b00000011, 0b00000000, 0b00000011, 0b00000000,
0b00000111, 0b00000000, 0b00000111, 0b00010000,
0b00001011, 0b10110000, 0b00001011, 0b11111000,
0b00011011, 0b11111000, 0b00011111, 0b01111000,
0b00111111, 0b01111100, 0b00111100, 0b01111100,
0b00111100, 0b00111100, 0b00011000, 0b00011000,
0b00001100, 0b00010000, 0b00000000, 0b00000000
};


static const unsigned char PROGMEM snowflake[] ={
0b00000000, 0b00000000, 0b00000001, 0b10000000,
0b00000011, 0b11000000, 0b00010001, 0b10001000,
0b00110111, 0b11101100, 0b00011101, 0b10111000, 
0b00101101, 0b10110100, 0b00011111, 0b11111000,
0b00011111, 0b11111000, 0b00101101, 0b10110100,
0b00011101, 0b10111000, 0b00110111, 0b11101100,
0b00010001, 0b10001000, 0b00000011, 0b11000000,
0b00000001, 0b10000000, 0b00000000, 0b00000000,
};

uint8_t currentDirection = APDS9960_DOWN; // 默认方向
int8_t icons[NUMFLAKES][3];               // 动画数据
unsigned long lastUpdateTime = 0;         // 动画上次更新的时间
const uint16_t updateInterval = 50;   
// uint8_t currentGesture = 0; 
bool initialized = false;
float currentTemp = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
 
  // 初始化传感器
  if (!apds.begin() || !display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS) || !bme.begin(0x76)) {
    Serial.println("Device initialization failed! Please check your wiring.");
    for (;;);
  }
  display.clearDisplay();
  // display.setTextSize(1);
  // display.setTextColor(SSD1306_WHITE);
  // display.setCursor(0, 0);
  // display.println("Hello, world!");
  // display.display();
  // Serial.println("OLED initialized successfully.");

  
  // Serial.println("Device initialized!");
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);


  // 启用手势检测
  apds.enableGesture(true);

  // 清除显示
  display.clearDisplay();
  display.println("Ready");
  initializeAnimation(flame,LOGO_WIDTH,LOGO_HEIGHT);
  // display.display();

}

void loop() {
  // 获取温度
  float temperature = bme.readTemperature();
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // 获取手势
  uint8_t gesture = apds.readGesture();
  Serial.print(gesture);
  unsigned long currentTime = millis();
    
  if (gesture != 0) {
    currentDirection = gesture;
    initialized = false;
    

    // Debug gesture
    switch(gesture) {
      case APDS9960_UP: 
        Serial.println(F("UP Gesture")); 
        // 闪4下绿的
        digitalWrite(GREEN_PIN, HIGH);
        delay(50);
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
        delay(50);
        digitalWrite(GREEN_PIN, LOW);
        delay(50);
        digitalWrite(GREEN_PIN, HIGH);
        delay(50);
        digitalWrite(GREEN_PIN, LOW);
        delay(50);
        digitalWrite(GREEN_PIN, HIGH);
        // 闪完画图
        break;
      case APDS9960_DOWN: 
      // 闪3下绿的
        Serial.println(F("DOWN Gesture")); 
        digitalWrite(GREEN_PIN, HIGH);
        delay(50);
        digitalWrite(GREEN_PIN, LOW);
        delay(50);
        digitalWrite(GREEN_PIN, HIGH);
        delay(50);
        digitalWrite(GREEN_PIN, LOW);
        delay(50);
        digitalWrite(GREEN_PIN, HIGH);
        break;
      case APDS9960_LEFT: 
      // 闪2下绿的
        Serial.println(F("LEFT Gesture")); 
        digitalWrite(GREEN_PIN, HIGH);
        delay(50);
        digitalWrite(GREEN_PIN, LOW);
        delay(50);
        digitalWrite(GREEN_PIN, HIGH);
        break;
      case APDS9960_RIGHT: 
      // 闪1下绿的
        Serial.println(F("RIGHT Gesture")); 
        digitalWrite(GREEN_PIN, HIGH);
        break;
    }
     delay(100);  // Show gesture
    
    // Reset LEDs
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
  // }
    // // Clear and update display
    // display.clearDisplay();
    // display.display();

   
  
    display.clearDisplay();

  // 根据温度绘制火焰或雪花
      if (currentTime - lastUpdateTime >= updateInterval) {
          lastUpdateTime = currentTime;

          // 根据温度选择图案
          // currentTemperature = bme.readTemperature();
          const uint8_t *bitmap = (temperature > 28) ? flame : snowflake;

          // 更新动画
          updateAnimation(bitmap, LOGO_WIDTH, LOGO_HEIGHT,temperature);
        }
      // if (temperature > 25) {
      //   updateAnimation(flame, LOGO_WIDTH, LOGO_HEIGHT,temperature);
      //   Serial.println(" draw flame");
      // } else {
      //   updateAnimation(snowflake, LOGO_WIDTH, LOGO_HEIGHT,temperature);
      //   Serial.println(" draw snow");
      // }

  // 根据温度映射到三色LED
    setRGBLED(temperature);

  // 显示更新
    display.display();
  

    delay(80);
  
  
}
// 设置RGB LED颜色
void setRGBLED(float temperature) {
  // int red = map(constrain(temperature, 25, 50), 25, 50, 0, 255);
  int blue = map(constrain(temperature, 15, 35), 2, 40, 255, 0);  // 蓝色从 2°C 到 40°C 逐渐降低
  int red = map(constrain(temperature, 15, 35), 2, 40, 0, 255);   // 红色从 2°C 到 40°C 逐渐增加


  analogWrite(A1, red);   // 红色
  // analogWrite(A1, green); // 绿色
  analogWrite(A0, blue);  // 蓝色

  Serial.print("LED Colors - R: ");
  Serial.print(red);
  // Serial.print(", G: ");
  // Serial.print(green);
  Serial.print(", B: ");
  Serial.println(blue);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

// void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h, int direction, float temperature) {
//   int8_t f, icons[NUMFLAKES][3];

//   // Initialize 'snowflake' positions
//   for(f=0; f< NUMFLAKES; f++) {
//     icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
//     icons[f][YPOS]   = -LOGO_HEIGHT;
//     icons[f][DELTAY] = random(1, 6);
//     Serial.print(F("x: "));
//     Serial.print(icons[f][XPOS], DEC);
//     Serial.print(F(" y: "));
//     Serial.print(icons[f][YPOS], DEC);
//     Serial.print(F(" dy: "));
//     Serial.println(icons[f][DELTAY], DEC);
//   }

//   for(;;) { // Loop forever...
//     display.clearDisplay(); // Clear the display buffer

//     // Draw each snowflake:
//     for(f=0; f< NUMFLAKES; f++) {
//       display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
//     }
//     String tempText = String(temperature) + " C";
    
//     // Get text bounds dynamically
    
//     // Set text size
//     display.setTextSize(2); // Set text size (2x scale)
    
//     // Set cursor to the center of the screen
//     display.setCursor(32,32);
//     display.setTextColor(SSD1306_WHITE);
//     display.println(tempText);

//     display.display(); // Show the display buffer on the screen
//     delay(200);        // Pause for 1/10 second

//     // Then update coordinates of each flake...
//     for(f=0; f< NUMFLAKES; f++) {
//       if (direction==APDS9960_UP){
//         icons[f][YPOS] -= icons[f][DELTAY];
//       }else if(direction ==APDS9960_DOWN){
//         icons[f][YPOS] += icons[f][DELTAY];
//       }else if(direction ==APDS9960_LEFT){
//         icons[f][XPOS] -= icons[f][DELTAY];

//       }else if(direction == APDS9960_RIGHT){
//         icons[f][XPOS] += icons[f][DELTAY];
//       }
     
//       // If snowflake is off the bottom of the screen...
//       if (icons[f][YPOS] >= display.height()) {
//         // Reinitialize to a random position, just off the top
//         icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
//         icons[f][YPOS]   = -LOGO_HEIGHT;
//         icons[f][DELTAY] = random(1, 6);
//       }
//     }
//   }
// }

void initializeAnimation(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  for (int f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS] = random(1 - LOGO_HEIGHT, display.height() / 2);
    icons[f][DELTAY] = random(1, 6);
  }
}
;

void updateAnimation(const uint8_t *bitmap, uint8_t w, uint8_t h,float temperature) {
  display.clearDisplay();
  if (!initialized) {
    // 初始化 'snowflake' 位置
    for (int f = 0; f < NUMFLAKES; f++) {
      icons[f][XPOS] = random(LOGO_WIDTH, display.width());
      icons[f][YPOS] = -LOGO_HEIGHT;
      icons[f][DELTAY] = random(1, 6);
    }
    initialized = true;
  }
  // 绘制每个元素
  for (int f = 0; f < NUMFLAKES; f++) {
    display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);

    // 根据方向更新位置
    switch (currentDirection) {
      case APDS9960_UP:
        icons[f][YPOS] -= icons[f][DELTAY];
        break;
      case APDS9960_DOWN:
        icons[f][YPOS] += icons[f][DELTAY];
        break;
      case APDS9960_LEFT:
        icons[f][XPOS] -= icons[f][DELTAY];
        break;
      case APDS9960_RIGHT:
        icons[f][XPOS] += icons[f][DELTAY];
        break;
    }

    // 如果超出边界，重置位置
    if (icons[f][YPOS] >= display.height() || icons[f][YPOS] < -h) {
      icons[f][XPOS] = random(1 - w, display.width());
      icons[f][YPOS] = random(1 - h, display.height() / 2);
    }
    if (icons[f][XPOS] >= display.width() || icons[f][XPOS] < -w) {
      icons[f][XPOS] = random(1 - w, display.width());
      icons[f][YPOS] = random(1 - h, display.height() / 2);
    }
  }

  // 更新显示
    String tempText = String(temperature) + " C";
    display.setTextSize(2); 
    display.setCursor(28,30);
    display.setTextColor(SSD1306_WHITE);
    display.println(tempText);
    display.display();
}


