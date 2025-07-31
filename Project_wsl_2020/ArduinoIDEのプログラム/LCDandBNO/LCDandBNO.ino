#include <math.h>
#include <Arduino.h>

#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
#define BNO055_SAMPLERATE_DELAY_MS 10
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);  // BNO用のI2Cを指定
int raw_deg;
int reset_deg;
int deg;
void bno_init();
int bno_process();
int IR_edge = -1;

#include <Adafruit_GFX.h>  //Adarfuitのライブラリーを読み込む
#include <Adafruit_SSD1306.h>
const int i2c_ADDRESS = 0x3C;              // I2Cのアドレス指定
Adafruit_SSD1306 display(128, 64, &Wire);  // ディスプレイ用のI2Cを指定
void dot_line(float, float);
void dot_Circle(int, int, int);

void setup() {
  Wire.begin();
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC, i2c_ADDRESS);  // ディスプレイを初期化。
  display.clearDisplay();                            // 画面描写をクリアー
  pinMode(0, OUTPUT);
  //analogWriteResolution(10);
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
  bno_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  int gyro_edge = bno_process();
  Serial.println(gyro_edge);

  float xx = cos((gyro_edge + 90) * PI / 180) * 30;
  float yy = sin((gyro_edge + 90) * PI / 180) * 30;

  dot_line(xx, yy);
  dot_Circle(0, 0, 30);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.print(gyro_edge);
  display.display();

  analogWrite(0, gyro_edge * 5 / 3.3);

  delay(10);
  display.clearDisplay();
}

// function

void bno_init() {
  delay(250);

  if (!bno.begin()) {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1) {
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 0);
      display.print("(-_-)Absolut death");
      display.display();
      delay(300);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 0);
      display.print("(>_<)Absolut death");
      display.display();
      delay(300);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 0);
      display.print("(#_#)Absolut death");
      display.display();
      delay(300);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 0);
      display.print("(;_;)Absolut death");
      display.display();
      delay(300);
      display.clearDisplay();
    }
  }

  bno.setExtCrystalUse(true);
}

int bno_process() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  raw_deg = euler.x();
  deg = (int)(euler.x() - reset_deg + 360) % 360;


  if (digitalRead(2) == LOW) {
    reset_deg = raw_deg;
  }

  return deg;
}

void dot_line(float display_x, float display_y) {
  int dot_x = int(display_x) + 64;
  int dot_y = -int(display_y) + 32;
  display.drawLine(64, 32, dot_x, dot_y, SSD1306_WHITE);
}

void dot_Circle(int display_x, int display_y, int Circle_far) {
  int dot_x = display_x + 64;
  int dot_y = -display_y + 32;
  display.drawCircle(dot_x, dot_y, Circle_far, SSD1306_WHITE);
}
