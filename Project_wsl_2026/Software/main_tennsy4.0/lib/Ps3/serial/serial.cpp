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
    (*ps3_serial).setTimeout(10);      // 10msでタイムアウトとする
}

void Ps3_serial_update()
{
    if ((*ps3_serial).available() > 0)
    {
        ps3_stick_lx = (*ps3_serial).readStringUntil('a').toInt() - 128; // 'a'まで読み、-128~127にする
        if (abs(ps3_stick_lx) <= ps3_stick_lx_adjust)                    // 調整値以下ならば
            ps3_stick_lx = 0;                                            // 0にする

        ps3_stick_ly = (*ps3_serial).readStringUntil('b').toInt() - 128; // 'b'まで読み、-128~127にする
        if (abs(ps3_stick_ly) <= ps3_stick_ly_adjust)                    // 調整値以下ならば
            ps3_stick_ly = 0;                                            // 0にする

        ps3_stick_rx = (*ps3_serial).readStringUntil('c').toInt() - 128; // 'c'まで読み、-128~127にする
        if (abs(ps3_stick_rx) <= ps3_stick_rx_adjust)                    // 調整値以下ならば
            ps3_stick_rx = 0;                                            // 0にする

        ps3_stick_ry = (*ps3_serial).readStringUntil('d').toInt() - 128; // 'd'まで読み、-128~127にする
        if (abs(ps3_stick_ry) <= ps3_stick_ry_adjust)                    // 調整値以下ならば
            ps3_stick_ry = 0;                                            // 0にする

        unsigned int sw_data_10 = (*ps3_serial).readStringUntil('e').toInt(); // 'e'まで読み、SWの情報を取得(10進数)

        int shift_num = 0; // 解析に使う変数
        for (int i = 0; i < 14; i++)
        {
            shift_num = 1 << i; // 0001を左にiだけ移動させる

            if ((shift_num & sw_data_10) > 0) // shift_numとsw_data_10の論理積が0より大きいならば
            {
                ps3_button_data[i] = true; // このSWは押されている
            }
            else
            {
                ps3_button_data[i] = false; // このSWは押されていない
            }
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
