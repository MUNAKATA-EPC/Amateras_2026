//--------------------------------------------------------------------------------
// Title: Main - [ ��荞�݂ƃW���C�� ]
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
// Program Name : ��荞�݂ƃW���C��.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double max_moter = 100;  	// ���[�^�[�ő�o��
  double IR_edge = 0;  	// IR�̊p�x
  double IR_frontValue = 3.5;  	// �O�Ƃ݂Ȃ��A�p�x�͈̔�
  double IR_motermax = 80;  	// ��荞�ލۂ̃��[�^�̍ő�o��
  double IR_motermin = 50;  	// ��荞�ލۂ̃��[�^�̍Œ�o��
  double IR_moterValue;  	// ��荞�ލۂ̃��[�^�̏o�͌���
  double IR_alpha = 40;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē�����
  double xx = 0;  	// �ŏI�̃��[�^�o�́iX���W�j
  double yy = 0;  	// �ŏI�̃��[�^�o�́iY���W�j
  while (TRUE) {
    if (gAD[CN2] < 511) {
      set_Led(0, LED_OFF);
      IR_edge = gAD[CN3] * 0.98 + 405;  	// �S�T�x���ɂ��炷
      IR_edge = IR_edge * 3.14 / 180;  	// ���W�A���ɒ���
      xx = cos(IR_edge) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iX���W�j
      yy = sin(IR_edge) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iY���W�j
    } else {
      set_Led(0, LED_ON);
      if (gAD[CN3] * 0.98 > IR_frontValue  &&  gAD[CN3] * 0.98 < 360 - IR_frontValue) {
        IR_moterValue = ( (IR_motermin - IR_motermax) / (IR_frontValue * IR_frontValue + 32400 - 360 * IR_frontValue) ) * gAD[CN3] * gAD[CN3];
        IR_moterValue = IR_moterValue + ( (360 * IR_motermax - 360 * IR_motermin) / (IR_frontValue * IR_frontValue + 32400 - 360 * IR_frontValue) ) * gAD[CN3];
        IR_moterValue = IR_moterValue + IR_motermax +  ( (32400 * IR_motermin - 32400 * IR_motermax) / (IR_frontValue * IR_frontValue + 32400 - 360 * IR_frontValue) );
        if (gAD[CN3] * 0.98 < 180) {
          IR_edge = gAD[CN3] * 0.98 + 405 + IR_alpha;  	// �S�T�x�{IR�Qalpha�������炷
          IR_edge = IR_edge * 3.14 / 180;  	// ���W�A���ɒ���
        } else {
          IR_edge = gAD[CN3] * 0.98 + 405 - IR_alpha;  	// �S�T�x-IR�Qalpha�������炷
          IR_edge = IR_edge * 3.14 / 180;  	// ���W�A���ɒ���
        }
        xx = cos(IR_edge) * IR_moterValue;  	// ��荞�ނɂ́A�A�iX���W�j
        yy = sin(IR_edge) * IR_moterValue;  	// ��荞�ނɂ́A�A�iY���W�j
      } else {
        xx = IR_motermax;  	// ���i�̂��߂ɂ́A�A�iX���W�j
        yy = IR_motermax;  	// ���i�̂��߂ɂ́A�A�iY���W�j
      }
    }
    gV[VAR_A] =  - xx;  	// ���
    gV[VAR_B] = yy;  	// ���
    gV[VAR_C] =  - yy;  	// ���
    gV[VAR_D] =  xx;  	// ���
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[4] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
