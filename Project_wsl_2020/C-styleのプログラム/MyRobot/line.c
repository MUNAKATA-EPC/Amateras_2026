//--------------------------------------------------------------------------------
// Title: Main - [ line ]
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
// Program Name : line.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double Line_motermax = 80;
  double xx = 0;
  double yy = 0;
  double Line_edge = 0;
  while (TRUE) {
    gV[VAR_L] = gAD[CN4] + 180;
    gV[VAR_L] = gV[VAR_L] % 360;
    Line_edge = gV[VAR_L] * 3.14 / 180;
    if (gAD[CN4] > 409) {
      set_Led(1, LED_ON);
      xx = 70;
      yy = 70;
    } else {
      set_Led(1, LED_OFF);
      xx = cos(Line_edge) * Line_motermax;
      yy = sin(Line_edge) * Line_motermax;
    }
    gV[VAR_X] = xx;
    gV[VAR_Y] = yy;
    gPwm[0] = gV[VAR_Y] < 0 ? (gV[VAR_Y] * -1) | 0x80 : gV[VAR_Y];
    gPwm[1] = gV[VAR_X] < 0 ? (gV[VAR_X] * -1) | 0x80 : gV[VAR_X];
    gPwm[2] = gV[VAR_X] < 0 ? gV[VAR_X] * -1 : gV[VAR_X] | 0x80;
    gPwm[3] = gV[VAR_Y] < 0 ? gV[VAR_Y] * -1 : gV[VAR_Y] | 0x80;
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
