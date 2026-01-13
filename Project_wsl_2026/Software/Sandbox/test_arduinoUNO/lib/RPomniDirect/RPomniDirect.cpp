// OmniDirection drive library v.1
//  ROBOPRO Shield 
//  written by yu okumura@furo  2013/7/11
// 2015/10/6 refine
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
#include "Arduino.h"
#include <RPlib.h>
#include "RPOmniDirect.h"

RPmotor _AMOT(1);  
RPmotor _BMOT(2);  
RPmotor _CMOT(0); 
RPmotor _DMOT(3); 

const int _mot[4]={2,1,0,3};
/*
RPomniDirect::RPomniDirect(RPmotor AMOT, RPmotor BMOT,RPmotor CMOT)
{
  _AMOT = AMOT;	//Using private variables for functions
  _BMOT = BMOT;
  _CMOT = CMOT;
}
*/

RPomniDirect::RPomniDirect(float a_gain, float b_gain , float c_gain, float f_gain)
{
	_a_gain = a_gain;
	_b_gain = b_gain;
	_c_gain = c_gain;
	_f_gain = f_gain;
	_limit = 1;
	_mota=2;
	_motb=1;
	_motc=0;
}



RPomniDirect::RPomniDirect(float a_gain, float b_gain , float c_gain, float f_gain, int limit)
{
	_a_gain = a_gain;
	_b_gain = b_gain;
	_c_gain = c_gain;
	_f_gain = f_gain;
	_limit = limit;
	_mota=2;
	_motb=1;
	_motc=0;
}

// add 2015/10/6
RPomniDirect::RPomniDirect(float a_gain, float b_gain , float c_gain, float f_gain,int motc,int motb,int mota)
{
	_a_gain = a_gain;
	_b_gain = b_gain;
	_c_gain = c_gain;
	_f_gain = f_gain;
	_mota=mota;
	_motb=motb;
	_motc=motc;
	_limit = 1;
}
/***************************************************/
/*	DCモータ用関数				  */
/***************************************************/
void RPomniDirect::motor(int speed, int ch)
{	

  switch (ch)
  {
  case 0:
    _AMOT.rotate(map(constrain(speed,-100,100),-100,100,-255,255));
    break;
  case 1:
    _BMOT.rotate(map(constrain(speed,-100,100),-100,100,-255,255));
    break;
  case 2:
    _CMOT.rotate(map(constrain(speed,-100,100),-100,100,-255,255));
		break;
   case 3:
    _DMOT.rotate(map(constrain(speed,-100,100),-100,100,-255,255));
    break;
  }
}

float fconstrain(float x, float a, float b)
{
	if(x<=a)return(a);	
	if(x>=b)return(b);	
	return(x);
}


//-------------------------------
void RPomniDirect::move(float vx, float vy, float  w)
{	
  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;

  // adjustment w
  /*
if(sqrt(vx*vx+vy*vy)<=10&&vx>0) w += 50*pow(cos(3*atan2(vy,vx)),2)*sqrt(vx*vx+vy*vy)/10;
   if(sqrt(vx*vx+vy*vy)<=10&&vx<0) w -= 50*pow(cos(3*atan2(vy,vx)),2)*sqrt(vx*vx+vy*vy)/10;
   */
	
	
	
	if(_limit)
	{
	/*リミッター*/
	vx=fconstrain(vx,-100.0f,100.0f);
	vy=fconstrain(vy,-100.0f,100.0f);
	w=fconstrain(w,-100.0f,100.0f);
	}
	
    w=-6*w;

	
	/*`摩擦補正 */
  if(sqrt(vx*vx+vy*vy)<=10){
    vx=0;
    vy=0;
  }
  if(sqrt(vx*vx+vy*vy)>10&&vx>0) w -= _f_gain*pow(cos(3*atan2(vy,vx)),2);
  if(sqrt(vx*vx+vy*vy)>10&&vx<0) w += _f_gain*pow(cos(3*atan2(vy,vx)),2);


	
  a -=   (float)vx;
  a += 0.1f * (float)w;

  b = 0.5f * (float)vx;
  b -= 0.866f * (float)vy;
  b += 0.1f * (float)w;

  c = 0.5f * (float)vx;
  c += 0.866f * (float)vy;
  c += 0.1f * (float)w;

  a=_a_gain*a;
  b=_b_gain*b;
  c=_c_gain*c;


  if(a > 0.0f) a += MOT_OFFSET;
  else if(a < 0.0f) a -= MOT_OFFSET;
  if(b > 0.0f) b += MOT_OFFSET;
  else if(b < 0.0f) b -= MOT_OFFSET;
  if(c > 0.0f) c += MOT_OFFSET;
  else if(c < 0.0f) c -= MOT_OFFSET;
/*
  motor((int)-a ,2); //amot
  motor((int)-b ,1); //bmot
  motor((int)-c ,0); //cmot
	*/
  motor((int)-a ,_mot[_motc]); //amot
  motor((int)-b ,_mot[_motb]); //bmot
  motor((int)-c ,_mot[_mota]); //cmot
	
	
	
}


