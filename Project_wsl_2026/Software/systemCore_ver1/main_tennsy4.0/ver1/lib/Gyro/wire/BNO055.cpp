#include "BNO055.hpp"

/* BNO055からのデータをI2Cで取得する */

Adafruit_BNO055* bno = nullptr;  // BNO055のインスタンス

int bno_reset_pin = 0;                // リセットスイッチのピン番号
int bno_reset_pinmode = INPUT_PULLUP; // リセットスイッチのピンモード

int bno_normal_deg = 0; // BNO055からの生データ格納用
int bno_reset_deg = 0;  // リセットスイッチが押されたときの角度
int bno_deg = 0;        // 補正後の角度格納用

void BNO055_set_resetpin(int pin, int pinmode)
{
    bno_reset_pin = pin;
    bno_reset_pinmode = pinmode;
    bno_reset_button.set_pin(bno_reset_pin, bno_reset_pinmode); // ピン設定
}

void BNO055_init(TwoWire* wire, uint8_t address)
{
    bno_reset_button.init(); // ボタン初期化

    if (bno != nullptr)
    {
        delete bno;
    }
    bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    Timer my_bno_timer;
    my_bno_timer.reset();
    while (!(*bno).begin(OPERATION_MODE_IMUPLUS) && my_bno_timer.get_time() < 5000)
    {
        delay(10); // BNO055の通信開始待ち
    }

    (*bno).setExtCrystalUse(true); // 外部水晶振動子使用
}

void BNO055_update()
{
    imu::Vector<3> euler = (*bno).getVector(Adafruit_BNO055::VECTOR_EULER);
    bno_normal_deg = euler.x(); // X軸の角度

    bno_reset_button.update();
    if (bno_reset_button.is_released()) // リセットボタンが押されたら
        bno_reset_deg = bno_normal_deg;

    // 補正後の角度 (-180〜180)
    bno_deg = (int)((bno_normal_deg - bno_reset_deg + 360) % 360);
}

int get_BNO055_deg()
{
    return bno_deg;
}
