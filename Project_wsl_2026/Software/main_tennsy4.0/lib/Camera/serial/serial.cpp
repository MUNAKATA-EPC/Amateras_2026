#include "serial.hpp"

/*カメラからの情報をシリアル通信で受信する*/

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

/*変数定義*/
HardwareSerial *camera_serial; // とりあえず定義
int camera_baudrate;           // ボートレート格納用

int16_t field_deg = -1;   // コートの角度格納用
bool field_exist = false; // コートが見えるどうか

int16_t yellow_goal_deg = -1;      // 黄色ゴールの角度格納用
int16_t yellow_goal_distance = -1; // 黄色ゴールの距離格納用
bool yellow_goal_exist = false;    // 黄色ゴールが見えるかどうか

int16_t blue_goal_deg = -1;      // 青色ゴールの角度格納用
int16_t blue_goal_distance = -1; // 青色ゴールの距離格納用
bool blue_goal_exist = false;    // 青色ゴールが見えるかどうか

void Camera_init(HardwareSerial *serial, int baudrate)
{
    camera_serial = serial;
    camera_baudrate = baudrate;
    (*camera_serial).begin(camera_baudrate);
}

int16_t read_int16_from_serial()
{
    uint8_t low = (*camera_serial).read();
    uint8_t high = (*camera_serial).read();
    return (int16_t)((high << 8) | low); // 上位バイトと下位バイトをつなげる(もともとはint16_tなのでキャストで戻す)
}

void Camera_update()
{
    // バッファに1フレーム分以上ある限りループ（10バイトデータ + 1同期ヘッダー）
    while ((*camera_serial).available() >= 10 + 1) // openmvからの10個のデータと1つの同期ヘッダーが溜まっているなら
    {
        if ((*camera_serial).peek() == head_byte) // 最初のブッファが同期ヘッダーなら
        {
            (*camera_serial).read(); // 同期ヘッダーを捨てる

            field_deg = read_int16_from_serial();            // コートの角度
            yellow_goal_deg = read_int16_from_serial();      // 黄色ゴールの角度
            yellow_goal_distance = read_int16_from_serial(); // 黄色ゴールの距離
            blue_goal_deg = read_int16_from_serial();        // 青ゴールの角度
            blue_goal_distance = read_int16_from_serial();   // 青ゴールの距離

            // それぞれあるかどうか判定
            field_exist = (field_deg != -1);                  // コートが見えるかどうか
            yellow_goal_exist = (yellow_goal_distance != -1); // 黄色ゴールが見えるかどうか
            blue_goal_exist = (blue_goal_distance != -1);     // 青ゴールが見えるかどうか
        }
        else // そうでないならゴミのバッファ
        {
            (*camera_serial).read(); // ブッファを捨てる
        }
    }
}

bool is_field_exist()
{
    return field_exist; // コートが見えるかどうかについてを返す
}

int16_t get_field_deg()
{
    return field_deg; // コートの角度を返す
}

bool is_yellow_goal_exist()
{
    return yellow_goal_exist; // 黄色ゴールが見えるかどうかについて返す
}

int16_t get_yellow_goal_deg()
{
    return yellow_goal_deg; // 黄色ゴールの角度を返す
}

int16_t get_yellow_goal_distance()
{
    return yellow_goal_distance; // 黄色ゴールの距離を返す
}

bool is_blue_goal_exist()
{
    return blue_goal_exist; // 青色ゴールが見えるかどうかについて返す
}

int16_t get_blue_goal_deg()
{
    return blue_goal_deg; // 青色ゴールの角度を返す
}

int16_t get_blue_goal_distance()
{
    return blue_goal_distance; // 青色ゴールの距離を返す
}
