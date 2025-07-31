#include "Arduino.h"
#include "DDK0668T.h"


void DDK0668T_move_v1(int a, int b, int c, int d , int e , int f )
{
	int val = 0;
	int input[6] = {a, b, c, d, e, f};
	String StringA = "0";
	String StringB = "0";
	String StringC = "0";
	String StringD = "0";
	String StringE = "0";
	String StringF = "0";
	
	String StringX = "0";
	String StringY = "0";
	String StringZ = "0";
	String String0 = "0";


	StringX = String(1);
	val = input[0];
	if (val < 0 && val >= -100)
	{
		StringY = String('R');
		val = abs(val);
		StringZ = String(val);
	}
	else if (val >= 0 && val <= 100)
	{
		StringY = String('F');
		StringZ = String(val);
	}
	else
	{
		StringY = String('F');
		StringZ = String(0);
	}
	if (val < 10)
	{
		String0 = String("00");
		StringZ = String0 + StringZ;
	}
	else if (val < 100)
	{
		String0 = String('0');
		StringZ = String0 + StringZ;
	}
	else
	{
		String0 = String("");
	}
	StringA = StringX + StringY + StringZ;


	StringX = String(2);
	val = input[1];
	if (val < 0 && val >= -100)
	{
		StringY = String('R');
		val = abs(val);
		StringZ = String(val);
	}
	else if (val >= 0 && val <= 100)
	{
		StringY = String('F');
		StringZ = String(val);
	}
	else
	{
		StringY = String('F');
		StringZ = String(0);
	}
	if (val < 10)
	{
		String0 = String("00");
		StringZ = String0 + StringZ;
	}
	else if (val < 100)
	{
		String0 = String('0');
		StringZ = String0 + StringZ;
	}
	else
	{
	}
	StringB = StringX + StringY + StringZ;


	StringX = String(3);
	val = input[2];
	if (val < 0 && val >= -100)
	{
		StringY = String('R');
		val = abs(val);
		StringZ = String(val);
	}
	else if (val >= 0 && val <= 100)
	{
		StringY = String('F');
		StringZ = String(val);
	}
	else
	{
		StringY = String('F');
		StringZ = String(0);
	}
	if (val < 10)
	{
		String0 = String("00");
		StringZ = String0 + StringZ;
	}
	else if (val < 100)
	{
		String0 = String('0');
		StringZ = String0 + StringZ;
	}
	else
	{
	}
	StringC = StringX + StringY + StringZ;


	StringX = String(4);
	val = input[3];
	if (val < 0 && val >= -100)
	{
		StringY = String('R');
		val = abs(val);
		StringZ = String(val);
	}
	else if (val >= 0 && val <= 100)
	{
		StringY = String('F');
		StringZ = String(val);
	}
	else
	{
		StringY = String('F');
		StringZ = String(0);
	}
	if (val < 10)
	{
		String0 = String("00");
		StringZ = String0 + StringZ;
	}
	else if (val < 100)
	{
		String0 = String('0');
		StringZ = String0 + StringZ;
	}
	else
	{
	}
	StringD = StringX + StringY + StringZ;

	
		StringX = String(5);
	val = input[4];
	if (val < 0 && val >= -100)
	{
		StringY = String('R');
		val = abs(val);
		StringZ = String(val);
	}
	else if (val >= 0 && val <= 100)
	{
		StringY = String('F');
		StringZ = String(val);
	}
	else
	{
		StringY = String('F');
		StringZ = String(0);
	}
	if (val < 10)
	{
		String0 = String("00");
		StringZ = String0 + StringZ;
	}
	else if (val < 100)
	{
		String0 = String('0');
		StringZ = String0 + StringZ;
	}
	else
	{
		String0 = String("");
	}
	StringE = StringX + StringY + StringZ;
	
	
		StringX = String(6);
	val = input[5];
	if (val < 0 && val >= -100)
	{
		StringY = String('R');
		val = abs(val);
		StringZ = String(val);
	}
	else if (val >= 0 && val <= 100)
	{
		StringY = String('F');
		StringZ = String(val);
	}
	else
	{
		StringY = String('F');
		StringZ = String(0);
	}
	if (val < 10)
	{
		String0 = String("00");
		StringZ = String0 + StringZ;
	}
	else if (val < 100)
	{
		String0 = String('0');
		StringZ = String0 + StringZ;
	}
	else
	{
		String0 = String("");
	}
	StringF = StringX + StringY + StringZ;
	
	
	Serial.println(StringA + StringB + StringC + StringD + StringE + StringF);

    /*
	if(PinNo == 2)
	{
		Serial2.println(StringA + StringB + StringC + StringD + StringE + StringF);
	}
	else if(PinNo == 3)
	{
		Serial3.println(StringA + StringB + StringC + StringD + StringE + StringF);
	}
	else
	{
		Serial1.println(StringA + StringB + StringC + StringD + StringE + StringF);
	}
    */
}