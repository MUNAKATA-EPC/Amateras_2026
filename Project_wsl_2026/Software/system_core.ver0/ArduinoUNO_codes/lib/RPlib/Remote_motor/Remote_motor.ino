
//-------------------------------------
// RPmotor drive Example
// 戦車タイプ、無線操作モード
//--------------------------------------
#include <RPlib.h>
#include <PS2X_lib.h>

//おまじない(ピン接続設定)
RPmotor mc1(MC3);
RPmotor mc2(MC0);
PS2X ps2x;

int rx,lx;
void setup()
{
   ps2x.config_gamepad(13,11,10,12, true, true);   //おまじない、ピン設定
}

void loop()
{
 ps2x.read_gamepad();    //ゲームパッドから値を読み込む
  
  rx=128-ps2x.Analog(PSS_RY); //アナログ右スティック上下
  lx=128-ps2x.Analog(PSS_LY); //アナログ左スティック上下
  
    mc1.rotate(rx);
    mc2.rotate(-lx); 
  
  delay(10);
}



