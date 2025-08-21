#include "serial.hpp"

/*LINEセンサーの状況をシリアル通信で受信する*/

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

HardwareSerial *line_serial; // とりあえず定義
uint32_t line_baudrate;      // ボートレート格納用

bool line_exist = false; // ラインがあるかどうか

uint32_t lines_data_bit_mask = 0; // 16+3個のラインセンサーの状況格納用

void LINE_init(HardwareSerial *serial, uint32_t baudrate)
{
    line_serial = serial;
    line_baudrate = baudrate;
    (*line_serial).begin(line_baudrate);
}

void LINE_serial_update()
{
    if ((*line_serial).available()) // 受信バッファが溜まっているなら
    {
        if ((*line_serial).peek() == head_byte) // 最初のブッファが同期ヘッダーなら
        {
            if ((*line_serial).available() >= 3 + 1) // seeeduinoxiaoからの3つのデータと1つの同期ヘッダーが溜まっているなら
            {
                (*line_serial).read(); // 同期ヘッダーを捨てる

                uint8_t low = (*line_serial).read();                      // ボタンの下位バイトを読み取る
                uint8_t middle = (*line_serial).read();                   // ボタンの中位バイトを読み取る
                uint8_t high = (*line_serial).read();                     // ボタンの上位バイトを読み取る
                lines_data_bit_mask = (high << 16) | (middle << 8) | low; // 上位バイトと中位バイトと下位バイトをつなげる

                if (lines_data_bit_mask > 0) // 一個でも1があったら0より大きくなる
                    line_exist = true;
                else
                    line_exist = false;
            }
        }
        else // そうでないならゴミのバッファ
        {
            while ((*line_serial).available())
                (*line_serial).read(); // ブッファを捨てる
        }
    }
}

bool get_LINE_data(uint8_t index)
{
    return ((1UL << index) & lines_data_bit_mask) > 0; // index分だけシフトした1との論理積が0よりも大きかったらそのbitは1
    // 1ULはオーバーフロー防止、1と同義
}

bool is_LINE_exist()
{
    return line_exist; // ラインがあるかどうかを返す
}
