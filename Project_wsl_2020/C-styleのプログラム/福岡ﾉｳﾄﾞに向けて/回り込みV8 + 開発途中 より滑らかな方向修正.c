//--------------------------------------------------------------------------------
// Title: Main - [ ��荞��V8 + �J���r�� ��芊�炩�ȕ����C�� ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111130-010002,cmt:
// CN01:IRedge,LINE��
// CN02:IRdist,LINE�E
// CN03:Gyro,LINE��
// CN04:IR1,LINE��
// CN05:IR2
// CN06:IR3,LINE�E��
// CN07:�@,LINE�O
// CN08:�@,LINE����
// CN09:
// CN10:
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : ��荞��V8 + �J���r�� ��芊�炩�ȕ����C��.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int MoterSpeed = 80;  	// IR�{�[����ǂ��Ƃ��̓�������
  int GyroMoterSpeed = 20;  	// �W���C���␳�̂Ƃ��̑���
  int IRaroundValue = 60;  	// ��荞�݂̊p�x
  double PIXY_val = 160;  	// PIXY�̒l
  double RfLb_X;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y;  	// ���O�A�E��̃��[�^�̏o��
  double IRedge;  	// IR�̊p�x
  double GYROedge;  	// �W���C���̊p�x
  while (TRUE) {
    if (get_timer(T1) < 400L) {
    } else if (sub_io_get_sensor(1, CN4) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = MoterSpeed;  	// �O�ɍs��
      LfRb_Y = MoterSpeed;  	// �O�ɍs��
    } else if (sub_io_get_sensor(1, CN7) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = -MoterSpeed;  	// ��ɍs��
      LfRb_Y = -MoterSpeed;  	// ��ɍs��
    } else if (sub_io_get_sensor(1, CN1) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = MoterSpeed;  	// �E�ɍs��
      LfRb_Y = -MoterSpeed;  	// �E�ɍs��
    } else if (sub_io_get_sensor(1, CN2) < 511) {
      set_Led(0, LED_ON);
      clr_timer(0);
      RfLb_X = -MoterSpeed;  	// ���ɍs��
      LfRb_Y = MoterSpeed;  	// ���ɍs��
    } else {
      set_Led(0, LED_OFF);
      RfLb_X = 0;  	// �O�ɍs��
      LfRb_Y = 0;  	// �O�ɍs��
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
