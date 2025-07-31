//--------------------------------------------------------------------------------
// Title: Main - [ �J��������ŏI�`�� ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-110010-010002,cmt:
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
// Program Name : �J��������ŏI�`��.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  double IR_motermax = 65;  	// ��荞�ލۂ̃��[�^�̍ő�o�͌���
  double IR_frontValue = 20;  	// �O�Ƃ݂Ȃ��A�p�x�͈̔́iIR�j
  double IR_front_number = 1.5;  	// �{�[�����O�ɋ߂����Ƃ���ɂ���Ƃ��̊p�x�������
  double IR_edge_move = 0;  	// IR�̊p�x
  double IR_edge_true = 0;  	// IR�̊p�x
  double IR_edgeAdjust = 0;  	// IR�̊p�x�̒����p�@�O�`�R�U�O�ɂ���p
  double IR_edgeCentralValue = 0;  	// IR�̊p�x�̒����p�@�O�`�R�U�O�ɂ���p
  double IR_alpha_max = 70;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē������imax�j
  double IR_alpha_min = 60;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē������imin�j
  double IR_alpha = 0;  	// ��荞�ލۂ̃{�[���ɑ΂��ĉ��x���炵�ē������i���ʁj
  double Gyro_moter_max = 15;  	// �W���C���̃��[�^�̍ő�o�͌���
  double Gyro_moter_min = 13;  	// �W���C���̃��[�^�̍ő�o�͌���
  double Gyro_frontValue = 1;  	// �O�Ƃ݂Ȃ��A�p�x�͈̔́iGyro�j
  double Gyro_edge = 0;  	// �W���C���̊p�x
  double Camera_xx;
  double Camera_yy;
  double Camera_motermax = 15;
  int Goal_number = 0;  	// 1�����S�[���A2���S�[��
  double Line_time = 400;  	// ���C���𓥂�ł��牽�b�ȏソ���Ă�����u���[�L���邩
  double Line_timer = 0;  	// ���C������Ŏg���^�C�}�[
  double Line_timerAdd = Line_time;  	// ���C���Z���T�[�Ŏg���^�C�}�[�̕t���i
  double Line_edge = 0;  	// LINE�̊p�x
  double xx = 0;  	// �ŏI�̃��[�^�o�́iX���W�j
  double yy = 0;  	// �ŏI�̃��[�^�o�́iY���W�j
  clr_timer(0);
  while (TRUE) {
    Line_timer = gT[T1] + Line_timerAdd;  	// ���C���^�C�}�[�̌v�Z
    gV[VAR_I] = (gAD[CN1] - IR_edgeAdjust - (IR_edgeCentralValue - IR_edgeAdjust)) + 360;  	// IR�p�x�̒���
    gV[VAR_I] = gV[VAR_I] % 360;
    IR_edge_true = gV[VAR_I] * 1.1;  	// IR�p�x��ǂށ@�@�x���̂܂�
    gV[VAR_I] = (gAD[CN1] - IR_edgeAdjust) + 360 - 45;  	// IR�p�x�̒���
    gV[VAR_I] = gV[VAR_I] % 360;
    IR_edge_move = gV[VAR_I] * 3.14 / 180 * 1.1;  	// IR�p�x��ǂށ@�@���W�A���ɒ���
    Gyro_edge = gAD[CN3] * 1.1;  	// �W���C����ǂ�
    i2c_get_pixy();
    if (gAD[CN5] < 511) {
      Goal_number = 1;
    } else {
      Goal_number = 2;
    }
    Camera_xx = get_pixydat_x(Goal_number);
    Camera_yy = get_pixydat_y(Goal_number);
    Gyro_edge = gAD[CN3] * 1.1;  	// �W���C����ǂ�
    gV[VAR_L] = gAD[CN4] + 180;
    gV[VAR_L] = gV[VAR_L] % 360;
    Line_edge = gV[VAR_L] * 3.14 / 180 * 1.1;  	// LINE��ǂށ@�@���W�A���ɒ���
    if (gAD[CN4] < 400) {
      motor(50, 50);
      if (Line_timer > Line_time) {
        Line_timerAdd = 0;  	// ���C���^�C�}�[�t���i�̒���
        gPwm[0] = 0x00 | 0x80;
        gPwm[1] = 0x00 | 0x80;
        gPwm[2] = 0x00 | 0x80;
        gPwm[3] = 0x00 | 0x80;
        pwm_out();
        clr_timer(0);
      }
      xx = cos(Line_edge) * IR_motermax;  	// LINE���痣���ɂ́A�A�iX���W�j
      yy = sin(Line_edge) * IR_motermax;  	// LINE���痣���ɂ́A�A�iY���W�j
    } else if (Gyro_edge > 50  &&  Gyro_edge < 310  &&  get_pixydat_index(Goal_number) == 0) {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ���C���^�C�}�[�t���i�̒���
      xx = 0;  	// 0
      yy = 0;  	// 0
    } else if (gAD[CN2] == 500) {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ���C���^�C�}�[�t���i�̒���
      xx = cos(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iX���W�j
      yy = sin(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iY���W�j
    } else if (IR_edge_true < IR_frontValue * IR_front_number  ||  IR_edge_true  > (360 - IR_frontValue) * IR_front_number) {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ���C���^�C�}�[�t���i�̒���
      if (IR_edge_true < IR_frontValue  ||  IR_edge_true  > 360 - IR_frontValue) {
        xx = IR_motermax;  	// ���i�̂��߂ɂ́A�A�iX���W�j
        yy = - IR_motermax;  	// ���i�̂��߂ɂ́A�A�iY���W�j
      } else {
        xx = cos(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iX���W�j
        yy = sin(IR_edge_move) * IR_motermax;  	// �{�[���̂ق��ɍs���ɂ́A�A�iY���W�j
      }
    } else {
      motor(-50, -50);
      Line_timerAdd = Line_time;  	// ���C���^�C�}�[�t���i�̒���
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
      xx = cos(IR_edge_move) * IR_motermax;  	// ��荞�ނɂ́A�A�iX���W�j
      yy = sin(IR_edge_move) * IR_motermax;  	// ��荞�ނɂ́A�A�iY���W�j
    }
    if (get_pixydat_index(Goal_number) == 0) {
      if (Gyro_edge < Gyro_frontValue) {
        gV[VAR_G] = 0;
      } else if (Gyro_edge > 360 - Gyro_frontValue) {
        gV[VAR_G] = 0;
      } else if (Gyro_edge < 180) {
        gV[VAR_G] = ( (Gyro_moter_min - Gyro_moter_max) / 32400 ) * Gyro_edge * Gyro_edge;
        gV[VAR_G] = gV[VAR_G] - (Gyro_moter_min - Gyro_moter_max) / 90 * Gyro_edge + Gyro_moter_min;
      } else {
        gV[VAR_G] = ( (Gyro_moter_min - Gyro_moter_max) / 32400 ) * Gyro_edge * Gyro_edge;
        gV[VAR_G] = gV[VAR_G] - (Gyro_moter_min - Gyro_moter_max) / 90 * Gyro_edge + Gyro_moter_min;
        gV[VAR_G] = - gV[VAR_G];
      }
    } else if (Camera_xx < 160) {
      gV[VAR_G] = Camera_motermax / 25600 * Camera_xx * Camera_xx - Camera_motermax / 80 * Camera_xx + Camera_motermax;
    } else {
      gV[VAR_G] = - Camera_motermax / 25600 * Camera_xx * Camera_xx + Camera_motermax / 80 * Camera_xx - Camera_motermax;
    }
    gV[VAR_A] =  yy + gV[VAR_G];  	// ���
    gV[VAR_B] = xx + gV[VAR_G];  	// ���
    gV[VAR_C] =  - xx + gV[VAR_G];  	// ���
    gV[VAR_D] =  -yy + gV[VAR_G];  	// ���
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
