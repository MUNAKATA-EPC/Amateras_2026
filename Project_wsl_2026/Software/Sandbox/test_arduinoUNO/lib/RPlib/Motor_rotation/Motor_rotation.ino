//-------------------------------------
// RPmotor drive Example
// Motor Rotation
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

//ピン接続設定
RPmotor mc0(3,4,2);
RPmotor mc1(5,A1,A0);
RPmotor mc2(6,8,7);
RPmotor mc3(9,13,12);

void setup()
{
	pinMode(10,OUTPUT);
}

void loop()
{
  int i;
  for(i=-255;i<=255;i++)
	{
		mc0.rotate(i);  //MC0正逆回転モード
  		mc1.rotate(i);  //MC1正逆回転モード
		mc2.rotate(i);  //MC2正逆回転モード
		mc3.rotate(i);  //MC3正逆回転モード
		if(i>0){digitalWrite(10,HIGH);}
		else{digitalWrite(10,LOW);}
		delay(10);
  }
  for(i=255;i>-255;i--)
	{
        	mc0.rotate(i);  //MC0正逆回転モード
  		mc1.rotate(i);  //MC1正逆回転モード
		mc2.rotate(i);  //MC2正逆回転モード
		mc3.rotate(i);  //MC3正逆回転モード
	    if(i>0){digitalWrite(10,HIGH);}
	    else{digitalWrite(10,LOW); } 
	  delay(10);
  }

}
