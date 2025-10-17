#include "ui.hpp"

inline Adafruit_SSD1306 *_ssd = nullptr;

inline uint8_t _address;

inline int _meterNumber = 0;

inline bool _actionDecided = false;
inline bool _modeDecided = false;
inline int _actionNumber = 0;
inline int _modeNumber = 0;
inline int _configNumber = 0;
inline bool _configActive[10] = {false}; // [Configの番号]

bool uiInit(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height)
{
    /*if (_ssd != nullptr)
        {
            delete _ssd;
        }*/
    _ssd = new Adafruit_SSD1306(width, height, wire, -1);

    _address = address;

    Timer my_display_timer;
    my_display_timer.reset();
    bool success = _ssd->begin(SSD1306_SWITCHCAPVCC, _address);
    while (!success && my_display_timer.msTime() < 100)
    {
        delay(10); // SSD1306との通信開始待ち
    }

    _ssd->clearDisplay(); // 画面初期化
    _ssd->display();      // 試験的描画

    return success;
}

// デバッグ用
void uiPrintDebug(const char *msg)
{
    _ssd->clearDisplay(); // 画面初期化
    _ssd->setTextSize(1);
    _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _ssd->setCursor(0, 0);
    _ssd->println(msg);
    _ssd->display(); // 試験的描画
}

// ssd1306ライブラリーの拡張
// メータ表示用
void uiDrawCircleMeter(bool show, int x0, int y0, int r, const char *s, int deg)
{
    _ssd->drawCircle(x0, y0, r, SSD1306_WHITE);

    int x1 = x0, y1 = y0;
    if (show)
    {
        x1 = x0 - sin(radians(deg)) * r;
        y1 = y0 - cos(radians(deg)) * r;
    }
    _ssd->drawLine(x0, y0, x1, y1, SSD1306_WHITE);

    String buf = s;
    buf += ":";
    buf += String(deg);

    int text_x = x0 - r, text_y = y0 + r + 1;
    _ssd->setTextSize(1);
    _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _ssd->setCursor(text_x, text_y);

    _ssd->print(buf);
}

void uiProcess(bool show, bool enterbtn, bool rightbtn, bool leftbtn)
{
    // 操作
    if (leftbtn)
    {
        if (_modeDecided)
        {
            _modeDecided = false;
            for (int i = 0; i < 10; i++)
                _configActive[i] = false;
            _configNumber = 0;

            _meterNumber = 0; // リセット
        }
        else if (_actionDecided)
        {
            _actionDecided = false;
            _modeNumber = 0;

            _meterNumber = 0; // リセット
        }
        else
        {
            _meterNumber = (_meterNumber + 1 + 6) % 6; // 0~4を周期するようにする
        }
    }

    if (!_actionDecided)
    {
        if (rightbtn)
        {
            _actionNumber++;
            if (_actionNumber > Action::Type::COUNT - 1)
                _actionNumber = 0;
        }
        if (enterbtn)
            _actionDecided = true;
    }
    else if (!_modeDecided)
    {
        if (rightbtn)
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
        if (enterbtn)
            _modeDecided = true;
    }
    else
    {
        if (rightbtn)
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

            // runは右が押されたらfalseになる
            _configActive[0] = false;
        }

        if (enterbtn)
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
    if (show)
    {
        _ssd->clearDisplay(); // 初期化

        _ssd->setTextSize(1);
        _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        _ssd->setCursor(0, 0);
        // action表示
        _ssd->print("> ");
        _ssd->print(actionName);
        if (!_actionDecided)
        {
            _ssd->println(" <");
            /*
            _ssd->setCursor(0, 10); // 改行だと多いのでセットし直す

            switch (_meterNumber)
            {
            case 0:
                _ssd->println("[gyro]");
                _ssd->print(" bno055");
                uiDrawCircleMeter(true, 87, 32, 20, "deg", bno.deg());
                break;
            case 1:
                _ssd->println("[ir]");
                _ssd->print(" dis:");
                _ssd->print(String(ir.dis()));
                uiDrawCircleMeter(true, 87, 32, 20, "deg", ir.deg());
                break;
            case 2:
                _ssd->println("[line]");
                _ssd->println("/normal");
                _ssd->println("/side");
                uiDrawCircleMeter(true, 67, 32, 20, "n", line.deg(LineSensor::RING));
                uiDrawCircleMeter(true, 107, 32, 20, "s", line.deg(LineSensor::SIDE));
                break;
            case 3:
                _ssd->println("[blueGoal]");
                _ssd->print(" dis:");
                _ssd->print(String(cam.dis(Openmv::BLUEGOAL)));
                uiDrawCircleMeter(true, 87, 32, 20, "deg", cam.deg(Openmv::BLUEGOAL));
                break;
            case 4:
                _ssd->println("[yellowGoal]");
                _ssd->print(" dis:");
                _ssd->print(String(cam.dis(Openmv::YELLOWGOAL)));
                uiDrawCircleMeter(true, 97, 32, 20, "deg", cam.deg(Openmv::YELLOWGOAL));
                break;
            case 5:
                _ssd->println("[field]");
                _ssd->print(" dis:");
                _ssd->print(String(cam.dis(Openmv::FIELD)));
                uiDrawCircleMeter(true, 87, 32, 20, "deg", cam.deg(Openmv::FIELD));
                break;
            }
            */
        }
        else
        {
            _ssd->println("");

            // mode表示
            _ssd->print(" > ");
            _ssd->print(modeName);
            if (!_modeDecided)
            {
                _ssd->println(" <");
            }
            else
            {
                _ssd->println("");

                // config 表示
                int configNameMaxLength = 0;
                for (int i = 0; i < configCount; i++)
                {
                    int len = strlen(configName[i]);
                    if (len > configNameMaxLength)
                        configNameMaxLength = len;
                }
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
                        int tabspace = (configNameMaxLength + 1) - strlen(configName[i]);
                        for (int i = 0; i < tabspace; i++)
                        {
                            _ssd->print(" ");
                        }
                    }
                    _ssd->print(":");
                    _ssd->print(_configActive[i] ? "on" : "off");

                    _ssd->println("");
                }
            }
        }

        _ssd->display(); // 表示
    }
}

// 　データ取得
bool uiActionDecided() { return _actionDecided; }
bool uiModeDecided() { return _modeDecided; }
bool uiRunning() { return _modeDecided && _actionDecided && _configActive[0]; }
int uiActionNumber() { return _actionNumber; }
int uiModeNumber() { return _modeNumber; }
int uiConfigNumber() { return _configNumber; }
bool uiConfigActive(uint8_t index) { return _configActive[index]; }
