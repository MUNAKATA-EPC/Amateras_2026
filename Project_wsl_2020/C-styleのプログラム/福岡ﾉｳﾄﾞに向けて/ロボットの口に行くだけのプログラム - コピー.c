//--------------------------------------------------------------------------------
// Title: Main - [ ���{�b�g�̌��ɍs�������̃v���O���� - �R�s�[ ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-110030-000002,cmt:
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
// Program Name : ���{�b�g�̌��ɍs�������̃v���O���� - �R�s�[.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 50;  	// IR�{�[����ǂ��Ƃ��̓�������
  const int GyroMoterSpeed = 50;  	// �W���C���␳�̂Ƃ��̑���
  const int IRaroundValue = 70;  	// ��荞�݂̊p�x
  double RfLb_X;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y;  	// ���O�A�E��̃��[�^�̏o��
  double IRedge;  	// IR�̊p�x
  double GYROedge;  	// �W���C���̊p�x
  while (TRUE) {
    IRedge = gAD[CN1] * 18 / 17;  	// IR�p�x����
    if (gAD[CN1] > 409) {
      RfLb_X = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
      LfRb_Y = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
    } else {
      if (IRedge > 180) {
        RfLb_X = cos((45 + IRedge + 360 - IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E�E�O�A����@�@�`���Ƀ{�[��������Ƃ��`
        LfRb_Y = sin((45 + IRedge + 360 - IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E���O�A�E��@�@�`���Ƀ{�[��������Ƃ��`
      } else {
        RfLb_X = cos((45 + IRedge + IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E�E�O�A����@�@�`�E�Ƀ{�[��������Ƃ��`
        LfRb_Y = sin((45 + IRedge + IRaroundValue) * 3.1415 / 180) * MoterSpeed;  	// ��荞�ނ��߂� ���� ���v�Z�E���O�A�E��@�@�`�E�Ƀ{�[��������Ƃ��`
      }
    }
    gV[VAR_G] = 0;  	// �W���C���␳�� ���� ���v�Z
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
