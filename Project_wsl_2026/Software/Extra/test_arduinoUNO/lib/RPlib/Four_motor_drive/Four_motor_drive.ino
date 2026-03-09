//-------------------------------------
// RPmotor drive Example
// Four mode drive for 4 motors
//-------------------------------------
// connection define : 
// RPmotor motor(PWM,IN1,IN2)
// RPmotor motor(MC1/MC2/MC3/MC4)
// *rotate mode
// motor.rotate( vel[-255 +255], delay[msec]);
// motor.rotate( vel[-255 +255]);
// *cw mode
// motor.cw( vel[0-255], delay[msec]);
// motor.cw( vel[0-255]);
// *ccw mode
// motor.ccw( vel[0-255], delay[msec]);
//  motor.ccw( vel[0-255]);
// *free mode
// motor.free( vel[0-255], delay[msec]);
//  motor.free();
// *brake mode
// motor.brake( vel[0-255], delay[msec]);
// motor.brake();
//
/// written by yu okumura@furo 2013/7/11
//--------------------------------------
#include <RPlib.h>

//おまじない(ピン接続設定)
RPmotor mc0(MC0);
RPmotor mc1(MC1);
RPmotor mc2(MC2);
RPmotor mc3(MC3);

void setup()
{

}

void loop()
{
  mc0.cw(100,1000);  //MC0正回転
  mc0.brake(100);  //MC0ブレーキモード
  mc0.ccw(100,1000);  //MC4逆回転
  mc0.free(100);  //MC0フリーモード

  mc1.cw(100,1000);  //MC1正回転
  mc1.brake();  //MC1ブレーキモード
  mc1.ccw(100,1000);  //MC1逆回転
  mc1.free();  //MC1フリーモード

  mc2.cw(100,1000);  //MC2正回転
  mc2.brake(100);  //MC2ブレーキモード
  mc2.ccw(100,1000);  //MC2逆回転
  mc2.free(100);  //MC2フリーモード

  mc3.cw(100,1000);  //MC3正回転
  mc3.brake(100);  //MC3ブレーキモード
  mc3.ccw(100,1000);  //MC3逆回転
  mc3.free(100);  //MC3フリーモード

}

