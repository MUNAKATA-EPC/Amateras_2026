#ifndef MYLIB_OPENMV_H

#define MYLIB_OPENMV_H

// カートの角度
extern int Soccer_field_max_deg;

// 黄色ゴール
extern int Yellow_goal_deg;
extern bool Yellow_goal_exist;
extern int Yellow_goal_distance;

// 青ゴール
extern int Blue_goal_deg;
extern bool Blue_goal_exist;
extern int Blue_goal_distance;

/*　SeeeduinoXiaoとの通信関連　*/

// OPENMVとの通信を開始する関数
void OpenMVbegin();
// OPENMVからのデータを入手する関数
void OpenMVget();

#endif