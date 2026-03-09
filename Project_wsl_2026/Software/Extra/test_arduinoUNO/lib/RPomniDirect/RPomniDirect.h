// OmniDirect drive library v.1
//  ROBOPRO Shield 
//  written by yu okumura@furo  2013/7/11
// 2015/10/6 refine
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
#include <float.h>
#include <math.h>

#ifndef RPomniDirect_h
#define RPOmniDirect_h

#include "Arduino.h"

#define MOT_OFFSET  10 //13
#define LIMIT_OFF 0
#define LIMIT_ON 1

class RPomniDirect
{
  public:
	RPomniDirect(float a_gain, float b_gain , float c_gain, float f_gain);	
	RPomniDirect(float a_gain, float b_gain , float c_gain, float f_gain, int limit);
	RPomniDirect(float a_gain, float b_gain , float c_gain, float f_gain,int motc,int motb,int mota);
	void move(float vx, float vy, float w);
	void motor(int speed, int ch);
  private:
	float _a_gain;
	float _b_gain;
	float _c_gain;
	float _f_gain;
	int _limit;
	int _mota;
	int _motb;
	int _motc;
		};

#endif

