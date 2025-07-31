#ifndef USE_M5PICO_H

#define USE_M5PICO_H

// 起動したときの待ち時間をいくらにするか
extern const int m5pico_TalkStart_ms;

// M5-picoのデータを格納する変数
extern int left_X, left_Y, right_X, right_Y;
extern bool SW_left, SW_right, SW_up, SW_down;
extern bool SW_square, SW_circle, SW_triangle, SW_cross;
extern bool SW_left1, SW_left2, SW_right1, SW_right2;
extern bool SW_select, SW_start;
extern bool SWleftStick_left, SWleftStick_right, SWleftStick_up, SWleftStick_down;
extern bool SWrightStick_left, SWrightStick_right, SWrightStick_up, SWrightStick_down;
// AnalogStickの角度・距離を格納する変数
extern float LStick_Angle, LStick_Dist, RStick_Angle, RStick_Dist;
// AnalogStickの調節用の角度
extern int L_adjust, R_adjust;

// 昔のPs3のデータをいれる変数
extern String Old_Ps3_Data;

// M5-picoとのシリアル通信の開始（初期設定）する関数
void M5PICObegin(int SerialSpeed);

// AnalogStickの0の調整を行う関数->Stick_Adjust(LStickの調整値, RStickの調整値);
void Stick_Adjust(int a, int b);

// M5-picoからのデータを読み取り解析し代入する関数
void M5PICOget();

#endif