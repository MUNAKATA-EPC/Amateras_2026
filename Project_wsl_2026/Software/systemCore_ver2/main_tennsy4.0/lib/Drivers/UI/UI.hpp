#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Functions.hpp"
#include "Sensors.hpp"

class UI
{
private:
    Adafruit_SSD1306 *_ssd = nullptr;

    Button *_enterbtn;
    Button *_rightbtn;
    Button *_leftbtn;
    uint8_t _address;

public:
    // 関数・コントラクタ
    UI(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height);
    void begin(Button *enterbtn, Button *rightbtn, Button *leftbtn);
    void process();
};