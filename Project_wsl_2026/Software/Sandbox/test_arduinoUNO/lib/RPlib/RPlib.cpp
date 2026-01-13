// motor drive library v.1
//  ROBOPRO Shield 
//  written by yu okumura@furo  2013/7/11
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

#include "Arduino.h"
#include "RPlib.h"

// ROBOPRO SHIELD DEFINITION
//3
#define PWMA1 9
#define AIN11 13
#define AIN21 12

//2
#define PWMB1 6
#define BIN11 8
#define BIN21 7

//1
#define PWMA2 5
#define AIN12 A1
#define AIN22 A0

//0
#define PWMB2 3
#define BIN12 4
#define BIN22 2

//100リミットの場合
//#define SETPWM analogWrite (_PWMA, constrain(map(velo,0,100,0,255),0,255));
#define SETPWM analogWrite (_PWMA, velo);

RPmotor::RPmotor(int PWMA, int AIN1, int AIN2)
{
  pinMode(PWMA, OUTPUT);	//All the pins are setup to OUTPUT
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  _PWMA = PWMA;	//Using private variables for functions
  _AIN1 = AIN1;
  _AIN2 = AIN2;
}

RPmotor::RPmotor(int ch)
{
	switch (ch){
			case 3:
			   pinMode(PWMA1, OUTPUT);	//All the pins are setup to OUTPUT
				pinMode(AIN11, OUTPUT);
				pinMode(AIN21, OUTPUT);

  _PWMA = PWMA1;	//Using private variables for functions
  _AIN1 = AIN11;
  _AIN2 = AIN21;
			break;
		  case 2:
			    pinMode(PWMB1, OUTPUT);	//All the pins are setup to OUTPUT
				pinMode(BIN11, OUTPUT);
				pinMode(BIN21, OUTPUT);

  _PWMA = PWMB1;	//Using private variables for functions
  _AIN1 = BIN11;
  _AIN2 = BIN21;

			break;
case 1:
			   pinMode(PWMA2, OUTPUT);	//All the pins are setup to OUTPUT
				pinMode(AIN12, OUTPUT);
				pinMode(AIN22, OUTPUT);

  _PWMA = PWMA2;	//Using private variables for functions
  _AIN1 = AIN12;
  _AIN2 = AIN22;
			break;
		  case 0:
			    pinMode(PWMB2, OUTPUT);	//All the pins are setup to OUTPUT
				pinMode(BIN12, OUTPUT);
				pinMode(BIN22, OUTPUT);
  _PWMA = PWMB2;	//Using private variables for functions
  _AIN1 = BIN12;
  _AIN2 = BIN22;
	break;}
}

//Rotate2
void RPmotor::rotate(int velo, int time)
{	
	if(velo>0){
	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, LOW);
		SETPWM;
	}
	if(velo<=0){
	digitalWrite(_AIN1, LOW); 
		digitalWrite(_AIN2, HIGH);
		velo=-1*velo;
		SETPWM;
	}
	//brake
	delay(time);
	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, HIGH);
	analogWrite (_PWMA, 0);
}

//CW2
void RPmotor::cw(int velo, int time)
{	
	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, LOW);
	SETPWM;
	delay(time);
	//brake
	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, HIGH);
	analogWrite (_PWMA, 0);
}
// CCW2
void RPmotor::ccw(int velo, int time)
{	
	digitalWrite(_AIN1, LOW); 
	digitalWrite(_AIN2, HIGH);
	SETPWM;
	delay(time);
	//brake
	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, HIGH);
	analogWrite (_PWMA, 0);

}

//Rotate1
void RPmotor::rotate(int velo)
{	
	if(velo>0){
	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, LOW);
		SETPWM;
	}
	if(velo<=0){
	digitalWrite(_AIN1, LOW); 
		digitalWrite(_AIN2, HIGH);
		velo=-1*velo;
		SETPWM;
	}
}

//CW1
void RPmotor::cw(int velo)
{	
	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, LOW);
	SETPWM;
}
// CCW1
void RPmotor::ccw(int velo)
{	
	digitalWrite(_AIN1, LOW); 
	digitalWrite(_AIN2, HIGH);
	SETPWM;
}
//brake motion
void RPmotor::brake(int time)
{	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, HIGH);
	analogWrite (_PWMA, 0);
	delay(time);
}

//free motion
void RPmotor::free(int time)
{	
	digitalWrite(_AIN1, LOW); 
	digitalWrite(_AIN2, LOW);
	analogWrite (_PWMA, 255);
	delay(time);

}
//brake motion0
void RPmotor::brake(void)
{	digitalWrite(_AIN1, HIGH); 
	digitalWrite(_AIN2, HIGH);
	analogWrite (_PWMA, 0);
}

//free motion0
void RPmotor::free(void)
{	
	digitalWrite(_AIN1, LOW); 
	digitalWrite(_AIN2, LOW);
	analogWrite (_PWMA, 255);
}

//------------- ultrasonic sensor

int ussRead(int ch)
{
 float duration, distance;
  pinMode(ch, OUTPUT);
  digitalWrite(ch, LOW);
  delayMicroseconds(1);
  digitalWrite(ch, HIGH);
	delayMicroseconds(1);
digitalWrite(ch, LOW);
  pinMode(ch, INPUT);
  duration = pulseIn(ch, HIGH);
  distance = (duration/2) / 31.4;
 return distance;  
}

