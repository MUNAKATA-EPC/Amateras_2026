#include "bno.hpp"

static Adafruit_BNO055 *_bno = nullptr;
static TwoWire *_wire_ptr = nullptr;
static uint8_t _i2c_addr = 0;

static int _deg_normal = 0;
static int _deg_reset = 0;
static int _deg = 0;

// 監視用変数
static float _last_raw_x = -999.0f;    // 丸める前の値を保持
static uint32_t _last_read_ms = 0UL;   // 前回のI2C読み取り時刻
static uint32_t _last_change_ms = 0UL; // 前回の「値が変化した」時刻

bool bnoInit(TwoWire *wire, uint8_t address)
{
    _wire_ptr = wire;
    _i2c_addr = address;

    if (_bno != nullptr)
    {
        delete _bno;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    // Teensy 4.0は高速なため、BNOの起動を確実に待つ (最大1秒)
    uint32_t start = millis();
    bool success = false;
    while (millis() - start < 1000)
    {
        if (_bno->begin(OPERATION_MODE_IMUPLUS))
        {
            success = true;
            break;
        }
        // 初期化中の短いブロックは許容
        arm_dcache_flush_delete((void *)_bno, sizeof(Adafruit_BNO055)); // Teensy用キャッシュ対策
    }

    if (success)
    {
        _bno->setExtCrystalUse(true);
        _last_change_ms = millis(); // 初期化直後は現在時刻でリセット
    }
    return success;
}

void bnoUpdate(bool resetbtn)
{
    if (_bno == nullptr)
        return;

    uint32_t now = millis();

    // 1. 読み取り頻度の制限 (20ms)
    if (now - _last_read_ms < 20)
        return;
    _last_read_ms = now;

    // 2. データの取得
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    float current_raw_x = (float)euler.x();

    // 3. フリーズ検知 (floatレベルでの比較)
    // 正常なら静止していても 0.0625(1/16度) 単位のノイズが乗るはず
    if (current_raw_x != _last_raw_x)
    {
        _last_change_ms = now;
        _last_raw_x = current_raw_x;
    }
    else
    {
        // 全く同じ値が 1秒間 続いた場合のみ異常とみなす
        if (now - _last_change_ms > 1000)
        {
            bnoInit(_wire_ptr, _i2c_addr);
            _last_change_ms = now; // 再初期化ループ防止
            return;
        }
    }

    // 4. 計算用の int キャストはここで最後に行う
    _deg_normal = (int)current_raw_x;

    if (resetbtn)
    {
        _deg_reset = _deg_normal;
    }

    _deg = (_deg_normal - _deg_reset + 360) % 360;
    if (_deg > 180)
        _deg -= 360;
}

int bnoDeg()
{
    return _deg;
}