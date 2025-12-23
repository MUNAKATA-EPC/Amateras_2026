#include "bno.hpp"

// #define NORMAL

#ifdef NORMAL

static Adafruit_BNO055 *_bno = nullptr;

static int _deg_normal = 0; // BNO055からの生角度
static int _deg_reset = 0;  // リセット時の角度
static int _deg = 0;        // 補正後の角度格納用

bool bnoInit(TwoWire *wire, uint8_t address)
{
    if (_bno != nullptr)
    {
        delete _bno;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    Timer timer;
    timer.reset();
    bool success = _bno->begin(OPERATION_MODE_IMUPLUS);
    while (!success && timer.msTime() < 100UL)
    {
        delay(10); // BNO055の通信開始待ち
    }
    _bno->setExtCrystalUse(true); // 外部水晶振動子使用

    return success;
}

void bnoUpdate(bool resetbtn)
{
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    _deg_normal = (int)euler.x(); // X軸の角度

    if (resetbtn) // リセットボタンが押されたら
        _deg_reset = _deg_normal;

    // 補正後の角度
    _deg = (int)((_deg_normal - _deg_reset + 360) % 360);

    _deg = (_deg > 180) ? _deg - 360 : _deg; // -180 ~ 180に変換
}

int bnoDeg() { return _deg; }

#else

#include "bno.hpp"

static Adafruit_BNO055 *_bno = nullptr;

// 割り込みで更新されるため volatile を付与
static volatile int _deg_normal = 0; // BNO055からの生角度
static volatile int _deg_reset = 0;  // リセット時の角度
static volatile int _deg = 0;        // 補正後の角度格納用

static IntervalTimer _bnoTimer;

// 割り込みハンドラ：バックグラウンドで角度を取得し計算
void FASTRUN bnoBackgroundUpdate()
{
    if (_bno == nullptr)
        return;

    // オブジェクトからオイラー角を取得
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    int current_normal = (int)euler.x(); // X軸の角度（生値）

    _deg_normal = current_normal;

    // 補正計算（-180 ~ 180の範囲に正規化）
    int diff = current_normal - _deg_reset;
    int corrected = (diff + 360) % 360;

    if (corrected > 180)
    {
        corrected -= 360;
    }

    _deg = corrected;
}

bool bnoInit(TwoWire *wire, uint8_t address)
{
    if (_bno != nullptr)
    {
        delete _bno;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    Timer timer;
    timer.reset();
    bool success = _bno->begin(OPERATION_MODE_IMUPLUS);
    while (!success && timer.msTime() < 100UL)
    {
        delay(10); // BNO055の通信開始待ち
        success = _bno->begin(OPERATION_MODE_IMUPLUS);
    }

    if (success)
    {
        _bno->setExtCrystalUse(true); // 外部水晶振動子使用

        // 10ms (10000us) 間隔で更新。BNO055の内部更新レートに対して適切。
        _bnoTimer.begin(bnoBackgroundUpdate, 10000);
    }

    return success;
}

// 引数をフラグとして使い、リセット処理を行う
void bnoUpdate(bool resetbtn)
{
    if (resetbtn)
    {
        // 現在の生角度をリセット基準点として記録
        _deg_reset = _deg_normal;
    }
}

int bnoDeg() { return _deg; }

#endif
