#ifndef MYLIB_SW_H

#define MYLIB_SW_H

// タクトスイッチの状態を格納する変数(長押し対策あり)
extern bool LeftSW, RightSW, EnterSW;
// タクトスイッチの昔の状態を格納する変数
extern bool Old_LeftSW, Old_RightSW, Old_EnterSW;
// タクトスイッチの状態を格納する変数(長押し対策なし)
extern bool LeftSW_true, RightSW_true, EnterSW_true, BNO055resetSW;
// トグルスイッチの状況を格納する変数
extern bool MotorToggle; // モータのトグルスイッチ (1ならばモータON, 0ならばモータOFF)
extern bool CamToggle;   // カメラのトグルスイッチ (1ならば黄色攻め, 0ならば青攻め)

// タクトスイッチを初期化する関数
void SWbegin();
// タクトスイッチの状態を取得する関数
void TactSWget();

#endif