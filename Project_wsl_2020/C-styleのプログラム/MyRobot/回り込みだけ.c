//--------------------------------------------------------------------------------
// Title: Main - [ ��荞�݂��� ]
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
// Program Name : ��荞�݂���.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double IR_motermax = 55;  	// ��荞�ލۂ̃��[�^�̍ő�o��
  double IR_moterValue;  	// ��荞�ލۂ̃��[�^�̍ő�o�͌���
  double IR_frontValue = 10;  	// �O�Ƃ݂Ȃ��A�p�x�͈̔́iIR�j
  double IR_edge = 0;  	// IR�̊p�x
  double IR_alpha = 70;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē�����
  double Gyro_moterValue = 20;  	// �W���C���̃��[�^�̍ő�o�͌���
  double Gyro_frontValue = 2.5;  	// �O�Ƃ݂Ȃ��A�p�x�͈̔́iGyro�j
  double Gyro_edge = 0;  	// �W���C���̊p�x
  double xx = 0;  	// �ŏI�̃��[�^�o�́iX���W�j
  double yy = 0;  	// �ŏI�̃��[�^�o�́iY���W�j
  while (TRUE) {
    if (gAD[CN2] < 500) {
      set_Led(0, LED_OFF);
      IR_edge = gAD[CN1] * 9 / 8 + 405;  	// �S�T�x���ɂ��炷
      IR_edge = IR_edge * 3.14 / 180;  	// ���W�A���ɒ���
      xx = cos(IR_edge) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iX���W�j
      yy = sin(IR_edge) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iY���W�j
    } else if (gAD[CN1] * 9 / 8 < IR_frontValue * 1.2  &&  gAD[CN1] * 9 / 8  > 360 - IR_frontValue * 1.2) {
      set_Led(0, LED_OFF);
      IR_edge = gAD[CN1] * 1.06 + 405;  	// �S�T�x���ɂ��炷
      IR_edge = IR_edge * 3.14 / 180;  	// ���W�A���ɒ���
      xx = cos(IR_edge) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iX���W�j
      yy = sin(IR_edge) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iY���W�j
    } else {
      set_Led(0, LED_ON);
      IR_moterValue = IR_motermax;
      if (gAD[CN1] * 9 / 8 > IR_frontValue  &&  gAD[CN1] * 9 / 8 < 360 - IR_frontValue) {
        if (gAD[CN1] * 9 / 8 < 180) {
          IR_edge = gAD[CN1] * 9 / 8 + 405 + IR_alpha;  	// �S�T�x�{IR�Qalpha�������炷
          IR_edge = IR_edge * 3.14 / 180;  	// ���W�A���ɒ���
        } else {
          IR_edge = gAD[CN1] * 9 / 8 + 405 - IR_alpha;  	// �S�T�x-IR�Qalpha�������炷
          IR_edge = IR_edge * 3.14 / 180;  	// ���W�A���ɒ���
        }
        xx = cos(IR_edge) * IR_moterValue;  	// ��荞�ނɂ́A�A�iX���W�j
        yy = sin(IR_edge) * IR_moterValue;  	// ��荞�ނɂ́A�A�iY���W�j
      } else {
        xx = IR_motermax;  	// ���i�̂��߂ɂ́A�A�iX���W�j
        yy = IR_motermax;  	// ���i�̂��߂ɂ́A�A�iY���W�j
      }
    }
    Gyro_edge = gAD[CN3];
    if (Gyro_edge > 50  &&  Gyro_edge < 310) {
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
    gV[VAR_A] =  - xx + gV[VAR_G];  	// ���
    gV[VAR_B] = yy + gV[VAR_G];  	// ���
    gV[VAR_C] =  - yy + gV[VAR_G];  	// ���
    gV[VAR_D] =  xx + gV[VAR_G];  	// ���
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
