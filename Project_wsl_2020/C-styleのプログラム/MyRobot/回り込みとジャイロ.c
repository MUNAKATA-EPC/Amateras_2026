//--------------------------------------------------------------------------------
// Title: Main - [ 回り込みとジャイロ ]
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
// Program Name : 回り込みとジャイロ.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double max_moter = 100;  	// モーター最大出力
  double IR_edge = 0;  	// IRの角度
  double IR_frontValue = 3.5;  	// 前とみなす、角度の範囲
  double IR_motermax = 80;  	// 回り込む際のモータの最大出力
  double IR_motermin = 50;  	// 回り込む際のモータの最低出力
  double IR_moterValue;  	// 回り込む際のモータの出力結果
  double IR_alpha = 40;  	// 回り込む際のボールに対して何度ずらして動くか
  double xx = 0;  	// 最終のモータ出力（X座標）
  double yy = 0;  	// 最終のモータ出力（Y座標）
  while (TRUE) {
    if (gAD[CN2] < 511) {
      set_Led(0, LED_OFF);
      IR_edge = gAD[CN3] * 0.98 + 405;  	// ４５度左にずらす
      IR_edge = IR_edge * 3.14 / 180;  	// ラジアンに直す
      xx = cos(IR_edge) * IR_motermax;  	// ボールのほうに行くには、、（X座標）
      yy = sin(IR_edge) * IR_motermax;  	// ボールのほうに行くには、、（Y座標）
    } else {
      set_Led(0, LED_ON);
      if (gAD[CN3] * 0.98 > IR_frontValue  &&  gAD[CN3] * 0.98 < 360 - IR_frontValue) {
        IR_moterValue = ( (IR_motermin - IR_motermax) / (IR_frontValue * IR_frontValue + 32400 - 360 * IR_frontValue) ) * gAD[CN3] * gAD[CN3];
        IR_moterValue = IR_moterValue + ( (360 * IR_motermax - 360 * IR_motermin) / (IR_frontValue * IR_frontValue + 32400 - 360 * IR_frontValue) ) * gAD[CN3];
        IR_moterValue = IR_moterValue + IR_motermax +  ( (32400 * IR_motermin - 32400 * IR_motermax) / (IR_frontValue * IR_frontValue + 32400 - 360 * IR_frontValue) );
        if (gAD[CN3] * 0.98 < 180) {
          IR_edge = gAD[CN3] * 0.98 + 405 + IR_alpha;  	// ４５度＋IR＿alphaだけずらす
          IR_edge = IR_edge * 3.14 / 180;  	// ラジアンに直す
        } else {
          IR_edge = gAD[CN3] * 0.98 + 405 - IR_alpha;  	// ４５度-IR＿alphaだけずらす
          IR_edge = IR_edge * 3.14 / 180;  	// ラジアンに直す
        }
        xx = cos(IR_edge) * IR_moterValue;  	// 回り込むには、、（X座標）
        yy = sin(IR_edge) * IR_moterValue;  	// 回り込むには、、（Y座標）
      } else {
        xx = IR_motermax;  	// 直進のためには、、（X座標）
        yy = IR_motermax;  	// 直進のためには、、（Y座標）
      }
    }
    gV[VAR_A] =  - xx;  	// 代入
    gV[VAR_B] = yy;  	// 代入
    gV[VAR_C] =  - yy;  	// 代入
    gV[VAR_D] =  xx;  	// 代入
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[4] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
