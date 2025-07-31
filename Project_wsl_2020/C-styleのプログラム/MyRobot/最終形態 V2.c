//--------------------------------------------------------------------------------
// Title: Main - [ 最終形態 V2 ]
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
// Program Name : 最終形態 V2.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double IR_motermax = 80;  	// 回り込む際のモータの最大出力
  double IR_moterValue;  	// 回り込む際のモータの最大出力結果
  double IR_frontValue = 10;  	// 前とみなす、角度の範囲（IR）
  double IR_edge = 0;  	// IRの角度
  double IR_alpha_max = 70;  	// 回り込む際のボールに対して何度ずらして動くか（max）
  double IR_alpha_min = 30;  	// 回り込む際のボールに対して何度ずらして動くか（min）
  double IR_alpha = 0;  	// 回り込む際のボールに対して何度ずらして動くか（結果）
  double Gyro_moterValue = 15;  	// ジャイロのモータの最大出力結果
  double Gyro_frontValue = 2.5;  	// 前とみなす、角度の範囲（Gyro）
  double Gyro_edge = 0;  	// ジャイロの角度
  double Line_edge = 0;  	// LINEの角度
  double xx = 0;  	// 最終のモータ出力（X座標）
  double yy = 0;  	// 最終のモータ出力（Y座標）
  while (TRUE) {
    IR_edge = gAD[CN1] * 3.14 / 180;  	// IR角度を読む　　ラジアンに直す
    Gyro_edge = gAD[CN3];  	// ジャイロを読む
    gV[VAR_L] = gAD[CN4] + 180;
    gV[VAR_L] = gV[VAR_L] % 360;
    Line_edge = gV[VAR_L] * 3.14 / 180;  	// LINEを読む　　ラジアンに直す
    if (gAD[CN4] < 400) {
      set_Led(0, LED_OFF);
      set_Led(1, LED_ON);
      xx = cos(Line_edge) * IR_motermax;  	// LINEから離れるには、、（X座標）
      yy = sin(Line_edge) * IR_motermax;  	// LINEから離れるには、、（Y座標）
    } else if (gAD[CN2] < 500) {
      set_Led(0, LED_OFF);
      set_Led(1, LED_OFF);
      xx = cos(IR_edge + (405) * 3.14 / 360 ) * IR_motermax;  	// ボールのほうに行くには、、（X座標）
      yy = sin(IR_edge + (405) * 3.14 / 360 ) * IR_motermax;  	// ボールのほうに行くには、、（Y座標）
    } else if (IR_edge < ( IR_frontValue * 1.2 ) * 3.14 / 180  ||  IR_edge  > ( 360 - IR_frontValue * 1.2 ) * 3.14 / 180 && IR_edge < ( 360 ) * 3.14 /180) {
      set_Led(0, LED_OFF);
      set_Led(1, LED_OFF);
      xx = cos( IR_edge + (405) * 3.14 / 360 ) * IR_motermax;  	// ボールのほうに行くには、、（X座標）
      yy = sin( IR_edge + (405) * 3.14 / 360 ) * IR_motermax;  	// ボールのほうに行くには、、（Y座標）
    } else {
      set_Led(0, LED_ON);
      set_Led(1, LED_OFF);
      IR_moterValue = IR_motermax;
      if (IR_edge > ( IR_frontValue ) * 3.14 / 180  &&  IR_edge < ( 360 - IR_frontValue ) * 3.14 / 180) {
        IR_alpha = ( (IR_alpha_min - IR_alpha_max) / 32400 ) * IR_edge * 180 / 3.14 * IR_edge * 180 / 3.14;
        IR_alpha = IR_alpha - (IR_alpha_min - IR_alpha_max) / 90 * IR_edge * 180 / 3.14 + IR_alpha_min;
        if (IR_edge < (180) * 3.14 / 180) {
          IR_edge += IR_alpha * 3.14 / 180;  	// IR＿alphaだけずらす
        } else {
          IR_edge -= IR_alpha * 3.14 / 180;  	// -IR＿alphaだけずらす
        }
        xx = cos(IR_edge + (405) * 3.14 / 360 ) * IR_moterValue;  	// 回り込むには、、（X座標）
        yy = sin(IR_edge + (405) * 3.14 / 360 ) * IR_moterValue;  	// 回り込むには、、（Y座標）
      } else if (gAD[CN1] > 409) {
        xx = 0;
        yy = 0;
      } else {
        xx = IR_motermax;  	// 直進のためには、、（X座標）
        yy = IR_motermax;  	// 直進のためには、、（Y座標）
      }
    }
    if (Gyro_edge > 50  &&  Gyro_edge < 310 && gAD[CN4] > 400) {
      xx = 0;
      yy = 0;
    }
    if (Gyro_edge < Gyro_frontValue) {
      gV[VAR_G] = 0;
    } else if (Gyro_edge > 360 - Gyro_frontValue) {
      gV[VAR_G] = 0;
    } else if (Gyro_edge < 180) {
      gV[VAR_G] = Gyro_edge / 180 * Gyro_moterValue;
    } else {
      Gyro_edge -= 360;
      gV[VAR_G] = Gyro_edge / 180 * Gyro_moterValue;
    }
    gV[VAR_A] =  - xx + gV[VAR_G];  	// 代入
    gV[VAR_B] = yy + gV[VAR_G];  	// 代入
    gV[VAR_C] =  - yy + gV[VAR_G];  	// 代入
    gV[VAR_D] =  xx + gV[VAR_G];  	// 代入
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
