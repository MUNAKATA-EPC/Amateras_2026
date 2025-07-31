//--------------------------------------------------------------------------------
// Title: Main - [ onigiri機体　V9からジャイロ ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111130-010002,cmt:
// CN01:IRedge,LINE左
// CN02:IRdist,LINE右
// CN03:Gyro,LINE後
// CN04:IR1,LINE左前
// CN05:IR2
// CN06:IR3,LINE右後
// CN07:　,LINE右前
// CN08:　,LINE左後
// CN09:
// CN10:
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : onigiri機体　V9からジャイロ.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int MoterSpeed = 0;  	// IRボールを追うときの動く速さ
  int GyroMoterSpeed = 100;  	// ジャイロ補正のときの速さ
  int IRaroundValue = 60;  	// 回り込みの角度
  double PIXY_val = 160;  	// PIXYの値
  double RfLb_X = 0;  	// 右前、左後のモータの出力
  double LfRb_Y = 0;  	// 左前、右後のモータの出力
  double IRedge;  	// IRの角度
  double GYROedge;  	// ジャイロの角度
  while (TRUE) {
    if (gAD[CN3] > 388) {
      gV[VAR_G] = -GyroMoterSpeed;
    } else if (gAD[CN3] < 347) {
      gV[VAR_G] = GyroMoterSpeed;
    } else {
      gV[VAR_G] = 0;
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
