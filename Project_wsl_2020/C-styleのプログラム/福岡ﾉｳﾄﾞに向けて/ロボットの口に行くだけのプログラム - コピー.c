//--------------------------------------------------------------------------------
// Title: Main - [ ロボットの口に行くだけのプログラム - コピー ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-110030-000002,cmt:
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
// Program Name : ロボットの口に行くだけのプログラム - コピー.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 50;  	// IRボールを追うときの動く速さ
  const int GyroMoterSpeed = 50;  	// ジャイロ補正のときの速さ
  const int IRaroundValue = 70;  	// 回り込みの角度
  double RfLb_X;  	// 右前、左後のモータの出力
  double LfRb_Y;  	// 左前、右後のモータの出力
  double IRedge;  	// IRの角度
  double GYROedge;  	// ジャイロの角度
  while (TRUE) {
    IRedge = gAD[CN1] * 18 / 17;  	// IR角度を代入
    if (gAD[CN1] > 409) {
      RfLb_X = 0;  	// IRボールが見えていないので出力は無し
      LfRb_Y = 0;  	// IRボールが見えていないので出力は無し
    } else {
      if (IRedge > 180) {
        RfLb_X = cos((45 + IRedge + 360 - IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・右前、左後　　～左にボールがあるとき～
        LfRb_Y = sin((45 + IRedge + 360 - IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・左前、右後　　～左にボールがあるとき～
      } else {
        RfLb_X = cos((45 + IRedge + IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・右前、左後　　～右にボールがあるとき～
        LfRb_Y = sin((45 + IRedge + IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・左前、右後　　～右にボールがあるとき～
      }
    }
    gV[VAR_G] = 0;  	// ジャイロ補正の 速さ を計算
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
