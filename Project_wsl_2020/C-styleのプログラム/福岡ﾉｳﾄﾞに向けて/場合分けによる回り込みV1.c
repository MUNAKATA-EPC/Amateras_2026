//--------------------------------------------------------------------------------
// Title: Main - [ �ꍇ�����ɂ���荞��V1 ]
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
// Program Name : �ꍇ�����ɂ���荞��V1.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  const int MoterSpeed = 50;  	// IR�{�[����ǂ��Ƃ��̓�������
  const int GyroMoterSpeed = 50;  	// �W���C���␳�̂Ƃ��̑���
  double RfLb_X;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y;  	// ���O�A�E��̃��[�^�̏o��
  double GYROedge;
  while (TRUE) {
    if (gAD[CN1] < 92) {
      motor(50, 50);
      set_Led(1, LED_ON);
    } else if (gAD[CN1] < 173) {
      motor(50, 50);
      set_Led(1, LED_OFF);
    } else if (gAD[CN1] < 368) {
      motor(-50, -50);
      set_Led(1, LED_OFF);
    } else if (gAD[CN1] < 470) {
      motor(50, 50);
      set_Led(1, LED_OFF);
    } else if (gAD[CN1] < 511) {
      motor(-50, -50);
      set_Led(1, LED_OFF);
    } else {
    }
    GYROedge = gAD[CN3] * 18 / 17;  	// �W���C���p�x����
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
