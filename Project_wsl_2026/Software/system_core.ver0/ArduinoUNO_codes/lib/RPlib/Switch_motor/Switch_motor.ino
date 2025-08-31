//-------------------------------------
// RPmotor drive Example
// Switch Motor Drive
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
RPmotor mc1(MC1);
RPmotor mc2(MC2);
RPmotor mc3(MC3);
//RPmotor mc4(MC4);

int d1Pin = 10;
int d2Pin = 11;

void setup()
{
  pinMode(d1Pin, INPUT);
  pinMode(d2Pin, INPUT);
}

void loop()
{

  if(digitalRead(d1Pin)==HIGH&&digitalRead(d2Pin)==HIGH)
  {
    mc1.free();
    mc2.free();
    mc3.cw(50);  //MC3正回転

  }

  if(digitalRead(d1Pin)==HIGH&&digitalRead(d2Pin)==LOW)
  {
    mc1.cw(50);  //MC1正回転
    mc2.free();
    mc3.free();
  }

  if(digitalRead(d1Pin)==LOW&&digitalRead(d2Pin)==HIGH)
  {
    mc1.free();
    mc2.cw(50);  //MC2正回転
    mc3.free();
  }

  if(digitalRead(d1Pin)==LOW&&digitalRead(d2Pin)==LOW)
  {
    mc1.free();
    mc2.free();
    mc3.free();
  }
}



