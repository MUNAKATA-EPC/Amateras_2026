#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Enums/Enums.hpp"
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

    bool _actionDecided = false;
    bool _modeDecided = false;
    int _actionNumber = 0;
    int _modeNumber = 0;
    int _configNumber = 0;
    bool _configActive[Action::Type::COUNT][10][10] = {false}; // [Actionの番号][Modeの番号][Configの番号]

public:
    // 関数・コントラクタ
    UI(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height);
    void begin(Button *enterbtn, Button *rightbtn, Button *leftbtn);
    void process();

    // データの取得
    bool actionDecided() const { return _actionDecided; }
    bool modeDecided() const { return _modeDecided; }
    int actionNumber() const { return _actionNumber; }
    int modeNumber() const { return _modeNumber; }
    int configNumber() const { return _configNumber; }
    bool configActive(uint8_t index) const { return _configActive[_actionNumber][_modeNumber][index]; }
};