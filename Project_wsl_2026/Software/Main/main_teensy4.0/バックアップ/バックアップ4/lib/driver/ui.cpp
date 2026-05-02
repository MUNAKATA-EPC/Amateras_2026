#include "ui.hpp"

static Adafruit_SSD1306 *_ssd = nullptr;

static uint8_t _address;

static bool _action_decided = false;
static bool _mode_decided = false;

#define METER_COUNT 10
static bool _meter_flag = false;
static int _meter_number = 0;
static int _action_number = 0;
static int _mode_number = 0;

#define CONFIG_DATA_LIMIT 4
static int _config_number = 0;
static unsigned int _config_data[CONFIG_DATA_LIMIT] = {0U}; // 0~3個のconfigがある（Runも含む）

// 初期化
bool uiInit(TwoWire *wire, uint8_t address, uint8_t width, uint8_t height)
{
    _ssd = new Adafruit_SSD1306(width, height, wire, -1);
    _address = address;

    Timer timer;
    timer.reset();
    bool success = false;
    while (timer.msTime() < 1000UL)
    {
        success = _ssd->begin(SSD1306_SWITCHCAPVCC, _address);
        if (success)
        {
            break;
        }
        else
        {
            delay(50);
        }
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
// ssdプリント用
void ssdPrint(String str)
{
    char buf[20];
    str.toCharArray(buf, 20U);
    _ssd->print(buf);
}
// メータ描画用
void uiDrawCircleMeter(int x0, int y0, int r, const char *s, int deg)
{
    _ssd->drawCircle(x0, y0, r, SSD1306_WHITE);

    int x1 = x0, y1 = y0;
    x1 = x0 - sinf(radians(deg)) * r;
    y1 = y0 - cosf(radians(deg)) * r;
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

// ボタンの更新
void uiButtonUpdate(bool enterbtn, bool backbtn, bool rightbtn, bool leftbtn)
{
    // 操作
    if (backbtn)
    {
        tone(BUZZER_PIN, BuzzerPitches::MyC7, 50);

        if (_mode_decided)
        {
            _mode_decided = false;
            for (int i = 0; i < CONFIG_DATA_LIMIT; i++)
                _config_data[i] = 0;
            _config_number = 0;

            _meter_number = 0; // リセット
        }
        else if (_action_decided)
        {
            _action_decided = false;
            _mode_number = 0;

            _meter_number = 0; // リセット
        }
        else
        {
            _meter_flag = (_meter_flag == true) ? false : true;

            _meter_number = (_meter_number + 1 + METER_COUNT) % METER_COUNT; // 0~(METER_COUNT-1)を周期するようにする
        }
    }

    if (!_action_decided)
    {
        if (rightbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _action_number++;
            if (_action_number > Action::Type::COUNT - 1)
                _action_number = 0;
        }
        if (leftbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _action_number--;
            if (_action_number < 0)
                _action_number = Action::Type::COUNT - 1;
        }

        if (enterbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _action_decided = true;
        }
    }
    else if (!_mode_decided)
    {
        if (rightbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _mode_number++;
            switch (_action_number)
            {
            case Action::Type::ATTACKER:
                if (_mode_number > Attacker::Mode::MODE_COUNT - 1)
                    _mode_number = 0;
                break;
            case Action::Type::DEFENDER:
                if (_mode_number > Defender::Mode::MODE_COUNT - 1)
                    _mode_number = 0;
                break;
            case Action::Type::TEST:
                if (_mode_number > Test::Mode::MODE_COUNT - 1)
                    _mode_number = 0;
                break;
            case Action::Type::RADICON:
                if (_mode_number > Radicon::Mode::MODE_COUNT - 1)
                    _mode_number = 0;
                break;
            }
        }
        if (leftbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _mode_number--;
            switch (_action_number)
            {
            case Action::Type::ATTACKER:
                if (_mode_number < 0)
                    _mode_number = Attacker::Mode::MODE_COUNT - 1;
                break;
            case Action::Type::DEFENDER:
                if (_mode_number < 0)
                    _mode_number = Defender::Mode::MODE_COUNT - 1;
                break;
            case Action::Type::TEST:
                if (_mode_number < 0)
                    _mode_number = Test::Mode::MODE_COUNT - 1;
                break;
            case Action::Type::RADICON:
                if (_mode_number < 0)
                    _mode_number = Radicon::Mode::MODE_COUNT - 1;
                break;
            }
        }

        if (enterbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _mode_decided = true;
        }
    }
    else
    {
        if (rightbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _config_number = (_config_number + 1) % CONFIG_DATA_LIMIT; // 0~CONFIG_DATA_LIMIT-1個のconfigがある

            // runは右が押されたらfalseになる
            _config_data[0] = false;
        }
        if (leftbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            _config_number = (_config_number - 1 + CONFIG_DATA_LIMIT) % CONFIG_DATA_LIMIT; // 0~CONFIG_DATA_LIMIT-1個のconfigがある

            // runは左が押されたらfalseになる
            _config_data[0] = false;
        }

        if (enterbtn)
        {
            tone(BUZZER_PIN, BuzzerPitches::MyE7, 50);

            if (_config_number == 0)
            {
                _config_data[_config_number] = !_config_data[_config_number]; // これはRunだからスイッチする
            }
            else
            {
                _config_data[_config_number]++; // 加算する
            }
        }
    }
}
// メインのuiの描画用
static String actionName = "";
static String modeName = "";
static String configName[3] = {""}; // CONFIG_DATA_LIMIT (4) から Run設定分 (1) を引いた3個のConfig名を表示

void uiDrawMain()
{
    // 表示
    switch (_action_number)
    {
    case Action::Type::ATTACKER:
        actionName = "Attacker";
        if (_action_decided)
        {
            switch (_mode_number)
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
            configName[1] = "p";
            configName[2] = "d";
        }
        break;

    case Action::Type::DEFENDER:
        actionName = "Defender";
        if (_action_decided)
        {
            switch (_mode_number)
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
        if (_action_decided)
        {
            switch (_mode_number)
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
            case Test::Mode::LINE_TRACE:
                modeName = "LineTrace";
                break;
            case Test::Mode::MOTOR:
                modeName = "Motor";
                break;
            default:
                modeName = "Unknown";
                break;
            }
        }
        break;

    case Action::Type::RADICON:
        actionName = "Radicon";
        if (_action_decided)
        {
            switch (_mode_number)
            {
            case Radicon::Mode::RECORD:
                modeName = "Record";
                break;
            case Radicon::Mode::REPLAY:
                modeName = "Replay";
                break;
            case Radicon::Mode::ASOBI:
                modeName = "Asobi";
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

    // 画面描画
    _ssd->setTextSize(1);
    _ssd->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    _ssd->setCursor(0, 0);

    // action表示
    _ssd->print("> ");

    ssdPrint(actionName);

    if (!_action_decided)
    {
        _ssd->print(" <");
    }
    else
    {
        _ssd->println("");

        // mode表示
        _ssd->print(" > ");
        ssdPrint(modeName);
        if (!_mode_decided)
        {
            _ssd->println(" <");
        }
        else
        {
            _ssd->println("");

            // run系のプリント
            String run_str = (_config_number == 0) ? "|" : " ";
            run_str += "Run:";
            run_str += _config_data[0] ? "on" : "off";

            _ssd->setTextSize(2);
            ssdPrint(run_str);
            _ssd->println(""); // Run表示の後に改行を追加

            // config系のプリント
            int maxLen = 0;
            for (int i = 0; i < 3; i++)
            {
                int len = (int)configName[i].length();

                if (len > maxLen)
                    maxLen = len;
            }

            // 文字列の末尾にスペースを詰める処理
            for (int i = 0; i < 3; i++)
            {
                // スペースを詰める幅を決定
                int behindLen = maxLen - configName[i].length();

                for (int ii = 0; ii < behindLen; ii++)
                    configName[i] += " ";
            }

            String config_str = "";
            for (int i = 0; i < 3; i++)
            {
                // 選択されているConfig (i+1) をチェック
                config_str = (_config_number == (i + 1)) ? " |" : "  ";
                // configName[i] (スペース調整済み) とコロン':'を結合
                config_str += configName[i] + ":" + String(_config_data[(i + 1)]) + "\n";

                _ssd->setTextSize(1);
                ssdPrint(config_str);
            }
        }
    }
}

// 　データ取得
int uiMeterNumber() { return _meter_number; }
bool uiActionDecided() { return _action_decided; }
bool uiModeDecided() { return _mode_decided; }
bool uiRunning() { return _mode_decided && _action_decided && _config_data[0]; }
int uiActionNumber() { return _action_number; }
int uiModeNumber() { return _mode_number; }
int uiConfigNumber() { return _config_number; }