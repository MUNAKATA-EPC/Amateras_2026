#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// SSD1306のサイズに合わせて調整（ここでは128x64）
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// 使用するI2Cインスタンスとアドレス
#define OLED_ADDRESS 0x3C

// ディスプレイオブジェクトをWire1で初期化
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);

void setup() {
  // ディスプレイ初期化
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (1) {
      // エラー時にここで止まる
    }
  }

  // 画面初期化
  display.clearDisplay();

  // テキスト表示
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello from Wire1!");
  display.display();
}

void loop() {
  // 何もしない
}
