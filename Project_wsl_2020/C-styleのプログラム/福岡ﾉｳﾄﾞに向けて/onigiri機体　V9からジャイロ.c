//--------------------------------------------------------------------------------
// Title: Main - [ onigiri�@�́@V9����W���C�� ]
// Build:DAISEN C-Style for TJ3B  [ Ver.20190706 ] - [ Build_V190617 ]
//--------------------------------------------------------------------------------
// SetupVal:111111-111130-010002,cmt:
// CN01:IRedge,LINE��
// CN02:IRdist,LINE�E
// CN03:Gyro,LINE��
// CN04:IR1,LINE���O
// CN05:IR2
// CN06:IR3,LINE�E��
// CN07:�@,LINE�E�O
// CN08:�@,LINE����
// CN09:
// CN10:
//--------------------------------------------------------------------------------
#include "D_Main.h"
#include "D_I2C.h"
#include "D_SIO.h"
#include "D_EIO.h"
//--------------------------------------------------------------------------------
// Program Name : onigiri�@�́@V9����W���C��.C
//--------------------------------------------------------------------------------
void user_main(void)
{
  int MoterSpeed = 0;  	// IR�{�[����ǂ��Ƃ��̓�������
  int GyroMoterSpeed = 100;  	// �W���C���␳�̂Ƃ��̑���
  int IRaroundValue = 60;  	// ��荞�݂̊p�x
  double PIXY_val = 160;  	// PIXY�̒l
  double RfLb_X = 0;  	// �E�O�A����̃��[�^�̏o��
  double LfRb_Y = 0;  	// ���O�A�E��̃��[�^�̏o��
  double IRedge;  	// IR�̊p�x
  double GYROedge;  	// �W���C���̊p�x
  while (TRUE) {
    if (gAD[CN3] > 388) {
      gV[VAR_G] = -GyroMoterSpeed;
    } else if (gAD[CN3] < 347) {
      gV[VAR_G] = GyroMoterSpeed;
    } else {
      gV[VAR_G] = 0;
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
