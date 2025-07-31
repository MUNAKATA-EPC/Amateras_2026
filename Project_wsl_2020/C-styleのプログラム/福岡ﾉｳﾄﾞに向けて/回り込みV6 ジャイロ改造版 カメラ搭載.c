//--------------------------------------------------------------------------------
// Title: Main - [ ��荞��V6 �W���C�������� �J�������� ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111130-010002,cmt:
// CN01:Ball
// CN02:Line
// CN03:L-Touch
// CN04:R-Touch
// CN05:C-Touch
// CN06:
// CN07:
// CN08:
// CN09:
// CN10:
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : ��荞��V6 �W���C�������� �J��������.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int MoterSpeed = 80;  	// IR�{�[����ǂ��Ƃ��̓�������
  int GyroMoterSpeed = 20;  	// �W���C���␳�̂Ƃ��̑���
  int IRaroundValue = 60;  	// ��荞�݂̊p�x
  double RfLb_X;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y;  	// ���O�A�E��̃��[�^�̏o��
  double IRedge;  	// IR�̊p�x
  double GYROedge;  	// �W���C���̊p�x
  while (TRUE) {
    if (get_pixydat_index(1) == 0) {
      if (gAD[CN3] > 388) {
        gV[VAR_G] = -GyroMoterSpeed;
      } else if (gAD[CN3] < 347) {
        gV[VAR_G] = GyroMoterSpeed;
      } else {
        gV[VAR_G] = 0;
      }
    } else {
      if (get_pixydat_x(1) > 180) {
        gV[VAR_G] = -GyroMoterSpeed;
      } else if (get_pixydat_x(1) < 140) {
        gV[VAR_G] = GyroMoterSpeed;
      } else {
        gV[VAR_G] = 0;
      }
    }
    motor(-50, -50);
    if (gAD[CN5] < gAD[CN4]  &&  gAD[CN5] < gAD[CN6]) {
      RfLb_X = MoterSpeed;  	// �^�������s��
      LfRb_Y = MoterSpeed;  	// �^�������s��
    } else {
      IRedge = gAD[CN1] * 18/15;  	// IR�p�x����
      if (IRedge > 180) {
        IRedge = gAD[CN1] * 27/29;  	// IR�p�x����
      }
      if (IRedge > 400) {
        RfLb_X = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
        LfRb_Y = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
      } else if (gAD[CN2] > 306) {
        if (IRedge > 180) {
          gV[VAR_I] = 45 + IRedge + 360 - IRaroundValue;
          gV[VAR_I] = gV[VAR_I] % 360;
          RfLb_X = cos(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E�E�O�A����@�@�`���Ƀ{�[��������Ƃ��`
          LfRb_Y = sin(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E���O�A�E��@�@�`���Ƀ{�[��������Ƃ��`
        } else {
          gV[VAR_I] = 45 + IRedge + IRaroundValue;
          gV[VAR_I] = gV[VAR_I] % 360;
          RfLb_X = cos(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E�E�O�A����@�@�`�E�Ƀ{�[��������Ƃ��`
          LfRb_Y = sin(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E���O�A�E��@�@�`�E�Ƀ{�[��������Ƃ��`
        }
      } else {
        gV[VAR_I] = 45 + IRedge;
        gV[VAR_I] = gV[VAR_I] % 360;
        RfLb_X = cos(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// IR�{�[���̕����֍s�����߂� ���� ���v�Z�E�E�O�A����
        LfRb_Y = sin(gV[VAR_I] * 3.1415 / 180) * MoterSpeed;  	// IR�{�[���̕����֍s�����߂� ���� ���v�Z�E���O�A�E��
      }
    }
    gV[VAR_A] = - RfLb_X + gV[VAR_G];  	// �ŏI�I�Ȍv�Z���ʂ����E���O���[�^
    gV[VAR_B] = LfRb_Y + gV[VAR_G];  	// �ŏI�I�Ȍv�Z���ʂ����E�E�O���[�^
    gV[VAR_C] = - LfRb_Y + gV[VAR_G];  	// �ŏI�I�Ȍv�Z���ʂ����E���ヂ�[�^
    gV[VAR_D] = RfLb_X + gV[VAR_G];  	// �ŏI�I�Ȍv�Z���ʂ����E�E�ヂ�[�^
    gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
    gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
    gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
    gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
    pwm_out();
  }
}
//--------------------------------------------------------------------------------
