#include "BNO055.hpp"

/*BNO055からのデータをI2Cで入手する*/

Adafruit_BNO055 *bno = nullptr; // とりあえず定義

int bno_reset_pin = 0;                // ジャイロのリセットスイッチのピン番号格納用
int bno_reset_pinmode = INPUT_PULLUP; // ジャイロのリセットスイッチのピンモード格納用

int bno_normal_deg = 0; // BNO055からの生データ格納用
int bno_reset_deg = 0;  // リセットスイッチが押されたときの角度格納用
int bno_deg = 0;        // 採取的な角度格納用

/*ボタンを定義*/
Button bno_reset_button; // BNO055のリセットボタン用

void BNO055_set_resetpin(int pin, int pinmode)
{
    bno_reset_pin = pin;
    bno_reset_pinmode = pinmode;

    bno_reset_button.set_pin(bno_reset_pin, bno_reset_pinmode); // ピンをセット
}

void BNO055_init(TwoWire *wire, uint8_t address)
{
    bno_reset_button.init(); // 設定する

    if (bno != nullptr)
    {
        delete bno;
    }
    bno = new Adafruit_BNO055(BNO055_ID, address, wire); // 新しく作る

    Timer my_bno_timer;
    my_bno_timer.reset(); // タイマーをリセット
    while (!(*bno).begin(OPERATION_MODE_IMUPLUS) && my_bno_timer.get_time() < 5000)
    {
        delay(10); // 10ms待機
    } // BNO055との通信が成功するか5000ms経つかそれまで待つ

    (*bno).setExtCrystalUse(true); // セットする
}

void BNO055_update()
{
    imu::Vector<3> euler = (*bno).getVector(Adafruit_BNO055::VECTOR_EULER); // BNO055からデータを取得

    bno_normal_deg = euler.x(); // X軸のデータを取得

    bno_reset_button.update();          // 更新する
    if (bno_reset_button.is_released()) // ボタンが押されたなら
        bno_reset_deg = bno_normal_deg; // reset_degを更新

    bno_deg = (int)(bno_normal_deg - bno_reset_deg + 360) % 360; // 角度を計算
}

int get_BNO055_deg()
{
    return bno_deg; // 計算したジャイロの角度を返す
}