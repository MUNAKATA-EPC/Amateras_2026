#include "line.hpp"

//// プライベート変数・クラス・関数 ////

static HardwareSerial *_serial = nullptr;
static uint32_t _baudrate = 9600;

// 調整時はセンサーの値全てが送られてくる
static bool _is_adjusting = false;
static int _values_to_adjust[16] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};

// エンジェル用変数
static bool _ring_detected = false, _old_ring_detected = false;
static bool _side_detected = false;
static bool _sensor[19] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

static float _ring_x = 0.0f;
static float _ring_y = 0.0f;
static MovementAverage _ave_ring_xy[2] = {MovementAverage(5), MovementAverage(5)};

static Timer _detecting_timer;
static unsigned long _line_ring_detecting_time = 0UL;

static int _ring_deg = 0xFF, _ring_first_deg = 0xFF;
static float _ring_dis = 0xFF;

static int _last_ring_deg = 0xFF, _last_ring_dis = 0xFF; // 最後に計測した角度・距離格納用

static bool _side_right = false;
static bool _side_left = false;
static bool _side_back = false;

static int _side_deg = 0xFF;

static PacketManager packet_to_adjust; // 調整用パケットマネージャー
static PacketManager packet;           // パケットマネージャー

//// パブリック関数 ////

bool lineInit(HardwareSerial *serial, uint32_t baudrate)
{
    _serial = serial;
    _baudrate = baudrate;

    // 初期化
    _ring_detected = _side_detected = false;
    _ring_x = _ring_y = 0.0f;
    _ring_deg = _side_deg = 0xFF;
    _ring_dis = 0xFF;
    _last_ring_deg = _last_ring_dis = 0xFF;
    _side_right = _side_left = _side_back = false;

    for (int i = 0; i < 19; i++)
        _sensor[i] = false;

    _serial->begin(_baudrate);

    packet_to_adjust.setup(0x66, 32, 0xBB); // フレームヘッダー、データサイズ、エンドヘッダーを設定
    packet.setup(0x55, 3, 0xAA);            // フレームヘッダー、データサイズ、エンドヘッダーを設定

    return true;
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
    _ring_x = 0.0f; // 初期化
    _ring_y = 0.0f; // 初期化

    if (_ring_detected)
    {
        int ringDetectedCount = 0;
        for (int i = 0; i < 16; i++)
        {
            if (_sensor[i] == true)
            {
                ringDetectedCount++;
                _ring_x += cosf(radians(22.5f * i));
                _ring_y += sinf(radians(22.5f * i));
            }
        }

        if (ringDetectedCount > 0)
        {
            _ring_x = _ave_ring_xy[0].add(_ring_x * 100.0f / ringDetectedCount);
            _ring_y = _ave_ring_xy[1].add(_ring_y * 100.0f / ringDetectedCount);
            _ring_dis = sqrtf(_ring_x * _ring_x + _ring_y * _ring_y);

            if (_ring_dis == 0.0f) // 合力が0なら反応したセンサーの位置で補完する
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
                _ring_deg = (int)roundf(degrees(atan2f(_ring_y, _ring_x)));
            }

            _last_ring_deg = _ring_deg;
            _last_ring_dis = _ring_dis;
        }
        else // ringDetectedCount == 0 の場合 (リングセンサーの反応がない)
        {
            _ave_ring_xy[0].reset();
            _ave_ring_xy[1].reset();

            _ring_dis = 0xFF;
            _ring_deg = 0xFF;
        }
    }
    else
    {
        _ave_ring_xy[0].reset();
        _ave_ring_xy[1].reset();

        _ring_dis = 0xFF;
        _ring_deg = 0xFF;
    }

    // サイドライン
    float side_ring_x = 0.0f, side_ring_y = 0.0f;

    if (_side_detected)
    {
        side_ring_x += (_side_right == true) ? cosf(radians(270.0f)) : 0.0f; // 前が0度としているのでx,yが反転する
        side_ring_x += (_side_left == true) ? cosf(radians(90.0f)) : 0.0f;   // 前が0度としているのでx,yが反転する
        side_ring_x += (_side_back == true) ? cosf(radians(180.0f)) : 0.0f;  // 前が0度としているのでx,yが反転する

        side_ring_y += (_side_right == true) ? sinf(radians(270.0f)) : 0.0f; // 前が0度としているのでx,yが反転する
        side_ring_y += (_side_left == true) ? sinf(radians(90.0f)) : 0.0f;   // 前が0度としているのでx,yが反転する
        side_ring_y += (_side_back == true) ? sinf(radians(180.0f)) : 0.0f;  // 前が0度としているのでx,yが反転する

        _side_deg = (int)roundf(degrees(atan2f(side_ring_y, side_ring_x)));
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

float lineRingDis() { return _ring_dis; }

float lineRingX() { return _ring_x; }
float lineRingY() { return _ring_y; }

bool lineRingFirstDetedcted() { return _ring_detected == true && _old_ring_detected == false; }
int lineRingFirstDeg() { return _ring_first_deg; }
unsigned long lineRingDetectingTime() { return _line_ring_detecting_time; }

int lineLastRingDeg() { return _last_ring_deg; }
float lineLastRingDis() { return _last_ring_dis; }

int lineChunkCount(LineChunk (&chunk)[16])
{
    // 初期化
    for (int i = 0; i < 16; i++)
        chunk[i] = {-1, -1, 0, 0xFF};

    if (!_ring_detected)
        return 0;

    int chunk_count = 0; // 何個チャンクがあったかの計測
    int chunk_next_index = 0;
    bool in_chunk = false; // 現在チャンクの中をカウント中かどうかのフラグ

    for (int i = 0; i < 16; i++)
    {
        if (_sensor[i] == true)
        {
            if (!in_chunk) // チャンクの開始
            {
                chunk[chunk_next_index].start_index = i;
                in_chunk = true;
                chunk_next_index++;
                chunk_count++;
            }
            else
            {
                chunk[chunk_next_index - 1].length++; // lengthとはstart_index = 1,end_index = 3の時1〇2〇3で2となる
            }
        }
        else
        {
            if (in_chunk) // チャンクの終了
            {
                chunk[chunk_next_index - 1].end_index = i - 1;
                in_chunk = false;
            }
        }
    }

    // 配列の最後が true のまま終わった場合
    if (in_chunk)
    {
        chunk[chunk_next_index - 1].end_index = 15;
    }

    // 例えばインデックス0から2　と　インデックス13から15　の2チャンクの場合、結合し1チャンクにする処理
    if (chunk_count > 1 && chunk[0].start_index == 0 && chunk[chunk_count - 1].end_index == 15)
    {
        int new_start_index = chunk[chunk_count - 1].start_index;
        int new_end_index = chunk[0].end_index;

        chunk[0].start_index = new_start_index;
        chunk[0].end_index = new_end_index;
        chunk[0].length = (new_end_index - new_start_index + 16) % 16;

        chunk[chunk_count - 1] = {-1, -1, 0, 0xFF};

        chunk_count--;
    }

    for (int i = 0; i < chunk_count; i++) // チャンクの角度を計算
    {
        float middle_index = fmodf(chunk[i].start_index + 0.5f * chunk[i].length, 16.0f); // 中間のインデックスを求める
        chunk[i].deg = normalizeDeg((int)roundf(middle_index * 22.5f));
    }

    return chunk_count; // 見つかったチャンク数を返す
}