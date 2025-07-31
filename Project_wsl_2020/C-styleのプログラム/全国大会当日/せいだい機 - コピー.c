//--------------------------------------------------------------------------------
// Title: Main - [ せいだい機 - コピー ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-110030-010002,cmt:
// CN01:前
// CN02:右
// CN03:後
// CN04:左
// CN05:
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
// Program Name : せいだい機 - コピー.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int MoterSpeed = 85;  	// IRボールを追うときの動く速さ
  int GyroMoterSpeed = 15;  	// ジャイロ補正のときの速さ
  int IRaroundValue = 60;  	// 回り込みの角度
  double PIXY_val = 160;  	// PIXYの値
  double RfLb_X;  	// 右前、左後のモータの出力
  double LfRb_Y;  	// 左前、右後のモータの出力
  double IRedge;  	// IRの角度
  double GYROedge;  	// ジャイロの角度
  gV[VAR_P] = 1;
  set_Led(1, LED_ON);
  set_Led(0, LED_OFF);
  while (TRUE) {
    if (gAD[CN8] < 102) {
      gV[VAR_P] = gV[VAR_P] + 1;
      if (gV[VAR_P] > 3) {
        gV[VAR_P] = 1;
      }
      if (gV[VAR_P] == 1) {
        set_Led(1, LED_ON);
        set_Led(0, LED_OFF);
      } else if (gV[VAR_P] == 2) {
        set_Led(1, LED_OFF);
        set_Led(0, LED_ON);
      } else {
        set_Led(1, LED_ON);
        set_Led(0, LED_ON);
      }
      while (gAD[CN8] < 102) {
      }
    }
    if (get_pixydat_index(gV[VAR_P]) > 0  &&  gV[VAR_P] < 3) {
      if (gV[VAR_P] == 1) {
        if (chk_pixydat_p(1, 0x0444)) {
          gV[VAR_G] = -GyroMoterSpeed;
        } else if (chk_pixydat_p(1, 0x0111)) {
          gV[VAR_G] = GyroMoterSpeed;
        } else {
          gV[VAR_G] = 0;
        }
      } else {
        if (chk_pixydat_p(2, 0x0444)) {
          gV[VAR_G] = -GyroMoterSpeed;
        } else if (chk_pixydat_p(2, 0x0111)) {
          gV[VAR_G] = GyroMoterSpeed;
        } else {
          gV[VAR_G] = 0;
        }
      }
    } else {
      if (gAD[CN1] < 370  &&  gAD[CN1] > 5) {
        gV[VAR_G] = -GyroMoterSpeed;
      } else if (gAD[CN1] >= 370  &&  gAD[CN1] < 718) {
        gV[VAR_G] = GyroMoterSpeed;
      } else {
        gV[VAR_G] = 0;
      }
    }
    if (sub_io_get_sensor(1, CN1) < 511) {
      set_Led(2, LED_ON);
      clr_timer(0);
      RfLb_X = -MoterSpeed;  	// 後に行く
      LfRb_Y = -MoterSpeed;  	// 後に行く
    } else if (sub_io_get_sensor(1, CN3) < 511) {
      set_Led(2, LED_ON);
      clr_timer(0);
      RfLb_X = MoterSpeed;  	// 前に行く
      LfRb_Y = MoterSpeed;  	// 前に行く
    } else if (sub_io_get_sensor(1, CN2) < 511) {
      set_Led(2, LED_ON);
      clr_timer(0);
      RfLb_X = -MoterSpeed;  	// 左に行く
      LfRb_Y = MoterSpeed;  	// 左に行く
    } else if (sub_io_get_sensor(1, CN4) < 511) {
      set_Led(2, LED_ON);
      clr_timer(0);
      RfLb_X = MoterSpeed;  	// 右に行く
      LfRb_Y = -MoterSpeed;  	// 右に行く
    } else if (get_timer(T1) < 200L) {
    } else {
      set_Led(2, LED_OFF);
      IRedge = gAD[CN2] * 18/15;  	// IR角度を代入
      if (gAD[CN5] < gAD[CN4]  &&  gAD[CN5] < gAD[CN6]) {
        RfLb_X = MoterSpeed;  	// 真っすぐ行く
        LfRb_Y = MoterSpeed;  	// 真っすぐ行く
      } else {
        if (IRedge > 180) {
          IRedge = gAD[CN2] * 27/29;  	// IR角度を代入
        }
        if (IRedge > 400) {
          set_Led(3, LED_OFF);
          RfLb_X = -MoterSpeed * 2 / 3;  	// IRボールが見えていないので出力は無し
          LfRb_Y = -MoterSpeed * 2/ 3;  	// IRボールが見えていないので出力は無し
        } else if (gAD[CN3] > 300) {
          set_Led(3, LED_ON);
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
          set_Led(3, LED_OFF);
          gV[VAR_I] = 45 + IRedge;
          gV[VAR_I] = gV[VAR_I] % 360;
          RfLb_X = cos(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・右前、左後
          LfRb_Y = sin(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// IRボールの方向へ行くための 速さ を計算・左前、右後
        }
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
