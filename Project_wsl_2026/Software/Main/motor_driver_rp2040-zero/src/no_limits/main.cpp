#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "motor.hpp"
#include "packetManager.hpp"

// パケットマネージャーの定義
PacketManager packet;

// モーター設定
Motor motors[4];

// Core 0
void setup()
{
  // モーターピン設定
  motors[0].setPin(2, 4, 3);
  motors[1].setPin(5, 7, 6);
  motors[2].setPin(8, 14, 15);
  motors[3].setPin(26, 27, 28);

  Serial.begin(9600);    // PC用
  Serial1.begin(115200); // Teensy用
  Serial1.setTimeout(10);

  Serial1.flush(); // 受信バッファをクリア
  while (!Serial1.available())
    ;                     // Teensyからの信号を待つ
  rp2040.fifo.push(true); // Core1起動

  packet.setup(0x55, 8, 0xAA);
}

const int stop_v = -32768;
int power[4] = {0, 0, 0, 0};

void loop()
{
  while (Serial1.available() > 0)
  {
    packet.add(Serial1.read());

    if (packet.isComplete())
    {
      // パケットが完成するたびにデータを読み出す
      uint8_t low0 = packet.get(1);
      uint8_t high0 = packet.get(2);
      uint8_t low1 = packet.get(3);
      uint8_t high1 = packet.get(4);
      uint8_t low2 = packet.get(5);
      uint8_t high2 = packet.get(6);
      uint8_t low3 = packet.get(7);
      uint8_t high3 = packet.get(8);

      power[0] = (int)int16_t((uint16_t(high0) << 8) | uint16_t(low0));
      power[1] = (int)int16_t((uint16_t(high1) << 8) | uint16_t(low1));
      power[2] = (int)int16_t((uint16_t(high2) << 8) | uint16_t(low2));
      power[3] = (int)int16_t((uint16_t(high3) << 8) | uint16_t(low3));

      power[0] = constrain(power[0], -32768, 32767);
      power[1] = constrain(power[1], -32768, 32767);
      power[2] = constrain(power[2], -32768, 32767);
      power[3] = constrain(power[3], -32768, 32767);
    }
  }

  for (int i = 0; i < 4; i++)
  {
    // モーター制御
    if (power[i] != stop_v)
    {
      motors[i].move(power[i]);
    }
    else
    {
      motors[i].stop();
    }
  }
}

// NeoPixel設定
#define PIN_NEOPIXEL 16
#define NUM_PIXELS 1
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Core 1
void setup1()
{
  pixels.begin();
  pixels.setBrightness(30);

  // 赤で待機（Core 0からの信号待ち）
  pixels.setPixelColor(0, pixels.Color(50, 0, 0));
  pixels.show();

  while (!rp2040.fifo.available())
    ;
  rp2040.fifo.pop(); // 起動信号を消費

  // 起動イルミネーション（虹色を1周させる）
  for (uint32_t i = 0; i < 65536; i += 256)
  {
    uint32_t rgb_color = pixels.ColorHSV(i, 255, 50);
    pixels.setPixelColor(0, rgb_color);
    pixels.show();
    delay(10);
  }

  // 終了後に一旦消灯
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
  delay(100);
}

void loop1()
{
  static uint32_t current_color = pixels.Color(0, 0, 0);

  if (rp2040.fifo.available())
  {
    int deg = (int)rp2040.fifo.pop(); // 角度を受信

    if (deg == 0xFF)
    {
      current_color = pixels.Color(17, 17, 17); // 停止時は暗い白
    }
    else
    {
      // Core 0からの角度(-180~180)を0~65535に変換
      uint16_t hue = (uint16_t)(deg * 65535 / 360);
      current_color = pixels.ColorHSV(hue, 255, 50);
    }
  }

  pixels.setPixelColor(0, current_color);
  pixels.show();
  delay(20);
}