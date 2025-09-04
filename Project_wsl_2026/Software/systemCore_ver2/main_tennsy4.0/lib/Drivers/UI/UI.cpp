#include "UI.hpp"

UI::UI(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height)
{
    /*if (_ssd != nullptr)
    {
        delete _ssd;
    }*/
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
    // 初期化
    _ssd->clearDisplay();

    //  ボタン
    _enterbtn->update();
    _rightbtn->update();
    _leftbtn->update();

    // 操作
    if (!_actionDecided)
    {
        if (_rightbtn->isReleased())
        {
            _actionNumber++;
            if (_actionNumber > Action::Type::COUNT - 1)
                _actionNumber = 0;
        }
        if (_enterbtn->isReleased())
            _actionDecided = true;
    }
    else if (!_modeDecided)
    {
        if (_rightbtn->isReleased())
        {
            _modeNumber++;
            switch (_actionNumber)
            {
            case Action::Type::ATTACKER:
                if (_modeNumber > Attacker::Mode::MODE_COUNT - 1)
                    _modeNumber = 0;

                break;

            case Action::Type::DEFENDER:
                if (_modeNumber > Defender::Mode::MODE_COUNT - 1)
                    _modeNumber = 0;

                break;

            case Action::Type::TEST:
                if (_modeNumber > Test::Mode::MODE_COUNT - 1)
                    _modeNumber = 0;

                break;

            case Action::Type::RADICON:
                if (_modeNumber > Radicon::Mode::MODE_COUNT - 1)
                    _modeNumber = 0;

                break;
            }
        }

        if (_enterbtn->isReleased())
            _modeDecided = true;

        if (_leftbtn->isReleased())
            _actionDecided = false;
    }
    else
    {
        if (_rightbtn->isReleased())
        {
            _configNumber++;
            switch (_actionNumber)
            {
            case Action::ATTACKER:
                if (_configNumber > Attacker::Config::CONFIG_COUNT - 1)
                    _configNumber = 0;

                break;

            case Action::DEFENDER:
                if (_configNumber > Defender::Config::CONFIG_COUNT - 1)
                    _configNumber = 0;

                break;

            case Action::TEST:
                if (_configNumber > Test::Config::CONFIG_COUNT - 1)
                    _configNumber = 0;

                break;

            case Action::RADICON:
                if (_configNumber > Radicon::Config::CONFIG_COUNT - 1)
                    _configNumber = 0;

                break;
            }

            if (_enterbtn->isReleased())
            {
                if (_configActive[_actionNumber][_modeNumber][_configNumber] == true)
                    _configActive[_actionNumber][_modeNumber][_configNumber] = false;
                else
                    _configActive[_actionNumber][_modeNumber][_configNumber] = true;
            }

            if (_leftbtn->isReleased())
                _modeDecided = false;
        }
    }

    //  表示
    _ssd->display();
}