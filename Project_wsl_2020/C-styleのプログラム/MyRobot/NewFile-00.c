//--------------------------------------------------------------------------------
// Title: Main - [ NewFile-00 ]
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
// Program Name : NewFile-00.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double IR_motermax = 80;  	// ��荞�ލۂ̃��[�^�̍ő�o��
  double IR_moterValue;  	// ��荞�ލۂ̃��[�^�̍ő�o�͌���
  double IR_frontValue = 10;  	// �O�Ƃ݂Ȃ��A�p�x�͈̔́iIR�j
  double IR_number = 9 / 8;  	// IR�̊p�x�������
  double IR_front_number = 1.02;  	// �{�[�����O�ɋ߂����Ƃ���ɂ���Ƃ��̊p�x�������
  double IR_edge_move = 0;  	// IR�̊p�x
  double IR_edge_true = 0;  	// IR�̊p�x
  double IR_alpha_max = 60;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē������imax�j
  double IR_alpha_min = 30;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē������imin�j
  double IR_alpha = 0;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē������i���ʁj
  double Gyro_moterValue = 15;  	// �W���C���̃��[�^�̍ő�o�͌���
  double Gyro_frontValue = 2.5;  	// �O�Ƃ݂Ȃ��A�p�x�͈̔́iGyro�j
  double Gyro_edge = 0;  	// �W���C���̊p�x
  double Line_edge = 0;  	// LINE�̊p�x
  double xx = 0;  	// �ŏI�̃��[�^�o�́iX���W�j
  double yy = 0;  	// �ŏI�̃��[�^�o�́iY���W�j
  while (TRUE) {
    gV[VAR_I] = gAD[CN1] + 405;
    gV[VAR_I] = gV[VAR_I] % 360;
    IR_edge_move = gV[VAR_I] * 3.14 / 180 * IR_number;  	// IR�p�x��ǂށ@�@���W�A���ɒ���
    IR_edge_true = gAD[CN1] * IR_number;  	// IR�p�x��ǂށ@�@�x���̂܂�
    Gyro_edge = gAD[CN3];  	// �W���C����ǂ�
    gV[VAR_L] = gAD[CN4] + 180;
    gV[VAR_L] = gV[VAR_L] % 360;
    Line_edge = gV[VAR_L] * 3.14 / 180;  	// LINE��ǂށ@�@���W�A���ɒ���
    if (gAD[CN4] < 400) {
      clr_timer(0);
      set_Led(0, LED_OFF);
      set_Led(1, LED_ON);
      xx = cos(Line_edge) * IR_motermax;  	// LINE���痣���ɂ́A�A�iX���W�j
      yy = sin(Line_edge) * IR_motermax;  	// LINE���痣���ɂ́A�A�iY���W�j
    } else if (get_timer(T1) < 400L) {
      set_Led(0, LED_OFF);
      set_Led(1, LED_ON);
      xx = cos(Line_edge) * IR_motermax;  	// LINE���痣���ɂ́A�A�iX���W�j
      yy = sin(Line_edge) * IR_motermax;  	// LINE���痣���ɂ́A�A�iY���W�j
    } else if (gAD[CN2] < 500) {
      set_Led(0, LED_OFF);
      set_Led(1, LED_OFF);
      xx = cos(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iX���W�j
      yy = sin(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iY���W�j
    } else if (IR_edge_true < IR_frontValue * IR_front_number  ||  IR_edge_true  > 360 - IR_frontValue * IR_front_number && IR_edge_true < 360) {
      set_Led(0, LED_OFF);
      set_Led(1, LED_OFF);
      xx = cos(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iX���W�j
      yy = sin(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iY���W�j
    } else {
      set_Led(0, LED_ON);
      set_Led(1, LED_OFF);
      IR_moterValue = IR_motermax;
      if (IR_edge_true > IR_frontValue  &&  IR_edge_true < 360 - IR_frontValue) {
        IR_alpha = ( (IR_alpha_min - IR_alpha_max) / 32400 ) * IR_edge_true * IR_edge_true;
        IR_alpha = IR_alpha - (IR_alpha_min - IR_alpha_max) / 90 * IR_edge_true + IR_alpha_min;
        if (IR_edge_true < 180) {
          IR_edge_move += IR_alpha * 3.14 / 180;  	// IR�Qalpha�������炷
        } else {
          IR_edge_move -= IR_alpha * 3.14 / 180;  	// -IR�Qalpha�������炷
        }
        IR_edge_move= IR_edge_move * 180 / 3.14 + 360;
        gV[VAR_J] = IR_edge_move;  	// �O�`�R�U�O�x�ɒ������
        gV[VAR_J] = gV[VAR_J] % 360;
        IR_edge_move = gV[VAR_J] * 3.14 / 180;  	// �O�`�R�U�O�x�ɒ������
        xx = cos(IR_edge_move) * IR_moterValue;  	// ��荞�ނɂ́A�A�iX���W�j
        yy = sin(IR_edge_move) * IR_moterValue;  	// ��荞�ނɂ́A�A�iY���W�j
      } else if (gAD[CN1] > 409) {
        xx = 0;
        yy = 0;
      } else {
        xx = IR_motermax;  	// ���i�̂��߂ɂ́A�A�iX���W�j
        yy = IR_motermax;  	// ���i�̂��߂ɂ́A�A�iY���W�j
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
