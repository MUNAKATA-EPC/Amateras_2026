#ifndef USE_CONTROLSW_H

#define USE_CONTROLSW_H

// コントロール基板上のピン番号
extern int ControlSW_right_pin, ControlSW_front_pin, ControlSW_left_pin;
// 今のSWの状況
extern bool ControlSW_right, ControlSW_front, ControlSW_left;
// 昔のSWの状況
extern bool old_ControlSW_right, old_ControlSW_front, old_ControlSW_left;
// 本当のSWの状況
extern bool ControlSW_right_True, ControlSW_front_True, ControlSW_left_True;

// どのピンをコントロール基盤のSWピンとして使うか
void ControlSWbegin(int a, int b, int c);

// SWの計算をする関数
void ControlSWget();

#endif