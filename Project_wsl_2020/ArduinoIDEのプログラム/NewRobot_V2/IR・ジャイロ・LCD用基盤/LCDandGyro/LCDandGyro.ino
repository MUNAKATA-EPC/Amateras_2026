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
int IR_edge = -1, IR_dist = -1, gyro_edge = 0, posi = -1;

#include <Adafruit_GFX.h>  //Adarfuitのライブラリーを読み込む
#include <Adafruit_SSD1306.h>
const int i2c_ADDRESS = 0x3C;              // I2Cのアドレス指定
Adafruit_SSD1306 display(128, 64, &Wire);  // ディスプレイ用のI2Cを指定
void dot_line(float, float);
void dot_Circle(int, int, int);

void setup() {
  pinMode(0, OUTPUT);
  pinMode(8, OUTPUT);
  analogWriteResolution(10);
  Wire.begin();
  Wire.setClock(25000);  // Set I2C speed to 100kHz
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC, i2c_ADDRESS);  // ディスプレイを初期化。
  display.clearDisplay();                            // 画面描写をクリアー
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
  Serial1.begin(2000000);
  bno_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  gyro_edge = bno_process();
  analogWrite(0, gyro_edge * 5 / 3.3);
  if (Serial1.available()) {
    String message = Serial1.readStringUntil('\n');  // 改行までの文字列を受信
    int IR_data = message.toInt();                   // 文字列を整数に変換
    if (IR_data == -1 ||  IR_data == 0) {
      IR_edge = -1;
      IR_dist = -1;
    } else if (IR_data < 1000) {
      IR_edge = IR_data;
    } else {
      IR_dist = IR_data - 1000;
    }
    Serial.println(IR_dist);  // 受信した整数をシリアルモニタに表示
  }

  float gyro_xx = cos((gyro_edge + 90) * PI / 180) * 30;
  float gyro_yy = sin((gyro_edge + 90) * PI / 180) * 30;
  dot_line(-32, 0, gyro_xx - 32, gyro_yy);
  dot_Circle(-32, 0, 30);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.print(gyro_edge);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 56);
  display.print("Gyro");

  float IR_xx = cos((IR_edge + 90) * PI / 180) * 30;
  float IR_yy = sin((IR_edge + 90) * PI / 180) * 30;
  if (IR_edge == -1) {
    dot_line(32, 0, 32, 0);
    posi = -1;
  } else {
    dot_line(32, 0, IR_xx + 32, IR_yy);
    posi = analogRead(1);
  }
  dot_Circle(32, 0, 30);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(64, 0);
  display.print(IR_edge);
  display.setCursor(106, 0);
  display.print(IR_dist);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(64, 56);
  if (IR_dist == -1) {
    display.print("IR");
  } else if (IR_dist < posi) {
    display.print("IRnear");
    digitalWrite(8,HIGH);
  } else {
    display.print("IRfar");
    digitalWrite(8,LOW);
  }

  display.display();
  delay(1);
  display.clearDisplay();
}

// function

void bno_init() {
  delay(1000);

  if (!bno.begin()) {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1) {
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 0);
      display.print("(-_-)Absolut death");
      display.display();
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

void dot_line(float display_x1, float display_y1, float display_x2, float display_y2) {
  int dot_x1 = int(display_x1) + 64;
  int dot_y1 = -int(display_y1) + 32;
  int dot_x2 = int(display_x2) + 64;
  int dot_y2 = -int(display_y2) + 32;
  display.drawLine(dot_x1, dot_y1, dot_x2, dot_y2, SSD1306_WHITE);
}

void dot_Circle(int display_x, int display_y, int Circle_far) {
  int dot_x = display_x + 64;
  int dot_y = -display_y + 32;
  display.drawCircle(dot_x, dot_y, Circle_far, SSD1306_WHITE);
}