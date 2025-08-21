#include "serial.hpp"

/*カメラからの情報をシリアル通信で受信する*/

/*変数定義*/
HardwareSerial *camera_serial; // とりあえず定義
uint32_t camera_baudrate;      // ボートレート格納用

int16_t field_deg = -1;       // コートの角度格納用
bool field_exsit = false;     // コートが見えるどうか

int16_t yellow_goal_deg = -1;       // 黄色ゴールの角度格納用
int16_t yellow_goal_distance = -1;  // 黄色ゴールの距離格納用
bool yellow_goal_exsit = false;     // 黄色ゴールが見えるかどうか

int16_t blue_goal_deg = -1;       // 青色ゴールの角度格納用
int16_t blue_goal_distance = -1;  // 青色ゴールの距離格納用
bool blue_goal_exsit = false;     // 青色ゴールが見えるかどうか

void Camera_init(HardwareSerial *serial, uint32_t baudrate)
{
    camera_serial = serial;
    camera_baudrate = baudrate;
    camera_serial->begin(camera_baudrate);
    camera_serial->setTimeout(50);
}

void Camera_update()
{
    if (camera_serial->available() > 0)
    {
        field_deg = camera_serial->readStringUntil('a').toInt();            //'a'まで読む
        yellow_goal_deg = camera_serial->readStringUntil('b').toInt();      //'b'まで読む
        yellow_goal_distance = camera_serial->readStringUntil('c').toInt(); //'c'まで読む
        blue_goal_deg = camera_serial->readStringUntil('d').toInt();        //'d'まで読む
        blue_goal_distance = camera_serial->readStringUntil('e').toInt();   //'e'まで読む

        field_exsit = (field_deg != -1);
        yellow_goal_exsit = (yellow_goal_deg != -1);
        blue_goal_exsit = (blue_goal_deg != -1);
    }
}

bool is_field_exist()
{
    return field_exsit; // コートが見えるかどうかについてを返す
}

int16_t get_field_deg()
{
    return field_deg; // コートの角度を返す
}

bool is_yellow_goal_exist()
{
    return yellow_goal_exsit; // 黄色ゴールが見えるかどうかについて返す
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
    return blue_goal_exsit; // 青色ゴールが見えるかどうかについて返す
}

int16_t get_blue_goal_deg()
{
    return blue_goal_deg; // 青色ゴールの角度を返す
}

int16_t get_blue_goal_distance()
{
    return blue_goal_distance; // 青色ゴールの距離を返す
}
