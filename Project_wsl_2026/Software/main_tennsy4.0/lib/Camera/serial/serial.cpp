#include "serial.hpp"

/*カメラからの情報をシリアル通信で受信する*/

HardwareSerial *openmv_serial; // とりあえず定義
int openmv_baudrate;           // ボートレート格納用

int field_deg = -1;       // コートの角度格納用
bool field_exsit = false; // コートが見えるどうか

int yellow_goal_deg = -1;       // 黄色ゴールの角度格納用
int yellow_goal_distance = -1;  // 黄色ゴールの距離格納用
bool yellow_goal_exsit = false; // 黄色ゴールが見えるかどうか

int blue_goal_deg = -1;       // 青色ゴールの角度格納用
int blue_goal_distance = -1;  // 青色ゴールの距離格納用
bool blue_goal_exsit = false; // 青色ゴールが見えるかどうか

void Camera_init(HardwareSerial *serial, int baudrate)
{
    openmv_serial = serial;
    openmv_baudrate = baudrate;
    (*openmv_serial).begin(openmv_baudrate);
    (*openmv_serial).setTimeout(10);
}

void Camera_update()
{
    if ((*openmv_serial).available() > 0)
    {
        field_deg = (*openmv_serial).readStringUntil('a').toInt();            //'a'まで読む
        yellow_goal_deg = (*openmv_serial).readStringUntil('b').toInt();      //'b'まで読む
        yellow_goal_distance = (*openmv_serial).readStringUntil('c').toInt(); //'c'まで読む
        blue_goal_deg = (*openmv_serial).readStringUntil('d').toInt();        //'d'まで読む
        blue_goal_distance = (*openmv_serial).readStringUntil('e').toInt();   //'e'まで読む

        if (field_deg == -1)
            field_exsit = false; // 存在しない
        else
            field_exsit = true; // 存在する

        if (yellow_goal_deg == -1)
            yellow_goal_exsit = false; // 存在しない
        else
            yellow_goal_exsit = true; // 存在する

        if (blue_goal_deg == -1)
            blue_goal_exsit = false; // 存在しない
        else
            blue_goal_exsit = true; // 存在する
    }
}

bool is_field_exist()
{
    return field_exsit; // コートが見えるかどうかについてを返す
}

int get_field_deg()
{
    return field_deg; // コートの角度を返す
}

bool is_yellow_goal_exist()
{
    return yellow_goal_exsit; // 黄色ゴールが見えるかどうかについて返す
}

int get_yellow_goal_deg()
{
    return yellow_goal_deg; // 黄色ゴールの角度を返す
}

int get_yellow_goal_distance()
{
    return yellow_goal_distance; // 黄色ゴールの距離を返す
}

bool is_blue_goal_exist()
{
    return blue_goal_exsit; // 青色ゴールが見えるかどうかについて返す
}

int get_blue_goal_deg()
{
    return blue_goal_deg; // 青色ゴールの角度を返す
}

int get_blue_goal_distance()
{
    return blue_goal_distance; // 青色ゴールの距離を返す
}