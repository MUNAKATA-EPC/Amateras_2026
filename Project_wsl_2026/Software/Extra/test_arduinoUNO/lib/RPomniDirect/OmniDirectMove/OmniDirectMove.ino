//-----------------------------------------------
// control omni dirctinal robot
// オムニホイールロボットをプログラムする
//-----------------------------------------------
// RPomniDirect omni(Again,Bgain,Cgain,Fgain);
// Againはモータ0の重み
// Bgainはモータ1の重み
// Cgainはモータ2の重み
// Fgainは摩擦の補正
//  omni.move(vx,vy,w);
// vx: x方向速度
// vy: y方向速度
// w: 回転方向速度
//----------------------
#include "RPomniDirect.h"
#include <RPlib.h>

RPomniDirect omniBot(1.0f,1.0f,1.0f,50.0f);

void setup()
{

}

void loop()
{
 omniBot.move(20,0,0); //右方向へ移動
 delay(1000);
 omniBot.move(0,20,0); //正面へ移動
 delay(1000);
 omniBot.move(-20,0,0); //左へ移動
 delay(1000);
 omniBot.move(0,-20,0); //手前へ移動
 delay(1000);
 omniBot.move(0,0,40); //時回りに旋回
 delay(1000);
}
