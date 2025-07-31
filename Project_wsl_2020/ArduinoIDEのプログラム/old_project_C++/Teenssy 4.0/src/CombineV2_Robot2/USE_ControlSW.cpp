#include <Arduino.h>
#include "USE_ControlSW.h"

// コントロール基板上のピン番号
int ControlSW_right_pin, ControlSW_front_pin, ControlSW_left_pin;
// 今のSWの状況
bool ControlSW_right = 0, ControlSW_front = 0, ControlSW_left = 0;
// 前のSWの状況
bool old_ControlSW_right = 0, old_ControlSW_front = 0, old_ControlSW_left = 0;
// 本当のSWの状況
bool ControlSW_right_True = 0, ControlSW_front_True = 0, ControlSW_left_True = 0;

void ControlSWbegin(int a, int b, int c)
{
    // それぞれの変数に格納
    ControlSW_right_pin = a;
    ControlSW_front_pin = b;
    ControlSW_left_pin = c;
}

void ControlSWget()
{
    // 昔の状況を保存
    old_ControlSW_right = ControlSW_right;
    old_ControlSW_front = ControlSW_front;
    old_ControlSW_left = ControlSW_left;
    // 今の状況を保存
    ControlSW_right = digitalRead(ControlSW_right_pin);
    ControlSW_right_True = ControlSW_right;
    if (old_ControlSW_right && ControlSW_right)
        ControlSW_right = 0;
    ControlSW_front = digitalRead(ControlSW_front_pin);
    ControlSW_front_True = ControlSW_front;
    if (old_ControlSW_front && ControlSW_front)
        ControlSW_front = 0;
    ControlSW_left = digitalRead(ControlSW_left_pin);
    ControlSW_left_True = ControlSW_left;
    if (old_ControlSW_left && ControlSW_left)
        ControlSW_left = 0;
}