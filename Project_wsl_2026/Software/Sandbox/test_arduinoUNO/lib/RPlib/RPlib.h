// motor drive library v.1.1
//  ROBOPRO Shield 
//  written by yu okumura@furo  2013/7/11
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
#ifndef RPlib_h
#define RPlib_h

#define MC0 0
#define MC1 1
#define MC2 2
#define MC3 3

#define D0 10
#define D1 9
#define D2 2
#define D3 3
#define S0 6
#define S1 7
#define S2 8
#define S3 5
#define S4 2
#define S5 3

#define ON HIGH
#define OFF LOW

//---------暫定USSセンサパラメータ----------------------
#define US0 0
#define US1 A3
#define US2 A2
//-------------------------------

#include "Arduino.h"

int ussRead(int ch);

class RPmotor
{
  public:
	RPmotor(int PWMA, int AIN1, int AIN2);
	RPmotor(int ch);
    void rotate(int velo, int time);
	void cw(int velo, int time);
	void ccw(int velo, int time);
	
	void rotate(int velo);
	void cw(int velo);
	void ccw(int velo);
	
	void brake(int time);    	
	void free(int time);
	void brake(void);    	
	void free(void);
	
  private:
    int _PWMA;
    int _AIN1;
	int _AIN2;
};

#endif


