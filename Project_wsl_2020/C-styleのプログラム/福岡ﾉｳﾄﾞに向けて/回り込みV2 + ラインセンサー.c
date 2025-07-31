//--------------------------------------------------------------------------------
// Title: Main - [ ��荞��V2 + ���C���Z���T�[ ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111110-002002,cmt:
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
// Program Name : ��荞��V2 + ���C���Z���T�[.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 45;  	// IR�{�[����ǂ��Ƃ��̓�������
  const int GyroMoterSpeed = 15;  	// �W���C���␳�̂Ƃ��̑���
  const int IRaroundValue = 60;  	// ��荞�݂̊p�x
  double RfLb_X;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y;  	// ���O�A�E��̃��[�^�̏o��
  double IRedge;  	// IR�̊p�x
  double LINEedge;  	// LINE�̊p�x
  double GYROedge;  	// �W���C���̊p�x
  while (gAD[CN10] > 409) {
    gPwm[0] = 0x32 | 0x80;
    gPwm[1] = 0x32;
    gPwm[2] = 0x32 | 0x80;
    gPwm[3] = 0x32;
    pwm_out();
  }
  while (TRUE) {
    if (gAD[CN6] > 368) {
      gPwm[0] = 0x14 | 0x80;
      gPwm[1] = 0x14 | 0x80;
      gPwm[2] = 0x14 | 0x80;
      gPwm[3] = 0x14 | 0x80;
      pwm_out();
    } else if (gAD[CN6] < 306) {
      gPwm[0] = 0x14;
      gPwm[1] = 0x14;
      gPwm[2] = 0x14;
      gPwm[3] = 0x14;
      pwm_out();
    } else {
      gV[VAR_G] = -GyroMoterSpeed;
      set_Led(1, LED_OFF);
      IRedge = gAD[CN1] * 18 / 17;  	// IR�p�x����
      LINEedge = gAD[CN10] * 18 / 17;  	// IR�p�x����
      if (gAD[CN5] < 511) {
        motor(-50, -50);
        gV[VAR_G] = 0;  	// �Î~����
        RfLb_X = 0;  	// �Î~����
        LfRb_Y = 0;  	// �Î~����
      } else if (LINEedge < 400) {
        motor(50, 50);
        gV[VAR_L] = LINEedge + 180;
        gV[VAR_L] = gV[VAR_L] % 360;
        RfLb_X = cos(gV[VAR_L] * 3.1415 / 180) * MoterSpeed;  	// �������痣��铮��
        LfRb_Y = sin(gV[VAR_L] * 3.1415 / 180) * MoterSpeed;  	// �������痣��铮��
      } else if (gAD[CN1] < 10) {
        motor(-50, -50);
        RfLb_X = MoterSpeed;  	// �^�������s��
        LfRb_Y = MoterSpeed;  	// �^�������s��
      } else if (gAD[CN2] > 306) {
        motor(-50, -50);
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
        motor(-50, -50);
        if (IRedge > 400) {
          RfLb_X = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
          LfRb_Y = 0;  	// IR�{�[���������Ă��Ȃ��̂ŏo�͖͂���
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
}
//--------------------------------------------------------------------------------
