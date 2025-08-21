#include "serial.hpp"

/*LINEセンサーの状況をシリアル通信で受信する*/

HardwareSerial *line_serial; // とりあえず定義
int line_baudrate;           // ボートレート格納用

bool line_exist = false; // ラインがあるかどうか

bool line_data[19] = {0}; // 受信した19個のデータ格納用

void LINE_init(HardwareSerial *serial, int baudrate)
{
    line_serial = serial;
    line_baudrate = baudrate;
    (*line_serial).begin(line_baudrate);
    (*line_serial).setTimeout(50);
}

void LINE_serial_update()
{
    if ((*line_serial).available() > 0)
    {
        // 情報を10進数で受信->2進数に直す
        uint32_t data_10 = (*line_serial).readStringUntil('\n').toInt(); // 16個(エンジェル)+3個(右・左・後)のデータを受信

        bool line_flag = false; // ラインの反応を見るフラグ
        uint32_t shift_num;     // 解析に使う変数
        for (uint8_t i = 0; i < 19; i++)
        {
            shift_num = 1UL << i; // 0001を左にiだけ移動させる

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

        line_exist = line_flag; // 存在するかどうか
    }
}

bool get_LINE_data(uint8_t index)
{
    if (index > 18)
        return false; // 範囲外のデータ請求ならばfalseを返す
    else
        return line_data[index]; // index番目のラインセンサーの状況を返す
}

bool is_LINE_exist()
{
    return line_exist; // ラインが有るかどうかを返す
}
