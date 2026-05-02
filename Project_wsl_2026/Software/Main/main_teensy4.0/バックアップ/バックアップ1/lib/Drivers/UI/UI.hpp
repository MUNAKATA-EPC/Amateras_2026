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

    uint8_t _address;

    int _meterNumber = 0;

    bool _actionDecided = false;
    bool _modeDecided = false;
    int _actionNumber = 0;
    int _modeNumber = 0;
    int _configNumber = 0;
    bool _configActive[10] = {false}; // [Configの番号]

public:
    // 関数・コンストラクタ
    UI(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height);
    bool begin();
    void process(bool show, bool enterbtn, bool rightbtn, bool leftbtn);

    // ssd1306の拡張
    void circleMeter(bool show, int x0, int y0, int r, const char *s, int deg);
    void debug(const char *msg);

    // データの取得
    bool actionDecided() const { return _actionDecided; }
    bool modeDecided() const { return _modeDecided; }
    bool running() const { return _modeDecided && _actionDecided && _configActive[0]; }
    int actionNumber() const { return _actionNumber; }
    int modeNumber() const { return _modeNumber; }
    int configNumber() const { return _configNumber; }
    bool configActive(uint8_t index) const { return _configActive[index]; }
};