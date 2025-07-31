//--------------------------------------------------------------------------------
// Title: Main - [ 回り込みV6 ジャイロ改造版 カメラ搭載 ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111130-010002,cmt:
// CN01:Ball
// CN02:Line
// CN03:L-Touch
// CN04:R-Touch
// CN05:C-Touch
// CN06:
// CN07:
// CN08:
// CN09:
// CN10:
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : 回り込みV6 ジャイロ改造版 カメラ搭載.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int MoterSpeed = 80;  	// IRボールを追うときの動く速さ
  int GyroMoterSpeed = 20;  	// ジャイロ補正のときの速さ
  int IRaroundValue = 60;  	// 回り込みの角度
  double RfLb_X;  	// 右前、左後のモータの出力
  double LfRb_Y;  	// 左前、右後のモータの出力
  double IRedge;  	// IRの角度
  double GYROedge;  	// ジャイロの角度
  while (TRUE) {
    if (get_pixydat_index(1) == 0) {
      if (gAD[CN3] > 388) {
        gV[VAR_G] = -GyroMoterSpeed;
      } else if (gAD[CN3] < 347) {
        gV[VAR_G] = GyroMoterSpeed;
      } else {
        gV[VAR_G] = 0;
      }
    } else {
      if (get_pixydat_x(1) > 180) {
        gV[VAR_G] = -GyroMoterSpeed;
      } else if (get_pixydat_x(1) < 140) {
        gV[VAR_G] = GyroMoterSpeed;
      } else {
        gV[VAR_G] = 0;
      }
    }
    motor(-50, -50);
    if (gAD[CN5] < gAD[CN4]  &&  gAD[CN5] < gAD[CN6]) {
      RfLb_X = MoterSpeed;  	// 真っすぐ行く
      LfRb_Y = MoterSpeed;  	// 真っすぐ行く
    } else {
      IRedge = gAD[CN1] * 18/15;  	// IR角度を代入
      if (IRedge > 180) {
        IRedge = gAD[CN1] * 27/29;  	// IR角度を代入
      }
      if (IRedge > 400) {
        RfLb_X = 0;  	// IRボールが見えていないので出力は無し
        LfRb_Y = 0;  	// IRボールが見えていないので出力は無し
      } else if (gAD[CN2] > 306) {
        if (IRedge > 180) {
          gV[VAR_I] = 45 + IRedge + 360 - IRaroundValue;
          gV[VAR_I] = gV[VAR_I] % 360;
          RfLb_X = cos(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・右前、左後　　～左にボールがあるとき～
          LfRb_Y = sin(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・左前、右後　　～左にボールがあるとき～
        } else {
          gV[VAR_I] = 45 + IRedge + IRaroundValue;
          gV[VAR_I] = gV[VAR_I] % 360;
          RfLb_X = cos(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・右前、左後　　～右にボールがあるとき～
          LfRb_Y = sin(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// 回り込むための 速さ を計算・左前、右後　　～右にボールがあるとき～
        }
      } else {
        gV[VAR_I] = 45 + IRedge;
        gV[VAR_I] = gV[VAR_I] % 360;
        RfLb_X = cos(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・右前、左後
        LfRb_Y = sin(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・左前、右後
      }
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
