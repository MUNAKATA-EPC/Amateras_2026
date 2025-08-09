#include "serial.hpp"

/*PS3の状況をシリアル通信で受信する*/

HardwareSerial *ps3_serial; // とりあえず定義
int ps3_baudrate;           // ボートレート格納用

void IR_init(HardwareSerial *serial, int baudrate)
{
    ps3_serial = serial;
    ps3_baudrate = baudrate;
    (*ps3_serial).begin(ps3_baudrate); // ボートレート定義
    (*ps3_serial).setTimeout(10);      // 10msでタイムアウトとする
}

void IR_update()
{
    if ((*ps3_serial).available() > 0)
    {
        // = (*ps3_serial).readStringUntil('a').toInt(); // 'a'まで読む
    }
}
