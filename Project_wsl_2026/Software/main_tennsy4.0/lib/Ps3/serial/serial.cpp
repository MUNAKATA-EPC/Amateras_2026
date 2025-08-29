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
    // バッファに1フレーム分以上ある限りループ（6バイトデータ + 1同期ヘッダー = 7バイト）
    while ((*ps3_serial).available() >= 7)
    {
        // --- 同期ズレ対策: ヘッダーが出るまで捨てる ---
        while ((*ps3_serial).available() > 0 && (*ps3_serial).peek() != head_byte)
        {
            (*ps3_serial).read(); // ゴミを捨てる
        }

        if ((*ps3_serial).available() < 7)
            break; // まだ7バイト揃っていなければ抜ける

        if ((*ps3_serial).peek() == head_byte) // ヘッダー確認
        {
            (*ps3_serial).read(); // ヘッダーを捨てる

            // 順に読み取り
            ps3_stick_lx = (int8_t)(*ps3_serial).read();        // 左ステックX (-128~127)
            ps3_stick_ly = (int8_t)(-(*ps3_serial).read() - 1); // 左ステックY
            ps3_stick_rx = (int8_t)(*ps3_serial).read();        // 右ステックX
            ps3_stick_ry = (int8_t)(-(*ps3_serial).read() - 1); // 右ステックY

            uint8_t low = (*ps3_serial).read();                      // ボタン下位バイト
            uint8_t high = (*ps3_serial).read();                     // ボタン上位バイト
            ps3_buttons_data_bit_mask = (uint16_t(high) << 8) | low; // 16bit にまとめる
        }
        else
        {
            (*ps3_serial).read(); // ゴミを捨てる
        }
    }
}

int get_Ps3_stick_lx()
{
    if (abs(ps3_stick_lx) > ps3_stick_lx_adjust)
        return ps3_stick_lx; // 左スティックのx方向を返す

    return 0; // 調整値以下なので0を返す
}

int get_Ps3_stick_ly()
{
    if (abs(ps3_stick_ly) > ps3_stick_ly_adjust)
        return ps3_stick_ly; // 左スティックのy方向を返す

    return 0; // 調整値以下なので0を返す
}

int get_Ps3_stick_rx()
{
    if (abs(ps3_stick_rx) > ps3_stick_rx_adjust)
        return ps3_stick_rx; // 右スティックのx方向を返す

    return 0; // 調整値以下なので0を返す
}

int get_Ps3_stick_ry()
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
