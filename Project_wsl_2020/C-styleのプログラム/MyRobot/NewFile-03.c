//--------------------------------------------------------------------------------
// Title: Main - [ NewFile-03 ]
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
// Program Name : NewFile-03.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double Gyro_edge = 0;
  double Gyro_motermax = 80;
  double moter_right = 0;
  double moter_left = 0;
  while (TRUE) {
    Gyro_edge = gAD[CN3] * 9 / 8;
    if (Gyro_edge < 25) {
      Gyro_edge = 25;
    } else if (Gyro_edge > 335) {
      Gyro_edge = -25;
    } else if (Gyro_edge > 180) {
      Gyro_edge -= 360;
    }
    Gyro_edge = Gyro_edge / 180 * Gyro_motermax;
    gV[VAR_G] = Gyro_edge;
    gV[VAR_A] = - moter_left + gV[VAR_G];
    gV[VAR_D] = moter_left + gV[VAR_G];
    gV[VAR_B] = moter_right + gV[VAR_G];
    gV[VAR_C] = - moter_right + gV[VAR_G];
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
