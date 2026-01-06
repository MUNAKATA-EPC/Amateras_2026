#include "line.hpp"

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
    while (_serial->available() > 0)
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

// 反応の多い部分をチャンクごとに分けて返す
int lineChunkCount(int (&chunk_indexes)[32])
{
    int chunk_count = 0;
    for (int i = 0; i < 32; i++)
        chunk_indexes[i] = -1;

    // 最初の「緑（false）」の位置を探す（ここを起点にすれば跨ぎが起きない）
    int start_offset = -1;
    for (int i = 0; i < 16; i++)
    {
        if (_sensor[i] == false)
        {
            start_offset = i;
            break;
        }
    }

    // すべてtrue（全点灯）の場合の処理
    if (start_offset == -1)
    {
        chunk_indexes[0] = 0;
        chunk_indexes[1] = 15;
        return 1;
    }

    // start_offsetから16個分を順番にチェック
    bool in_chunk = false;
    for (int j = 0; j < 16; j++)
    {
        int idx = (start_offset + j) % 16;

        if (_sensor[idx] == true)
        {
            if (!in_chunk)
            {
                // チャンク開始
                chunk_indexes[chunk_count * 2] = idx;
                in_chunk = true;
            }
            // 常に現在の位置を終了候補として更新
            chunk_indexes[chunk_count * 2 + 1] = idx;
        }
        else
        {
            if (in_chunk)
            {
                // チャンク終了
                chunk_count++;
                in_chunk = false;
                if (chunk_count >= 16)
                    break; // 配列溢れ防止
            }
        }
    }

    // 最後にチャンクの中にいたままループが終わった場合のカウントアップ
    if (in_chunk)
    {
        chunk_count++;
    }

    return chunk_count;
}

// 白線の位置を検出する
LinePosi linePositionCheck()
{
    int line_chunk_indexes[32], line_chunk_count;
    line_chunk_count = lineChunkCount(line_chunk_indexes); // ラインチャンク取得

    if (line_chunk_count == 0)
    {
        return LinePosi::No_line;
    }
    else if (line_chunk_count == 1)
    {
    }
    else if (line_chunk_count == 2)
    {
    }

    return LinePosi::Haji_line;
}