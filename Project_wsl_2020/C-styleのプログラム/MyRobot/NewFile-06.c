//--------------------------------------------------------------------------------
// Title: Main - [ NewFile-06 ]
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
// Program Name : NewFile-06.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double IR_motermax = 80;
  double IR_edge = 0;
  double moter_LF = 0;
  double moter_RF = 0;
  double IR_arpha = 0.8;
  while (TRUE) {
    if (gAD[CN3] < 409) {
      if (gAD[CN2] > 306) {
        set_Led(1, LED_ON);
        IR_edge = gAD[CN3] * 0.98;
        IR_edge = IR_edge * 3.14 / 180;
        if (IR_edge < 180 * 3.14 / 180) {
          moter_LF = - IR_motermax * cos(IR_edge + IR_arpha + 45 * 3.14 /180);
          moter_RF = - IR_motermax * cos(IR_edge + IR_arpha + 135 * 3.14 /180);
        } else {
          moter_LF = - IR_motermax * cos(IR_edge - IR_arpha + 45 * 3.14 /180);
          moter_RF = - IR_motermax * cos(IR_edge - IR_arpha + 135 * 3.14 /180);
        }
      } else {
        set_Led(1, LED_OFF);
        IR_edge = gAD[CN3] * 0.98;
        IR_edge = IR_edge * 3.14 / 180;
        moter_LF = - IR_motermax * cos(IR_edge + 45 * 3.14 /180);
        moter_RF = - IR_motermax * cos(IR_edge + 135 * 3.14 /180);
      }
      gV[VAR_A] = moter_LF;
      gV[VAR_B] = moter_RF;
      gV[VAR_C] = - moter_LF;
      gV[VAR_D] = - moter_RF;
      gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
      gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
      gPwm[2] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
      gPwm[4] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
      pwm_out();
    } else {
      gPwm[0] = 0x00 | 0x80;
      gPwm[1] = 0x00 | 0x80;
      gPwm[2] = 0x00 | 0x80;
      gPwm[4] = 0x00 | 0x80;
      pwm_out();
    }
  }
}
//--------------------------------------------------------------------------------
