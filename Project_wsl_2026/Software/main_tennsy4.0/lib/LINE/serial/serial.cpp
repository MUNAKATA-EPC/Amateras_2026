#include "serial.hpp"

/*LINEセンサーの状況をシリアル通信で受信する*/

HardwareSerial *line_serial; // とりあえず定義
int line_baudrate;           // ボートレート格納用

bool line_exist = false; // ラインがあるかどうか

bool line_data[19] = {0}; // 受信した19個のデータ格納用

void LINEserial_init(HardwareSerial *serial, int baudrate)
{
    line_serial = serial;
    line_baudrate = baudrate;
    (*line_serial).begin(line_baudrate);
    (*line_serial).setTimeout(10);
}

void LINEserial_update()
{
    if ((*line_serial).available() > 0)
    {
        // 情報を10進数で受信->2進数に直す
        unsigned int data_10 = (*line_serial).readStringUntil('\n').toInt(); // 16個(エンジェル)+3個(右・左・後)のデータを受信

        bool line_flag = false; // ラインの反応を見るフラグ
        int shift_num;          // 解析に使う変数
        for (int i = 0; i < 19; i++)
        {
            shift_num = 1 << i; // 0001を左にiだけ移動させる

            if ((shift_num & data_10) > 0) // shift_numとdata_10の論理積が0より大きいならば
            {
                line_flag = true;    // 反応したら旗を立てる
                line_data[i] = true; // i番目のセンサーは反応している
            }
            else
            {
                line_data[i] = false; // i番目のセンサーは反応していない
            }
        }

        if (line_flag)
            line_exist = true; // 存在する
        else
            line_exist = false; // 存在しない
    }
}

int get_LINE_data(int pin)
{
    if (pin < 0 || pin > 18)
        return 0; // 範囲外のデータ請求ならば0を返す
    else
        return line_data[pin]; // pin番目のラインセンサーの状況を返す
}

bool is_LINE_exist()
{
    return line_exist; // ラインが有るかどうかを返す
}