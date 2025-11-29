#include <Arduino.h>
#include <VarSpeedServo.h>
#include <math.h>

#define ARM_A                   148    // 上腕長さ
#define ARM_B                   160    // 下腕長さ
#define ARM_2AB                 47360  // 2*A*B
#define ARM_A2                  21904  // A^2
#define ARM_B2                  25600  // B^2
#define ARM_A2B2                47504  // A^2 + B^2

#define ARM_STRETCH_MIN         80   //0
#define ARM_STRETCH_MAX         250   //210
#define ARM_HEIGHT_MIN           -30  // max -180
#define ARM_HEIGHT_MAX          120  // max 150
#define ARM_ROTATION_MIN        -90
#define ARM_ROTATION_MAX        90

#define GRIP_OPEN         110  //125
#define GRIP_CLOSE        55    //40

#define FIXED_OFFSET_R          45  //初期姿勢と鉛直のズレ
#define FIXED_OFFSET_L          30  //初期姿勢と鉛直のズレ

// PEN Mode
#define UP 0
#define DOWN -40
#define PI 3.14159

// Servo CHANNEL
#define SERVO_GRIP              5     //S3
#define SERVO_ROT               8    //S2
#define SERVO_R                 6    //S0
#define SERVO_L                 7    //S1

#define SERVO_GRIP_ROT          2   //S4

/*
// custum ----------------------written by yu@furo
 #define SERVO_MG995_MAX 2475
 #define SERVO_MG995_MIN 525
 #define SERVO_SG90_MAX 2320
 #define SERVO_SG90_MIN 680
 //-------------------------------
 */

class ArmRobot
{
public:
  ArmRobot();
  void setup(double initR, double initL, double initROT, double initGRIP);
  void setPosition(double _stretch, double _height, double _rotation);    // 
  void setServoSpeed(char _servoNum, unsigned char _servoSpeed); // 0=full speed, 1-255 slower to faster

  void gripperCatch();    //
  void gripperRelease();  //
  void penUp();
  void penDown();
  void setXYZ(double x, double y, double z);//
  void traceLine(double xy1[], double xy2[]);//

  //global
  double adjZero_L;
  double adjZero_R;
  double adjZero_ROT;
  double adjZero_GRIP;
  
  double current_st;
  double current_ht;
  double current_rot;
private:
  unsigned char servoSpdR;
  unsigned char servoSpdL;
  unsigned char servoSpdROT;
  unsigned char servoSpdGRIP;
  /***************  Create servo objects  ***************/
  VarSpeedServo servoR;
  VarSpeedServo servoL;
  VarSpeedServo servoROT;
  VarSpeedServo servoGRIP;
};






