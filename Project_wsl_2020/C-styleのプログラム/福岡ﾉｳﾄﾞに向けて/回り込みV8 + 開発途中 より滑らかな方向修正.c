//--------------------------------------------------------------------------------
// Title: Main - [ 回り込みV8 + 開発途中 より滑らかな方向修正 ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111130-010002,cmt:
// CN01:IRedge,LINE左
// CN02:IRdist,LINE右
// CN03:Gyro,LINE後
// CN04:IR1,LINE後
// CN05:IR2
// CN06:IR3,LINE右後
// CN07:　,LINE前
// CN08:　,LINE左後
// CN09:
// CN10:
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : 回り込みV8 + 開発途中 より滑らかな方向修正.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int MoterSpeed = 80;  	// IRボールを追うときの動く速さ
  int GyroMoterSpeed = 20;  	// ジャイロ補正のときの速さ
  int IRaroundValue = 60;  	// 回り込みの角度
  double PIXY_val = 160;  	// PIXYの値
  double RfLb_X;  	// 右前、左後のモータの出力
  double LfRb_Y;  	// 左前、右後のモータの出力
  double IRedge;  	// IRの角度
  double GYROedge;  	// ジャイロの角度
  while (TRUE) {
    if (get_timer(T1) < 400L) {
    } else if (sub_io_get_sensor(1, CN4) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = MoterSpeed;  	// 前に行く
      LfRb_Y = MoterSpeed;  	// 前に行く
    } else if (sub_io_get_sensor(1, CN7) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = -MoterSpeed;  	// 後に行く
      LfRb_Y = -MoterSpeed;  	// 後に行く
    } else if (sub_io_get_sensor(1, CN1) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = MoterSpeed;  	// 右に行く
      LfRb_Y = -MoterSpeed;  	// 右に行く
    } else if (sub_io_get_sensor(1, CN2) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = -MoterSpeed;  	// 左に行く
      LfRb_Y = MoterSpeed;  	// 左に行く
    } else {
      set_Led(0, LED_OFF);
      RfLb_X = 0;  	// 前に行く
      LfRb_Y = 0;  	// 前に行く
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
