//--------------------------------------------------------------------------------
// Title: Main - [ ��荞��V3 + ���C���Z���T�[ ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111110-010002,cmt:
// CN01:
// CN02:
// CN03:
// CN04:
// CN05:
// CN06:
// CN07:����
// CN08:�E�O
// CN09:�E��
// CN10:���O
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : ��荞��V3 + ���C���Z���T�[.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 80;  	// IR�{�[����ǂ��Ƃ��̓�������
  const int GyroMoterSpeed = 15;  	// �W���C���␳�̂Ƃ��̑���
  const int IRaroundValue = 60;  	// ��荞�݂̊p�x
  double RfLb_X;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y;  	// ���O�A�E��̃��[�^�̏o��
  double IRedge;  	// IR�̊p�x
  double GYROedge;  	// �W���C���̊p�x
  double GYRORESETedge;  	// �W���C���̊p�x
  while (TRUE) {
    set_Led(1, LED_OFF);
    if (gAD[CN5] < 511) {
      gPwm[0] = 0x00 | 0x80;
      gPwm[1] = 0x00 | 0x80;
      gPwm[2] = 0x00 | 0x80;
      gPwm[3] = 0x00 | 0x80;
      pwm_out();
      if (gAD[CN7] < 500  ||  gAD[CN8] < 500  ||  gAD[CN9] < 500  ||  gAD[CN10] < 500) {
        motor(50, 50);
      } else {
        motor(-50, -50);
      }
    } else {
      if (gAD[CN8] < 511) {
        motor(50, 50);
        gV[VAR_A] = MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E���O���[�^
        gV[VAR_B] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�O���[�^
        gV[VAR_C] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E���ヂ�[�^
        gV[VAR_D] = - MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�ヂ�[�^
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else if (gAD[CN10] < 511) {
        motor(50, 50);
        gV[VAR_A] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E���O���[�^
        gV[VAR_B] = - MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�O���[�^
        gV[VAR_C] = MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E���ヂ�[�^
        gV[VAR_D] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�ヂ�[�^
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else if (gAD[CN7] < 511) {
        motor(50, 50);
        gV[VAR_A] = -MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E���O���[�^
        gV[VAR_B] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�O���[�^
        gV[VAR_C] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E���ヂ�[�^
        gV[VAR_D] = MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�ヂ�[�^
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else if (gAD[CN9] < 511) {
        motor(50, 50);
        gV[VAR_A] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E���O���[�^
        gV[VAR_B] = MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�O���[�^
        gV[VAR_C] = - MoterSpeed;  	// �ŏI�I�Ȍv�Z���ʂ����E���ヂ�[�^
        gV[VAR_D] = 0;  	// �ŏI�I�Ȍv�Z���ʂ����E�E�ヂ�[�^
        gPwm[0] = gV[VAR_A] < 0 ? (gV[VAR_A] * -1) | 0x80 : gV[VAR_A];
        gPwm[1] = gV[VAR_B] < 0 ? (gV[VAR_B] * -1) | 0x80 : gV[VAR_B];
        gPwm[2] = gV[VAR_C] < 0 ? (gV[VAR_C] * -1) | 0x80 : gV[VAR_C];
        gPwm[3] = gV[VAR_D] < 0 ? (gV[VAR_D] * -1) | 0x80 : gV[VAR_D];
        pwm_out();
        wait_ms(300);
      } else {
        motor(-50, -50);
        if (gAD[CN1] < 10) {
          RfLb_X = MoterSpeed;  	// �^�������s��
          LfRb_Y = MoterSpeed;  	// �^�������s��
        } else {
          IRedge = gAD[CN1] * 18 / 17;  	// IR�p�x����
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
    if (gAD[CN4] < 511) {
      GYRORESETedge = gAD[CN6] * 18 / 17;
    }
    gV[VAR_G] = gAD[CN6] * 18 / 17 - GYRORESETedge + 360;  	// �W���C���p�x����
    gV[VAR_G] = gV[VAR_G] % 360;
    GYROedge = gV[VAR_G];
    if (GYROedge < 180) {
      gV[VAR_G] = GyroMoterSpeed;  	// �W���C���␳�� ���� ���v�Z
    } else {
      gV[VAR_G] = -GyroMoterSpeed;  	// �W���C���␳�� ���� ���v�Z
    }
  }
}
//--------------------------------------------------------------------------------
