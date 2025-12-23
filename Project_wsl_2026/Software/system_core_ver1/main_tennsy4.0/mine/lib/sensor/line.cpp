#include "line.hpp"

// #define NORMAL

#ifdef NORMAL

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

// 調整時はセンサーの値全てが送られてくる
static bool _is_adjusting = false;
static int _values_to_adjust[16] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};

// エンジェル用変数
static float _x = 0.0f;
static float _y = 0.0f;

static bool _ring_detected = false, _old_ring_detected = false;
static bool _side_detected = false;
static bool _sensor[19] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

static Timer _detecting_timer;
static unsigned long _line_ring_detecting_time = 0UL;

static bool _side_right = false;
static bool _side_left = false;
static bool _side_back = false;

static int _ring_deg = 0xFF, _ring_first_deg = 0xFF;
static int _side_deg = 0xFF;

static float _dis = 0xFF;

static PacketManager packet_to_adjust; // 調整用パケットマネージャー
static PacketManager packet;           // パケットマネージャー

bool lineInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    // 初期化
    _ring_detected = _side_detected = false;
    _ring_deg = _side_deg = 0xFF;
    _dis = 0xFF;
    _side_right = _side_left = _side_back = false;
    _x = _y = 0.0f;

    for (int i = 0; i < 19; i++)
        _sensor[i] = false;

    _serial->begin(_baudrate);

    packet_to_adjust.setup(0x66, 32, 0xBB); // フレームヘッダー、データサイズ、エンドヘッダーを設定
    packet.setup(0x55, 3, 0xAA);            // フレームヘッダー、データサイズ、エンドヘッダーを設定

    Timer timer;
    timer.reset();
    bool success = false;
    while (!success && timer.msTime() < 100UL)
    {
        success = _serial->available() > 0; // 1個以上データが来たら成功しているとみなす
        delay(10);                          // irの通信開始待ち
    }

    return success;
}

void lineUpdate()
{
    _old_ring_detected = _ring_detected; // 前回のフレームのリング検出状態を保存

    // データの受け取り
    int data_count = _serial->available();
    for (int i = 0; i < data_count; i++)
    {
        uint8_t data = _serial->read();
        packet_to_adjust.add(data);
        packet.add(data);

        if (packet_to_adjust.isComplete())
        {
            _is_adjusting = true; // 調整中フラグを立てる

            for (int i = 0; i < 16; i++)
            {
                uint8_t low = packet_to_adjust.get(1 + i * 2);
                uint8_t high = packet_to_adjust.get(2 + i * 2);

                _values_to_adjust[i] = (int16_t((uint16_t(high) << 8) | uint16_t(low))); // 上位バイトと下位バイトをつなげる
            }

            packet.reset();
            packet_to_adjust.reset();
        }

        if (packet.isComplete())
        {
            _is_adjusting = false; // 調整中フラグを下ろす

            uint8_t low = packet.get(1);
            uint8_t middle = packet.get(2);
            uint8_t high = packet.get(3);

            uint32_t bit_mask = ((uint32_t)high << 16) | ((uint32_t)middle << 8) | (uint32_t)low;

            // エンジェルの検出
            _ring_detected = false;

            for (int i = 0; i < 19; i++)
            {
                _sensor[i] = (bit_mask & (1UL << i));
                if (_sensor[i] == true && i < 16)
                    _ring_detected = true;
            }

            // 　サイドの検出
            _side_right = _sensor[16];
            _side_left = _sensor[17];
            _side_back = _sensor[18];

            _side_detected = _side_left || _side_right || _side_back;

            packet.reset();
            packet_to_adjust.reset();
        }
    }

    // 計算
    // エンジェル
    _x = 0.0f; // 初期化
    _y = 0.0f; // 初期化

    if (_ring_detected)
    {
        int ringDetectedCount = 0;
        for (int i = 0; i < 16; i++)
        {
            if (_sensor[i] == true)
            {
                ringDetectedCount++;
                _x += cosf(radians(22.5f * i));
                _y += sinf(radians(22.5f * i));
            }
        }

        if (ringDetectedCount > 0)
        {
            _x = _x * 100.0f / ringDetectedCount;
            _y = _y * 100.0f / ringDetectedCount;
            _dis = sqrtf(_x * _x + _y * _y);

            if (_dis == 0.0f) // 合力が0なら反応したセンサーの位置で補完する
            {
                for (int i = 0; i < 16; i++)
                {
                    if (_sensor[i] == true)
                    {
                        _ring_deg = normalizeDeg(22.5f * i + 90);
                        break;
                    }
                }
            }
            else
            {
                _ring_deg = (int)roundf(degrees(atan2f(_y, _x)));
            }
        }
        else // ringDetectedCount == 0 の場合 (リングセンサーの反応がない)
        {
            _dis = 0xFF;
            _ring_deg = 0xFF;
        }
    }
    else
    {
        _dis = 0xFF;
        _ring_deg = 0xFF;
    }

    // サイドライン
    float side_x = 0.0f, side_y = 0.0f;

    if (_side_detected)
    {
        side_x += (_side_right == true) ? cosf(radians(270.0f)) : 0.0f; // 前が0度としているのでx,yが反転する
        side_x += (_side_left == true) ? cosf(radians(90.0f)) : 0.0f;   // 前が0度としているのでx,yが反転する
        side_x += (_side_back == true) ? cosf(radians(180.0f)) : 0.0f;  // 前が0度としているのでx,yが反転する

        side_y += (_side_right == true) ? sinf(radians(270.0f)) : 0.0f; // 前が0度としているのでx,yが反転する
        side_y += (_side_left == true) ? sinf(radians(90.0f)) : 0.0f;   // 前が0度としているのでx,yが反転する
        side_y += (_side_back == true) ? sinf(radians(180.0f)) : 0.0f;  // 前が0度としているのでx,yが反転する

        _side_deg = (int)round(degrees(atan2f(side_y, side_x)));
    }
    else
    {
        _side_deg = 0xFF;
    }

    // エンジェル初検出時の処理
    if (_ring_detected == true && _old_ring_detected == false)
    {
        _ring_first_deg = _ring_deg;
        _detecting_timer.reset();
    }
    else if (_ring_detected == false)
    {
        _ring_first_deg = 0xFF;
    }
    _line_ring_detecting_time = _detecting_timer.msTime();
}

bool lineIsAdjusting() { return _is_adjusting; }
int lineValueToAdjust(uint8_t index) { return (index >= 0 && index < 16) ? _values_to_adjust[index] : -1; }

bool lineRingDetected() { return _ring_detected; }
bool lineSideDetected() { return _side_detected; }

bool lineSensorDetected(uint8_t index) { return _sensor[index]; }

bool lineSideRightDetected() { return _side_right; }
bool lineSideLeftDetected() { return _side_left; }
bool lineSideBackDetected() { return _side_back; }

int lineRingDeg() { return _ring_deg; }
int lineSideDeg() { return _side_deg; }

float lineRingDis() { return _dis; }

float lineRingX() { return _x; }
float lineRingY() { return _y; }

bool lineRingFirstDetedcted() { return _ring_detected == true && _old_ring_detected == false; }
int lineRingFirstDeg() { return _ring_first_deg; }
unsigned long lineRingDetectingTime() { return _line_ring_detecting_time; }

#else

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

static volatile bool _is_adjusting = false;
static volatile int _values_to_adjust[16] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};

static volatile float _x = 0.0f, _y = 0.0f;
static volatile bool _ring_detected = false, _old_ring_detected = false;
static volatile bool _side_detected = false;
static volatile bool _sensor[19] = {false};

static Timer _detecting_timer;
static volatile unsigned long _line_ring_detecting_time = 0UL;

static volatile bool _side_right = false, _side_left = false, _side_back = false;
static volatile int _ring_deg = 0xFF, _ring_first_deg = 0xFF, _side_deg = 0xFF;
static volatile float _dis = 0xFF;

static PacketManager packet_to_adjust;
static PacketManager packet;
static IntervalTimer _lineTimer; // バックグラウンド処理用

// 内部計算用関数（割り込み内から呼ばれる）
void FASTRUN processLineCalculation()
{
    _old_ring_detected = _ring_detected;

    // エンジェルライン計算
    float tx = 0.0f, ty = 0.0f;
    _ring_detected = false;
    int ringDetectedCount = 0;

    for (int i = 0; i < 16; i++)
    {
        if (_sensor[i])
        {
            _ring_detected = true;
            ringDetectedCount++;

            float angle = (float)i * 22.5f * (PI / 180.0f);
            tx += cosf(angle);
            ty += sinf(angle);
        }
    }

    if (_ring_detected && ringDetectedCount > 0)
    {
        tx = tx * 100.0f / (float)ringDetectedCount;
        ty = ty * 100.0f / (float)ringDetectedCount;
        _x = tx;
        _y = ty;
        _dis = sqrtf(tx * tx + ty * ty);

        if (_dis < 0.01f)
        { // 合力がほぼ0の場合
            for (int i = 0; i < 16; i++)
            {
                if (_sensor[i])
                {
                    _ring_deg = normalizeDeg((float)i * 22.5f + 90.0f);
                    break;
                }
            }
        }
        else
        {
            _ring_deg = (int)roundf(degrees(atan2f(ty, tx)));
        }
    }
    else
    {
        _x = 0.0f;
        _y = 0.0f;
        _dis = 0xFF;
        _ring_deg = 0xFF;
    }

    // サイドライン計算
    _side_right = _sensor[16];
    _side_left = _sensor[17];
    _side_back = _sensor[18];
    _side_detected = _side_left || _side_right || _side_back;

    if (_side_detected)
    {
        float sx = 0.0f, sy = 0.0f;
        if (_side_right)
        {
            sx += cosf(270.0f * PI / 180.0f);
            sy += sinf(270.0f * PI / 180.0f);
        }
        if (_side_left)
        {
            sx += cosf(90.0f * PI / 180.0f);
            sy += sinf(90.0f * PI / 180.0f);
        }
        if (_side_back)
        {
            sx += cosf(180.0f * PI / 180.0f);
            sy += sinf(180.0f * PI / 180.0f);
        }
        _side_deg = (int)roundf(degrees(atan2f(sy, sx)));
    }
    else
    {
        _side_deg = 0xFF;
    }

    // 検出時間と初回検出ロジック
    if (_ring_detected && !_old_ring_detected)
    {
        _ring_first_deg = _ring_deg;
        _detecting_timer.reset();
    }
    else if (!_ring_detected)
    {
        _ring_first_deg = 0xFF;
    }
    _line_ring_detecting_time = _detecting_timer.msTime();
}

// 割り込みハンドラ
void FASTRUN lineBackgroundUpdate()
{
    if (_serial == nullptr)
        return;

    while (_serial->available() > 0)
    {
        uint8_t data = _serial->read();
        packet_to_adjust.add(data);
        packet.add(data);

        if (packet_to_adjust.isComplete())
        {
            _is_adjusting = true;
            for (int i = 0; i < 16; i++)
            {
                uint8_t low = packet_to_adjust.get(1 + i * 2);
                uint8_t high = packet_to_adjust.get(2 + i * 2);
                _values_to_adjust[i] = (int16_t)((uint16_t)high << 8 | low);
            }
            packet.reset();
            packet_to_adjust.reset();
        }

        if (packet.isComplete())
        {
            _is_adjusting = false;
            uint32_t bit_mask = ((uint32_t)packet.get(3) << 16) | ((uint32_t)packet.get(2) << 8) | (uint32_t)packet.get(1);

            for (int i = 0; i < 19; i++)
            {
                _sensor[i] = (bit_mask & (1UL << i));
            }

            processLineCalculation(); // パケットが揃った時だけ計算を実行

            packet.reset();
            packet_to_adjust.reset();
        }
    }
}

bool lineInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;
    _serial->begin(_baudrate);

    packet_to_adjust.setup(0x66, 32, 0xBB);
    packet.setup(0x55, 3, 0xAA);

    _lineTimer.begin(lineBackgroundUpdate, 50000); // 50ms周期で監視

    Timer timer;
    timer.reset();
    while (timer.msTime() < 100UL)
    {
        if (_serial->available() > 0)
            return true;
        delay(5);
    }
    return false;
}

// 互換性のために残す タイマーが自動で行うため呼ぶ必要はない
void lineUpdate() {}

// ラインのデータ取得
bool lineIsAdjusting() { return _is_adjusting; }
int lineValueToAdjust(uint8_t index) { return (index < 16) ? _values_to_adjust[index] : -1; }
bool lineRingDetected() { return _ring_detected; }
bool lineSideDetected() { return _side_detected; }
bool lineSensorDetected(uint8_t index) { return _sensor[index]; }
bool lineSideRightDetected() { return _side_right; }
bool lineSideLeftDetected() { return _side_left; }
bool lineSideBackDetected() { return _side_back; }
int lineRingDeg() { return _ring_deg; }
int lineSideDeg() { return _side_deg; }
float lineRingDis() { return _dis; }
float lineRingX() { return _x; }
float lineRingY() { return _y; }
bool lineRingFirstDetedcted() { return _ring_detected && !_old_ring_detected; }
int lineRingFirstDeg() { return _ring_first_deg; }
unsigned long lineRingDetectingTime() { return _line_ring_detecting_time; }

#endif