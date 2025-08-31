#ifndef MYLIB_M5STAMPPS3_H

#define MYLIB_M5STAMPPS3_H

// それぞれのコントローラの情報が格納される変数
extern int Left_X, Left_Y, Right_X, Right_Y;
extern bool SW_up, SW_down, SW_right, SW_left;
extern bool SW_triangle, SW_cross, SW_circle, SW_square;
extern bool SW_left1, SW_left2, SW_right1, SW_right2;
extern bool SWleftStick_left, SWleftStick_right, SWleftStick_up, SWleftStick_down;
extern bool SWrightStick_left, SWrightStick_right, SWrightStick_up, SWrightStick_down;
// AnalogStickの角度・距離を格納する変数
extern float LStick_Angle, LStick_Dist, RStick_Angle, RStick_Dist;
// AnalogStickの調節用の角度
extern int L_adjust, R_adjust;
// PS3からのデータを格納する変数
extern String M5STAMPps3data;

// M5STAMPps3との通信を開始する関数
void M5STAMPps3begin();
// M5STAMPps3からのデータを入手する関数
void M5STAMPps3get();

#endif