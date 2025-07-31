//--------------------------------------------------------------------------------
// Title: Main - [ 回り込みV3 + ラインセンサー ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111110-010002,cmt:
// CN01:
// CN02:
// CN03:
// CN04:
// CN05:
// CN06:
// CN07:左後
// CN08:右前
// CN09:右後
// CN10:左前
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : 回り込みV3 + ラインセンサー.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 80;  	// IRボールを追うときの動く速さ
  const int GyroMoterSpeed = 15;  	// ジャイロ補正のときの速さ
  const int IRaroundValue = 60;  	// 回り込みの角度
  double RfLb_X;  	// 右前、左後のモータの出力
  double LfRb_Y;  	// 左前、右後のモータの出力
  double IRedge;  	// IRの角度
  double GYROedge;  	// ジャイロの角度
  double GYRORESETedge;  	// ジャイロの角度
  while (TRUE) {
    set_Led(1, LED_OFF);
    if (gAD[CN5] < 511) {
      gPwm[0] = 0x00 | 0x80;
      gPwm[1] = 0x00 | 0x80;
      gPwm[2] = 0x00 | 0x80;
      gPwm[3] = 0x00 | 0x80;
      pwm_out();
      if (gAD[CN7] < 500  ||  gAD[CN8] < 500  ||  gAD[CN9] < 500  ||  gAD[CN10] < 500) {
        motor(50, 50);
      } else {
        motor(-50, -50);
      }
    } else {
      if (gAD[CN8] < 511) {
        motor(50, 50);
        gV[VAR_A] = MoterSpeed;  	// 最終的な計算結果を代入・左前モータ
        gV[VAR_B] = 0;  	// 最終的な計算結果を代入・右前モータ
        gV[VAR_C] = 0;  	// 最終的な計算結果を代入・左後モータ
        gV[VAR_D] = - MoterSpeed;  	// 最終的な計算結果を代入・右後モータ
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else if (gAD[CN10] < 511) {
        motor(50, 50);
        gV[VAR_A] = 0;  	// 最終的な計算結果を代入・左前モータ
        gV[VAR_B] = - MoterSpeed;  	// 最終的な計算結果を代入・右前モータ
        gV[VAR_C] = MoterSpeed;  	// 最終的な計算結果を代入・左後モータ
        gV[VAR_D] = 0;  	// 最終的な計算結果を代入・右後モータ
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else if (gAD[CN7] < 511) {
        motor(50, 50);
        gV[VAR_A] = -MoterSpeed;  	// 最終的な計算結果を代入・左前モータ
        gV[VAR_B] = 0;  	// 最終的な計算結果を代入・右前モータ
        gV[VAR_C] = 0;  	// 最終的な計算結果を代入・左後モータ
        gV[VAR_D] = MoterSpeed;  	// 最終的な計算結果を代入・右後モータ
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else if (gAD[CN9] < 511) {
        motor(50, 50);
        gV[VAR_A] = 0;  	// 最終的な計算結果を代入・左前モータ
        gV[VAR_B] = MoterSpeed;  	// 最終的な計算結果を代入・右前モータ
        gV[VAR_C] = - MoterSpeed;  	// 最終的な計算結果を代入・左後モータ
        gV[VAR_D] = 0;  	// 最終的な計算結果を代入・右後モータ
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else {
        motor(-50, -50);
        if (gAD[CN1] < 10) {
          RfLb_X = MoterSpeed;  	// 真っすぐ行く
          LfRb_Y = MoterSpeed;  	// 真っすぐ行く
        } else {
          IRedge = gAD[CN1] * 18 / 17;  	// IR角度を代入
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
    if (gAD[CN4] < 511) {
      GYRORESETedge = gAD[CN6] * 18 / 17;
    }
    gV[VAR_G] = gAD[CN6] * 18 / 17 - GYRORESETedge + 360;  	// ジャイロ角度を代入
    gV[VAR_G] = gV[VAR_G] % 360;
    GYROedge = gV[VAR_G];
    if (GYROedge < 180) {
      gV[VAR_G] = GyroMoterSpeed;  	// ジャイロ補正の 速さ を計算
    } else {
      gV[VAR_G] = -GyroMoterSpeed;  	// ジャイロ補正の 速さ を計算
    }
  }
}
//--------------------------------------------------------------------------------
