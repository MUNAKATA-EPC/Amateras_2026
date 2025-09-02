
//--------------------------------------------------
// control omni dirctinal robot
// オムニホイールロボットをゲームパッドでコントロールする
// パラメータ調整用プログラム
//-------------------------------------------------
// RPomniDirect omni(Again,Bgain,Cgain,Fgain);
// Againはモータ0の重み
// Bgainはモータ1の重み
// Cgainはモータ2の重み
// Fgainは摩擦の補正
//  omni.move(vx,vy,w);
// vx: x方向速度
// vy: y方向速度
// w: 回転方向速度
//--------------------------------------------
#include <RPomniDirect.h>
#include <RPlib.h>
#include <PS2X_lib.h>

//おまじない
RPomniDirect omniBot(1.0f,1.0f,1.0f,40.0f);//オムニホイール調整用パラメータ
PS2X ps2x;

float xx,yy,ww;//移動速度変数

void setup()
{
  ps2x.config_gamepad(13,11,10,12, true, true);   //おまじない、ピン設定
}

void loop()
{
  ps2x.read_gamepad();    //ゲームパッドから値を読み込む

  xx=ps2x.Analog(PSS_LX); //アナログ左スティック左右
  yy=ps2x.Analog(PSS_LY); //アナログ左スティック上下
  ww=ps2x.Analog(PSS_RX); //アナログ右スティック左右

  if(ps2x.Button(PSB_RED))
   {  //右斜め30度
    omniBot.move(64*cos(PI*1/6),64*sin(PI*1/6),0);
   } 
  else if(ps2x.Button(PSB_PINK)) 
  {//左斜め30度
    omniBot.move(64*cos(PI*5/6),64*sin(PI*5/6),0);
  }
  else if(ps2x.Button(PSB_BLUE)) 
  {
    omniBot.move(64,0,0);  //真横移動
  }
  else if(ps2x.Button(PSB_GREEN))
  {
    omniBot.move(0,64,0);//前進
  }
  else
  {
  omniBot.move(xx/2,yy/2,2*ww);
  }
  delay(10);
}

