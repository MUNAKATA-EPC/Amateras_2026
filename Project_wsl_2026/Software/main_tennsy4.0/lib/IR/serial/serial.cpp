#include "serial.hpp"

/*IRボールの状況をシリアル通信で受信する*/

HardwareSerial *ir_serial; // とりあえず定義
int ir_baudrate;           // ボートレート格納用

bool ir_exist = false; // IRボールがあるかどうか

int ir_deg = -1;      // IRボールの角度格納用
int ir_distance = -1; // IRボールの距離格納用

void IRserial_init(HardwareSerial *serial, int baudrate)
{
    ir_serial = serial;
    ir_baudrate = baudrate;
    (*ir_serial).begin(ir_baudrate); // ボートレート定義
    (*ir_serial).setTimeout(10);     // 10msでタイムアウトとする
}

void IRserial_update()
{
    if ((*ir_serial).available() > 0)
    {
        ir_deg = (*ir_serial).readStringUntil('a').toInt();      // 'a'まで読む
        ir_distance = (*ir_serial).readStringUntil('b').toInt(); //'b'まで読む

        if (ir_deg == -1)
            ir_exist = false; // 存在しない
        else
        {
            ir_exist = true; // 存在する

            ir_deg = (ir_deg - 359 + 360) % 360; // 調整
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
