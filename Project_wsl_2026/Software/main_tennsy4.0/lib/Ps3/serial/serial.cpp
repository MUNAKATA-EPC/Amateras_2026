#include "serial.hpp"

/*PS3の状況をシリアル通信で受信する*/

HardwareSerial *ps3_serial; // とりあえず定義
int ps3_baudrate;           // ボートレート格納用

int ps3_stick_lx = 0;        // PS3の左スティックのX方向格納用
int ps3_stick_lx_adjust = 0; // PS3の左スティックの調整値格納用

int ps3_stick_ly = 0;        // PS3の左スティックのY方向格納用
int ps3_stick_ly_adjust = 0; // PS3の左スティックのY方向調整値格納用

int ps3_stick_rx = 0;        // PS3の右スティックのX方向格納用
int ps3_stick_rx_adjust = 0; // PS3の右スティックのx方向調整値格納用

int ps3_stick_ry = 0;        // PS3の右スティックのY方向格納用
int ps3_stick_ry_adjust = 0; // PS3の右スティックのY方向調整値格納用

bool ps3_button_data[14] = {false}; // PS3のbuttonの情報格納用

void Ps3_set_stick_adjust(int lx_adjust, int ly_adjust, int rx_adjust, int ry_adjust)
{
    ps3_stick_lx_adjust = lx_adjust; // 左スティックのX方向の調整値を設定
    ps3_stick_ly_adjust = ly_adjust; // 左スティックのY方向の調整値を設定
    ps3_stick_rx_adjust = rx_adjust; // 右スティックのX方向の調整値を設定
    ps3_stick_ry_adjust = ry_adjust; // 右スティックのY方向の調整値を設定
}

void Ps3_init(HardwareSerial *serial, int baudrate)
{
    ps3_serial = serial;
    ps3_baudrate = baudrate;
    (*ps3_serial).begin(ps3_baudrate); // ボートレート定義
    (*ps3_serial).setTimeout(50);      // 50msでタイムアウトとする
}

void Ps3_serial_update()
{
    if ((*ps3_serial).available() > 0)
    {
        // 左スティックのX方向を受信（-128~127にする）
        int received_data_a = (*ps3_serial).readStringUntil('a').toInt() - 128;
        if (received_data_a >= -128 && received_data_a <= 127) // 範囲外なら無視
        {
            ps3_stick_lx = received_data_a;               // 左スティックX方向に代入
            if (abs(ps3_stick_lx) <= ps3_stick_lx_adjust) // 調整値以下ならば
                ps3_stick_lx = 0;                         // 0にする
        }

        // 左スティックのY方向を受信（-127~128にする）
        int received_data_b = -((*ps3_serial).readStringUntil('b').toInt() - 128);
        if (received_data_b >= -127 && received_data_b <= 128) // 範囲外なら無視
        {
            ps3_stick_ly = received_data_b;               // 左スティックY方向に代入
            if (abs(ps3_stick_ly) <= ps3_stick_ly_adjust) // 調整値以下ならば
                ps3_stick_ly = 0;                         // 0にする
        }

        // 右スティックのX方向を受信（-128~127にする）
        int received_data_c = (*ps3_serial).readStringUntil('c').toInt() - 128;
        if (received_data_c >= -128 && received_data_c <= 127) // 範囲外なら無視
        {
            ps3_stick_rx = received_data_c;               // 右スティックX方向に代入
            if (abs(ps3_stick_rx) <= ps3_stick_rx_adjust) // 調整値以下ならば
                ps3_stick_rx = 0;                         // 0にする
        }

        // 右スティックのY方向を受信（-127~128にする）
        int received_data_d = -((*ps3_serial).readStringUntil('d').toInt() - 128);
        if (received_data_d >= -127 && received_data_d <= 128) // 範囲外なら無視
        {
            ps3_stick_ry = received_data_d;               // 右スティックY方向に代入
            if (abs(ps3_stick_ry) <= ps3_stick_ry_adjust) // 調整値以下ならば
                ps3_stick_ry = 0;                         // 0にする
        }

        // ボタンの情報を受信（10進数）
        unsigned int sw_data_10 = (*ps3_serial).readStringUntil('e').toInt();

        // ボタンをbit解析して格納
        for (int i = 0; i < 14; i++)
        {
            int shift_num = 1 << i; // 0001を左にiだけ移動
            if ((shift_num & sw_data_10) > 0)
                ps3_button_data[i] = true; // このSWは押されている
            else
                ps3_button_data[i] = false; // このSWは押されていない
        }
    }
}

int get_Ps3_stick_lx()
{
    return ps3_stick_lx; // 左ステックのX方向を返す
}

int get_Ps3_stick_ly()
{
    return ps3_stick_ly; // 左ステックのY方向を返す
}

int get_Ps3_stick_rx()
{
    return ps3_stick_rx; // 右ステックのX方向を返す
}

int get_Ps3_stick_ry()
{
    return ps3_stick_ry; // 右ステックのY方向を返す
}

int get_Ps3_button_data(int index)
{
    if (index < 0 || index > 13)
        return 0; // 範囲外のデータ請求ならば0を返す
    else
        return ps3_button_data[index]; // index番目のSWの状況を返す
}

bool is_Ps3_stick_left_move()
{
    return (ps3_stick_lx != 0 || ps3_stick_ly != 0); // 左ステックが触られたかどうかを返す
}

bool is_Ps3_stick_right_move()
{
    return (ps3_stick_rx != 0 || ps3_stick_ry != 0); // 右ステックが触られたかどうかを返す
}
