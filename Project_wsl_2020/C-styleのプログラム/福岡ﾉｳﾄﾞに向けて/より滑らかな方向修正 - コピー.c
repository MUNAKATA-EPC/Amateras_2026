//--------------------------------------------------------------------------------
// Title: Main - [ より滑らかな方向修正 - コピー ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111130-010002,cmt:
// CN01:IRedge,LINE
// CN02:IRdist,LINE
// CN03:Gyro,LINE
// CN04:IR1
// CN05:IR2
// CN06:IR3
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
// Program Name : より滑らかな方向修正 - コピー.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int GyroMoterSpeed = 40;  	// ジャイロ補正のときの速さ
  int PIXY_val = 160;  	// PIXY補正で滑らかに制御するときの範囲（0 ~ 160）
  while (TRUE) {
    if (get_pixydat_index(1) == 0) {
      set_Led(0, LED_OFF);
      set_Led(3, LED_OFF);
      if (gAD[CN3] > 388) {
        gV[VAR_G] = -GyroMoterSpeed;
      } else if (gAD[CN3] < 347) {
        gV[VAR_G] = GyroMoterSpeed;
      } else {
        gV[VAR_G] = 0;
      }
    } else {
      set_Led(0, LED_ON);
      gV[VAR_G] = GyroMoterSpeed * (160 - get_pixydat_x(1)) / PIXY_val;
    }
    gPwm[0] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    gPwm[1] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    gPwm[2] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    gPwm[3] = gV[VAR_G] < 0 ? (gV[VAR_G] * -1) | 0x80 : gV[VAR_G];
    pwm_out();
  }
  if (160 - get_pixydat_x(1) < PIXY_val  &&  160 - get_pixydat_x(1) > -PIXY_val) {
    set_Led(3, LED_ON);
  } else {
    set_Led(3, LED_OFF);
    if (get_pixydat_x(1) > 160) {
      gV[VAR_G] = -GyroMoterSpeed;
    } else {
      gV[VAR_G] = GyroMoterSpeed;
    }
  }
}
//--------------------------------------------------------------------------------
