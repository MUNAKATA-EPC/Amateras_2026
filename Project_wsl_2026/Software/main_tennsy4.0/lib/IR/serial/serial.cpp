#include "serial.hpp"

/*IRボールの状況をシリアル通信で受信する*/

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

HardwareSerial *ir_serial; // とりあえず定義
uint32_t ir_baudrate;      // ボートレート格納用

bool ir_exist = false; // IRボールがあるかどうか
int ir_deg = -1;       // IRボールの角度格納用
int ir_value = -1;     // IRボールの値格納用

void IR_init(HardwareSerial *serial, uint32_t baudrate)
{
    ir_serial = serial;
    ir_baudrate = baudrate;
    (*ir_serial).begin(ir_baudrate); // ボートレート定義
    (*ir_serial).setTimeout(10);     // 10msでタイムアウト
}

void IR_update()
{
    /*
    // バッファに1フレーム分以上ある限りループ（4バイトデータ + 1同期ヘッダー）
    while ((*ir_serial).available() >= 4 + 1) // seeeduinoxiaoからの4つのデータと1つの同期ヘッダーが溜まっているなら
    {
        if ((*ir_serial).peek() == head_byte) // 最初のブッファが同期ヘッダーなら
        {
            (*ir_serial).read(); // 同期ヘッダーを捨てる

            uint8_t low1 = (*ir_serial).read();                        // ボールの角度の下位バイトを読み取る
            uint8_t high1 = (*ir_serial).read();                       // ボールの角度の上位バイトを読み取る
            ir_deg = int16_t((uint16_t(high1) << 8) | uint16_t(low1)); // 上位バイトと下位バイトをつなげる(もともとはint16_tなのでキャストで戻す)

            uint8_t low2 = (*ir_serial).read();                             // ボールの距離の下位バイトを読み取る
            uint8_t high2 = (*ir_serial).read();                            // ボールの距離の上位バイトを読み取る
            ir_distance = int16_t((uint16_t(high2) << 8) | uint16_t(low2)); // 上位バイトと下位バイトをつなげる(もともとはint16_tなのでキャストで戻す)

            if (ir_deg == -1)
                ir_exist = false;
            else
            {
                ir_exist = true;
            }
        }
        else // そうでないならゴミのバッファ
        {
            (*ir_serial).read(); // ブッファを捨てる
        }
    }
    */
    while ((*ir_serial).available() >= 2 + 2 + 2) // 角度・距離は1バイト以上だけど多めのデータがたまっていたら
    {
        if ((*ir_serial).peek() == 'a') // 最初のブッファが'a'
        {
            (*ir_serial).read(); // 同期ヘッダーを捨てる

            ir_deg = (*ir_serial).readStringUntil('b').toInt(); // bまで読む

            ir_value = (*ir_serial).readStringUntil('c').toInt(); // cまで読む

            if (ir_deg == -1)
                ir_exist = false;
            else
            {
                ir_exist = true;
                ir_deg = (ir_deg - 358 + 360) % 360; // 調整
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
    if (!is_IR_exist())
        return -1;

    return 1023 - ir_value; // IRボールの距離を返す
}

int get_IR_value()
{
    return ir_value; // IRボールの値を返す
}