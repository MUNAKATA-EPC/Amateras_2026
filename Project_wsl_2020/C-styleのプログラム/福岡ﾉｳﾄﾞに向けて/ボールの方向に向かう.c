//--------------------------------------------------------------------------------
// Title: Main - [ �{�[���̕����Ɍ����� ]
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
// Program Name : �{�[���̕����Ɍ�����.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 30;  	// IR�{�[����ǂ��Ƃ��̓�������
  const int GyroMoterSpeed = 30;  	// �W���C���␳�̂Ƃ��̑���
  double RfLb_X;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y;  	// ���O�A�E��̃��[�^�̏o��
  double IRedge;  	// IR�̊p�x
  double GYROedge;  	// �W���C���̊p�x
  while (TRUE) {
    IRedge = gAD[CN1];  	// IR�p�x����
    if (IRedge > 400) {
      RfLb_X = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
      LfRb_Y = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
    } else {
      RfLb_X = cos((45 + IRedge) * 3.1415 / 180) * MoterSpeed;  	// IR�{�[���̕����֍s�����߂� ���� ���v�Z�E�E�O�A����
      LfRb_Y = sin((45 + IRedge) * 3.1415 / 180) * MoterSpeed;  	// IR�{�[���̕����֍s�����߂� ���� ���v�Z�E���O�A�E��
    }
    GYROedge = gAD[CN3];  	// �W���C���p�x����
    if (GYROedge < 180) {
      gV[VAR_G] = (GYROedge) * GyroMoterSpeed / 180;  	// �W���C���␳�� ���� ���v�Z
    } else {
      gV[VAR_G] = (GYROedge - 360) * GyroMoterSpeed / 180;  	// �W���C���␳�� ���� ���v�Z
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
