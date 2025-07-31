//--------------------------------------------------------------------------------
// Title: Main - [ カメラ入り最終形態 ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-110010-010002,cmt:
// CN01:Ball
// CN02:Line
// CN03:L-Touch
// CN04:R-Touch
// CN05:C-Touch
// CN06:
// CN07:
// CN08:
// CN09:RED3
// CN10:RED2
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : カメラ入り最終形態.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double IR_motermax = 65;  	// 回り込む際のモータの最大出力結果
  double IR_frontValue = 20;  	// 前とみなす、角度の範囲（IR）
  double IR_front_number = 1.5;  	// ボールが前に近しいところにいるときの角度を微調整
  double IR_edge_move = 0;  	// IRの角度
  double IR_edge_true = 0;  	// IRの角度
  double IR_edgeAdjust = 0;  	// IRの角度の調整用　０～３６０にする用
  double IR_edgeCentralValue = 0;  	// IRの角度の調整用　０～３６０にする用
  double IR_alpha_max = 70;  	// 回り込む際のボールに対して何度ずらして動くか（max）
  double IR_alpha_min = 60;  	// 回り込む際のボールに対して何度ずらして動くか（min）
  double IR_alpha = 0;  	// 回り込む際のボールに対して何度ずらして動くか（結果）
  double Gyro_moter_max = 15;  	// ジャイロのモータの最大出力結果
  double Gyro_moter_min = 13;  	// ジャイロのモータの最大出力結果
  double Gyro_frontValue = 1;  	// 前とみなす、角度の範囲（Gyro）
  double Gyro_edge = 0;  	// ジャイロの角度
  double Camera_xx;
  double Camera_yy;
  double Camera_motermax = 15;
  int Goal_number = 0;  	// 1が黄ゴール、2が青ゴール
  double Line_time = 400;  	// ラインを踏んでから何秒以上たっていたらブレーキするか
  double Line_timer = 0;  	// ライン制御で使うタイマー
  double Line_timerAdd = Line_time;  	// ラインセンサーで使うタイマーの付属品
  double Line_edge = 0;  	// LINEの角度
  double xx = 0;  	// 最終のモータ出力（X座標）
  double yy = 0;  	// 最終のモータ出力（Y座標）
  clr_timer(0);
  while (TRUE) {
    Line_timer = gT[T1] + Line_timerAdd;  	// ラインタイマーの計算
    gV[VAR_I] = (gAD[CN1] - IR_edgeAdjust - (IR_edgeCentralValue - IR_edgeAdjust)) + 360;  	// IR角度の調整
    gV[VAR_I] = gV[VAR_I] % 360;
    IR_edge_true = gV[VAR_I] * 1.1;  	// IR角度を読む　　度数のまま
    gV[VAR_I] = (gAD[CN1] - IR_edgeAdjust) + 360 - 45;  	// IR角度の調整
    gV[VAR_I] = gV[VAR_I] % 360;
    IR_edge_move = gV[VAR_I] * 3.14 / 180 * 1.1;  	// IR角度を読む　　ラジアンに直す
    Gyro_edge = gAD[CN3] * 1.1;  	// ジャイロを読む
    i2c_get_pixy();
    if (gAD[CN5] < 511) {
      Goal_number = 1;
    } else {
      Goal_number = 2;
    }
    Camera_xx = get_pixydat_x(Goal_number);
    Camera_yy = get_pixydat_y(Goal_number);
    Gyro_edge = gAD[CN3] * 1.1;  	// ジャイロを読む
    gV[VAR_L] = gAD[CN4] + 180;
    gV[VAR_L] = gV[VAR_L] % 360;
    Line_edge = gV[VAR_L] * 3.14 / 180 * 1.1;  	// LINEを読む　　ラジアンに直す
    if (gAD[CN4] < 400) {
      motor(50, 50);
      if (Line_timer > Line_time) {
        Line_timerAdd = 0;  	// ラインタイマー付属品の調整
        gPwm[0] = 0x00 | 0x80;
        gPwm[1] = 0x00 | 0x80;
        gPwm[2] = 0x00 | 0x80;
        gPwm[3] = 0x00 | 0x80;
        pwm_out();
        clr_timer(0);
      }
      xx = cos(Line_edge) * IR_motermax;  	// LINEから離れるには、、（X座標）
      yy = sin(Line_edge) * IR_motermax;  	// LINEから離れるには、、（Y座標）
    } else if (Gyro_edge > 50  &&  Gyro_edge < 310  &&  get_pixydat_index(Goal_number) == 0) {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ラインタイマー付属品の調整
      xx = 0;  	// 0
      yy = 0;  	// 0
    } else if (gAD[CN2] == 500) {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ラインタイマー付属品の調整
      xx = cos(IR_edge_move) * IR_motermax;  	// ボールのほうに行くには、、（X座標）
      yy = sin(IR_edge_move) * IR_motermax;  	// ボールのほうに行くには、、（Y座標）
    } else if (IR_edge_true < IR_frontValue * IR_front_number  ||  IR_edge_true  > (360 - IR_frontValue) * IR_front_number) {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ラインタイマー付属品の調整
      if (IR_edge_true < IR_frontValue  ||  IR_edge_true  > 360 - IR_frontValue) {
        xx = IR_motermax;  	// 直進のためには、、（X座標）
        yy = - IR_motermax;  	// 直進のためには、、（Y座標）
      } else {
        xx = cos(IR_edge_move) * IR_motermax;  	// ボールのほうに行くには、、（X座標）
        yy = sin(IR_edge_move) * IR_motermax;  	// ボールのほうに行くには、、（Y座標）
      }
    } else {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ラインタイマー付属品の調整
      IR_alpha = ( (IR_alpha_min - IR_alpha_max) / 32400 ) * IR_edge_true * IR_edge_true;
      IR_alpha = IR_alpha - (IR_alpha_min - IR_alpha_max) / 90 * IR_edge_true + IR_alpha_min;
      if (IR_edge_true < 180) {
        IR_edge_move += IR_alpha * 3.14 / 180;  	// IR＿alphaだけずらす
      } else {
        IR_edge_move -= IR_alpha * 3.14 / 180;  	// -IR＿alphaだけずらす
      }
      IR_edge_move= IR_edge_move * 180 / 3.14 + 360;
      gV[VAR_J] = IR_edge_move;  	// ０～３６０度に直す作業
      gV[VAR_J] = gV[VAR_J] % 360;
      IR_edge_move = gV[VAR_J] * 3.14 / 180;  	// ０～３６０度に直す作業
      xx = cos(IR_edge_move) * IR_motermax;  	// 回り込むには、、（X座標）
      yy = sin(IR_edge_move) * IR_motermax;  	// 回り込むには、、（Y座標）
    }
    if (get_pixydat_index(Goal_number) == 0) {
      if (Gyro_edge < Gyro_frontValue) {
        gV[VAR_G] = 0;
      } else if (Gyro_edge > 360 - Gyro_frontValue) {
        gV[VAR_G] = 0;
      } else if (Gyro_edge < 180) {
        gV[VAR_G] = ( (Gyro_moter_min - Gyro_moter_max) / 32400 ) * Gyro_edge * Gyro_edge;
        gV[VAR_G] = gV[VAR_G] - (Gyro_moter_min - Gyro_moter_max) / 90 * Gyro_edge + Gyro_moter_min;
      } else {
        gV[VAR_G] = ( (Gyro_moter_min - Gyro_moter_max) / 32400 ) * Gyro_edge * Gyro_edge;
        gV[VAR_G] = gV[VAR_G] - (Gyro_moter_min - Gyro_moter_max) / 90 * Gyro_edge + Gyro_moter_min;
        gV[VAR_G] = - gV[VAR_G];
      }
    } else if (Camera_xx < 160) {
      gV[VAR_G] = Camera_motermax / 25600 * Camera_xx * Camera_xx - Camera_motermax / 80 * Camera_xx + Camera_motermax;
    } else {
      gV[VAR_G] = - Camera_motermax / 25600 * Camera_xx * Camera_xx + Camera_motermax / 80 * Camera_xx - Camera_motermax;
    }
    gV[VAR_A] =  yy + gV[VAR_G];  	// 代入
    gV[VAR_B] = xx + gV[VAR_G];  	// 代入
    gV[VAR_C] =  - xx + gV[VAR_G];  	// 代入
    gV[VAR_D] =  -yy + gV[VAR_G];  	// 代入
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
