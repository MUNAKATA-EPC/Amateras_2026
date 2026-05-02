#include "serial.hpp"

/*IRボールの状況をシリアル通信で受信する*/

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

HardwareSerial *ir_serial; // とりあえず定義
uint32_t ir_baudrate;      // ボートレート格納用

bool ir_exist = false;    // IRボールがあるかどうか
int16_t ir_deg = -1;      // IRボールの角度格納用
int16_t ir_distance = -1; // IRボールの距離格納用
int16_t ir_value = -1;    // IRボールの値格納用

void IR_init(HardwareSerial *serial, uint32_t baudrate)
{
    ir_serial = serial;
    ir_baudrate = baudrate;
    (*ir_serial).begin(ir_baudrate); // ボートレート定義
    // (*ir_serial).setTimeout(10);
}

void IR_update()
{
    // バッファに1フレーム分以上ある限りループ（4バイトデータ + 1同期ヘッダー = 5バイト）
    while ((*ir_serial).available() >= 5) // seeeduinoxiaoからの4つのデータと1つの同期ヘッダーが溜まっているなら
    {
        // --- 同期ズレ対策: ヘッダー(0xAA)が出るまでゴミを捨てる ---
        while ((*ir_serial).available() > 0 && (*ir_serial).peek() != head_byte)
        {
            (*ir_serial).read(); // ゴミを捨てる
        }

        if ((*ir_serial).available() < 5)
            break; // まだ5バイト揃っていなければ抜ける

        if ((*ir_serial).peek() == head_byte) // 最初のブッファが同期ヘッダーなら
        {
            (*ir_serial).read(); // 同期ヘッダーを捨てる

            uint8_t low1 = (*ir_serial).read();                        // ボールの角度の下位バイトを読み取る
            uint8_t high1 = (*ir_serial).read();                       // ボールの角度の上位バイトを読み取る
            ir_deg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる

            uint8_t low2 = (*ir_serial).read();                          // ボールの値の下位バイトを読み取る
            uint8_t high2 = (*ir_serial).read();                         // ボールの値の上位バイトを読み取る
            ir_value = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる

            if (ir_deg == -1)
            {
                ir_exist = false;

                ir_distance = -1;
            }
            else
            {
                ir_exist = true;

                ir_distance = map(constrain(ir_value, 0, 300), 0, 300, 300, 0); // 0~300を300~0にするそれを距離とする
            }
        }
        else // そうでないならゴミのバッファ
        {
            (*ir_serial).read(); // ブッファを捨てる
        }
    }
}

bool is_IR_exist()
{
    return ir_exist; // IRボールが存在するかどうかについてを返す
}

int get_IR_deg()
{
    return ir_deg; // IRボールの角度を返す
}

int get_IR_distance()
{
    return ir_distance; // IRボールの距離を返す
}

int get_IR_value()
{
    return ir_value; // IRボールの値を返す
}
