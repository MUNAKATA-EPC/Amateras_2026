#include "UI.hpp"

UI::UI(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height)
{
    if (_ssd != nullptr)
    {
        delete _ssd;
    }
    _ssd = new Adafruit_SSD1306(width, height, wire, -1);

    _address = address;
}

void UI::begin(Button *enterbtn, Button *rightbtn, Button *leftbtn)
{
    Timer my_display_timer;
    my_display_timer.reset();
    while (!_ssd->begin(SSD1306_SWITCHCAPVCC, _address) && my_display_timer.msTime() < 5000)
    {
        delay(10); // SSD1306との通信開始待ち
    }
    if (!_ssd->begin(SSD1306_SWITCHCAPVCC, _address))
    {
        Serial.println("ssd timeout");
    }
    _ssd->clearDisplay(); // 画面初期化

    _enterbtn = enterbtn;
    _rightbtn = rightbtn;
    _leftbtn = leftbtn;

    _enterbtn->begin();
    _rightbtn->begin();
    _leftbtn->begin();
}

void UI::process()
{
}