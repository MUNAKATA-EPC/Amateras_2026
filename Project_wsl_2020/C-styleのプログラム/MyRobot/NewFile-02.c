//--------------------------------------------------------------------------------
// Title: Main - [ NewFile-02 ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [Main - [ NewFile-02 ]] - [ Build_V190617 ]
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
// Program Name : NewFile-02.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double Camera_xx;
  double Camera_yy;
  double Camera_motermax = 30;
  double Gyro_moter_max = 20;  	// ジャイロのモータの最大出力結果
  double Gyro_moter_min = 10;  	// ジャイロのモータの最大出力結果
  double Gyro_frontValue = 1;  	// 前とみなす、角度の範囲（Gyro）
  double Gyro_edge = 0;  	// ジャイロの角度
  while (TRUE) {
    i2c_get_pixy();
    Camera_xx = get_pixydat_x(1);
    Camera_yy = get_pixydat_y(1);
    Gyro_edge = gAD[CN3] * 1.1;  	// ジャイロを読む
    if (get_pixydat_index(1) == 0) {
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
    gPwm[0] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    gPwm[1] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    gPwm[2] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    gPwm[3] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
