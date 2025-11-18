#include "ui.hpp"

static Adafruit_SSD1306 *_ssd = nullptr;

static uint8_t _address;

static bool _actionDecided = false;
static bool _modeDecided = false;

static int _meterNumber = 0;
static int _actionNumber = 0;
static int _modeNumber = 0;

#define CONFIG_DATA_LIMIT 4
static int _configNumber = 0;
static unsigned int _configData[CONFIG_DATA_LIMIT] = {0}; // 0~3個のconfigがある（Runも含む）

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
// 設定のプリント・出力

// メインのuiの描画用
void uiDrawMain(bool enterbtn, bool rightbtn, bool leftbtn)
{
    // 操作
    if (leftbtn)
    {
        if (_modeDecided)
        {
            _modeDecided = false;
            for (int i = 0; i < CONFIG_DATA_LIMIT; i++)
                _configData[i] = 0;
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
            _configNumber = (_configNumber + 1) % CONFIG_DATA_LIMIT; // 0~CONFIG_DATA_LIMIT-1個のconfigがある

            // runは右が押されたらfalseになる
            _configData[0] = false;
        }

        if (enterbtn)
        {
            if (_configNumber == 0)
            {
                _configData[_configNumber] = !_configData[_configNumber]; // これはRunだからスイッチする
            }
            else
            {
                _configData[_configNumber]++; // 加算する
            }
        }
    }

    // 表示
    const char *actionName = nullptr;
    const char *modeName = nullptr;
    const char *configName[3] = {nullptr, nullptr, nullptr};

    switch (_actionNumber)
    {
    case Action::Type::ATTACKER:
        actionName = "Attacker";
        if (_actionDecided)
        {
            switch (_modeNumber)
            {
            case Attacker::Mode::YELLOWGOAL:
                modeName = "YellowGoal";
                break;
            case Attacker::Mode::BLUEGOAL:
                modeName = "BlueGoal";
                break;
            case Attacker::Mode::GYRO:
                modeName = "Gyro";
                break;
            default:
                modeName = "Unknown";
                break;
            }
            configName[0] = "speed";
            configName[1] = "speed";
            configName[2] = "speed";
        }
        break;

    case Action::Type::DEFENDER:
        actionName = "Defender";
        if (_actionDecided)
        {
            switch (_modeNumber)
            {
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
        break;

    default:
        actionName = "Unknown";
        modeName = "Unknown";
        break;
    }

    if (actionName == nullptr)
    {
        actionName = "";
    }
    if (modeName == nullptr)
    {
        modeName = "";
    }
    for (int i = 0; i < 3; i++)
    {
        if (configName[i] == nullptr)
        {
            configName[i] = "";
        }
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
        _ssd->print(" <");
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
            _ssd->setTextSize(2);
            if (_configNumber == 0)
            {
                _ssd->print("|Run:");

                String buf_str = (_configData[0]) ? "on" : "off";
                char buf_char[5];
                buf_str.toCharArray(buf_char, 5);

                _ssd->print(buf_char);
            }
            else
            {
                _ssd->print(" Run:off");

                _ssd->setTextSize(1);
                _ssd->println("");
                for (int i = 0; i < _configNumber; i++)
                {
                    _ssd->println("");
                }
                _ssd->print(" |");
            }
        }
    }
}

// 　データ取得
int uiMeterNumber() { return _meterNumber; }
bool uiActionDecided() { return _actionDecided; }
bool uiModeDecided() { return _modeDecided; }
bool uiRunning() { return _modeDecided && _actionDecided && _configData[0]; }
int uiActionNumber() { return _actionNumber; }
int uiModeNumber() { return _modeNumber; }
int uiConfigNumber() { return _configNumber; }
bool uiConfigData(uint8_t index) { return _configData[index]; }
