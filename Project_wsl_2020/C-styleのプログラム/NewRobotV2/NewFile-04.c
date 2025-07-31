//--------------------------------------------------------------------------------
// Title: Main - [ NewFile-04 ]
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
// Program Name : NewFile-04.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double IR_motermax = 80;
  double xx = 0;
  double yy = 0;
  double IR_edge = 0;
  while (TRUE) {
    IR_edge = gAD[CN3] - 0;
    IR_edge = IR_edge * 0.98;
    IR_edge = IR_edge * 3.14 / 180;
    if (IR_edge > 450 * 3.14 / 180) {
      xx = 0;
      yy = 0;
    } else {
      xx = cos(IR_edge) * IR_motermax;
      yy = sin(IR_edge) * IR_motermax;
    }
    gV[VAR_X] = xx;
    gV[VAR_Y] = yy;
    gPwm[0] = gV[VAR_Y] < 0 ? (gV[VAR_Y] * -1) | 0x80 : gV[VAR_Y];
    gPwm[1] = gV[VAR_X] < 0 ? (gV[VAR_X] * -1) | 0x80 : gV[VAR_X];
    gPwm[2] = gV[VAR_X] < 0 ? gV[VAR_X] * -1 : gV[VAR_X] | 0x80;
    gPwm[4] = gV[VAR_Y] < 0 ? gV[VAR_Y] * -1 : gV[VAR_Y] | 0x80;
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
