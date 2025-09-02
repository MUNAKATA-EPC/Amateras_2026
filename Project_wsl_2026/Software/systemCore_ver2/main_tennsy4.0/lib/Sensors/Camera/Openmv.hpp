#pragma once

#include <Arduino.h>

// シリアル定義・開始
void Camera_init(HardwareSerial *serial, int baudrate);

// 更新
void Camera_update();

// コートが存在するかどうか
bool is_field_exist();

// コートの角度を出力
int get_field_deg();

// 黄色ゴールが存在するかどうか
bool is_yellow_goal_exist();

// 黄色ゴールの角度を出力
int get_yellow_goal_deg();

// 黄色ゴールの距離を出力
int get_yellow_goal_distance();

// 青ゴールが存在するかどうか
bool is_blue_goal_exist();

// 青色ゴールの角度を出力
int get_blue_goal_deg();

// 青色ゴールの距離を出力
int get_blue_goal_distance();

#pragma once

#include <Arduino.h>

class Data
{
private:
    bool _detected;
    int _deg;
    double _dis;

public:
    // データの取得
    bool detected() const { return _detected; }
    int deg() const { return _deg; }
    double dis() const { return _dis; }

    //_degと_disをセットする
    void set(int deg, double dis)
    {
        _deg = deg;
        _dis = dis;
        _detected = (deg != -1);
    }
};

class Openmv
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;
    
    // 下位バイトと上位バイトを読み取りつなげる
    int16_t readAndDocking();

public:
    // 関数
    void init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void update();                                                             // 更新・計算

    Data yellowGoal;
    Data blueGoal;
    Data field;
};

extern Openmv cam;
