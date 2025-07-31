//--------------------------------------------------------------------------------
// Title: Main - [ ラインから離れれるプログラム ]
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
// Program Name : ラインから離れれるプログラム.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 70;  	// IRボールを追うときの動く速さ
  const int GyroMoterSpeed = 30;  	// ジャイロ補正のときの速さ
  double RfLb_X;  	// 右前、左後のモータの出力
  double LfRb_Y;  	// 左前、右後のモータの出力
  double IRedge;  	// IRの角度
  double LINEedge;  	// IRの角度
  double GYROedge;  	// ジャイロの角度
  while (TRUE) {
    IRedge = gAD[CN1];  	// IR角度を代入
    LINEedge = gAD[CN4] + 180;  	// IR角度を代入
    if (gAD[CN4] > 409) {
      motor(-50, -50);
      if (IRedge > 400) {
        RfLb_X = 0;  	// IRボールが見えていないので出力は無し
        LfRb_Y = 0;  	// IRボールが見えていないので出力は無し
      } else {
        RfLb_X = cos((45 + IRedge) * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・右前、左後
        LfRb_Y = sin((45 + IRedge) * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・左前、右後
      }
    } else {
      motor(50, 50);
      RfLb_X = cos((45 + LINEedge) * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・右前、左後
      LfRb_Y = sin((45 + LINEedge) * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・左前、右後
    }
    if (gAD[CN5] < 511) {
      RfLb_X = 0;  	// IRボールが見えていないので出力は無し
      LfRb_Y = 0;  	// IRボールが見えていないので出力は無し
    }
    GYROedge = gAD[CN7];  	// ジャイロ角度を代入
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
