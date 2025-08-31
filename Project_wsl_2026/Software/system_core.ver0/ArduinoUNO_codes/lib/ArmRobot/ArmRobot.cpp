#include <Arduino.h>
#include <math.h>
#include "ArmRobot.h"

ArmRobot::ArmRobot()
{
	
}


void ArmRobot::setup(double initR, double initL, double initROT, double initGRIP)
{
	
	adjZero_R = initR ;
	adjZero_L = initL ;
	adjZero_ROT= initROT;
	adjZero_GRIP= initGRIP;
	
	// 初期値ですべてのサーボが90degになる
	servoL.attach(SERVO_L, SERVO_MG995_MIN, SERVO_MG995_MAX);
	servoR.attach(SERVO_R, SERVO_MG995_MIN, SERVO_MG995_MAX);
	servoROT.attach(SERVO_ROT, SERVO_MG995_MIN, SERVO_MG995_MAX);
	servoGRIP.attach(SERVO_GRIP, SERVO_SG90_MIN, SERVO_SG90_MAX);
	
	//ゆっくりとオフセットまで移動するシーケンス
	
	servoR.write(adjZero_R +90 ,10);
	servoL.write(adjZero_L +90 ,10);
	servoROT.write(adjZero_ROT +90 ,10);
	servoGRIP.write(adjZero_GRIP+90,10);
	
	
}


void ArmRobot::setPosition(double _stretch, double _height, double _rotation)
{
	
	
	current_st = _stretch;
	current_ht=  _height;
	current_rot = _rotation;
	
	//制限する
	_stretch=constrain(_stretch,ARM_STRETCH_MIN ,ARM_STRETCH_MAX); 
	_height=constrain(_height,ARM_HEIGHT_MIN,ARM_HEIGHT_MAX);
	_rotation=constrain(_rotation,ARM_ROTATION_MIN,ARM_ROTATION_MAX);
	
	
	double stretch2height2 = _stretch * _stretch + _height * _height;              // 
	double angleA = (acos( (ARM_A2B2 - stretch2height2) / ARM_2AB )) * RAD_TO_DEG; // angle between the upper and the lower
	double angleB = (atan(_height/_stretch)) * RAD_TO_DEG;                         // 
	double angleC = (acos((ARM_A2 + stretch2height2 -ARM_B2)/(2 * ARM_A * sqrt(stretch2height2)))) * RAD_TO_DEG; // 
	double angleR = 180 - angleA - angleB - angleC + FIXED_OFFSET_R; //+ offsetR;        // 
	double angleL = angleB + angleC + FIXED_OFFSET_L ;// + offsetL;                       // 
	
	servoR.write(angleR + adjZero_R, servoSpdR, false);
	servoL.write(angleL +adjZero_L , servoSpdL, false);
	servoROT.write( 90 + adjZero_ROT +_rotation  , servoSpdROT  , false);
	
}
void ArmRobot::penUp()
{
	for(double i=current_ht;i<UP;i+=2){
		setPosition(current_st, i, current_rot);
		delay(10);
	}
	current_ht=UP;
	//delay(1500);
}

void ArmRobot::penDown()
{
	for(double i=current_ht;i>DOWN;i-=2){
		setPosition(current_st, i, current_rot);
		delay(10);
	}
	current_ht=DOWN;
	//delay(1500);
}

void ArmRobot::setServoSpeed(char _servoNum, unsigned char _servoSpeed) // 0=full speed, 1-255 slower to faster
{
	switch(_servoNum)
	{
	  case SERVO_L:
		servoSpdL = _servoSpeed;
		break;
	  case SERVO_R:
		servoSpdR = _servoSpeed;
		break;
	  case SERVO_ROT:
		servoSpdROT = _servoSpeed;
		break;
	  case SERVO_GRIP:
		servoSpdGRIP = _servoSpeed;
		break;
	  default: 
		break;
	}
}

void ArmRobot::gripperCatch()
{
	//  servoHand.attach(SERVO_HAND, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
	servoGRIP.write(GRIP_CLOSE + adjZero_GRIP  , 0, false);
}

void ArmRobot::gripperRelease()
{
	
	//servoHand.attach(SERVO_HAND, D009A_SERVO_MIN_PUL, D009A_SERVO_MAX_PUL);
	servoGRIP.write(GRIP_OPEN +   adjZero_GRIP , 0, false);
}

//-------------------------------

#define RADTODEG 180/PI
#define X_OFFSET 150 //アームヨー軸からのsetPositino の原点位置
#define PEN_OFFSET 35 //35mm

void ArmRobot::setXYZ(double x, double y, double z)
{
	//  double c= -0.1*x-5;//高さ方向補正	
	x+=X_OFFSET;
	double r = sqrt(x*x+y*y);
	double theta = atan2(y,x);  //radian
	setPosition(r-PEN_OFFSET, z,theta*RADTODEG);//XYZ関数内ではsetPositionの原点から75ミリの位置がゼロ点、範囲は(from -75 to 75)
}

//-------------------------------
//指定座標から座標まで直線で補完して手先をトレースする
void ArmRobot::traceLine(double xy1[], double xy2[])
{
	double d = sqrt((xy2[0]-xy1[0])*(xy2[0]-xy1[0]) + (xy2[1]-xy1[1])*(xy2[1]-xy1[1]));
	double dx,dy;
	
	if(d==0){
		dx=0;dy=0;
	}
	else{
	dx= (xy2[0]-xy1[0])/(d);
	dy= (xy2[1]-xy1[1])/(d);
	}
	
	for(double i=0;i<=d;i+=1)
	{
		setXYZ(xy1[0]+dx*i,xy1[1]+dy*i,current_ht);  
		delay(50);
	}
	
}





