//-----------------------------------------------------------------------
// オムニホイールロボットをゲームパッドでコントロールする
// 旋回可能、完全版
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
#include <RPomniDirect.h>
#include <RPlib.h>
#include <PS2X_lib.h>

//おまじない
RPomniDirect omniBot(1.0f,1.0f,1.0f,50.0f);//オムニホイール調整用パラメータ
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

  if(ps2x.Button(PSB_L1) || ps2x.Button(PSB_R1)) 
  {  //L1/R1ボタン押しでリミッタ解除モード
    omniBot.move(xx,yy,ww); 
  }
  else if(ps2x.Button(PSB_L2) || ps2x.Button(PSB_R2)) 
  {  //L2/R2ボタン押しでゆっくりモード
    omniBot.move(xx/6,yy/6,ww/6);
  }
  else
  {   //普通モード
    omniBot.move(xx/2,yy/2,ww/2);
  }
  delay(10);
}

