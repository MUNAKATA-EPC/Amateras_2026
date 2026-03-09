/*
*    LedControl.cpp - A library for controling Leds with a MAX7219/MAX7221
*    Copyright (c) 2007 Eberhard Fahle
* 
*    Permission is hereby granted, free of charge, to any person
*    obtaining a copy of this software and associated documentation
*    files (the "Software"), to deal in the Software without
*    restriction, including without limitation the rights to use,
*    copy, modify, merge, publish, distribute, sublicense, and/or sell
*    copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following
*    conditions:
* 
*    This permission notice shall be included in all copies or 
*    substantial portions of the Software.
* 
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*    OTHER DEALINGS IN THE SOFTWARE.

digit arrangement

  1
6   2
  3
4   5
  0   7

*/

#include <Arduino.h>
#include "LedControl.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15
#define SEGC(v) (v&B01100010)|((v&(0x01<<7))>>5)|((v&(0x01<<4))<<3)|((v&(0x01<<3))<<1)|((v&(0x01<<2))>>2)|((v&(0x01))<<3)

int row_list[]={3,1,2,4,7,0,6,5};// Dp A B C D E F G => C A B D G Dp F E
int col_list[]={3,2,6,7,0,1,4,5};//digit

unsigned char  segc( unsigned char v)
{
	unsigned char cv;
	
	cv = v&B01100010;  //A B F 
	cv |= (v&(0x01<<7))>>5; // Dp 
	cv |= (v&(0x01<<4))<<3; //C
	cv |= (v&(0x01<<3))<<1; //D
	cv |= (v&(0x01<<2))>>2; //E
	cv |= (v&(0x01))<<3;  //G
	
	return cv; 
}

LedControl::LedControl(int dataPin, int clkPin, int csPin, int numDevices) {
	SPI_MOSI=dataPin;
	SPI_CLK=clkPin;
	SPI_CS=csPin;
	if(numDevices<=0 || numDevices>8 )
	numDevices=8;
	maxDevices=numDevices;
	pinMode(SPI_MOSI,OUTPUT);
	pinMode(SPI_CLK,OUTPUT);
	pinMode(SPI_CS,OUTPUT);
	digitalWrite(SPI_CS,HIGH);
	SPI_MOSI=dataPin;
	for(int i=0;i<64;i++) 
	status[i]=0x00;
	for(int i=0;i<maxDevices;i++) {
		spiTransfer(i,OP_DISPLAYTEST,0);
		//scanlimit is set to max on startup
		setScanLimit(i,7);
		//decode is done in source
		spiTransfer(i,OP_DECODEMODE,0);
		clearDisplay(i);
		//we go into shutdown-mode on startup
		shutdown(i,true);
	}
}

int LedControl::getDeviceCount() {
	return maxDevices;
}

void LedControl::shutdown(int addr, bool b) {
	if(addr<0 || addr>=maxDevices)
	return;
	if(b)
	spiTransfer(addr, OP_SHUTDOWN,0);
	else
	spiTransfer(addr, OP_SHUTDOWN,1);
}

void LedControl::setScanLimit(int addr, int limit) {
	if(addr<0 || addr>=maxDevices)
	return;
	if(limit>=0 || limit<8)
	spiTransfer(addr, OP_SCANLIMIT,limit);
}

void LedControl::setIntensity(int addr, int intensity) {
	if(addr<0 || addr>=maxDevices)
	return;
	if(intensity>=0 || intensity<16)	
	spiTransfer(addr, OP_INTENSITY,intensity);
	
}

void LedControl::clearDisplay(int addr) {
	int offset;
	
	if(addr<0 || addr>=maxDevices)
	return;
	offset=addr*8;
	for(int i=0;i<8;i++) {
		status[offset+i]=0;
		spiTransfer(addr, i+1,status[offset+i]);
	}
}

void LedControl::setLed(int addr,  int column, int row, boolean state) {
	int offset;
	byte val=0x00;
	
	if(addr<0 || addr>=maxDevices)
	return;
	if(row<0 || row>7 || column<0 || column>7)
	return;
	offset=addr*8;
	val=B10000000 >> row_list[row];
	if(state)
	status[offset+column]=status[offset+column]|val;
	else {
		val=~val;
		status[offset+column]=status[offset+column]&val;
	}
	spiTransfer(addr, col_list[column]+1,status[offset+column]);
}

void LedControl::setRow(int addr, int row, byte value) {
	int offset;
	if(addr<0 || addr>=maxDevices)
	return;
	if(row<0 || row>7)
	return;
	offset=addr*8;
	status[offset+row]=SEGC(value);
	spiTransfer(addr, col_list[row]+1,status[offset+row]);
}

void LedControl::setColumn(int addr, int col, byte value) {
	byte val;
	
	if(addr<0 || addr>=maxDevices)
	return;
	if(col<0 || col>7) 
	return;
	for(int row=0;row<8;row++) {
		val=value >> (7-row);
		val=val & 0x01;
		setLed(addr,row,col,val);
	}
}

void LedControl::setDigit(int addr, int digit, byte value, boolean dp) {
	int offset;
	byte v;
	
	if(addr<0 || addr>=maxDevices)
	return;
	if(digit<0 || digit>7 || value>15)
	return;
	offset=addr*8;
	v=charTable[value];
	if(dp)
	v|=B00000100;
	status[offset+digit]=v;
	spiTransfer(addr, col_list[digit]+1,v);
	
}

void LedControl::setChar(int addr, int digit, char value, boolean dp) {
	int offset;
	byte index,v;
	
	if(addr<0 || addr>=maxDevices)
	return;
	if(digit<0 || digit>7)
	return;
	offset=addr*8;
	index=(byte)value;
	if(index >127) {
		//nothing define we use the space char
		value=32;
	}
	v=charTable[index];
	if(dp)
	v|=SEGC(B10000000);
	status[offset+digit]=v;
	spiTransfer(addr, col_list[digit]+1,v);
}

void LedControl::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
	//Create an array with the data to shift out
	int offset=addr*2;
	int maxbytes=maxDevices*2;
	
	for(int i=0;i<maxbytes;i++)
	spidata[i]=(byte)0;
	//put our device data into the array
	spidata[offset+1]=opcode;
	spidata[offset]=data;
	//enable the line 
	digitalWrite(SPI_CS,LOW);
	//Now shift out the data 
	for(int i=maxbytes;i>0;i--)
	shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
	//latch the data onto the display
	digitalWrite(SPI_CS,HIGH);
}    

//-------------- Original
//十進数を表示する -999 to 9999 まで
void LedControl::setDec(int addr, int dec)
{
	if(dec<=9999&&dec>=0)
	{
    setDigit(addr,0,dec/1000,false);
    setDigit(addr,1,(dec%1000)/100,false);
    setDigit(addr,2,(dec%100)/10,false);
		setDigit(addr,3,(dec%10),false);	
		}
	else if(dec>=-999&&dec<0)
	{
	clearDisplay(addr);
	setLed(addr, 0, 3 ,HIGH);
	setDigit(addr,1,(-dec%1000)/100,false);
    setDigit(addr,2,(-dec%100)/10,false);
	setDigit(addr,3,(-dec%10),false);
	}	
	else
	{
	setDigit(addr,0,8,true);
	setDigit(addr,1,8,true);
	setDigit(addr,2,8,true);
	setDigit(addr,3,8,true);		
	}
	
}




