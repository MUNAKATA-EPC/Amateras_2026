#include "serial.hpp"

/*PS3の状況をシリアル通信で受信する*/

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

HardwareSerial *ps3_serial; // とりあえず定義
uint32_t ps3_baudrate;      // ボートレート格納用

int8_t ps3_stick_lx = 0;        // PS3の左スティックのX方向格納用
int8_t ps3_stick_lx_adjust = 0; // PS3の左スティックの調整値格納用

int8_t ps3_stick_ly = 0;        // PS3の左スティックのY方向格納用
int8_t ps3_stick_ly_adjust = 0; // PS3の左スティックのY方向調整値格納用

int8_t ps3_stick_rx = 0;        // PS3の右スティックのX方向格納用
int8_t ps3_stick_rx_adjust = 0; // PS3の右スティックのx方向調整値格納用

int8_t ps3_stick_ry = 0;        // PS3の右スティックのY方向格納用
int8_t ps3_stick_ry_adjust = 0; // PS3の右スティックのY方向調整値格納用

uint16_t ps3_buttons_data_bit_mask = 0; // 14個のボタンの状況格納用

void Ps3_set_stick_adjust(int8_t lx_adjust, int8_t ly_adjust, int8_t rx_adjust, int8_t ry_adjust)
{
    ps3_stick_lx_adjust = lx_adjust; // 左スティックのX方向の調整値を設定
    ps3_stick_ly_adjust = ly_adjust; // 左スティックのY方向の調整値を設定
    ps3_stick_rx_adjust = rx_adjust; // 右スティックのX方向の調整値を設定
    ps3_stick_ry_adjust = ry_adjust; // 右スティックのY方向の調整値を設定
}

void Ps3_init(HardwareSerial *serial, uint32_t baudrate)
{
    ps3_serial = serial;
    ps3_baudrate = baudrate;
    (*ps3_serial).begin(ps3_baudrate); // ボートレート定義
}

void Ps3_serial_update()
{
    if ((*ps3_serial).available()) // 受信バッファが溜まっているなら
    {
        if ((*ps3_serial).peek() == head_byte) // 最初のブッファが同期ヘッダーなら
        {
            if ((*ps3_serial).available() >= 6 + 1) // m5stamp-picoからの6つのデータと1つの同期ヘッダーが溜まっているなら
            {
                (*ps3_serial).read(); // 同期ヘッダーを捨てる

                ps3_stick_lx = (*ps3_serial).read(); // 左ステックのx座標を読み取る
                ps3_stick_ly = (*ps3_serial).read(); // 左ステックのy座標を読み取る
                ps3_stick_rx = (*ps3_serial).read(); // 右ステックのx座標を読み取る
                ps3_stick_ry = (*ps3_serial).read(); // 右ステックのy座標を読み取る

                uint8_t low = (*ps3_serial).read();             // ボタンの下位バイトを読み取る
                uint8_t high = (*ps3_serial).read();            // ボタンの上位バイトを読み取る
                ps3_buttons_data_bit_mask = (high << 8) | low; // 上位バイトと下位バイトをつなげる
            }
        }
        else // そうでないならゴミのバッファ
        {
            while ((*ps3_serial).available())
                (*ps3_serial).read(); // ブッファを捨てる
        }
    }
}

int8_t get_Ps3_stick_lx()
{
    if (abs(ps3_stick_lx) > ps3_stick_lx_adjust)
        return ps3_stick_lx; // 左スティックのx方向を返す

    return 0; // 調整値以下なので0を返す
}

int8_t get_Ps3_stick_ly()
{
    if (abs(ps3_stick_ly) > ps3_stick_ly_adjust)
        return ps3_stick_ly; // 左スティックのy方向を返す

    return 0; // 調整値以下なので0を返す
}

int8_t get_Ps3_stick_rx()
{
    if (abs(ps3_stick_rx) > ps3_stick_rx_adjust)
        return ps3_stick_rx; // 右スティックのx方向を返す

    return 0; // 調整値以下なので0を返す
}

int8_t get_Ps3_stick_ry()
{
    if (abs(ps3_stick_ry) > ps3_stick_ry_adjust)
        return ps3_stick_ry; // 右スティックのy方向を返す

    return 0; // 調整値以下なので0を返す
}

bool get_Ps3_button_data(uint8_t index)
{
    return ((1 << index) & ps3_buttons_data_bit_mask) > 0; // index分だけシフトした1との論理積が0よりも大きかったらそのbitは1
}

bool is_Ps3_stick_left_move()
{
    return (get_Ps3_stick_lx() != 0 || get_Ps3_stick_ly() != 0); // 左ステックが触られたかどうかを返す
}

bool is_Ps3_stick_right_move()
{
    return (get_Ps3_stick_rx() != 0 || get_Ps3_stick_ry() != 0); // 右ステックが触られたかどうかを返す
}
