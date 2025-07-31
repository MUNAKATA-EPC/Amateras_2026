//--------------------------------------------------------------------------------
// Title: Main - [ NewFile-07 ]
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
// Program Name : NewFile-07.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  gV[VAR_A] = 0;
  while (TRUE) {
    gV[VAR_A] = 80;
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[2] = gV[VAR_A] < 0 ? gV[VAR_A] * -1 : gV[VAR_A] | 0x80;
    gPwm[4] = gV[VAR_A] < 0 ? gV[VAR_A] * -1 : gV[VAR_A] | 0x80;
    pwm_out();
    wait_ms(3000);
    gV[VAR_A] = -80;
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[2] = gV[VAR_A] < 0 ? gV[VAR_A] * -1 : gV[VAR_A] | 0x80;
    gPwm[4] = gV[VAR_A] < 0 ? gV[VAR_A] * -1 : gV[VAR_A] | 0x80;
    pwm_out();
    wait_ms(3000);
  }
}
//--------------------------------------------------------------------------------
