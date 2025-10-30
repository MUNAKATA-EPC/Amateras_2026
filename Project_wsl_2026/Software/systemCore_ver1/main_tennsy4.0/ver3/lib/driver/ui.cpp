#include "ui.hpp"

static Adafruit_SSD1306 *_ssd = nullptr;

static uint8_t _address;

static int _meterNumber = 0;

static bool _actionDecided = false;
static bool _modeDecided = false;
static int _actionNumber = 0;
static int _modeNumber = 0;
static int _configNumber = 0;
static bool _configActive[10] = {false}; // [Configの番号]

// 初期化
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

// 画面表示・初期化
void uiShow()
{
    _ssd->display(); // 描画実行
}
void uiClear()
{
    _ssd->clearDisplay(); // 描画初期化
}

// デバッグ描画用
void uiPrintDebug(const char *msg)
{
    _ssd->setTextSize(1);
    _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _ssd->setCursor(0, 0);
    _ssd->println(msg);
}

// プリント用
void uiPrint(int x0, int y0, String msg)
{
    char msg_char[50]; // 十分に確保
    msg.toCharArray(msg_char, 50);

    _ssd->setTextSize(1);
    _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _ssd->setCursor(x0, y0);

    _ssd->print(msg_char);
}
// メータ描画用
void uiDrawCircleMeter(int x0, int y0, int r, const char *s, int deg)
{
    _ssd->drawCircle(x0, y0, r, SSD1306_WHITE);

    int x1 = x0, y1 = y0;
    x1 = x0 - sin(radians(deg)) * r;
    y1 = y0 - cos(radians(deg)) * r;
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

// メインのuiの描画用
void uiDrawMain(bool enterbtn, bool rightbtn, bool leftbtn)
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
            _meterNumber = (_meterNumber + 1 + 8) % 8; // 0~7を周期するようにする
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
    _ssd->setTextSize(1);
    _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _ssd->setCursor(0, 0);
    // action表示
    _ssd->print("> ");
    _ssd->print(actionName);
    if (!_actionDecided)
    {
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
}

// 　データ取得
int uiMeterNumber() { return _meterNumber; }
bool uiActionDecided() { return _actionDecided; }
bool uiModeDecided() { return _modeDecided; }
bool uiRunning() { return _modeDecided && _actionDecided && _configActive[0]; }
int uiActionNumber() { return _actionNumber; }
int uiModeNumber() { return _modeNumber; }
int uiConfigNumber() { return _configNumber; }
bool uiConfigActive(uint8_t index) { return _configActive[index]; }
