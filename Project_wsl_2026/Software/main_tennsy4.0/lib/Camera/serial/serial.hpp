#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <Arduino.h>

// シリアル定義・開始
void Camera_init(HardwareSerial *serial, int baudrate);

// 更新
void Camera_update();

// コートが存在するかどうか
bool is_field_exist();

// コートの角度を出力
int16_t get_field_deg();

// 黄色ゴールが存在するかどうか
bool is_yellow_goal_exist();

// 黄色ゴールの角度を出力
int16_t get_yellow_goal_deg();

// 黄色ゴールの距離を出力
int16_t get_yellow_goal_distance();

// 青ゴールが存在するかどうか
bool is_blue_goal_exist();

// 青色ゴールの角度を出力
int16_t get_blue_goal_deg();

// 青色ゴールの距離を出力
int16_t get_blue_goal_distance();

#endif
