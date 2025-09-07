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
    bool ssdsuccess = _ssd->begin(SSD1306_SWITCHCAPVCC, _address);
    while (!ssdsuccess && my_display_timer.msTime() < 5000)
    {
        delay(10); // SSD1306との通信開始待ち
    }
    if (!ssdsuccess)
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

    // ボタン
    _enterbtn->update();
    _rightbtn->update();
    _leftbtn->update();

    // 操作
    if (_leftbtn->isReleased())
    {
        if (_modeDecided)
        {
            _modeDecided = false;
            for (int i = 0; i < 10; i++)
                _configActive[i] = false;
            _configNumber = 0;
        }
        else if (_actionDecided)
        {
            _actionDecided = false;
            _modeNumber = 0;
        }
    }

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
    }
    else
    {
        if (_rightbtn->isReleased())
        {
            _configNumber++;
            uint8_t configCount = 0;
            switch (_actionNumber)
            {
            case Action::ATTACKER:
                configCount = Attacker::Config::CONFIG_COUNT;
                break;
            case Action::DEFENDER:
                configCount = Defender::Config::CONFIG_COUNT;
                break;
            case Action::TEST:
                configCount = Test::Config::CONFIG_COUNT;
                break;
            case Action::RADICON:
                configCount = Radicon::Config::CONFIG_COUNT;
                break;
            }
            if (_configNumber >= configCount)
                _configNumber = 0;
        }

        if (_enterbtn->isReleased())
        {
            _configActive[_configNumber] = !_configActive[_configNumber];
        }
    }

    // 表示
    const char *actionName;
    const char *modeName;
    const char *configName[10]; // 最大10個まで対応
    uint8_t configCount = 0;

    switch (_actionNumber)
    {
    case Action::Type::ATTACKER:
        actionName = "Attacker";
        if (_actionDecided)
        {
            switch (_modeNumber)
            {
            case Attacker::Mode::GYRO:
                modeName = "Gyro";
                break;
            case Attacker::Mode::YELLOWGOAL:
                modeName = "YellowGoal";
                break;
            case Attacker::Mode::BLUEGOAL:
                modeName = "BlueGoal";
                break;
            default:
                modeName = "Unknown";
                break;
            }
        }
        else
        {
            modeName = "";
        }

        if (_modeDecided)
        {
            configCount = Attacker::Config::CONFIG_COUNT;
            configName[0] = "Run";
            configName[1] = "Low";
            configName[2] = "Middle";
            configName[3] = "High";
        }
        break;

    case Action::Type::DEFENDER:
        actionName = "Defender";
        if (_actionDecided)
        {
            switch (_modeNumber)
            {
            case Defender::Mode::GYRO:
                modeName = "Gyro";
                break;
            case Defender::Mode::YELLOWGOAL:
                modeName = "YellowGoal";
                break;
            case Defender::Mode::BLUEGOAL:
                modeName = "BlueGoal";
                break;
            default:
                modeName = "Unknown";
                break;
            }
        }
        else
        {
            modeName = "";
        }

        if (_modeDecided)
        {
            configCount = Defender::Config::CONFIG_COUNT;
            configName[0] = "Run";
            configName[1] = "Low";
            configName[2] = "Middle";
            configName[3] = "High";
        }
        break;

    case Action::Type::TEST:
        actionName = "Test";
        if (_actionDecided)
        {
            switch (_modeNumber)
            {
            case Test::Mode::KICKER:
                modeName = "Kicker";
                break;
            case Test::Mode::GYRO:
                modeName = "Gyro";
                break;
            case Test::Mode::CAM:
                modeName = "Cam";
                break;
            default:
                modeName = "Unknown";
                break;
            }
        }
        else
        {
            modeName = "";
        }

        if (_modeDecided)
        {
            configCount = Test::Config::CONFIG_COUNT;
            configName[0] = "Run";
        }
        break;

    case Action::Type::RADICON:
        actionName = "Radicon";
        if (_actionDecided)
        {
            switch (_modeNumber)
            {
            case Radicon::Mode::SPEED_50CC:
                modeName = "50cc";
                break;
            case Radicon::Mode::SPEED_100CC:
                modeName = "100cc";
                break;
            case Radicon::Mode::SPEED_200CC:
                modeName = "200cc";
                break;
            default:
                modeName = "Unknown";
                break;
            }
        }
        else
        {
            modeName = "";
        }

        if (_modeDecided)
        {
            configCount = Radicon::Config::CONFIG_COUNT;
            configName[0] = "Run";
            configName[1] = "LineAuto";
            configName[2] = "AttackerAuto";
            configName[3] = "KickerAuto";
        }
        break;

    default:
        actionName = "Unknown";
        modeName = "Unknown";
        break;
    }
    // 画面描画
    _ssd->setTextSize(1);
    _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _ssd->setCursor(0, 0);
    _ssd->println(actionName);
    if (_actionDecided)
        _ssd->print(">");
    else
        _ssd->print("");
    _ssd->println(modeName);
    // config 表示
    for (int i = 0; i < configCount; i++)
    {
        if (i == 0) // Runは大きく
            _ssd->setTextSize(2);
        else
            _ssd->setTextSize(1);

        if (i == _configNumber) // 選択中の文字は変更する
        {
            //_ssd->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            if (i == 0)
                _ssd->print("|");
            else
                _ssd->print(" |");
        }
        else
        {
            //_ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            if (i == 0)
                _ssd->print(" ");
            else
                _ssd->print("  ");
        }

        _ssd->print(configName[i]);
        if (i != 0) // Runじゃないなら
        {
            int tabspace = 7 - strlen(configName[i]);
            for (int i = 0; i < tabspace; i++)
            {
                _ssd->print(" ");
            }
        }
        _ssd->print(":");
        _ssd->print(_configActive[i] ? "on" : "off");

        _ssd->println("");
    }

    _ssd->display();
}
