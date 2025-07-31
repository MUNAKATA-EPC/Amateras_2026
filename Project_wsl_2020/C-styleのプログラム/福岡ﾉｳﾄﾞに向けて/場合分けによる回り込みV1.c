//--------------------------------------------------------------------------------
// Title: Main - [ 場合分けによる回り込みV1 ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-110010-000002,cmt:
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
// Program Name : 場合分けによる回り込みV1.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 50;  	// IRボールを追うときの動く速さ
  const int GyroMoterSpeed = 50;  	// ジャイロ補正のときの速さ
  double RfLb_X;  	// 右前、左後のモータの出力
  double LfRb_Y;  	// 左前、右後のモータの出力
  double GYROedge;
  while (TRUE) {
    if (gAD[CN1] < 92) {
      motor(50, 50);
      set_Led(1, LED_ON);
    } else if (gAD[CN1] < 173) {
      motor(50, 50);
      set_Led(1, LED_OFF);
    } else if (gAD[CN1] < 368) {
      motor(-50, -50);
      set_Led(1, LED_OFF);
    } else if (gAD[CN1] < 470) {
      motor(50, 50);
      set_Led(1, LED_OFF);
    } else if (gAD[CN1] < 511) {
      motor(-50, -50);
      set_Led(1, LED_OFF);
    } else {
    }
    GYROedge = gAD[CN3] * 18 / 17;  	// ジャイロ角度を代入
    if (GYROedge < 180) {
      gV[VAR_G] = (GYROedge) * GyroMoterSpeed / 180;  	// ジャイロ補正の 速さ を計算
    } else {
      gV[VAR_G] = (GYROedge - 360) * GyroMoterSpeed / 180;  	// ジャイロ補正の 速さ を計算
    }
    gV[VAR_A] = - RfLb_X + gV[VAR_G];  	// 最終的な計算結果を代入・左前モータ
    gV[VAR_B] = LfRb_Y + gV[VAR_G];  	// 最終的な計算結果を代入・右前モータ
    gV[VAR_C] = - LfRb_Y + gV[VAR_G];  	// 最終的な計算結果を代入・左後モータ
    gV[VAR_D] = RfLb_X + gV[VAR_G];  	// 最終的な計算結果を代入・右後モータ
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
