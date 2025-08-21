#include "serial.hpp"

/*LINEセンサーの状況をシリアル通信で受信する*/

HardwareSerial *line_serial; // とりあえず定義
uint32_t line_baudrate;      // ボートレート格納用

bool line_exist = false; // ラインがあるかどうか

uint16_t lines_data_bit_mask = 0; // 16個のラインセンサーの状況格納用

void LINE_init(HardwareSerial *serial, uint32_t baudrate)
{
    line_serial = serial;
    line_baudrate = baudrate;
    (*line_serial).begin(line_baudrate);
}

void LINE_serial_update()
{
    if ((*line_serial).available() > 0)
    {
        uint8_t low = (*line_serial).read();     // ボタンの下位バイトを読み取る
        uint8_t high = (*line_serial).read();    // ボタンの上位バイトを読み取る
        lines_data_bit_mask = (high << 8) | low; // 上位バイトと下位バイトをつなげる

        if (lines_data_bit_mask > 0) // 一個でも1があったら0より大きくなる
            line_exist = true;
        else
            line_exist = false;

        while ((*line_serial).available())
            (*line_serial).read(); // 残りのブッファを捨てる
    }
}

bool get_LINE_data(uint8_t index)
{
    return ((1 << index) & lines_data_bit_mask) > 0; // index分だけシフトした1との論理積が0よりも大きかったらそのbitは1
}

bool is_LINE_exist()
{
    return line_exist; // ラインが有るかどうかを返す
}
